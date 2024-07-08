/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_SCRIPT_H
#define META_OCEAN_INTERACTION_JS_SCRIPT_H

#include "ocean/interaction/javascript/JavaScript.h"

#include <v8.h>

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements an object holding a JavaScript code.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSScript
{
	public:

		/**
		 * Creates a new script object.
		 */
		JSScript() = default;

		/**
		 * Compiles and runs a script code.
		 * @param code Script code to compile
		 * @param errorMessage Resulting error message, if any
		 * @return True, if no error occurred during compilation
		 */
		bool compileAndRun(const std::string& code, std::string& errorMessage);

		/**
		 * Runs the entire script code.
		 * @param returnValue Resulting return value, if any
		 * @param errorMessage Resulting error message, if any
		 * @return True, if no error occurred
		 */
		bool run(v8::Handle<v8::Value>& returnValue, std::string& errorMessage);

		/**
		 * Returns whether the script has been compiled successfully.
		 * @return True, if so
		 */
		inline bool isCompiled() const;

		/**
		 * Extracts the error message from a tryCatch object.
		 * @param tryCatch The tryCatch object from which the error will be extracted
		 * @return The extracted error message
		 */
		static std::string extractErrorMessage(const v8::TryCatch& tryCatch);

	protected:

		/// Script handle object.
		v8::UniquePersistent<v8::Script> script_;
};

inline bool JSScript::isCompiled() const
{
	return !script_.IsEmpty();
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_SCRIPT_H
