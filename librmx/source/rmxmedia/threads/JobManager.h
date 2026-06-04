/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	JobManager
*		Manages jobs to execute minor tasks in a separate thread.
*/

#pragma once


namespace rmx
{
	class JobWorkerThread;
	class JobBase;


	// Job manager
	class JobManager
	{
	public:
		JobManager();
		~JobManager();

		void setMaxThreads(int count);

		void insertJob(JobBase& job);
		void insertJob(JobBase& job, float priority);
		void removeJob(JobBase& job);

		int getJobCount()  { return (int)mJobs.size(); }
		int getFinishedCount();

		JobBase* getNextJob();
		JobBase* getNextJobBlocking();

		void getJobList(std::vector<JobBase*>& output);

		void onJobChanged();

	private:
		JobBase* getNextJobInternal();
		void stopAllThreads();

	private:
		SDL_cond* mConditionVariable;
		SDL_mutex* mConditionLock;

		// Worker threads
		int mMaxThreads;
		std::vector<JobWorkerThread*> mThreads;

		// Registered jobs
		//  -> Not using a data structure optimized for getting the next job (using priority);
		//     but that's probably overkill anyways if the number of active jobs is not more than a few dozens
		std::vector<JobBase*> mJobs;
		uint32 mNextDelayedJobTicks;
		bool mSearchforJobs;
	};



	// Base class for jobs
	class JobBase
	{
	friend class JobManager;
	friend class JobWorkerThread;

	public:
		enum JobState
		{
			JobState_INACTIVE,	// Initial state before being added to the job manager
			JobState_WAITING,	// Waiting for execution
			JobState_RUNNING,	// Currently being executed
			JobState_DONE		// Job finished, nothing left to do
		};

	public:
		JobBase();
		virtual ~JobBase()  {}

		inline const String& getJobType() const		{ return mJobType; }

		inline float getJobPriority() const			{ return mJobPriority; }
		void setJobPriority(float priority);

		inline uint32 getJobDelayUntilTicks() const	{ return mJobDelayUntilTicks; }
		void setJobDelayUntilTicks(uint32 sdlTicks);

		inline bool isJobRegistered() const	{ return (nullptr != mRegisteredAtManager); }
		inline bool isJobWaiting() const	{ return (mJobState == JobState_WAITING); }
		inline bool isJobRunning() const	{ return (mJobState == JobState_RUNNING); }
		inline bool isJobDone() const		{ return (mJobState == JobState_DONE); }

		bool callJobFuncOnCallingThread();
		void executeOnCallingThread();

	protected:
		// The method to implement in a sub-class; returns true when the job is finished
		virtual bool jobFunc()  { return true; }

	protected:
		inline bool shouldJobBeRunning() const	{ return mJobShouldBeRunning; }

	protected:
		String mJobType;							// Type string for the job

	private:
		JobManager* mRegisteredAtManager;	// Job manager instance this is registered at (should actually always be FTX::JobManager or nullptr)
		JobState mJobState;	// Current state
		bool mJobShouldBeRunning;			// Can be set to false while running to signal the jobFunc that it should abort
		float mJobPriority;					// Priority, higher values will be preferred; jobs with negative priorities won't get processed at all
		uint32 mJobDelayUntilTicks;				// SDL ticks value until when the job should get delayed; 0 if no delay active (which is the default)
	};



	// Worker thread processing jobs
	class JobWorkerThread final : public ThreadBase
	{
	public:
		JobWorkerThread(JobManager& jobManager, int index);
		virtual void threadFunc();

	public:
		float mInactivityDelay;

	private:
		JobManager& mJobManager;
	};
}
