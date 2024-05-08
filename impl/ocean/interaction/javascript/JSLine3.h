/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_LINE_3_H
#define META_OCEAN_INTERACTION_JS_LINE_3_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript Line3 object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSLine3 : public JSObject<JSLine3, Line3>
{
	friend class JSObject<JSLine3, Line3>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			/// The accessor for the point property, a Vector3 object.
			AI_POINT,
			/// The accessor for the direction property, a Vector3 object.
			AI_DIRECTION
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			/**
			 * Returns whether two line objects are equal up to some epsilon.
			 * <pre>
			 * Boolean = Line3.isEqual(Line3)
			 * </pre>
			 */
			FI_IS_EQUAL,

			/**
			 * Returns the distance between the line and another object
			 * <pre>
			 * Number = Line3.distance(Line3)
			 * Number = Line3.distance(Vector3)
			 * </pre>
			 */
			FI_DISTANCE,

			/**
			 * Returns the point on this line which is nearest to an aribtrary given point.
			 * <pre>
			 * Vector3 = Line3.nearestPoint(Vector3)
			 * </pre>
			 */
			FI_NEAREST_POINT,

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

inline const char* JSLine3::objectName()
{
	return "Line3";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_LINE_3_H
