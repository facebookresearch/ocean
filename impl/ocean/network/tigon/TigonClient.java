// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.network.tigon;

import okhttp3.Interceptor;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

import com.facebook.netlite.certificatepinning.okhttp.FbCertificatePinnerFactory;

import com.facebook.tigon.iface.TigonServiceHolder;
import com.facebook.tigon.oktigon.OkTigonService;
import com.facebook.tigon.oktigon.OkTigonServiceHolder;

import android.os.Build;

import java.io.IOException;
import java.util.concurrent.Executor;

/**
 * This class implements a Tigon client which allows to make HTTP and GraphQL queries on the native C++ side.
 * For now this class actually implements only a TigonService which is accessible on the native C++ side.
 * @ingroup networktigon
 */
public class TigonClient
{
	/**
	 * Creates a new Tigon client object.
	 * @param accessToken The access token of the login session
	 */
	public TigonClient(String accessToken)
	{
		accessToken_ = accessToken;

		String userAgentString = "Ocean Agent";

		OkHttpClient.Builder httpClientBuilder = new OkHttpClient.Builder();

		httpClientBuilder.addInterceptor(new Interceptor() {
			@Override
			public Response intercept(Interceptor.Chain chain) throws IOException
			{
				Request request = chain.request();

				String accessToken = TigonClient.this.accessToken_;

				if (accessToken.isEmpty())
				{
					request = request.newBuilder().build();
				}
				else
				{
					request = request.newBuilder().addHeader("Authorization", "Bearer " + accessToken).build();
				}

				return chain.proceed(request);
			}
		});

		httpClientBuilder.certificatePinner(FbCertificatePinnerFactory.create(Build.TIME)); // see more details https://fburl.com/wiki/0qwq5jmq

		OkTigonService tigonService = new OkTigonService(httpClientBuilder.build(), null, userAgentString);
		tigonServiceHolder_ = new OkTigonServiceHolder(tigonService);

		executor_ = new Executor() {
			@Override
			public void execute(Runnable runnable)
			{
				runnable.run();
			}
		};

		TigonClientJni.setTigonService(tigonServiceHolder_, executor_);
	}

	/**
	 * Releases the Tigon client.
	 */
	public void release()
	{
		TigonClientJni.releaseTigonService();
	}

	/// The holder of the Tigon service so that the Tigon service can be accessed via JNI.
	TigonServiceHolder tigonServiceHolder_;

	/// The executor to be used to execute the queries.
	Executor executor_;

	/// The access token of the login session.
	String accessToken_;
}
