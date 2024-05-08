/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_RENDERING_OBJECT_H
#define META_OCEAN_INTERACTION_JS_RENDERING_OBJECT_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript RenderingObject object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSRenderingObject : public JSObject<JSRenderingObject, Rendering::ObjectRef>
{
	friend class JSObject<JSRenderingObject, Rendering::ObjectRef>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			/// The accessor for the name property, a String value.
			AI_NAME,
			/// The accessor for the type property, a String value.
			AI_TYPE
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			FI_IS_VALID,
			FI_IS_INVALID,

			FI_ADD,
			FI_REMOVE,

			FI_PARENT,
			FI_PARENTS,

			FI_ACTIVE_NODE,
			FI_AMBIENT_COLOR,
			FI_ASPECT_RATIO,
			FI_ATTENUATION,
			FI_ATTRIBUTE,
			FI_BACKGROUND,
			FI_BACKGROUND_COLOR,
			FI_CHILD,
			FI_COLORS,
			FI_CONE_ANGLE,
			FI_CULLING_MODE,
			FI_DEPTH,
			FI_DIRECTION,
			FI_DIFFUSE_COLOR,
			FI_EMISSIVE_COLOR,
			FI_ENABLED,
			FI_ENVIRONMENT_MODE,
			FI_EXIST_PARAMETER,
			FI_FACE_MODE,
			FI_FAR_DISTANCE,
			FI_HEADLIGHT,
			FI_HEIGHT,
			FI_INTENSITY,
			FI_IS_COMPILED,
			FI_MAGNIFICATION_FILTER_MODE,
			FI_MEDIUM,
			FI_MINIFICATION_FILTER_MODE,
			FI_NEAR_DISTANCE,
			FI_NORMALS,
			FI_NUMBER_ATTRIBUTES,
			FI_NUMBER_CHILDREN,
			FI_PARAMETER,
			FI_PARAMETER_ELEMENTS,
			FI_PARAMETER_TYPE,
			FI_POSITION,
			FI_RADIUS,
			FI_REFLECTIVITY,
			FI_ROTATION,
			FI_SCALE,
			FI_SIZE,
			FI_SPECULAR_COLOR,
			FI_SPECULAR_EXPONENT,
			FI_SPOT_EXPONENT,
			FI_TEXTURE_COORDINATES,
			FI_TRANSLATION,
			FI_TRANSFORMATION,
			FI_TRANSPARENCY,
			FI_USE_HEADLIGHT,
			FI_VERTICES,
			FI_VERTEX_SET,
			FI_VISIBLE,
			FI_WIDTH,
			FI_WORLD_TRANSFORMATION,

			FI_SET_ACTIVE_NODE,
			FI_SET_AMBIENT_COLOR,
			FI_SET_ASPECT_RATIO,
			FI_SET_ATTENUATION,
			FI_ADD_BACKGROUND,
			FI_REMOVE_BACKGROUND,
			FI_SET_BACKGROUND_COLOR,
			FI_SET_COLORS,
			FI_SET_CONE_ANGLE,
			FI_SET_CODE,
			FI_SET_CODE_FILE,
			FI_SET_CULLING_MODE,
			FI_SET_DEPTH,
			FI_SET_DIRECTION,
			FI_SET_DIFFUSE_COLOR,
			FI_SET_EMISSIVE_COLOR,
			FI_SET_ENABLED,
			FI_SET_ENVIRONMENT_MODE,
			FI_SET_FACE_MODE,
			FI_SET_FAR_DISTANCE,
			FI_SET_USE_HEADLIGHT,
			FI_SET_HEIGHT,
			FI_SET_INTENSITY,
			FI_SET_LEFT_PROJECTION_MATRIX,
			FI_SET_LEFT_TRANSFORMATION,
			FI_SET_MAGNIFICATION_FILTER_MODE,
			FI_SET_MEDIUM,
			FI_SET_MINIFICATION_FILTER_MODE,
			FI_SET_NEAR_DISTANCE,
			FI_SET_NORMALS,
			FI_SET_PARAMETER,
			FI_SET_POSITION,
			FI_SET_PROJECTION_MATRIX,
			FI_SET_RADIUS,
			FI_SET_REFLECTIVITY,
			FI_SET_RIGHT_PROJECTION_MATRIX,
			FI_SET_RIGHT_TRANSFORMATION,
			FI_SET_ROTATION,
			FI_SET_SAMPLER,
			FI_SET_SIZE,
			FI_SET_SPECULAR_COLOR,
			FI_SET_SPECULAR_EXPONENT,
			FI_SET_SPOT_EXPONENT,
			FI_SET_TEXTURE_COORDINATES,
			FI_SET_TRANSLATION,
			FI_SET_TRANSFORMATION,
			FI_SET_TRANSPARENCY,
			FI_SET_VERTICES,
			FI_SET_VERTEX_SET,
			FI_SET_VISIBLE,
			FI_SET_WIDTH
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

inline const char* JSRenderingObject::objectName()
{
	return "RenderingObject";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_RENDERING_OBJECT_H
