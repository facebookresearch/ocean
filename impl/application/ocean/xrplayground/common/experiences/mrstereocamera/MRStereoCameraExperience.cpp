// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/mrstereocamera/MRStereoCameraExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/DepthAttribute.h"
#include "ocean/rendering/StereoAttribute.h"
#include "ocean/rendering/Textures.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

MRStereoCameraExperience::~MRStereoCameraExperience()
{
	// nothing to do here
}

bool MRStereoCameraExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	bool allCamerasAccessed = true;

	for (unsigned int cameraIndex = 0u; cameraIndex < 2u; ++cameraIndex)
	{
		const std::string mediumUrl = "LiveVideoId:" + String::toAString(cameraIndex);

		Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(mediumUrl);

		if (frameMedium)
		{
			frameMedium->start();

			Rendering::TransformRef transform = engine->factory().createTransform();

			experienceScene()->addChild(transform);

			renderingTransforms_.emplace_back(transform);
			frameMediums_.emplace_back(std::move(frameMedium));
		}
		else
		{
			Log::warning() << "Failed to access '" << mediumUrl << "'";
			allCamerasAccessed = false;
		}
	}

	if (!allCamerasAccessed)
	{
		showMessage(MT_CAMERA_ACCESS_FAILED);
	}

	return true;
}

bool MRStereoCameraExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	frameMediums_.clear();

	renderingTransforms_.clear();

	return true;
}

Timestamp MRStereoCameraExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(frameMediums_.size() == renderingTransforms_.size());

	for (size_t frameIndex = 0; frameIndex < frameMediums_.size(); ++frameIndex)
	{
		if ((visualizationMode_ == VM_LEFT_CAMERA_BOTH_EYES && frameIndex != 0)
				|| (visualizationMode_ == VM_RIGHT_CAMERA_BOTH_EYES && frameIndex != 1))
		{
			continue;
		}

		const Media::FrameMediumRef& frameMedium = frameMediums_[frameIndex];
		const Rendering::TransformRef& renderingTransform = renderingTransforms_[frameIndex];

		if (renderingTransform->numberChildren() == 0u)
		{
			SharedAnyCamera camera;
			FrameRef frame = frameMedium->frame(&camera); // latest frame

			if (frame && frame->isValid() && camera && camera->isValid())
			{
				const HomogenousMatrix4 device_T_camera(frameMedium->device_T_camera());

				const unsigned int width = frame->width();
				const unsigned int height = frame->height();

				ocean_assert(width != 0u && height != 0u);
				const Scalar invWidth = Scalar(1) / Scalar(width);
				const Scalar invHeight = Scalar(1) / Scalar(height);

				constexpr Scalar distance = 0.5;
				constexpr unsigned int gridSizePixels = 20u;

				const unsigned int xSteps = width / gridSizePixels + 1u;
				const unsigned int ySteps = height / gridSizePixels + 1u;

				Vectors3 vertices;
				vertices.reserve(xSteps * ySteps);

				Vectors2 textureCoordinates;
				textureCoordinates.reserve(xSteps * ySteps);

				Vectors3 normals;
				normals.reserve(xSteps * ySteps);

				Rendering::TriangleFaces triangleFaces;
				triangleFaces.reserve((xSteps - 1u) * (ySteps - 1u) * 2u);

				Indices32 validIndices;
				validIndices.reserve(xSteps * ySteps);

				for (unsigned int yStep = 0u; yStep < ySteps; ++yStep)
				{
					const Scalar y = Scalar(yStep * gridSizePixels);

					for (unsigned int xStep = 0u; xStep < xSteps; ++xStep)
					{
						const Vector2 imagePoint(Scalar(xStep * gridSizePixels), y);

						const Vector3 normal = camera->vector(imagePoint, true);
						const Vector2 projectedImagePoint = camera->projectToImage(normal);

						// often the camera model is not valid at the corners of the image
						// ensuring that the re-projection is matches that image point

						if (projectedImagePoint.sqrDistance(imagePoint) < Numeric::sqr(Scalar(0.1)))
						{
							validIndices.emplace_back(Index32(vertices.size()));

							vertices.emplace_back(device_T_camera * (normal * distance));
							textureCoordinates.emplace_back(imagePoint.x() * invWidth, Scalar(1) - imagePoint.y() * invHeight);

							normals.emplace_back(-normal);
						}
						else
						{
							validIndices.emplace_back(Index32(-1));
						}
					}
				}

				unsigned int index = 0u;

				for (unsigned int yStep = 0u; yStep < ySteps; ++yStep)
				{
					for (unsigned int xStep = 0u; xStep < xSteps; ++xStep)
					{
						if (yStep < ySteps - 1u && xStep < xSteps - 1u)
						{
							const Index32 index0 = validIndices[index];
							const Index32 index1 = validIndices[index + xSteps];
							const Index32 index2 = validIndices[index + xSteps + 1u];
							const Index32 index3 = validIndices[index + 1u];

							if (index0 != Index32(-1) && index1 != Index32(-1) && index2 != Index32(-1))
							{
								triangleFaces.emplace_back(index0, index1, index2);
							}

							if (index0 != Index32(-1) && index2 != Index32(-1) && index3 != Index32(-1))
							{
								triangleFaces.emplace_back(index0, index2, index3);
							}
						}

						++index;
					}
				}

				const Rendering::VertexSetRef vertexSet = engine->factory().createVertexSet();
				vertexSet->setVertices(vertices);
				vertexSet->setTextureCoordinates(textureCoordinates, 0);
				vertexSet->setNormals(normals);

				const Rendering::TrianglesRef triangles = engine->factory().createTriangles();
				triangles->setVertexSet(vertexSet);
				triangles->setFaces(triangleFaces);

				const Rendering::AttributeSetRef attributeSet = engine->factory().createAttributeSet();

				const Rendering::MediaTexture2DRef texture = engine->factory().createMediaTexture2D();
				texture->setMedium(frameMedium);
				texture->setMagnificationFilterMode(Rendering::Texture::MAG_MODE_LINEAR);

				const Rendering::TexturesRef textures = engine->factory().createTextures();
				textures->addTexture(texture);

				attributeSet->addAttribute(textures);

				if (visualizationMode_ == VM_BOTH_CAMERAS_INDIVIDUAL_EYES)
				{
					const Rendering::StereoAttributeRef stereoAttribute = engine->factory().createStereoAttribute();
					stereoAttribute->setStereoVisibility(frameIndex == 0u ? Rendering::StereoAttribute::SV_LEFT : Rendering::StereoAttribute::SV_RIGHT);

					attributeSet->addAttribute(stereoAttribute);
				}

				Rendering::DepthAttributeRef depthAttribute = engine->factory().createDepthAttribute();
				depthAttribute->setTestingEnabled(false);
				depthAttribute->setWritingEnabled(false);
				attributeSet->addAttribute(depthAttribute);

				const Rendering::GeometryRef geometry = engine->factory().createGeometry();
				geometry->addRenderable(triangles, attributeSet);

				renderingTransform->addChild(geometry);
			}
		}
		else
		{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
			const FrameRef frame = frameMedium->frame(); // latest frame

			if (frame && frame->isValid())
			{
				const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(frame->timestamp());

				if (world_T_device.isValid())
				{
					experienceScene()->setTransformation(world_T_device);
				}
			}
#endif
		}
	}

	return timestamp;
}

void MRStereoCameraExperience::onKeyPress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& key, const Timestamp /*timestamp*/)
{
	if (key == "A" || key == "X")
	{
		visualizationMode_ = VisualizationMode((visualizationMode_ + 1u) % VM_END);

		for (Rendering::TransformRef& transform : renderingTransforms_)
		{
			transform->clear();
		}
	}
}

std::unique_ptr<XRPlaygroundExperience> MRStereoCameraExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MRStereoCameraExperience());
}

}

}
