/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_NETWORK_TCP_CLIENT_ANDROID_DEMO_NETWORK_TCP_CLIENT_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_NETWORK_TCP_CLIENT_ANDROID_DEMO_NETWORK_TCP_CLIENT_H

#include "application/ocean/demo/network/ApplicationDemoNetwork.h"

#include <jni.h>

/**
 * @ingroup applicationdemonetwork
 * @defgroup applicationdemomnetworktcpclientandroid TCP Client (Android)
 * @{
 * The demo application demonstrates the implementation of a TCP client application.<br>
 * This application is platform dependent and is implemented for android platforms.<br>
 * @}
 */

/**
 * Native interface function to connect the TCP client.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param hostAddress Address of the host providing the TCP server
 * @param hostPort Port of the TCP server
 * @ingroup applicationdemomnetworktcpclientandroid
 */
extern "C" jboolean Java_com_meta_ocean_demo_network_tcpclient_DemoNetworkTCPClientActivity_invokeConnectClient(JNIEnv* env, jobject javaThis, jstring hostAddress, jint hostPort);

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_NETWORK_TCP_CLIENT_ANDROID_DEMO_NETWORK_TCP_CLIENT_H
