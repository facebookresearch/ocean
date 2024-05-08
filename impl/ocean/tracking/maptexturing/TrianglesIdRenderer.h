/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_TRIANGLES_ID_RENDERER_H
#define META_OCEAN_TRACKING_MAPTEXTURING_TRIANGLES_ID_RENDERER_H

#include "ocean/tracking/maptexturing/MapTexturing.h"

#include "ocean/base/Frame.h"

#include "ocean/math/HomogenousMatrix4.h"

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
 * This class implements a renderer for triangles with individual color ids.
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT TrianglesIdRenderer
{
	public:

		/**
		 * Destructs this renderer and releases all resources.
		 */
		~TrianglesIdRenderer();

		/**
		 * Initializes this renderer.
		 * @param engine The rendering engine to be used
		 * @return True, if succeeded
		 */
		bool initialize(const Rendering::Engine& engine);

		/**
		 * Renders triangles with individual ids.
		 * @param vertexSet The vertex set containing all triangle vertices, must be valid
		 * @param triangles The triangles to render, must be valid
		 * @param numberTriangles The number of triangles to render, with range [1, infinity)
		 * @param trianglesStateFramebuffer The framebuffer holding the states of the individual triangles, must be valid
		 * @param projectionMatrix The projection matrix to be used, must be valid
		 * @param world_T_camera The transformation between camera and world, must be valid
		 * @param width The width of the framebuffer in which the triangles will be rendered, in pixel, with range [1, infinity)
		 * @param height The width of the framebuffer in which the triangles will be rendered, in pixel, with range [1, infinity)
		 * @param minimalTriangleState The minimal state of a triangle to that it will be rendered, with range [0, 255]
		 * @param idFrame Optional resulting frame containing the color output (the id frame) of the renderer, nullptr if not of interest
		 * @param depthFrame Optional resulting frame containing the z-buffer of the renderer, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool render(const Rendering::VertexSetRef& vertexSet, const Rendering::TrianglesRef& triangles, const unsigned int numberTriangles, const Rendering::TextureFramebufferRef& trianglesStateFramebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& world_T_camera, const unsigned int width, const unsigned int height, const uint8_t minimalTriangleState, Frame* idFrame = nullptr, Frame* depthFrame = nullptr);

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

		/// The texture framebuffer used when rendering the color ids.
		Rendering::TextureFramebufferRef textureFramebuffer_;

		/// The platform-specific shader part.
		static const char* partPlatform_;

		/// The vertex shader part.
		static const char* programVertexShaderColorId_;

		/// The fragment shader part.
		static const char* programFragmentShaderColorId_;
};

inline const Rendering::TextureFramebufferRef& TrianglesIdRenderer::textureFramebuffer() const
{
	return textureFramebuffer_;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_TRIANGLES_ID_RENDERER_H
