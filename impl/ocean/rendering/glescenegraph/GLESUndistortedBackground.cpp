/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESUndistortedBackground.h"

#include "ocean/rendering/DepthAttribute.h"

#include "ocean/rendering/Engine.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESUndistortedBackground::GLESUndistortedBackground() :
	GLESBackground(),
	UndistortedBackground()
{
	triangleStrips_ = engine().factory().createTriangleStrips();
	vertexSet_ = engine().factory().createVertexSet();
	textures_ = engine().factory().createTextures();
	texture_ = engine().factory().createMediaTexture2D();
	attributeSet_ = engine().factory().createAttributeSet();

	DepthAttributeRef depthAttribute = engine().factory().createDepthAttribute();
	depthAttribute->setTestingEnabled(false);
	depthAttribute->setWritingEnabled(false);
	attributeSet_->addAttribute(depthAttribute);

	textures_->setTexture(texture_, 0);
	attributeSet_->addAttribute(textures_);

	triangleStrips_->setVertexSet(vertexSet_);

	registerDynamicUpdateObject();
}

GLESUndistortedBackground::~GLESUndistortedBackground()
{
	unregisterDynamicUpdateObject();
}

const Timestamp& GLESUndistortedBackground::cameraTimestamp() const
{
	return cameraTimestamp_;
}

const SquareMatrix4& GLESUndistortedBackground::normalizedCameraFrustumMatrix() const
{
	return normalizedCameraFrustumMatrix_;
}

Texture2DRef GLESUndistortedBackground::texture() const
{
	return texture_;
}

Texture2DRef GLESUndistortedBackground::offsetTexture() const
{
	return offsetTexture_;
}

unsigned int GLESUndistortedBackground::horizontalElements() const
{
	return horizontalElements_;
}

unsigned int GLESUndistortedBackground::verticalElements() const
{
	return verticalElements_;
}

void GLESUndistortedBackground::setHorizontalElements(const unsigned int elements)
{
	if (elements < 1 || horizontalElements_ == elements)
	{
		return;
	}

	horizontalElements_ = elements;
	cameraChanged_ = true;
}

void GLESUndistortedBackground::setVerticalElements(const unsigned int elements)
{
	if (elements < 1 || verticalElements_ == elements)
	{
		return;
	}

	verticalElements_ = elements;
	cameraChanged_ = true;
}

bool GLESUndistortedBackground::setDisplayType(const DisplayType type)
{
	if (type == DT_FASTEST)
	{
		return true;
	}

	Log::warning() << "This display type is not supported!";
	return false;
}

void GLESUndistortedBackground::addToTraverser(const GLESFramebuffer& /*framebuffer*/, const SquareMatrix4& /*projectionMatrix*/, const HomogenousMatrix4& camera_T_parent, const Lights& lights, GLESTraverser& traverser) const
{
	const ScopedLock scopedLock(objectLock);

	if (!visible_)
	{
		return;
	}

	const SquareMatrix3 normalMatrix(camera_T_parent.rotationMatrix().inverted().transposed());

	traverser.addRenderable(triangleStrips_, attributeSet_, HomogenousMatrix4(backgroundPosition, backgroundOrientation), normalMatrix, lights);
}

void GLESUndistortedBackground::onMediumCameraChanged(const Timestamp timestamp)
{
	cameraTimestamp_ = timestamp;

	rebuildPrimitive();
}

void GLESUndistortedBackground::rebuildPrimitive()
{
	ocean_assert(mediumCamera_);
	ocean_assert(horizontalElements_ > 0u && verticalElements_ > 0u);
	ocean_assert(backgroundDistance > Numeric::eps());

	if (!mediumCamera_)
	{
		return;
	}

	Vertices vertices;
	Normals normals;
	TextureCoordinates textureCoordinates;
	VertexIndexGroups strips(verticalElements_);

	if (mediumCamera_->name() == AnyCameraPinhole::WrappedCamera::name())
	{
		const AnyCameraPinhole& anyCameraPinhole = dynamic_cast<const AnyCameraPinhole&>(*mediumCamera_);
		const PinholeCamera& pinholeCamera = anyCameraPinhole.actualCamera();

		const SquareMatrix4 frustumMatrix(pinholeCamera.frustumMatrix(Scalar(0.01), backgroundDistance));

		const Scalar scalingValues[16] =
		{
			texture_->imageTextureRatio().x() * Scalar(0.5), 0, 0, 0,
			0, texture_->imageTextureRatio().y() * Scalar(0.5), 0, 0,
			0, 0, Scalar(0.5), 0,
			texture_->imageTextureRatio().x() * Scalar(0.5), texture_->imageTextureRatio().y() * Scalar(0.5), Scalar(0.5), 1
		};

		const SquareMatrix4 scalingMatrix(scalingValues);

		normalizedCameraFrustumMatrix_ = scalingMatrix * frustumMatrix * HomogenousMatrix4(backgroundPosition, backgroundOrientation).inverted();

		const Scalar left = -Numeric::tan(pinholeCamera.fovXLeft()) * backgroundDistance;
		const Scalar right = Numeric::tan(pinholeCamera.fovXRight()) * backgroundDistance;
		const Scalar top = Numeric::tan(pinholeCamera.fovYTop()) * backgroundDistance;
		const Scalar bottom = -Numeric::tan(pinholeCamera.fovYBottom()) * backgroundDistance;

		const Scalar horizontalStep = (right - left) / Scalar(horizontalElements_);
		const Scalar verticalStep = (top - bottom) / Scalar(verticalElements_);

		unsigned int vertexCount = 0u;

		for (unsigned int y = 0; y < verticalElements_; y++)
		{
			VertexIndices& strip = strips[y];

			const Scalar yTexelTop = Scalar(y) / Scalar(verticalElements_);
			const Scalar yTexelBottom = Scalar(y + 1) / Scalar(verticalElements_);

			const Scalar yPixelTop = Scalar(pinholeCamera.height() - 1) * yTexelTop;
			const Scalar yPixelBottom = Scalar(pinholeCamera.height() - 1) * yTexelBottom;

			ocean_assert(yPixelTop >= 0 && yPixelTop < Scalar(pinholeCamera.height()));
			ocean_assert(yPixelBottom >= 0 && yPixelBottom < Scalar(pinholeCamera.height()));

			for (unsigned int x = 0; x <= horizontalElements_; x++)
			{
				vertices.emplace_back(left + Scalar(x) * horizontalStep, top - Scalar(y) * verticalStep, -backgroundDistance);
				vertices.emplace_back(left + Scalar(x) * horizontalStep, top - Scalar(y + 1) * verticalStep, -backgroundDistance);

				const Scalar xTexel = Scalar(x) / Scalar(horizontalElements_);
				const Scalar xPixel = Scalar(pinholeCamera.width() - 1) * xTexel;
				ocean_assert(xPixel >= 0 && xPixel < Scalar(pinholeCamera.width()));

				const Vector2 undistortedTop(xPixel, yPixelTop);
				const Vector2 undistortedBottom(xPixel, yPixelBottom);

				const Vector2 distortedTop(pinholeCamera.distort<true>(undistortedTop));
				const Vector2 distortedBottom(pinholeCamera.distort<true>(undistortedBottom));

				const Scalar xDistortedTexelTop = distortedTop.x() / Scalar(pinholeCamera.width() - 1);
				const Scalar yDistortedTexelTop = distortedTop.y() / Scalar(pinholeCamera.height() - 1);

				const Scalar xDistortedTexelBottom = distortedBottom.x() / Scalar(pinholeCamera.width() - 1);
				const Scalar yDistortedTexelBottom = distortedBottom.y() / Scalar(pinholeCamera.height() - 1);

				textureCoordinates.emplace_back(xDistortedTexelTop, 1 - yDistortedTexelTop);
				textureCoordinates.emplace_back(xDistortedTexelBottom, 1 - yDistortedTexelBottom);

				strip.push_back(vertexCount++);
				strip.push_back(vertexCount++);
			}
		}
	}
	else
	{
		normalizedCameraFrustumMatrix_.toNull();

		unsigned int vertexCount = 0u;

		for (unsigned int y = 0; y < verticalElements_; y++)
		{
			VertexIndices& strip = strips[y];

			const Scalar yTexelTop = Scalar(y) / Scalar(verticalElements_);
			const Scalar yTexelBottom = Scalar(y + 1) / Scalar(verticalElements_);

			const Scalar yPixelTop = Scalar(mediumCamera_->height() - 1) * yTexelTop;
			const Scalar yPixelBottom = Scalar(mediumCamera_->height() - 1) * yTexelBottom;

			ocean_assert(yPixelTop >= 0 && yPixelTop < Scalar(mediumCamera_->height()));
			ocean_assert(yPixelBottom >= 0 && yPixelBottom < Scalar(mediumCamera_->height()));

			for (unsigned int x = 0; x <= horizontalElements_; x++)
			{
				const Scalar xTexel = Scalar(x) / Scalar(horizontalElements_);
				const Scalar xPixel = Scalar(mediumCamera_->width() - 1) * xTexel;
				ocean_assert(xPixel >= 0 && xPixel < Scalar(mediumCamera_->width()));

				const Vector3 topRay = mediumCamera_->vector(Vector2(xPixel, yPixelTop), true /*makeUnitVector*/) * backgroundDistance;
				const Vector3 bottomRay = mediumCamera_->vector(Vector2(xPixel, yPixelBottom), true /*makeUnitVector*/) * backgroundDistance;

				vertices.push_back(topRay);
				vertices.push_back(bottomRay);

				textureCoordinates.emplace_back(xTexel, Scalar(1) - yTexelTop);
				textureCoordinates.emplace_back(xTexel, Scalar(1) - yTexelBottom);

				strip.push_back(vertexCount++);
				strip.push_back(vertexCount++);
			}
		}
	}

	normals.insert(normals.begin(), vertices.size(), Vector3(0, 0, 1));

	vertexSet_->setTextureCoordinates(textureCoordinates, 0);
	vertexSet_->setVertices(vertices);
	vertexSet_->setNormals(normals);

	triangleStrips_->setVertexSet(vertexSet_);
	triangleStrips_->setStrips(strips);
}

}

}

}
