/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_COLOR_H
#define META_OCEAN_INTERACTION_JS_COLOR_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript Color object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSColor : public JSObject<JSColor, RGBAColor>
{
	friend class JSObject<JSColor, RGBAColor>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			/// The accessor for the red property, a number value.
			AI_RED,
			/// The accessor for the green property, a number value.
			AI_GREEN,
			/// The accessor for the blue property, a number value.
			AI_BLUE,
			/// The accessor for the alpha property, a number value.
			AI_ALPHA
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			FI_CLAMP,
			FI_CLAMPED,
			FI_INTERPOLATE,
			FI_INVERT,
			FI_INVERTED,
			FI_IS_EQUAL,
			FI_IS_NORMALIZED,
			FI_MULTIPLY,
			FI_NORMALIZE,
			FI_NORMALIZED,
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

inline const char* JSColor::objectName()
{
	return "Color";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_COLOR_H
