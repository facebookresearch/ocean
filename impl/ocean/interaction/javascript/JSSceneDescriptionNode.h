/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_SCENE_DESCRIPTION_NODE_H
#define META_OCEAN_INTERACTION_JS_SCENE_DESCRIPTION_NODE_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript SceneDescriptionNode object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSSceneDescriptionNode : public JSObject<JSSceneDescriptionNode, SceneDescription::NodeRef>
{
	friend class JSObject<JSSceneDescriptionNode, SceneDescription::NodeRef>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			/// The accessor for the name property, a string value.
			AI_NAME,
			/// The accessor for the type property, a string value.
			AI_TYPE
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			FI_FIELD,
			FI_SET_FIELD,

			FI_HAS_FIELD,
			FI_FIELD_TYPE,
			FI_FIELD_DIMENSION,

			FI_IS_VALID,
			FI_IS_INVALID,

			FI_PARENT,
			FI_PARENTS
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

inline const char* JSSceneDescriptionNode::objectName()
{
	return "SceneDescriptionNode";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_SCENE_DESCRIPTION_NODE_H
