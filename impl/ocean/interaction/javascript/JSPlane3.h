/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_PLANE_3_H
#define META_OCEAN_INTERACTION_JS_PLANE_3_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript Plane object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSPlane3 : public JSObject<JSPlane3, Plane3>
{
	friend class JSObject<JSPlane3, Plane3>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			/// The accessor for the normal property, a Vector3 object.
			AI_NORMAL,
			/// The accessor for the distance property, a Number value.
			AI_DISTANCE
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			/**
			 * Returns the intersection between this plane and another plane  or a line.
			 * <pre>
			 * Line3 = Plane3.intersection(Plane3)
			 * Vector3 = Plane3.intersection(Line3)
			 * </pre>
			 */
			FI_INTERSECTION,

			/**
			 * Returns whether two line objects are equal up to some epsilon.
			 * <pre>
			 * Boolean = Line3.isEqual(Line3)
			 * </pre>
			 */
			FI_IS_EQUAL,

			/**
			 * Returns whether two plane objects, or this plane and another line are parallel.
			 * <pre>
			 * Boolean = Plane3.isParallel(Plane3)
			 * Boolean = Plane3.isParallel(Line3)
			 * </pre>
			 */
			FI_IS_PARALLEL,

			/**
			 * Returns a string with the values of this line object.
			 * <pre>
			 * String = Line3.string()
			 * </pre>
			 */
			FI_STRING
		};

	public:

		/**
		 * Returns the JavaScript name of this object.
		 * @return The object's JavaScript name
		 */
		static inline const char* objectName();

	protected:

		/**
		 * Creates the function template and object template for this object.
		 */
		static void createFunctionTemplate();
};

inline const char* JSPlane3::objectName()
{
	return "Plane3";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_PLANE_3_H
