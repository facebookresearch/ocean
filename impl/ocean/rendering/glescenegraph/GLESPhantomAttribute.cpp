/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESPhantomAttribute.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESGeometry.h"
#include "ocean/rendering/glescenegraph/GLESPrimitive.h"
#include "ocean/rendering/glescenegraph/GLESUndistortedBackground.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

#include "ocean/rendering/Engine.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESPhantomAttribute::GLESPhantomAttribute() :
	phantomMode(PM_INVALID),
	phantomBackgroundColor(0.0f, 0.0f, 0.0f)
{
	registerDynamicUpdateObject();
}

GLESPhantomAttribute::~GLESPhantomAttribute()
{
	unregisterDynamicUpdateObject();
}

void GLESPhantomAttribute::bindAttribute(const GLESFramebuffer& /*framebuffer*/, GLESShaderProgram& shaderProgram)
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(GL_NO_ERROR == glGetError());

	switch (attributePhantomMode)
	{
		case PhantomAttribute::PM_DEBUG:
			// nothing to do here
			break;

		case PhantomAttribute::PM_COLOR:
		{
			const GLint locationColor = glGetUniformLocation(shaderProgram.id(), "color");
			setUniform(locationColor, phantomBackgroundColor);

			break;
		}

		case PhantomAttribute::EPM_VIDEO:
		{
			ocean_assert(false && "Missing implementation");
			break;
		}

		case PhantomAttribute::EPM_VIDEO_FAST:
		{
			// we simply draw the phantom objects with a full transparent color because we simply want to have the depth values in the z-buffer for occlusion handling
			const GLint locationColor = glGetUniformLocation(shaderProgram.id(), "color");
			setUniform(locationColor, Vector4(0.0, 0, 0, 0.0));

			glEnable(GL_BLEND);
			ocean_assert(GL_NO_ERROR == glGetError());

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			ocean_assert(GL_NO_ERROR == glGetError());

			break;
		}

		case PhantomAttribute::EPM_VIDEO_FAST_TEXTURE_COORDINATES:
		{
			ocean_assert(!phantomTextureCoordinateTransform.isNull());

			const SmartObjectRef<GLESTexture> backgroundTexture(phantomUndistortedBackgroundTexture);
			ocean_assert(backgroundTexture);

			backgroundTexture->bindTexture(shaderProgram, 0u);

			const GLint frustumMatrixLocation = glGetUniformLocation(shaderProgram.id(), "textureFrustumMatrix");
			setUniform(frustumMatrixLocation, phantomTextureCoordinateTransform);

			break;
		}

		default:
			ocean_assert(false && "Missing implementation");
	}
}

void GLESPhantomAttribute::unbindAttribute()
{
	ocean_assert(GL_NO_ERROR == glGetError());
	glDisable(GL_BLEND);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESPhantomAttribute::onDynamicUpdate(const ViewRef& view, const Timestamp timestamp)
{
	// Store the background color for the render mode PM_COLOR
	phantomBackgroundColor = view->backgroundColor();

	// Update the view mode, if applicable
	UndistortedBackgroundRef undistortedBackground(view->background());
	PhantomMode mode = view->phantomMode();

	if (mode == PhantomAttribute::PM_DEFAULT)
	{
		if (view->background() && undistortedBackground)
		{
			mode = PhantomMode(PhantomAttribute::EPM_VIDEO_FAST);
		}
		else
		{
			mode = PhantomAttribute::PM_COLOR;
		}
	}

	if (mode == PhantomAttribute::PM_VIDEO)
	{
		if (undistortedBackground && undistortedBackground->displayType() == UndistortedBackground::DT_FASTEST)
		{
			mode = PhantomMode(PhantomAttribute::EPM_VIDEO_FAST);
		}
		else
		{
			mode = PhantomMode(PhantomAttribute::EPM_VIDEO);
		}
	}

	// When the rendering mode changes, tell the "management" that a new shader may be needed.
	if (attributePhantomMode != (unsigned int)(mode))
	{
		attributePhantomMode = mode;

		const ObjectRefs parents = parentObjects();
		for (ObjectRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
		{
			const SmartObjectRef<GLESAttributeSet> attributeSet(*i);

			if (attributeSet)
			{
				attributeSet->resetShaderProgram();
			}
		}
	}

	ocean_assert(attributePhantomMode != PhantomAttribute::PM_INVALID);
	ocean_assert(attributePhantomMode != PhantomAttribute::PM_DEFAULT);

	updateBackgroundInformation(view, timestamp);
}

GLESAttribute::ProgramType GLESPhantomAttribute::necessaryShader() const
{
	GLESAttribute::ProgramType type = GLESAttribute::PI_DEBUG_GRAY;

	switch (attributePhantomMode)
	{
		case PhantomAttribute::PM_COLOR:
			return GLESAttribute::PT_STATIC_COLOR;

		case PhantomAttribute::EPM_VIDEO_FAST:
			type = GLESAttribute::PT_PHANTOM_VIDEO_FAST;
			break;

		case PhantomAttribute::EPM_VIDEO_FAST_TEXTURE_COORDINATES:
			type = GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST;
			break;

		default:
			ocean_assert(attributePhantomMode == PhantomAttribute::PM_DEBUG);
			return GLESAttribute::PI_DEBUG_GRAY;
	}

	if (phantomUndistortedBackgroundTexture.isNull())
		return type;

	switch (phantomUndistortedBackgroundTexture->frameType().pixelFormat())
	{
		case FrameType::FORMAT_BGR24:
		case FrameType::FORMAT_BGRA32:
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_BGRA);
			break;

		case FrameType::FORMAT_RGB24:
		case FrameType::FORMAT_RGBA32:
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_RGBA);
			break;

		case FrameType::FORMAT_YVU24:
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_YVU24);
			break;

		case FrameType::FORMAT_YUV24:
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_YUV24);
			break;

		case FrameType::FORMAT_Y_VU12:
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_Y_VU12);
			break;

		case FrameType::FORMAT_Y_UV12:
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_Y_UV12);
			break;

		case FrameType::FORMAT_Y_U_V12:
		case FrameType::FORMAT_Y_V_U12:
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_Y_U_V12);
			break;

		default:
			ocean_assert(false && "Invalid pixel format!");
			break;
	}

	return type;
}

void GLESPhantomAttribute::updateBackgroundInformation(const ViewRef& view, const Timestamp timestamp)
{
	phantomTextureCoordinateTransform.toNull();

	if (attributePhantomMode != EPM_VIDEO_FAST)
	{
		return;
	}

	// Update matrices, texture IDs, etc., which are required for PM_VIDEO
	const SmartObjectRef<GLESUndistortedBackground> undistortedBackground(view->background());

	if (undistortedBackground.isNull())
	{
		return;
	}

	// first we check whether this phantom attribute belongs to a VertexSet with explicit phantom texture coordinate

	const ObjectRefSet parentNodes(this->parentNodes());

	for (ObjectRefSet::iterator i = parentNodes.begin(); phantomTextureCoordinateTransform.isNull() && i != parentNodes.end(); ++i)
	{
		const SmartObjectRef<GLESGeometry> geometry(*i);
		ocean_assert(geometry);

		for (unsigned int n = 0u; phantomTextureCoordinateTransform.isNull() && n < geometry->numberRenderables(); ++n)
		{
			const SmartObjectRef<GLESPrimitive> primitive(geometry->renderable(n));

			if (primitive)
			{
				const SmartObjectRef<GLESVertexSet> vertexSet(primitive->vertexSet());

				if (vertexSet)
				{
					const std::string phantomTextureCoordinate = vertexSet->phantomTextureCoordinateSystem();

					HomogenousMatrix4 worldTransformation(true);

					if (!phantomTextureCoordinate.empty())
					{
						const ObjectRef referenceObject(engine().object(phantomTextureCoordinate));

						if (referenceObject)
						{
							NodeRef referenceNode(referenceObject);
							if (referenceNode)
							{
								worldTransformation = referenceNode->worldTransformation();
							}
							else
							{
								const ObjectRefSet parents(referenceObject->parentNodes());
								if (!parents.empty())
								{
									const NodeRef parentNode(*parents.begin());
									ocean_assert(parentNode);

									worldTransformation = parentNode->worldTransformation();
								}
							}

							const HomogenousMatrix4 modelView(view->transformation().inverted());
							phantomTextureCoordinateTransform = (undistortedBackground->normalizedCameraFrustumMatrix() * modelView * worldTransformation);
						}
					}
				}
			}
		}
	}

	ExtendedPhantomMode targetPhantomMode = EPM_INVALID;

	if (undistortedBackground->offsetTexture().isNull())
	{
		targetPhantomMode = phantomTextureCoordinateTransform.isNull() ? EPM_VIDEO_FAST : EPM_VIDEO_FAST_TEXTURE_COORDINATES;
	}
	else
	{
		targetPhantomMode = phantomTextureCoordinateTransform.isNull() ? EPM_VIDEO : EPM_VIDEO_TEXTURE_COORDINATES;
	}

	if (attributePhantomMode != (unsigned int)(targetPhantomMode))
	{
		attributePhantomMode = targetPhantomMode;
		phantomVideoTimestamp.toInvalid();
	}

	if (phantomVideoTimestamp != timestamp)
	{
		if (undistortedBackground->cameraTimestamp() != phantomVideoTimestamp)
		{
			phantomVideoTimestamp = undistortedBackground->cameraTimestamp();

			phantomNormalizedCameraFrustumMatrix = undistortedBackground->normalizedCameraFrustumMatrix();
			phantomUndistortedBackgroundTexture  = undistortedBackground->texture();

			if (undistortedBackground->offsetTexture())
			{
				phantomUndistortedBackgroundOffsetTexture = undistortedBackground->offsetTexture();
			}
		}
	}
}

} // namespace GLESceneGraph

} // namespace Rendering

} // namespace Ocean
