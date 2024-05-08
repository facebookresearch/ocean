/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_TRIANGLES_MANAGER_RENDERER_H
#define META_OCEAN_TRACKING_MAPTEXTURING_TRIANGLES_MANAGER_RENDERER_H

#include "ocean/tracking/maptexturing/MapTexturing.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/Frame.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Points.h"
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
 * This class implements a manager for active, textured, retired, and deleted triangles.
 * The manager holds the states of the individual triangles in an own framebuffer.
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT TrianglesManagerRenderer
{
	public:

		/**
		 * Destructs this renderer and releases all resources.
		 */
		~TrianglesManagerRenderer();

		/**
		 * Initializes the manager.
		 * @param engine The rendering engine to be used
		 * @param framebufferWidth The width of the framebuffer, in pixel, with range [1, infinity)
		 * @param framebufferHeight The height of the framebuffer, in pixel, with range [1, infinity)
		 * @return True, if succeeded
		 */
		bool initialize(const Rendering::Engine& engine, unsigned int framebufferWidth, unsigned int framebufferHeight);

		/**
		 * Updates the manager with ids of new triangles.
		 * @param newTriangleIds The ids of all new triangles
		 * @param stateFrame Optional resulting frame holding the states of all triangles, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool updateNewTriangles(const Indices32& newTriangleIds, Frame* stateFrame = nullptr);

		/**
		 * Updates the manager with ids of recently textured triangles.
		 * @param triangleIdFramebuffer The framebuffer holding all triangles recently textured, must be valid
		 * @param stateFrame Optional resulting frame holding the states of all triangles, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool updateTexturedTriangles(const Rendering::TextureFramebufferRef& triangleIdFramebuffer, Frame* stateFrame = nullptr);

		/**
		 * Updates the manager with ids of recently textured triangles.
		 * @param texturedTriangleIds The ids of all textured triangles
		 * @param stateFrame Optional resulting frame holding the states of all triangles, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool updateTexturedTriangles(const Indices32& texturedTriangleIds, Frame* stateFrame = nullptr);

		/**
		 * Updates the manager with ids of retired triangles.
		 * @param retiredTriangleIds The ids of all retired triangles
		 * @param stateFrame Optional resulting frame holding the states of all triangles, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool updateRetiredTriangles(const Indices32& retiredTriangleIds, Frame* stateFrame = nullptr);

		/**
		 * Updates the manager with ids of deleted triangles.
		 * @param deletedTriangleIds The ids of all deleted triangles
		 * @param stateFrame Optional resulting frame holding the states of all triangles, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool updateDeletedTriangles(const Indices32& deletedTriangleIds, Frame* stateFrame = nullptr);

		/**
		 * Returns the states of all triangles this manager.
		 * @param numberTriangles The number of triangles for which the states will be returned, with range [0, infinity)
		 * @return The states of the individual triangles, valid until the manager's data will be modified
		 */
		ConstArrayAccessor<uint8_t> triangleStates(const unsigned int numberTriangles);

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
		 * Sets the color value (the state) of several triangles.
		 * @param triangleIds The ids of all triangles for which the state will be set
		 * @param triangleColor The color value (the state) to be set
		 * @return True, if succeeded
		 */
		bool setTriangles(const Indices32& triangleIds, const float triangleColor);

		/**
		 * Copies the active state framebuffer to the copy framebuffer.
		 * @return True, if succeeded
		 */
		bool copyFramebuffer();

	protected:

		//// The width of the framebuffer in pixel.
		unsigned int framebufferWidth_ = 0u;

		/// The height of the framebuffer in pixel.
		unsigned int framebufferHeight_ = 0u;

		/// True, if the manager's data has been changed recently.
		bool hasChanged_ = false;

		/// The frame holding a copy of the triangle states, valid as long as 'hasChanged_ == false'
		Frame triangleStatesFrame_;

		/// The shader program allowing to set triangle states.
		Rendering::ShaderProgramRef shaderProgramSetTriangles_;

		/// The shader program allowing to set the states of textured triangles.
		Rendering::ShaderProgramRef shaderProgramTexturedTriangles_;

		/// The shader program allowing to make a copy of the state framebuffer.
		Rendering::ShaderProgramRef shaderProgramCopyFramebuffer_;

		/// The shader program allowing to set the states of retired triangles.
		Rendering::ShaderProgramRef shaderProgramRetiredTriangles_;

		/// The texture framebuffer holding the states of the individual triangles.
		Rendering::TextureFramebufferRef textureFramebuffer_;

		/// The copy of the texture framebuffer holding the states of the individual triangles.
		Rendering::TextureFramebufferRef copyTextureFramebuffer_;

		/// The vertex set holding the vertices to be rendered.
		Rendering::VertexSetRef vertexSet_;

		/// The points object which will be used to render points.
		Rendering::PointsRef points_;

		/// The triangles object which will be used to render triangles.
		Rendering::TrianglesRef triangles_;

		/// The platform-specific shader part.
		static const char* partPlatform_;

		/// The vertex shader program to set triangle states.
		static const char* programVertexShaderSetTriangles_;

		/// The vertex shader program to render planar textured triangles.
		static const char* programVertexShaderPlanarTexturedTriangles_;

		/// The vertex shader program to render retired triangles.
		static const char* programVertexShaderRetiredTriangles_;

		/// The fragment shader program to render triangle states.
		static const char* programFragmentShaderSetTriangles_;

		/// The fragment shader program to render active triangles.
		static const char* programFragmentShaderActiveTriangles_;

		/// The fragment shader program to render retired triangles.
		static const char* programFragmentShaderRetiredTriangles_;

		/// The fragment shader program to copy the framebuffer.
		static const char* programFragmentShaderCopyFramebuffer_;
};

inline const Rendering::TextureFramebufferRef& TrianglesManagerRenderer::textureFramebuffer() const
{
	return textureFramebuffer_;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_TRIANGLES_MANAGER_RENDERER_H
