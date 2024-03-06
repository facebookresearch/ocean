// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/network/tcpclient/android/DemoNetworkTCPClient.h"
#include "application/ocean/demo/network/tcpclient/android/ClientThread.h"

#include "ocean/platform/android/Utilities.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_demo_network_tcpclient_DemoNetworkTCPClientActivity_invokeConnectClient(JNIEnv* env, jobject javaThis, jstring hostAddress, jint hostPort)
{
	const std::string stringHostAddress = Platform::Android::Utilities::toAString(env, hostAddress);

	return ClientThread::get().connect(stringHostAddress, hostPort);
}
