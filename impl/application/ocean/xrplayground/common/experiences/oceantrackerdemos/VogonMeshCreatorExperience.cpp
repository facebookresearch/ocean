// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/VogonMeshCreatorExperience.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/math/SophusUtilities.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/scenedescription/sdx/x3d/Utilities.h"

#include <reconstruction/systems/DefaultFusionSystem.h>

namespace Ocean
{

namespace XRPlayground
{

VogonMeshCreatorExperience::~VogonMeshCreatorExperience()
{
	// nothing to do here
}

bool VogonMeshCreatorExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Depth Tracker");
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Depth Tracker");
#endif

	if (!sceneTracker6DOF_)
	{
		Log::error() << "VOGON Mesh Creator Tracker Experience could not access the depth tracker";
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

	sceneTrackerSampleEventSubscription_ = sceneTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &VogonMeshCreatorExperience::onSceneTrackerSample));

	if (!anchoredContentManager_.initialize(std::bind(&VogonMeshCreatorExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (userInterface.isIOS())
	{
		showUserInterinterfaceIOS(userInterface);
	}
#else
	// no GUI on Android platforms
	start(0.03f, false /*createPerVertexColors*/);
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	return true;
}

bool VogonMeshCreatorExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
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

Timestamp VogonMeshCreatorExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (isActive_)
	{
		TemporaryScopedLock scopedLock(lock_);
			const Devices::SceneTracker6DOF::ObjectId objectId(objectId_);

			Rendering::TriangleFaces recentTriangleFaces(std::move(recentTriangleFaces_));
			Vectors3 recentVertices(std::move(recentVertices_));
			Vectors3 recentPerVertexNormals(std::move(recentPerVertexNormals_));
			RGBAColors recentPerVertexColors(std::move(recentPerVertexColors_));
		scopedLock.release();

		if (!renderingGroup_ && objectId != Devices::SceneTracker6DOF::invalidObjectId())
		{
			renderingGroup_ = engine->factory().createGroup();

			constexpr Scalar visibilityRadius = Scalar(1000); // 1km
			constexpr Scalar engagementRadius = Scalar(10000);
			anchoredContentManager_.addContent(renderingGroup_, sceneTracker6DOF_, objectId, visibilityRadius, engagementRadius);
		}

		if (renderingGroup_ && !recentTriangleFaces.empty())
		{
			renderingGroup_->clear();

			Rendering::VertexSetRef vertexSet;

			switch (renderMode_)
			{
				case RM_MESH:
					renderingGroup_->addChild(Rendering::Utilities::createMesh(engine, recentVertices, recentTriangleFaces, RGBAColor(0.7f, 0.7f, 0.7f), recentPerVertexNormals, Vectors2(), Frame(), Media::FrameMediumRef(), nullptr, nullptr, &vertexSet));
					if (vertexSet)
					{
						vertexSet->setColors(recentPerVertexColors);
					}
					break;

				case RM_WIREFRAME:
					renderingGroup_->addChild(Rendering::Utilities::createLines(*engine, recentVertices, recentTriangleFaces, RGBAColor(0.7f, 0.7f, 0.7f), recentPerVertexColors, nullptr, nullptr, nullptr, &vertexSet));
					break;

				case RM_POINT_CLOUD:
					renderingGroup_->addChild(Rendering::Utilities::createPoints(*engine, recentVertices, RGBAColor(0.7f, 0.7f, 0.7f), Scalar(5), RGBAColors(), nullptr, nullptr, nullptr, &vertexSet));
					break;

				case RM_END:
					ocean_assert(false && "This should never happen!");
					break;
			};


		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

void VogonMeshCreatorExperience::onMouseRelease(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& /*screenPosition*/, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
	renderMode_ = RenderMode((renderMode_ + 1u) % RM_END);
}

bool VogonMeshCreatorExperience::start(const float voxelSize, const bool createPerVertexColors)
{
	if (voxelSize_ <= 0.0f || voxelSize_ > 0.1f)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (isActive_)
	{
		return false;
	}

	if (!sceneTracker6DOF_->start())
	{
		Log::error() << "Vogon Mesh Creator Experience could not start the scene tracker";
		return false;
	}

	voxelSize_ = voxelSize;
	createPerVertexColors_ = createPerVertexColors;
	isActive_ = true;

	startThread();

	return true;
}

bool VogonMeshCreatorExperience::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!isActive_)
	{
		return false;
	}

	isActive_ = false;

	stopThread();

	return true;
}

bool VogonMeshCreatorExperience::exportMesh(const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

	std::ofstream stream(filename, std::ios::binary);

	if (!stream.good())
	{
		return false;
	}

	stream << "#X3D V3.0 utf8\n\n#Created with XRPlayground, Vogon Mesh Creator Experience\n\n";

	if (recentVertices_.empty())
	{
		return false;
	}

	if (!SceneDescription::SDX::X3D::Utilities::writeIndexedFaceSet(stream, recentVertices_, recentTriangleFaces_, RGBAColor(0.7f, 0.7f, 0.7f), recentPerVertexNormals_, recentPerVertexColors_))
	{
		return false;
	}

	stream << "\n";

	return true;
}

std::unique_ptr<XRPlaygroundExperience> VogonMeshCreatorExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new VogonMeshCreatorExperience());
}

void VogonMeshCreatorExperience::onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample(sample);

	if (!sceneTrackerSample || sceneTrackerSample->sceneElements().empty())
	{
		return;
	}

	for (size_t n = 0; n < sceneTrackerSample->objectIds().size(); ++n)
	{
		const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneTrackerSample->sceneElements()[n];

		if (sceneElement && sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_DEPTH)
		{
			const ScopedLock scopedLock(lock_);

			objectId_ = sceneTrackerSample->objectIds()[n];

			break;
		}
	}
}

void VogonMeshCreatorExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

void VogonMeshCreatorExperience::threadRun()
{
	ocean_assert(sceneTracker6DOF_);
	ocean_assert(frameMedium_);
	ocean_assert(voxelSize_ > 0.0f && voxelSize_ <= 0.1f);

	vogon::DefaultFusionSystem vogonFusionSystem(voxelSize_, createPerVertexColors_);

	Timestamp lastSampleTimestamp(false);
	Timestamp nextMeshTimestamp(false);

	HighPerformanceStatistic performanceFuse;
	HighPerformanceStatistic performanceMesh;

	while (!isActive_ || !shouldThreadStop()) // in case isActive_ == false, we make one final mesh extraction
	{
		const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample = sceneTracker6DOF_->sample();

		if (!sceneTrackerSample || sceneTrackerSample->timestamp() <= lastSampleTimestamp || sceneTrackerSample->sceneElements().size() != 1)
		{
			sleep(1u);
			continue;
		}

		lastSampleTimestamp = sceneTrackerSample->timestamp();

		const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneTrackerSample->sceneElements().front();

		if (sceneElement == nullptr || sceneElement->sceneElementType() != Devices::SceneTracker6DOF::SceneElement::SET_DEPTH)
		{
			sleep(1u);
			continue;
		}

		const HomogenousMatrix4 world_T_camera(sceneTrackerSample->positions().front(), sceneTrackerSample->orientations().front());
		const HomogenousMatrix4 world_T_flippedCamera(PinholeCamera::flippedTransformationRightSide(world_T_camera));

		const Devices::SceneTracker6DOF::SceneElementDepth& sceneElementDepth = (const Devices::SceneTracker6DOF::SceneElementDepth&)(*sceneElement);

		const SharedAnyCamera depthCamera = sceneElementDepth.camera();

		std::shared_ptr<Frame> confidence;
		const std::shared_ptr<Frame> depth = sceneElementDepth.depth(&confidence);

		ocean_assert(depthCamera && depth);

		// if available, we use the confidence map to remove all depth values not associated with highest confidence

		Frame filteredDepth(*depth, Frame::ACM_USE_KEEP_LAYOUT);

		if (confidence && confidence->width() == depth->width() && confidence->height() == depth->height() && confidence->isPixelFormatCompatible(FrameType::FORMAT_Y8))
		{
			filteredDepth.set(depth->frameType(), true, true);

			for (unsigned int y = 0u; y < filteredDepth.height(); ++y)
			{
				const uint8_t* const confidenceRow = confidence->constrow<uint8_t>(y);
				const float* const sourceDepthRow = depth->constrow<float>(y);
				float* const targetDepthRow = filteredDepth.row<float>(y);

				for (unsigned int x = 0u; x < filteredDepth.width(); ++x)
				{
					if (confidenceRow[x] >= 2u)
					{
						targetDepthRow[x] = sourceDepthRow[x];
					}
					else
					{
						targetDepthRow[x] = 0.0f; // VOGON expects 0 for an unknown depth
					}
				}
			}
		}

		const perception::ConstImageSlice32f depthSlice(filteredDepth.constdata<float>(), int(filteredDepth.width()), int(filteredDepth.height()), int(filteredDepth.strideElements()));
		const Eigen::Vector4f depthCameraParameters = Eigen::Vector4f(float(depthCamera->focalLengthX()), float(depthCamera->focalLengthY()), float(depthCamera->principalPointX()), float(depthCamera->principalPointY()));

		const Sophus::SE3f poseWorldFromFlippedCamera(SophusUtilities::toSE3<Scalar, float>(world_T_flippedCamera));

		if (createPerVertexColors_)
		{
			SharedAnyCamera colorCamera;
			const FrameRef colorFrame = frameMedium_->frame(lastSampleTimestamp, &colorCamera);
			if (colorFrame.isNull() || colorFrame->timestamp() != lastSampleTimestamp)
			{
				Log::warning() << "Missing frame for sample";
				continue;
			}

			Frame rgbFrame;
			if (!CV::FrameConverter::Comfort::convert(*colorFrame, FrameType(*colorFrame, FrameType::FORMAT_RGB24), rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
			{
				ocean_assert(false && "This should never happen!");
				continue;
			}

			ocean_assert(rgbFrame.isContinuous());
			const perception::ConstImageSlice8uC3 colorSlice((const perception::Pixel8uC3*)(rgbFrame.constdata<uint8_t>()), int(rgbFrame.width()), int(rgbFrame.height()), int(rgbFrame.width()));
			const Eigen::Vector4f colorCameraParameters = Eigen::Vector4f(float(colorCamera->focalLengthX()), float(colorCamera->focalLengthY()), float(colorCamera->principalPointX()), float(colorCamera->principalPointY()));

			performanceFuse.start();
				vogonFusionSystem.fuse(depthSlice, depthCameraParameters, poseWorldFromFlippedCamera, colorSlice, colorCameraParameters, poseWorldFromFlippedCamera, 0.1f, 3.0f);
			performanceFuse.stop();
		}
		else
		{
			performanceFuse.start();
				vogonFusionSystem.fuse(depthSlice, depthCameraParameters, poseWorldFromFlippedCamera, 0.1f, 3.0f);
			performanceFuse.stop();
		}

		if (!isActive_ || lastSampleTimestamp >= nextMeshTimestamp)
		{
			std::vector<Eigen::Vector3f> vertices;
			std::vector<int> triangleIndices;
			std::vector<Eigen::Vector3<uint8_t>> perVertexColors;
			std::vector<Eigen::Vector3f> vertexNormals;

			performanceMesh.start();
				vogonFusionSystem.generateMesh();

				if (createPerVertexColors_)
				{
					vogonFusionSystem.getColoredMesh(vertices, triangleIndices, perVertexColors, &vertexNormals);
				}
				else
				{
					vogonFusionSystem.getMesh(vertices, triangleIndices, &vertexNormals);
				}

				ocean_assert(vertices.size() == vertexNormals.size());
				ocean_assert(triangleIndices.size() % 3 == 0);
			performanceMesh.stop();

			Rendering::TriangleFaces recentTriangleFaces;
			recentTriangleFaces.reserve(triangleIndices.size() / 3);

			for (size_t n = 0; n < triangleIndices.size(); n += 3)
			{
				recentTriangleFaces.emplace_back(Index32(triangleIndices[n + 0]), Index32(triangleIndices[n + 1]), Index32(triangleIndices[n + 2]));
			}

			Vectors3 recentVertices;
			recentVertices.reserve(vertices.size());

			for (const Eigen::Vector3f& vertex : vertices)
			{
				recentVertices.emplace_back(Scalar(vertex.x()), Scalar(vertex.y()), Scalar(vertex.z()));
			}

			Vectors3 recentPerVertexNormals;
			recentPerVertexNormals.reserve(vertexNormals.size());

			for (const Eigen::Vector3f& vertexNormal : vertexNormals)
			{
				recentPerVertexNormals.emplace_back(Scalar(vertexNormal.x()), Scalar(vertexNormal.y()), Scalar(vertexNormal.z()));
			}

			RGBAColors recentPerVertexColors;

			if (!perVertexColors.empty())
			{
				recentPerVertexColors.reserve(perVertexColors.size());

				constexpr float inv255 = 1.0f / 255.0f;
				for (const Eigen::Vector3<uint8_t>& perVertexColor : perVertexColors)
				{
					recentPerVertexColors.emplace_back(float(perVertexColor.x()) * inv255, float(perVertexColor.y()) * inv255, float(perVertexColor.z()) * inv255);
				}
			}

			nextMeshTimestamp = lastSampleTimestamp + 1.0;

			const ScopedLock scopedLock(lock_);

			recentTriangleFaces_ = std::move(recentTriangleFaces);
			recentVertices_ = std::move(recentVertices);
			recentPerVertexNormals_ = std::move(recentPerVertexNormals);
			recentPerVertexColors_ = std::move(recentPerVertexColors);

			if (performanceMesh.measurements() % 10u == 0u)
			{
				Log::info() << "Performance fuse: " << performanceFuse.percentileMseconds(0.95) << "ms, mesh: " << performanceMesh.percentileMseconds(0.95) << "ms, for " << recentVertices_.size() << " vertices";
			}

			if (!isActive_)
			{
				// we have the latest state of the mesh and the user has stopped the meshing process
				break;
			}
		}
	}
}

}

}
