/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESText.h"
#include "ocean/rendering/glescenegraph/GLESMaterial.h"
#include "ocean/rendering/glescenegraph/GLESMediaTexture2D.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

#include "ocean/cv/fonts/FontManager.h"

#include "ocean/media/Manager.h"
#include "ocean/media/PixelImage.h"

#include "ocean/rendering/MediaTexture2D.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESText::FontManager::FontData GLESText::FontManager::fontData(Rendering::Engine& engine, const std::string& fontFamily, const std::string& styleName)
{
	ocean_assert(!fontFamily.empty());

	const ScopedLock scopedLock(lock_);

	FontMap::const_iterator i = fontMap_.find(FontPair(fontFamily, styleName));

	if (i != fontMap_.cend())
	{
		return i->second;
	}

	CV::Fonts::SharedFont localFont = font(fontFamily, styleName);

	if (!localFont)
	{
		return FontData();
	}

	Frame fontFrame;
	CV::Fonts::Font::SharedCharacters fontCharacters;

	if (!localFont->characterFrame(fontFrame, &fontCharacters))
	{
		return FontData();
	}

	const unsigned int textureWidth = fontFrame.width();
	const unsigned int textureHeight = fontFrame.height();

	const Media::PixelImageRef pixelImage = Media::Manager::get().newMedium("FONT_PIXEL_IMAGE", Media::Medium::PIXEL_IMAGE, true /*useExclusive*/);
	ocean_assert(pixelImage);

	ocean_assert(engine.timestamp().isValid());
	fontFrame.setTimestamp(engine.timestamp());

	if (!pixelImage->start() || !pixelImage->setPixelImage(std::move(fontFrame)))
	{
		return FontData();
	}

	const Rendering::MediaTexture2DRef mediaTexture = engine.factory().createMediaTexture2D();
	ocean_assert(mediaTexture);

	mediaTexture->setMedium(pixelImage);

	mediaTexture->setMagnificationFilterMode(Rendering::Texture::MAG_MODE_LINEAR);
	mediaTexture->setMinificationFilterMode(Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR);
	mediaTexture->setUseMipmaps(true);

	mediaTexture->setWrapTypeS(Rendering::Texture::WRAP_CLAMP);
	mediaTexture->setWrapTypeT(Rendering::Texture::WRAP_CLAMP);

	const Rendering::TexturesRef textures = engine.factory().createTextures();
	ocean_assert(textures);

	textures->addTexture(mediaTexture);

	FontData fontData(localFont, textures, fontCharacters, textureWidth, textureHeight);

	fontMap_[FontPair(fontFamily, styleName)] = fontData;

	return fontData;
}

CV::Fonts::SharedFont GLESText::FontManager::font(const std::string& fontFamily, const std::string& styleName)
{
	return CV::Fonts::FontManager::get().font(fontFamily, 64u, styleName, 72u);
}

void GLESText::FontManager::release()
{
	const ScopedLock scopedLock(lock_);

	fontMap_.clear();
}

GLESText::GLESText() :
	GLESShape(),
	GLESTriangles(),
	Text()
{
	backgroundMaterial_ = engine().factory().createMaterial();
	backgroundMaterial_->setDiffuseColor(RGBAColor(0, 0, 0));

	fontFamily_ = availableDefaultFont(&styleName_);
}

GLESText::~GLESText()
{
	// nothing to do here
}

std::string GLESText::text() const
{
	const ScopedLock scopedLock(objectLock);

	return text_;
}

Vector2 GLESText::size() const
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(resultingSize_.x() >= 0 && resultingSize_.y() >= 0);

	if (needsRebuild_ || resultingSize_.isNull())
	{
		// we have not yet build the geometry, so we calculate the size in advance

		const CV::Fonts::SharedFont font = FontManager::get().font(fontFamily_, styleName_);

		if (font)
		{
			CV::PixelBoundingBoxesI linePixelBoundingBoxes;
			unsigned int textWidthPixels = 0u;
			unsigned int textHeightPixels = 0u;
			Scalar textWidth = 0;
			Scalar textHeight = 0;

			if (calculateTextSize(*font, linePixelBoundingBoxes, textWidthPixels, textHeightPixels, textWidth, textHeight))
			{
				return Vector2(textWidth, textHeight);
			}
		}

		return Vector2(0, 0);
	}

	return resultingSize_;
}

std::string GLESText::fontFamily(std::string* styleName) const
{
	const ScopedLock scopedLock(objectLock);

	if (styleName != nullptr)
	{
		*styleName = styleName_;
	}

	return fontFamily_;
}

MaterialRef GLESText::backgroundMaterial() const
{
	const ScopedLock scopedLock(objectLock);

	return backgroundMaterial_;
}

GLESText::AlignmentMode GLESText::alignmentMode() const
{
	const ScopedLock scopedLock(objectLock);

	return alignmentMode_;
}

GLESText::HorizontalAnchor GLESText::horizontalAnchor() const
{
	const ScopedLock scopedLock(objectLock);

	return horizontalAnchor_;
}

GLESText::VerticalAnchor GLESText::verticalAnchor() const
{
	const ScopedLock scopedLock(objectLock);

	return verticalAnchor_;
}

void GLESText::setText(const std::string& text)
{
	const ScopedLock scopedLock(objectLock);

	if (text_ != text)
	{
		text_ = text;
		needsRebuild_ = true;
	}
}

bool GLESText::setSize(const Scalar fixedWidth, const Scalar fixedHeight, const Scalar fixedLineHeight)
{
	if (fixedWidth > 0 && fixedLineHeight > 0)
	{
		ocean_assert(false && "Invalid size parameters");
		return false;
	}

	if (fixedHeight > 0 && fixedLineHeight > 0)
	{
		ocean_assert(false && "Invalid size parameters");
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	if (fixedWidthHeight_ != Vector2(fixedWidth, fixedHeight))
	{
		fixedWidthHeight_ = Vector2(fixedWidth, fixedHeight);
		needsRebuild_ = true;
	}

	if (fixedLineHeight_ != fixedLineHeight)
	{
		fixedLineHeight_ = fixedLineHeight;
		needsRebuild_ = true;
	}

	return true;
}

void GLESText::setFont(const std::string& fontFamily, const std::string& styleName)
{
	const ScopedLock scopedLock(objectLock);

	if (fontFamily_ != fontFamily || styleName_ != styleName)
	{
		fontFamily_ = fontFamily;
		styleName_ = styleName;
		needsRebuild_ = true;
	}
}

void GLESText::setBackgroundMaterial(const MaterialRef& material)
{
	const ScopedLock scopedLock(objectLock);

	const float oldTransparency = backgroundMaterial_ ? backgroundMaterial_->transparency() : 0.0f;
	const float newTransparency = material ? material->transparency() : 0.0f;

	if (oldTransparency != newTransparency)
	{
		needsRebuild_ = true;
	}

	backgroundMaterial_ = material;
}

void GLESText::setAlignmentMode(const AlignmentMode alignmentMode)
{
	const ScopedLock scopedLock(objectLock);

	if (alignmentMode_ != alignmentMode)
	{
		alignmentMode_ = alignmentMode;
		needsRebuild_ = true;
	}
}

void GLESText::setHorizontalAnchor(const HorizontalAnchor horizontalAnchor)
{
	const ScopedLock scopedLock(objectLock);

	if (horizontalAnchor_ != horizontalAnchor)
	{
		horizontalAnchor_ = horizontalAnchor;
		needsRebuild_ = true;
	}
}

void GLESText::setVerticalAnchor(const VerticalAnchor verticalAnchor)
{
	const ScopedLock scopedLock(objectLock);

	if (verticalAnchor_ != verticalAnchor)
	{
		verticalAnchor_ = verticalAnchor;
		needsRebuild_ = true;
	}
}

void GLESText::setGeometryLookupTable(const LookupCorner2<Vector3>& lookupTable)
{
	const ScopedLock scopedLock(objectLock);

	if (lookupTable_ != lookupTable)
	{
		lookupTable_ = lookupTable;
		needsRebuild_ = true;
	}
}

bool GLESText::isFontAvailable(const std::string& familyName, const std::string& styleName) const
{
	return CV::Fonts::FontManager::get().hasFont(familyName, styleName);
}

std::string GLESText::availableDefaultFont(std::string* styleName) const
{
	std::string internalStyleName;
	std::string familyName = CV::Fonts::FontManager::systemDefaultFontFamily(&internalStyleName);

	if (CV::Fonts::FontManager::get().hasFont(familyName, internalStyleName))
	{
		if (styleName != nullptr)
		{
			*styleName = internalStyleName;
		}

		return familyName;
	}

	return std::string();
}

std::vector<std::string> GLESText::availableFamilyNames() const
{
	return CV::Fonts::FontManager::get().familyNames();
}

std::vector<std::string> GLESText::availableStyleNames(const std::string& familyName) const
{
	return CV::Fonts::FontManager::get().styleNames(familyName);
}

void GLESText::render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights)
{
	const ScopedLock scopedLock(objectLock);

	if (needsRebuild_)
	{
		rebuildPrimitives();
		needsRebuild_ = false;
	}

	if (!textures_ || vboIndices_ == 0u)
	{
		return;
	}

	const SmartObjectRef<GLESVertexSet> glesVertexSet(vertexSet());
	if (glesVertexSet.isNull())
	{
		return;
	}

	const bool transparentText = backgroundMaterial_ && backgroundMaterial_->transparency() == 1.0f;

	const GLESAttribute::ProgramType additionalProgramType = transparentText ? GLESAttribute::PT_TEXT_TRANSPARENT_Y : GLESAttribute::PT_TEXT_OPAQUE_Y;

	GLESTextures& additionalAttribute = textures_.force<GLESTextures>();

	attributeSet.bindAttributes(framebuffer, projectionMatrix, camera_T_object, camera_T_world, normalMatrix, lights, additionalProgramType, &additionalAttribute);

	if (attributeSet.shaderProgram() && attributeSet.shaderProgram()->isCompiled())
	{
		if (!attributeSet.containsAttribute(TYPE_MATERIAL))
		{
			// we create an unlit text

			const GLint locationDiffuseColor = glGetUniformLocation(attributeSet.shaderProgram()->id(), "material.diffuseColor");
			ocean_assert(locationDiffuseColor != -1);

			setUniform(locationDiffuseColor, RGBAColor(0, 0, 0));

			const GLint locationEmissiveColor = glGetUniformLocation(attributeSet.shaderProgram()->id(), "material.emissiveColor");
			ocean_assert(locationEmissiveColor != -1);

			setUniform(locationEmissiveColor, RGBAColor(1, 1, 1));
		}

		if (backgroundMaterial_ && !transparentText)
		{
			backgroundMaterial_.force<GLESMaterial>().bindMaterial(framebuffer, *attributeSet.shaderProgram(), "backgroundMaterial");
		}

		glesVertexSet->bindVertexSet(attributeSet.shaderProgram()->id());

		drawTriangles();
	}

	attributeSet.unbindAttributes();
}

GLESText::ObjectType GLESText::type() const
{
	return TYPE_TEXT;
}

std::string GLESText::descriptiveInformation() const
{
	const ScopedLock scopedLock(objectLock);

	std::string result = Object::descriptiveInformation();

	if (!text_.empty())
	{
		result += ", with string '" + text_ + "'";
	}

	return result;
}

void GLESText::release()
{
	FontManager::get().release();
}

void GLESText::rebuildPrimitives()
{
	if (fontFamily_.empty())
	{
		fontFamily_ = availableDefaultFont(&styleName_);
	}

	FontManager::FontData fontData = FontManager::get().fontData(engine(), fontFamily_, styleName_);

	if (!fontData.isValid() || text_.empty())
	{
		setFaces(TriangleFaces());
		shapeVertexSet->set(Vectors3(), Vectors3(), Vectors2(), RGBAColors());

		return;
	}

	textures_ = fontData.textures_; // re-setting the texture in any case

	resultingSize_ = Vector2(0, 0);

	if (!fontData.font_)
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	ocean_assert(fontData.characters_);

	const unsigned int firstCharacter = fontData.characters_->front().characterIndex();
	const unsigned int endCharacter = firstCharacter + (unsigned int)(fontData.characters_->size());

	if (firstCharacter > 32u)
	{
		ocean_assert(false && "The space character must be included!");
		return;
	}

	// first we determine the size/extent of the text block

	CV::PixelBoundingBoxesI linePixelBoundingBoxes; // bounding boxes of each text line in the domain of the font's pixel image

	unsigned int textWidthPixels = 0u; // width and height in the domain of the font's pixel image
	unsigned int textHeightPixels = 0u;

	Scalar textWidth = 0; // width and height in space
	Scalar textHeight = 0;

	if (!calculateTextSize(*fontData.font_, linePixelBoundingBoxes, textWidthPixels, textHeightPixels, textWidth, textHeight))
	{
		setFaces(TriangleFaces());
		shapeVertexSet->set(Vectors3(), Vectors3(), Vectors2(), RGBAColors());

		return;
	}

	// we determine the horizontal and vertical offsets necessary for the anchors

	Scalar vertexAnchorOffsetX = 0;
	switch (horizontalAnchor_)
	{
		case HA_LEFT:
			break;

		case HA_CENTER:
			vertexAnchorOffsetX = -textWidth * Scalar(0.5);
			break;

		case HA_RIGHT:
			vertexAnchorOffsetX = -textWidth;
			break;
	};

	Scalar vertexAnchorOffsetY = 0;
	switch (verticalAnchor_)
	{
		case VA_TOP:
			break;

		case VA_MIDDLE:
			vertexAnchorOffsetY = textHeight * Scalar(0.5);
			break;

		case VA_BOTTOM:
			vertexAnchorOffsetY = textHeight;
			break;
	};

	const bool drawBackground = backgroundMaterial_ ? backgroundMaterial_->transparency() < 1.0f - NumericF::weakEps() : false; // drawing the background in case the valid background material is not fully transparent

	Vectors3 vertices;
	vertices.reserve(drawBackground ? text_.size() * 4 : text_.size() * 16);

	Vectors2 textureCoordinates;
	textureCoordinates.reserve(vertices.capacity());

	TriangleFaces triangleFaces;
	triangleFaces.reserve(vertices.capacity() / 2);

	const unsigned int firstLineWidthPixels = linePixelBoundingBoxes.front().isValid() ? linePixelBoundingBoxes.front().width() : 0u;

	ocean_assert(!linePixelBoundingBoxes.empty() && textWidthPixels >= firstLineWidthPixels);

	int xPixel = 0;
	switch (alignmentMode_)
	{
		case AM_LEFT:
			break;

		case AM_CENTER:
			xPixel = (textWidthPixels - firstLineWidthPixels) / 2u;
			break;

		case AM_RIGHT:
			xPixel = textWidthPixels - firstLineWidthPixels;
			break;
	}

	const unsigned int textLineHeightPixels = fontData.font_->height();

	int yPixel = 0;

	const Scalar invTextWidth = textWidth / Scalar(textWidthPixels);
	const Scalar invTextHeight = textHeight / Scalar(textHeightPixels);

	const Scalar invTextureWidth = Scalar(1) / Scalar(fontData.textureWidth_);
	const Scalar invTextureHeight = Scalar(1) / Scalar(fontData.textureHeight_);

	ocean_assert(firstCharacter >= 32u);
	const CV::Fonts::Font::Character& spaceCharacter = (*fontData.characters_)[32u - firstCharacter];

	const Scalar textureSpaceX = (Scalar(spaceCharacter.framePositionX()) + Scalar(spaceCharacter.advanceX()) * Scalar(0.5)) * invTextureWidth; // center point within the space glyph
	const Scalar textureSpaceY = Scalar(1) - Scalar(textLineHeightPixels) * Scalar(0.5) * invTextureHeight;
	const Vectors2 textureCoordinatesSpace(4, Vector2(textureSpaceX, textureSpaceY));

	size_t lineIndex = 0;
	Scalar lastVertexRight = vertexAnchorOffsetX;

	for (size_t n = 0; n < text_.size(); ++n)
	{
		const unsigned int value = text_[n];

		if (value == (unsigned int)('\n'))
		{
			if (drawBackground)
			{
				const Scalar vertexBlockTop = -Scalar(yPixel) * invTextHeight + vertexAnchorOffsetY;
				const Scalar vertexBlockBottom = -Scalar(yPixel + textLineHeightPixels) * invTextHeight + vertexAnchorOffsetY;
				const Scalar vertexBlockRight = Scalar(textWidthPixels) * invTextWidth + vertexAnchorOffsetX;

				// right block

				if (lastVertexRight != vertexBlockRight && vertexBlockTop != vertexBlockBottom)
				{
					const unsigned int currentVertexIndex = (unsigned int)(vertices.size());

					vertices.emplace_back(lastVertexRight, vertexBlockTop, Scalar(0));
					vertices.emplace_back(lastVertexRight, vertexBlockBottom, Scalar(0));
					vertices.emplace_back(vertexBlockRight, vertexBlockBottom, Scalar(0));
					vertices.emplace_back(vertexBlockRight, vertexBlockTop, Scalar(0));

					textureCoordinates.insert(textureCoordinates.cend(), textureCoordinatesSpace.cbegin(), textureCoordinatesSpace.cend());

					triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 1u, currentVertexIndex + 2u);
					triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 2u, currentVertexIndex + 3u);
				}
			}

			++lineIndex;
			ocean_assert(lineIndex < linePixelBoundingBoxes.size());

			const unsigned int lineWidthPixels = linePixelBoundingBoxes[lineIndex].isValid() ? linePixelBoundingBoxes[lineIndex].width() : 0u;

			ocean_assert(textWidthPixels >= lineWidthPixels);

			xPixel = 0;
			switch (alignmentMode_)
			{
				case AM_LEFT:
					break;

				case AM_CENTER:
					xPixel = (textWidthPixels - lineWidthPixels) / 2u;
					break;

				case AM_RIGHT:
					xPixel = textWidthPixels - lineWidthPixels;
					break;
			}

			yPixel += int(textLineHeightPixels);

			lastVertexRight = vertexAnchorOffsetX;

			continue;
		}

		if (value < firstCharacter || value >= endCharacter)
		{
			// we simply skip the character
			continue;
		}

		const CV::Fonts::Font::Character& character = (*fontData.characters_)[value - firstCharacter];

		const int xPixelStart = std::max(xPixel + character.bearingX(), 0);
		const int xPixelEnd = std::min(xPixel + character.bearingX() + int(character.width()), int(textWidthPixels));

		const int yPixelStart = std::max(yPixel + int(character.linePositionY()), 0);
		const int yPixelEnd = std::min(yPixel + int(character.linePositionY()) + int(character.height()), int(textHeightPixels));

		if (xPixelStart < xPixelEnd && yPixelStart < yPixelEnd)
		{
			const unsigned int xStartTexture = character.framePositionX() + (unsigned int)(xPixelStart - (xPixel + character.bearingX()));
			const unsigned int yStartTexture = character.framePositionY() + (unsigned int)(yPixelStart - (yPixel + int(character.linePositionY())));

			const unsigned int widthPixel = (unsigned int)(xPixelEnd - xPixelStart);
			const unsigned int heightPixel = (unsigned int)(yPixelEnd - yPixelStart);

			ocean_assert(widthPixel <= character.width());
			ocean_assert(heightPixel <= character.height());

			unsigned int currentVertexIndex = (unsigned int)(vertices.size());

			const Scalar vertexLeft = Scalar(xPixelStart) * invTextWidth + vertexAnchorOffsetX;
			const Scalar vertexRight = Scalar(xPixelEnd) * invTextWidth + vertexAnchorOffsetX;
			const Scalar vertexTop = -Scalar(yPixelStart) * invTextHeight + vertexAnchorOffsetY;
			const Scalar vertexBottom = -Scalar(yPixelEnd) * invTextHeight + vertexAnchorOffsetY;

			ocean_assert(vertexLeft != vertexRight && vertexTop != vertexBottom);

			vertices.emplace_back(vertexLeft, vertexTop, Scalar(0));
			vertices.emplace_back(vertexLeft, vertexBottom, Scalar(0));
			vertices.emplace_back(vertexRight, vertexBottom, Scalar(0));
			vertices.emplace_back(vertexRight, vertexTop, Scalar(0));

			constexpr Scalar extraTextureBorder = Scalar(0.15); // increasing texture area by 0.15 pixels to avoid that character ends directly at the end of the texture

			const Scalar textureLeft = (Scalar(xStartTexture) - extraTextureBorder) * invTextureWidth;
			const Scalar textureRight = (Scalar(xStartTexture + widthPixel) + extraTextureBorder) * invTextureWidth;
			const Scalar textureTop = Scalar(1) - (Scalar(yStartTexture) - extraTextureBorder) * invTextureHeight;
			const Scalar textureBottom = Scalar(1) - (Scalar(yStartTexture + heightPixel) + extraTextureBorder) * invTextureHeight;

			ocean_assert(Numeric::isInsideRange(0, textureLeft, 1) && Numeric::isInsideRange(0, textureRight, 1));
			ocean_assert(Numeric::isInsideRange(0, textureTop, 1) && Numeric::isInsideRange(0, textureBottom, 1));

			textureCoordinates.emplace_back(textureLeft, textureTop);
			textureCoordinates.emplace_back(textureLeft, textureBottom);
			textureCoordinates.emplace_back(textureRight, textureBottom);
			textureCoordinates.emplace_back(textureRight, textureTop);

			triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 1u, currentVertexIndex + 2u);
			triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 2u, currentVertexIndex + 3u);
			currentVertexIndex += 4u;

			if (drawBackground)
			{
				const Scalar vertexBlockTop = -Scalar(yPixel) * invTextHeight + vertexAnchorOffsetY;
				const Scalar vertexBlockBottom = -Scalar(yPixel + textLineHeightPixels) * invTextHeight + vertexAnchorOffsetY;

				if (character.linePositionY() > 0u)
				{
					// top block

					if (vertexLeft != vertexRight && vertexTop != vertexBlockTop)
					{
						vertices.emplace_back(vertexLeft, vertexBlockTop, Scalar(0));
						vertices.emplace_back(vertexLeft, vertexTop, Scalar(0));
						vertices.emplace_back(vertexRight, vertexTop, Scalar(0));
						vertices.emplace_back(vertexRight, vertexBlockTop, Scalar(0));

						textureCoordinates.insert(textureCoordinates.cend(), textureCoordinatesSpace.cbegin(), textureCoordinatesSpace.cend());

						triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 1u, currentVertexIndex + 2u);
						triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 2u, currentVertexIndex + 3u);
						currentVertexIndex += 4u;
					}
				}

				ocean_assert(character.linePositionY() + character.height() <= textLineHeightPixels);
				if (character.linePositionY() + character.height() != textLineHeightPixels)
				{
					// bottom block

					if (vertexLeft != vertexRight && vertexBottom != vertexBlockBottom)
					{
						vertices.emplace_back(vertexLeft, vertexBottom, Scalar(0));
						vertices.emplace_back(vertexLeft, vertexBlockBottom, Scalar(0));
						vertices.emplace_back(vertexRight, vertexBlockBottom, Scalar(0));
						vertices.emplace_back(vertexRight, vertexBottom, Scalar(0));

						textureCoordinates.insert(textureCoordinates.cend(), textureCoordinatesSpace.cbegin(), textureCoordinatesSpace.cend());

						triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 1u, currentVertexIndex + 2u);
						triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 2u, currentVertexIndex + 3u);
						currentVertexIndex += 4u;
					}
				}

				// always a left block, even if it's the first character (due to offset within the glyph, or due to alignment)

				if (vertexLeft != lastVertexRight && vertexBlockTop != vertexBlockBottom)
				{
					vertices.emplace_back(lastVertexRight, vertexBlockTop, Scalar(0));
					vertices.emplace_back(lastVertexRight, vertexBlockBottom, Scalar(0));
					vertices.emplace_back(vertexLeft, vertexBlockBottom, Scalar(0));
					vertices.emplace_back(vertexLeft, vertexBlockTop, Scalar(0));

					textureCoordinates.insert(textureCoordinates.cend(), textureCoordinatesSpace.cbegin(), textureCoordinatesSpace.cend());

					triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 1u, currentVertexIndex + 2u);
					triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 2u, currentVertexIndex + 3u);
					currentVertexIndex += 4u;
				}
			}

			lastVertexRight = vertexRight;
		}
		else
		{
			unsigned int currentVertexIndex = (unsigned int)(vertices.size());

			const Scalar vertexRight = Scalar(xPixelStart + int(character.advanceX())) * invTextWidth + vertexAnchorOffsetX;
			const Scalar vertexTop = -Scalar(yPixel) * invTextHeight + vertexAnchorOffsetY;
			const Scalar vertexBottom = -Scalar(yPixel + fontData.font_->height()) * invTextHeight + vertexAnchorOffsetY;

			if (lastVertexRight != vertexRight && vertexTop != vertexBottom)
			{
				vertices.emplace_back(lastVertexRight, vertexTop, Scalar(0));
				vertices.emplace_back(lastVertexRight, vertexBottom, Scalar(0));
				vertices.emplace_back(vertexRight, vertexBottom, Scalar(0));
				vertices.emplace_back(vertexRight, vertexTop, Scalar(0));

				textureCoordinates.insert(textureCoordinates.cend(), textureCoordinatesSpace.cbegin(), textureCoordinatesSpace.cend());

				triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 1u, currentVertexIndex + 2u);
				triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 2u, currentVertexIndex + 3u);
				currentVertexIndex += 4u;
			}

			lastVertexRight = vertexRight;
		}

		xPixel += int(character.advanceX());
	}

	if (drawBackground && (lastVertexRight != vertexAnchorOffsetX || text_.back() == '\n'))
	{
		const Scalar vertexBlockTop = -Scalar(yPixel) * invTextHeight + vertexAnchorOffsetY;
		const Scalar vertexBlockBottom = -Scalar(yPixel + textLineHeightPixels) * invTextHeight + vertexAnchorOffsetY;
		const Scalar vertexBlockRight = Scalar(textWidthPixels) * invTextWidth + vertexAnchorOffsetX;

		if (lastVertexRight != vertexBlockRight && vertexBlockTop != vertexBlockBottom)
		{
			// right block of last line

			const unsigned int currentVertexIndex = (unsigned int)(vertices.size());

			vertices.emplace_back(lastVertexRight, vertexBlockTop, Scalar(0));
			vertices.emplace_back(lastVertexRight, vertexBlockBottom, Scalar(0));
			vertices.emplace_back(vertexBlockRight, vertexBlockBottom, Scalar(0));
			vertices.emplace_back(vertexBlockRight, vertexBlockTop, Scalar(0));

			textureCoordinates.insert(textureCoordinates.cend(), textureCoordinatesSpace.cbegin(), textureCoordinatesSpace.cend());

			triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 1u, currentVertexIndex + 2u);
			triangleFaces.emplace_back(currentVertexIndex + 0u, currentVertexIndex + 2u, currentVertexIndex + 3u);
		}
	}

	Vectors3 normals;

	if (lookupTable_)
	{
		// for now we have a plain text, we recalculate the vertices based on the lookup table

		const Scalar invX = Scalar(lookupTable_.binsX()) / textWidth;
		const Scalar invY = Scalar(lookupTable_.binsY()) / textHeight;

		for (size_t n = 0; n < vertices.size(); ++n)
		{
			Vector3& vertex = vertices[n];

			ocean_assert(Numeric::isEqualEps(vertex.z()));

			const Scalar lookupX = minmax(Scalar(0), (vertex.x() + vertexAnchorOffsetX) * invX, Scalar(lookupTable_.sizeX()));
			const Scalar lookupY = minmax(Scalar(0), (-vertex.y() + vertexAnchorOffsetY) * invY, Scalar(lookupTable_.sizeY()));

			vertex = lookupTable_.bilinearValue(lookupX, lookupY);

			if (n % 4 == 3)
			{
				ocean_assert(vertices[n] != vertices[n - 1] && vertices[n] != vertices[n - 2] && vertices[n] != vertices[n - 3]);
				ocean_assert(vertices[n - 1] != vertices[n - 2] && vertices[n - 1] != vertices[n - 3]);
				ocean_assert(vertices[n - 2] != vertices[n - 3]);
			}
		}

		normals = TriangleFace::calculatePerFaceNormals(triangleFaces, vertices, true /*counterClockWise*/);

		normals = TriangleFace::calculateSmoothedPerVertexNormals(triangleFaces, vertices, normals);
	}
	else
	{
		normals = Vectors3(vertices.size(), Vector3(0, 0, 1)); // all normals point into the same direction
	}

	ocean_assert(vertices.size() == textureCoordinates.size());
	ocean_assert(vertices.size() == normals.size());

	ocean_assert(vertices.size() == triangleFaces.size() * 2);

	shapeVertexSet->setVertices(vertices);
	shapeVertexSet->setNormals(normals);
	shapeVertexSet->setTextureCoordinates(textureCoordinates, 0u);

	setFaces(triangleFaces);
	setVertexSet(shapeVertexSet);

	resultingSize_ = Vector2(textWidth, textHeight);

	const Scalar xTextCenter = textWidth * Scalar(0.5) + vertexAnchorOffsetX;
	const Scalar yTextCenter = -textHeight * Scalar(0.5) + vertexAnchorOffsetY;

	boundingBox_ = BoundingBox(Box3(Vector3(xTextCenter, yTextCenter, 0), textWidth, textHeight, 0));
}

void GLESText::updateBoundingBox()
{
	// nothing to do here, as the bounding box will been updated in rebuildPrimitives() already.
}

bool GLESText::calculateTextSize(const CV::Fonts::Font& font, CV::PixelBoundingBoxesI& linePixelBoundingBoxes, unsigned int& textWidthPixels, unsigned int& textHeightPixels, Scalar& textWidth, Scalar& textHeight) const
{
	linePixelBoundingBoxes.clear();

	if (text_.empty())
	{
		textWidthPixels = 0u;
		textHeightPixels = 0u;
		textWidth = 0;
		textHeight = 0;

		return true;
	}

	if (!font.textExtent(text_, linePixelBoundingBoxes) || linePixelBoundingBoxes.empty())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	int lineBoundingBoxLeftMin = NumericT<int>::maxValue();
	int lineBoundingBoxRightMax = NumericT<int>::minValue();

	textWidthPixels = 0u;
	textHeightPixels = 0u;
	for (const CV::PixelBoundingBoxI& linePixelBoundingBox : linePixelBoundingBoxes)
	{
		if (linePixelBoundingBox.isValid())
		{
			lineBoundingBoxLeftMin = std::min(linePixelBoundingBox.left(), lineBoundingBoxLeftMin);
			lineBoundingBoxRightMax = std::max(linePixelBoundingBox.right(), lineBoundingBoxRightMax);
			textHeightPixels += linePixelBoundingBox.height();
		}
		else
		{
			// we have an empty row
			textHeightPixels += font.height();
		}
	}

	ocean_assert(lineBoundingBoxLeftMin != NumericT<int>::maxValue() && lineBoundingBoxRightMax != NumericT<int>::minValue());
	textWidthPixels = (unsigned int)(lineBoundingBoxRightMax - lineBoundingBoxLeftMin + 1);

	if (textWidthPixels == 0u || textHeightPixels == 0u)
	{
		return false;
	}

	// now we determine the size of the text block in space

	textWidth = 0;
	textHeight = 0;

	if (fixedWidthHeight_.x() > Numeric::eps())
	{
		ocean_assert(fixedLineHeight_ <= 0);

		textWidth = fixedWidthHeight_.x();

		if (fixedWidthHeight_.y() > Numeric::eps())
		{
			textHeight = fixedWidthHeight_.y();
		}
		else
		{
			textHeight = textWidth * Scalar(textHeightPixels) / Scalar(textWidthPixels);
		}
	}
	else if (fixedWidthHeight_.y() > Numeric::eps())
	{
		ocean_assert(fixedLineHeight_ <= 0);

		textHeight = fixedWidthHeight_.y();

		textWidth = textHeight * Scalar(textWidthPixels) / Scalar(textHeightPixels);
	}
	else if (fixedLineHeight_ > Numeric::eps())
	{
		textHeight = fixedLineHeight_ * Scalar(linePixelBoundingBoxes.size());

		textWidth = textHeight * Scalar(textWidthPixels) / Scalar(textHeightPixels);
	}
	else
	{
		return false;
	}

	return textWidth > 0 && textHeight > 0;
}

}

}

}
