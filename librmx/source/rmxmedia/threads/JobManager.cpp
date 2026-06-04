/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxmedia.h"


namespace rmx
{

	JobManager::JobManager() : mConditionVariable(nullptr), mConditionLock(nullptr), mMaxThreads(1), mNextDelayedJobTicks(0), mSearchforJobs(true)
	{
		mConditionVariable = SDL_CreateCond();
		mConditionLock = SDL_CreateMutex();
	}

	JobManager::~JobManager()
	{
		stopAllThreads();
		SDL_DestroyCond(mConditionVariable);
		SDL_DestroyMutex(mConditionLock);
	}

	void JobManager::setMaxThreads(int count)
	{
		mMaxThreads = clamp(count, 0, 16);
	}

	void JobManager::insertJob(JobBase& job)
	{
		if (nullptr != job.mRegisteredAtManager)
		{
			if (job.mRegisteredAtManager != this)
				return;
			if (job.mJobState != JobBase::JobState_INACTIVE && job.mJobState != JobBase::JobState_DONE)
				return;

			// Job is already registered here, but needs to have its state reset back to waiting
		}
		else
		{
			// Register job here
			job.mRegisteredAtManager = this;

			// Make sure we have enough worker threads
			//  -> TODO: Only create a new one if all existing threads are actually busy
			const int index = (int)mThreads.size();
			if (index < mMaxThreads)
			{
				JobWorkerThread* thread = new JobWorkerThread(*this, index);
				mThreads.push_back(thread);
				thread->startThread();
			}
		}

		// Job is ready to be processed
		job.mJobState = JobBase::JobState_WAITING;

		// Wake up a thread
		if (!mThreads.empty())
		{
			SDL_LockMutex(mConditionLock);
			mJobs.push_back(&job);
			SDL_CondSignal(mConditionVariable);
			SDL_UnlockMutex(mConditionLock);
		}
		else
		{
			// In case there are no worker threads, execute on the calling thread
			job.executeOnCallingThread();
		}
	}

	void JobManager::insertJob(JobBase& job, float priority)
	{
		job.mJobPriority = priority;
		insertJob(job);
	}

	void JobManager::removeJob(JobBase& job)
	{
		if (job.mRegisteredAtManager != this)
			return;

		bool wasRemoved = false;
		SDL_LockMutex(mConditionLock);
		for (size_t i = 0; i < mJobs.size(); ++i)
		{
			if (mJobs[i] == &job)
			{
				// Swap with last
				if (i + 1 < mJobs.size())
				{
					mJobs[i] = mJobs.back();
				}
				mJobs.pop_back();
				wasRemoved = true;
			}
		}
		job.mRegisteredAtManager = nullptr;
		SDL_UnlockMutex(mConditionLock);

		if (wasRemoved)
		{
			// Wait until job execution is done
			job.mJobPriority = -1.0f;
			job.mJobShouldBeRunning = false;
			while (job.mJobState == JobBase::JobState_RUNNING)
			{
				SDL_Delay(1);
			}
		}
	}

	int JobManager::getFinishedCount()
	{
		int count = 0;
		SDL_LockMutex(mConditionLock);
		for (size_t i = 0; i < mJobs.size(); ++i)
		{
			if (mJobs[i]->isJobDone())
				++count;
		}
		SDL_UnlockMutex(mConditionLock);
		return count;
	}

	JobBase* JobManager::getNextJob()
	{
		SDL_LockMutex(mConditionLock);
		JobBase* job = getNextJobInternal();
		SDL_UnlockMutex(mConditionLock);
		return job;
	}

	JobBase* JobManager::getNextJobBlocking()
	{
		// Wait until there's at leat one job available
		SDL_LockMutex(mConditionLock);
		JobBase* job = getNextJobInternal();
		while (nullptr == job && mSearchforJobs)
		{
			// Using a time-out for two reasons:
			//  - to have a chance to check if "mShouldBeRunning" changed outside
			//  - to react to a delayed job, if there's no other jobs at the moment
			uint32 timeoutMilliseconds = 100;
			if (mNextDelayedJobTicks != 0xffffffff)
			{
				const uint32 currentTicks = SDL_GetTicks();
				if (mNextDelayedJobTicks > currentTicks)
					timeoutMilliseconds = mNextDelayedJobTicks - currentTicks;
			}
			SDL_CondWaitTimeout(mConditionVariable, mConditionLock, timeoutMilliseconds);
			job = getNextJobInternal();
		}
		SDL_UnlockMutex(mConditionLock);
		return job;
	}

	void JobManager::getJobList(std::vector<JobBase*>& output)
	{
		SDL_LockMutex(mConditionLock);
		output = mJobs;
		SDL_UnlockMutex(mConditionLock);
	}

	void JobManager::onJobChanged()
	{
		SDL_LockMutex(mConditionLock);
		SDL_CondSignal(mConditionVariable);
		SDL_UnlockMutex(mConditionLock);
	}

	JobBase* JobManager::getNextJobInternal()
	{
		// Select waiting job with highest priority
		mNextDelayedJobTicks = 0xffffffff;	// This will get updated as well
		JobBase* bestJob = nullptr;
		const uint32 currentTicks = SDL_GetTicks();
		for (size_t i = 0; i < mJobs.size(); ++i)
		{
			JobBase* job = mJobs[i];
			if (job->mJobState == JobBase::JobState_WAITING)
			{
				if (job->mJobDelayUntilTicks <= currentTicks)
				{
					if (nullptr == bestJob || job->mJobPriority > bestJob->mJobPriority)
					{
						bestJob = job;
					}
				}
				else
				{
					if (job->mJobPriority >= 0.0f && job->mJobDelayUntilTicks < mNextDelayedJobTicks)
					{
						mNextDelayedJobTicks = job->mJobDelayUntilTicks;
					}
				}
			}
		}
		if (nullptr != bestJob)
		{
			// Ignore priorities below 0.0f
			if (bestJob->mJobPriority >= 0.0f)
			{
				bestJob->mJobShouldBeRunning = true;
				bestJob->mJobState = JobBase::JobState_RUNNING;
			}
			else
			{
				bestJob = nullptr;
			}
		}
		return bestJob;
	}

	void JobManager::stopAllThreads()
	{
		mSearchforJobs = false;
		for (size_t i = 0; i < mThreads.size(); ++i)
		{
			mThreads[i]->signalStopThread(false);
		}
		for (size_t i = 0; i < mThreads.size(); ++i)
		{
			mThreads[i]->joinThread();
		}
		for (size_t i = 0; i < mThreads.size(); ++i)
		{
			delete mThreads[i];
		}
		mThreads.clear();
	}



	JobBase::JobBase() : mRegisteredAtManager(nullptr), mJobState(JobState_INACTIVE), mJobShouldBeRunning(false), mJobPriority(0.0f), mJobDelayUntilTicks(0)
	{
	}

	void JobBase::setJobPriority(float priority)
	{
		// Jobs with negative priority are deactivated, i.e. won't get processed
		//  -> But when this changes, a thread possibly needs to be woken up
		const bool wakeUpThread = (mJobPriority < 0.0f && priority >= 0.0f);
		mJobPriority = priority;

		if (wakeUpThread && nullptr != mRegisteredAtManager)
		{
			mRegisteredAtManager->onJobChanged();
		}
	}

    void JobBase::setJobDelayUntilTicks(uint32 sdlTicks)
    {
		// If the job delay gets reduced (possibly to zero, i.e. deactivating the delay), a thread possibly needs to be woken up
		const bool wakeUpThread = (sdlTicks < mJobDelayUntilTicks);
		mJobDelayUntilTicks = sdlTicks;

		if (wakeUpThread && nullptr != mRegisteredAtManager)
		{
			mRegisteredAtManager->onJobChanged();
		}
	}

	bool JobBase::callJobFuncOnCallingThread()
	{
		mJobShouldBeRunning = true;
		mJobState = JobBase::JobState_RUNNING;

		// Call job function implementation once
		const bool result = jobFunc();
		if (result)
		{
			// Job is done
			mJobState = JobBase::JobState_DONE;
		}
		else
		{
			// Set back to waiting state
			mJobState = JobBase::JobState_WAITING;
		}
		return result;
	}

	void JobBase::executeOnCallingThread()
	{
		if (mJobState <= JobBase::JobState_WAITING)
		{
			mJobShouldBeRunning = true;
			mJobState = JobBase::JobState_RUNNING;

			// Execute until done
			while (!jobFunc())
			{
			}

			mJobShouldBeRunning = false;
			mJobState = JobBase::JobState_DONE;
		}
	}



	JobWorkerThread::JobWorkerThread(JobManager& jobManager, int index) :
		mInactivityDelay(0.1f),
		mJobManager(jobManager)
	{
		// Index goes unused at the moment
	}

	void JobWorkerThread::threadFunc()
	{
		while (mShouldBeRunning)
		{
			JobBase* job = mJobManager.getNextJobBlocking();
			if (nullptr != job)
			{
				// Execute job
				const bool result = job->jobFunc();
				if (result)
				{
					// Job is done
					job->mJobState = JobBase::JobState_DONE;
					job->mRegisteredAtManager->removeJob(*job);
				}
				else
				{
					// Set back to waiting state
					//  -> Note that the job's priority might have changed, or there's another job with higher priority now, so don't just continue with this job
					job->mJobState = JobBase::JobState_WAITING;
				}
			}
		}
	}

}
