// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/ObjectCaptureExperience.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/io/File.h"

#include "ocean/io/image/Image.h"

#include "ocean/math/SophusUtilities.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/scenedescription/sdx/x3d/Utilities.h"

#include <mr/obcap/ObjectCapturePipeline.h>

#include <reconstruction/meshing/TriangleMesh.h>
#include <reconstruction/meshing/TriangleMeshStructured.h>

namespace Ocean
{

namespace XRPlayground
{

ObjectCaptureExperience::~ObjectCaptureExperience()
{
	// nothing to do here
}

bool ObjectCaptureExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Depth Tracker");
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Depth Tracker");
#endif

	if (!sceneTracker6DOF_)
	{
		Log::error() << "Object Capture Experience could not access the depth tracker";
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

	sceneTrackerSampleEventSubscription_ = sceneTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &ObjectCaptureExperience::onSceneTrackerSample));

	// let's cover the video background with an almost opaque sphere
	Rendering::AttributeSetRef sphereAttributeSet;
	experienceScene()->addChild(Rendering::Utilities::createSphere(engine, Scalar(100), RGBAColor(0.0f, 0.0f, 0.0f, 0.8f), nullptr, &sphereAttributeSet));
	Rendering::PrimitiveAttributeRef spherePrimitiveAttributeSet = engine->factory().createPrimitiveAttribute();
	spherePrimitiveAttributeSet->setCullingMode(Rendering::PrimitiveAttribute::CULLING_NONE);
	spherePrimitiveAttributeSet->setLightingMode(Rendering::PrimitiveAttribute::LM_TWO_SIDED_LIGHTING);
	sphereAttributeSet->addAttribute(spherePrimitiveAttributeSet);

	if (!anchoredContentManager_.initialize(std::bind(&ObjectCaptureExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
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

bool ObjectCaptureExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
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

Timestamp ObjectCaptureExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (isActive_)
	{
		if (Media::LiveVideoRef liveVideo = frameMedium_)
		{
			const double exposureDuration = liveVideo->exposureDuration();

			if (exposureDuration == 0.0)
			{
				Log::info() << "Setting a fixed exposure and white balance";

				liveVideo->setExposureDuration(-1.0);
			}
		}
	}

	if (isActive_ || isFinalReconstruction_)
	{
		Rendering::TriangleFaces recentTriangleFaces;
		Vectors3 recentVertices;
		Vectors3 recentPerVertexNormals;
		RGBAColors recentPerVertexColors;

		Frame finalTextureAtlas;
		Vectors2 finalTextureCoordinates;

		TemporaryScopedLock scopedLock(lock_);
			const Devices::SceneTracker6DOF::ObjectId objectId(objectId_);

			if (!isFinalReconstruction_)
			{
				recentTriangleFaces = std::move(recentTriangleFaces_);
				recentVertices = std::move(recentVertices_);
				recentPerVertexNormals = std::move(recentPerVertexNormals_);
				recentPerVertexColors = std::move(recentPerVertexColors_);
			}
			else
			{
				recentTriangleFaces = Rendering::TriangleFaces(recentTriangleFaces_.begin(), recentTriangleFaces_.end());
				recentVertices = Vectors3(recentVertices_.begin(), recentVertices_.end());
				recentPerVertexNormals = Vectors3(recentPerVertexNormals_.begin(), recentPerVertexNormals_.end());
				recentPerVertexColors = RGBAColors(recentPerVertexColors_.begin(), recentPerVertexColors_.end());

				finalTextureAtlas = Frame(finalTextureAtlas_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
				finalTextureCoordinates = Vectors2(finalTextureCoordinates_.begin(), finalTextureCoordinates_.end());
			}
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

			const RGBAColor defaultColor = (finalTextureAtlas.isValid() || !recentPerVertexColors.empty()) ? RGBAColor(false) : RGBAColor(0.7f, 0.7f, 0.7f);
			Rendering::VertexSetRef vertexSet;
			renderingGroup_->addChild(Rendering::Utilities::createMesh(engine, recentVertices, recentTriangleFaces, defaultColor, recentPerVertexNormals, finalTextureCoordinates, std::move(finalTextureAtlas), Media::FrameMediumRef(), nullptr, nullptr, &vertexSet));

			if (vertexSet && !isFinalReconstruction_)
			{
				vertexSet->setColors(recentPerVertexColors);
			}
		}

		if (isFinalReconstruction_)
		{
			isFinalReconstruction_ = false;
		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

bool ObjectCaptureExperience::start(const unsigned int objectSize)
{
	if (objectSize >= 3u)
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
		Log::error() << "Object Capture Experience could not start the scene tracker";
		return false;
	}

	objectSize_ = objectSize;
	isActive_ = true;

	startThread();

	return true;
}

bool ObjectCaptureExperience::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!isActive_)
	{
		return false;
	}

	isActive_ = false;

	// not calling stopThread() here because the thread will have to wait for the results from object capture

	return true;
}

bool ObjectCaptureExperience::exportMesh(const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

	std::ofstream stream(filename, std::ios::binary);

	if (!stream.good())
	{
		return false;
	}

	stream << "#X3D V3.0 utf8\n\n#Created with XRPlayground, Object Capture Experience\n\n";

	if (recentVertices_.empty())
	{
		return false;
	}

	const IO::File textureFilename(IO::File(filename).base() + "_texture.png");

	if (!SceneDescription::SDX::X3D::Utilities::writeIndexedFaceSet(stream, recentVertices_, recentTriangleFaces_, RGBAColor(false), recentPerVertexNormals_, recentPerVertexColors_, finalTextureCoordinates_, textureFilename.name()))
	{
		return false;
	}

	stream << "\n";

	if (!IO::Image::writeImage(finalTextureAtlas_, textureFilename()))
	{
		return false;
	}

	return true;
}

std::unique_ptr<XRPlaygroundExperience> ObjectCaptureExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new ObjectCaptureExperience());
}

void ObjectCaptureExperience::onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
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

void ObjectCaptureExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

void ObjectCaptureExperience::threadRun()
{
	ocean_assert(sceneTracker6DOF_);
	ocean_assert(frameMedium_);
	ocean_assert(objectSize_ < 3u);

	constexpr mr::ObjectCapturePipelineBase::ObjectType objectType[3] =
	{
		mr::ObjectCapturePipelineBase::ObjectType::Small,
		mr::ObjectCapturePipelineBase::ObjectType::Medium,
		mr::ObjectCapturePipelineBase::ObjectType::Large,
	};

	mr::ObjectCapturePipeline objectCapturePipeline(objectType[objectSize_]);
	objectCapturePipeline.start();

	Timestamp lastSampleTimestamp(false);
	Timestamp nextMeshTimestamp(false);

	HighPerformanceStatistic performanceFuse;
	HighPerformanceStatistic performanceMesh;

	while (isActive_ && !shouldThreadStop())
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
						targetDepthRow[x] = 0.0f; // Object capture uses VOGON, which expects 0 for an unknown depth
					}
				}
			}
		}

		const perception::ConstImageSlice32f depthSlice(filteredDepth.constdata<float>(), int(filteredDepth.width()), int(filteredDepth.height()), int(filteredDepth.strideElements()));
		const Eigen::Vector4f depthCameraParameters = Eigen::Vector4f(float(depthCamera->focalLengthX()), float(depthCamera->focalLengthY()), float(depthCamera->principalPointX()), float(depthCamera->principalPointY()));

		const Sophus::SE3f poseWorldFromFlippedCamera(SophusUtilities::toSE3<Scalar, float>(world_T_flippedCamera));

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

		ocean_assert(double(lastSampleTimestamp) >= 0.0);
		const uint64_t lastSampleTimestampNs = uint64_t(lastSampleTimestamp.nanoseconds());
		const float frameExposureSeconds = 1.0f / 30.0f; // **TODO** Is this information available somewhere?

		performanceFuse.start();
			objectCapturePipeline.addDepthAndColorData(depthSlice, depthCameraParameters, poseWorldFromFlippedCamera, colorSlice, colorCameraParameters, poseWorldFromFlippedCamera, lastSampleTimestampNs, frameExposureSeconds);
		performanceFuse.stop();

		if (lastSampleTimestamp >= nextMeshTimestamp)
		{
			std::shared_ptr<const vogon::MeshBase> mesh;

			performanceMesh.start();
				mesh = objectCapturePipeline.getMesh();

				ocean_assert(mesh != nullptr);
				ocean_assert(mesh->vertexCount() == mesh->vertexNormalCount());
				ocean_assert(mesh->vertexCount() == mesh->vertexColorCount() || mesh->vertexColorCount() == 0);
				ocean_assert(mesh->faceIndexCount() % 3 == 0);
			performanceMesh.stop();

			Rendering::TriangleFaces recentTriangleFaces;
			Vectors3 recentVertices;
			Vectors3 recentPerVertexNormals;
			RGBAColors recentPerVertexColors;

			if (mesh == nullptr || !extractFromVogonMesh(*mesh, recentVertices, recentTriangleFaces, recentPerVertexNormals, &recentPerVertexColors))
			{
				Log::error() << "Failed to extract the mesh information";

				continue;
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

	std::future<void> finishedFuture = objectCapturePipeline.finishAsync();

	const Timestamp startWaitTimestamp(true);

	while (finishedFuture.valid() && !shouldThreadStop())
	{
		Log::info() << "Waiting for the refined mesh from object capture ...";

		if (finishedFuture.wait_for(std::chrono::seconds(1)) == std::future_status::ready && (objectCapturePipeline.getState() == mr::ObjectCapturePipelineBase::State::Finished || objectCapturePipeline.getState() == mr::ObjectCapturePipelineBase::State::Stopped))
		{
			finishedFuture.get();

			Log::info() << "Refined mesh became available after " << String::toAString(double(Timestamp(true) - startWaitTimestamp), 1u) << " seconds.";

			vogon::TriangleMesh triangleMesh;

			objectCapturePipeline.getMesh(triangleMesh.vertices(), triangleMesh.vertexNormals(), triangleMesh.faceIndices());

			perception::Image8uC3 textureAtlasSlice;
			std::vector<Eigen::Vector2f> textureCoordinates;
			std::vector<uint32_t> textureIndices;

			objectCapturePipeline.getTexture(textureCoordinates, textureIndices, textureAtlasSlice);

			// Removing untextured triangles.
			mr::ObjectCapturePipelineBase::removeUntexturedFaces(triangleMesh.faceIndices(), textureIndices);

			// Convert texture coordinates from 3 coordinates per triangle to 1 coordinate per vertex
			std::pair<vogon::TriangleMesh, std::vector<Eigen::Vector2f>> updatedMeshAndTextureCoordinates = triangleMesh.splitVerticesWithMultipleUv(textureCoordinates, textureIndices);

			const vogon::TriangleMesh& updatedMesh = updatedMeshAndTextureCoordinates.first;
			const std::vector<Eigen::Vector2f>& updatedTextureCoordinates = updatedMeshAndTextureCoordinates.second;

			Rendering::TriangleFaces recentTriangleFaces;
			Vectors3 recentVertices;
			Vectors3 recentPerVertexNormals;

			if (extractFromVogonMesh(updatedMesh, recentVertices, recentTriangleFaces, recentPerVertexNormals, /* vertexColors */ nullptr))
			{
				unsigned int paddingElements = 0u;
				if (!Ocean::Frame::strideBytes2paddingElements(FrameType::FORMAT_RGB24, textureAtlasSlice.width(), textureAtlasSlice.strideInBytes(), paddingElements))
				{
					ocean_assert(false && "This should never happen!");
				}

				Frame finalTextureAtlas(FrameType(textureAtlasSlice.width(), textureAtlasSlice.height(), FrameType::FORMAT_RGB24, Ocean::FrameType::ORIGIN_UPPER_LEFT), textureAtlasSlice.data(), Frame::CM_COPY_REMOVE_PADDING_LAYOUT, paddingElements, lastSampleTimestamp);

				Vectors2 finalTextureCoordinates;
				finalTextureCoordinates.reserve(updatedTextureCoordinates.size());

				for (const Eigen::Vector2f& updatedTextureCoordinate : updatedTextureCoordinates)
				{
					finalTextureCoordinates.emplace_back(Scalar(updatedTextureCoordinate.x()), Scalar(updatedTextureCoordinate.y()));
				}

				const ScopedLock scopedLock(lock_);

				recentTriangleFaces_ = std::move(recentTriangleFaces);
				recentVertices_ = std::move(recentVertices);
				recentPerVertexNormals_ = std::move(recentPerVertexNormals);
				// not copying the vertex colors because a texture is available

				finalTextureAtlas_ = std::move(finalTextureAtlas);
				finalTextureCoordinates_ = std::move(finalTextureCoordinates);

				isFinalReconstruction_ = true;

				Log::info() << "Successfully processed the refined mesh from object capture";

				return;
			}
		}
	}

	Log::error() << "Failed to get a final mesh reconstruction";
}

bool ObjectCaptureExperience::extractFromVogonMesh(const vogon::MeshBase& vogonMesh, Vectors3& vertices, Rendering::TriangleFaces& triangleFaces, Vectors3& vertexNormals, RGBAColors* vertexColors)
{
	const vogon::TriangleMesh* triangleMesh = dynamic_cast<const vogon::TriangleMesh*>(&vogonMesh);

	if (triangleMesh != nullptr)
	{
		const std::vector<Eigen::Vector3f>& vogonVertices = triangleMesh->vertices();
		const std::vector<uint32_t>& vogonFaceIndices = triangleMesh->faceIndices();
		const std::vector<Eigen::Vector3<uint8_t>>& vogonVertexColors = triangleMesh->vertexColors();
		const std::vector<Eigen::Vector3f>& vogonVertexNormals = triangleMesh->vertexNormals();

		Rendering::TriangleFaces localTriangleFaces;
		localTriangleFaces.reserve(vogonFaceIndices.size() / 3);

		for (size_t n = 0; n < vogonFaceIndices.size(); n += 3)
		{
			localTriangleFaces.emplace_back(Index32(vogonFaceIndices[n + 0]), Index32(vogonFaceIndices[n + 1]), Index32(vogonFaceIndices[n + 2]));
		}

		triangleFaces = std::move(localTriangleFaces);


		Vectors3 localVertices;
		localVertices.reserve(vogonVertices.size());

		for (const Eigen::Vector3f& vertex : vogonVertices)
		{
			localVertices.emplace_back(Scalar(vertex.x()), Scalar(vertex.y()), Scalar(vertex.z()));
		}

		vertices = std::move(localVertices);


		Vectors3 localVertexNormals;
		localVertexNormals.reserve(vertexNormals.size());

		for (const Eigen::Vector3f& vertexNormal : vogonVertexNormals)
		{
			localVertexNormals.emplace_back(Scalar(vertexNormal.x()), Scalar(vertexNormal.y()), Scalar(vertexNormal.z()));
		}

		vertexNormals = std::move(localVertexNormals);


		if (vertexColors != nullptr)
		{
			RGBAColors localVertexColors;

			if (!vogonVertexColors.empty())
			{
				localVertexColors.reserve(vogonVertexColors.size());

				constexpr float inv255 = 1.0f / 255.0f;
				for (const Eigen::Vector3<uint8_t>& perVertexColor : vogonVertexColors)
				{
					localVertexColors.emplace_back(float(perVertexColor.x()) * inv255, float(perVertexColor.y()) * inv255, float(perVertexColor.z()) * inv255);
				}
			}

			*vertexColors = std::move(localVertexColors);
		}

		return true;
	}

	const vogon::TriangleMeshStructured* triangleMeshStructured = dynamic_cast<const vogon::TriangleMeshStructured*>(&vogonMesh);

	if (triangleMeshStructured != nullptr)
	{
		const size_t vogonFaceIndexCount = triangleMeshStructured->faceIndexCount();
		const std::array<const int32_t*, 1> vogonFaceIndices = triangleMeshStructured->faceIndices().ptr().rawPtr();

		ocean_assert(vogonFaceIndexCount == 0 || (vogonFaceIndexCount % 3 == 0 && vogonFaceIndices[0] != nullptr));

		Rendering::TriangleFaces localTriangleFaces;
		localTriangleFaces.reserve(vogonFaceIndexCount / 3);

		for (size_t n = 0; n < vogonFaceIndexCount; n += 3)
		{
			localTriangleFaces.emplace_back(Index32(vogonFaceIndices[0][n + 0]), Index32(vogonFaceIndices[0][n + 1]), Index32(vogonFaceIndices[0][n + 2]));
		}

		triangleFaces = std::move(localTriangleFaces);


		const size_t vogonVertexCount = triangleMeshStructured->vertexCount();
		const std::array<const float*, 3> vogonVertices = triangleMeshStructured->vertices().ptr().rawPtr();

		ocean_assert(vogonVertexCount == 0 || vogonVertices[0] != nullptr);

		Vectors3 localVertices;
		localVertices.reserve(vogonVertexCount);

		for (size_t n = 0; n < vogonVertexCount; ++n)
		{
			localVertices.emplace_back(Scalar(vogonVertices[0][n]), Scalar(vogonVertices[1][n]), Scalar(vogonVertices[2][n]));
		}

		vertices = std::move(localVertices);


		const size_t vogonVertexNormalCount = triangleMeshStructured->vertexNormalCount();
		const std::array<const float*, 3> vogonVertexNormals = triangleMeshStructured->vertexNormals().ptr().rawPtr();

		ocean_assert(vogonVertexNormalCount == 0 || vogonVertexNormals[0] != nullptr);

		Vectors3 localVertexNormals;
		localVertexNormals.reserve(vogonVertexNormalCount);

		for (size_t n = 0; n < vogonVertexNormalCount; ++n)
		{
			localVertexNormals.emplace_back(Scalar(vogonVertexNormals[0][n]), Scalar(vogonVertexNormals[1][n]), Scalar(vogonVertexNormals[2][n]));
		}

		if (vertexColors != nullptr)
		{
			const size_t vogonVertexColorCount = triangleMeshStructured->vertexColorCount();
			const std::array<const uint8_t*, 3> vogonVertexColors = triangleMeshStructured->vertexColors().ptr().rawPtr();

			ocean_assert(vogonVertexColorCount == 0 || vogonVertexColors[0] != nullptr);

			RGBAColors localVertexColors;
			localVertexColors.reserve(vogonVertexColorCount);

			constexpr float inv255 = 1.0f / 255.0f;
			for (size_t n = 0; n < vogonVertexColorCount; ++n)
			{
				localVertexColors.emplace_back(float(vogonVertexColors[0][n]) * inv255, float(vogonVertexColors[1][n]) * inv255, float(vogonVertexColors[2][n]) * inv255);
			}

			*vertexColors = std::move(localVertexColors);
		}

		return true;
	}

	return false;
}

}

}
