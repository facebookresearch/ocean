/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_TEXTURE_GENERATOR_H
#define META_OCEAN_TRACKING_MAPTEXTURING_TEXTURE_GENERATOR_H

#include "ocean/tracking/maptexturing/MapTexturing.h"
#include "ocean/tracking/maptexturing/DepthBufferRenderer.h"
#include "ocean/tracking/maptexturing/HashableTriangle.h"
#include "ocean/tracking/maptexturing/MeshRenderer.h"
#include "ocean/tracking/maptexturing/RetiredTrianglesRenderer.h"
#include "ocean/tracking/maptexturing/TextureAtlas.h"
#include "ocean/tracking/maptexturing/TexturedTrianglesRenderer.h"
#include "ocean/tracking/maptexturing/TexturedTrianglesRendererBackup.h"
#include "ocean/tracking/maptexturing/TrianglesManagerRenderer.h"
#include "ocean/tracking/maptexturing/TrianglesIdRenderer.h"
#include "ocean/tracking/maptexturing/VisibleTrianglesRenderer.h"

#include "ocean/base/Frame.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/TriangleFace.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

/**
 * This class implements a texture generator for scene elements.
 * The generator separates the scene elements into individual meshes sharing the same texture.
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT TextureGenerator
{
	public:

		/**
		 * Definition of individual texturing modes.
		 */
		enum TexturingMode : uint32_t
		{
			/// An invalid texturing mode.
			TM_INVALID = 0u,
			/// Keeping retired triangles.
			TM_KEEP_RETIRED,
			/// Redraw retired triangles.
			TM_REDRAW_RETIRED
		};

		/**
		 * This class stores the relevant information of a mesh textures with the same texture.
		 */
		class Mesh
		{
			public:

				/// The vertices of the mesh.
				Vectors3 vertices_;

				/// The triangle faces of the mesh.
				Rendering::TriangleFaces triangleFaces_;

				/// The texture coordinates, one for each vertex.
				Vectors2 textureCoordinates_;
		};

		/**
		 * Definition of an unordered map mapping Mesh ids to mesh objects.
		 */
		typedef std::unordered_map<Index32, Mesh> MeshMap;

	protected:

		/**
		 * This class stores the relevant information for a triangle.
		 */
		class TriangleObject
		{
			public:

				/**
				 * Creates a new triangle object.
				 * @param id The id of the triangle
				 * @param frameIndex The index of the frame in which the triangle has been created
				 */
				inline TriangleObject(const Index32 id, const Index32 frameIndex);

			public:

				/// The unique ids of the triangle.
				Index32 id_ = Index32(-1);

				/// The index of the frame in which the triangle existed the last time.
				Index32 frameIndex_ = Index32(-1);
		};

		/**
		 * Definition of an unordered map mapping hash-able triangles to a TriangleObject objects.
		 */
		typedef std::unordered_map<HashableTriangle, TriangleObject, HashableTriangle> TriangleMap;

		/**
		 * Definition of a vector holding hash-able triangle objects.
		 */
		typedef std::vector<HashableTriangle> Triangles;

		/**
		 * Definition of a map mapping unique scene mesh ids to vertex counts.
		 */
		typedef std::unordered_map<Index32, size_t> SceneMeshVertexCounterMap;

		/**
		 * Definition of an unordered map mapping mesh ids to texture framebuffers.
		 */
		typedef std::unordered_map<Index32, Rendering::TextureFramebufferRef> TextureFramebufferMap;

	public:

		/**
		 * Creates a new texture generator.
		 * @param texturingMode The texturing mode to be used
		 */
		explicit inline TextureGenerator(const TexturingMode texturingMode = TM_REDRAW_RETIRED);

		/**
		 * Updates the generator's meshes with new meshes given as scene elements.
		 * The mesh can be updated before each new frame is processed or with a lower frequency (e.g., only for every second frame or only once before the very first frame).
		 * @param sceneElement The new scene element to be used to update the generator
		 * @param skipIfVertexNumberHasNotChanged True, to skip all meshes when their vertex number has not changed; False, to handle meshes even when their vertex number have not changed
		 * @return True, if succeeded
		 */
		bool updateMesh(const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement, const bool skipIfVertexNumberHasNotChanged);

		/**
		 * Processes a new frame and updates or sets the texture of the texture atlases whenever necessary.
		 * @param rgbFrame The new frame to be used, with pixel format FORMAT_RGB24, must be valid
		 * @param anyCamera The camera profile defining the projection of the given frame, must be valid
		 * @param world_T_camera The transformation between camera and world, with the camera pointing into the negative z-space with y-axis up, must be valid
		 * @param engine The rendering engine to be used, must be valid
		 * @return True, if succeeded
		 */
		bool processFrame(Frame&& rgbFrame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Rendering::EngineRef& engine);

		/**
		 * Renders the current textured mesh for a given camera pose.
		 * @param anyCamera The camera profile used when rendering the image, must be valid
		 * @param world_T_camera The transformation between camera and world, must be valid
		 * @param engine The rendering engine to be used, must be valid
		 * @param renderTexturedTrianglesOnly True, to render only textured triangles; False, to render also non-textured triangles
		 * @param frame The resulting rendered frame
		 * @return True, if succeeded
		 */
		bool render(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Rendering::EngineRef& engine, const bool renderTexturedTrianglesOnly, Frame& frame);

		/**
		 * Returns the ids of the generated textured meshes.
		 * @return The meshes' ids
		 */
		Indices32 meshIds() const;

		/**
		 * Returns a current mesh.
		 * @param meshId The id of the mesh to return
		 * @param mesh The resulting mesh
		 * @param meshTexture The resulting texture of the mesh, invalid if the mesh is not textured
		 * @return True, if succeeded
		 */
		bool exportMesh(const Index32 meshId, Mesh& mesh, Frame& meshTexture);

		/**
		 * Exports all currently textured meshes and creates an X3D scene file.
		 * @param filename The filename of the resulting scene file
		 * @param exportTexturedTrianglesOnly True, to export only textured triangles; False, to export also non-textured triangles
		 * @return True, if succeeded
		 */
		bool exportMeshs(const std::string& filename, const bool exportTexturedTrianglesOnly);

		/**
		 * Returns whether this generator holds at least one triangle.
		 * @return True, if succeeded
		 */
		inline bool isValid() const;

	protected:

		/**
		 * Processes a new frame and updates or sets the texture of the texture atlases whenever necessary with texturing mode TM_KEEP_RETIRED.
		 * @param rgbFrame The new frame to be used, with pixel format FORMAT_RGB24, must be valid
		 * @param anyCamera The camera profile defining the projection of the given frame, must be valid
		 * @param world_T_camera The transformation between camera and world, with the camera pointing into the negative z-space with y-axis up, must be valid
		 * @param engine The rendering engine to be used, must be valid
		 * @return True, if succeeded
		 */
		bool processFrameKeepRetired(Frame&& rgbFrame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Rendering::EngineRef& engine);

		/**
		 * Processes a new frame and updates or sets the texture of the texture atlases whenever necessary with texturing mode TM_REDRAW_RETIRED.
		 * @param rgbFrame The new frame to be used, with pixel format FORMAT_RGB24, must be valid
		 * @param anyCamera The camera profile defining the projection of the given frame, must be valid
		 * @param world_T_camera The transformation between camera and world, with the camera pointing into the negative z-space with y-axis up, must be valid
		 * @param engine The rendering engine to be used, must be valid
		 * @return True, if succeeded
		 */
		bool processFrameRedrawRetired(Frame&& rgbFrame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Rendering::EngineRef& engine);

		/**
		 * Extracts a mesh of this texture generator with texture coordinates associated with the texture atlas.
		 * @param meshId The id of the mesh to extract
		 * @param mesh The resulting mesh
		 * @return True, if at least one triangle was extracted
		 */
		bool extractMeshesWithAtlasTextureCoordinates(const Index32 meshId, Mesh& mesh);

		/**
		 * Extracts all meshes of this texture generator with texture coordinates associated with the texture atlas.
		 * @param meshMap The resulting map with the individual meshes
		 * @param exportTexturedTrianglesOnly True, to extract only textured triangles; False, to extract also non-textured triangles
		 * @return True, if at least one triangle was extracted
		 */
		bool extractMeshesWithAtlasTextureCoordinates(MeshMap& meshMap, const bool exportTexturedTrianglesOnly);

		/**
		 * Returns an invalid triangle id.
		 * @return The invalid triangle id
		 */
		static constexpr Index32 invalidTriangleId();

	protected:

		/// The texturing mode to be used.
		TexturingMode texturingMode_ = TM_INVALID;

		/// The counter for triangle ids.
		Index32 triangleIdCounter_ = invalidTriangleId();

		/// The map mapping hash-able triangles to triangle objects.
		TriangleMap triangleMap_;

		/// The vector of all triangles, with ids identical to indices.
		Triangles triangles_ = Triangles(1024 * 1024);

		/// The ids of triangles which can be reused as they have been deleted before.
		Indices32 freeTriangleIds_;

		/// The current frame index.
		unsigned int frameIndex_ = 0u;

		/// The dummy framebuffer for platforms not having their own main framebuffer.
		Rendering::FramebufferRef dummyFramebuffer_;

		/// The renderer able to render triangles with individual color ids of all active triangles (not including retired triangles).
		TrianglesIdRenderer activeTrianglesIdRenderer_;

		/// The renderer able to render triangles with individual color ids of all existing triangles (active and retired triangles).
		TrianglesIdRenderer anyTrianglesIdRenderer_;

#ifndef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/// The renderer able to render textured triangles to the texture atlas.
		TexturedTrianglesRenderer texturedTrianglesRenderer_;

#endif

		/// The renderer able to render textured triangles to the texture atlas.
		TexturedTrianglesRendererBackup texturedTrianglesRendererBackup_;

		/// The renderer able to render all retired triangles.
		RetiredTrianglesRenderer retiredTrianglesRenderer_;

		/// The renderer able to down-sample and to filter a depth buffer.
		DepthBufferRenderer depthBufferRenderer_;

		/// The renderer able to determine which triangles are visible.
		VisibleTrianglesRenderer visibleTrianglesRenderer_;

		/// The renderer able to manage the states of all textures.
		TrianglesManagerRenderer trianglesManagerRenderer_;

		/// The renderer for the entire mesh.
		MeshRenderer meshRenderer_;

		/// The profile of the texture atlas to be used for all textures.
		TextureAtlas textureAtlas_;

		/// Reusable frame for triangle ids.
		Frame reusableIdFrame_;

		/// Reusable frame for the z-buffer.
		Frame reusableDepthFrame_;

		/// The ids of all new triangles since the last update call.
		Indices32 newTriangleIds_;

		/// The ids of all retired triangles since the last update call.
		Indices32 retiredTriangleIds_;

		/// The reusable vector holding ids of deleted triangle.
		Indices32 deletedTriangleIds_;

		/// The vertex set holding the vertices to be rendered.
		Rendering::VertexSetRef renderingVertexSet_;

		/// The triangle object which will be used to render the triangles.
		Rendering::TrianglesRef renderingTriangles_;

		/// True, if any mesh scene mesh has changed.
		bool anySceneMeshHasChanged_ = false;

		/// The latest vertex counts of all scene meshes.
		SceneMeshVertexCounterMap latestSceneMeshVertexCounterMap_;

		/// The generator's lock.
		mutable Lock lock_;
};

inline TextureGenerator::TextureGenerator(const TexturingMode texturingMode) :
	texturingMode_(texturingMode)
{
	// nothing to do here
}

inline TextureGenerator::TriangleObject::TriangleObject(const Index32 id, const Index32 frameIndex) :
	id_(id),
	frameIndex_(frameIndex)
{
	// nothing to do here
}

inline bool TextureGenerator::isValid() const
{
	return !triangleMap_.empty();
}

constexpr Index32 TextureGenerator::invalidTriangleId()
{
	return 0u;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_TEXTURE_GENERATOR_H
