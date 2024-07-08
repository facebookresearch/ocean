/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/base/console/android/DemoConsole.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Exception.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Processor.h"
#include "ocean/base/String.h"

#include "ocean/platform/android/Utilities.h"

using namespace Ocean;

void Java_com_meta_ocean_app_demo_base_console_android_DemoConsoleActivity_invokeApplication(JNIEnv* env, jobject javaThis)
{
	try
	{
		Log::info() << "Console demo application";
		Log::info() << " ";

		Log::info() << "Platform: " << Build::buildString();
		Log::info() << "Build: " << Build::buildString();
		Log::info() << "Time: " << DateTime::localString();
		Log::info() << " ";

		Log::info() << "While the hardware supports the following SIMD instructions:";
		Log::info() << Processor::translateInstructions(Processor::get().instructions());
		Log::info() << " ";

		Log::info() << "Processor: " << Processor::brand();
		Log::info() << " ";

		std::string deviceBrand;
		if (Platform::Android::Utilities::deviceBrand(env, deviceBrand))
		{
			Log::info() << "Device brand: " << deviceBrand;
			Log::info() << " ";
		}

		std::string deviceModel;
		if (Platform::Android::Utilities::deviceModel(env, deviceModel))
		{
			Log::info() << "Device model: " << deviceModel;
			Log::info() << " ";
		}

		std::string packageName;
		if (Platform::Android::Utilities::determinePackageName(packageName))
		{
			Log::info() << "Package name: " << packageName;
			Log::info() << " ";
		}

		std::string value;
		if (Platform::Android::Utilities::systemPropertyValue("ro.product.cpu.abi", value))
		{
			Log::info() << "CPU ABI: " << value;
			Log::info() << " ";
		}

		Log::info() << "... do something here ...";
		Log::info() << " ";

		Log::info() << "Finished";
	}
	catch (const Exception& exception)
	{
		Log::error() << "Unhandled exception:\n";
		Log::error() << exception.what();
	}
}
