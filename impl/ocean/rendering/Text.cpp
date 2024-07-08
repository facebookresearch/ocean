/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Text.h"

namespace Ocean
{

namespace Rendering
{

Text::Text() :
	Shape()
{
	// nothing to do here
}

Text::~Text()
{
	// nothing to do here
}

std::string Text::text() const
{
	throw NotSupportedException("Text::text() is not supported.");
}

Vector2 Text::size() const
{
	throw NotSupportedException("Text::size() is not supported.");
}

std::string Text::fontFamily(std::string* /*styleName*/) const
{
	throw NotSupportedException("Text::fontFamily() is not supported.");
}

MaterialRef Text::backgroundMaterial() const
{
	throw NotSupportedException("Text::backgroundMaterial() is not supported.");
}

Text::AlignmentMode Text::alignmentMode() const
{
	throw NotSupportedException("Text::alignmentMode() is not supported.");
}

Text::HorizontalAnchor Text::horizontalAnchor() const
{
	throw NotSupportedException("Text::horizontalAnchor() is not supported.");
}

Text::VerticalAnchor Text::verticalAnchor() const
{
	throw NotSupportedException("Text::verticalAnchor() is not supported.");
}

void Text::setText(const std::string& /*text*/)
{
	throw NotSupportedException("Text::setText() is not supported.");
}

bool Text::setSize(const Scalar /*fixedWidth*/, const Scalar /*fixedHeight*/, const Scalar /*fixedLineHeight*/)
{
	throw NotSupportedException("Text::setSize() is not supported.");
}

void Text::setFont(const std::string& /*fontFamily*/, const std::string& /*styleName*/)
{
	throw NotSupportedException("Text::setFont() is not supported.");
}

void Text::setBackgroundMaterial(const MaterialRef& /*material*/)
{
	throw NotSupportedException("Text::setBackgroundMaterial() is not supported.");
}

void Text::setAlignmentMode(const AlignmentMode /*alignmentMode*/)
{
	throw NotSupportedException("Text::setAlignmentMode() is not supported.");
}

void Text::setHorizontalAnchor(const HorizontalAnchor /*horizontalAnchor*/)
{
	throw NotSupportedException("Text::setHorizontalAnchor() is not supported.");
}

void Text::setVerticalAnchor(const VerticalAnchor /*verticalAnchor*/)
{
	throw NotSupportedException("Text::setVerticalAnchor() is not supported.");
}

void Text::setGeometryLookupTable(const LookupCorner2<Vector3>& /*lookupTable*/)
{
	throw NotSupportedException("Text::setGeometryLookupTable() is not supported.");
}

bool Text::isFontAvailable(const std::string& /*familyName*/, const std::string& /*styleName*/) const
{
	throw NotSupportedException("Text::isFontAvailable() is not supported.");
}

std::string Text::availableDefaultFont(std::string* /*styleName*/) const
{
	throw NotSupportedException("Text::availableDefaultFont() is not supported.");
}

std::vector<std::string> Text::availableFamilyNames() const
{
	throw NotSupportedException("Text::availableFamilyNames() is not supported.");
}

std::vector<std::string> Text::availableStyleNames(const std::string& /*familyName*/) const
{
	throw NotSupportedException("Text::availableStyleNames() is not supported.");
}

Text::ObjectType Text::type() const
{
	return TYPE_TEXT;
}

}

}
