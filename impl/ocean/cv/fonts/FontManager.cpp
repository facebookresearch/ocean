/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/fonts/FontManager.h"

#include "ocean/cv/fonts/freetype/FTFontManager.h"

namespace Ocean
{

namespace CV
{

namespace Fonts
{

FontManager::FontManager() :
	ftFontManager_(new FTFontManager())
{
	// nothing to do here
}

FontManager::~FontManager()
{
	// nothing to do here
}

size_t FontManager::registerSystemFonts()
{
	if (ftFontManager_)
	{
		return ftFontManager_->registerSystemFonts();
	}

	return 0;
}

bool FontManager::registerFont(const std::string& fontFile, std::string* familyName, std::string* styleName)
{
	if (ftFontManager_)
	{
		return ftFontManager_->registerFont(fontFile, familyName, styleName);
	}

	return false;
}

size_t FontManager::registerFonts(const std::string& fontDirectory)
{
	if (ftFontManager_)
	{
		return ftFontManager_->registerFonts(fontDirectory);
	}

	return false;
}

bool FontManager::registerFont(const void* fontMemory, const size_t fontMemorySize, std::string* familyName, std::string* styleName)
{
	if (ftFontManager_)
	{
		return ftFontManager_->registerFont(fontMemory, fontMemorySize, familyName, styleName);
	}

	return false;
}

SharedFont FontManager::font(const std::string& familyName, const unsigned int size, const std::string& styleName, const unsigned int dpi)
{
	if (ftFontManager_)
	{
		return ftFontManager_->font(familyName, size, styleName, dpi);
	}

	return nullptr;
}

bool FontManager::hasFont(const std::string& familyName, const std::string& styleName) const
{
	if (ftFontManager_)
	{
		return ftFontManager_->hasFont(familyName, styleName);
	}

	return false;
}

std::vector<std::string> FontManager::familyNames() const
{
	if (ftFontManager_)
	{
		return ftFontManager_->familyNames();
	}

	return std::vector<std::string>();
}

std::vector<std::string> FontManager::styleNames(const std::string& familyName) const
{
	if (ftFontManager_)
	{
		return ftFontManager_->styleNames(familyName);
	}

	return std::vector<std::string>();
}

std::string FontManager::systemDefaultFontFamily(std::string* styleName)
{
	if (styleName != nullptr)
	{
		*styleName = "Regular";
	}

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS
	return std::string("Calibri");
#endif

#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS
	return std::string("Helvetica");
#endif

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	return std::string(".SF Compact Rounded");
#endif

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	return std::string("Roboto");
#endif

	return std::string("Arial");
}

}

}

}
