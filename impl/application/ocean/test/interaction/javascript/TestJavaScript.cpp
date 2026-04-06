/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/interaction/javascript/TestJavaScript.h"

#include "ocean/base/Thread.h"

#include <v8.h>
#include <libplatform/libplatform.h>

using namespace Ocean;

/**
 * Simple singleton implementation holding a persistent object template.
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
				uniqueObjectTemplate_.Reset(isolate, objectTemplate);
			}

			return uniqueObjectTemplate_.Get(isolate);
		}

	private:

		/// Persistent object template object.
		v8::Persistent<v8::ObjectTemplate> uniqueObjectTemplate_;
};

/**
 * Simple thread implementation creating a V8 context in a separate thread.
 */
class TestThread : public Thread
{
	public:

		/**
		 * Creates a new test thread.
		 * @param isolate The V8 isolate to use
		 */
		explicit TestThread(v8::Isolate* isolate) :
			isolate_(isolate)
		{
			// nothing to do here
		}

	private:

		/**
		 * Member thread function.
		 */
		void threadRun() override
		{
			const v8::Locker locker(isolate_);
			const v8::Isolate::Scope isolateScope(isolate_);
			const v8::HandleScope handleScope(isolate_);

			context_ = v8::UniquePersistent<v8::Context>(isolate_, v8::Context::New(isolate_, nullptr, Singleton::get().objectTemplate()));
		}

	private:

		/// The V8 isolate.
		v8::Isolate* isolate_ = nullptr;

		/// Own V8 context.
		v8::UniquePersistent<v8::Context> context_;
};

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int /*argc*/, wchar_t** /*argv*/)
#elif defined(__APPLE__) || defined(__linux__)
	// main function on OSX and Linux platforms
	int main(int /*argc*/, char** /*argv*/)
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	// Initialize V8

	std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();

	v8::Isolate::CreateParams createParams;
	createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

	v8::Isolate* isolate = v8::Isolate::New(createParams);

	std::cout << "V8 version: " << V8_MAJOR_VERSION << "." << V8_MINOR_VERSION << "." << V8_BUILD_NUMBER << "." << V8_PATCH_LEVEL << std::endl;

	// Start the background thread which will lock/enter the isolate itself
	TestThread testThread(isolate);
	testThread.startThread();

	// Wait for the background thread to finish
	Thread::sleep(1000u);

	// Main thread: lock, enter the isolate, and create/destroy 100 contexts
	{
		const v8::Locker locker(isolate);
		const v8::Isolate::Scope isolateScope(isolate);
		const v8::HandleScope handleScope(isolate);

		for (unsigned int n = 0u; n < 100u; ++n)
		{
			v8::Persistent<v8::Context> anyContext(isolate, v8::Context::New(isolate, nullptr, Singleton::get().objectTemplate()));
			anyContext.Reset();
		}
	}

	std::cout << "V8 threading test succeeded." << std::endl;

	// Teardown V8

	isolate->Dispose();

	v8::V8::Dispose();
	v8::V8::DisposePlatform();

	delete createParams.array_buffer_allocator;

	return 0;
}
