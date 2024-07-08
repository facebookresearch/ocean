/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_VIEW_H
#define META_OCEAN_RENDERING_VIEW_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Background.h"
#include "ocean/rendering/Node.h"
#include "ocean/rendering/Object.h"
#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/PhantomAttribute.h"

#include "ocean/math/Line3.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/RGBAColor.h"
#include "ocean/math/SquareMatrix4.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration.
class View;

/**
 * Definition of a smart object reference holding a view node.
 * A view can holds several background objects, however only the first one is used for rendering.
 * @see SmartObjectRef, View.
 * @ingroup rendering
 */
typedef SmartObjectRef<View> ViewRef;

/**
 * This class is the base class for all rendering views.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT View : virtual public Object
{
	public:

		/**
		 * Definition of a vector holding background objects.
		 */
		typedef std::vector<BackgroundRef> Backgrounds;

	public:

		/**
		 * Returns the aspect ratio of this view.
		 * @return Ratio between width and height
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setAspectRatio().
		 */
		virtual Scalar aspectRatio() const;

		/**
		 * Returns the distance to the near clipping plane.
		 * @return Distance to near clipping plane
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setNearDistance(), farDistance().
		 */
		virtual Scalar nearDistance() const;

		/**
		 * Returns the distance to the far clipping plane.
		 * @return Distance to far clipping plane
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setFarDistance(), nearDistance().
		 */
		virtual Scalar farDistance() const;

		/**
		 * Returns the transformation between view and world (the extrinsic camera data, the inverse of the known View Matrix).
		 * @return The view's transformation, which is world_T_view
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setTransformation().
		 */
		virtual HomogenousMatrix4 transformation() const;

		/**
		 * Returns the projection matrix of this view.
		 * @return The view's projection matrix, which is clip_T_view
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual SquareMatrix4 projectionMatrix() const;

		/**
		 * Returns the background color of the view.
		 * @return Background color
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see background(), backgrounds().
		 */
		virtual RGBAColor backgroundColor() const;

		/**
		 * Returns the currently used background of the view.
		 * @return Background object
		 * @see backgrounds(), backgroundColor().
		 */
		virtual BackgroundRef background() const;

		/**
		 * Returns all defined background objects.
		 * A view can hold more than one background object, however only the first background object is used for rendering
		 * @return Defined background objects
		 * @see background(), backgroundColor().
		 */
		virtual Backgrounds backgrounds() const;

		/**
		 * Returns whether the head light is enabled.
		 * @return True, if so
		 * @see setUseHeadlight().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool useHeadlight() const;

		/**
		 * Returns the phantom mode of this view.
		 * @return Phantom mode
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setPhantomMode().
		 */
		virtual PhantomAttribute::PhantomMode phantomMode() const;

		/**
		 * Returns a viewing ray into the scene.
		 * The start position is located at the center of projection.
		 * @param x Horizontal position on the projection plane defining the intersection between plane and ray, in pixel
		 * @param y Vertical position on the projection plane defining the intersection between plane and ray, in pixel
		 * @param width Horizontal width of the projection plane in pixel
		 * @param height Vertical height of the projection plane in pixel
		 * @return Resulting viewing ray
		 */
		virtual Line3 viewingRay(const Scalar x, const Scalar y, const unsigned int width, const unsigned int height) const;

		/**
		 * Sets the aspect ratio of this view.
		 * @param aspectRatio The ratio between width and height, with range (0, infinity)
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see aspectRatio().
		 */
		virtual bool setAspectRatio(const Scalar aspectRatio);

		/**
		 * Sets the distance to the near clipping plane.
		 * @param distance The distance to the near clipping plane to set, with range (0, farDistance())
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see nearDistance(), setFarDistance(), setNearFarDistance().
		 */
		virtual bool setNearDistance(const Scalar distance);

		/**
		 * Sets the distance to the far clipping plane.
		 * @param distance The distance to the far clipping plane to set, with range (nearDistance(), infinity)
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see farDistance(), setNearDistance(), setNearFarDistance().
		 */
		virtual bool setFarDistance(const Scalar distance);

		/**
		 * Sets the distance to the near and far clipping plane.
		 * @param nearDistance The distance to the near clipping plane, with range (0, farDistance)
		 * @param farDistance The distance to the far clipping plane to set, with range (nearDistance, infinity)
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see farDistance(), setNearDistance().
		 */
		virtual bool setNearFarDistance(const Scalar nearDistance, const Scalar farDistance);

		/**
		 * Sets the position and orientation of the view in world coordinates (the extrinsic camera data, the inverse of the known View Matrix).
		 * @param world_T_view The transformation converting the view to world, must be valid
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see transformation().
		 */
		virtual void setTransformation(const HomogenousMatrix4& world_T_view);

		/**
		 * Sets the background color of the view.
		 * @param color Background color to set
		 * @return True, if the color is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see backgroundColor(), addBackground(), removeBackground().
		 */
		virtual bool setBackgroundColor(const RGBAColor& color);

		/**
		 * Adds a new background object to the background stack of the view.
		 * A view can hold more than one background object, however the first background object is used for rendering only.
		 * @param background Background to add
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see background(), removeBackground(), setBackgroundColor().
		 */
		virtual void addBackground(const BackgroundRef& background);

		/**
		 * Removes a background object from the background stack of this view.
		 * If the background to remove is the currently first object in the background stack, the next background object in the queue will be used for rendering.
		 * @param background Background to remove
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void removeBackground(const BackgroundRef& background);

		/**
		 * Sets whether the headlight should be activated.
		 * @param state True, to activate the headlight
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see useHeadlight().
		 */
		virtual void setUseHeadlight(const bool state);

		/**
		 * Sets the phantom mode of this view.
		 * @param mode Phantom mode to be set
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see phantomMode().
		 */
		virtual bool setPhantomMode(const PhantomAttribute::PhantomMode mode);

		/**
		 * Fits the camera to a given node.
		 * If no node is specified the entire scene world is fitted.
		 * @param node Node to fit the camera to
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void fitCamera(const NodeRef& node = NodeRef());

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new view object.
		 */
		View();

		/**
		 * Destructs a view object.
		 */
		~View() override;

	protected:

		/// The view's background objects.
		Backgrounds backgrounds_;
};

}

}

#endif // META_OCEAN_RENDERING_VIEW_H
