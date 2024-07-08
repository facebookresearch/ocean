/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSScript.h"
#include "ocean/interaction/javascript/JSBase.h"
#include "ocean/interaction/javascript/JSContext.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

bool JSScript::compileAndRun(const std::string& code, std::string& errorMessage)
{
	if (!script_.IsEmpty())
	{
		ocean_assert(false && "This script is already compiled");
		return false;
	}

	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::MaybeLocal<v8::String> maybeCode(v8::String::NewFromUtf8(isolate, code.c_str()));

	if (maybeCode.IsEmpty())
	{
		errorMessage = "Not enough memory";
		return false;
	}

	const v8::TryCatch tryCatch(isolate);

	v8::MaybeLocal<v8::Script> maybeScript(v8::Script::Compile(JSContext::currentContext(), maybeCode.ToLocalChecked()));

	if (!maybeScript.IsEmpty())
	{
		script_ = v8::UniquePersistent<v8::Script>(isolate, maybeScript.ToLocalChecked());
	}

	v8::MaybeLocal<v8::Value> scriptResult;

	if (!script_.IsEmpty())
	{
		scriptResult = script_.Get(isolate)->Run(JSContext::currentContext());
	}

	if (scriptResult.IsEmpty())
	{
		errorMessage = extractErrorMessage(tryCatch);
	}

	return !script_.IsEmpty();
}

bool JSScript::run(v8::Handle<v8::Value>& returnValue, std::string& errorMessage)
{
	if (script_.IsEmpty())
	{
		ocean_assert(false && "Script is not compiled!");
		return false;
	}

	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	const v8::TryCatch tryCatch(isolate);

	v8::MaybeLocal<v8::Value> scriptResult = script_.Get(isolate)->Run(JSContext::currentContext());

	if (!scriptResult.IsEmpty())
	{
		returnValue = scriptResult.ToLocalChecked();
	}

	if (tryCatch.HasCaught())
	{
		errorMessage = extractErrorMessage(tryCatch);

		return false;
	}

	return true;
}

std::string JSScript::extractErrorMessage(const v8::TryCatch& tryCatch)
{
	std::string result;

	const v8::Local<v8::Message> message(tryCatch.Message());

	const std::string sourceLine(JSBase::toAString(message->GetSourceLine(JSContext::currentContext())));

	if (!sourceLine.empty())
	{
		result += std::string("\"") + sourceLine + std::string("\": ");
	}

	const v8::Maybe<int32_t> maybeLine = message->GetLineNumber(JSContext::currentContext());

	if (maybeLine.IsJust())
	{
		result += std::string("In line: ") + String::toAString(maybeLine.FromJust());
	}

	const v8::Maybe<int32_t> maybeColumn = message->GetStartColumn(JSContext::currentContext());

	if (maybeColumn.IsJust())
	{
		result += std::string(", in column: ") + String::toAString(maybeColumn.FromJust()) + std::string(": ");
	}

	if (!tryCatch.Exception().IsEmpty() && tryCatch.Exception()->IsString())
	{
		const std::string error(JSBase::toAString(tryCatch.Exception()->ToString(JSContext::currentContext())));

		if (error.empty())
		{
			result += error;
		}
	}

	return result;
}

}

}

}
