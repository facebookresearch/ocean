/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_DEPTH_BUFFER_RENDERER_H
#define META_OCEAN_TRACKING_MAPTEXTURING_DEPTH_BUFFER_RENDERER_H

#include "ocean/tracking/maptexturing/MapTexturing.h"

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
 * This class implements a renderer for a depth buffer.
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT DepthBufferRenderer
{
	public:

		/**
		 * Destructs this renderer and releases all resources.
		 */
		~DepthBufferRenderer();

		/**
		 * Down-samples and filters a depth framebuffer.
		 * @param engine The rendering engine to be used
		 * @param sourceFramebuffer The source texture framebuffer from which the depth buffer will be down-sampled and filtered, must be valid
		 * @param downsampleIterations The number of down-sample iterations, with range [0, infinity)
		 */
		bool downsampleAndFilterDepthBuffer(const Rendering::Engine& engine, const Rendering::TextureFramebufferRef& sourceFramebuffer, const unsigned int downsampleIterations);

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

		/**
		 * Down-samples a framebuffer by a factor of two.
		 * @param sourceFramebuffer The source framebuffer to down-sample, must be valid
		 * @param targetFramebuffer The target framebuffer receiving the down-sampled source framebuffer, must be valid
		 * @return True, if succeeded
		 */
		bool downsampleFramebuffer(const Rendering::TextureFramebufferRef& sourceFramebuffer, Rendering::TextureFramebufferRef& targetFramebuffer);

		/**
		 * Applies a 3x3 min-filter step to the source framebuffer.
		 * @param sourceFramebuffer The source framebuffer to which the filter will be applied, must be valid
		 * @param targetFramebuffer The target framebuffer receiving the filtered result, must be valid
		 * @return True, if succeeded
		 */
		bool minFilterFramebuffer(const Rendering::TextureFramebufferRef& sourceFramebuffer, Rendering::TextureFramebufferRef& targetFramebuffer);

	protected:

		/// The shader program downsampling the depth buffer.
		Rendering::ShaderProgramRef shaderProgramDownsampling_;

		/// The shader program filtering the depth buffer.
		Rendering::ShaderProgramRef shaderProgramMinFiltering_;

		/// The texture framebuffer used when rendering the color ids.
		std::vector<Rendering::TextureFramebufferRef> textureFramebuffers_;

		/// The vertex set holding the vertices to be rendered.
		Rendering::VertexSetRef vertexSet_;

		/// The triangle object which will be used to render the triangles.
		Rendering::TrianglesRef triangles_;

		/// The platform-specific shader part.
		static const char* partPlatform_;

		/// The vertex shader part.
		static const char* programVertexShader_;

		/// The fragment shader part for downsampling.
		static const char* programFragmentShaderDownsampling_;

		/// The fragment shader part for 3x3 box filtering.
		static const char* programFragmentShaderMinFiltering_;
};

inline const Rendering::TextureFramebufferRef& DepthBufferRenderer::textureFramebuffer() const
{
	ocean_assert(!textureFramebuffers_.empty());
	return textureFramebuffers_.back();
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_DEPTH_BUFFER_RENDERER_H
