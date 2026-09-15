/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "rmxbase/file/FileProvider.h"


namespace rmx
{

	class API_EXPORT RealFileProvider : public FileProvider
	{
	public:
		bool exists(const std::wstring& path) ;
		bool getFileSize(const std::wstring& filename, uint64& outFileSize) ;
		bool getFileTime(const std::wstring& filename, time_t& outFileTime) ;
		bool readFile(const std::wstring& filename, std::vector<uint8>& outData) ;

		bool renameFile(const std::wstring& oldFilename, const std::wstring& newFilename) ;
		bool listFiles(const std::wstring& path, bool recursive, std::vector<FileIO::FileEntry>& outFileEntries) ;
		bool listFilesByMask(const std::wstring& filemask, bool recursive, std::vector<FileIO::FileEntry>& outFileEntries) ;
		bool listDirectories(const std::wstring& path, std::vector<std::wstring>& outDirectories) ;
		InputStream* createInputStream(const std::wstring& filename) ;

	private:
		std::wstring mRealLocation;
	};

}
