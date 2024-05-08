/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FONTS_FONT_MANAGER_H
#define META_OCEAN_CV_FONTS_FONT_MANAGER_H

#include "ocean/cv/fonts/Fonts.h"
#include "ocean/cv/fonts/Font.h"

#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace CV
{

namespace Fonts
{

/// Forward declaration for `/freetype/FTFontManage.h`
class FTFontManager;

/**
 * This class implements the manager for all fonts.
 * Use this singleton to register or acquire fonts.<br>
 * Before a font can be used/acquired, the font needs to be registered.<br>
 * Font can be registered based on a supported font file, or based on the memory containing a supported font file.
 * @ingroup cvfonts
 */
class OCEAN_CV_FONTS_EXPORT FontManager : public Singleton<FontManager>
{
	friend class Singleton<FontManager>;

	public:

		/**
		 * Registers all fonts available on the system.
		 * @return The number of successfully registered fonts, with range [0, infinity)
		 * @see registerFont().
		 */
		size_t registerSystemFonts();

		/**
		 * Registers a new font by a given font file.
		 * @param fontFile The filname of a new font to be registered, must be valid
		 * @param familyName Optional resulting name of the font family, nullptr otherwise
		 * @param styleName Optional resulting name of the font syle (if known), nullptr otherwise
		 * @return True, if succeeded
		 * @see registerSystemFonts(), registerFonts(), font().
		 */
		bool registerFont(const std::string& fontFile, std::string* familyName = nullptr, std::string* styleName = nullptr);

		/**
		 * Registers all font files located in a specified directory.
		 * @param fontDirectory The directory in which all font files will be registered, must be valid
		 * @return The number successfully registered fonts, with range [0, infinity)
		 * @see registerSystemFonts(), registerFont(), font().
		 */
		size_t registerFonts(const std::string& fontDirectory);

		/**
		 * Registers a new font by a given font file already stored in memory.
		 * @param fontMemory The memory holding the loaded font file, must be valid
		 * @param fontMemorySize The size of the memory in bytes, with range [1, infinity)
		 * @param familyName Optional resulting name of the font family, nullptr otherwise
		 * @param styleName Optional resulting name of the font syle (if known), nullptr otherwise
		 * @return True, if succeeded
		 * @see registerSystemFonts(), registerFonts(), font().
		 */
		bool registerFont(const void* fontMemory, const size_t fontMemorySize, std::string* familyName = nullptr, std::string* styleName = nullptr);

		/**
		 * Registers a new font by a given font file which is already in memory.
		 * The requested font must have been registered before.
		 * @param familyName The name of the font family, empty to get any registered font
		 * @param size The size of the font, in dots, with range [1, infinity)
		 * @param styleName Optinal style name of the font, empty to get any style
		 * @param dpi The dots per inch of the font, with range [1, infinity)
		 * @return The requested font if existing, nullptr otherwise
		 * @see registerFont().
		 */
		SharedFont font(const std::string& familyName, const unsigned int size, const std::string& styleName = std::string(), const unsigned int dpi = 72u);

		/**
		 * Returns whether a specific font is registered.
		 * @param familyName The name of the font family to check
		 * @param styleName Optional explicit font style to check
		 * @return True, if so
		 */
		bool hasFont(const std::string& familyName, const std::string& styleName = std::string()) const;

		/**
		 * Returns the family names of all registered fonts.
		 * @return The family names of all registered fonts
		 */
		std::vector<std::string> familyNames() const;

		/**
		 * Returns the style names of all registered fonts with specified family name.
		 * @param familyName The family name of the fonts for which all style names will be returned
		 * @return The style names of all registered fonts
		 */
		std::vector<std::string> styleNames(const std::string& familyName) const;

		/**
		 * Returns whether this manager is valid and ready to use.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns the family name of the system's default font.
		 * The font may not be registered.
		 * @param styleName Optional resulting style name of the resulting font, nullptr if not of interest
		 * @return The system's default font
		 */
		static std::string systemDefaultFontFamily(std::string* styleName = nullptr);

	protected:

		/**
		 * Creates a new manager for fonts.
		 */
		FontManager();

		/**
		 * Destructs the FontManager object.
		 */
		~FontManager();

	protected:

		/// The actual implementation of the font manager.
		std::unique_ptr<FTFontManager> ftFontManager_;
};

inline bool FontManager::isValid() const
{
	return ftFontManager_.get() != nullptr;
}

}

}

}

#endif // META_OCEAN_CV_FONTS_FONT_MANAGER_H
