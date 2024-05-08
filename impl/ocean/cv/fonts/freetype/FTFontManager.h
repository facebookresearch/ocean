/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FONTS_FREETYPE_FT_FONT_MANAGER_H
#define META_OCEAN_CV_FONTS_FREETYPE_FT_FONT_MANAGER_H

#include "ocean/cv/fonts/freetype/FreeType.h"

#include "ocean/cv/fonts/Font.h"

#include <unordered_map>

namespace Ocean
{

namespace CV
{

namespace Fonts
{

/**
 * This class implements the actual manager for all FreeType fonts.
 * Do not call functions of this class, but use the `FontManager` singleton instead.
 * All fonts which are registered at the FontManager do not consume aditional resources until the font is actually aqcuired.
 * @see FontManager.
 * @ingroup cvfonts
 */
class OCEAN_CV_FONTS_EXPORT FTFontManager
{
	friend class FontManager;
	friend class Face;

	protected:

		/**
		 * This class wrapps a FreeType face.
		 * Each face represent a font with individual size.
		 */
		class OCEAN_CV_FONTS_EXPORT Face
		{
			protected:

				/**
				 * Definition of a map mapping size and dpi to font objects.
				 */
				typedef std::unordered_map<uint64_t, SharedFont> FontMap;

			public:

				/**
				 * Move constructor.
				 * @param face The face object to be moved
				 */
				inline Face(Face&& face);

				/**
				 * Creates a new face object for a given FreeType face and keeps ownership of this face object.
				 * @param fontManager The owner of this new object
				 * @param ftFace The FreeType face defining the font, will be release after usage, can be nullptr if a valid 'filename' is provided
				 * @param filename The filename from which the FreeType face can be accessed when actually needed, can be empty if 'ftFace' is valid
				 */
				inline Face(FTFontManager& fontManager, const FT_Face& ftFace, const std::string& filename);

				/**
				 * Destructs a Face object.
				 */
				inline ~Face();

				/**
				 * Returns the font associated with this Face object for a specified size.
				 * If the font does not exist yet, the font will be create and initialized.
				 * @param size The size of the font, in dots, with range [1, infinity)
				 * @param dpi The dots per inch of the font, with range [1, infinity)
				 * @return The requested font, nullptr if invalid
				 */
				SharedFont font(const unsigned int size, const unsigned int dpi);

				/**
				 * Returns the filename of this face object.
				 * @return The face object's filename, if known
				 */
				const std::string& filename() const;

				/**
				 * Move operator.
				 * @param face The object to be moved
				 * @return The reference to this object
				 */
				Face& operator=(Face&& face);

			protected:

				/**
				 * Disabled copy constructor.
				 * @param face The object which would be copied
				 */
				Face(const Face& face) = delete;

				/**
				 * Disabled copy operator.
				 * @param face The object which would be copied
				 * @return The reference to this object
				 */
				Face& operator=(const Face& face) = delete;

			protected:

				/// The owner of the face object.
				FTFontManager& fontManager_;

				/// The name of the file from which the FreeType face can be acquired if requested.
				std::string filename_;

				/// The FreeType face (which represents a font) associated with this object, nullptr if the face object needs to be acquired from the file first.
				FT_Face ftFace_ = nullptr;

				/// The map of font objects, one object for each individual combination of size and dpi.
				FontMap fontMap_;
		};

		/**
		 * Definition of a map mapping style names to Face objects.
		 */
		typedef std::unordered_map<std::string, Face> FaceMap;

		/**
		 * Definition of a map mapping family names to face maps.
		 */
		typedef std::unordered_map<std::string, FaceMap> FamilyMap;

	public:

		/**
		 * Destructs the FontManager object.
		 */
		~FTFontManager();

	protected:

		/**
		 * Creates a new manager for fonts.
		 */
		FTFontManager();

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
		 * Registers a new font by a given font file which is already in memory.
		 * @param fontMemory The memory holding the loaded font file, must be valid
		 * @param fontMemorySize The size of the memory in bytes, with range [1, infinity)
		 * @param familyName Optional resulting name of the font family, nullptr otherwise
		 * @param styleName Optional resulting name of the font syle (if known), nullptr otherwise
		 * @return True, if succeeded
		 * @see registerFonts(), font().
		 */
		bool registerFont(const void* fontMemory, const size_t fontMemorySize, std::string* familyName = nullptr, std::string* styleName = nullptr);

		/**
		 * Registers a new font by a FreeType face.
		 * @param ftFace The FreeType face representing the font, the FreeType face will be released afterwards
		 * @param filename The filename from which the FreeType face can be re-created so that a placeholder is registered, empty if unknown or to avoid registering a placeholder
		 * @param familyName Optional resulting name of the font family, nullptr otherwise
		 * @param styleName Optional resulting name of the font syle (if known), nullptr otherwise
		 * @return True, if succeeded
		 * @see registerFonts(), font().
		 */
		bool registerFont(const FT_Face& ftFace, const std::string& filename, std::string* familyName = nullptr, std::string* styleName = nullptr);

		/**
		 * Returns a font with specific family name and size.
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
		 * @return The style names of all registered font
		 */
		std::vector<std::string> styleNames(const std::string& familyName) const;

		/**
		 * Returns whether this manager is valid and ready to use.
		 * @return True, if so
		 */
		inline bool isValid() const;

	protected:

		/// The map mapping font family names to face maps.
		FamilyMap familyMap_;

		/// The FreeType library object.
		FT_Library library_;

		/// The manager's lock.
		mutable Lock lock_;
};

inline FTFontManager::Face::Face(Face&& face) :
	fontManager_(face.fontManager_)
{
	*this = std::move(face);
}

inline FTFontManager::Face::Face(FTFontManager& fontManager, const FT_Face& ftFace, const std::string& filename) :
	fontManager_(fontManager),
	filename_(filename),
	ftFace_(ftFace)
{
	ocean_assert(ftFace_ != nullptr || !filename_.empty());
}

inline FTFontManager::Face::~Face()
{
	if (ftFace_ != nullptr)
	{
		FT_Done_Face(ftFace_);
	}
}

inline const std::string& FTFontManager::Face::filename() const
{
	return filename_;
}

inline bool FTFontManager::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return library_ != nullptr;
}

}

}

}

#endif // META_OCEAN_CV_FONTS_FREETYPE_FT_FONT_MANAGER_H
