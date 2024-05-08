/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_CONSOLE_ANDROID_DEMO_CONSOLE_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_CONSOLE_ANDROID_DEMO_CONSOLE_H

#include "application/ocean/demo/base/ApplicationDemoBase.h"

#include <jni.h>

/**
 * @ingroup applicationdemobase
 * @defgroup applicationdemobaseconsoleandroid Console (Android)
 * @{
 * The demo application demonstrates the implementation of a simple console application.<br>
 * This application is platform dependent and is implemented for android platforms.
 * @}
 */

/**
 * Native interface function to invoke the demo application.
 * @param env Native interface environment
 * @param javaThis JNI object
 */
extern "C" void Java_com_meta_ocean_app_demo_base_console_android_DemoConsoleActivity_invokeApplication(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_CONSOLE_ANDROID_DEMO_CONSOLE_H
