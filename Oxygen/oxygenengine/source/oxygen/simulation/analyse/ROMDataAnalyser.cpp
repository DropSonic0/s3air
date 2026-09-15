/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/simulation/analyse/ROMDataAnalyser.h"
#include "oxygen/application/Configuration.h"
#include "oxygen/helper/JsonHelper.h"


ROMDataAnalyser::ROMDataAnalyser()
{
	// Load files
	loadDataFromJSONs(Configuration::instance().mAnalysisDir);

#ifdef DEBUG
	// Just for generating some data for development
//	processData();
#endif
}

ROMDataAnalyser::~ROMDataAnalyser()
{
	// Save everything
	if (mAnyChange)
	{
		saveDataToJSONs(Configuration::instance().mAnalysisDir);
	}
}

bool ROMDataAnalyser::hasEntry(std::string_view categoryName, uint32 address) const
{
	return (nullptr != const_cast<ROMDataAnalyser*>(this)->findEntry(categoryName, address, false));
}

void ROMDataAnalyser::beginEntry(std::string_view categoryName, uint32 address)
{
	RMX_CHECK(nullptr == mCurrentCategory, "ROMDataAnalyser: Don't call \"beginEntry\" without closing old entry with \"endEntry\"", );
	RMX_CHECK(nullptr == mCurrentEntry,    "ROMDataAnalyser: Don't call \"beginEntry\" without closing old entry with \"endEntry\"", );
	RMX_CHECK(mCurrentObjectStack.empty(), "ROMDataAnalyser: Don't call \"beginEntry\" without closing old entry with \"endEntry\"", );

	mCurrentEntry = findEntry(categoryName, address, true, &mCurrentCategory);
	mCurrentObjectStack.clear();
	mCurrentObjectStack.push_back(&mCurrentEntry->mContent);
	mAnyChange = true;
}

void ROMDataAnalyser::endEntry()
{
	RMX_CHECK(mCurrentObjectStack.size() <= 1, "ROMDataAnalyser: Close all objects before calling \"endEntry\"", );

	mCurrentCategory = nullptr;
	mCurrentEntry = nullptr;
	mCurrentObjectStack.clear();
}

void ROMDataAnalyser::addKeyValue(std::string_view key, std::string_view value)
{
	RMX_CHECK(!mCurrentObjectStack.empty(), "ROMDataAnalyser: No current object when calling \"addKeyValue\"", return);

	mCurrentObjectStack.back()->mKeyValuePairs[std::string(key.data(), key.length())].assign(value.data(), value.length());
	mAnyChange = true;
}

void ROMDataAnalyser::beginObject(std::string_view key)
{
	RMX_CHECK(!mCurrentObjectStack.empty(), "ROMDataAnalyser: No current object when calling \"beginObject\"", return);

	Object& child = mCurrentObjectStack.back()->mChildObjects[std::string(key.data(), key.length())];
	mCurrentObjectStack.push_back(&child);
	mAnyChange = true;
}

void ROMDataAnalyser::endObject()
{
	RMX_CHECK(!mCurrentObjectStack.empty(), "ROMDataAnalyser: No current object when calling \"endObject\"", return);

	mCurrentObjectStack.pop_back();
}

ROMDataAnalyser::Category* ROMDataAnalyser::findCategory(std::string_view categoryName, bool create)
{
	const uint64 hash = rmx::getMurmur2_64(categoryName);
	const auto it = mCategories.find(hash);
	if (it == mCategories.end())
	{
		if (!create)
			return nullptr;

		Category& category = mCategories[hash];
		category.mName.assign(categoryName.data(), categoryName.length());
		return &category;
	}
	else
	{
		return &it->second;
	}
}

ROMDataAnalyser::Entry* ROMDataAnalyser::findEntry(std::string_view categoryName, uint32 address, bool create, Category** outCategory)
{
	Category* category = findCategory(categoryName, create);
	if (nullptr != outCategory)
		*outCategory = category;

	if (nullptr == category)
		return nullptr;

	const auto it = category->mEntries.find(address);
	if (it == category->mEntries.end())
	{
		if (!create)
			return nullptr;

		Entry& entry = category->mEntries[address];
		return &entry;
	}
	else
	{
		return &it->second;
	}
}

void ROMDataAnalyser::loadDataFromJSONs(std::wstring_view filepath)
{
	mCategories.clear();

	FileCrawler fc;
	fc.addFiles(std::wstring(filepath.data(), filepath.length()) + L"romdata_*.json");
	for (size_t fileIndex = 0; fileIndex < fc.size(); ++fileIndex)
	{
		const FileCrawler::FileEntry* fileEntry = fc[fileIndex];
		if (nullptr == fileEntry)
			continue;

		const std::wstring filename = std::wstring(filepath.data(), filepath.length()) + fileEntry->mFilename;
		Json::Value root = JsonHelper::loadFile(filename);
		if (root.isNull())
			continue;

		for (Json::ValueConstIterator it = root.begin(); it != root.end(); ++it)
		{
			for (Json::ValueConstIterator it2 = it->begin(); it2 != it->end(); ++it2)
			{
				const uint32 address = (uint32)rmx::parseInteger(String(it2.key().asCString()));
				Object& object = findEntry(it.key().asCString(), address, true)->mContent;
				recursiveLoadDataFromJSON(*it2, object);
			}
		}
	}
}

void ROMDataAnalyser::recursiveLoadDataFromJSON(const Json::Value& json, Object& outObject)
{
	for (Json::ValueConstIterator it = json.begin(); it != json.end(); ++it)
	{
		if (it->isObject())
		{
			Object& child = outObject.mChildObjects[it.key().asCString()];
			recursiveLoadDataFromJSON(*it, child);
		}
		else
		{
			outObject.mKeyValuePairs[it.key().asCString()] = it->asCString();
		}
	}
}

void ROMDataAnalyser::saveDataToJSONs(std::wstring_view filepath)
{
	for (std::unordered_map<uint64, Category>::const_iterator it = mCategories.begin(); it != mCategories.end(); ++it)
	{
		Json::Value root;

		const Category& category = it->second;
		Json::Value categoryJson;
		for (std::map<uint32, Entry>::const_iterator it2 = category.mEntries.begin(); it2 != category.mEntries.end(); ++it2)
		{
			const Entry& entry = it2->second;
			Json::Value entryJson;
			recursiveSaveDataToJSON(entryJson, entry.mContent);
			categoryJson[rmx::hexString(it2->first, 6).c_str()] = entryJson;
		}
		root[category.mName.c_str()] = categoryJson;

		// Save file
		const std::wstring filename = std::wstring(filepath.data(), filepath.length()) + L"romdata_" + *String(category.mName).toWString() + L".json";
		JsonHelper::saveFile(filename, root);

	#if 0
		if (category.mName == "TableLookup")
		{
			String output;
			for (const auto& pair2 : category.mEntries)
			{
				output << rmx::hexString(pair2.first, 6) << "\r\n";
				output << "\t// Targets:\r\n";
				for (const auto& pair3 : pair2.second.mContent.mKeyValuePairs)
				{
					const int value = (int)rmx::parseInteger(pair3.first);
					output << "\t//  - " << pair3.second << "\t-> objA0.base_state = " << rmx::hexString(value, 2) << "\r\n";
				}
				output << "\r\n";
			}
			output.saveFile("TableLookup.txt");
		}
	#endif
	}

	mAnyChange = false;
}

void ROMDataAnalyser::recursiveSaveDataToJSON(Json::Value& outJson, const Object& object)
{
	for (std::map<std::string, std::string>::const_iterator it = object.mKeyValuePairs.begin(); it != object.mKeyValuePairs.end(); ++it)
	{
		outJson[it->first.c_str()] = it->second.c_str();
	}
	for (std::map<std::string, Object>::const_iterator it = object.mChildObjects.begin(); it != object.mChildObjects.end(); ++it)
	{
		Json::Value childJson;
		recursiveSaveDataToJSON(childJson, it->second);
		outJson[it->first.c_str()] = childJson;
	}
}

void ROMDataAnalyser::processData()
{
#ifdef DEBUG
	String output[2];
	String entryOutput;
	const Category& debugCategory = mCategories[16289744412002469748];
	for (std::map<uint32, Entry>::const_iterator it = debugCategory.mEntries.begin(); it != debugCategory.mEntries.end(); ++it)
	{
		entryOutput.clear();
		const Object& parent = it->second.mContent;
		const auto& keyValue = parent.mKeyValuePairs;

		if (*mapFind(keyValue, std::string("function")) == "SpawnChildObjects")
		{
			for (size_t index = 0; ; ++index)
			{
				const Object* child = mapFind(parent.mChildObjects, std::to_string(index));
				if (nullptr == child)
					break;

				const uint32 updateAddress = (uint32)rmx::parseInteger(*mapFind(child->mKeyValuePairs, std::string("update_routine")));
				const uint8 subType = (uint8)rmx::parseInteger(*mapFind(child->mKeyValuePairs, std::string("subtype")));
				const int offset_x = (int8)(uint8)rmx::parseInteger(*mapFind(child->mKeyValuePairs, std::string("offset_x")));
				const int offset_y = (int8)(uint8)rmx::parseInteger(*mapFind(child->mKeyValuePairs, std::string("offset_y")));

				entryOutput << "spawnChildObject(" << rmx::hexString(updateAddress, 6) << ", " << rmx::hexString(subType, 2) << ", " << offset_x << ", " << offset_y << ")\r\n";
			}

			if (entryOutput.nonEmpty())
			{
				output[0] << "\r\n";
				output[0] << "// \"spawnChildObjects(" << rmx::hexString(it->first, 6) << ")\" replaced by:\r\n";
				output[0] << entryOutput;
				output[0] << "\r\n\r\n";
			}
		}
		else if (*mapFind(keyValue, std::string("function")) == "SpawnSimpleChildObjects")
		{
			const uint32 updateAddress = (uint32)rmx::parseInteger(*mapFind(keyValue, std::string("update_routine")));
			const uint8 count = (uint8)rmx::parseInteger(*mapFind(keyValue, std::string("count")));

			output[1] << "\r\n";
			output[1] << "// \"spawnSimpleChildObjects(" << rmx::hexString(it->first, 6) << ")\" replaced by:\r\n";
			output[1] << "spawnSimpleChildObjects(" << rmx::hexString(updateAddress, 6) << ", " << count << ")\r\n\r\n";
		}
	}
	output[0].saveFile("___internal/analysis/romdata_out_SpawnChildObjects.lemon");
	output[1].saveFile("___internal/analysis/romdata_out_SpawnSimpleChildObjects.lemon");
#endif
}
