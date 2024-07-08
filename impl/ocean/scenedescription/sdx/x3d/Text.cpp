/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Text.h"
#include "ocean/scenedescription/sdx/x3d/FontStyle.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Text::Text(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometryNode(environment),
	maxExtent_(0),
	solid_(false)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createText();
}

Text::NodeSpecification Text::specifyNode()
{
	NodeSpecification specification("Text");

	registerField(specification, "fontStyle", fontStyle_, ACCESS_GET_SET);
	registerField(specification, "length", length_, ACCESS_GET_SET);
	registerField(specification, "maxExtent", maxExtent_, ACCESS_GET_SET);
	registerField(specification, "string", string_, ACCESS_GET_SET);
	registerField(specification, "lineBounds", lineBounds_, ACCESS_GET);
	registerField(specification, "origin", origin_, ACCESS_GET);
	registerField(specification, "textBounds", textBounds_, ACCESS_GET);
	registerField(specification, "solid", solid_, ACCESS_NONE);

	X3DGeometryNode::registerFields(specification);

	return specification;
}

void Text::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGeometryNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::TextRef text(renderingObject_);

		if (text)
		{
			applyString(*text);

			const SDXNodeRef fontStyleNode(fontStyle_.value());
			if (fontStyleNode)
			{
				fontStyleNode->initialize(scene, timestamp);

				fontStyleNode.force<FontStyle>().applyToText(*text);
			}
			else
			{
				if (!FontStyle::applyFontToText(*text))
				{
					Log::warning() << "The matching font exists for Text node";
				}

				text->setSize(0, 0, 1);
			}

			if (!length_.values().empty())
			{
				Log::warning() << "Text does not support length values";
			}

			if (Numeric::isNotEqualEps(maxExtent_.value()))
			{
				Log::warning() << "Text does not support maxExtent values";
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void Text::onFieldChanged(const std::string& fieldName)
{
	try
	{
		const Rendering::TextRef text(renderingObject_);

		if (text)
		{
			if (fieldName == "string")
			{
				applyString(*text);
			}
			else if (fieldName == "fontStyle")
			{
				const SDXNodeRef fontStyleNode(fontStyle_.value());
				if (fontStyleNode)
				{
					fontStyleNode.force<FontStyle>().applyToText(*text);
				}
			}
			else if (fieldName == "length")
			{
				if (!length_.values().empty())
				{
					Log::warning() << "Text does not support length values";
				}
			}
			else if (fieldName == "maxExtent")
			{
				if (Numeric::isNotEqualEps(maxExtent_.value()))
				{
					Log::warning() << "Text does not support maxExtent values";
				}
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DNode::onFieldChanged(fieldName);
}

size_t Text::objectAddress() const
{
	return size_t(this);
}

void Text::applyString(Rendering::Text& text) const
{
	std::string textLines;

	for (const std::string& line : string_.values())
	{
		if (!textLines.empty())
		{
			textLines += '\n';
		}

		textLines += line;
	}

	if (!textLines.empty())
	{
		text.setText(textLines);
	}
}

}

}

}

}
