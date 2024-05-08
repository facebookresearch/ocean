/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/interaction/javascript/win/JavaScriptMain.h"

#include "ocean/base/Thread.h"

#include <v8.h>

#include <windows.h>

using namespace Ocean;

/**
 * Simple singleton implementation.
 */
class Singleton
{
	public:

		/**
		 * Returns the unique singleton object.
		 */
		static Singleton& get()
		{
			static Singleton singleton;
			return singleton;
		}

		/**
		 * Returns the unique object template.
		 */
		v8::Local<v8::ObjectTemplate> objectTemplate()
		{
			v8::Isolate* isolate = v8::Isolate::GetCurrent();

			if (uniqueObjectTemplate_.IsEmpty())
			{
				v8::Local<v8::ObjectTemplate> objectTemplate(v8::ObjectTemplate::New(isolate));

				// normally here function templates would be added
				//objectTemplate->Set(v8::String::New("functionName"), createSpecificFunctionTemplate());

				uniqueObjectTemplate_.Reset(isolate, objectTemplate);
			}

			return uniqueObjectTemplate_.Get(isolate);
		}

	private:

		/// Persistent object template object.
		v8::Persistent<v8::ObjectTemplate> uniqueObjectTemplate_;
};

/**
 * Simple thread implementation.
 */
class TestThread : public Thread
{
	private:

		/**
		 * Member thread function.
		 */
		void threadRun() override
		{
			v8::Isolate* isolate = v8::Isolate::GetCurrent();

			const v8::Locker locker(isolate);
			const v8::HandleScope handleScope(isolate);

			context_ = v8::UniquePersistent<v8::Context>(isolate, v8::Context::New(isolate, nullptr, Singleton::get().objectTemplate()));
		}

	private:

		/// Own V8 context.
		v8::UniquePersistent<v8::Context> context_;
};

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int /*argc*/, wchar_t** /*argv*/)
#elif defined(__APPLE__) || defined(__linux__)
	// main function on OSX platforms
	int main(int /*argc*/, char** /*argv*/)
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	TestThread testThread;
	testThread.startThread();

	Thread::sleep(1000u);

	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	for (unsigned int n = 0u; n < 100u; ++n)
	{
		const v8::Locker locker(isolate);
		const v8::HandleScope handleScope(isolate);

		{
			v8::Persistent<v8::Context> anyContext = v8::Persistent<v8::Context>(isolate, v8::Context::New(isolate, nullptr, Singleton::get().objectTemplate()));

			// do something with this context

			anyContext.Empty();
		}
	}

	std::cout << "Press a key to exit" << std::endl;
	getchar();

	return 0;
}
