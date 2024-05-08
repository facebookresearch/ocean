/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_TEXTURED_TRIANGLES_RENDERER_H
#define META_OCEAN_TRACKING_MAPTEXTURING_TEXTURED_TRIANGLES_RENDERER_H

#include "ocean/tracking/maptexturing/MapTexturing.h"
#include "ocean/tracking/maptexturing/HashableTriangle.h"
#include "ocean/tracking/maptexturing/TextureAtlas.h"

#include "ocean/base/Frame.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/Engine.h"
#include "ocean/rendering/ShaderProgram.h"
#include "ocean/rendering/TextureFramebuffer.h"
#include "ocean/rendering/TriangleFace.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

/**
 * This class implements a renderer rendering textured triangles into a texture atlas using a geometry shader.
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT TexturedTrianglesRenderer
{
	protected:

		/**
		 * Definition of an unordered map mapping mesh ids to texture framebuffers.
		 */
		typedef std::unordered_map<Index32, Rendering::TextureFramebufferRef> TextureFramebufferMap;

	public:

		/**
		 * Destructs this renderer and releases all resources.
		 */
		~TexturedTrianglesRenderer();

		/**
		 * Updates this renderer with a new live frame which will be used as source texture.
		 * @param engine The rendering engine to be used
		 * @param rgbFrame The frame to be used as source texture, must be valid
		 * @return True, if succeeded
		 */
		bool update(const Rendering::Engine& engine, Frame&& rgbFrame);

		/**
		 * Renders textured triangles into a texture atlas.
		 * @param engine The rendering engine to be used
		 * @param numberTriangles The number of triangles to render, with range [0, infinity)
		 * @param projectionMatrix The projection matrix to be used, must be valid
		 * @param world_T_camera The transformation between camera and world, must be valid
		 * @param vertexSet The vertex set containing all triangle vertices, must be valid
		 * @param triangles The triangles to render, must be valid
		 * @param visibleTrianglesFramebuffer The texture framebuffer containing all visible triangles, must be valid
		 * @param textureAtlas The texture atlas defining the layout
		 * @return True, if succeeded
		 */
		bool render(const Rendering::Engine& engine, const unsigned int numberTriangles, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& world_T_camera, const Rendering::VertexSetRef& vertexSet, const Rendering::TrianglesRef& triangles, const Rendering::TextureFramebufferRef& visibleTrianglesFramebuffer, const TextureAtlas& textureAtlas);

		/**
		 * Returns the map mapping mesh ids to texture framebuffers.
		 * @return The map of texture framebuffers
		 */
		inline const TextureFramebufferMap& textureFramebufferMap() const;

		/**
		 * Returns the latest texture atlas image for a specified mesh.
		 * @param meshId The id of the mesh for which the latest atlas image will be returned, must be valid
		 * @param textureFrame The resulting atlas image
		 * @return True, if succeeded
		 */
		bool latestTexture(const Index32 meshId, Frame& textureFrame) const;

		/**
		 * Returns the ids of all textured meshes.
		 * @return The ids of all meshes
		 */
		Indices32 meshIds() const;

		/**
		 * Explicitly releases this renderer and releases all resources.
		 */
		void release();

		/**
		 * Returns whether this renderer has been initialized successfully.
		 * @return True, if succeeded
		 */
		inline bool isValid() const;

	protected:

		/**
		 * Renders textured triangles into a texture atlas.
		 * @param engine The rendering engine to be used
		 * @param meshId The id of the mesh for which the textured triangles will be rendered, must be valid
		 * @param trianglesInMesh The number of triangles in the mesh, with range [1, infinity)
		 * @param projectionMatrix The projection matrix to be used, must be valid
		 * @param world_T_camera The transformation between camera and world, must be valid
		 * @param vertexSet The vertex set containing all triangle vertices, must be valid
		 * @param triangles The triangles to render, must be valid
		 * @param visibleTrianglesFramebuffer The texture framebuffer containing all visible triangles, must be valid
		 * @param textureAtlas The texture atlas defining the layout
		 * @return True, if succeeded
		 */
		bool renderMesh(const Rendering::Engine& engine, const Index32 meshId, const unsigned int trianglesInMesh, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& world_T_camera, const Rendering::VertexSetRef& vertexSet, const Rendering::TrianglesRef& triangles, const Rendering::TextureFramebufferRef& visibleTrianglesFramebuffer, const TextureAtlas& textureAtlas);

	protected:

		/// The shader program rendering individual triangles with individual color ids.
		Rendering::ShaderProgramRef shaderProgram_;

		/// The map of texture framebuffers, one for each mesh id.
		TextureFramebufferMap textureFramebufferMap_;

		/// The frame texture holding the texture information for the triangles.
		Rendering::FrameTexture2DRef texture_;

		/// The platform-specific shader part.
		static const char* partPlatform_;

		/// The shader part for the texture atlas.
		static const char* partTextureAtlas_;

		/// The geometry shader part.
		static const char* programGeometryShader_;

		/// The vertex shader part.
		static const char* programVertexShader_;

		/// The fragment shader part.
		static const char* programFragmentShader_;
};

inline const TexturedTrianglesRenderer::TextureFramebufferMap& TexturedTrianglesRenderer::textureFramebufferMap() const
{
	return textureFramebufferMap_;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_TEXTURED_TRIANGLES_RENDERER_H
