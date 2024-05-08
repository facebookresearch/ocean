/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_FRAMEBUFFER_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/DynamicObject.h"
#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/PhantomAttribute.h"
#include "ocean/rendering/PrimitiveAttribute.h"
#include "ocean/rendering/Scene.h"
#include "ocean/rendering/View.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Line3.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Framebuffer;

/**
 * Definition of a smart object reference holding a framebuffer.
 * @see SmartObjectRef, Framebuffer.
 * @ingroup rendering
 */
typedef SmartObjectRef<Framebuffer> FramebufferRef;

/**
 * This class is the base class for all rendering framebuffers.
 * The framebuffer holds the view, the scene and is able to render frames respectively.<br>
 * A framebuffer instance can be received from the associated engine instance.<br>
 * @see Engine, Scene, View
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Framebuffer : virtual public Object
{
	public:

		/**
		 * Definition of different framebuffer types.
		 */
		enum FramebufferType
		{
			/// Invalid framebuffer type.
			FRAMEBUFFER_INVALID,
			/// Bitmap framebuffer type.
			FRAMEBUFFER_BITMAP,
			/// Window framebuffer type.
			FRAMEBUFFER_WINDOW
		};

		/**
		 * Definition of different render techniques.
		 */
		enum RenderTechnique
		{
			/// Full render technique.
			TECHNIQUE_FULL,
			/// Render technique with textures.
			TECHNIQUE_TEXTURED,
			/// Render technique with simple shading.
			TECHNIQUE_SHADED,
			/// Render technique without lighting.
			TECHNIQUE_UNLIT
		};

		/**
		 * Definition of different shadow techniques.
		 */
		enum ShadowTechnique
		{
			/// No use of shadows.
			SHADOW_TECHNIQUE_NONE,
			/// Use of shadow maps.
			SHADOW_TECHNIQUE_MAPS
		};

		/**
		 * This class holds framebuffer configurations.
		 */
		class FramebufferConfig
		{
			public:

				/// True, for a framebuffer using a stencil buffer.
				bool useStencilBuffer = false;
		};

		/**
		 * Definition of different rendering face modes.
		 * These modes correspond to those defined in PrimitiveAttribute except the meaning of the default mode.<br>
		 * The default face mode gives the responsibility to the renderable object themselves, and does not overwrite the global state.
		 * @see PrimitiveAttribute::FaceMode.
		 */
		typedef PrimitiveAttribute::FaceMode FaceMode;

		/**
		 * Definition of different rendering face culling modes.
		 * These modes correspond to those defined in PrimitiveAttribute except the meaning of the default culling mode.<br>
		 * The default culling mode gives the responsibility to the renderable object themselves, and does not overwrite the global state.
		 * @see PrimitiveAttribute::CullingMode.
		 */
		typedef PrimitiveAttribute::CullingMode CullingMode;

		/**
		 * Definition of different face lighting modes.
		 * These modes correspond to those defined in PrimitiveAttribute except the meaning of the default lighting mode.<br>
		 * The default culling mode gives the responsibility to the renderable object themselves, and does not overwrite the global state.
		 * @see PrimitiveAttribute::LightingMode.
		 */
		typedef PrimitiveAttribute::LightingMode LightingMode;

		/**
		 * Definition of different rendering phantom modes.
		 * These modes correspond to those defined in PhantomMode except the meaning of the default culling mode.<br>
		 * The default culling mode gives the responsibility to the renderable objects themselves, and does not overwrite the global state.
		 */
		typedef PhantomAttribute::PhantomMode PhantomMode;

		/**
		 * Definition of a callback function for render events.
		 * This callback can be used to apply custom render calls if necessary.
		 * First parameter (eyeIndex): The index of the framebuffer (eye) to be rendered, will always be 0 in a mono view
		 * Second parameter (view_T_world): The current transformation between world and the current view (camera/eye), also known as viewing matrix
		 * Third parameter (projection): The projection matrix of the view (eye)
		 * Fourth parameter (predictedDisplayTime): The predicted timestamp when the image will be display
		 */
		typedef Callback<void, const size_t, const HomogenousMatrix4, const SquareMatrix4, const Timestamp> RenderCallback;

	public:

		/**
		 * Returns the type of this framebuffer.
		 * @return Framebuffer type
		 */
		virtual FramebufferType framebufferType() const = 0;

		/**
		 * Returns the scene connected with this framebuffer.
		 * @return Scene connected with this framebuffer
		 * @see addScenes(), removeScene(), clearScenes().
		 */
		virtual const Scenes& scenes() const;

		/**
		 * Returns the view connected with this framebuffer.
		 * @return View connected with this framebuffer
		 * @see setView().
		 */
		virtual const ViewRef& view() const;

		/**
		 * Returns the viewport of this framebuffer.
		 * @param left Resulting left framebuffer position in pixel
		 * @param top Resulting top framebuffer position in pixel
		 * @param width Resulting framebuffer width in pixel
		 * @param height Resulting framebuffer height in pixel
		 * @see setViewport().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void viewport(unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height) const;

		/**
		 * Returns the global face mode of the entire framebuffer.
		 * @return Global framebuffer face mode
		 * @see setFaceMode(), PrimitiveAttribute::faceMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual FaceMode faceMode() const;

		/**
		 * Returns the global face culling mode of the entire framebuffer.
		 * @return Global framebuffer culling mode
		 * @see setCullingMode(), PrimitiveAttribute::cullingMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual CullingMode cullingMode() const;

		/**
		 * Returns the global lighting mode of the entire framebuffer.
		 * @return Global framebuffer lighting mode
		 * @see setLightingMode(), PrimitiveAttribute::lightingMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual LightingMode lightingMode() const;

		/**
		 * Returns the render technique of the entire framebuffer.
		 * @return Framebuffer render technique
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual RenderTechnique renderTechnique() const;

		/**
		 * Returns whether the framebuffer supports quad-buffered stereo views.
		 * @return True, if so
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool isQuadbufferedStereoSupported() const;

		/**
		 * Returns whether the framebuffer supports hardware anti-aliasing.
		 * @param buffers Number of buffers to be checked
		 * @return True, if so
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool isAntialiasingSupported(const unsigned int buffers) const;

		/**
		 * Returns whether the framebuffer uses hardware anti-aliasing.
		 * @return True, if so
		 * @see isAntialiasingSupported().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool isAntialiasing() const;

		/**
		 * Returns the shadow technique of the entire framebuffer.
		 * @return Framebuffer shadow technique
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ShadowTechnique shadowTechnique() const;

		/**
		 * Returns the transformation between display and device.
		 * @return The transformation between display and device
		 */
		virtual HomogenousMatrix4 device_T_display() const;

		/**
		 * Returns whether the framebuffer is synchronized with the vertical sync signal of the display device.
		 * @return True, if so
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool verticalSynchronization() const;

		/**
		 * Returns all supported extensions of this framebuffer.
		 * @return Extensions separated by a blank
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual std::string extensions() const;

		/**
		 * Returns whether the framebuffer and thus e.g. the platform supports a specific extension.
		 * @param extension The extension to check, must be valid
		 * @return True, if the extension is supported
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool hasExtension(const std::string& extension) const;

		/**
		 * Adds a new scene to the framebuffer.
		 * @param scene The scene to add
		 * @see removeScene(), clearScenes(), scenes().
		 */
		virtual void addScene(const SceneRef& scene);

		/**
		 * Removes a scene from the framebuffer.
		 * @param scene The scene to remove
		 * @see addScene(), clearScenes(), scenes().
		 */
		virtual void removeScene(const SceneRef& scene);

		/**
		 * Clears all scenes registered at the framebuffer.
		 * @see addScene(), removeScene(), scenes().
		 */
		virtual void clearScenes();

		/**
		 * Connects a view with this framebuffer.
		 * @param view The view to set
		 * @see view().
		 */
		virtual void setView(const ViewRef& view);

		/**
		 * Sets the viewport of this framebuffer.
		 * @param left New left position of the framebuffer in pixel
		 * @param top New top position of the framebuffer in pixel
		 * @param width New width of the framebuffer in pixel
		 * @param height New height of the framebuffer in pixel
		 * @see viewport().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setViewport(const unsigned int left, const unsigned int top, const unsigned int width, const unsigned int height);

		/**
		 * Sets the preferred pixel format of this framebuffer.
		 * This value should be defined before initialization during e.g. a window context.<br>
		 * Beware: There is no guarantee that this pixel format can be provides by the underlying hardware!
		 * @param pixelFormat The preferred pixel format used for rendering
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setPreferredPixelFormat(const FrameType::PixelFormat pixelFormat);

		/**
		 * Sets the global face mode of the entire framebuffer.
		 * This face mode can be used to overwrite the individual modes of all renderable nodes.
		 * @param faceMode Global face mode to set
		 * @see faceMode(), PrimitiveAttribute::setFaceMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setFaceMode(const FaceMode faceMode);

		/**
		 * Sets the global culling mode of the entire framebuffer.
		 * This culling mode can be used to overwrite the individual modes of all renderable nodes.
		 * @param cullingMode Global culling mode to set
		 * @see cullingMode(), PrimitiveAttribute::setCullingMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setCullingMode(const CullingMode cullingMode);

		/**
		 * Sets the global lighting mode of the entire framebuffer.
		 * This lighting mode can be used to overwrite the individual modes of all renderable nodes.
		 * @param lightingMode Global lighting mode to set
		 * @see lightingMode(), PrimitiveAttribute::setLightingMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setLightingMode(const LightingMode lightingMode);

		/**
		 * Sets the rendering technique of the entire framebuffer.
		 * @param technique Render technique to set
		 * @see renderTechnique().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setRenderTechnique(const RenderTechnique technique);

		/**
		 * Sets whether this framebuffer will support quad-buffered stereo views.
		 * Beware: Set this option once before initialization
		 * @param enable True, if quad-buffered stereo views will be supported
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setSupportQuadbufferedStereo(const bool enable);

		/**
		 * Sets whether this framebuffer will support hardware anti-aliasing.
		 * Beware: Set this option once before initialization
		 * @param buffers Number of buffers used for anti-aliasing
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setSupportAntialiasing(const unsigned int buffers);

		/**
		 * Sets whether the framebuffer uses hardware anti-aliasing.
		 * The framebuffer must support anti-aliasing to set this option.
		 * @see setSupportAntialiasing().
		 * @param antialiasing True, if so
		 * @return True, if succeeded
		 */
		virtual bool setAntialiasing(const bool antialiasing);

		/**
		 * Sets the shadow technique of the entire framebuffer.
		 * @param technique Shadow technique to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setShadowTechnique(const ShadowTechnique technique);

		/**
		 * Sets the transformation between display and device.
		 * @param device_T_display The transformation between display and device, must be valid
		 * @return True, if the transformation could be set
		 */
		virtual bool setDevice_T_display(const HomogenousMatrix4& device_T_display);

		/**
		 * Sets whether the framebuffer will is synchronized with the vertical sync signal of the display device.
		 * @param synchronize True, if so
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setVerticalSynchronization(const bool synchronize);

		/**
		 * Sets an optional callback function for pre-render events.
		 * The callback function will be invoked after the framebuffer has been cleared but before the scene graph content is rendered into the framebuffer.
		 * @param renderCallback The callback function to be set, an invalid object to remove a previously registered callback
		 * @see setPostRenderCallback().
		 */
		inline void setPreRenderCallback(const RenderCallback& renderCallback);

		/**
		 * Sets an optional callback function for pre-render events.
		 * The callback function will be invoked after the scene graph content has been rendered into the framebuffer.
		 * @param renderCallback The callback function to be set, an invalid object to remove a previously registered callback
		 * @see setPreRenderCallback().
		 */
		inline void setPostRenderCallback(const RenderCallback& renderCallback);

		/**
		 * Renders the next frame into the framebuffer.
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void render();

		/**
		 * Renders the next frame and copies the image content into to a given frame.
		 * @param frame The frame receiving the image content, will be modified if the frame is invalid or if the frame type does not match
		 * @param depthFrame Optional resulting depth frame, nullptr if not of interest
		 * @return True, if succeeded; False, if the function is not supported
		 */
		virtual bool render(Frame& frame, Frame* depthFrame = nullptr);

		/**
		 * Returns the renderable object intersected by a given ray.
		 * @param ray Intersection ray
		 * @param renderable Resulting renderable object intersected by the given ray, if any
		 * @param position Resulting intersection point on the renderable object
		 * @return True, if a renderable object has been intersected
		 */
		virtual bool intersection(const Line3& ray, RenderableRef& renderable, Vector3& position);

		/**
		 * Makes this framebuffer to the current one.
		 * @see makeNoncurrent().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void makeCurrent();

		/**
		 * Makes this framebuffer to non current.
		 * @see makeCurrent().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void makeNoncurrent();

		/**
		 * Linearizes a given z-buffer depth frame with non-linear (e.g., logarithmic) depth.
		 * @param depthFrame The non-linear depth to linearize, must be valid
		 * @param nearDistance The distance to the near clipping plane of the depth frame, with range (0, infinity)
		 * @param farDistance The distance to the far clipping plane of the depth frame, with range (nearDistance, infinity)
		 * @return True, if succeeded
		 */
		virtual bool linearizeDepthFrame(Frame& depthFrame, const Scalar nearDistance, const Scalar farDistance) const;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Returns the lock object of this engine.
		 * Beware: Use this lock with caution.
		 * @return Engine lock object
		 */
		inline Lock& lock() const;

	protected:

		/**
		 * Creates a new framebuffer.
		 */
		Framebuffer();

		/**
		 * Destructs a framebuffer.
		 */
		~Framebuffer() override;

		/**
		 * Initializes the framebuffer.
		 * @param shareFramebuffer Optional framebuffer to share resources with
		 * @return True, if succeeded
		 */
		virtual bool initialize(const FramebufferRef& shareFramebuffer = FramebufferRef());

		/**
		 * Releases the framebuffer.
		 * This function will be called one before the framebuffer will be destroyed.
		 */
		virtual void release();

	protected:

		/// Scenes registered at the framebuffer.
		Scenes framebufferScenes;

		/// View connected with the framebuffer.
		ViewRef framebufferView;

		/// Optional callback function for pre-render events.
		RenderCallback preRenderCallback_;

		/// Optional callback function for post-render events.
		RenderCallback postRenderCallback_;
};

inline Lock& Framebuffer::lock() const
{
	return objectLock;
}

inline void Framebuffer::setPreRenderCallback(const RenderCallback& renderCallback)
{
	const ScopedLock scopedLock(objectLock);

	preRenderCallback_ = renderCallback;
}

inline void Framebuffer::setPostRenderCallback(const RenderCallback& renderCallback)
{
	const ScopedLock scopedLock(objectLock);

	postRenderCallback_ = renderCallback;
}

}

}

#endif // META_OCEAN_RENDERING_FRAMEBUFFER_H
