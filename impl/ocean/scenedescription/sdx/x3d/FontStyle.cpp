/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/FontStyle.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

FontStyle::FontStyle(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DFontStyleNode(environment),
	family_("SERIF"),
	horizontal_(true),
	justify_("BEGIN"),
	leftToRight_(true),
	size_(1),
	spacing_(1),
	style_("PLAIN"),
	topToBottom_(true)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

FontStyle::NodeSpecification FontStyle::specifyNode()
{
	NodeSpecification specification("FontStyle");

	registerField(specification, "family", family_, ACCESS_NONE);
	registerField(specification, "horizontal", horizontal_, ACCESS_NONE);
	registerField(specification, "justify", justify_, ACCESS_NONE);
	registerField(specification, "language", language_, ACCESS_NONE);
	registerField(specification, "leftToRight", leftToRight_, ACCESS_NONE);
	registerField(specification, "size", size_, ACCESS_NONE);
	registerField(specification, "spacing", spacing_, ACCESS_NONE);
	registerField(specification, "style", style_, ACCESS_NONE);
	registerField(specification, "topToBottom", topToBottom_, ACCESS_NONE);

	X3DNode::registerFields(specification);

	return specification;
}

void FontStyle::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DNode::onInitialize(scene, timestamp);

	try
	{
		const NodeRefs parents(parentNodes());

		for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
		{
			const SDXNodeRef parent(*i);
			ocean_assert(parent);

			const Rendering::TextRef text(parent->renderingObject());

			if (text)
			{
				applyToText(*text);
			}
		}
	}
	catch (const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

size_t FontStyle::objectAddress() const
{
	return size_t(this);
}

void FontStyle::applyToText(Rendering::Text& text) const
{
	if (!applyFontToText(text, family_.values(), style_.value()))
	{
		Log::warning() << "The matching font exists for FontStyle node";
	}

	if (!horizontal_.value())
	{
		Log::warning() << "FontStyle does not support vertical text";
	}

	Rendering::Text::AlignmentMode alignmentMode = Rendering::Text::AM_LEFT;
	Rendering::Text::HorizontalAnchor horizontalAnchor = Rendering::Text::HA_LEFT;
	Rendering::Text::VerticalAnchor verticalAnchor = Rendering::Text::VA_TOP;

	if (justify_.values().size() >= 1)
	{
		const std::string& majorValue = justify_.values()[0];

		if (majorValue.empty() || majorValue == "BEGIN" || majorValue == "FIRST")
		{
			// nothing to do
		}
		else if (majorValue == "MIDDLE")
		{
			alignmentMode = Rendering::Text::AM_CENTER;
			horizontalAnchor = Rendering::Text::HA_CENTER;
		}
		else if (majorValue == "END")
		{
			alignmentMode = Rendering::Text::AM_RIGHT;
			horizontalAnchor = Rendering::Text::HA_RIGHT;
		}
		else
		{
			Log::warning() << "FontStyle does not support \"" << majorValue << "\" as major justify value";
		}

		if (justify_.values().size() >= 2)
		{
			const std::string& minorValue = justify_.values()[1];

			if (minorValue.empty() || minorValue == "BEGIN")
			{
				// nothing to do
			}
			else if (minorValue == "MIDDLE")
			{
				verticalAnchor = Rendering::Text::VA_MIDDLE;
			}
			else if (minorValue == "END")
			{
				verticalAnchor = Rendering::Text::VA_BOTTOM;
			}
			else
			{
				Log::warning() << "FontStyle does not support \"" << minorValue << "\" as minor justify value";
			}
		}
	}

	text.setAlignmentMode(alignmentMode);
	text.setHorizontalAnchor(horizontalAnchor);
	text.setVerticalAnchor(verticalAnchor);

	if (!leftToRight_.value())
	{
		Log::warning() << "FontStyle does not support right to left text";
	}

	if (!topToBottom_.value())
	{
		Log::warning() << "FontStyle does not support bottom to top text";
	}

	const Scalar sizeValue = size_.value();

	if (sizeValue > Numeric::eps())
	{
		text.setSize(0, 0, sizeValue);
	}
	else
	{
		Log::warning() << "FontStyle size needs to be greather than zero";
	}

	if (Numeric::isNotEqual(spacing_.value(), 1))
	{
		Log::warning() << "FontStyle does not support spacing other than 1.0";
	}
}

bool FontStyle::applyFontToText(Rendering::Text& text, const std::vector<std::string>& fontFamilyNames, const std::string& fontStyleName)
{
	if (fontFamilyNames.empty())
	{
		const std::string familyName = text.availableDefaultFont();

		if (!familyName.empty())
		{
			std::string fontStyle = fontStyleName != "PLAIN" ? fontStyleName : std::string();

			if (text.isFontAvailable(familyName, fontStyle))
			{
				text.setFont(familyName, fontStyle);
				return true;
			}
		}
	}

	for (const std::string& familyValue : fontFamilyNames)
	{
		std::string familyName = familyValue;

		if (familyName.empty() || familyName == "SERIF")
		{
			familyName = text.availableDefaultFont();
		}

		if (!familyName.empty())
		{
			const std::string styleName = fontStyleName != "PLAIN" ? fontStyleName : std::string();

			if (text.isFontAvailable(familyName, styleName))
			{
				text.setFont(familyName, styleName);
				return true;
			}
		}
	}

	return false;
}

}

}

}

}
