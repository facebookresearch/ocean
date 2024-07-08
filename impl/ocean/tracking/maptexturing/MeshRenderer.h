/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_MESH_RENDERER_H
#define META_OCEAN_TRACKING_MAPTEXTURING_MESH_RENDERER_H

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
 * This class implements a renderer for the entire mesh.
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT MeshRenderer
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
		~MeshRenderer();

		/**
		 * Initializes this renderer.
		 * @param engine The rendering engine to be used
		 * @return True, if succeeded
		 */
		bool initialize(const Rendering::Engine& engine);

		/**
		 * Renders textured triangles into a texture atlas.
		 * @param engine The rendering engine to be used
		 * @param vertexSet The vertex set containing all triangle vertices, must be valid
		 * @param triangles The triangles to render, must be valid
		 * @param numberTriangles The number of triangles to render, with range [0, infinity)
		 * @param anyCamera The camera profile to be used when rendering, must be valid
		 * @param world_T_camera The transformation between camera and world, must be valid
		 * @param textureFramebufferMap The map of mesh ID to texture, used for rendering associated triangles with the textures.
		 * @param trianglesStateFramebuffer The framebuffer holding the states of the individual triangles, must be valid
		 * @param textureAtlas The texture atlas defining the layout
		 * @param renderedFrame The optionally resulting frame to which the rendered frame is copied, will be ignored if set to `nullptr`.
		 * @return True, if succeeded
		 */
		bool render(const Rendering::Engine& engine, const Rendering::VertexSetRef& vertexSet, const Rendering::TrianglesRef& triangles, const unsigned int numberTriangles, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const TextureFramebufferMap& textureFramebufferMap, const Rendering::TextureFramebufferRef& trianglesStateFramebuffer, const TextureAtlas& textureAtlas, Frame* renderedFrame = nullptr);

		/**
		 * Returns the current texture framebuffer of this renderer.
		 * @return The renderer's texture framebuffer
		 */
		inline const Rendering::TextureFramebufferRef& textureFramebuffer() const;

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

		/// The shader program rendering individual triangles with individual color ids.
		Rendering::ShaderProgramRef shaderProgram_;

		/// The texture framebuffer holding the states of the individual triangles.
		Rendering::TextureFramebufferRef textureFramebuffer_;

		/// The platform-specific shader part.
		static const char* partPlatform_;

		/// The shader part for the texture atlas.
		static const char* partTextureAtlas_;

		/// The vertex shader part.
		static const char* programVertexShader_;

		/// The fragment shader part.
		static const char* programFragmentShader_;
};

inline const Rendering::TextureFramebufferRef& MeshRenderer::textureFramebuffer() const
{
	return textureFramebuffer_;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_MESH_RENDERER_H
