/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_PARALLEL_VIEW_H
#define META_OCEAN_RENDERING_PARALLEL_VIEW_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/View.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class ParallelView;

/**
 * Definition of a smart object reference holding a parallel view node.
 * @see SmartObjectRef, ParallelView.
 * @ingroup rendering
 */
using ParallelViewRef = SmartObjectRef<ParallelView>;

/**
 * This class is the base class for all parallel views
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT ParallelView : virtual public View
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Returns the width of the orthographic viewing box in world units.
		 * @return The view width in world units
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Scalar width() const;

		/**
		 * Sets the width of the orthographic viewing box in world units.
		 * The height is automatically derived from the width and aspect ratio.
		 * @param width The view width in world units, with range (0, infinity)
		 * @return True if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setWidth(const Scalar width);

	protected:

		/**
		 * Creates a new parallel view object.
		 */
		ParallelView();

		/**
		 * Destructs a parallel view object.
		 */
		~ParallelView() override;
};

}

}

#endif // META_OCEAN_RENDERING_PARALLEL_VIEW_H
