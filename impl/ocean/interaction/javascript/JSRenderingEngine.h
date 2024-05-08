/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_RENDERING_ENGINE_H
#define META_OCEAN_INTERACTION_JS_RENDERING_ENGINE_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript RenderingEngine object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSRenderingEngine : public JSObject<JSRenderingEngine, Rendering::EngineRef>
{
	friend class JSObject<JSRenderingEngine, Rendering::EngineRef>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			/// The accessor for the name property, a String value.
			AI_NAME
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			/**
			 * Creates a new rendering object.
			 * <pre>
			 * RenderingObject = RenderingEngine.createObject()
			 * </pre>
			 */
			FI_CREATE_OBJECT,

			/**
			 * Returns the framebuffer rendering object.
			 * <pre>
			 * RenderingObject = RenderingEngine.framebuffer()
			 * </pre>
			 */
			FI_FRAMEBUFFER,

			/**
			 * Returns whether this rendering engine is valid.
			 * <pre>
			 * Boolean = RenderingEngine.isValid()
			 * </pre>
			 */
			FI_IS_VALID,

			/**
			 * Returns whether this rendering engine is invalid.
			 * <pre>
			 * Boolean = RenderingEngine.isInvalid()
			 * </pre>
			 */
			FI_IS_INVALID,

			/**
			 * Finds any rendering object with specified name.
			 * <pre>
			 * RenderingObject = RenderingEngine.findObject(String)
			 * </pre>
			 */
			FI_FIND_OBJECT,

			/**
			 * Finds all rendering objects with sepcified name.
			 * <pre>
			 * Array<RenderingObject> = RenderingEngine.findObjects(String)
			 * </pre>
			 */
			FI_FIND_OBJECTS,

			/**
			 * Returns the view rendering object.
			 * <pre>
			 * RenderingObject = RenderingEngine.view()
			 * </pre>
			 */
			FI_VIEW
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

inline const char* JSRenderingEngine::objectName()
{
	return "RenderingEngine";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_RENDERING_ENGINE_H
