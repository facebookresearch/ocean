// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/metaportation/LegacyMetaportationScannerExperience.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/io/image/Image.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/math/SophusUtilities.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Textures.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/mapbuilding/PatchTracker.h"
#include "ocean/tracking/mapbuilding/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

LegacyMetaportationScannerExperience::MeshObject::MeshObject(Vectors3&& vertices, Vectors2&& textureCoordinates, Rendering::TriangleFaces&& triangleFaces, Frame&& textureFrame, const HomogenousMatrix4& world_T_mesh) :
	vertices_(std::move(vertices)),
	textureCoordinates_(std::move(textureCoordinates)),
	textureFrame_(std::move(textureFrame)),
	triangleFaces_(std::move(triangleFaces)),
	world_T_mesh_(world_T_mesh)
{
	// nothing to do here
}

LegacyMetaportationScannerExperience::MeshObject::MeshObject(Vectors3&& vertices, Vectors3&& perVertexNormals, RGBAColors&& perVertexColors, Rendering::TriangleFaces&& triangleFaces, const HomogenousMatrix4& world_T_mesh) :
	vertices_(std::move(vertices)),
	perVertexNormals_(std::move(perVertexNormals)),
	perVertexColors_(std::move(perVertexColors)),
	triangleFaces_(std::move(triangleFaces)),
	world_T_mesh_(world_T_mesh)
{
	// nothing to do here
}

LegacyMetaportationScannerExperience::MeshRenderingObject::MeshRenderingObject(const Rendering::Engine& engine)
{
	transform_ = engine.factory().createTransform();
}

void LegacyMetaportationScannerExperience::MeshRenderingObject::update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors2& textureCoordinates, const Rendering::TriangleFaces& triangleFaces, const Frame& textureFrame, const HomogenousMatrix4& world_T_mesh)
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
		texture_->setTexture(Frame(textureFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT));
	}

	transform_->setTransformation(world_T_mesh);
}

void LegacyMetaportationScannerExperience::MeshRenderingObject::update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors3& perVertexNormals, const RGBAColors& perVertexColors, const Rendering::TriangleFaces& triangleFaces, const HomogenousMatrix4& world_T_mesh)
{
	if (!vertexSet_)
	{
		vertexSet_ = engine.factory().createVertexSet();

		triangles_ = engine.factory().createTriangles();
		triangles_->setVertexSet(vertexSet_);

		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		if (perVertexColors.empty())
		{
			const Rendering::MaterialRef material = engine.factory().createMaterial();
			material->setDiffuseColor(RGBAColor(0.7f, 0.7f, 0.7f));
			attributeSet->addAttribute(material);
		}

		Rendering::GeometryRef geometry = engine.factory().createGeometry();
		geometry->addRenderable(triangles_, attributeSet);

		transform_->addChild(geometry);
	}

	ocean_assert(vertexSet_ && triangles_);

	vertexSet_->setVertices(vertices);
	vertexSet_->setNormals(perVertexNormals);
	vertexSet_->setColors(perVertexColors);

	triangles_->setFaces(triangleFaces);

	transform_->setTransformation(world_T_mesh);
}

void LegacyMetaportationScannerExperience::MeshRenderingObject::update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors3& perVertexNormals, const RGBAColors& perVertexColors, const Indices32& triangleFaceIndices, const HomogenousMatrix4& world_T_mesh)
{
	Rendering::TriangleFaces triangleFaces;
	triangleFaces.reserve(triangleFaceIndices.size() / 3);

	for (size_t n = 0; n < triangleFaceIndices.size(); n += 3)
	{
		triangleFaces.emplace_back(triangleFaceIndices[n + 0], triangleFaceIndices[n + 1], triangleFaceIndices[n + 2]);
	}

	update(engine, vertices, perVertexNormals, perVertexColors, triangleFaces, world_T_mesh);
}

void LegacyMetaportationScannerExperience::MeshRenderingObject::update(const Rendering::Engine& engine, const Vectors3& vertices, const RGBAColors& perVertexColors, const Rendering::TriangleFaces& triangleFaces, const HomogenousMatrix4& world_T_mesh)
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

	update(engine, vertices, normals, perVertexColors, triangleFaces, world_T_mesh);
}

LegacyMetaportationScannerExperience::MapCreatorWorker::MapCreatorWorker(const Devices::Tracker6DOFRef& tracker6DOF, const Media::FrameMediumRef& frameMedium) :
	tracker6DOF_(tracker6DOF),
	frameMedium_(frameMedium)
{
	ocean_assert(tracker6DOF_ && frameMedium_);

	if (tracker6DOF_ && frameMedium_)
	{
		constexpr size_t maximalFeaturesPerFrame = 400;
		constexpr double newFeaturesInterval = 0.1;

		constexpr bool keepUnlocatedFeatures = false;
		constexpr size_t minimalNumberObservationsPerFeature = 15;
		constexpr Scalar minimalBoxDiagonalForLocatedFeature = Scalar(0.05); // 5cm

		const Tracking::MapBuilding::PatchTracker::Options options(maximalFeaturesPerFrame, newFeaturesInterval, keepUnlocatedFeatures, minimalNumberObservationsPerFeature, minimalBoxDiagonalForLocatedFeature);

		patchTracker_ = std::make_shared<Tracking::MapBuilding::PatchTracker>(std::make_shared<Tracking::MapBuilding::UnifiedDescriptorExtractorFreakMultiDescriptor256>(), options);

		startThread();
	}
}

LegacyMetaportationScannerExperience::MapCreatorWorker::~MapCreatorWorker()
{
	stopThreadExplicitly();
}

bool LegacyMetaportationScannerExperience::MapCreatorWorker::latestMap(Vectors3& objectPoints, Indices32& objectPointIds, std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap>& unifiedDescriptorMap) const
{
	const ScopedLock scopedLock(lock_);

	if (!patchTracker_ || !isThreadActive())
	{
		return false;
	}

	objectPoints = patchTracker_->latestObjectPoints(&objectPointIds);

	unifiedDescriptorMap = patchTracker_->unifiedDescriptorMap()->clone();

	return true;
}

void LegacyMetaportationScannerExperience::MapCreatorWorker::threadRun()
{
	ocean_assert(tracker6DOF_ && frameMedium_ && patchTracker_);

	Index32 frameIndex = 0u;
	Timestamp lastTimestamp(false);

	constexpr unsigned int pyramidLayers = 5u;

	Tracking::MapBuilding::PatchTracker::SharedFramePyramid yPreviousFramePyramid;
	Tracking::MapBuilding::PatchTracker::SharedFramePyramid yCurrentFramePyramid;

	while (!shouldThreadStop())
	{
		const Devices::Tracker6DOF::Tracker6DOFSampleRef sample(tracker6DOF_->sample()); // get the latest tracker sample

		if (!sample || sample->timestamp() <= lastTimestamp)
		{
			sleep(1u);
			continue;
		}

		SharedAnyCamera frameAnyCamera;
		const FrameRef frameRef = frameMedium_->frame(sample->timestamp(), &frameAnyCamera);

		if (!frameRef || !frameAnyCamera || frameRef->timestamp() != sample->timestamp())
		{
			sleep(1u);
			continue;
		}

		lastTimestamp = sample->timestamp();

		if (firstFrameTimestamp_.isInvalid())
		{
			firstFrameTimestamp_ = lastTimestamp;
		}

		if (sample->objectIds().empty())
		{
			continue;
		}

		const HomogenousMatrix4 world_T_camera(sample->positions().front(), sample->orientations().front());

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType(*frameRef, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		while (yFrame.width() * yFrame.height() > 640u * 480u / 2u)
		{
			CV::FrameShrinker::downsampleByTwo11(yFrame);
		}

		SharedAnyCamera anyCamera = frameAnyCamera->clone();

		if (yFrame.width() != anyCamera->width() || yFrame.height() != anyCamera->height())
		{
			anyCamera = frameAnyCamera->clone(yFrame.width(), yFrame.height());

			if (!anyCamera)
			{
				ocean_assert(false && "Failed to scale camera profile!");
				break;
			}
		}

		if (!yCurrentFramePyramid)
		{
			yCurrentFramePyramid = std::make_shared<CV::FramePyramid>();
		}

		yCurrentFramePyramid->replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.channels(), yFrame.pixelOrigin(), pyramidLayers, yFrame.paddingElements(), true /*copyfirstLayer*/, nullptr, yFrame.timestamp());

		TemporaryScopedLock scopedLock(lock_);
			patchTracker_->trackFrame(frameIndex++, *anyCamera, world_T_camera, yCurrentFramePyramid, sample->timestamp());
		scopedLock.release();

		if (yPreviousFramePyramid.use_count() == 1)
		{
			// nobody is using the previous pyramid anymore
			std::swap(yPreviousFramePyramid, yCurrentFramePyramid);
		}
		else
		{
			yCurrentFramePyramid = nullptr;
		}
	}
}

bool LegacyMetaportationScannerExperience::TexturedMeshGenerator::start()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(processorState_ == PS_STOPPED);
	processorState_ = PS_IDLE;

	return true;
}

bool LegacyMetaportationScannerExperience::TexturedMeshGenerator::stop()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(processorState_ != PS_STOPPED);
	processorState_ = PS_STOPPED;

	return true;
}

bool LegacyMetaportationScannerExperience::TexturedMeshGenerator::updateMesh(Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement)
{
	if (!recentSceneElement)
	{
		return false;
	}

	ocean_assert(recentSceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_MESHES);

	TemporaryScopedLock scopedLock(lock_);

	if (processorState_ == PS_IDLE)
	{
		processorState_ = PS_UPDATING_MESH;

		scopedLock.release();

		textureGenerator_.updateMesh(recentSceneElement, true);

		scopedLock.relock(lock_);

		ocean_assert(processorState_ == PS_UPDATING_MESH);
		processorState_ = PS_MESH_UPDATED;

		scopedLock.release();

		return true;
	}

	return false;
}

void LegacyMetaportationScannerExperience::TexturedMeshGenerator::processFrame(const Media::FrameMediumRef& frameMedium, const Timestamp& recentSceneElementTimestamp, const HomogenousMatrix4& world_T_recentCamera, const Rendering::EngineRef& engine)
{
	ocean_assert(frameMedium && recentSceneElementTimestamp.isValid());

	TemporaryScopedLock scopedLock(lock_);

	ocean_assert(processorState_ != PS_PROCESS_FRAME);
	if ((processorState_ == PS_IDLE || processorState_ == PS_MESH_UPDATED) && recentSceneElementTimestamp > lastProcessedFrameTimestamp_)
	{
		processorState_ = PS_PROCESS_FRAME;

		scopedLock.release();

		SharedAnyCamera anyCamera;
		const FrameRef frame = frameMedium->frame(recentSceneElementTimestamp, &anyCamera);

		if (frame && anyCamera && frame->timestamp() == recentSceneElementTimestamp)
		{
			Frame rgbFrame;
			if (CV::FrameConverter::Comfort::convert(*frame, FrameType(*frame, FrameType::FORMAT_RGB24), rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
			{
				textureGenerator_.processFrame(std::move(rgbFrame), *anyCamera, world_T_recentCamera, engine);
			}

			lastProcessedFrameTimestamp_ = recentSceneElementTimestamp;
		}

		scopedLock.relock(lock_);

		processorState_ = PS_IDLE;
	}
}

bool LegacyMetaportationScannerExperience::TexturedMeshGenerator::exportMesh(MeshObjectMap& meshObjectMap)
{
	TemporaryScopedLock scopedLock(lock_);

	if (processorState_ == PS_IDLE || processorState_ == PS_MESH_UPDATED)
	{
		const Indices32 meshIds = textureGenerator_.meshIds();

		if (!meshIds.empty())
		{
			processorState_ = PS_MESH_EXPORTING;
			scopedLock.release();

			meshObjectMap.clear();

			Tracking::MapTexturing::TextureGenerator::Mesh mesh;
			Frame meshTexture;

			for (const Index32& meshId : meshIds)
			{
				if (textureGenerator_.exportMesh(meshId, mesh, meshTexture))
				{
					meshObjectMap.emplace(meshId, MeshObject(std::move(mesh.vertices_), std::move(mesh.textureCoordinates_), std::move(mesh.triangleFaces_), std::move(meshTexture), HomogenousMatrix4(true)));
				}
			}

			scopedLock.relock(lock_);

			ocean_assert(processorState_ == PS_MESH_EXPORTING);
			processorState_ = PS_IDLE;

			return true;
		}
	}

	return false;
}

bool LegacyMetaportationScannerExperience::TexturedMeshGenerator::isActive()
{
	const ScopedLock scopedLock(lock_);
	return processorState_ != PS_STOPPED;
}

LegacyMetaportationScannerExperience::ColoredMeshGenerator::ColoredMeshGenerator() :
	vogonFusionSystem_(0.03f, true)
{
	// nothing to do here
}

bool LegacyMetaportationScannerExperience::ColoredMeshGenerator::start()
{
	if (isActive_)
	{
		return false;
	}

	isActive_ = true;
	return true;
}

bool LegacyMetaportationScannerExperience::ColoredMeshGenerator::stop()
{
	if (!isActive_)
	{
		return false;
	}

	isActive_ = false;
	return true;
}

bool LegacyMetaportationScannerExperience::ColoredMeshGenerator::updateMesh(Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement, const Media::FrameMediumRef& frameMedium, const Timestamp& recentSceneElementTimestamp, const HomogenousMatrix4& world_T_recentCamera)
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

	return true;
}

bool LegacyMetaportationScannerExperience::ColoredMeshGenerator::exportMesh(MeshObjectMap& meshObjectMap)
{
	std::vector<Eigen::Vector3f> vertices;
	std::vector<int> triangleIndices;
	std::vector<Eigen::Vector3<uint8_t>> perVertexColors;
	std::vector<Eigen::Vector3f> vertexNormals;

	vogonFusionSystem_.generateMesh();
	vogonFusionSystem_.getColoredMesh(vertices, triangleIndices, perVertexColors, &vertexNormals);

	if (vertices.empty())
	{
		return false;
	}

	ocean_assert(vertices.size() == vertexNormals.size());
	ocean_assert(triangleIndices.size() % 3 == 0);

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
	recentPerVertexColors.reserve(perVertexColors.size());

	constexpr float inv255 = 1.0f / 255.0f;
	for (const Eigen::Vector3<uint8_t>& perVertexColor : perVertexColors)
	{
		recentPerVertexColors.emplace_back(float(perVertexColor.x()) * inv255, float(perVertexColor.y()) * inv255, float(perVertexColor.z()) * inv255);
	}

	meshObjectMap.clear();

	constexpr Index32 meshId = 0;

	meshObjectMap.emplace(meshId, MeshObject(std::move(recentVertices), std::move(recentPerVertexNormals), std::move(recentPerVertexColors), std::move(recentTriangleFaces), HomogenousMatrix4(true)));

	return true;
}

bool LegacyMetaportationScannerExperience::ColoredMeshGenerator::isActive()
{
	return isActive_;
}

LegacyMetaportationScannerExperience::NetworkWorker::NetworkWorker(const Rendering::TextRef& renderingText, const Network::Address4& address, const Network::Port& port) :
	renderingText_(renderingText),
	address_(address),
	port_(port)
{
	startThread();
}

LegacyMetaportationScannerExperience::NetworkWorker::~NetworkWorker()
{
	stopThreadExplicitly();
}

void LegacyMetaportationScannerExperience::NetworkWorker::newPose(const HomogenousMatrix4& world_T_recentCamera)
{
	// sending the recent device position immediately

	std::ostringstream stringStream(std::ios::binary);
	IO::OutputBitstream bitstream(stringStream);

	if (writeTransformationToStream(world_T_recentCamera, bitstream))
	{
		const std::string data = stringStream.str();
		ocean_assert(!data.empty());

		const ScopedLock scopedLock(poseLock_);

		if (udpClient_.send(address_, port_, data.c_str(), data.size()) == Network::Socket::SR_SUCCEEDED)
		{
			renderingText_->setText("Connection started");
		}
		else
		{
			renderingText_->setText("Failed to send camera pose");
		}
	}
}

void LegacyMetaportationScannerExperience::NetworkWorker::newMap(Vectors3&& objectPoints, Indices32&& objectPointIds, std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap>&& unifiedDescriptorMap)
{
	const ScopedLock scopedLock(mapLock_);

	mapObjectPoints_ = std::move(objectPoints);
	mapObjectPointIds_ = std::move(objectPointIds);
	mapUnifiedDescriptorMap_ = std::move(unifiedDescriptorMap);
}

void LegacyMetaportationScannerExperience::NetworkWorker::newMesh(MeshObjectMap&& meshObjectMap)
{
	const ScopedLock scopedLock(meshLock_);

	meshObjectMap_ = std::move(meshObjectMap);
}

void LegacyMetaportationScannerExperience::NetworkWorker::threadRun()
{
	ocean_assert(renderingText_);

	MeshObjectMap meshObjectMap;
	MeshObjectMap::iterator iMesh = meshObjectMap.end();

	unsigned int remainingMeshes = (unsigned int)(-1);

	while (!shouldThreadStop())
	{
		if (!tcpClient_.isConnected() && address_.isValid() && port_.isValid())
		{
			if (tcpClient_.connect(address_, port_))
			{
				renderingText_->setText("Connection started");
			}
			else
			{
				renderingText_->setText("Connection failed");
			}
		}

		if (!tcpClient_.isConnected())
		{
			sleep(5u);
			continue;
		}

		bool needSleep = true;

		{
			// sending the map to device

			TemporaryScopedLock scopedLock(mapLock_);
				const Vectors3 objectPoints(mapObjectPoints_);
				const Indices32 objectPointIds(mapObjectPointIds_);
				const std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap(mapUnifiedDescriptorMap_);
			scopedLock.release();

			if (!objectPoints.empty())
			{
				std::ostringstream stringStream(std::ios::binary);
				IO::OutputBitstream bitstream(stringStream);

				if (bitstream.write<unsigned long long>(mapTag_)
						&& writeObjectPointsToStream(objectPoints, objectPointIds, bitstream)
						&& Tracking::MapBuilding::Utilities::writeDescriptorMap(*unifiedDescriptorMap, bitstream))
				{
					const std::string data = stringStream.str();
					ocean_assert(!data.empty());

					if (tcpClient_.send(data.c_str(), data.size()) == Network::Socket::SR_SUCCEEDED)
					{
						renderingText_->setText("Connection started");
					}
					else
					{
						renderingText_->setText("Failed to send mesh");
					}

					needSleep = false;
				}
			}
		}

		{
			// sending the mesh to device

			TemporaryScopedLock scopedLock(meshLock_);

			if (iMesh == meshObjectMap.cend())
			{
				meshObjectMap = std::move(meshObjectMap_);
				iMesh = meshObjectMap.begin();

				remainingMeshes = (unsigned int)(meshObjectMap.size()) - 1u;
			}

			if (iMesh != meshObjectMap.cend())
			{
				const Index32& meshId = iMesh->first;
				const MeshObject& mesh = iMesh->second;

				std::ostringstream stringStream(std::ios::binary);
				IO::OutputBitstream bitstream(stringStream);

				Vectors2 flippedTextureCoordinates; // **TODO** workaround as the image will be flipped
				flippedTextureCoordinates.reserve(mesh.textureCoordinates().size());

				for (const Vector2& textureCoordinate : mesh.textureCoordinates())
				{
					flippedTextureCoordinates.emplace_back(textureCoordinate.x(), Scalar(1) - textureCoordinate.y());
				}

				ocean_assert(remainingMeshes < (unsigned int)(meshObjectMap.size()));

				if (writeMeshToStream(meshId, remainingMeshes, mesh.world_T_mesh(), mesh.vertices(), mesh.perVertexNormals(), mesh.perVertexColors(), flippedTextureCoordinates, mesh.triangleFaces(), mesh.textureFrame(), bitstream))
				{
					const std::string data = stringStream.str();
					ocean_assert(!data.empty());

					scopedLock.release();

					if (tcpClient_.send(data.c_str(), data.size()) == Network::Socket::SR_SUCCEEDED)
					{
						renderingText_->setText("Connection started");
					}
					else
					{
						renderingText_->setText("Failed to send mesh");
					}

					needSleep = false;

					++iMesh;
					--remainingMeshes;
				}
			}
		}

		if (needSleep)
		{
			sleep(1u);
		}
	}
}

LegacyMetaportationScannerExperience::~LegacyMetaportationScannerExperience()
{
	// nothing to do here
}

bool LegacyMetaportationScannerExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
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

	if (!anchoredContentManager_.initialize(std::bind(&LegacyMetaportationScannerExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
	absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
	absoluteTransform->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));
	experienceScene()->addChild(absoluteTransform);

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, "Enter IP address and port as\nprovided in XRPlaygroud for Quest", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, Scalar(0.005), 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingText_);

	absoluteTransform->addChild(textTransform);

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (userInterface.isIOS())
	{
		showUserInterinterfaceIOS(userInterface);
	}
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	return true;
}

bool LegacyMetaportationScannerExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
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

Timestamp LegacyMetaportationScannerExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock outerScopedLock(lock_);
		const Devices::SceneTracker6DOF::ObjectId objectId(objectId_);
		const Timestamp recentSceneElementTimestamp(recentSceneElementTimestamp_);
		const HomogenousMatrix4 world_T_recentCamera(world_T_recentCamera_);
	outerScopedLock.release();

	if (!renderingGroup_ && objectId != Devices::SceneTracker6DOF::invalidObjectId())
	{
		renderingGroup_ = engine->factory().createGroup();

		constexpr Scalar visibilityRadius = Scalar(1000); // 1km
		constexpr Scalar engagementRadius = Scalar(10000);
		anchoredContentManager_.addContent(renderingGroup_, sceneTracker6DOF_, objectId, visibilityRadius, engagementRadius);
	}

	if (frameMedium_ && world_T_recentCamera.isValid())
	{
		if (texturedMeshGenerator_)
		{
			texturedMeshGenerator_->processFrame(frameMedium_, recentSceneElementTimestamp, world_T_recentCamera, engine);
		}
	}

	if (renderingGroup_)
	{
		bool skipExportingMesh = false;

		{
			// we check whether we have an exported mesh from the last frame, which we have not yet forwarded to the rendering engine (to avoid exporting and forwarding in the same frame due to performance)

			const ScopedLock scopedLock(meshObjectMapLock_);

			if (!meshObjectMapForRendering_.empty())
			{
				for (MeshObjectMap::const_iterator iMeshObject = meshObjectMapForRendering_.cbegin(); iMeshObject != meshObjectMapForRendering_.cend(); ++iMeshObject)
				{
					MeshRenderingObjectMap::iterator iMesh = meshRenderingObjectMap_.find(iMeshObject->first);
					if (iMesh == meshRenderingObjectMap_.cend())
					{
						iMesh = meshRenderingObjectMap_.emplace(iMeshObject->first, *engine).first;
						renderingGroup_->addChild(iMesh->second.transform());
					}

					const MeshObject& meshObject = iMeshObject->second;

					if (meshObject.textureFrame().isValid())
					{
						iMesh->second.update(*engine, meshObject.vertices(), meshObject.textureCoordinates(), meshObject.triangleFaces(), meshObject.textureFrame(), HomogenousMatrix4(true));
					}
					else
					{
						if (meshObject.perVertexNormals().empty())
						{
							iMesh->second.update(*engine, meshObject.vertices(), meshObject.perVertexColors(), meshObject.triangleFaces(), HomogenousMatrix4(true));
						}
						else
						{
							iMesh->second.update(*engine, meshObject.vertices(), meshObject.perVertexNormals(), meshObject.perVertexColors(), meshObject.triangleFaces(), HomogenousMatrix4(true));
						}
					}
				}

				meshObjectMapForSending_ = std::move(meshObjectMapForRendering_);

				skipExportingMesh = true; // we did enough in this frame already
			}
		}

		if (texturedMeshGenerator_)
		{
			constexpr double updateInterval = 0.5;

			if (!skipExportingMesh && (renderingObjectTimestamp_.isInvalid() || timestamp >= renderingObjectTimestamp_ + updateInterval))
			{
				MeshObjectMap meshObjectMap;
				if (texturedMeshGenerator_->exportMesh(meshObjectMap))
				{
					const ScopedLock scopedLock(meshObjectMapLock_);
					meshObjectMapForRendering_ = std::move(meshObjectMap);
				}

				renderingObjectTimestamp_ = timestamp;
			}
		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	const bool meshingIsActive = (texturedMeshGenerator_ && texturedMeshGenerator_->isActive()) || (coloredMeshGenerator_ && coloredMeshGenerator_->isActive());

	if (meshingIsActive)
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

bool LegacyMetaportationScannerExperience::start(const ScanningMode scanningMode, const Network::Address4& address, const Network::Port& port)
{
	const ScopedLock scopedLock(lock_);

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	if (scanningMode == SM_TEXTURED)
	{
		sceneTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Scene Tracker");
	}
	else
	{
		ocean_assert(scanningMode == SM_PER_VERTEX_COLORS);
		sceneTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Depth Tracker");
	}
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	if (scanningMode == SM_TEXTURED)
	{
		sceneTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Scene Tracker");
	}
	else
	{
		ocean_assert(scanningMode == SM_PER_VERTEX_COLORS);
		sceneTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Depth Tracker");
	}
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

	sceneTrackerSampleEventSubscription_ = sceneTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &LegacyMetaportationScannerExperience::onSceneTrackerSample));

	if (sceneTracker6DOF_.isNull() || !sceneTracker6DOF_->start())
	{
		Log::error() << "Scene Tracker Experience could not start the scene tracker";
		return false;
	}

	scanningMode_ = scanningMode;

	address_ = address;
	port_ = port;

	if (scanningMode == SM_TEXTURED)
	{
		texturedMeshGenerator_ = std::make_shared<TexturedMeshGenerator>();
		texturedMeshGenerator_->start();
	}
	else
	{
		ocean_assert(scanningMode == SM_PER_VERTEX_COLORS);

		coloredMeshGenerator_ = std::make_shared<ColoredMeshGenerator>();
		coloredMeshGenerator_->start();
	}

	startThread();

	return true;
}

bool LegacyMetaportationScannerExperience::stop()
{
	const ScopedLock scopedLock(lock_);

	if ((texturedMeshGenerator_ && !texturedMeshGenerator_->isActive()) || (coloredMeshGenerator_ && !coloredMeshGenerator_->isActive()))
	{
		return false;
	}

	stopThread();

	if (texturedMeshGenerator_)
	{
		texturedMeshGenerator_->stop();
	}
	if (coloredMeshGenerator_)
	{
		coloredMeshGenerator_->stop();
	}

	if (sceneTracker6DOF_ && !sceneTracker6DOF_->stop())
	{
		Log::error() << "Scene Tracker Experience could not stop the scene tracker";
		return false;
	}

	address_ = Network::Address4();
	port_ = Network::Port();

	return true;
}

std::unique_ptr<XRPlaygroundExperience> LegacyMetaportationScannerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new LegacyMetaportationScannerExperience());
}

void LegacyMetaportationScannerExperience::onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
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
			const Devices::SceneTracker6DOF::SceneElement::SceneElementType expectedSceneElementType = scanningMode_ == SM_TEXTURED ? Devices::SceneTracker6DOF::SceneElement::SET_MESHES : Devices::SceneTracker6DOF::SceneElement::SET_DEPTH;

			if (sceneElement->sceneElementType() == expectedSceneElementType)
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
}

void LegacyMetaportationScannerExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

void LegacyMetaportationScannerExperience::threadRun()
{
	MapCreatorWorker mapCreatorWorker(sceneTracker6DOF_, frameMedium_);

	NetworkWorker networkWorker(renderingText_, address_, port_);

	Timestamp lastSceneElementTimestamp(false);

	Timestamp nextMapTimestamp = Timestamp(Timestamp(true) + 2.0);
	double mapTimestampInterval = 0.5;

	Timestamp nextMeshTimestamp = Timestamp(true);
	double meshTimestampInterval = 0.5;

	while (!shouldThreadStop())
	{
		bool needSleep = true;

		TemporaryScopedLock scopedLock(lock_);
			const HomogenousMatrix4	world_T_recentCamera(world_T_recentCamera_);
			const Timestamp recentSceneElementTimestamp(recentSceneElementTimestamp_);
		scopedLock.release();

		if (texturedMeshGenerator_)
		{
			if (texturedMeshGenerator_->updateMesh(std::move(recentSceneElement_)))
			{
				needSleep = false;
			}
		}
		else if (coloredMeshGenerator_)
		{
			if (coloredMeshGenerator_->updateMesh(std::move(recentSceneElement_), frameMedium_, recentSceneElementTimestamp, world_T_recentCamera))
			{
				needSleep = false;
			}

			constexpr double updateInterval = 0.5;

			if (renderingObjectTimestamp_.isInvalid() || recentSceneElementTimestamp >= renderingObjectTimestamp_ + updateInterval)
			{
				MeshObjectMap meshObjectMap;
				if (coloredMeshGenerator_->exportMesh(meshObjectMap))
				{
					const ScopedLock innerScopedLock(meshObjectMapLock_);
					meshObjectMapForRendering_ = std::move(meshObjectMap);
				}

				renderingObjectTimestamp_ = recentSceneElementTimestamp;
			}
		}

		{
			// sending the device pose to device

			if (recentSceneElementTimestamp > lastSceneElementTimestamp)
			{
				lastSceneElementTimestamp = recentSceneElementTimestamp;

				networkWorker.newPose(world_T_recentCamera);

				needSleep = false;
			}
		}

		const Timestamp currentTimestamp(true);

		{
			// sending the map to device

			if (currentTimestamp >= nextMapTimestamp)
			{
				Vectors3 objectPoints;
				Indices32 objectPointIds;
				std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap;

				if (mapCreatorWorker.latestMap(objectPoints, objectPointIds, unifiedDescriptorMap) && !objectPoints.empty() && unifiedDescriptorMap)
				{
					networkWorker.newMap(std::move(objectPoints), std::move(objectPointIds), std::move(unifiedDescriptorMap));
					needSleep = false;

					mapTimestampInterval = std::min(mapTimestampInterval * 1.25, 10.0);
				}

				nextMapTimestamp = currentTimestamp + mapTimestampInterval;
			}
		}

		{
			// sending the mesh to device

			if (currentTimestamp >= nextMeshTimestamp)
			{
				scopedLock.relock(meshObjectMapLock_);

				if (!meshObjectMapForSending_.empty())
				{
					networkWorker.newMesh(std::move(meshObjectMapForSending_));
				}

				meshTimestampInterval = std::min(meshTimestampInterval * 1.05, 2.0);

				nextMeshTimestamp = currentTimestamp + meshTimestampInterval;
			}
		}

		if (needSleep)
		{
			sleep(5u);
		}
	}
}

bool LegacyMetaportationScannerExperience::writeTransformationToStream(const HomogenousMatrix4& world_T_camera, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(transformationTag_))
	{
		return false;
	}

	const HomogenousMatrixD4 worldD_T_cameraD(world_T_camera);

	constexpr unsigned long long version = 1ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write(worldD_T_cameraD.data(), sizeof(double) * 16))
	{
		return false;
	}

	return true;
}

bool LegacyMetaportationScannerExperience::writeMeshToStream(const Index32 meshId, const unsigned int remainingMeshes, const HomogenousMatrix4& world_T_mesh, const Vectors3& vertices, const Vectors3& perVertexNormals, const RGBAColors& perVertexColors, const Vectors2& textureCoordinates, const Rendering::TriangleFaces& triangleFaces, const Frame& textureFrame, IO::OutputBitstream& bitstream)
{
	ocean_assert(vertices.size() == perVertexNormals.size() || perVertexNormals.empty());
	ocean_assert(vertices.size() == textureCoordinates.size() || textureCoordinates.empty());

	if (vertices.empty() || (vertices.size() != perVertexNormals.size() && !perVertexNormals.empty()) || (vertices.size() != textureCoordinates.size() && !textureCoordinates.empty()))
	{
		return false;
	}

	if (!bitstream.write<unsigned long long>(meshTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 2ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write<Index32>(meshId))
	{
		return false;
	}

	if (!bitstream.write<unsigned int>(remainingMeshes))
	{
		return false;
	}

	if (!writeTransformationToStream(world_T_mesh, bitstream))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		const unsigned int numberVertices = (unsigned int)(vertices.size());
		if (!bitstream.write<unsigned int>(numberVertices))
		{
			return false;
		}

		if (!bitstream.write(vertices.data(), vertices.size() * sizeof(VectorF3)))
		{
			return false;
		}

		const unsigned int numberNormals = (unsigned int)(perVertexNormals.size());
		if (!bitstream.write<unsigned int>(numberNormals))
		{
			return false;
		}

		if (!bitstream.write(perVertexNormals.data(), perVertexNormals.size() * sizeof(VectorF3)))
		{
			return false;
		}

		const unsigned int numberTextureCoordinates = (unsigned int)(textureCoordinates.size());
		if (!bitstream.write<unsigned int>(numberTextureCoordinates))
		{
			return false;
		}

		if (!bitstream.write(textureCoordinates.data(), textureCoordinates.size() * sizeof(VectorF2)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 verticesF;
		verticesF.reserve(vertices.size());
		for (const Vector3& vertex : vertices)
		{
			verticesF.emplace_back(float(vertex.x()), float(vertex.y()), float(vertex.z()));
		}

		VectorsF3 perVertexNormalsF;
		perVertexNormalsF.reserve(perVertexNormals.size());
		for (const Vector3& perVertexNormal : perVertexNormals)
		{
			perVertexNormalsF.emplace_back(float(perVertexNormal.x()), float(perVertexNormal.y()), float(perVertexNormal.z()));
		}

		VectorsF2 textureCoordinatesF;
		textureCoordinatesF.reserve(textureCoordinates.size());
		for (const Vector2& textureCoordinate : textureCoordinates)
		{
			textureCoordinatesF.emplace_back(float(textureCoordinate.x()), float(textureCoordinate.y()));
		}

		const unsigned int numberVertices = (unsigned int)(verticesF.size());
		if (!bitstream.write<unsigned int>(numberVertices))
		{
			return false;
		}

		if (!bitstream.write(verticesF.data(), verticesF.size() * sizeof(VectorF3)))
		{
			return  false;
		}

		const unsigned int numberNormals = (unsigned int)(perVertexNormalsF.size());
		if (!bitstream.write<unsigned int>(numberNormals))
		{
			return false;
		}

		if (!bitstream.write(perVertexNormalsF.data(), perVertexNormalsF.size() * sizeof(VectorF3)))
		{
			return  false;
		}

		const unsigned int numberTextureCoordinates = (unsigned int)(textureCoordinatesF.size());
		if (!bitstream.write<unsigned int>(numberTextureCoordinates))
		{
			return false;
		}

		if (!bitstream.write(textureCoordinatesF.data(), textureCoordinatesF.size() * sizeof(VectorF2)))
		{
			return  false;
		}
	}

	const unsigned int numberPerVertexColors = (unsigned int)(perVertexColors.size());
	if (!bitstream.write<unsigned int>(numberPerVertexColors))
	{
		return false;
	}

	static_assert(sizeof(RGBAColor) == sizeof(float) * 4, "Invalid data type!");
	if (!bitstream.write(perVertexColors.data(), sizeof(RGBAColor) * numberPerVertexColors))
	{
		return false;
	}

	const unsigned int numberTriangleFaces = (unsigned int)(triangleFaces.size());
	if (!bitstream.write<unsigned int>(numberTriangleFaces))
	{
		return false;
	}

	static_assert(sizeof(Rendering::TriangleFace) == sizeof(Index32) * 3, "Invalid data type!");
	if (!bitstream.write(triangleFaces.data(), triangleFaces.size() * sizeof(Rendering::TriangleFace)))
	{
		return false;
	}

	if (textureFrame.isValid())
	{
		std::vector<uint8_t> encodedTextureFrameBuffer;
		if (!IO::Image::encodeImage(textureFrame, "jpg", encodedTextureFrameBuffer))
		{
			return false;
		}

		const unsigned int textureFrameSize = (unsigned int)(encodedTextureFrameBuffer.size());
		if (!bitstream.write<unsigned int>(textureFrameSize))
		{
			return false;
		}

		if (!bitstream.write(encodedTextureFrameBuffer.data(), encodedTextureFrameBuffer.size()))
		{
			return false;
		}
	}
	else
	{
		constexpr unsigned int textureFrameSize = 0u;
		if (!bitstream.write<unsigned int>(textureFrameSize))
		{
			return false;
		}
	}

	return true;
}

bool LegacyMetaportationScannerExperience::writeObjectPointsToStream(const Vectors3& objectPoints, const Indices32& objectPointIds, IO::OutputBitstream& bitstream)
{
	ocean_assert(objectPoints.size() == objectPointIds.size());

	if (objectPoints.empty() || objectPoints.size() != objectPointIds.size())
	{
		return false;
	}

	if (!bitstream.write<unsigned long long>(objectPointsTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write<unsigned int>((unsigned int)(objectPoints.size())))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		if (!bitstream.write(objectPoints.data(), objectPoints.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 objectPointsF;
		objectPointsF.reserve(objectPoints.size());
		for (const Vector3& objectPoint : objectPoints)
		{
			objectPointsF.emplace_back(float(objectPoint.x()), float(objectPoint.y()), float(objectPoint.z()));
		}

		if (!bitstream.write(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
		{
			return  false;
		}
	}

	if (!bitstream.write<unsigned int>((unsigned int)(objectPointIds.size())))
	{
		return false;
	}

	if (!bitstream.write(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool LegacyMetaportationScannerExperience::readTransformationFromStream(IO::InputBitstream& bitstream, HomogenousMatrix4& world_T_camera)
{
	unsigned long long tag;
	if (!bitstream.read<unsigned long long>(tag) || tag != transformationTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	HomogenousMatrixD4 worldD_T_cameraD;
	if (!bitstream.read(worldD_T_cameraD.data(), sizeof(double) * 16))
	{
		return false;
	}

	world_T_camera = HomogenousMatrix4(worldD_T_cameraD);

	return world_T_camera.isValid();
}

bool LegacyMetaportationScannerExperience::readMeshFromStream(IO::InputBitstream& bitstream, Index32& meshId, unsigned int& remainingMeshes, HomogenousMatrix4& world_T_mesh, Vectors3& vertices, Vectors3& perVertexNormals, RGBAColors& perVertexColors, Vectors2& textureCoordinates, Rendering::TriangleFaces& triangleFaces, Frame& textureFrame)
{
	constexpr unsigned int maximalVertices = 10u * 1000 * 1000u;
	constexpr unsigned int maximalImageSize = 20u * 1024u * 1024u;

	unsigned long long tag;
	if (!bitstream.read<unsigned long long>(tag) || tag != meshTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 2ull)
	{
		return false;
	}

	if (!bitstream.read<Index32>(meshId))
	{
		return false;
	}

	if (!bitstream.read<unsigned int>(remainingMeshes))
	{
		return false;
	}

	if (!readTransformationFromStream(bitstream, world_T_mesh))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		unsigned int numberVertices = 0u;
		if (!bitstream.read<unsigned int>(numberVertices) || numberVertices > maximalVertices)
		{
			return false;
		}

		vertices.resize(numberVertices);
		if (!bitstream.read(vertices.data(), vertices.size() * sizeof(VectorF3)))
		{
			return false;
		}

		unsigned int numberNormals = 0u;
		if (!bitstream.read<unsigned int>(numberNormals) || numberNormals > maximalVertices)
		{
			return false;
		}

		if (numberVertices != numberNormals && numberNormals != 0u)
		{
			return false;
		}

		perVertexNormals.resize(numberNormals);
		if (!bitstream.read(perVertexNormals.data(), perVertexNormals.size() * sizeof(VectorF3)))
		{
			return false;
		}

		unsigned int numberTextureCoordinates = 0u;
		if (!bitstream.read<unsigned int>(numberTextureCoordinates) || numberTextureCoordinates > maximalVertices)
		{
			return false;
		}

		if (numberVertices != numberTextureCoordinates && numberTextureCoordinates != 0u)
		{
			return false;
		}

		textureCoordinates.resize(numberTextureCoordinates);
		if (!bitstream.read(textureCoordinates.data(), textureCoordinates.size() * sizeof(VectorF2)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		unsigned int numberVertices = 0u;
		if (!bitstream.read<unsigned int>(numberVertices) || numberVertices > maximalVertices)
		{
			return false;
		}

		VectorsF3 verticesF(numberVertices);
		if (!bitstream.read(verticesF.data(), verticesF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		unsigned int numberNormals = 0u;
		if (!bitstream.read<unsigned int>(numberNormals) || numberNormals > maximalVertices)
		{
			return false;
		}

		if (numberVertices != numberNormals && numberNormals != 0u)
		{
			return false;
		}

		VectorsF3 perVertexNormalsF(numberNormals);
		if (!bitstream.read(perVertexNormalsF.data(), perVertexNormalsF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		unsigned int numberTextureCoordinates = 0u;
		if (!bitstream.read<unsigned int>(numberTextureCoordinates) || numberTextureCoordinates > maximalVertices)
		{
			return false;
		}

		if (numberVertices != numberTextureCoordinates && numberTextureCoordinates != 0u)
		{
			return false;
		}

		VectorsF2 textureCoordinatesF(numberTextureCoordinates);
		if (!bitstream.read(textureCoordinatesF.data(), textureCoordinatesF.size() * sizeof(VectorF2)))
		{
			return false;
		}

		vertices.reserve(verticesF.size());
		for (const VectorF3& vertexF : verticesF)
		{
			vertices.emplace_back(Scalar(vertexF.x()), Scalar(vertexF.y()), Scalar(vertexF.z()));
		}

		perVertexNormals.reserve(perVertexNormalsF.size());
		for (const VectorF3& perVertexNormalF : perVertexNormalsF)
		{
			perVertexNormals.emplace_back(Scalar(perVertexNormalF.x()), Scalar(perVertexNormalF.y()), Scalar(perVertexNormalF.z()));
		}

		textureCoordinates.reserve(textureCoordinatesF.size());
		for (const VectorF2& textureCoordinateF : textureCoordinatesF)
		{
			textureCoordinates.emplace_back(Scalar(textureCoordinateF.x()), Scalar(textureCoordinateF.y()));
		}
	}

	unsigned int numberPerVertexColors = 0u;
	if (!bitstream.read<unsigned int>(numberPerVertexColors))
	{
		return false;
	}

	if ((unsigned int)(vertices.size()) != numberPerVertexColors && numberPerVertexColors != 0u)
	{
		return false;
	}

	static_assert(sizeof(RGBAColor) == sizeof(float) * 4, "Invalid data type!");
	perVertexColors.resize(numberPerVertexColors);
	if (!bitstream.read(perVertexColors.data(), sizeof(RGBAColor) * numberPerVertexColors))
	{
		return false;
	}

	unsigned int numberTriangleFaces = 0u;
	if (!bitstream.read<unsigned int>(numberTriangleFaces) || numberTriangleFaces == 0u || numberTriangleFaces > maximalVertices)
	{
		return false;
	}

	triangleFaces.resize(numberTriangleFaces);
	if (!bitstream.read(triangleFaces.data(), triangleFaces.size() * sizeof(Rendering::TriangleFace)))
	{
		return false;
	}

	unsigned int textureFrameSize = 0u;
	if (!bitstream.read<unsigned int>(textureFrameSize))
	{
		return false;
	}

	if (textureFrameSize != 0u)
	{
		if (textureFrameSize >= maximalImageSize)
		{
			return false;
		}

		std::vector<uint8_t> encodedTextureFrameBuffer(textureFrameSize);
		if (!bitstream.read(encodedTextureFrameBuffer.data(), encodedTextureFrameBuffer.size()))
		{
			return false;
		}

		textureFrame = IO::Image::decodeImage(encodedTextureFrameBuffer.data(), encodedTextureFrameBuffer.size(), "jpg");

		if (!textureFrame.isValid())
		{
			return false;
		}
	}

	return true;
}

bool LegacyMetaportationScannerExperience::determineMeshIdInStream(IO::InputBitstream& bitstream, Index32& meshId)
{
	const IO::ScopedInputBitstream scopedInputStream(bitstream);

	unsigned long long tag;
	if (!bitstream.read<unsigned long long>(tag) || tag != meshTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 2ull)
	{
		return false;
	}

	if (!bitstream.read<Index32>(meshId))
	{
		return false;
	}

	return true;
}

bool LegacyMetaportationScannerExperience::readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds)
{
	constexpr unsigned int maximalObjectPoints = 100u * 1000u;

	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != objectPointsTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberObjectPoints = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	if (numberObjectPoints > maximalObjectPoints)
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		objectPoints.resize(numberObjectPoints);

		if (!bitstream.read(objectPoints.data(), objectPoints.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 objectPointsF(numberObjectPoints);
		if (!bitstream.read(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		objectPoints.clear();
		objectPoints.reserve(objectPointsF.size());

		for (VectorF3& objectPointF : objectPointsF)
		{
			objectPoints.emplace_back(Scalar(objectPointF.x()), Scalar(objectPointF.y()), Scalar(objectPointF.z()));
		}
	}

	unsigned int numberObjectPointIds = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPointIds))
	{
		return false;
	}

	if (numberObjectPoints != numberObjectPointIds)
	{
		return false;
	}

	objectPointIds.resize(numberObjectPointIds);

	if (!bitstream.read(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

}

}
