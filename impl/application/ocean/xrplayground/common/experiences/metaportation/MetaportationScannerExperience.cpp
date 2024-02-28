// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/metaportation/MetaportationScannerExperience.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolator.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/io/Compression.h"

#include "ocean/io/image/Image.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/math/SophusUtilities.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/network/Resolver.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/mapbuilding/PatchTracker.h"
#include "ocean/tracking/mapbuilding/Utilities.h"
#include "ocean/tracking/mapbuilding/RelocalizerStereo.h"

namespace Ocean
{

namespace XRPlayground
{

using namespace Tracking::MapTexturing;

bool MetaportationScannerExperience::FeatureMap::latestFeatureMap(Tracking::MapBuilding::SharedUnifiedFeatureMap& unifiedFeatureMap)
{
	const ScopedLock scopedLock(lock_);

	if (!unifiedFeatureMap_)
	{
		return false;
	}

	unifiedFeatureMap = std::move(unifiedFeatureMap_);

	return true;
}

void MetaportationScannerExperience::FeatureMap::onReceiveData(IO::InputBitstream& bitstream)
{
	Vectors3 mapObjectPoints;
	Indices32 mapObjectPointIds;
	std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> mapUnifiedDescriptorMap;

	unsigned long long mapTag = 0u;
	if (bitstream.read<unsigned long long>(mapTag) && mapTag == mapTag_
			&& readObjectPointsFromStream(bitstream, mapObjectPoints, mapObjectPointIds)
			&& Tracking::MapBuilding::Utilities::readDescriptorMap(bitstream, mapUnifiedDescriptorMap))
	{
		ocean_assert(mapObjectPoints.size() == mapObjectPointIds.size());

		const ScopedLock scopedLock(lock_);

		mapObjectPoints_ = std::move(mapObjectPoints);
		mapObjectPointIds_ = std::move(mapObjectPointIds);
		mapUnifiedDescriptorMap_ = std::move(mapUnifiedDescriptorMap);

		if (!isThreadActive())
		{
			startThread();
		}
	}
	else
	{
		Log::error() << "Failed to decode map";
	}
}

void MetaportationScannerExperience::FeatureMap::threadRun()
{
	RandomGenerator randomGenerator;

	while (!shouldThreadStop())
	{
		sleep(1u);

		TemporaryScopedLock scopedLock(lock_);

			if (mapObjectPoints_.empty())
			{
				continue;
			}

			Vectors3 mapObjectPoints(std::move(mapObjectPoints_));
			Indices32 mapObjectPointIds = std::move(mapObjectPointIds_);
			std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> mapUnifiedDescriptorMap = std::move(mapUnifiedDescriptorMap_);

		scopedLock.release();

		ocean_assert(!mapObjectPoints.empty());

		using ImagePointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptor256;
		using ObjectPointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptors256;
		using ObjectPointVocabularyDescriptor = Tracking::MapBuilding::UnifiedDescriptor::BinaryDescriptor<256u>;

		using UnifiedFeatureMap = Tracking::MapBuilding::UnifiedFeatureMapT<ImagePointDescriptor, ObjectPointDescriptor, ObjectPointVocabularyDescriptor>;

		Tracking::MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap = std::make_shared<UnifiedFeatureMap>(std::move(mapObjectPoints), std::move(mapObjectPointIds), std::move(mapUnifiedDescriptorMap), randomGenerator, &UnifiedFeatureMap::VocabularyForest::TVocabularyTree::determineClustersMeanForBinaryDescriptor<256u>, &Tracking::MapBuilding::UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap);

		scopedLock.relock(lock_);

		unifiedFeatureMap_ = std::move(unifiedFeatureMap);
	}
}

MetaportationScannerExperience::MeshGenerator::MeshGenerator(const ScanningMode scanningMode) :
	scanningMode_(scanningMode),
	vogonFusionSystem_(0.03f, scanningMode == SM_PER_VERTEX_COLORS)
{
	// nothing to do here
}

bool MetaportationScannerExperience::MeshGenerator::start()
{
	if (isActive_)
	{
		return false;
	}

	isActive_ = true;
	return true;
}

bool MetaportationScannerExperience::MeshGenerator::stop()
{
	if (!isActive_)
	{
		return false;
	}

	isActive_ = false;
	return true;
}

bool MetaportationScannerExperience::MeshGenerator::updateMesh(Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement, const Media::FrameMediumRef& frameMedium, const Timestamp& recentSceneElementTimestamp, const HomogenousMatrix4& world_T_recentCamera)
{
	if (!recentSceneElement)
	{
		return false;
	}

	ocean_assert(recentSceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_DEPTH);

	const HomogenousMatrix4 world_T_flippedCamera(PinholeCamera::flippedTransformationRightSide(world_T_recentCamera));

	const Devices::SceneTracker6DOF::SceneElementDepth& sceneElementDepth = (const Devices::SceneTracker6DOF::SceneElementDepth&)(*recentSceneElement);

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

	SharedAnyCamera colorCamera;
	const FrameRef colorFrame = frameMedium->frame(recentSceneElementTimestamp, &colorCamera);
	if (colorFrame.isNull() || colorFrame->timestamp() != recentSceneElementTimestamp)
	{
		Log::warning() << "Missing frame for sample";
		return false;
	}

	if (this->scanningMode_ == SM_PER_VERTEX_COLORS)
	{
		Frame rgbFrame;
		if (!CV::FrameConverter::Comfort::convert(*colorFrame, FrameType(*colorFrame, FrameType::FORMAT_RGB24), rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		ocean_assert(rgbFrame.isContinuous());
		const perception::ConstImageSlice8uC3 colorSlice((const perception::Pixel8uC3*)(rgbFrame.constdata<uint8_t>()), int(rgbFrame.width()), int(rgbFrame.height()), int(rgbFrame.width()));
		const Eigen::Vector4f colorCameraParameters = Eigen::Vector4f(float(colorCamera->focalLengthX()), float(colorCamera->focalLengthY()), float(colorCamera->principalPointX()), float(colorCamera->principalPointY()));

		vogonFusionSystem_.fuse(depthSlice, depthCameraParameters, poseWorldFromFlippedCamera, colorSlice, colorCameraParameters, poseWorldFromFlippedCamera, 0.1f, 3.0f);
	}
	else
	{
		vogonFusionSystem_.fuse(depthSlice, depthCameraParameters, poseWorldFromFlippedCamera, 0.1f, 3.0f);
	}

	ocean_assert(colorCamera->name() == AnyCameraPinhole::WrappedCamera::name());
	if (colorCamera->name() == AnyCameraPinhole::WrappedCamera::name())
	{
		const PinholeCamera& actualColorCamera = ((const AnyCameraPinhole&)(*colorCamera)).actualCamera();

		frustums_.emplace_back(world_T_recentCamera, actualColorCamera, Scalar(0.1), Scalar(3));
	}

	if (world_T_previousCamera_.isValid())
	{
		ocean_assert(recentSceneElementTimestamp.isValid());

		const Scalar interval = Scalar(double(recentSceneElementTimestamp - previousSceneElementTimestamp_));
		ocean_assert(interval > Numeric::eps());

		const HomogenousMatrix4 previousCamera_T_camera(world_T_previousCamera_.inverted() * world_T_recentCamera);

		const Scalar translation = previousCamera_T_camera.translation().length() / interval;
		const Scalar rotation = previousCamera_T_camera.rotation().angle() / interval;

		if (translation <= Scalar(0.35) && rotation <= Numeric::deg2rad(Scalar(45)))
		{
			Frame downsampledColorFrame;
			if (CV::FrameShrinker::downsampleByTwo11(*colorFrame, downsampledColorFrame))
			{
				SharedAnyCamera downsampledColorCamera = colorCamera->clone(downsampledColorFrame.width(), downsampledColorFrame.height());
				ocean_assert(downsampledColorCamera);

				Frame downsampledFilteredDepth = NewTextureGenerator::downsampleDepthFrame(filteredDepth);

				keyframes_.emplace_back(NewTextureGenerator::Keyframe(std::move(downsampledColorCamera), world_T_recentCamera, std::move(downsampledColorFrame), std::move(downsampledFilteredDepth)));
			}
		}
	}

	world_T_previousCamera_ = world_T_recentCamera;
	previousSceneElementTimestamp_ = recentSceneElementTimestamp;

	if (recentSceneElementTimestamp >= nextMeshTimestamp_ && !keyframes_.empty() && textureGenerator_.readyToProcessMeshes())
	{
		std::vector<Eigen::Vector3f> vogonVertices;
		std::vector<int> vogonTriangleIndices;

		vogonFusionSystem_.generateMesh();
		vogonFusionSystem_.getMesh(vogonVertices, vogonTriangleIndices);

		ocean_assert(vogonTriangleIndices.size() % 3 == 0);

		Vectors3 vertices;
		vertices.reserve(vogonVertices.size());

		for (const Eigen::Vector3f& vogonVertex : vogonVertices)
		{
			vertices.emplace_back(Scalar(vogonVertex.x()), Scalar(vogonVertex.y()), Scalar(vogonVertex.z()));
		}

		Rendering::TriangleFaces triangleFaces = Rendering::TriangleFace::indices2triangleFaces((const Index32*)(vogonTriangleIndices.data()), vogonTriangleIndices.size());

		nextMeshTimestamp_ = recentSceneElementTimestamp + 1.0;

		textureGenerator_.processMeshes(std::move(frustums_), std::move(vertices), std::move(triangleFaces), std::move(keyframes_));
	}

	return true;
}

bool MetaportationScannerExperience::MeshGenerator::latestMesh(NewTextureGenerator::TexturedMeshMap& texturedMeshMap, Frame& textureFrame)
{
	return textureGenerator_.latestTexturedMeshes(texturedMeshMap, textureFrame);
}

bool MetaportationScannerExperience::MeshGenerator::isActive()
{
	return isActive_;
}

MetaportationScannerExperience::RelocalizerWorker::RelocalizerWorker(Media::FrameMediumRef frameMedium, Devices::SceneTracker6DOFRef sceneTracker6DOF, FeatureMap& featureMap) :
	frameMedium_(std::move(frameMedium)),
	sceneTracker6DOF_(std::move(sceneTracker6DOF)),
	featureMap_(featureMap)
{
	startThread();
}

MetaportationScannerExperience::RelocalizerWorker::~RelocalizerWorker()
{
	stopThreadExplicitly();
}

HomogenousMatrix4 MetaportationScannerExperience::RelocalizerWorker::headsetWorld_T_phoneWorld(const Timestamp& timestamp)
{
	return headsetWorld_T_phoneWorld_.transformation(timestamp);
}

void MetaportationScannerExperience::RelocalizerWorker::threadRun()
{
	ocean_assert(frameMedium_);

	Tracking::MapBuilding::RelocalizerStereo relocalizerStereo(Tracking::MapBuilding::Relocalizer::detectFreakFeatures);

	Timestamp lastFrameTimestamp(false);

	HomogenousMatrices4 phoneWorld_T_cameras;
	HomogenousMatrices4 headsetWorld_T_cameras;

	phoneWorld_T_cameras.reserve(128);
	headsetWorld_T_cameras.reserve(128);

	RandomGenerator randomGenerator;

	Frame yFrameA;
	Frame yFrameB;

	SharedAnyCamera cameraA;
	SharedAnyCamera cameraB;

	HomogenousMatrix4 device_T_cameraA(false);
	HomogenousMatrix4 device_T_cameraB(false);

	HomogenousMatrix4 phoneWorld_T_cameraA(false);
	HomogenousMatrix4 phoneWorld_T_cameraB(false);

	while (!shouldThreadStop())
	{
		Tracking::MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap;
		if (featureMap_.latestFeatureMap(unifiedFeatureMap))
		{
			relocalizerStereo.setFeatureMap(std::move(unifiedFeatureMap));
		}

		if (!relocalizerStereo.isValid())
		{
			sleep(5u);
			continue;
		}

		SharedAnyCamera camera;
		const FrameRef frame = frameMedium_->frame(&camera);

		if (frame.isNull() || frame->timestamp() <= lastFrameTimestamp)
		{
			sleep(1u);
			continue;
		}

		ocean_assert(camera);

		lastFrameTimestamp = frame->timestamp();

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(*frame, FrameType(*frame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		CV::FrameInterpolator::resize(yFrame, yFrame.width() * 375u / 1000u, yFrame.height() * 375u / 1000u);

		camera = camera->clone(yFrame.width(), yFrame.height());
		if (!camera)
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		const Devices::Tracker6DOF::Tracker6DOFSampleRef sample = sceneTracker6DOF_->sample(lastFrameTimestamp);

		if (sample.isNull() || sample->timestamp() != lastFrameTimestamp || sample->objectIds().empty())
		{
			Log::debug() << "Missing pose for camera frame, e.g., SLAM not initialized";

			continue;
		}

		ocean_assert(sample->referenceSystem() == Devices::Tracker6DOF::RS_DEVICE_IN_OBJECT);
		const HomogenousMatrix4 phoneWorld_T_camera(sample->positions().front(), sample->orientations().front());
		ocean_assert(phoneWorld_T_camera.isValid());

		if (yFrameA.isNull())
		{
			yFrameA = std::move(yFrame);
			cameraA = std::move(camera);
			device_T_cameraA.toIdentity();
			phoneWorld_T_cameraA = phoneWorld_T_camera;

			continue;
		}

		const HomogenousMatrix4 cameraA_T_cameraB(phoneWorld_T_cameraA.inverted() * phoneWorld_T_camera);

		Vector3 cameraA_t_cameraB(cameraA_T_cameraB.translation());
		cameraA_t_cameraB.z() = 0; // we want baseline not along depth

		if (cameraA_t_cameraB.sqr() < Numeric::sqr(Scalar(0.04)))
		{
			// too close to each other
			continue;
		}

		yFrameB = std::move(yFrame);
		cameraB = std::move(camera);
		device_T_cameraB = cameraA_T_cameraB;
		phoneWorld_T_cameraB = phoneWorld_T_camera;

		HomogenousMatrix4 headsetWorld_T_roughDevice(false);

		const Timestamp latestTransformationTimestamp = headsetWorld_T_phoneWorld_.timestamp();

		if (latestTransformationTimestamp + 2.0 > lastFrameTimestamp)
		{
			HomogenousMatrix4 headsetWorld_T_phoneWorld = headsetWorld_T_phoneWorld_.transformation(lastFrameTimestamp);
			if (headsetWorld_T_phoneWorld.isValid())
			{
				const HomogenousMatrix4 cameraB_T_device(device_T_cameraB.inverted());

				headsetWorld_T_roughDevice = headsetWorld_T_phoneWorld * phoneWorld_T_cameraB * cameraB_T_device;
				headsetWorld_T_roughDevice = HomogenousMatrix4(headsetWorld_T_roughDevice.translation(), headsetWorld_T_roughDevice.rotation()); // removing potential scale
			}
		}

		HomogenousMatrix4 headsetWorld_T_device(false);

		constexpr unsigned int minimalNumberCorrespondences = 100u;
		constexpr Scalar maximalProjectionError = Scalar(4.0);
		constexpr Scalar inlierRate = Scalar(0.15);

		if (relocalizerStereo.relocalize(*cameraA, *cameraB, device_T_cameraA, device_T_cameraB, yFrameA, yFrameB, headsetWorld_T_device, minimalNumberCorrespondences, maximalProjectionError, inlierRate, headsetWorld_T_roughDevice))
		{
			ocean_assert(headsetWorld_T_device.isValid());

			const HomogenousMatrix4 headsetWorld_T_camera(headsetWorld_T_device * device_T_cameraB);

			phoneWorld_T_cameras.emplace_back(phoneWorld_T_camera);
			headsetWorld_T_cameras.emplace_back(headsetWorld_T_camera);

			constexpr size_t maxPosePairs = 200;

			if (phoneWorld_T_cameras.size() >= maxPosePairs * 10)
			{
				// removing the unused poses
				phoneWorld_T_cameras = HomogenousMatrices4(phoneWorld_T_cameras.data() + phoneWorld_T_cameras.size() - maxPosePairs, phoneWorld_T_cameras.data() + phoneWorld_T_cameras.size());
				headsetWorld_T_cameras = HomogenousMatrices4(headsetWorld_T_cameras.data() + headsetWorld_T_cameras.size() - maxPosePairs, headsetWorld_T_cameras.data() + headsetWorld_T_cameras.size());
			}

			const size_t posePairs = std::min(phoneWorld_T_cameras.size(), maxPosePairs);
			const size_t poseOffset = phoneWorld_T_cameras.size() - posePairs;
			ocean_assert(poseOffset + posePairs == phoneWorld_T_cameras.size());

			HomogenousMatrix4 headsetWorld_T_phoneWorld(false);
			Scalar scale;
			if (Geometry::AbsoluteTransformation::calculateTransformationWithOutliers(phoneWorld_T_cameras.data() + poseOffset, headsetWorld_T_cameras.data() + poseOffset, posePairs, headsetWorld_T_phoneWorld, Scalar(0.5), Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric, &scale) && scale >= Scalar(0.9) && scale <= Scalar(1.1))
			{
				headsetWorld_T_phoneWorld.applyScale(Vector3(scale, scale, scale));

				headsetWorld_T_phoneWorld_.setTransformation(headsetWorld_T_phoneWorld, lastFrameTimestamp);
			}
		}
	}
}

MetaportationScannerExperience::NetworkWorker::NetworkWorker(MetaportationScannerExperience& owner, FeatureMap& featureMap, const Rendering::TextRef& renderingText) :
	owner_(owner),
	featureMap_(featureMap),
	renderingText_(renderingText)
{
	tcpServer_.setPort(Network::Port(6000, Network::Port::TYPE_READABLE));

	tcpServer_.setConnectionRequestCallback(Network::PackagedTCPServer::ConnectionRequestCallback::create(*this, &NetworkWorker::onConnectionRequestFromHeadsetTCP));
	tcpServer_.setDisconnectCallback(Network::PackagedTCPServer::DisconnectCallback::create(*this, &NetworkWorker::onConnectionDisconnectedFromHeadsetTCP));
	tcpServer_.setReceiveCallback(Network::PackagedTCPServer::ReceiveCallback::create(*this, &NetworkWorker::onReceiveFromHeadsetTCP));

	if (tcpServer_.start())
	{
		startThread();
	}
}

MetaportationScannerExperience::NetworkWorker::~NetworkWorker()
{
	stopThreadExplicitly();
}

void MetaportationScannerExperience::NetworkWorker::newPose(const HomogenousMatrix4& world_T_recentCamera)
{
	// sending the recent device position immediately

	std::ostringstream stringStream(std::ios::binary);
	IO::OutputBitstream bitstream(stringStream);

	if (writeTransformationToStream(world_T_recentCamera, bitstream))
	{
		const std::string data = stringStream.str();
		ocean_assert(!data.empty());

		const ScopedLock scopedLock(poseLock_);

		if (!udpAddress_.isValid() || !udpPort_.isValid() || udpClient_.send(udpAddress_, udpPort_, data.c_str(), data.size()) != Network::Socket::SR_SUCCEEDED)
		{
			renderingText_->setText("Failed to send camera pose");
		}
	}
}

Network::Port MetaportationScannerExperience::NetworkWorker::tcpServerPort()
{
	return tcpServer_.port();
}

void MetaportationScannerExperience::NetworkWorker::newMesh(const HomogenousMatrix4& hmdWorld_T_phoneWorld, NewTextureGenerator::TexturedMeshMap&& texturedMeshMap, Frame&& textureFrame)
{
	const ScopedLock scopedLock(meshLock_);

	const size_t queueSize = meshesQueue_.size();

	// due skip too old meshes to reduce the memory load on the phone
	while (meshesQueue_.size() > 50)
	{
		meshesQueue_.pop();
	}

	if (queueSize != meshesQueue_.size())
	{
		Log::warning() << "Skipped " << queueSize - meshesQueue_.size() << " meshes from the network queue";
	}

	meshesQueue_.emplace(hmdWorld_T_phoneWorld, std::move(texturedMeshMap), std::move(textureFrame));
}

void MetaportationScannerExperience::NetworkWorker::threadRun()
{
	ocean_assert(renderingText_);

	while (!shouldThreadStop())
	{
		const Network::PackagedTCPServer::ConnectionId connectionId(connectionId_);

		if (connectionId == Network::PackagedTCPServer::invalidConnectionId())
		{
			sleep(5u);
			continue;
		}

		bool needSleep = true;

		{
			// sending the mesh to device

			TemporaryScopedLock scopedLock(meshLock_);

				Meshes meshes;
				if (!meshesQueue_.empty())
				{
					meshes = std::move(meshesQueue_.front());
					meshesQueue_.pop();
				}

			scopedLock.release();

			if (meshes.isValid())
			{
				std::ostringstream stringStream(std::ios::binary);
				IO::OutputBitstream bitstream(stringStream);

				bool succeeded = false;

				if (writeMeshesToStream(meshes, bitstream))
				{
					const std::string meshData = stringStream.str();
					ocean_assert(!meshData.empty());

					IO::Compression::Buffer compressedMeshData;
					if (IO::Compression::gzipCompress(meshData.c_str(), meshData.size(), compressedMeshData))
					{
						succeeded = tcpServer_.send(connectionId, compressedMeshData.data(), compressedMeshData.size()) == Network::Socket::SR_SUCCEEDED;
					}
				}

				if (succeeded)
				{
					renderingText_->setText("Connection started");
					succeeded = true;
				}
				else
				{
					renderingText_->setText("Failed to send mesh");
				}

				needSleep = false;
			}
		}

		if (needSleep)
		{
			sleep(1u);
		}
	}
}

bool MetaportationScannerExperience::NetworkWorker::onConnectionRequestFromHeadsetTCP(const Network::Address4& senderAddress, const Network::Port& senderPort, const Network::ConnectionOrientedServer::ConnectionId connectionId)
{
	Log::info() << "CameraStreamingReceiverExperience: Established connection from " << senderAddress.readable() << ":" << senderPort.readable();

	ocean_assert(renderingText_);
	if (renderingText_)
	{
		renderingText_->setText(" Connection established from \n" + senderAddress.readable() + " ");
	}

	const ScopedLock scopedLock(poseLock_);

	connectionId_ = connectionId;

	udpAddress_ = senderAddress;

	return true;
}

void MetaportationScannerExperience::NetworkWorker::onConnectionDisconnectedFromHeadsetTCP(const Network::ConnectionOrientedServer::ConnectionId connectionId)
{
	Log::info() << "CameraStreamingReceiverExperience: Disconnected stream";

	ocean_assert(renderingText_);
	if (renderingText_)
	{
		renderingText_->setText(" Disconnected stream ");
	}
}

void MetaportationScannerExperience::NetworkWorker::onReceiveFromHeadsetTCP(const Network::ConnectionOrientedServer::ConnectionId connectionId, const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size > 0);

	const ScopedUncompressedMemory scopedUncompressedMemory(data, size);

	const std::string inputString((const char*)(scopedUncompressedMemory.data()), scopedUncompressedMemory.size());

	std::istringstream stringStream(inputString, std::ios::binary);
	IO::InputBitstream bitstream(stringStream);

	unsigned long long tag = 0u;
	if (bitstream.look<unsigned long long>(tag))
	{
		if (tag == mapTag_)
		{
			featureMap_.onReceiveData(bitstream);
		}
		else if (tag == portTag_)
		{
			unsigned long long portTag = 0u;
			unsigned short portValue = 0u;

			if (bitstream.read<unsigned long long>(portTag) && portTag == portTag_ && bitstream.read<unsigned short>(portValue))
			{
				const Network::Port udpPort(portValue, Network::Port::TYPE_BIG_ENDIAN);

				TemporaryScopedLock scopedLock(poseLock_);
					udpPort_ = udpPort;
				scopedLock.release();

				Log::info() << "Decoded target UDP port: " << udpPort.readable();

				if (!owner_.start(SM_TEXTURED))
				{
					Log::error() << "Failed to start the session";
				}
			}
			else
			{
				Log::error() << "Failed to decode UDP port";
			}
		}
	}
}

MetaportationScannerExperience::~MetaportationScannerExperience()
{
	// nothing to do here
}

bool MetaportationScannerExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

	if (!engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			frameMedium_ = undistortedBackground->medium();
		}
	}

	// let's cover the video background with an almost opaque sphere
	Rendering::AttributeSetRef sphereAttributeSet;
	experienceScene()->addChild(Rendering::Utilities::createSphere(engine, Scalar(100), RGBAColor(0.0f, 0.0f, 0.0f, 0.8f), nullptr, &sphereAttributeSet));
	Rendering::PrimitiveAttributeRef spherePrimitiveAttributeSet = engine->factory().createPrimitiveAttribute();
	spherePrimitiveAttributeSet->setCullingMode(Rendering::PrimitiveAttribute::CULLING_NONE);
	spherePrimitiveAttributeSet->setLightingMode(Rendering::PrimitiveAttribute::LM_TWO_SIDED_LIGHTING);
	sphereAttributeSet->addAttribute(spherePrimitiveAttributeSet);

	if (!anchoredContentManager_.initialize(std::bind(&MetaportationScannerExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
	absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
	absoluteTransform->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));
	experienceScene()->addChild(absoluteTransform);

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Scan the QR code with the \n Metaportation experience in the headset ", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, Scalar(0.005), 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingText_);

	absoluteTransform->addChild(textTransform);

	const Network::Resolver::Addresses4 localAddresses = Network::Resolver::get().localAddresses();

	if (!localAddresses.empty())
	{
		ocean_assert(networkWorker_ == nullptr);
		networkWorker_ = std::make_shared<NetworkWorker>(*this, featureMap_, renderingText_);

		const Network::Port tcpServerPort = networkWorker_->tcpServerPort();

		if (tcpServerPort.isValid())
		{
			Log::info() << "Creating QR Code for local address " << localAddresses.front().readable() << ", " << tcpServerPort.readable();

			const unsigned int address = localAddresses.front();
			const unsigned short port = tcpServerPort;

			static_assert(sizeof(address) + sizeof(port) == 6, "Invalid data type!");

			std::vector<uint8_t> payload(8);
			payload[0] = uint8_t('M'); // tag for Metaportation
			payload[1] = uint8_t('P');
			memcpy(payload.data() + 2, &address, sizeof(address));
			memcpy(payload.data() + 6, &port, sizeof(port));

			Frame qrCode = generateQRCodeFrame(payload);

			if (qrCode)
			{
				Rendering::AttributeSetRef attributeSet;
				renderingTransformQRCode_ = Rendering::Utilities::createBox(engine, Vector3(1, 1, 0), nullptr, &attributeSet);
				renderingTransformQRCode_->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(-0.1), Scalar(-1.6))));
				absoluteTransform->addChild(renderingTransformQRCode_);

				Rendering::TexturesRef textures = engine->factory().createTextures();
				Rendering::FrameTexture2DRef frameTexture = engine->factory().createFrameTexture2D();

				frameTexture->setTexture(std::move(qrCode));
				frameTexture->setUseMipmaps(true);
				frameTexture->setMinificationFilterMode(Rendering::FrameTexture2D::MIN_MODE_LINEAR_MIPMAP_LINEAR);
				frameTexture->setMagnificationFilterMode(Rendering::FrameTexture2D::MAG_MODE_LINEAR);
				textures->addTexture(frameTexture);

				attributeSet->addAttribute(textures);

				// let's cover the video background with a fully opaque, white sphere
				Rendering::MaterialRef sphereMaterial;
				renderingTransformQRCode_->addChild(Rendering::Utilities::createSphere(engine, Scalar(80), RGBAColor(1.0f, 1.0f, 1.0f, 1.0f), nullptr, &sphereAttributeSet, &sphereMaterial));
				sphereMaterial->setEmissiveColor(RGBAColor(1.0f, 1.0f, 1.0f, 1.0f));
				spherePrimitiveAttributeSet = engine->factory().createPrimitiveAttribute();
				spherePrimitiveAttributeSet->setCullingMode(Rendering::PrimitiveAttribute::CULLING_NONE);
				spherePrimitiveAttributeSet->setLightingMode(Rendering::PrimitiveAttribute::LM_TWO_SIDED_LIGHTING);
				sphereAttributeSet->addAttribute(spherePrimitiveAttributeSet);
			}
			else
			{
				renderingText_->setText(" Failed to create QR Code ");
			}
		}
		else
		{
			renderingText_->setText(" Failed to start TCP server ");
		}
	}
	else
	{
		renderingText_->setText(" Failed to determine local address ");
	}

	return true;
}

bool MetaportationScannerExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	sceneTrackerSampleEventSubscription_.release();

	renderingTransformQRCode_.release();
	renderingGroup_.release();

	anchoredContentManager_.release();

	sceneTracker6DOF_.release();

	return true;
}

Timestamp MetaportationScannerExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock outerScopedLock(lock_);
		const Devices::SceneTracker6DOF::ObjectId objectId(objectId_);
	outerScopedLock.release();

	if (!renderingGroup_ && objectId != Devices::SceneTracker6DOF::invalidObjectId())
	{
		renderingGroup_ = engine->factory().createGroup();

		constexpr Scalar visibilityRadius = Scalar(1000); // 1km
		constexpr Scalar engagementRadius = Scalar(10000);
		anchoredContentManager_.addContent(renderingGroup_, sceneTracker6DOF_, objectId, visibilityRadius, engagementRadius);
	}

	if (renderingGroup_)
	{
		NewTextureGenerator::TexturedMeshMap texturedMeshMap;
		Frame textureFrame;

		if (meshGenerator_ && meshGenerator_->latestMesh(texturedMeshMap, textureFrame))
		{
			Rendering::AttributeSetRef attributeSet = engine->factory().createAttributeSet();

			Frame downsampledTextureFrame;
			if (CV::FrameShrinker::downsampleByTwo11(textureFrame, downsampledTextureFrame)) // we downsample the texture to reduce memory usage on the phone
			{
				CV::FrameShrinker::downsampleByTwo11(downsampledTextureFrame);

				Rendering::FrameTexture2DRef frameTexture = engine->factory().createFrameTexture2D();
				frameTexture->setTexture(std::move(downsampledTextureFrame));

				Rendering::TexturesRef textures = engine->factory().createTextures();
				textures->addTexture(frameTexture);

				attributeSet->addAttribute(textures);
			}

			for (NewTextureGenerator::TexturedMeshMap::const_iterator iTexturedMesh = texturedMeshMap.cbegin(); iTexturedMesh != texturedMeshMap.cend(); ++iTexturedMesh)
			{
				const NewTextureGenerator::TexturedMesh& texturedMesh = iTexturedMesh->second;

				const Vectors3& vertices = texturedMesh.vertices_;
				const Vectors2& textureCoordinates = texturedMesh.textureCoordinates_;

				Rendering::VertexSetRef vertexSet = engine->factory().createVertexSet();
				vertexSet->setVertices(vertices);
				vertexSet->setTextureCoordinates(textureCoordinates, 0u);

				Rendering::TrianglesRef triangles = engine->factory().createTriangles();
				triangles->setFaces((unsigned int)(vertices.size()) / 3u);

				triangles->setVertexSet(vertexSet);

				Rendering::GeometryRef geometry = engine->factory().createGeometry();
				geometry->addRenderable(triangles, attributeSet);

				Rendering::TransformRef transform = engine->factory().createTransform();
				transform->addChild(geometry);

				renderingBlockMap_[iTexturedMesh->first] = transform;
			}

			renderingGroup_->clear();
			for (RenderingBlockMap::const_iterator iBlock = renderingBlockMap_.cbegin(); iBlock != renderingBlockMap_.cend(); ++iBlock)
			{
				renderingGroup_->addChild(iBlock->second);
			}

			const std::shared_ptr<NetworkWorker> networkWorker(networkWorker_);
			const std::shared_ptr<RelocalizerWorker> relocalizerWorker(relocalizerWorker_);

			if (networkWorker)
			{
				const HomogenousMatrix4 headsetWorld_T_phoneWorld(relocalizerWorker->headsetWorld_T_phoneWorld(timestamp));

				if (headsetWorld_T_phoneWorld.isValid())
				{
					networkWorker->newMesh(headsetWorld_T_phoneWorld, std::move(texturedMeshMap), std::move(textureFrame));
				}
			}
		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	if (meshGenerator_ && meshGenerator_->isActive())
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

bool MetaportationScannerExperience::start(const ScanningMode scanningMode)
{
	const ScopedLock scopedLock(lock_);

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Depth Tracker");
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Depth Tracker");
#endif

	if (!sceneTracker6DOF_ || !frameMedium_)
	{
		Log::error() << "Scene Tracker Experience could not access the scene tracker";
		return false;
	}

	if (Devices::VisualTrackerRef visualTracker = sceneTracker6DOF_)
	{
		visualTracker->setInput(frameMedium_);
	}

	sceneTrackerSampleEventSubscription_ = sceneTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &MetaportationScannerExperience::onSceneTrackerSample));

	if (sceneTracker6DOF_.isNull() || !sceneTracker6DOF_->start())
	{
		Log::error() << "Scene Tracker Experience could not start the scene tracker";
		return false;
	}

	meshGenerator_ = std::make_shared<MeshGenerator>(scanningMode);
	meshGenerator_->start();

	startThread();

	renderingTransformQRCode_->setVisible(false);

	return true;
}

bool MetaportationScannerExperience::stop()
{
	const ScopedLock scopedLock(lock_);

	if (meshGenerator_ && !meshGenerator_->isActive())
	{
		return false;
	}

	stopThread();

	if (meshGenerator_)
	{
		meshGenerator_->stop();
	}

	if (sceneTracker6DOF_ && !sceneTracker6DOF_->stop())
	{
		Log::error() << "Scene Tracker Experience could not stop the scene tracker";
		return false;
	}

	return true;
}

std::unique_ptr<XRPlaygroundExperience> MetaportationScannerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MetaportationScannerExperience());
}

void MetaportationScannerExperience::onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample(sample);

	if (!sceneTrackerSample || sceneTrackerSample->sceneElements().empty())
	{
		return;
	}

	for (size_t n = 0; n < sceneTrackerSample->objectIds().size(); ++n)
	{
		const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneTrackerSample->sceneElements()[n];

		if (sceneElement)
		{
			if (sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_DEPTH)
			{
				const ScopedLock scopedLock(lock_);

				recentSceneElement_ = sceneElement;
				objectId_ = sceneTrackerSample->objectIds()[n];

				recentSceneElementTimestamp_ = sceneTrackerSample->timestamp();

				phoneWorld_T_recentCamera_ = HomogenousMatrix4(sceneTrackerSample->positions().front(), sceneTrackerSample->orientations().front());

				break;
			}
		}
	}
}

void MetaportationScannerExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

void MetaportationScannerExperience::threadRun()
{
	ocean_assert(relocalizerWorker_ == nullptr);
	relocalizerWorker_ = std::make_shared<RelocalizerWorker>(frameMedium_, sceneTracker6DOF_, featureMap_);

	Timestamp lastSceneElementTimestamp(false);

	bool needSleep = false;

	while (!shouldThreadStop())
	{
		if (needSleep)
		{
			sleep(1u);
		}

		needSleep = true;

		TemporaryScopedLock scopedLock(lock_);
			const HomogenousMatrix4	phoneWorld_T_recentCamera(phoneWorld_T_recentCamera_);
			const Timestamp recentSceneElementTimestamp(recentSceneElementTimestamp_);
		scopedLock.release();

		if (meshGenerator_)
		{
			if (meshGenerator_->updateMesh(std::move(recentSceneElement_), frameMedium_, recentSceneElementTimestamp, phoneWorld_T_recentCamera_))
			{
				needSleep = false;
			}
		}

		{
			// sending the device pose to device

			if (recentSceneElementTimestamp > lastSceneElementTimestamp)
			{
				lastSceneElementTimestamp = recentSceneElementTimestamp;

				const HomogenousMatrix4& headsetWorld_T_phoneWorld(relocalizerWorker_->headsetWorld_T_phoneWorld(lastSceneElementTimestamp));

				if (headsetWorld_T_phoneWorld.isValid())
				{
					networkWorker_->newPose(headsetWorld_T_phoneWorld * phoneWorld_T_recentCamera_);
				}

				needSleep = false;
			}
		}


	}

	relocalizerWorker_ = nullptr;
	networkWorker_ = nullptr;
}

Frame MetaportationScannerExperience::generateQRCodeFrame(const std::vector<uint8_t>& data)
{
	ocean_assert(!data.empty());

	CV::Detector::QRCodes::QRCode code;
	if (!CV::Detector::QRCodes::QRCodeEncoder::encodeBinary(data, CV::Detector::QRCodes::QRCode::ECC_07, code))
	{
		Log::warning() << "QR code generation failed.";
		return Frame();
	}

	Frame yFrame = CV::Detector::QRCodes::Utilities::draw(code, /* frameSize */ 512u, /* allowTrueMultiple */ true, /* border */ 2u);
	ocean_assert(yFrame.isValid());

	return yFrame;
}

}

}
