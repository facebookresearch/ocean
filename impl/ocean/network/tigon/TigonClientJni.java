// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.network.tigon;

import java.util.concurrent.Executor;

import com.facebook.tigon.iface.TigonServiceHolder;

/**
 *
 * @ingroup networktigon
 */
public class TigonClientJni
{
	/**
	 * Java native interface function to set or to update the Tigon service for Tigon requests.
	 * @param env JNI environment
	 * @param javaThis JNI object
	 * @param tigonServiceHolder The holder of the Tigon service, must be valid
	 * @param executor The executor to be used to execute the queries, can be null to use a pure native executor
	 * @return True, if succeeded
	 */
	public static native boolean setTigonService(TigonServiceHolder tigonServiceHolder, Executor executor);

	/**
	 * Java native interface function to release the Tigon service for Tigon requests.
	 * @param env JNI environment
	 * @param javaThis JNI object
	 * @return True, if succeeded
	 */
	public static native boolean releaseTigonService();
}
