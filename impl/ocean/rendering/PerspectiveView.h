/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_PERSPECTIVE_VIEW_H
#define META_OCEAN_RENDERING_PERSPECTIVE_VIEW_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/View.h"

#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class PerspectiveView;

/**
 * Definition of a smart object reference holding a perspective view node.
 * @see SmartObjectRef, PerspectiveView.
 * @ingroup rendering
 */
typedef SmartObjectRef<PerspectiveView> PerspectiveViewRef;

/**
 * This class is the base class for all perspective views.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT PerspectiveView : virtual public View
{
	public:

		/**
		 * Returns the horizontal field of view in radian.
		 * @return Horizontal field of view in radian, with range (0, PI)
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Scalar fovX() const;

		/**
		 * Sets the horizontal field of view in radian.
		 * @param fovx Horizontal field of view in radian, with range (0, PI)
		 * @return True, if the value is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setFovX(const Scalar fovx);

		/**
		 * Calculates the ideal field of view matching to the current background.
		 * If the background holds a medium and no valid frame has been provided by this medium jet this function will return the view's field of view.
		 * @param validCamera Optional resulting state determining whether the background had a valid camera object
		 * @return Resulting field of view in radian
		 */
		virtual Scalar idealFovX(bool* validCamera = nullptr) const;

		/**
		 * Updates the view's field of view to the ideal field of view matching with the current background.
		 */
		virtual void updateToIdealFovX();

		/**
		 * Sets the projection matrix explicitly.
		 * @param clip_T_view The projection matrix to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setProjectionMatrix(const SquareMatrix4& clip_T_view);

		/**
		 * Fits the camera to a given node.
		 * @see View::fitCamera().
		 */
		void fitCamera(const NodeRef& node = NodeRef()) override;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new perspective view object.
		 */
		PerspectiveView();

		/**
		 * Destructs a perspective view object.
		 */
		~PerspectiveView() override;
};

}

}

#endif // META_OCEAN_RENDERING_PERSPECTIVE_VIEW_H
