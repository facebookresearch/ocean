/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_FONT_STYLE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_FONT_STYLE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DFontStyleNode.h"

#include "ocean/rendering/Text.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d font style.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT FontStyle : virtual public X3DFontStyleNode
{
	friend class Text;

	public:

		/**
		 * Creates an abstract x3d sensor node.
		 * @param environment Node environment
		 */
		explicit FontStyle(const SDXEnvironment* environment);

	protected:

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

		/**
		 * Applies all properties to a given text object.
		 * @param text The text to which all properties will be applied
		 */
		void applyToText(Rendering::Text& text) const;

		/**
		 * Applies the font property to a given text object.
		 * @param text The text to which all properties will be applied
		 * @param fontFamilyNames Several names of font families, the first existing will be applied, empty to use the default font
		 * @param fontStyleName The optional name of the font style, empty to use the default style
		 * @return True, if succeeded
		 */
		static bool applyFontToText(Rendering::Text& text, const std::vector<std::string>& fontFamilyNames = std::vector<std::string>(), const std::string& fontStyleName = std::string());

	protected:

		/// Family field.
		MultiString family_;

		/// horizontal field.
		SingleBool horizontal_;

		/// Justify field.
		MultiString justify_;

		/// Language field.
		SingleString language_;

		/// LeftToRight field.
		SingleBool leftToRight_;

		/// Size field.
		SingleFloat size_;

		/// Spacing field.
		SingleFloat spacing_;

		/// Style field.
		SingleString style_;

		/// TopToBottom field.
		SingleBool topToBottom_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_FONT_STYLE_H
