/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_VISIBLE_TRIANGLES_RENDERER_H
#define META_OCEAN_TRACKING_MAPTEXTURING_VISIBLE_TRIANGLES_RENDERER_H

#include "ocean/tracking/maptexturing/MapTexturing.h"

#include "ocean/base/Frame.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Points.h"
#include "ocean/rendering/ShaderProgram.h"
#include "ocean/rendering/TextureFramebuffer.h"
#include "ocean/rendering/TriangleFace.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

/**
 * This class implements a renderer for all visible triangles.
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT VisibleTrianglesRenderer
{
	public:

		/**
		 * Destructs this renderer and releases all resources.
		 */
		~VisibleTrianglesRenderer();

		/**
		 * Extracts the visible triangles out of a framebuffer in which all triangles have been rendered.
		 * @param engine The rendering engine to be used
		 * @param trianglesIdFramebuffer The texture framebuffer holding in which all triangles have been rendered, must be valid
		 * @param downsampledDepthFramebuffer The down-sampled and filtered depth buffer of the framebuffer with triangle ids, must be valid
		 * @param nearDistance The view's near distance, with range (0, infinity)
		 * @param farDistance the view's far distance, with range (nearDistance, infinity)
		 * @return True, if succeeded
		 */
		bool render(const Rendering::Engine& engine, const Rendering::TextureFramebufferRef& trianglesIdFramebuffer, const Rendering::TextureFramebufferRef& downsampledDepthFramebuffer, const Scalar nearDistance, const Scalar farDistance);

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

		/// The shader program rendering the ids of visible triangles.
		Rendering::ShaderProgramRef shaderProgramVisibleTriangles_;

		/// The shader program removing the ids of (partially) occluded triangles.
		Rendering::ShaderProgramRef shaderProgramOccludedTriangles_;

		/// The texture framebuffer used when rendering the color ids.
		Rendering::TextureFramebufferRef textureFramebuffer_;

		/// The vertex set holding the vertices to be rendered.
		Rendering::VertexSetRef vertexSet_;

		/// The triangle object which will be used to render the triangles.
		Rendering::PointsRef points_;

		/// The intermediate frame holding the ids of all visible triangles.
		Frame idFrame_;

		/// The platform-specific shader part.
		static const char* partPlatform_;

		/// The vertex shader part.
		static const char* programVertexShaderVisibleTriangles_;

		/// The vertex shader part.
		static const char* programVertexShaderOccludedTriangles_;

		/// The fragment shader part.
		static const char* programFragmentShaderVisibleTriangles_;

		/// The fragment shader part.
		static const char* programFragmentShaderOccludedTriangles_;
};

inline const Rendering::TextureFramebufferRef& VisibleTrianglesRenderer::textureFramebuffer() const
{
	return textureFramebuffer_;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_VISIBLE_TRIANGLES_RENDERER_H
