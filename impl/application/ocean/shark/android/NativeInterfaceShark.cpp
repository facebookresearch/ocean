/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/shark/android/NativeInterfaceShark.h"
#include "application/ocean/shark/android/GLMainView.h"

namespace Ocean
{

namespace Shark
{

namespace Android
{

jboolean Java_com_meta_ocean_app_shark_android_NativeInterfaceShark_loadScene(JNIEnv* env, jobject javaThis, jstring filename, jboolean replace)
{
	jboolean isCopy = false;
	const char* filenamePtr = env->GetStringUTFChars(filename, &isCopy);

	bool result = false;

	if (filenamePtr)
	{
		const std::string sceneFilename(filenamePtr);

		if (!sceneFilename.empty())
			result = GLMainView::get<GLMainView>().loadScene(sceneFilename, replace);

		env->ReleaseStringUTFChars(filename, filenamePtr);
	}

	return result;
}

}

}

}
