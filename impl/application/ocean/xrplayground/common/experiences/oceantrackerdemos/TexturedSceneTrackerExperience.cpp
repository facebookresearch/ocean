// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/TexturedSceneTrackerExperience.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/rendering/Textures.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

TexturedSceneTrackerExperience::MeshRenderingObject::MeshRenderingObject(const Rendering::Engine& engine)
{
	transform_ = engine.factory().createTransform();
}

void TexturedSceneTrackerExperience::MeshRenderingObject::update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors2& textureCoordinates, const Rendering::TriangleFaces& triangleFaces, Frame&& textureFrame)
{
	if (!vertexSet_)
	{
		vertexSet_ = engine.factory().createVertexSet();

		triangles_ = engine.factory().createTriangles();
		triangles_->setVertexSet(vertexSet_);

		texture_ = engine.factory().createFrameTexture2D();

		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const Rendering::TexturesRef textures = engine.factory().createTextures();
		textures->addTexture(texture_);
		attributeSet->addAttribute(textures);

		Rendering::GeometryRef geometry = engine.factory().createGeometry();
		geometry->addRenderable(triangles_, attributeSet);

		transform_->addChild(geometry);
	}

	ocean_assert(vertexSet_ && triangles_ && texture_);

	vertexSet_->setVertices(vertices);
	vertexSet_->setTextureCoordinates(textureCoordinates, 0u);

	triangles_->setFaces(triangleFaces);

	if (textureFrame)
	{
		texture_->setTexture(std::move(textureFrame));
	}
}

void TexturedSceneTrackerExperience::MeshRenderingObject::update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors3& perVertexNormals, const Rendering::TriangleFaces& triangleFaces)
{
	if (!vertexSet_)
	{
		vertexSet_ = engine.factory().createVertexSet();

		triangles_ = engine.factory().createTriangles();
		triangles_->setVertexSet(vertexSet_);

		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const Rendering::MaterialRef material = engine.factory().createMaterial();
		material->setDiffuseColor(RGBAColor(0.7f, 0.7f, 0.7f));
		attributeSet->addAttribute(material);

		Rendering::GeometryRef geometry = engine.factory().createGeometry();
		geometry->addRenderable(triangles_, attributeSet);

		transform_->addChild(geometry);
	}

	ocean_assert(vertexSet_ && triangles_);

	vertexSet_->setVertices(vertices);
	vertexSet_->setNormals(perVertexNormals);
	triangles_->setFaces(triangleFaces);
}

void TexturedSceneTrackerExperience::MeshRenderingObject::update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors3& perVertexNormals, const Indices32& triangleFaceIndices)
{
	Rendering::TriangleFaces triangleFaces;
	triangleFaces.reserve(triangleFaceIndices.size() / 3);

	for (size_t n = 0; n < triangleFaceIndices.size(); n += 3)
	{
		triangleFaces.emplace_back(triangleFaceIndices[n + 0], triangleFaceIndices[n + 1], triangleFaceIndices[n + 2]);
	}

	update(engine, vertices, perVertexNormals, triangleFaces);
}

void TexturedSceneTrackerExperience::MeshRenderingObject::update(const Rendering::Engine& engine, const Vectors3& vertices, const Rendering::TriangleFaces& triangleFaces)
{
	Vectors3 normals;
	normals.reserve(triangleFaces.size() * 3);

	for (const Rendering::TriangleFace& triangleFace : triangleFaces)
	{
		const Vector3& vertex0 = vertices[triangleFace[0]];
		const Vector3& vertex1 = vertices[triangleFace[1]];
		const Vector3& vertex2 = vertices[triangleFace[2]];

		const Vector3 normal = (vertex1 - vertex0).cross(vertex2 - vertex0).normalizedOrZero();

		normals.emplace_back(normal);
		normals.emplace_back(normal);
		normals.emplace_back(normal);
	}

	update(engine, vertices, normals, triangleFaces);
}

TexturedSceneTrackerExperience::~TexturedSceneTrackerExperience()
{
	// nothing to do here
}

bool TexturedSceneTrackerExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Scene Tracker");
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Scene Tracker");
#endif

	if (!sceneTracker6DOF_)
	{
		Log::error() << "Scene Tracker Experience could not access the scene tracker";
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(sceneTracker6DOF_);
	if (visualTracker && !engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			frameMedium_ = undistortedBackground->medium();

			if (frameMedium_)
			{
				visualTracker->setInput(frameMedium_);
			}
		}
	}

	sceneTrackerSampleEventSubscription_ = sceneTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &TexturedSceneTrackerExperience::onSceneTrackerSample));

	// let's cover the video background with an almost opaque sphere
	Rendering::AttributeSetRef sphereAttributeSet;
	experienceScene()->addChild(Rendering::Utilities::createSphere(engine, Scalar(100), RGBAColor(0.0f, 0.0f, 0.0f, 0.8f), nullptr, &sphereAttributeSet));
	Rendering::PrimitiveAttributeRef spherePrimitiveAttributeSet = engine->factory().createPrimitiveAttribute();
	spherePrimitiveAttributeSet->setCullingMode(Rendering::PrimitiveAttribute::CULLING_NONE);
	spherePrimitiveAttributeSet->setLightingMode(Rendering::PrimitiveAttribute::LM_TWO_SIDED_LIGHTING);
	sphereAttributeSet->addAttribute(spherePrimitiveAttributeSet);

	if (!anchoredContentManager_.initialize(std::bind(&TexturedSceneTrackerExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (userInterface.isIOS())
	{
		showUserInterinterfaceIOS(userInterface);
	}
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	return true;
}

bool TexturedSceneTrackerExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	stopThreadExplicitly();

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (userInterface.isIOS())
	{
		unloadUserInterinterfaceIOS(userInterface);
	}
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	sceneTrackerSampleEventSubscription_.release();

	renderingGroup_.release();

	anchoredContentManager_.release();

	sceneTracker6DOF_.release();

	return true;
}

Timestamp TexturedSceneTrackerExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock outerScopedLock(lock_);
		const Devices::SceneTracker6DOF::ObjectId objectId(objectId_);

		const Timestamp recentSceneElementTimestamp(recentSceneElementTimestamp_);
		recentSceneElementTimestamp_.toInvalid();

		const HomogenousMatrix4 world_T_recentCamera(world_T_recentCamera_);
		world_T_recentCamera_.toNull();
	outerScopedLock.release();

	if (!renderingGroup_ && objectId != Devices::SceneTracker6DOF::invalidObjectId())
	{
		renderingGroup_ = engine->factory().createGroup();

		constexpr Scalar visibilityRadius = Scalar(1000); // 1km
		constexpr Scalar engagementRadius = Scalar(10000);
		anchoredContentManager_.addContent(renderingGroup_, sceneTracker6DOF_, objectId, visibilityRadius, engagementRadius);
	}

	if (frameMedium_)
	{
		TemporaryScopedLock innerScopedLock(lock_);

		ocean_assert(processorState_ != PS_PROCESS_FRAME);
		if ((processorState_ == PS_IDLE || processorState_ == PS_MESH_UPDATED) && recentSceneElementTimestamp > lastProcessedFrameTimestamp_)
		{
			processorState_ = PS_PROCESS_FRAME;

			innerScopedLock.release();

			SharedAnyCamera anyCamera;
			const FrameRef frame = frameMedium_->frame(recentSceneElementTimestamp, &anyCamera);

			if (frame && anyCamera && frame->timestamp() == recentSceneElementTimestamp)
			{
				Frame rgbFrame;
				if (CV::FrameConverter::Comfort::convert(*frame, FrameType(*frame, FrameType::FORMAT_RGB24), rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
				{
					textureGenerator_.processFrame(std::move(rgbFrame), *anyCamera, world_T_recentCamera, engine);
				}

				lastProcessedFrameTimestamp_ = recentSceneElementTimestamp;
			}

			innerScopedLock.relock(lock_);

			processorState_ = PS_IDLE;
		}
	}

	if (renderingGroup_)
	{
		constexpr double updateInterval = 0.25;

		if (renderingObjectTimestamp_.isInvalid() || timestamp >= renderingObjectTimestamp_ + updateInterval)
		{
			TemporaryScopedLock innerScopedLock(lock_);

			if (processorState_ == PS_IDLE || processorState_ == PS_MESH_UPDATED)
			{
				const Indices32 meshIds = textureGenerator_.meshIds();

				if (!meshIds.empty())
				{
					processorState_ = PS_MESH_EXPORTING;
					innerScopedLock.release();

					Tracking::MapTexturing::TextureGenerator::Mesh mesh;
					Frame meshTexture;

					for (const Index32& meshId : meshIds)
					{
						if (textureGenerator_.exportMesh(meshId, mesh, meshTexture))
						{
							MeshRenderingObjectMap::iterator iMesh = meshRenderingObjectMap_.find(meshId);
							if (iMesh == meshRenderingObjectMap_.cend())
							{
								iMesh = meshRenderingObjectMap_.emplace(meshId, *engine).first;
								renderingGroup_->addChild(iMesh->second.transform());
							}

							if (meshTexture.isValid())
							{
								iMesh->second.update(*engine, mesh.vertices_, mesh.textureCoordinates_, mesh.triangleFaces_, std::move(meshTexture));
							}
							else
							{
								iMesh->second.update(*engine, mesh.vertices_, mesh.triangleFaces_);
							}
						}
					}

					innerScopedLock.relock(lock_);

					processorState_ = PS_IDLE;
				}
			}

			renderingObjectTimestamp_ = timestamp;
		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	if (processorState_ != PS_STOPPED)
	{
		if (Media::LiveVideoRef liveVideo = frameMedium_)
		{
			const double exposureDuration = liveVideo->exposureDuration();

			if (exposureDuration == 0.0)
			{
				// setting a fixed exposure and white balance
				liveVideo->setExposureDuration(-1.0);
			}
		}
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

bool TexturedSceneTrackerExperience::start()
{
	const ScopedLock scopedLock(lock_);

	if (processorState_ != PS_STOPPED)
	{
		return false;
	}

	if (!sceneTracker6DOF_->start())
	{
		Log::error() << "Scene Tracker Experience could not start the scene tracker";
		return false;
	}

	processorState_ = PS_IDLE;

	startThread();

	return true;
}

bool TexturedSceneTrackerExperience::stop()
{
	const ScopedLock scopedLock(lock_);

	if (processorState_ == PS_STOPPED)
	{
		return false;
	}

	stopThread();

	processorState_ = PS_STOPPED;

	return true;
}

bool TexturedSceneTrackerExperience::exportMesh(const std::string& path)
{
	const ScopedLock scopedLock(lock_);

	return textureGenerator_.exportMeshs(path, true);
}

std::unique_ptr<XRPlaygroundExperience> TexturedSceneTrackerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new TexturedSceneTrackerExperience());
}

void TexturedSceneTrackerExperience::threadRun()
{
	while (!shouldThreadStop())
	{
		TemporaryScopedLock scopedLock(lock_);

			ocean_assert(processorState_ != PS_UPDATING_MESH);
			if (processorState_ != PS_IDLE)
			{
				scopedLock.release();

				sleep(1u);
				continue;
			}

			const Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement(std::move(recentSceneElement_));

			if (!recentSceneElement)
			{
				scopedLock.release();

				sleep(1u);
				continue;
			}

			processorState_ = PS_UPDATING_MESH;

		scopedLock.release();

		ocean_assert(recentSceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_MESHES);

		textureGenerator_.updateMesh(recentSceneElement, true);

		scopedLock.relock(lock_);
			ocean_assert(processorState_ == PS_UPDATING_MESH);
			processorState_ = PS_MESH_UPDATED;
		scopedLock.release();

		sleep(5u);
	}
}

void TexturedSceneTrackerExperience::onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample(sample);

	if (!sceneTrackerSample || sceneTrackerSample->sceneElements().empty())
	{
		return;
	}

	for (size_t n = 0; n < sceneTrackerSample->objectIds().size(); ++n)
	{
		const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneTrackerSample->sceneElements()[n];

		if (sceneElement && sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_MESHES)
		{
			const ScopedLock scopedLock(lock_);

			recentSceneElement_ = sceneElement;
			objectId_ = sceneTrackerSample->objectIds()[n];

			recentSceneElementTimestamp_ = sceneTrackerSample->timestamp();

			world_T_recentCamera_ = HomogenousMatrix4(sceneTrackerSample->positions().front(), sceneTrackerSample->orientations().front());

			break;
		}
	}
}

void TexturedSceneTrackerExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

}

}
