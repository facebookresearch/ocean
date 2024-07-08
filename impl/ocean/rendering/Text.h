/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_TEXT_H
#define META_OCEAN_RENDERING_TEXT_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/Shape.h"

#include "ocean/math/Lookup2.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Text;

/**
 * Definition of a smart object reference holding a text.
 * @see SmartObjectRef, Text.
 * @ingroup rendering
 */
typedef SmartObjectRef<Text> TextRef;

/**
 * This class is the base class for all texts.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Text : virtual public Shape
{
	public:

		/**
		 * Definition of individual alignment modes within the text block.
		 */
		enum AlignmentMode
		{
			/**
			 * The text is aligned to the left of the text block.
			 * The alignment mode is visualized below:
			 * <pre>
			 *  ------------------------
			 * | This is the first line |
			 * | The second line        |
			 * | ...                    |
			 * | The last line          |
			 *  ------------------------
			 * </pre>
			 */
			AM_LEFT,

			/**
			 * The text is aligned at the center of the text block.
			 * The alignment mode is visualized below:
			 * <pre>
			 *  ------------------------
			 * | This is the first line |
			 * |    The second line     |
			 * |          ...           |
			 * |      The last line     |
			 *  ------------------------
			 * </pre>
			 */
			AM_CENTER,

			/**
			 * The text is aligned to the left of the text block.
			 * The alignment mode is visualized below:
			 * <pre>
			 *  ------------------------
			 * | This is the first line |
			 * |        The second line |
			 * |                    ... |
			 * |          The last line |
			 *  ------------------------
			 * </pre>
			 */
			AM_RIGHT
		};

		/**
		 * Definition of individual horizontal anchor points for the entire text block.
		 */
		enum HorizontalAnchor
		{
			/**
			 * The text block is anchored at the left edge.
			 * The anchor mode is visualized below:
			 * <pre>
			 * (A)-------------------------          (A): HA_LEFT, VA_TOP
			 *  |  This is the first line  |
			 * (B) ...                     |         (B): HA_LEFT, VA_MIDDLE
			 *  |  The last line           |
			 * (C)-------------------------          (C): HA_LEFT, VA_BOTTOM
			 * </pre>
			 */
			HA_LEFT,

			/**
			 * The text block is anchored at the center of the text block.
			 * The anchor mode is visualized below:
			 * <pre>
			 *  ------------(A)------------          (A): HA_CENTER, VA_TOP
			 * |  This is the first line   |
			 * |  ...       (B)            |         (B): HA_CENTER, VA_MIDDLE
			 * |  The last line            |
			 *   -----------(C)------------          (C): HA_CENTER, VA_BOTTOM
			 * </pre>
			 */
			HA_CENTER,

			/**
			 * The text block is anchored at the right edge.
			 * The anchor mode is visualized below:
			 * <pre>
			 *  -------------------------(A)          (A): HA_RIGHT, VA_TOP
			 * |  This is the first line  |
			 * |  ...                    (B)          (B): HA_RIGHT, VA_MIDDLE
			 * |  The last line           |
			 *  -------------------------(C)          (C): HA_RIGHT, VA_BOTTOM
			 * </pre>
			 */
			HA_RIGHT
		};

		/**
		 * Definition of individual vertical anchor points for the entire text block.
		 */
		enum VerticalAnchor
		{
			/// The text block is anchored at the top edge.
			VA_TOP,
			/// The text block is anchored at middle oft he text block.
			VA_MIDDLE,
			/// The text block is anchored at the bottom edge.
			VA_BOTTOM
		};

	public:

		/**
		 * Returns the text to be rendered.
		 * @return The text which will be rendered
		 */
		virtual std::string text() const;

		/**
		 * Returns the actual width and height of the text.
		 * @return The text's width and height
		 */
		virtual Vector2 size() const;

		/**
		 * Returns the font which is used to render the text.
		 * @param styleName Optional resulting style name of the font
		 * @return The text's font
		 */
		virtual std::string fontFamily(std::string* styleName = nullptr) const;

		/**
		 * Returns the material which is used to render the background of the text.
		 * By default, the text will be rendered with a black opaque background.
		 * @return The text's background material
		 */
		virtual MaterialRef backgroundMaterial() const;

		/**
		 * Returns the alignment mode of the text.
		 * @return The text's alignment mode
		 */
		virtual AlignmentMode alignmentMode() const;

		/**
		 * Returns the horizontal anchor of the text.
		 * @return The text's horizontal anchor
		 */
		virtual HorizontalAnchor horizontalAnchor() const;

		/**
		 * Returns the vertical anchor of the text.
		 * @return The text's vertical anchor
		 */
		virtual VerticalAnchor verticalAnchor() const;

		/**
		 * Sets the text to be rendred.
		 * Multiple lines can be rendered by adding a line feed character ('\n') at the end of a line.
		 * @param text The text to be rendered
		 */
		virtual void setText(const std::string& text);

		/**
		 * Sets the size of the text.
		 * Four different size modes are supported:<br>
		 * 1) The text block has a fixed width and the height is automatically determined by preserving the aspect ratio.<br>
		 * 2) The text block has a fixed height and the width is automatically determined by preserving the aspect ratio.<br>
		 * 3) The text block has a fixed width and fixed height the text's aspect ratio is not preserved.<br>
		 * 4) The width and height of the text block is automatically determined by ensuring that each text line has a specified height while the aspect ratio is preserved.<br>
		 * @param fixedWidth The fixed width of the text block, with range (0, infinity), 0 to define the width indirect via 'fixedHeight' or 'fixedLineHeight'
		 * @param fixedHeight The fixed height of the text block, with range (0, infinity), 0 to define the height indirect via 'fixedWidth' or 'fixedLineHeight'
		 * @param fixedLineHeight The height of each line in the text block, with range (0, infinity), 0 to define the size via 'fixedWidth' and/or 'fixedHeight'
		 * @return True, if the provided combination of size parameter is valid
		 */
		virtual bool setSize(const Scalar fixedWidth, const Scalar fixedHeight, const Scalar fixedLineHeight);

		/**
		 * Sets the font to be used when rendering the text.
		 * @param fontFamily The family identifying the font, must be valid
		 * @param styleName The optional style name of the font, empty to use the default style
		 * @see availableFamilyNames(), availableStyleNames().
		 */
		virtual void setFont(const std::string& fontFamily, const std::string& styleName = std::string());

		/**
		 * Returns the material which is used to render the background of the text.
		 * By default, the text will be rendered with a black opaque background.<br>
		 * The background can be fully customized by e.g., setting a different diffuse color or the transparency of the background.
		 *
		 * To render the text without background, the material must be entirely transparent and a BlendAttribute must be attached to the corresponding AttributeSet:
		 * @code
		 * BlendAttributeRef blendAttribute = engine().factory().createBlendAttribute();
		 *
		 * blendAttribute->setSourceFunction(BlendAttribute::BlendingFunction::FUNCTION_ONE);
		 * blendAttribute->setDestinationFunction(BlendAttribute::BlendingFunction::FUNCTION_ONE_MINUS_SOURCE_ALPHA); // fully transparent with pre-multiplied alpha
		 *
		 * textAttributeSet->addAttribute(blendAttribute);
		 *
		 * textBackgroundMaterial->setTransparency(1.0f);
		 * @endcode
		 *
		 * To render the text with partially transparent background, the material must be translucent and a BlendAttribute must be attached to the corresponding AttributeSet:
		 * @code
		 * BlendAttributeRef blendAttribute = engine().factory().createBlendAttribute();
		 *
		 * blendAttribute->setSourceFunction(BlendAttribute::BlendingFunction::FUNCTION_SOURCE_ALPHA);
		 * blendAttribute->setDestinationFunction(BlendAttribute::BlendingFunction::FUNCTION_ONE_MINUS_SOURCE_ALPHA); // translucent with standard blending
		 *
		 * textAttributeSet->addAttribute(blendAttribute);
		 *
		 * textBackgroundMaterial->setTransparency(0.5f);
		 * @endcode
		 *
		 * To render the text with opaque background, the material must be opaque:
		 * @code
		 * textBackgroundMaterial->setTransparency(0.0f);
		 * @endcode
		 *
		 * @param material The background material to be set
		 */
		virtual void setBackgroundMaterial(const MaterialRef& material);

		/**
		 * Sets the alignment mode of the text.
		 * @param alignmentMode The alignment node to be set
		 */
		virtual void setAlignmentMode(const AlignmentMode alignmentMode);

		/**
		 * Sets the horizontal anchor of the text.
		 * @param horizontalAnchor The horizontal anchor to be set
		 */
		virtual void setHorizontalAnchor(const HorizontalAnchor horizontalAnchor);

		/**
		 * Sets the vertical anchor of the text.
		 * @param verticalAnchor The vertical anchor to be set
		 */
		virtual void setVerticalAnchor(const VerticalAnchor verticalAnchor);

		/**
		 * Sets an explicit lookup table which will be used to determine the text geometry (the vertices of the text).
		 * In case a lookup table is set, the horizontal and vertical anchor do not have any meaning anymore.<br>
		 * The width and height of the lookup table must be identical to the number of horizontal and vertical bins.
		 * @param lookupTable The lookup table to be set, an invalid lookup table to remove a previously set table
		 */
		virtual void setGeometryLookupTable(const LookupCorner2<Vector3>& lookupTable);

		/**
		 * Returns whether a specific font is currently available.
		 * @param familyName The name of the font family to check
		 * @param styleName Optional explicit font style to check
		 * @return True, if so
		 */
		virtual bool isFontAvailable(const std::string& familyName, const std::string& styleName = std::string()) const;

		/**
		 * Returns the default front currently available.
		 * @param styleName Optional resulting style name of the resulting font, nullptr if not of interest
		 * @return The family name of the currently available font, empty if no default is available
		 */
		virtual std::string availableDefaultFont(std::string* styleName = nullptr) const;

		/**
		 * Returns all available family names.
		 * @return The family names which are available
		 * @see setFont(), availableStyleNames().
		 */
		virtual std::vector<std::string> availableFamilyNames() const;

		/**
		 * Returns the style names of all available fonts with specified family name.
		 * @param familyName The family name of the fonts for which all available style names will be returned
		 * @return The style names of all available fonts
		 */
		virtual std::vector<std::string> availableStyleNames(const std::string& familyName) const;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new text object.
		 */
		Text();

		/**
		 * Destructs the text object.
		 */
		~Text() override;
};

}

}

#endif // META_OCEAN_RENDERING_TEXT_H
