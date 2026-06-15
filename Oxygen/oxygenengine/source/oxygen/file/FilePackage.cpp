/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/file/FilePackage.h"
#include "oxygen/helper/Utils.h"


#if defined(PLATFORM_PS3)
const char FilePackage::PackageHeader::SIGNATURE[5] = "OPCK";
#endif

#include "Endian/S3AIREndian.hpp"



bool FilePackage::loadPackage(std::wstring_view packageFilename, std::map<std::wstring, PackedFile>& outPackedFiles, InputStream*& inputStream, bool forceLoadAll, bool showErrors)
{
	// Try to load the package
	RMX_ASSERT(nullptr == inputStream, "Input stream was already opened");
	inputStream = FTX::FileSystem->createInputStream(packageFilename);
	if (nullptr == inputStream)
		return false;

	std::vector<uint8> content;
	content.resize(PackageHeader::HEADER_SIZE);
	if (inputStream->read(&content[0], PackageHeader::HEADER_SIZE) != PackageHeader::HEADER_SIZE)
		return false;

	VectorBinarySerializer headerSerializer(true, content);
	PackageHeader header;
	if (!readPackageHeader(header, headerSerializer))
	{
		if (showErrors)
		{
			if (header.mFormatVersion == PackageHeader::CURRENT_FORMAT_VERSION)
			{
				RMX_ERROR("Unsupported format version " << header.mFormatVersion << " of file '" << WString(packageFilename).toStdString() << "'", );
			}
			else
			{
				RMX_ERROR("Invalid signature of file '" << WString(packageFilename).toStdString() << "'", );
			}
		}
		return false;
	}

	RMX_LOG_INFO("Package '" << WString(packageFilename).toStdString() << "' detected as " << (header.mBigEndian ? "Big-Endian" : "Little-Endian"));

	// Load table of contents
	content.resize(PackageHeader::HEADER_SIZE + header.mEntryHeaderSize);
	if (inputStream->read(&content[PackageHeader::HEADER_SIZE], header.mEntryHeaderSize) != header.mEntryHeaderSize)
		return false;

	// Use a fresh serializer for the entries, as the buffer was resized
	VectorBinarySerializer serializer(true, content);
	serializer.skip(PackageHeader::HEADER_SIZE);

	// Read entry headers
	for (size_t i = 0; i < header.mNumEntries; ++i)
	{
		std::wstring key;
		uint16 keyLengthValue;
		uint32 posValue;
		uint32 sizeValue;

		// Note: The key length is written as a uint16 when the limit is 1024
		serializer.serialize(keyLengthValue);
		if (header.mBigEndian) keyLengthValue = S3AIRByteswap(keyLengthValue);

		if (keyLengthValue > 0 && keyLengthValue <= 1024)
		{
			std::vector<char> buffer((size_t)keyLengthValue);
			serializer.read(&buffer[0], (size_t)keyLengthValue);
			rmx::UTF8Conversion::convertFromUTF8(std::string_view(&buffer[0], (size_t)keyLengthValue), key);
		}
		else if (keyLengthValue > 1024)
		{
			serializer.skip(keyLengthValue);
		}

		serializer.serialize(posValue);
		if (header.mBigEndian) posValue = S3AIRByteswap(posValue);

		serializer.serialize(sizeValue);
		if (header.mBigEndian) sizeValue = S3AIRByteswap(sizeValue);

		PackedFile& packedFile = outPackedFiles[key];
		packedFile.mPath = key;
		packedFile.mPositionInFile = posValue;
		packedFile.mSizeInFile = sizeValue;

#if defined(PLATFORM_PS3)
		if (i < 5)
		{
			RMX_LOG_INFO("Entry " << i << ": '" << WString(key).toStdString() << "', pos: " << posValue << ", size: " << sizeValue);
		}
#endif
	}

	if (forceLoadAll)
	{
		// Read entry contents
		for (auto& pair : outPackedFiles)
		{
			pair.second.mContent.resize((size_t)pair.second.mSizeInFile);
			inputStream->setPosition(pair.second.mPositionInFile);
			const size_t bytesRead = inputStream->read(&pair.second.mContent[0], (size_t)pair.second.mSizeInFile);
			RMX_CHECK(pair.second.mSizeInFile == bytesRead, "Failed to load entry '" << WString(pair.first).toStdString() << "' from package", continue);
			pair.second.mLoadedContent = true;
		}
	}
	return true;
}

void FilePackage::createFilePackage(const std::wstring& packageFilename, const std::vector<std::wstring>& includedPaths, const std::vector<std::wstring>& excludedPaths, const std::wstring& comparisonPath, uint32 contentVersion, bool forceReplace)
{
	// Collect file contents
	std::map<std::wstring, PackedFile> packedFiles;
	{
		FileCrawler fc;
		for (const std::wstring& includedPath : includedPaths)
		{
			fc.addFiles(includedPath, true);
			// TODO: Remove duplicates
		}

		for (size_t i = 0; i < fc.size(); ++i)
		{
			const auto& entry = *fc[i];
			std::wstring path = entry.mPath + entry.mFilename;

			bool add = true;
			for (const std::wstring& excludedPath : excludedPaths)
			{
				if (utils::startsWith(path, excludedPath))
				{
					add = false;
					break;
				}
			}

			if (add)
			{
				std::vector<uint8> content;
				if (FTX::FileSystem->readFile(path, content))
				{
					PackedFile& packedFile = packedFiles[path];
					packedFile.mContent.swap(content);
				}
			}
		}
	}

	// Check against existing file, if there is one already
	if (!forceReplace && !comparisonPath.empty())
	{
		std::map<std::wstring, PackedFile> existingPackedFiles;
		InputStream* inputStream = nullptr;
		if (loadPackage(comparisonPath + packageFilename, existingPackedFiles, inputStream, true, false))
		{
			delete inputStream;

			// Compare
			bool isEqual = (existingPackedFiles.size() == packedFiles.size());
			if (isEqual)
			{
				for (const auto& pair : packedFiles)
				{
					PackedFile* packedFile = mapFind(existingPackedFiles, pair.first);
					if (nullptr == packedFile || packedFile->mContent != pair.second.mContent)
					{
						isEqual = false;
						break;
					}
				}
			}

			if (isEqual)
			{
				// Do not overwrite the existing file, as content has not changed (even though the content version might have changed)
				return;
			}
		}
	}

	// Collect output content
	std::vector<uint8> output;
	size_t entryHeaderSize = 0;
	{
		VectorBinarySerializer serializer(false, output);

		serializer.write(PackageHeader::SIGNATURE, 4);
		const uint32 formatVersion = PackageHeader::CURRENT_FORMAT_VERSION;
		const LE<uint32> formatVersionLE = formatVersion;
		serializer.write(&formatVersionLE.raw, 4);

		const LE<uint32> contentVersionLE = contentVersion;
		serializer.write(&contentVersionLE.raw, 4);

		const size_t headerSizePosition = output.size();
		const LE<uint32> zeroLE = 0;
		serializer.write(&zeroLE.raw, 4);		// Will get overwritten

		const LE<uint32> numEntriesLE = (uint32)packedFiles.size();
		serializer.write(&numEntriesLE.raw, 4);

		for (std::map<std::wstring, PackedFile>::iterator it = packedFiles.begin(); it != packedFiles.end(); ++it)
		{
			const std::wstring& key = it->first;
			PackedFile& packedFile = it->second;
			serializer.write(key, 1024);
			packedFile.mPositionInFile = (uint32)output.size();		// Temporarily misusing this variable to store the position where to write the content's position in file when it got determined
			serializer.write(&zeroLE.raw, 4);							// Will get overwritten
			const LE<uint32> contentSizeLE = (uint32)packedFile.mContent.size();
			serializer.write(&contentSizeLE.raw, 4);
		}

		// Write entry header size
		entryHeaderSize = output.size() - PackageHeader::HEADER_SIZE;
		const LE<uint32> entryHeaderSizeLE = (uint32)entryHeaderSize;
		memcpy(&output[headerSizePosition], &entryHeaderSizeLE.raw, 4);

		for (std::map<std::wstring, PackedFile>::iterator it = packedFiles.begin(); it != packedFiles.end(); ++it)
		{
			PackedFile& packedFile = it->second;
			const uint32 position = (uint32)output.size();
			serializer.write(&packedFile.mContent[0], packedFile.mContent.size());
			const LE<uint32> positionLE = position;
			memcpy(&output[packedFile.mPositionInFile], &positionLE.raw, 4);
			packedFile.mPositionInFile = position;
		}
	}

	// Save output file
	FTX::FileSystem->saveFile(packageFilename, output);
}

bool FilePackage::readPackageHeader(PackageHeader& outHeader, VectorBinarySerializer& serializer)
{
	// Read header
	char signature[4];
	serializer.read(signature, 4);
	if (memcmp(signature, PackageHeader::SIGNATURE, 4) != 0)
		return false;

	// Read format version and detect endianness
	// The file stores the format version at this offset.
	// We read it as raw bytes and then check which endianness matches the expected CURRENT_FORMAT_VERSION.
	
	// On PS3 (BE host), an LE file will have bytes 03 00 00 00.
	// A native 32-bit read will result in 0x03000000.
	const uint8* ptr = serializer.peek();
	uint32 rawFormatVersion = rmx::readMemoryUnaligned<uint32>(ptr);

	// To correctly identify the file's endianness:
	// 1. If raw matches 3, the file's endianness matches the host's endianness.
	// 2. If S3AIRByteswap(raw) matches 3, the file's endianness is the opposite of the host's.
	bool fileIsLittleEndian, fileIsBigEndian;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	fileIsLittleEndian = (S3AIRByteswap(rawFormatVersion) == PackageHeader::CURRENT_FORMAT_VERSION);
	fileIsBigEndian = (rawFormatVersion == PackageHeader::CURRENT_FORMAT_VERSION);
#else
	fileIsLittleEndian = (rawFormatVersion == PackageHeader::CURRENT_FORMAT_VERSION);
	fileIsBigEndian = (S3AIRByteswap(rawFormatVersion) == PackageHeader::CURRENT_FORMAT_VERSION);
#endif

	if (fileIsLittleEndian)
	{
		outHeader.mBigEndian = false;
	}
	else if (fileIsBigEndian)
	{
		outHeader.mBigEndian = true;
	}
	else
	{
		outHeader.mFormatVersion = rawFormatVersion;
		return false;
	}

	outHeader.mFormatVersion = PackageHeader::CURRENT_FORMAT_VERSION;

	// Advance past format version
	serializer.skip(4);

	serializer.serialize(outHeader.mContentVersion);
	serializer.serialize(outHeader.mEntryHeaderSize);
	uint32 numEntries;
	serializer.serialize(numEntries);

	if (outHeader.mBigEndian)
	{
		outHeader.mContentVersion = S3AIRByteswap(outHeader.mContentVersion);
		outHeader.mEntryHeaderSize = S3AIRByteswap(outHeader.mEntryHeaderSize);
		numEntries = S3AIRByteswap(numEntries);
	}
	outHeader.mNumEntries = (size_t)numEntries;

#if defined(PLATFORM_PS3)
	RMX_LOG_INFO("Package detected as " << (outHeader.mBigEndian ? "Big-Endian" : "Little-Endian") << " with " << outHeader.mNumEntries << " entries");
#endif

	RMX_ASSERT(serializer.getReadPosition() == PackageHeader::HEADER_SIZE, "Got wrong package header size");
	return true;
}
