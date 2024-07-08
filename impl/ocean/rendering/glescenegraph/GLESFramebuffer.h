/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GLES_FRAMEBUFFER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESTextureFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESTraverser.h"

#include "ocean/rendering/Framebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a base for all GLESceneGraph framebuffers.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESFramebuffer :
	virtual public GLESObject,
	virtual public Framebuffer
{
	public:

		/**
		 * Definitions of individual stereo framebuffers.
		 */
		enum StereoType
		{
			/// An invalid framebuffer type.
			ST_INVALID = 0,
			/// Framebuffer of a mono view.
			ST_MONO,
			/// Framebuffer of a left stereo view.
			ST_LEFT,
			/// Framebuffer of a right stereo view.
			ST_RIGHT
		};

	public:

		/**
		 * Returns the viewport of this framebuffer.
		 * @see Framebuffer::viewport().
		 */
		void viewport(unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height) const override;

		/**
		 * Returns whether the framebuffer supports hardware anti-aliasing.
		 * @see Framebuffer::isAntialiasingSupported().
		 */
		bool isAntialiasingSupported(const unsigned int buffers) const override;

		/**
		 * Returns whether the framebuffer uses hardware anti-aliasing.
		 * @see Framebuffer::isAntialiasing().
		 */
		bool isAntialiasing() const override;

		/**
		 * Returns the face mode of the entire framebuffer.
		 * @see Framebuffer::faceMode().
		 */
		FaceMode faceMode() const override;

		/**
		 * Returns the global face culling mode of the entire framebuffer.
		 * @see Framebuffer::cullingMode().
		 */
		CullingMode cullingMode() const override;

		/**
		 * Returns the global lighting mode of the entire framebuffer.
		 * @see Framebuffer::lightingMode().
		 */
		LightingMode lightingMode() const override;

		/**
		 * Returns the render technique of the entire framebuffer.
		 * @see Framebuffer::renderTechnique().
		 */
		RenderTechnique renderTechnique() const override;

		/**
		 * Returns the shadow technique of the entire framebuffer.
		 * @see Framebuffer::shadowTechnique().
		 */
		ShadowTechnique shadowTechnique() const override;

		/**
		 * Returns the stereo type of this framebuffer.
		 * @return The framebuffer's stereo type, ST_MONO by default
		 */
		virtual StereoType stereoType() const;

		/**
		 * Sets the viewport of this framebuffer.
		 * @see Framebuffer::setViewport().
		 */
		void setViewport(const unsigned int left, const unsigned int top, const unsigned int width, const unsigned int height) override;

		/**
		 * Sets whether the framebuffer uses hardware anti-aliasing.
		 * @see Framebuffer::setAntialiasing().
		 */
		bool setAntialiasing(const bool antialiasing) override;

		/**
		 * Sets the face mode of the entire framebuffer.
		 * @see Framebuffer::setFaceMode().
		 */
		void setFaceMode(const FaceMode mode) override;

		/**
		 * Sets the global culling mode of the entire framebuffer.
		 * @see Framebuffer::setCullingMode().
		 */
		void setCullingMode(const CullingMode cullingMode) override;

		/**
		 * Sets the global lighting mode of the entire framebuffer.
		 * @see Framebuffer::setLightingMode().
		 */
		void setLightingMode(const LightingMode lightingMode) override;

		/**
		 * Sets the rendering technique of the entire framebuffer.
		 * @see Framebuffer::setRenderTechnique().
		 */
		void setRenderTechnique(const RenderTechnique technique) override;

		/**
		 * Sets the shadow technique of the entire framebuffer.
		 * @see Framebuffer::setShadowTechnique().
		 */
		void setShadowTechnique(const ShadowTechnique technique) override;

		/**
		 * Sets the stereo type of this framebuffer.
		 * @param stereoType The stereo type to set
		 */
		virtual void setStereoType(const StereoType stereoType);

		/**
		 * Renders the next frame into the framebuffer.
		 * @see Framebuffer::render().
		 */
		void render() override;

		/**
		 * Returns the renderable object intersected by a given ray.
		 * @see Framebuffer::intersection().
		 */
		bool intersection(const Line3& ray, RenderableRef& renderable, Vector3& position) override;

		/**
		 * Returns whether the platform supports a specific extension.
		 * @see Framebuffer::hasExtension().
		 */
		bool hasExtension(const std::string& extension) const override;

		/**
		 * Linearizes a given z-buffer depth frame with non-linear (e.g., logarithmic) depth.
		 * By default in OpenGL, the depth coordinates range from −1 to 1, corresponding to the near and far clipping planes (after clipping and division by w).<br>
		 * @param depthFrame The non-linear depth to linearize, with value range [-1, 1], must be valid
		 * @param nearDistance The distance to the near clipping plane of the depth frame, with range (0, infinity)
		 * @param farDistance The distance to the far clipping plane of the depth frame, with range (nearDistance, infinity)
		 * @return True, if succeeded
		 */
		bool linearizeDepthFrame(Frame& depthFrame, const Scalar nearDistance, const Scalar farDistance) const override;

	protected:

		/**
		 * Creates a new GLESceneGraph framebuffer object.
		 */
		GLESFramebuffer();

		/**
		 * Destructs a GLESceneGraph framebuffer object.
		 */
		~GLESFramebuffer() override;

		/**
		 * Initializes the framebuffer.
		 * @see Framebuffer::initialize().
		 */
		bool initialize(const FramebufferRef& shareFramebuffer = FramebufferRef()) override;

		/**
		 * Initialize the OpenGL ES display and context.
		 * @return True, if succeeded
		 */
		virtual bool initializeContext();

		/**
		 * Initialize the OpenGL ES environment properties.
		 * @return True, if succeeded
		 */
		virtual bool initializeOpenGLES();

		/**
		 * Releases the framebuffer.
		 * @see Framebuffer::release().
		 */
		void release() override;

	protected:

#ifndef OCEAN_RENDERING_GLES_USE_ES
		/// The face mode which is only supported in OpenGL.
		FaceMode faceMode_ = PrimitiveAttribute::MODE_FACE;
#endif

		/// Global lighting mode for the entire framebuffer.
		CullingMode cullingMode_ = PrimitiveAttribute::CULLING_DEFAULT;

		/// Global lighting mode for the entire framebuffer.
		LightingMode lightingMode_ = PrimitiveAttribute::LM_DEFAULT;

		/// The string will all extensions separated by spaces.
		std::string extensions_;

		/// Viewport left position.
		unsigned int viewportLeft_ = (unsigned int)(-1);

		/// Viewport top position.
		unsigned int viewportTop_ = (unsigned int)(-1);

		/// Viewport width value.
		unsigned int viewportWidth_ = (unsigned int)(0);

		/// Viewport height value.
		unsigned int viewportHeight_ = (unsigned int)(0);

		/// The stereo framebuffer type.
		StereoType stereoType_ = ST_MONO;

		/// The traverser which is used for rendering.
		GLESTraverser traverser_;

		/// The texture framebuffer which is used for picking objects.
		SmartObjectRef<GLESTextureFramebuffer> pickingTextureFramebuffer_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_FRAMEBUFFER_H
