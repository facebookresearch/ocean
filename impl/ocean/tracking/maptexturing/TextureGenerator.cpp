/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/TextureGenerator.h"

#include "ocean/cv/FrameConverterColorMap.h"

#include "ocean/io/File.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/Utilities.h"

#include "ocean/rendering/BitmapFramebuffer.h"
#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/TextureFramebuffer.h"
#include "ocean/rendering/TriangleFace.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/rendering/glescenegraph/GLESFrameTexture2D.h"
#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESTextureFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESTriangles.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

#include "ocean/scenedescription/sdx/x3d/Utilities.h"

#include "ocean/tracking/Utilities.h"

#if defined(_WINDOWS)
	#include "ocean/platform/win/System.h"
	#include "ocean/platform/win/Utilities.h"

	#include "ocean/rendering/glescenegraph/windows/GLESBitmapFramebuffer.h"
#endif

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

bool TextureGenerator::updateMesh(const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement, const bool skipIfVertexNumberHasNotChanged)
{
	const Devices::SceneTracker6DOF::SceneElementMeshes& sceneElementMeshes = (const Devices::SceneTracker6DOF::SceneElementMeshes&)(*sceneElement);

	ocean_assert(!lock_.isLocked() && "Multi-threading should be handled outside of this class");
	const ScopedLock scopedLock(lock_);

	SceneMeshVertexCounterMap newSceneMeshVertexCounterMap;

	ocean_assert(!anySceneMeshHasChanged_);
	anySceneMeshHasChanged_ = false;

	size_t expectedTriangles = 0;
	for (const Devices::SceneTracker6DOF::SceneElementMeshes::SharedMesh& mesh : sceneElementMeshes.meshes())
	{
		ocean_assert(mesh);
		expectedTriangles += mesh->triangleIndices().size() / 3;

		const SceneMeshVertexCounterMap::const_iterator iLastSceneMesh = latestSceneMeshVertexCounterMap_.find(mesh->meshId());
		if (iLastSceneMesh == latestSceneMeshVertexCounterMap_.cend() || mesh->vertices().size() != iLastSceneMesh->second)
		{
			anySceneMeshHasChanged_ = true;
		}

		newSceneMeshVertexCounterMap[mesh->meshId()] = mesh->vertices().size();
	}

	if (!skipIfVertexNumberHasNotChanged || latestSceneMeshVertexCounterMap_.size() != newSceneMeshVertexCounterMap.size())
	{
		anySceneMeshHasChanged_ = true;
	}

	latestSceneMeshVertexCounterMap_ = std::move(newSceneMeshVertexCounterMap);

	if (!anySceneMeshHasChanged_)
	{
		return true;
	}

	const size_t capacity = std::max(size_t(1024 * 1024), expectedTriangles);
	if (triangleMap_.max_size() <= capacity)
	{
		if (triangleMap_.empty())
		{
			triangleMap_.reserve(capacity);
		}
		else
		{
			triangleMap_.reserve(std::max(capacity, triangleMap_.max_size() * 2));
		}
	}

	if (triangles_.size() < triangleIdCounter_ + expectedTriangles)
	{
		triangles_.resize(triangleIdCounter_ + expectedTriangles + 1024 * 512);
	}

	for (const Devices::SceneTracker6DOF::SceneElementMeshes::SharedMesh& mesh : sceneElementMeshes.meshes())
	{
		ocean_assert(mesh->triangleIndices().size() % 3 == 0);
		for (Indices32::const_iterator iIndex = mesh->triangleIndices().cbegin(); iIndex != mesh->triangleIndices().cend(); /*noop*/)
		{
			const Vector3 vertex0(mesh->world_T_mesh() * mesh->vertices()[*iIndex++]);
			const Vector3 vertex1(mesh->world_T_mesh() * mesh->vertices()[*iIndex++]);
			const Vector3 vertex2(mesh->world_T_mesh() * mesh->vertices()[*iIndex++]);

			if (vertex0 == vertex1 || vertex0 == vertex2 || vertex1 == vertex2)
			{
				continue;
			}

			const HashableTriangle hashableTriangle(vertex0, vertex1, vertex2);

			TriangleMap::iterator iTriangle = triangleMap_.find(hashableTriangle);

			if (iTriangle != triangleMap_.cend())
			{
				// the triangle exists already

				iTriangle->second.frameIndex_ = frameIndex_;
			}
			else
			{
				// the triangle is new

				Index32 newTriangleId;
				if (freeTriangleIds_.empty())
				{
					newTriangleId = ++triangleIdCounter_;
				}
				else
				{
					newTriangleId = freeTriangleIds_.back();
					freeTriangleIds_.pop_back();
				}

				triangleMap_.emplace(hashableTriangle, TriangleObject(newTriangleId, frameIndex_));

				triangles_[newTriangleId] = hashableTriangle;

				newTriangleIds_.emplace_back(newTriangleId);
			}
		}
	}

	for (TriangleMap::const_iterator iTriangle = triangleMap_.cbegin(); iTriangle != triangleMap_.cend(); ++iTriangle)
	{
		const TriangleObject& triangleObject = iTriangle->second;

		if (triangleObject.frameIndex_ != frameIndex_)
		{
			retiredTriangleIds_.emplace_back(triangleObject.id_);
		}
	}

	Log::debug() << "Triangle ratio: " << double(triangleMap_.size()) / double(expectedTriangles) * 100.0 << "%";

	return true;
}

bool TextureGenerator::processFrame(Frame&& rgbFrame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Rendering::EngineRef& engine)
{
	ocean_assert(rgbFrame.isValid() && anyCamera.isValid());
	ocean_assert(rgbFrame.width() == anyCamera.width() && rgbFrame.height() == anyCamera.height());
	ocean_assert(rgbFrame.pixelFormat() == FrameType::FORMAT_RGB24);

	ocean_assert(!lock_.isLocked() && "Multi-threading should be handled outside of this class");
	const ScopedLock scopedLock(lock_);

	if (engine->framebuffers().empty() && dummyFramebuffer_.isNull())
	{
		// we have a platform without a main framebuffer, so we need to create a dummy framebuffer

		dummyFramebuffer_ = engine->createFramebuffer(Rendering::Framebuffer::FRAMEBUFFER_BITMAP);
		dummyFramebuffer_->setView(engine->factory().createPerspectiveView());
	}

	if (renderingVertexSet_.isNull())
	{
		renderingVertexSet_ = engine->factory().createVertexSet();
		renderingTriangles_ = engine->factory().createTriangles();
		ocean_assert(renderingVertexSet_ && renderingTriangles_);

		renderingTriangles_->setVertexSet(renderingVertexSet_);
	}

	if (triangleIdCounter_ == 0u)
	{
		return true;
	}

	if (anySceneMeshHasChanged_)
	{
		const unsigned int numberTriangleIds = triangleIdCounter_ + 1u;

		renderingVertexSet_->setVertices((const Vector3*)(triangles_.data()), numberTriangleIds * 3u);
		renderingTriangles_->setFaces(numberTriangleIds);

		anySceneMeshHasChanged_ = false;
	}

	trianglesManagerRenderer_.initialize(*engine, 1024u, 1024u); // **TODO**

	if (texturingMode_ == TM_KEEP_RETIRED)
	{
		if (!processFrameKeepRetired(std::move(rgbFrame), anyCamera, world_T_camera, engine))
		{
			return false;
		}
	}
	else
	{
		ocean_assert(texturingMode_ == TM_REDRAW_RETIRED);

		if (!processFrameRedrawRetired(std::move(rgbFrame), anyCamera, world_T_camera, engine))
		{
			return false;
		}
	}

	++frameIndex_;

	return true;
}

bool TextureGenerator::render(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Rendering::EngineRef& engine, const bool renderTexturedTrianglesOnly, Frame& frame)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(world_T_camera.isValid());

	ocean_assert(!lock_.isLocked() && "Multi-threading should be handled outside of this class");
	const ScopedLock scopedLock(lock_);

#if 1

	OCEAN_SUPPRESS_UNUSED_WARNING(renderTexturedTrianglesOnly);

	if (!meshRenderer_.initialize(*engine))
	{
		return false;
	}

	const unsigned int numberTriangleIds = triangleIdCounter_ + 1u;

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
		const TextureFramebufferMap& textureFramebufferMap = texturedTrianglesRendererBackup_.textureFramebufferMap();
#else
		const TextureFramebufferMap& textureFramebufferMap = texturedTrianglesRenderer_.textureFramebufferMap();
#endif

	return meshRenderer_.render(*engine, renderingVertexSet_, renderingTriangles_, numberTriangleIds, anyCamera, world_T_camera, textureFramebufferMap, trianglesManagerRenderer_.textureFramebuffer(), textureAtlas_, &frame);

#else

	if (dummyFramebuffer_.isNull())
	{
		return false;
	}

	dummyFramebuffer_->setViewport(0u, 0u, anyCamera.width(), anyCamera.height());

	const Rendering::PerspectiveViewRef view = dummyFramebuffer_->view();
	if (view.isNull())
	{
		return false;
	}

	view->setProjectionMatrix(SquareMatrix4::projectionMatrix(anyCamera, view->nearDistance(), view->farDistance()));
	view->setTransformation(world_T_camera);

	if (dummyFramebuffer_->scenes().empty())
	{
		dummyFramebuffer_->addScene(engine->factory().createScene());
	}

	const Rendering::SceneRef scene = dummyFramebuffer_->scenes().front();

	scene->clear();

	Indices32 meshIds = texturedTrianglesRenderer_.meshIds();

	if (!renderTexturedTrianglesOnly)
	{
		meshIds.emplace_back(Index32(-1));
	}

	for (const Index32& meshId : meshIds)
	{
		Mesh mesh;
		Frame meshTexture;

		if (exportMesh(meshId, mesh, meshTexture))
		{
			const Rendering::VertexSetRef vertexSet = engine->factory().createVertexSet();
			vertexSet->setVertices(mesh.vertices_);

			if (!mesh.textureCoordinates_.empty())
			{
				vertexSet->setTextureCoordinates(mesh.textureCoordinates_, 0u);
			}

			const Rendering::TrianglesRef triangles = engine->factory().createTriangles();
			triangles->setFaces(mesh.triangleFaces_);
			triangles->setVertexSet(vertexSet);

			const Rendering::AttributeSetRef attributes = engine->factory().createAttributeSet();

			if (meshTexture)
			{
				const Rendering::FrameTexture2DRef texture = engine->factory().createFrameTexture2D();
				texture->setTexture(std::move(meshTexture));

				const Rendering::TexturesRef textures = engine->factory().createTextures();
				textures->addTexture(texture);

				attributes->addAttribute(textures);
			}

			const Rendering::GeometryRef geometry = engine->factory().createGeometry();
			geometry->addRenderable(triangles, attributes);

			scene->addChild(geometry);
		}
	}

	dummyFramebuffer_->render();

#ifdef _WINDOWS
	dummyFramebuffer_.force<Rendering::GLESceneGraph::Windows::GLESBitmapFramebuffer>().textureFramebuffer().force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().copyColorTextureToFrame(frame);
#endif

	return frame.isValid();

#endif
}

Indices32 TextureGenerator::meshIds() const
{
	ocean_assert(!lock_.isLocked() && "Multi-threading should be handled outside of this class");
	const ScopedLock scopedLock(lock_);

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	return texturedTrianglesRendererBackup_.meshIds();
#else
	return texturedTrianglesRenderer_.meshIds();
#endif
}

bool TextureGenerator::exportMesh(const Index32 meshId, Mesh& mesh, Frame& meshTexture)
{
	ocean_assert(!lock_.isLocked() && "Multi-threading should be handled outside of this class");
	const ScopedLock scopedLock(lock_);

	if (!extractMeshesWithAtlasTextureCoordinates(meshId, mesh))
	{
		return false;
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (!texturedTrianglesRendererBackup_.latestTexture(meshId, meshTexture))
#else
	if (!texturedTrianglesRenderer_.latestTexture(meshId, meshTexture))
#endif
	{
		meshTexture.release();
	}

	return true;
}

bool TextureGenerator::exportMeshs(const std::string& filename, const bool exportTexturedTrianglesOnly)
{
	ocean_assert(!lock_.isLocked() && "Multi-threading should be handled outside of this class");
	const ScopedLock scopedLock(lock_);

	MeshMap meshMap;

	if (!extractMeshesWithAtlasTextureCoordinates(meshMap, exportTexturedTrianglesOnly))
	{
		return false;
	}

	std::unordered_map<Index32, std::string> textureUrlMap;
	textureUrlMap.reserve(meshMap.size());

	Frame textureFrame;

	for (MeshMap::const_iterator iMesh = meshMap.cbegin(); iMesh != meshMap.cend(); ++iMesh)
	{
		if (iMesh->second.triangleFaces_.empty())
		{
			continue;
		}

		const Index32& meshId = iMesh->first;

		if (meshId != Index32(-1))
		{
#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
			if (!texturedTrianglesRendererBackup_.latestTexture(meshId, textureFrame))
#else
			if (!texturedTrianglesRenderer_.latestTexture(meshId, textureFrame))
#endif
			{
				return false;
			}

			const IO::File textureFile = IO::File(IO::File(filename).base() + "_texture_" + String::toAString(meshId, 3u) + ".png");

			ocean_assert(textureFrame);
			if (!Media::Utilities::saveImage(textureFrame, textureFile(), false))
			{
				return false;
			}

			textureUrlMap[meshId] = textureFile.name();
		}
		else
		{
			textureUrlMap[meshId] = std::string();
		}
	}

	std::ofstream stream(filename.c_str(), std::ios::binary);

	stream << "#X3D V3.0 utf8\n\n";

	for (MeshMap::const_iterator iMesh = meshMap.cbegin(); iMesh != meshMap.cend(); ++iMesh)
	{
		const Index32& meshId = iMesh->first;

		if (iMesh->second.triangleFaces_.empty())
		{
			continue;
		}

		const Mesh& mesh = iMesh->second;

		RGBAColor color(false);

		Vectors3 normals;

		if (meshId != Index32(-1))
		{
			normals = Vectors3(mesh.vertices_.size(), Vector3(0, 0, 1)); // **TODO**
		}
		else
		{
			color = RGBAColor(0.7f, 0.7f, 0.7f);

			for (const Rendering::TriangleFace& triangleFace : mesh.triangleFaces_)
			{
				const Vector3& vertex0 = mesh.vertices_[triangleFace[0]];
				const Vector3& vertex1 = mesh.vertices_[triangleFace[1]];
				const Vector3& vertex2 = mesh.vertices_[triangleFace[2]];

				const Vector3 normal = (vertex1 - vertex0).cross(vertex2 - vertex0).normalizedOrZero();

				normals.emplace_back(normal);
				normals.emplace_back(normal);
				normals.emplace_back(normal);
			}
		}

		ocean_assert(textureUrlMap.find(meshId) != textureUrlMap.cend());
		const std::string& textureUrl = textureUrlMap[meshId];

		if (!SceneDescription::SDX::X3D::Utilities::writeIndexedFaceSet(stream, mesh.vertices_, mesh.triangleFaces_, color, normals, RGBAColors(), mesh.textureCoordinates_, textureUrl))
		{
			return false;
		}

		stream << std::endl;
	}

	return true;
}

bool TextureGenerator::processFrameKeepRetired(Frame&& rgbFrame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Rendering::EngineRef& engine)
{
	ocean_assert(rgbFrame.isValid() && anyCamera.isValid());
	ocean_assert(rgbFrame.width() == anyCamera.width() && rgbFrame.height() == anyCamera.height());
	ocean_assert(rgbFrame.pixelFormat() == FrameType::FORMAT_RGB24);

	const unsigned int downsampledWidth = anyCamera.width() / 4u;
	const unsigned int downsampledHeight = anyCamera.height() / 4u;

	const unsigned int numberTriangleIds = triangleIdCounter_ + 1u;

	// first, we update the manager with all new triangle ids

	trianglesManagerRenderer_.updateNewTriangles(newTriangleIds_);
	newTriangleIds_.clear();


	// now, we update the manager with all retired triangle ids

	trianglesManagerRenderer_.updateRetiredTriangles(retiredTriangleIds_);
	retiredTriangleIds_.clear();

	constexpr Scalar nearDistance = Scalar(0.01);
	constexpr Scalar farDistance = Scalar(1000);
	const SquareMatrix4 projectionMatrix = SquareMatrix4::projectionMatrix(anyCamera, nearDistance, farDistance);


	// now, we render all triangles with their individual id

	if (!activeTrianglesIdRenderer_.initialize(*engine))
	{
		return false;
	}

	if (!activeTrianglesIdRenderer_.render(renderingVertexSet_, renderingTriangles_, numberTriangleIds, trianglesManagerRenderer_.textureFramebuffer(), projectionMatrix, world_T_camera, downsampledWidth, downsampledHeight, 126u))
	{
		return false;
	}

	if (!anyTrianglesIdRenderer_.initialize(*engine))
	{
		return false;
	}

	if (!anyTrianglesIdRenderer_.render(renderingVertexSet_, renderingTriangles_, numberTriangleIds, trianglesManagerRenderer_.textureFramebuffer(), projectionMatrix, world_T_camera, downsampledWidth, downsampledHeight, 10u))
	{
		return false;
	}


	// now, we determine the down-sampled and filtered depth buffer

	if (!depthBufferRenderer_.downsampleAndFilterDepthBuffer(*engine, anyTrianglesIdRenderer_.textureFramebuffer(), 3u))
	{
		return false;
	}


	// now, we determine which triangles are actually visible as these triangles will be textured

	if (!visibleTrianglesRenderer_.render(*engine, activeTrianglesIdRenderer_.textureFramebuffer(), depthBufferRenderer_.textureFramebuffer(), nearDistance, farDistance))
	{
		return false;
	}


	// now, tell the manager which triangles can to be textured

	if (!trianglesManagerRenderer_.updateTexturedTriangles(visibleTrianglesRenderer_.textureFramebuffer()))
	{
		return false;
	}


	// now, we texture all currently visible triangles

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	if (!texturedTrianglesRendererBackup_.update(*engine, std::move(rgbFrame)))
	{
		return false;
	}

	if (!texturedTrianglesRendererBackup_.render(*engine, triangles_.data(), numberTriangleIds, anyCamera, projectionMatrix, world_T_camera, visibleTrianglesRenderer_.textureFramebuffer(), textureAtlas_))
	{
		return false;
	}

#else

	if (!texturedTrianglesRenderer_.update(*engine, std::move(rgbFrame)))
	{
		return false;
	}

	if (!texturedTrianglesRenderer_.render(*engine, numberTriangleIds, projectionMatrix, world_T_camera, renderingVertexSet_, renderingTriangles_, visibleTrianglesRenderer_.textureFramebuffer(), textureAtlas_))
	{
		return false;
	}

#endif

	// now, we determine which of the retired triangles can be deleted

	ocean_assert(deletedTriangleIds_.empty());
	if (!retiredTrianglesRenderer_.render(*engine, renderingVertexSet_, renderingTriangles_, numberTriangleIds, projectionMatrix, world_T_camera, trianglesManagerRenderer_.textureFramebuffer(), depthBufferRenderer_.textureFramebuffer(), nearDistance, farDistance, deletedTriangleIds_))
	{
		return false;
	}

	for (const Index32& deletedTriangleId : deletedTriangleIds_)
	{
		freeTriangleIds_.emplace_back(deletedTriangleId);

		ocean_assert(deletedTriangleId < numberTriangleIds);
		ocean_assert(deletedTriangleId < triangles_.size());
		const HashableTriangle& triangle = triangles_[deletedTriangleId];

		ocean_assert(triangleMap_.find(triangle) != triangleMap_.cend());
		triangleMap_.erase(triangle);
	}


	// now, we can tell the manager which triangles have been deleted

	trianglesManagerRenderer_.updateDeletedTriangles(deletedTriangleIds_);

#ifdef OCEAN_DEBUG
	ConstArrayAccessor<uint8_t> debugStateAccessor = trianglesManagerRenderer_.triangleStates(numberTriangleIds);

	for (const Index32& freeTriangleId : freeTriangleIds_)
	{
		ocean_assert(debugStateAccessor.data()[freeTriangleId] == 0u);
	}
#endif

	deletedTriangleIds_.clear();

	return true;
}

bool TextureGenerator::processFrameRedrawRetired(Frame&& rgbFrame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Rendering::EngineRef& engine)
{
	ocean_assert(rgbFrame.isValid() && anyCamera.isValid());
	ocean_assert(rgbFrame.width() == anyCamera.width() && rgbFrame.height() == anyCamera.height());
	ocean_assert(rgbFrame.pixelFormat() == FrameType::FORMAT_RGB24);

	const unsigned int downsampledWidth = anyCamera.width() / 4u;
	const unsigned int downsampledHeight = anyCamera.height() / 4u;

	const unsigned int numberTriangleIds = triangleIdCounter_ + 1u;

	// first, we update the manager with all new triangle ids

	trianglesManagerRenderer_.updateNewTriangles(newTriangleIds_);

	if (!newTriangleIds_.empty())
	{
		if (!meshRenderer_.initialize(*engine))
		{
			return false;
		}

		const Scalar fovX = anyCamera.fovX();
		const AnyCameraPinhole wideAngleCamera(PinholeCamera(anyCamera.width() * 120u / 100u, anyCamera.height() * 120u / 100u, fovX + Numeric::deg2rad(20))); // **TODO** just increasing the viewport of the existing camera

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
		const TextureFramebufferMap& textureFramebufferMap = texturedTrianglesRendererBackup_.textureFramebufferMap();
#else
		const TextureFramebufferMap& textureFramebufferMap = texturedTrianglesRenderer_.textureFramebufferMap();
#endif

		if (!meshRenderer_.render(*engine, renderingVertexSet_, renderingTriangles_, numberTriangleIds, wideAngleCamera, world_T_camera, textureFramebufferMap, trianglesManagerRenderer_.textureFramebuffer(), textureAtlas_))
		{
			return false;
		}

		const unsigned int numberMeshes = (numberTriangleIds + textureAtlas_.numberTriangles() - 1u) / textureAtlas_.numberTriangles();

		texturedTrianglesRendererBackup_.update(*engine, Frame());
		texturedTrianglesRendererBackup_.render(*engine, triangles_.data(), numberMeshes, newTriangleIds_, wideAngleCamera, world_T_camera, textureFramebufferMap, meshRenderer_.textureFramebuffer(), textureAtlas_);

		trianglesManagerRenderer_.updateTexturedTriangles(newTriangleIds_);

		newTriangleIds_.clear();
	}
	else
	{
		// we can skip some frames
		if (frameIndex_ % 2u == 0u)
		{
			return true;
		}
	}

	// we delete all retired triangles

	for (const Index32& deletedTriangleId : retiredTriangleIds_)
	{
		freeTriangleIds_.emplace_back(deletedTriangleId);

		ocean_assert(deletedTriangleId < numberTriangleIds);
		ocean_assert(deletedTriangleId < triangles_.size());
		const HashableTriangle& triangle = triangles_[deletedTriangleId];

		ocean_assert(triangleMap_.find(triangle) != triangleMap_.cend());
		triangleMap_.erase(triangle);
	}

	// now, we can tell the manager which triangles have been deleted

	trianglesManagerRenderer_.updateDeletedTriangles(retiredTriangleIds_);
	retiredTriangleIds_.clear();


	// now, we render all triangles with their individual id

	constexpr Scalar nearDistance = Scalar(0.01);
	constexpr Scalar farDistance = Scalar(1000);
	const SquareMatrix4 projectionMatrix = SquareMatrix4::projectionMatrix(anyCamera, nearDistance, farDistance);

	if (!activeTrianglesIdRenderer_.initialize(*engine))
	{
		return false;
	}

	if (!activeTrianglesIdRenderer_.render(renderingVertexSet_, renderingTriangles_, numberTriangleIds, trianglesManagerRenderer_.textureFramebuffer(), projectionMatrix, world_T_camera, downsampledWidth, downsampledHeight, 126u))
	{
		return false;
	}

	if (!anyTrianglesIdRenderer_.initialize(*engine))
	{
		return false;
	}

	if (!anyTrianglesIdRenderer_.render(renderingVertexSet_, renderingTriangles_, numberTriangleIds, trianglesManagerRenderer_.textureFramebuffer(), projectionMatrix, world_T_camera, downsampledWidth, downsampledHeight, 10u))
	{
		return false;
	}


	// now, we determine the down-sampled and filtered depth buffer

	if (!depthBufferRenderer_.downsampleAndFilterDepthBuffer(*engine, anyTrianglesIdRenderer_.textureFramebuffer(), 3u))
	{
		return false;
	}


	// now, we determine which triangles are actually visible as these triangles will be textured

	if (!visibleTrianglesRenderer_.render(*engine, activeTrianglesIdRenderer_.textureFramebuffer(), depthBufferRenderer_.textureFramebuffer(), nearDistance, farDistance))
	{
		return false;
	}


	// now, tell the manager which triangles can to be textured

	if (!trianglesManagerRenderer_.updateTexturedTriangles(visibleTrianglesRenderer_.textureFramebuffer()))
	{
		return false;
	}


	// now, we texture all currently visible triangles

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	if (!texturedTrianglesRendererBackup_.update(*engine, std::move(rgbFrame)))
	{
		return false;
	}

	if (!texturedTrianglesRendererBackup_.render(*engine, triangles_.data(), numberTriangleIds, anyCamera, projectionMatrix, world_T_camera, visibleTrianglesRenderer_.textureFramebuffer(), textureAtlas_))
	{
		return false;
	}

#else

	if (!texturedTrianglesRenderer_.update(*engine, std::move(rgbFrame)))
	{
		return false;
	}

	if (!texturedTrianglesRenderer_.render(*engine, numberTriangleIds, projectionMatrix, world_T_camera, renderingVertexSet_, renderingTriangles_, visibleTrianglesRenderer_.textureFramebuffer(), textureAtlas_))
	{
		return false;
	}

#endif

	ocean_assert(deletedTriangleIds_.empty());

	return true;
}

bool TextureGenerator::extractMeshesWithAtlasTextureCoordinates(const Index32 meshId, Mesh& mesh)
{
	mesh.vertices_.clear();
	mesh.triangleFaces_.clear();
	mesh.textureCoordinates_.clear();

	bool hasAtLeastOneTriangle = false;

	const unsigned int numberTriangleIds = triangleIdCounter_ + 1u;

	const ScopedConstMemoryAccessor<uint8_t> triangleStates(trianglesManagerRenderer_.triangleStates(numberTriangleIds));

	if (meshId != Index32(-1))
	{
		const Index32 firstTriangleId = textureAtlas_.numberTriangles() * meshId;
		const Index32 endTriangleId = min(firstTriangleId + textureAtlas_.numberTriangles(), numberTriangleIds);

		Vector2 triangleTextureCoordinates[3];

		for (Index32 triangleId = firstTriangleId; triangleId < endTriangleId; ++triangleId)
		{
			ocean_assert(triangleId < triangleStates.size());
			if (triangleStates[triangleId] == 255u || triangleStates[triangleId] == 64u)
			{
				mesh.triangleFaces_.emplace_back(Index32(mesh.vertices_.size()));

				const Index32 meshTriangleId = triangleId % textureAtlas_.numberTriangles();

				textureAtlas_.triangleAtlasTextureCoordinates<true>(meshTriangleId, triangleTextureCoordinates);

				mesh.textureCoordinates_.emplace_back(triangleTextureCoordinates[0]);
				mesh.textureCoordinates_.emplace_back(triangleTextureCoordinates[1]);
				mesh.textureCoordinates_.emplace_back(triangleTextureCoordinates[2]);

				const HashableTriangle& triangle = triangles_[triangleId];

				mesh.vertices_.emplace_back(triangle.vertex0());
				mesh.vertices_.emplace_back(triangle.vertex1());
				mesh.vertices_.emplace_back(triangle.vertex2());

				hasAtLeastOneTriangle = true;
			}
		}
	}
	else
	{
		for (TriangleMap::const_iterator iTriangle = triangleMap_.cbegin(); iTriangle != triangleMap_.cend(); ++iTriangle)
		{
			ocean_assert(iTriangle->second.id_ < triangleStates.size());
			if (triangleStates[iTriangle->second.id_] >= 126u && triangleStates[iTriangle->second.id_] <= 130u)
			{
				mesh.triangleFaces_.emplace_back(Index32(mesh.vertices_.size()));

				mesh.vertices_.emplace_back(iTriangle->first.vertex0());
				mesh.vertices_.emplace_back(iTriangle->first.vertex1());
				mesh.vertices_.emplace_back(iTriangle->first.vertex2());

				hasAtLeastOneTriangle = true;
			}
		}
	}

	return hasAtLeastOneTriangle;
}

bool TextureGenerator::extractMeshesWithAtlasTextureCoordinates(MeshMap& meshMap, const bool exportTexturedTrianglesOnly)
{
	for (MeshMap::iterator iMesh = meshMap.begin(); iMesh != meshMap.end(); ++iMesh)
	{
		Mesh& mesh = iMesh->second;

		mesh.vertices_.clear();
		mesh.triangleFaces_.clear();
		mesh.textureCoordinates_.clear();
	}

	bool hasAtLeastOneTriangle = false;

	const unsigned int numberTriangleIds = triangleIdCounter_ + 1u;

	const ScopedConstMemoryAccessor<uint8_t> triangleStates(trianglesManagerRenderer_.triangleStates(numberTriangleIds));

	for (TriangleMap::const_iterator iTriangle = triangleMap_.cbegin(); iTriangle != triangleMap_.cend(); ++iTriangle)
	{
		ocean_assert(iTriangle->second.id_ < triangleStates.size());
		if (triangleStates[iTriangle->second.id_] == 255u || triangleStates[iTriangle->second.id_] == 64u)
		{
			const Index32& triangleId = iTriangle->second.id_;

			const Index32 meshId = triangleId / textureAtlas_.numberTriangles();

			Mesh& mesh = meshMap[meshId];

			mesh.triangleFaces_.emplace_back(Index32(mesh.vertices_.size()));

			const Index32 meshTriangleId = triangleId % textureAtlas_.numberTriangles();

			Vector2 triangleTextureCoordinates[3];
			textureAtlas_.triangleAtlasTextureCoordinates<true>(meshTriangleId, triangleTextureCoordinates);

			mesh.textureCoordinates_.emplace_back(triangleTextureCoordinates[0]);
			mesh.textureCoordinates_.emplace_back(triangleTextureCoordinates[1]);
			mesh.textureCoordinates_.emplace_back(triangleTextureCoordinates[2]);

			mesh.vertices_.emplace_back(iTriangle->first.vertex0());
			mesh.vertices_.emplace_back(iTriangle->first.vertex1());
			mesh.vertices_.emplace_back(iTriangle->first.vertex2());
		}
		else if (!exportTexturedTrianglesOnly)
		{
			const Index32 meshId = Index32(-1);

			Mesh& mesh = meshMap[meshId];

			mesh.triangleFaces_.emplace_back(Index32(mesh.vertices_.size()));

			mesh.vertices_.emplace_back(iTriangle->first.vertex0());
			mesh.vertices_.emplace_back(iTriangle->first.vertex1());
			mesh.vertices_.emplace_back(iTriangle->first.vertex2());
		}

		hasAtLeastOneTriangle = true;
	}

	// in case a mesh is actually empty, we remove the entire mesh

	for (MeshMap::iterator iMesh = meshMap.begin(); iMesh != meshMap.end(); /*noop*/)
	{
		if (iMesh->second.triangleFaces_.empty())
		{
			iMesh = meshMap.erase(iMesh);
		}
		else
		{
			++iMesh;
		}
	}

	return hasAtLeastOneTriangle;
}

}

}

}
