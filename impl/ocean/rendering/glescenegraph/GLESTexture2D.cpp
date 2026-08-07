/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTexture2D.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESTexture2D::GLESTexture2D() :
	GLESTexture(),
	Texture2D(),
	DynamicObject()
{
	registerDynamicUpdateObject();
}

GLESTexture2D::~GLESTexture2D()
{
	if (primaryTextureId_ != 0u)
	{
		glDeleteTextures(1, &primaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());
		primaryTextureId_ = 0u;
	}

	if (secondaryTextureId_ != 0u)
	{
		glDeleteTextures(1, &secondaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());
		secondaryTextureId_ = 0u;
	}

	unregisterDynamicUpdateObject();
}

GLESTexture2D::WrapType GLESTexture2D::wrapTypeS() const
{
	const ScopedLock scopedLock(objectLock);

	return wrapTypeS_;
}

GLESTexture2D::WrapType GLESTexture2D::wrapTypeT() const
{
	const ScopedLock scopedLock(objectLock);

	return wrapTypeT_;
}

bool GLESTexture2D::setWrapTypeS(const WrapType type)
{
	const ScopedLock scopedLock(objectLock);

	wrapTypeS_ = type;
	return true;
}

bool GLESTexture2D::setWrapTypeT(const WrapType type)
{
	const ScopedLock scopedLock(objectLock);

	wrapTypeT_ = type;
	return true;
}

std::string GLESTexture2D::textureName() const
{
	const ScopedLock scopedLock(objectLock);

	return textureName_;
}

bool GLESTexture2D::setTextureName(const std::string& name)
{
	if (name.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	textureName_ = name;

	return true;
}

void GLESTexture2D::createMipmap()
{
	ocean_assert(GL_NO_ERROR == glGetError());

	if (useMipmap_)
	{
		if (primaryTextureId_ != 0u)
		{
			glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
			ocean_assert(GL_NO_ERROR == glGetError());

			glGenerateMipmap(GL_TEXTURE_2D);
			ocean_assert(GL_NO_ERROR == glGetError());
		}

		if (secondaryTextureId_ != 0u)
		{
			glBindTexture(GL_TEXTURE_2D, secondaryTextureId_);
			ocean_assert(GL_NO_ERROR == glGetError());

			glGenerateMipmap(GL_TEXTURE_2D);
			ocean_assert(GL_NO_ERROR == glGetError());
		}
	}

	ocean_assert(GL_NO_ERROR == glGetError());
}

bool GLESTexture2D::defineTextureObject(const TextureProperties& properties, const FrameType& frameType)
{
	ocean_assert(frameType.isValid());

	if (!definePrimaryTextureObject(properties, frameType))
	{
		return false;
	}

	if (properties.needsSecondaryTextureObjects())
	{
		return defineSecondaryTextureObject(properties, frameType);
	}

	return true;
}

bool GLESTexture2D::definePrimaryTextureObject(const TextureProperties& properties, const FrameType& frameType)
{
	ocean_assert(frameType.isValid());

	unsigned int width = 0u, height = 0u;
	GLenum format = 0, type = 0;
	properties.primaryTextureProperties(frameType, width, height, format, type);

	if (primaryTextureId_ == 0u)
	{
		glGenTextures(1, &primaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());

		if (primaryTextureId_ == 0u)
		{
			ocean_assert(false && "This should never happen");
			return false;
		}
	}

	glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	// iOS seems not to support mipmaps with non-power-of-two resolutions

	if (!Utilities::isPowerOfTwo(width) || !Utilities::isPowerOfTwo(height))
	{
		if (minificationFilterMode_ != MIN_MODE_LINEAR)
		{
			Log::warning() << "Due to the non-power-of-two texture the minification filter was set to linear!";
			minificationFilterMode_ = MIN_MODE_LINEAR;
		}

		if (magnificationFilterMode_ != MAG_MODE_LINEAR)
		{
			Log::warning() << "Due to the non-power-of-two texture the magnification filter was set to linear!";
			magnificationFilterMode_ = MAG_MODE_LINEAR;
		}

		if (wrapTypeS_ != WRAP_CLAMP)
		{
			Log::warning() << "Due to the non-power-of-two texture the wrap-s mode was set to clamp-to-edge!";
			wrapTypeS_ = WRAP_CLAMP;
		}

		if (wrapTypeT_ != WRAP_CLAMP)
		{
			Log::warning() << "Due to the non-power-of-two texture the wrap-t mode was set to clamp-to-edge!";
			wrapTypeT_ = WRAP_CLAMP;
		}
	}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

bool GLESTexture2D::defineSecondaryTextureObject(const TextureProperties& properties, const FrameType& frameType)
{
	ocean_assert(frameType.isValid());
	ocean_assert(properties.needsSecondaryTextureObjects());

	unsigned int width = 0u;
	unsigned int height = 0u;
	GLenum format = 0;
	GLenum type = 0;

	if (!properties.secondaryTextureProperties(frameType, width, height, format, type))
	{
		return false;
	}

	if (secondaryTextureId_ == 0u)
	{
		glGenTextures(1, &secondaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());

		if (secondaryTextureId_ == 0u)
		{
			ocean_assert(false && "This should never happen");
			return false;
		}
	}

	glBindTexture(GL_TEXTURE_2D, secondaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

bool GLESTexture2D::determineAlignment(const Frame& frame, const unsigned int planeIndex, unsigned int& rowLength, unsigned int& byteAlignment)
{
	ocean_assert(frame.isValid() && planeIndex < frame.numberPlanes());

	const unsigned int planeStrideBytes = frame.strideBytes(planeIndex);
	ocean_assert(planeStrideBytes >= 1u);

	// GL_UNPACK_ROW_LENGTH counts pixels of the plane rather than bytes, so a padding which is not a whole number of plane pixels cannot be expressed;
	// rowLength is 0 in that case, which GL reads as "use the width"
	const unsigned int planeStrideElements = frame.strideElements(planeIndex);
	const unsigned int planeChannels = frame.planeChannels(planeIndex);

	if (planeChannels != 0u && planeStrideElements % planeChannels == 0u)
	{
		rowLength = planeStrideElements / planeChannels;
	}
	else
	{
		rowLength = 0u;
	}

	if (planeStrideBytes % 4u == 0u)
	{
		byteAlignment = 4u;
	}
	else if (planeStrideBytes % 2u == 0u)
	{
		byteAlignment = 2u;
	}
	else
	{
		byteAlignment = 1u;
	}

	return true;
}

const GLESTexture2D::TextureProperties* GLESTexture2D::textureProperties(const FrameType::PixelFormat pixelFormat)
{
	ocean_assert(pixelFormat != FrameType::FORMAT_UNDEFINED);

	static const TexturePropertiesMap texturePropertiesMap = createTexturePropertiesMap();

	TexturePropertiesMap::const_iterator iProperties = texturePropertiesMap.find(pixelFormat);

	if (iProperties != texturePropertiesMap.cend())
	{
		return &iProperties->second;
	}

	if (FrameType::dataType(pixelFormat) == FrameType::DT_SIGNED_FLOAT_32)
	{
		iProperties = texturePropertiesMap.find(FrameType::genericPixelFormat<float>(FrameType::channels(pixelFormat)));

		if (iProperties != texturePropertiesMap.cend())
		{
			return &iProperties->second;
		}
	}

	return nullptr;
}

GLESTexture2D::TexturePropertiesMap GLESTexture2D::createTexturePropertiesMap()
{
#ifdef OCEAN_RENDERING_GLES_USE_ES
	constexpr GLenum oneChannelFormat = GL_LUMINANCE;
	constexpr GLenum twoChannelsFormat = GL_LUMINANCE_ALPHA;
#else
	constexpr GLenum oneChannelFormat = GL_RED;
	constexpr GLenum twoChannelsFormat = GL_RG;
#endif

	TexturePropertiesMap map;

	// pixel formats which can be uploaded as they are, with one texture

	addTextureProperties(map, FrameType::FORMAT_BGR24, TextureProperties(GLESAttribute::PT_TEXTURE_BGRA, GL_RGB, GL_UNSIGNED_BYTE));
	addTextureProperties(map, FrameType::FORMAT_BGRA32, TextureProperties(GLESAttribute::PT_TEXTURE_BGRA, GL_RGBA, GL_UNSIGNED_BYTE));
	addTextureProperties(map, FrameType::FORMAT_RGB24, TextureProperties(GLESAttribute::PT_TEXTURE_RGBA, GL_RGB, GL_UNSIGNED_BYTE));
	addTextureProperties(map, FrameType::FORMAT_RGB4444, TextureProperties(GLESAttribute::PT_TEXTURE_RGBA, GL_RGB, GL_UNSIGNED_SHORT_4_4_4_4));
	addTextureProperties(map, FrameType::FORMAT_RGB5551, TextureProperties(GLESAttribute::PT_TEXTURE_RGBA, GL_RGB, GL_UNSIGNED_SHORT_5_5_5_1));
	addTextureProperties(map, FrameType::FORMAT_RGB565, TextureProperties(GLESAttribute::PT_TEXTURE_RGBA, GL_RGB, GL_UNSIGNED_SHORT_5_6_5));
	addTextureProperties(map, FrameType::FORMAT_RGBA32, TextureProperties(GLESAttribute::PT_TEXTURE_RGBA, GL_RGBA, GL_UNSIGNED_BYTE));
	addTextureProperties(map, FrameType::FORMAT_RGBA4444, TextureProperties(GLESAttribute::PT_TEXTURE_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4));
	addTextureProperties(map, FrameType::FORMAT_YA16, TextureProperties(GLESAttribute::PT_UNKNOWN, twoChannelsFormat, GL_UNSIGNED_BYTE));
	addTextureProperties(map, FrameType::FORMAT_Y8_LIMITED_RANGE, TextureProperties(GLESAttribute::PT_TEXTURE_Y, oneChannelFormat, GL_UNSIGNED_BYTE));
	addTextureProperties(map, FrameType::FORMAT_Y8_FULL_RANGE, TextureProperties(GLESAttribute::PT_TEXTURE_Y, oneChannelFormat, GL_UNSIGNED_BYTE));
	addTextureProperties(map, FrameType::FORMAT_YUV24, TextureProperties(GLESAttribute::PT_TEXTURE_YUV24, GL_RGB, GL_UNSIGNED_BYTE));
	addTextureProperties(map, FrameType::FORMAT_YVU24, TextureProperties(GLESAttribute::PT_TEXTURE_YVU24, GL_RGB, GL_UNSIGNED_BYTE));

	addTextureProperties(map, FrameType::genericPixelFormat<float, 1u>(), TextureProperties(GLESAttribute::PT_TEXTURE_Y, oneChannelFormat, GL_FLOAT));
	addTextureProperties(map, FrameType::genericPixelFormat<float, 2u>(), TextureProperties(GLESAttribute::PT_UNKNOWN, twoChannelsFormat, GL_FLOAT));
	addTextureProperties(map, FrameType::genericPixelFormat<float, 3u>(), TextureProperties(GLESAttribute::PT_TEXTURE_RGBA, GL_RGB, GL_FLOAT));
	addTextureProperties(map, FrameType::genericPixelFormat<float, 4u>(), TextureProperties(GLESAttribute::PT_TEXTURE_RGBA, GL_RGBA, GL_FLOAT));

	// pixel formats with two planes, the second plane holds both chroma channels

	addTextureProperties(map, FrameType::FORMAT_Y_UV12_LIMITED_RANGE, TextureProperties(GLESAttribute::PT_TEXTURE_Y_UV12, oneChannelFormat, GL_UNSIGNED_BYTE, twoChannelsFormat, GL_UNSIGNED_BYTE, 2u, 2u, SL_PLANE_1));
	addTextureProperties(map, FrameType::FORMAT_Y_UV12_FULL_RANGE, TextureProperties(GLESAttribute::PT_TEXTURE_Y_UV12, oneChannelFormat, GL_UNSIGNED_BYTE, twoChannelsFormat, GL_UNSIGNED_BYTE, 2u, 2u, SL_PLANE_1));
	addTextureProperties(map, FrameType::FORMAT_Y_VU12_LIMITED_RANGE, TextureProperties(GLESAttribute::PT_TEXTURE_Y_VU12, oneChannelFormat, GL_UNSIGNED_BYTE, twoChannelsFormat, GL_UNSIGNED_BYTE, 2u, 2u, SL_PLANE_1));
	addTextureProperties(map, FrameType::FORMAT_Y_VU12_FULL_RANGE, TextureProperties(GLESAttribute::PT_TEXTURE_Y_VU12, oneChannelFormat, GL_UNSIGNED_BYTE, twoChannelsFormat, GL_UNSIGNED_BYTE, 2u, 2u, SL_PLANE_1));

	// pixel formats with three planes, both chroma planes are stacked into the secondary texture,
	// the Y_U_V12 shader is used for Y_V_U12 as well, the source planes are simply swapped

	addTextureProperties(map, FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, TextureProperties(GLESAttribute::PT_TEXTURE_Y_U_V12, oneChannelFormat, GL_UNSIGNED_BYTE, oneChannelFormat, GL_UNSIGNED_BYTE, 2u, 1u, SL_PLANE_1_2));
	addTextureProperties(map, FrameType::FORMAT_Y_U_V12_FULL_RANGE, TextureProperties(GLESAttribute::PT_TEXTURE_Y_U_V12, oneChannelFormat, GL_UNSIGNED_BYTE, oneChannelFormat, GL_UNSIGNED_BYTE, 2u, 1u, SL_PLANE_1_2));
	addTextureProperties(map, FrameType::FORMAT_Y_V_U12_LIMITED_RANGE, TextureProperties(GLESAttribute::PT_TEXTURE_Y_U_V12, oneChannelFormat, GL_UNSIGNED_BYTE, oneChannelFormat, GL_UNSIGNED_BYTE, 2u, 1u, SL_PLANE_2_1));
	addTextureProperties(map, FrameType::FORMAT_Y_V_U12_FULL_RANGE, TextureProperties(GLESAttribute::PT_TEXTURE_Y_U_V12, oneChannelFormat, GL_UNSIGNED_BYTE, oneChannelFormat, GL_UNSIGNED_BYTE, 2u, 1u, SL_PLANE_2_1));

	// pixel formats which need a conversion before they can be uploaded, they take the properties of the pixel format they are converted to

	addConvertedTextureProperties(map, FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_Y8);
	addConvertedTextureProperties(map, FrameType::FORMAT_RGGB10_PACKED, FrameType::FORMAT_RGB24);
	addConvertedTextureProperties(map, FrameType::FORMAT_YUYV16, FrameType::FORMAT_RGB24);

	return map;
}

GLESTexture2D::TextureProperties::TextureProperties(const GLESAttribute::ProgramType programType, const GLenum primaryFormat, const GLenum primaryType) :
	programType_(programType),
	primaryFormat_(primaryFormat),
	primaryType_(primaryType)
{
	ocean_assert(primaryFormat_ != 0 && primaryType_ != 0);
}

GLESTexture2D::TextureProperties::TextureProperties(const GLESAttribute::ProgramType programType, const GLenum primaryFormat, const GLenum primaryType, const GLenum secondaryFormat, const GLenum secondaryType, const unsigned int secondaryWidthDivisor, const unsigned int secondaryHeightDivisor, const SecondaryLayout secondaryLayout) :
	programType_(programType),
	primaryFormat_(primaryFormat),
	primaryType_(primaryType),
	secondaryFormat_(secondaryFormat),
	secondaryType_(secondaryType),
	secondaryWidthDivisor_(secondaryWidthDivisor),
	secondaryHeightDivisor_(secondaryHeightDivisor),
	secondaryLayout_(secondaryLayout)
{
	ocean_assert(programType_ != GLESAttribute::PT_UNKNOWN);
	ocean_assert(primaryFormat_ != 0 && primaryType_ != 0);
	ocean_assert(secondaryFormat_ != 0 && secondaryType_ != 0);
	ocean_assert(secondaryWidthDivisor_ >= 1u && secondaryHeightDivisor_ >= 1u);
	ocean_assert(secondaryLayout_ != SL_NONE);
}

void GLESTexture2D::addTextureProperties(TexturePropertiesMap& map, const FrameType::PixelFormat pixelFormat, const TextureProperties& properties)
{
	// several pixel format names are aliases of each other, e.g., FORMAT_Y_U_V12_LIMITED_RANGE == FORMAT_Y_U_V12
	const bool inserted = map.emplace(pixelFormat, properties).second;
	ocean_assert_and_suppress_unused(inserted, inserted);
}

void GLESTexture2D::addConvertedTextureProperties(TexturePropertiesMap& map, const FrameType::PixelFormat pixelFormat, const FrameType::PixelFormat internalPixelFormat)
{
	const TexturePropertiesMap::const_iterator iInternal = map.find(internalPixelFormat);
	ocean_assert(iInternal != map.cend());

	// a conversion into a pixel format which needs a conversion itself is not supported
	ocean_assert(iInternal->second.internalPixelFormat_ == FrameType::FORMAT_UNDEFINED);

	TextureProperties properties = iInternal->second;
	properties.internalPixelFormat_ = internalPixelFormat;

	addTextureProperties(map, pixelFormat, properties);
}

FrameType GLESTexture2D::TextureProperties::internalFrameType(const FrameType& frameType) const
{
	ocean_assert(frameType.isValid());

	if (internalPixelFormat_ == FrameType::FORMAT_UNDEFINED)
	{
		return frameType;
	}

	return FrameType(frameType, internalPixelFormat_);
}

void GLESTexture2D::TextureProperties::primaryTextureProperties(const FrameType& frameType, unsigned int& width, unsigned int& height, GLenum& format, GLenum& type) const
{
	ocean_assert(frameType.isValid());

	width = frameType.width();
	height = frameType.height();
	format = primaryFormat_;
	type = primaryType_;
}

bool GLESTexture2D::TextureProperties::secondaryTextureProperties(const FrameType& frameType, unsigned int& width, unsigned int& height, GLenum& format, GLenum& type) const
{
	ocean_assert(frameType.isValid());

	if (!needsSecondaryTextureObjects())
	{
		width = 0u;
		height = 0u;
		format = 0;
		type = 0;

		return false;
	}

	ocean_assert(secondaryWidthDivisor_ != 0u && secondaryHeightDivisor_ != 0u);

	width = frameType.width() / secondaryWidthDivisor_;
	height = frameType.height() / secondaryHeightDivisor_;
	format = secondaryFormat_;
	type = secondaryType_;

	return true;
}

bool GLESTexture2D::primaryTextureName(const std::string& names, std::string& name)
{
	ocean_assert(!names.empty());
	if (names.empty())
	{
		return false;
	}

	// we accept the following format '<PRIMARY>,<SECONDARY>'

	const std::string::size_type position = names.find(',');

	if (position == std::string::npos)
	{
		name = names;
	}
	else
	{
		name = names.substr(0, position);
	}

	return true;
}

bool GLESTexture2D::secondaryTextureName(const std::string& names, std::string& name)
{
	ocean_assert(!names.empty());
	if (names.empty())
	{
		return false;
	}

	// we accept the following format '<PRIMARY>,<SECONDARY>'

	const std::string::size_type position = names.find(',');

	if (position == std::string::npos || position + 1 == names.size())
	{
		return false;
	}

	name = names.substr(position + 1);
	return true;
}

unsigned int GLESTexture2D::bindTexture(GLESShaderProgram& shaderProgram, const unsigned int id)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	const ScopedLock scopedLock(objectLock);

	if (primaryTextureId_ == 0u)
	{
		return 0u;
	}

	const GLenum glesMinificationFilterMode = translateMinificationFilterMode(minificationFilterMode_);
	const GLenum glesMagnificationFilterMode = translateMagnificationFilterMode(magnificationFilterMode_);

	const GLenum glesWrapTypeS = translateWrapType(wrapTypeS_);
	const GLenum glesWrapTypeT = translateWrapType(wrapTypeT_);

	glActiveTexture(GLenum(GL_TEXTURE0 + id));
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glesMinificationFilterMode);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glesMagnificationFilterMode);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glesWrapTypeS);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glesWrapTypeT);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationTextureTransformMatrix = glGetUniformLocation(shaderProgram.id(), "textureTransformationMatrix");
	if (locationTextureTransformMatrix != -1)
	{
		ocean_assert(transformation_.isValid());
		setUniform(locationTextureTransformMatrix, transformation_);
	}

	const GLint locationTextureOriginLowerLeft = glGetUniformLocation(shaderProgram.id(), "textureOriginLowerLeft");
	if (locationTextureOriginLowerLeft != -1)
	{
		setUniform(locationTextureOriginLowerLeft, textureFrameType_.pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT ? 1 : 0);
	}

	std::string primaryTexture;
	if (primaryTextureName(textureName_, primaryTexture))
	{
		const GLint locationTexture = glGetUniformLocation(shaderProgram.id(), primaryTexture.c_str());
		if (locationTexture != -1)
		{
			setUniform(locationTexture, int(id));
		}
	}

	if (secondaryTextureId_ == 0u)
	{
		return 1u;
	}

	glActiveTexture(GLenum(GL_TEXTURE0 + id + 1u));
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, secondaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glesMinificationFilterMode);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glesMagnificationFilterMode);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glesWrapTypeS);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glesWrapTypeT);
	ocean_assert(GL_NO_ERROR == glGetError());

	std::string secondaryTexture;
	if (secondaryTextureName(textureName_, secondaryTexture))
	{
		const GLint locationSecondaryTexture = glGetUniformLocation(shaderProgram.id(), secondaryTexture.c_str());

		if (locationSecondaryTexture != -1)
		{
			setUniform(locationSecondaryTexture, int(id + 1u));

			return 2u;
		}
		else
		{
			ocean_assert(false && "This should never happen!");
		}
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}

	return 1u;
}

bool GLESTexture2D::updateTexture(const Frame& frame)
{
	ocean_assert(frame.isValid());

	const TextureProperties* properties = textureProperties(frame.pixelFormat());

	if (properties == nullptr)
	{
		ocean_assert(false && "Missing implementation!");
		return false;
	}

	const FrameType internalFrameType = properties->internalFrameType(frame.frameType());
	ocean_assert(internalFrameType.isValid());

	sourceFrameType_ = frame.frameType();

	if (internalFrameType != textureFrameType_)
	{
		if (!defineTextureObject(*properties, internalFrameType))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		const bool resetShaderProgram = textureFrameType_.isValid();

		textureFrameType_ = internalFrameType;

		if (resetShaderProgram)
		{
			// the pixel format has changed, we need to use a new shader

			for (const ObjectRef& texturesObject : parentObjects())
			{
				for (const ObjectRef& attributeSetObject : texturesObject->parentObjects())
				{
					attributeSetObject.force<GLESAttributeSet>().resetShaderProgram();
				}
			}
		}
	}

	frameTimestamp_ = frame.timestamp();

	GLenum format = 0u;
	GLenum type = 0u;
	unsigned int width = 0u;
	unsigned int height = 0u;

	properties->primaryTextureProperties(textureFrameType_, width, height, format, type);

	// GL_UNPACK_ROW_LENGTH can only express a padding which is a whole number of plane pixels,
	// so any other padding is removed up front rather than being uploaded skewed
	Frame continuousFrame;
	for (unsigned int planeIndex = 0u; planeIndex < frame.numberPlanes(); ++planeIndex)
	{
		const unsigned int planeChannels = frame.planeChannels(planeIndex);

		if (planeChannels == 0u || frame.strideElements(planeIndex) % planeChannels != 0u)
		{
			continuousFrame = Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			break;
		}
	}

	const Frame& sourceFrame = continuousFrame.isValid() ? continuousFrame : frame;

	const Frame* primaryTextureFrame = &sourceFrame;
	bool mayNeedSecondaryTexture = true;

	if (sourceFrame.frameType() != textureFrameType_)
	{
		CV::FrameConverter::Options convertOptions;
		if (sourceFrame.pixelFormat() == FrameType::FORMAT_Y10_PACKED || sourceFrame.pixelFormat() == FrameType::FORMAT_RGGB10_PACKED)
		{
			ocean_assert(textureFrameType_.pixelFormat() == FrameType::FORMAT_Y8 || textureFrameType_.pixelFormat() == FrameType::FORMAT_RGB24);

			constexpr float gamma = 0.7f;
			convertOptions = CV::FrameConverter::Options(gamma);
		}

		if (!CV::FrameConverter::Comfort::convert(sourceFrame, textureFrameType_.pixelFormat(), textureFrameType_.pixelOrigin(), conversionFrame_, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr, convertOptions))
		{
			return false;
		}

		ocean_assert(conversionFrame_.numberPlanes() == 1u);

		primaryTextureFrame = &conversionFrame_;
		mayNeedSecondaryTexture = false;
	}

	ocean_assert(primaryTextureId_ != 0u);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(primaryTextureFrame != nullptr);
	ocean_assert(primaryTextureFrame->dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	unsigned int rowLength = 0u;
	unsigned int byteAlignment = 0u;
	if (!determineAlignment(*primaryTextureFrame, 0u, rowLength, byteAlignment))
	{
		return false;
	}

	glPixelStorei(GL_UNPACK_ROW_LENGTH, int(rowLength));
	glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, primaryTextureFrame->constdata<void>(0u));
	ocean_assert(GL_NO_ERROR == glGetError());

	// GL_UNPACK_ROW_LENGTH is global state, so it must not leak into any other upload
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	if (mayNeedSecondaryTexture && properties->secondaryTextureProperties(textureFrameType_, width, height, format, type))
	{
		ocean_assert(secondaryTextureId_ != 0u);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindTexture(GL_TEXTURE_2D, secondaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());

		switch (properties->secondaryLayout_)
		{
			case SL_PLANE_1:
			{
				rowLength = 0u;
				byteAlignment = 0u;
				if (!determineAlignment(sourceFrame, 1u, rowLength, byteAlignment))
				{
					return false;
				}

				glPixelStorei(GL_UNPACK_ROW_LENGTH, int(rowLength));
				glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
				ocean_assert(GL_NO_ERROR == glGetError());

				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, sourceFrame.constdata<void>(1u));
				ocean_assert(GL_NO_ERROR == glGetError());

				// GL_UNPACK_ROW_LENGTH is global state, so it must not leak into any other upload
				glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

				break;
			}

			case SL_PLANE_1_2:
			case SL_PLANE_2_1:
			{
				const unsigned int firstPlaneIndex = properties->secondaryLayout_ == SL_PLANE_1_2 ? 1u : 2u;
				const unsigned int secondPlaneIndex = properties->secondaryLayout_ == SL_PLANE_1_2 ? 2u : 1u;

				const GLsizei height_2 = height / 2;

				rowLength = 0u;
				byteAlignment = 0u;
				if (!determineAlignment(sourceFrame, firstPlaneIndex, rowLength, byteAlignment))
				{
					return false;
				}

				glPixelStorei(GL_UNPACK_ROW_LENGTH, int(rowLength));
				glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
				ocean_assert(GL_NO_ERROR == glGetError());

				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height_2, format, type, sourceFrame.constdata<void>(firstPlaneIndex));
				ocean_assert(GL_NO_ERROR == glGetError());

				// GL_UNPACK_ROW_LENGTH is global state, so it must not leak into any other upload
				glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

				rowLength = 0u;
				byteAlignment = 0u;
				if (!determineAlignment(sourceFrame, secondPlaneIndex, rowLength, byteAlignment))
				{
					return false;
				}

				glPixelStorei(GL_UNPACK_ROW_LENGTH, int(rowLength));
				glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
				ocean_assert(GL_NO_ERROR == glGetError());

				const GLint& yOffset = GLint(height_2);

				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, yOffset, width, height_2, format, type, sourceFrame.constdata<void>(secondPlaneIndex));
				ocean_assert(GL_NO_ERROR == glGetError());

				// GL_UNPACK_ROW_LENGTH is global state, so it must not leak into any other upload
				glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

				break;
			}

			case SL_NONE:
				ocean_assert(false && "This should never happen!");
				return false;
		}
	}
	else
	{
		if (secondaryTextureId_ != 0u)
		{
			// previously we needed a second texture, now we do not need it anymore

			glDeleteTextures(1, &secondaryTextureId_);
			ocean_assert(GL_NO_ERROR == glGetError());
			secondaryTextureId_ = 0u;
		}
	}

	if (useMipmap_)
	{
		createMipmap();
	}

	return true;
}

}

}

}
