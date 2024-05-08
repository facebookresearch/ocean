/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESUndistortedBackground.h"
#include "ocean/rendering/glescenegraph/GLESTriangleStrips.h"

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
	UndistortedBackground(),
#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
	backgroundHorizontalElements(1),
	backgroundVerticalElements(1)
#else
	backgroundHorizontalElements(20),
	backgroundVerticalElements(20)
#endif
{
	backgroundTriangleStrips = engine().factory().createTriangleStrips();
	backgroundVertexSet = engine().factory().createVertexSet();
	backgroundTextures = engine().factory().createTextures();
	texture_ = engine().factory().createMediaTexture2D();
	backgroundAttributeSet = engine().factory().createAttributeSet();

	DepthAttributeRef depthAttribute = engine().factory().createDepthAttribute();
	depthAttribute->setTestingEnabled(false);
	depthAttribute->setWritingEnabled(false);
	backgroundAttributeSet->addAttribute(depthAttribute);

	backgroundTextures->setTexture(texture_, 0);
	backgroundAttributeSet->addAttribute(backgroundTextures);

	backgroundTriangleStrips->setVertexSet(backgroundVertexSet);

	registerDynamicUpdateObject();
}

GLESUndistortedBackground::~GLESUndistortedBackground()
{
	unregisterDynamicUpdateObject();
}

const Timestamp& GLESUndistortedBackground::cameraTimestamp() const
{
	return backgroundCameraTimestamp;
}

const SquareMatrix4& GLESUndistortedBackground::normalizedCameraFrustumMatrix() const
{
	return backgroundNormalizedCameraFrustumMatrix;
}

Texture2DRef GLESUndistortedBackground::texture() const
{
	return texture_;
}

Texture2DRef GLESUndistortedBackground::offsetTexture() const
{
	return backgroundOffsetTexture;
}

unsigned int GLESUndistortedBackground::horizontalElements() const
{
	return backgroundHorizontalElements;
}

unsigned int GLESUndistortedBackground::verticalElements() const
{
	return backgroundVerticalElements;
}

void GLESUndistortedBackground::setHorizontalElements(const unsigned int elements)
{
	if (elements < 1 || backgroundHorizontalElements == elements)
		return;

	backgroundHorizontalElements = elements;
	cameraChanged_ = true;
}

void GLESUndistortedBackground::setVerticalElements(const unsigned int elements)
{
	if (elements < 1 || backgroundVerticalElements == elements)
		return;

	backgroundVerticalElements = elements;
	cameraChanged_ = true;
}

bool GLESUndistortedBackground::setDisplayType(const DisplayType type)
{
	if (type == DT_FASTEST)
		return true;

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

	traverser.addRenderable(backgroundTriangleStrips, backgroundAttributeSet, HomogenousMatrix4(backgroundPosition, backgroundOrientation), normalMatrix, lights);
}

void GLESUndistortedBackground::onMediumCameraChanged(const Timestamp timestamp)
{
	backgroundCameraTimestamp = timestamp;

	rebuildPrimitive();
}

void GLESUndistortedBackground::rebuildPrimitive()
{
	ocean_assert(mediumCamera_);
	ocean_assert(backgroundHorizontalElements > 0u && backgroundVerticalElements > 0u);
	ocean_assert(backgroundDistance > Numeric::eps());

	const SquareMatrix4 frustumMatrix(mediumCamera_.frustumMatrix(Scalar(0.01), backgroundDistance));

	const Scalar scalingValues[16] =
	{
		texture_->imageTextureRatio().x() * Scalar(0.5), 0, 0, 0,
		0, texture_->imageTextureRatio().y() * Scalar(0.5), 0, 0,
		0, 0, Scalar(0.5), 0,
		texture_->imageTextureRatio().x() * Scalar(0.5), texture_->imageTextureRatio().y() * Scalar(0.5), Scalar(0.5), 1
	};

	const SquareMatrix4 scalingMatrix(scalingValues);

	backgroundNormalizedCameraFrustumMatrix = scalingMatrix * frustumMatrix * HomogenousMatrix4(backgroundPosition, backgroundOrientation).inverted();

	const Scalar left = -Numeric::tan(mediumCamera_.fovXLeft()) * backgroundDistance;
	const Scalar right = Numeric::tan(mediumCamera_.fovXRight()) * backgroundDistance;
	const Scalar top = Numeric::tan(mediumCamera_.fovYTop()) * backgroundDistance;
	const Scalar bottom = -Numeric::tan(mediumCamera_.fovYBottom()) * backgroundDistance;

	const Scalar horizontalStep = (right - left) / Scalar(backgroundHorizontalElements);
	const Scalar verticalStep = (top - bottom) / Scalar(backgroundVerticalElements);

	Vertices vertices;
	Normals normals;
	TextureCoordinates textureCoordinates;
	VertexIndexGroups strips(backgroundVerticalElements);
	unsigned int vertexCount = (unsigned int)(-1);

	for (unsigned int y = 0; y < backgroundVerticalElements; y++)
	{
		VertexIndices& strip = strips[y];

		const Scalar yTexelTop = Scalar(y) / Scalar(backgroundVerticalElements);
		const Scalar yTexelBottom = Scalar(y + 1) / Scalar(backgroundVerticalElements);

		const Scalar yPixelTop = Scalar(mediumCamera_.height() - 1) * yTexelTop;
		const Scalar yPixelBottom = Scalar(mediumCamera_.height() - 1) * yTexelBottom;

		ocean_assert(yPixelTop >= 0 && yPixelTop < Scalar(mediumCamera_.height()));
		ocean_assert(yPixelBottom >= 0 && yPixelBottom < Scalar(mediumCamera_.height()));

		for (unsigned int x = 0; x <= backgroundHorizontalElements; x++)
		{
			vertices.push_back(Vertex(left + Scalar(x) * horizontalStep, top - Scalar(y) * verticalStep, -backgroundDistance));
			vertices.push_back(Vertex(left + Scalar(x) * horizontalStep, top - Scalar(y + 1) * verticalStep, -backgroundDistance));

			const Scalar xTexel = Scalar(x) / Scalar(backgroundHorizontalElements);
			const Scalar xPixel = Scalar(mediumCamera_.width() - 1) * xTexel;
			ocean_assert(xPixel >= 0 && xPixel < Scalar(mediumCamera_.width()));

			const Vector2 undistortedTop(xPixel, yPixelTop);
			const Vector2 undistortedBottom(xPixel, yPixelBottom);

			const Vector2 distortedTop(mediumCamera_.distort<true>(undistortedTop));
			const Vector2 distortedBottom(mediumCamera_.distort<true>(undistortedBottom));

			const Scalar xDistortedTexelTop = distortedTop.x() / Scalar(mediumCamera_.width() - 1);
			const Scalar yDistortedTexelTop = distortedTop.y() / Scalar(mediumCamera_.height() - 1);

			const Scalar xDistortedTexelBottom = distortedBottom.x() / Scalar(mediumCamera_.width() - 1);
			const Scalar yDistortedTexelBottom = distortedBottom.y() / Scalar(mediumCamera_.height() - 1);

			textureCoordinates.push_back(TextureCoordinate(xDistortedTexelTop, 1 - yDistortedTexelTop));
			textureCoordinates.push_back(TextureCoordinate(xDistortedTexelBottom, 1 - yDistortedTexelBottom));

			strip.push_back(++vertexCount);
			strip.push_back(++vertexCount);
		}
	}

	normals.insert(normals.begin(), vertices.size(), Vector3(0, 0, 1));

	backgroundVertexSet->setTextureCoordinates(textureCoordinates, 0);
	backgroundVertexSet->setVertices(vertices);
	backgroundVertexSet->setNormals(normals);

	backgroundTriangleStrips->setVertexSet(backgroundVertexSet);
	backgroundTriangleStrips->setStrips(strips);
}

}

}

}
