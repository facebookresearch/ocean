/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_TEXT_H
#define META_OCEAN_RENDERING_GLES_TEXT_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESShape.h"
#include "ocean/rendering/glescenegraph/GLESTriangles.h"

#include "ocean/base/Singleton.h"

#include "ocean/cv/PixelBoundingBox.h"

#include "ocean/cv/fonts/Font.h"

#include "ocean/rendering/Text.h"
#include "ocean/rendering/Textures.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph text object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESText :
	virtual public GLESShape,
	virtual protected GLESTriangles,
	virtual public Text
{
	friend class GLESFactory;

	protected:

		/**
		 * The manager providing access to the texture containing the font's characters and some associated information.
		 */
		class FontManager : public Singleton<FontManager>
		{
			friend class Singleton<FontManager>;

			public:

				/**
				 * This class stores the relevant information necessary to render the text using one specific font.
				 */
				class FontData
				{
					public:

						/**
						 * Default constructor creating an invalid object.
						 */
						FontData() = default;

						/**
						 * Creates a new object.
						 * @param font The font of this object, must be valid
						 * @param textures The textures object holding the texture with the font's characters, must be valid
						 * @param characters The layout information for all characters
						 * @param textureWidth The width of the texture, in pixel, with range [1, infinity)
						 * @param textureHeight The height of the texture, in pixel, with range [1, infinity)
						 */
						inline FontData(const CV::Fonts::SharedFont& font, const TexturesRef& textures, const CV::Fonts::Font::SharedCharacters& characters, const unsigned int textureWidth, const unsigned int textureHeight);

						/**
						 * Returns whether this object hold valid information.
						 * @return True, if so
						 */
						inline bool isValid() const;

					public:

						/// The font of this object.
						CV::Fonts::SharedFont font_;

						/// The textures object holding the texture with the font's characters.
						TexturesRef textures_;

						/// The layout information for all characters.
						CV::Fonts::Font::SharedCharacters characters_;

						/// The width of the texture, in pixel, with range [1, infinity), 0 if this object is invalid.
						unsigned int textureWidth_ = 0u;

						/// The height of the texture, in pixel, with range [1, infinity), 0 if this object is invalid.
						unsigned int textureHeight_ = 0u;
				};

			protected:

				/**
				 * Definition of a pair combining the font's family name and style name.
				 */
				typedef std::pair<std::string, std::string> FontPair;

				/**
				 * Definition of a map mapping the font's name pair to FontData objects.
				 */
				typedef std::map<FontPair, FontData> FontMap;

			public:

				/**
				 * Returns the relevant information necessary to render the text using a specific font.
				 * @param engine The rendering engine to be used
				 * @param fontFamily The family identifying the font, must be valid
				 * @param styleName The optional style name of the font, empty to use the default style
				 * @return The FontData object containing the relevant information, can be invalid
				 */
				FontData fontData(Rendering::Engine& engine, const std::string& fontFamily, const std::string& styleName = std::string());

				/**
				 * Returns the font (with correct font size) which is used to create the relevant information for the manager.
				 * @param fontFamily The family identifying the font, must be valid
				 * @param styleName The optional style name of the font, empty to use the default style
				 * @return The font, nullptr if unknown
				 */
				CV::Fonts::SharedFont font(const std::string& fontFamily, const std::string& styleName = std::string());

				/**
				 * Releases all text texture resources.
				 * The font manager holds textures even if the owning text objects are disposed.<br>
				 * Therefore, an explicit releasing of all text resources is necessary before the rendering engine is disposed.
				 */
				void release();

			protected:

				/// The map mapping font names to FontData objects.
				FontMap fontMap_;

				/// The manager's lock.
				Lock lock_;
		};

	public:

		/**
		 * Returns the text to be rendered.
		 * @see Text::text().
		 */
		std::string text() const override;

		/**
		 * Returns the actual width and height of the text.
		 * @see Text::size().
		 */
		Vector2 size() const override;

		/**
		 * Returns the font which is used to render the text.
		 * @see Text::fontFamily().
		 */
		std::string fontFamily(std::string* styleName = nullptr) const override;

		/**
		 * Returns the material which is used to render the background of the text.
		 * @see Text::backgroundMaterial().
		 */
		MaterialRef backgroundMaterial() const override;

		/**
		 * Returns the alignment mode of the text.
		 * @see Text::alignmentMode().
		 */
		AlignmentMode alignmentMode() const override;

		/**
		 * Returns the horizontal anchor of the text.
		 * @see Text::horizontalAnchor().
		 */
		HorizontalAnchor horizontalAnchor() const override;

		/**
		 * Returns the vertical anchor of the text.
		 * @return Text::verticalAnchor().
		 */
		VerticalAnchor verticalAnchor() const override;

		/**
		 * Sets the text to be rendred.
		 * @see Text::setText().
		 */
		void setText(const std::string& text) override;

		/**
		 * Sets the size of the text.
		 * @see Text::setSize().
		 */
		bool setSize(const Scalar fixedWidth, const Scalar fixedHeight, const Scalar fixedLineHeight) override;

		/**
		 * Sets the font to be used when rendering the text.
		 * @see Text::setFont().
		 */
		void setFont(const std::string& fontFamily, const std::string& styleName = std::string()) override;

		/**
		 * Returns the material which is used to render the background of the text.
		 * @see Text::setBackgroundMaterial().
		 */
		void setBackgroundMaterial(const MaterialRef& material) override;

		/**
		 * Sets the alignment mode of the text.
		 * @see Text::setAlignmentMode().
		 */
		void setAlignmentMode(const AlignmentMode alignmentMode) override;

		/**
		 * Sets the horizontal anchor of the text.
		 * @see Text::setHorizontalAnchor().
		 */
		void setHorizontalAnchor(const HorizontalAnchor horizontalAnchor) override;

		/**
		 * Sets the vertical anchor of the text.
		 * @see Text::setVerticalAnchor().
		 */
		void setVerticalAnchor(const VerticalAnchor verticalAnchor) override;

		/**
		 * Sets an explicit lookup table which will be used to determine the text geometry (the vertices of the text).
		 * @see Text::setGeometryLookupTable().
		 */
		void setGeometryLookupTable(const LookupCorner2<Vector3>& lookupTable) override;

		/**
		 * Returns whether a specific font is currently available.
		 * @see Text::isFontAvailable().
		 */
		bool isFontAvailable(const std::string& familyName, const std::string& styleName = std::string()) const override;

		/**
		 * Returns the default front currently available.
		 * @see Text::availableDefaultFont().
		 */
		std::string availableDefaultFont(std::string* styleName = nullptr) const override;

		/**
		 * Returns all available family names.
		 * @see Text::availableFamilyNames().
		 */
		std::vector<std::string> availableFamilyNames() const override;

		/**
		 * Returns the style names of all available fonts with specified family name.
		 * @see Text::availableStyleNames().
		 */
		std::vector<std::string> availableStyleNames(const std::string& familyName) const override;

		/**
		 * Renders the triangles defined by the associated vertex set and the defined triangle faces.
		 * @see Renderable::render().
		 */
		void render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights) override;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Returns descriptive information about the object as string.
		 * @see Object::descriptiveInformation().
		 */
		std::string descriptiveInformation() const override;

		/**
		 * Releases all text texture resources.
		 * The font manager holds textures even if the owning text objects are disposed.<br>
		 * Therefore, an explicit releasing of all text resources is necessary before the rendering engine is disposed.
		 */
		static void release();

	protected:

		/**
		 * Creates a new GLESceneGraph box object.
		 */
		GLESText();

		/**
		 * Destructs a GLESceneGraph box object.
		 */
		~GLESText() override;

		/**
		 * Rebuilds the primitive vertex, normals and face sets.
		 * @see GLESShape::rebuildPrimitives().
		 */
		void rebuildPrimitives() override;

		/**
		 * Updates the bounding box of this primitive.
		 * @see GLESIndependentPrimitive::updateBoundingBox().
		 */
		void updateBoundingBox() override;

		/**
		 * Calculates the size of the reulting text block.
		 * @param font The font to be used
		 * @param linePixelBoundingBoxes The resulting bounding boxes of the individual text lines, in the font pixel domain
		 * @param textWidthPixels The resulting width of the text block, in the font pixel domain
		 * @param textHeightPixels The resulting height of the text block, in the font pixel domain
		 * @param textWidth The resulting width of the text block, in space
		 * @param textHeight The resulting height of the text block, in space
		 * @return True, if succeeded
		 */
		bool calculateTextSize(const CV::Fonts::Font& font, CV::PixelBoundingBoxesI& linePixelBoundingBoxes, unsigned int& textWidthPixels, unsigned int& textHeightPixels, Scalar& textWidth, Scalar& textHeight) const;

	protected:

		/// The text which is actually rendered.
		std::string text_;

		/// The fixed size of the text; (0, 0) to use the.
		Vector2 fixedWidthHeight_ = Vector2(0, 0);

		/// The fixed height of each text line; 0 to use 'fixedWidthHeight_'.
		Scalar fixedLineHeight_ = Scalar(0.05);

		/// The actual resulting size of the rendered text; (0, 0) until known
		Vector2 resultingSize_ = Vector2(0, 0);

		/// The font family to be used.
		std::string fontFamily_;

		/// The font's style name to be used, empty to use the default style.
		std::string styleName_;

		/// The text's alignment mode.
		AlignmentMode alignmentMode_ = AM_LEFT;

		/// The text's horizontal anchor.
		HorizontalAnchor horizontalAnchor_ = HA_LEFT;

		/// The text's vertical anchor.
		VerticalAnchor verticalAnchor_ = VA_TOP;

		/// True, if the text premitives need to be rebuilt.
		bool needsRebuild_ = false;

		/// The textures object holding the texture with the font's characters.
		TexturesRef textures_;

		/// The background material to be used.
		MaterialRef backgroundMaterial_;

		/// The optional lookup table for the text geometry.
		LookupCorner2<Vector3> lookupTable_;
};

inline GLESText::FontManager::FontData::FontData(const CV::Fonts::SharedFont& font, const TexturesRef& textures, const CV::Fonts::Font::SharedCharacters& characters, const unsigned int textureWidth, const unsigned int textureHeight) :
	font_(font),
	textures_(textures),
	characters_(characters),
	textureWidth_(textureWidth),
	textureHeight_(textureHeight)
{
	// nothing to do here
}

inline bool GLESText::FontManager::FontData::isValid() const
{
	return bool(textures_);
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TEXT_H
