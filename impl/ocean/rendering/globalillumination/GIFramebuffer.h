/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GI_FRAMEBUFFER_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GILightSource.h"
#include "ocean/rendering/globalillumination/TracingGroup.h"
#include "ocean/rendering/globalillumination/Lighting.h"

#include "ocean/base/Frame.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a base for all Global Illumination framebuffers.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIFramebuffer :
	virtual public GIObject,
	virtual public Framebuffer
{
	public:

		/**
		 * Returns the global face mode of the entire framebuffer.
		 * @see Framebuffer::faceMode().
		 */
		FaceMode faceMode() const override;

		/**
		 * Returns the global face culling mode of the entire framebuffer.
		 * @see Framebuffer::cullingMode().
		 */
		CullingMode cullingMode() const override;

		/**
		 * Returns the render technique of the entire framebuffer.
		 * @see Framebuffer::renderTechnique().
		 */
		RenderTechnique renderTechnique() const override;

		/**
		 * Returns whether the framebuffer supports hardware antialiasing.
		 * @see Framebuffer::isAntialiasingSupported().
		 */
		bool isAntialiasingSupported(const unsigned int buffers) const override;

		/**
		 * Returns whether the framebuffer uses hardware antialiasing.
		 * @see Framebuffer::isAntialiasing().
		 */
		bool isAntialiasing() const override;

		/**
		 * Returns whether the framebuffer supports stereo views.
		 * @see Framebuffer::isQuadbufferedStereoSupported().
		 */
		bool isQuadbufferedStereoSupported() const override;

		/**
		 * Connects a view with this framebuffer.
		 * @see Framebuffer::setView().
		 */
		void setView(const ViewRef& view) override;

		/**
		 * Sets the global face mode of the entire framebuffer.
		 * @see Framebuffer::setFaceMode().
		 */
		void setFaceMode(const FaceMode faceMode) override;

		/**
		 * Sets the global culling mode of the entire framebuffer.
		 * @see Framebuffer::setCullingMode().
		 */
		void setCullingMode(const CullingMode cullingMode) override;

		/**
		 * Sets the rendering technique of the entire framebuffer.
		 * @see Framebuffer::setRenderTechnique().
		 */
		void setRenderTechnique(const RenderTechnique technique) override;

		/**
		 * Sets whether this framebuffer will support hardware antialiasing.
		 * @see setSupportAntialiasing().
		 */
		bool setSupportAntialiasing(const unsigned int buffers) override;

		/**
		 * Sets whether the framebuffer uses hardware antialiasing.
		 * @see Framebuffer::setAntialiasing().
		 */
		bool setAntialiasing(const bool antialiasing) override;

		/**
		 * Sets whether this framebuffer will support stereo views.
		 * @see Framebuffer::setSupportQuadbufferedStereo().
		 */
		bool setSupportQuadbufferedStereo(const bool enable) override;

		/**
		 * Makes this frambuffer to the current one.
		 * @see Framebuffer::makeCurrent().
		 */
		void makeCurrent() override;

		/**
		 * Makes this framebuffer to non current.
		 * @see Framebuffer::makeNoncurrent().
		 */
		void makeNoncurrent() override;

		/**
		 * Adds a new scene to the framebuffer.
		 * @see Framebuffer::addScene().
		 */
		void addScene(const SceneRef& scene) override;

		/**
		 * Removes a scene from the framebuffer.
		 * @see Framebuffer::removeScene().
		 */
		void removeScene(const SceneRef& scene) override;

		/**
		 * Cleares all scenes registered at the framebuffer.
		 * @see Framebuffer::clearScenes().
		 */
		void clearScenes() override;

		/**
		 * Returns the viewport of this framebuffer.
		 * @see Framebuffer::viewport().
		 */
		void viewport(unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height) const override;

		/**
		 * Sets the viewport of this framebuffer.
		 * @see Framebuffer::setViewport().
		 */
		void setViewport(const unsigned int left, const unsigned int top, const unsigned int width, const unsigned int height) override;

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

	protected:

		/**
		 * Creates a new NvidaSceneGraph framebuffer object.
		 * @param preferredGraphicAPI Preferred graphic API to be used for this framebuffer
		 */
		GIFramebuffer(const Engine::GraphicAPI preferredGraphicAPI = Engine::API_DEFAULT);

		/**
		 * Destructs a Global Illumination framebuffer object.
		 */
		~GIFramebuffer() override;

		/**
		 * Renders a subset of the frame into the framebuffer.
		 * @param lightSources The light sources that will be used for rendering
		 * @param group The group of tracing objects actually representing the geometry(s) of the scene
		 * @param threads The number of threads the are currently used concurrently to render the frame, with range [1, infinity)
		 * @param threadIndex The index of the thread currently executed, with range [0, threads)
		 * @param mustBeOne Unused parameter which must be 1
		 */
		void renderSubset(LightSources* lightSources, const TracingGroup* group, const unsigned int threads, const unsigned int threadIndex, const unsigned int mustBeOne);

		/**
		 * Renders a subset of the frame into the framebuffer.
		 * @param sobelResponse The sobel response of the pixel-accurate render result providing a measure for the number of necessary sub-pixel render iterations, must be valid
		 * @param sobelResponsePaddingElements The number of padding elements at the end of each sobel response row, in elements, with range [0, infinity)
		 * @param lightSources The light sources that will be used for rendering, must be valid
		 * @param group The group of tracing objects actually representing the geometry(s) of the scene
		 * @param threads The number of threads the are currently used concurrently to render the frame, with range [1, infinity)
		 * @param threadIndex The index of the thread currently executed, with range [0, threads)
		 * @param mustBeOne Unused parameter which must be 1
		 */
		void renderAntialiasedSubset(const uint8_t* sobelResponse, const unsigned int sobelResponsePaddingElements, LightSources* lightSources, const TracingGroup* group, const unsigned int threads, const unsigned int threadIndex, const unsigned int mustBeOne);

		/**
		 * Renders one specific ray for a given group of tracing objects and light sources.
		 * @param viewPosition The position of the view, defined within the same coordinate system as all other objects
		 * @param ray The specific ray for which the color value (the light) will be determined
		 * @param group The group of tracing objects actually representing the geometry(s) of the scene
		 * @param lightSources The light sources that will be used for rendering
		 * @param result The resulting color value (the light) of the specified ray
		 * @return True, if succeeded
		 */
		bool renderRay(const Vector3& viewPosition, const Line3& ray, const TracingGroup& group, const LightSources& lightSources, RGBAColor& result) const;

		/**
		 * Releases the framebuffer.
		 * @see Framebuffer::release().
		 */
		void release() override;

	protected:

		/// Preferred graphic API of this framebuffer.
		Engine::GraphicAPI preferredGraphicAPI_;

		/// Framebuffer frame.
		Frame frame_;

		/// Antialiasing enabled statement.
		bool antialiasingEnabled_;

		/// Optional framebuffer frame holding the sobel response for antialiasing.
		Frame sobelFrame_;

		/// Lighting modes for this framebuffer.
		Lighting::LightingModes lightingModes_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_FRAMEBUFFER_H
