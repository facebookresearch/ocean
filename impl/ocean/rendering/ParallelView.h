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
typedef SmartObjectRef<ParallelView> ParallelViewRef;

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
