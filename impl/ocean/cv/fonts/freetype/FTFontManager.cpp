/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/fonts/freetype/FTFontManager.h"
#include "ocean/cv/fonts/freetype/FTFont.h"
#include "ocean/cv/fonts/freetype/Utilities.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

namespace Ocean
{

namespace CV
{

namespace Fonts
{

SharedFont FTFontManager::Face::font(const unsigned int size, const unsigned int dpi)
{
	ocean_assert(size != 0u && dpi != 0u);

	if (ftFace_ == nullptr)
	{
		ocean_assert(!filename_.empty());

		constexpr FT_Long faceIndex = 0;
		FT_Face ftFace = nullptr;

		const FT_Error error = FT_New_Face(fontManager_.library_, filename_.c_str(), faceIndex, &ftFace);

		if (error != FT_Err_Ok)
		{
			ocean_assert(false && "Failed to (re-)register the font, this can happen if the font file has been deleted");
			return SharedFont();
		}

		ftFace_ = ftFace;
	}

	const uint64_t sizePair = uint64_t(dpi) << 32ull | uint64_t(size);

	FontMap::iterator i = fontMap_.find(sizePair);

	if (i == fontMap_.cend())
	{
		SharedFont newFont(new FTFont(ftFace_, size, dpi));

		if (newFont->isValid())
		{
			i = fontMap_.insert(std::make_pair(sizePair, std::move(newFont))).first;
		}
		else
		{
			i = fontMap_.insert(std::make_pair(sizePair, nullptr)).first;
		}
	}

	return i->second;
}

FTFontManager::Face& FTFontManager::Face::operator=(Face&& face)
{
	ocean_assert(&fontManager_ == &face.fontManager_);

	if (this != &face)
	{
		filename_ = std::move(face.filename_);

		ftFace_ = face.ftFace_;
		face.ftFace_ = nullptr;

		fontMap_ = std::move(face.fontMap_);
	}

	return *this;
}

FTFontManager::FTFontManager() :
	library_(nullptr)
{
	const FT_Error error = FT_Init_FreeType(&library_);

	if (error != FT_Err_Ok)
	{
		ocean_assert(false && "Failed to initialize free type!");

		Log::error() << "Failed to initialize FreeType: " << Utilities::translateErrorCode(error);
	}
}

FTFontManager::~FTFontManager()
{
	familyMap_.clear();

	if (library_ != nullptr)
	{
		const FT_Error error = FT_Done_FreeType(library_);
		ocean_assert_and_suppress_unused(error == FT_Err_Ok, error);
	}
}

size_t FTFontManager::registerSystemFonts()
{
	size_t registeredFonts = 0;

#if defined(OCEAN_PLATFORM_BUILD_APPLE)

	const IO::Directory directory("/System/Library/Fonts");

	if (directory.exists())
	{
		registeredFonts += registerFonts(directory());

		const IO::Directories subDirectories(directory.findDirectories(true /*recursive*/));

		for (const IO::Directory& subDirectory : subDirectories)
		{
			registeredFonts += registerFonts(subDirectory());
		}
	}

#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)

	const IO::Directory directory("/system/fonts");

	if (directory.exists())
	{
		registeredFonts += registerFonts(directory());

		const IO::Directories subDirectories(directory.findDirectories(true /*recursive*/));

		for (const IO::Directory& subDirectory : subDirectories)
		{
			registeredFonts += registerFonts(subDirectory());
		}
	}

#elif defined(OCEAN_PLATFORM_BUILD_WINDOWS)

	const IO::Directory directory("C:\\Windows\\Fonts");

	if (directory.exists())
	{
		registeredFonts += registerFonts(directory());
	}

#elif defined(OCEAN_PLATFORM_BUILD_LINUX)

	const IO::Directory directory("/usr/share/fonts");

	if (directory.exists())
	{
		registeredFonts += registerFonts(directory());

		const IO::Directories subDirectories(directory.findDirectories(true /*recursive*/));

		for (const IO::Directory& subDirectory : subDirectories)
		{
			registeredFonts += registerFonts(subDirectory());
		}
	}
#endif

	return registeredFonts;
}

bool FTFontManager::registerFont(const std::string& fontFile, std::string* familyName, std::string* styleName)
{
	ocean_assert(isValid());
	ocean_assert(!fontFile.empty());

	if (!isValid() || fontFile.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	constexpr FT_Long faceIndex = 0;
	FT_Face ftFace = nullptr;

	const FT_Error error = FT_New_Face(library_, fontFile.c_str(), faceIndex, &ftFace);

	if (error != FT_Err_Ok)
	{
		Log::warning() << "Failed to register font '" << fontFile << "': " << Utilities::translateErrorCode(error);
		return false;
	}

	return registerFont(ftFace, fontFile, familyName, styleName);
}

size_t FTFontManager::registerFonts(const std::string& fontDirectory)
{
	ocean_assert(!fontDirectory.empty());

	if (fontDirectory.empty())
	{
		return 0;
	}

	const IO::Directory directory(fontDirectory);

	if (!directory.exists())
	{
		Log::warning() << "The provided directory '" << directory() << "' does not exist";
		return 0;
	}

	const IO::Files files(directory.findFiles("*")); // searching for ttf, ttc, otf, ...

	size_t succeeded = 0;

	for (const IO::File& file : files)
	{
		if (registerFont(file()))
		{
			++succeeded;
		}
	}

	return succeeded;
}

bool FTFontManager::registerFont(const void* fontMemory, const size_t fontMemorySize, std::string* familyName, std::string* styleName)
{
	ocean_assert(isValid());
	ocean_assert(fontMemory != nullptr && fontMemorySize != 0);

	if (!isValid() || fontMemory == nullptr || fontMemorySize == 0)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	constexpr FT_Long faceIndex = 0;
	FT_Face ftFace = nullptr;

	const FT_Error error = FT_New_Memory_Face(library_, (const FT_Byte*)(fontMemory), FT_Long(fontMemorySize), faceIndex, &ftFace);

	if (error != FT_Err_Ok)
	{
#ifdef OCEAN_DEBUG
		Log::warning() << "<debug> Failed to register font from memory: " << Utilities::translateErrorCode(error);
#endif
		return false;
	}

	return registerFont(ftFace, std::string(), familyName, styleName);
}

bool FTFontManager::registerFont(const FT_Face& ftFace, const std::string& filename, std::string* familyName, std::string* styleName)
{
	ocean_assert(ftFace->family_name != nullptr);

	if (ftFace->family_name != nullptr)
	{
		FaceMap& faceMap = familyMap_[std::string(ftFace->family_name)];

		std::string internalStyleName = ftFace->style_name ? std::string(ftFace->style_name) : std::string();

		if (faceMap.find(internalStyleName) == faceMap.cend())
		{
			if (familyName)
			{
				*familyName = std::string(ftFace->family_name);
			}

			if (styleName)
			{
				*styleName = std::move(internalStyleName);
			}

			if (filename.empty())
			{
				faceMap.insert(std::make_pair(internalStyleName, Face(*this, ftFace, std::string())));
			}
			else
			{
				// now as we know the font's family name and style, we keep a placeholder with associated file until the font is actually acquired

				FT_Done_Face(ftFace);

				faceMap.insert(std::make_pair(internalStyleName, Face(*this, nullptr, filename)));
			}
			return true;
		}
	}

	// we did not use the face, so we have to release it now

	FT_Done_Face(ftFace);

	return false;
}

SharedFont FTFontManager::font(const std::string& familyName, const unsigned int size, const std::string& styleName, const unsigned int dpi)
{
	const ScopedLock scopedLock(lock_);

	if (familyName.empty())
	{
		// we return any of the fonts

		if (familyMap_.empty())
		{
			return nullptr;
		}

		FaceMap& faceMap = familyMap_.begin()->second;

		if (faceMap.empty())
		{
			return nullptr;
		}

		return faceMap.begin()->second.font(size, dpi);
	}

	FamilyMap::iterator iFamily = familyMap_.find(familyName);

	if (iFamily != familyMap_.end())
	{
		FaceMap& faceMap = iFamily->second;

		FaceMap::iterator iFace = faceMap.find(styleName);

		if (iFace != faceMap.cend())
		{
			return iFace->second.font(size, dpi);
		}

		if (styleName.empty() && !faceMap.empty())
		{
			return faceMap.begin()->second.font(size, dpi);
		}

		return nullptr;
	}

	return nullptr;
}

bool FTFontManager::hasFont(const std::string& familyName, const std::string& styleName) const
{
	const ScopedLock scopedLock(lock_);

	const FamilyMap::const_iterator iFamily = familyMap_.find(familyName);

	if (iFamily == familyMap_.cend())
	{
		return false;
	}

	if (!styleName.empty())
	{
		for (FaceMap::const_iterator iFace = iFamily->second.cbegin(); iFace != iFamily->second.cend(); ++iFace)
		{
			if (iFace->first == styleName)
			{
				return true;
			}
		}

		return false;
	}

	return true;
}

std::vector<std::string> FTFontManager::familyNames() const
{
	const ScopedLock scopedLock(lock_);

	std::vector<std::string> result;
	result.reserve(familyMap_.size());

	for (FamilyMap::const_iterator i = familyMap_.cbegin(); i != familyMap_.cend(); ++i)
	{
		result.emplace_back(i->first);
	}

	return result;
}

std::vector<std::string> FTFontManager::styleNames(const std::string& familyName) const
{
	const ScopedLock scopedLock(lock_);

	const FamilyMap::const_iterator iFamily = familyMap_.find(familyName);

	if (iFamily == familyMap_.cend())
	{
		return std::vector<std::string>();
	}

	std::vector<std::string> result;

	result.reserve(familyMap_.size());

	for (FaceMap::const_iterator iFace = iFamily->second.cbegin(); iFace != iFamily->second.cend(); ++iFace)
	{
		result.emplace_back(iFace->first);
	}

	return result;
}

}

}

}
