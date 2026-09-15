/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxbase.h>
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
#include <atomic>
#include <thread>
#endif

#ifdef PLATFORM_WEB
struct emscripten_fetch_t;
#endif


class Downloader
{
public:
	enum class State
	{
		NONE,
		RUNNING,
		DONE,
		FAILED
	};

public:
	static bool isDownloaderSupported();

public:
	~Downloader();

	inline State getState() const  { return mState; }
	inline bool isRunning() const  { return mState == State::RUNNING; }
	inline uint64 getBytesDownloaded() const  { return mBytesDownloaded; }

	void setupDownload(std::string_view url, std::wstring_view outputFilename);
	void startDownload();
	void stopDownload();

private:
	static size_t writeDataStatic(void* data, size_t size, size_t nmemb, Downloader* downloader);
	static void performDownloadStatic(Downloader* downloader);

	size_t writeData(void* data, size_t size, size_t nmemb);
	void performDownload();

private:
	std::string mURL;
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	std::thread* mThread = nullptr;
#else
	void* mThread = nullptr;
#endif
	State mState = State::NONE;
	std::wstring mOutputFilename;
	FileHandle mOutputFile;
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	std::atomic<uint64> mBytesDownloaded = 0;
	std::atomic<bool> mThreadRunning = false;
#else
	uint64 mBytesDownloaded = 0;
	bool mThreadRunning = false;
#endif

#ifdef PLATFORM_WEB
	emscripten_fetch_t* mFetch = nullptr;
#endif
};
