/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_RETIRED_TRIANGLES_RENDERER_H
#define META_OCEAN_TRACKING_MAPTEXTURING_RETIRED_TRIANGLES_RENDERER_H

#include "ocean/tracking/maptexturing/MapTexturing.h"

#include "ocean/base/Frame.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/ShaderProgram.h"
#include "ocean/rendering/TextureFramebuffer.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

/**
 * This class implements a renderer for retired triangle allowing to determine whether they can be deleted.
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT RetiredTrianglesRenderer
{
	public:

		/**
		 * Destructs this renderer and releases all resources.
		 */
		~RetiredTrianglesRenderer();

		/**
		 * Renders all retired triangles to determine whether they can be deleted.
		 * @param engine The rendering engine to be used
		 * @param vertexSet The vertex set containing all triangle vertices, must be valid
		 * @param triangles The triangles to render, must be valid
		 * @param numberTriangles The number of triangles to render, with range [1, infinity)
		 * @param projectionMatrix The projection matrix to be used, must be valid
		 * @param world_T_camera The transformation between camera and world, must be valid
		 * @param trianglesStateFramebuffer The framebuffer holding the states of the individual triangles, must be valid
		 * @param downsampledDepthFramebuffer The down-sampled and filtered texture framebuffer holding the depth image, must be valid
		 * @param nearDistance The view's near distance, with range (0, infinity)
		 * @param farDistance the view's far distance, with range (nearDistance, infinity)
		 * @param deletedTriangleIds The resulting ids of all triangles which's states have changed from retired to deleted
		 * @return True, if succeeded
		 */
		bool render(const Rendering::Engine& engine, const Rendering::VertexSetRef& vertexSet, const Rendering::TrianglesRef& triangles, const unsigned int numberTriangles, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& world_T_camera, const Rendering::TextureFramebufferRef& trianglesStateFramebuffer, const Rendering::TextureFramebufferRef& downsampledDepthFramebuffer, const Scalar nearDistance, const Scalar farDistance, Indices32& deletedTriangleIds);

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

		/// The shader program to render the retired triangles.
		Rendering::ShaderProgramRef shaderProgramRetiredTriangles_;

		/// The texture framebuffer in which the triangles will be rendered.
		Rendering::TextureFramebufferRef textureFramebuffer_;

		/// The intermediate state frame.
		Frame stateFrame_;

		/// The platform-specific shader part.
		static const char* partPlatform_;

		/// The vertex shader part.
		static const char* programVertexShaderRetiredTriangles_;

		/// The fragment shader part.
		static const char* programFragmentShaderRetiredTriangles_;
};

inline const Rendering::TextureFramebufferRef& RetiredTrianglesRenderer::textureFramebuffer() const
{
	return textureFramebuffer_;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_RETIRED_TRIANGLES_RENDERER_H
