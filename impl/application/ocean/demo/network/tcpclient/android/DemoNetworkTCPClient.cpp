/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/network/tcpclient/android/DemoNetworkTCPClient.h"
#include "application/ocean/demo/network/tcpclient/android/ClientThread.h"

#include "ocean/platform/android/Utilities.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_demo_network_tcpclient_DemoNetworkTCPClientActivity_invokeConnectClient(JNIEnv* env, jobject javaThis, jstring hostAddress, jint hostPort)
{
	const std::string stringHostAddress = Platform::Android::Utilities::toAString(env, hostAddress);

	return ClientThread::get().connect(stringHostAddress, hostPort);
}
