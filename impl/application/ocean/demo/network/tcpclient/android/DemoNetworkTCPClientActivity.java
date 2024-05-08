/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.demo.network.tcpclient;

import com.meta.ocean.base.BaseJni;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ScrollView;
import android.widget.TextView;
import java.util.Timer;
import java.util.TimerTask;

public class DemoNetworkTCPClientActivity extends Activity
{
	// load the library
	static
	{
		System.loadLibrary("OceanDemoNetworkTCPClient");
	}

	/// The timer necessary for the update of the text view.
	Timer timer;

	/// The text view showing the message.
	TextView textView;

	/**
	 * Native interface function to invoke the demo application.
	 * @param hostAddress The address of the server
	 * @param hostPort The (readable) port of the server
	 * @return True, if succeeded
	 */
	private native boolean invokeConnectClient(String hostAddress, int hostPort);

	/**
	 * Called when the activity is starting.
	 */
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        textView = new TextView(this);

        ScrollView scrollView = new ScrollView(this);
        scrollView.addView(textView);

        setContentView(scrollView);

        timer = new Timer();
        timer.schedule(new TimerTask()
	        {
	        	public void run()
	        	{
	        		timerMethod();
	        	}
	        }, 0, 100);

        textView.append("\n\nonCreate()\n");

        if (invokeConnectClient("chantal", 6003))
        	textView.append("invokeConnectClient() succeeded\n");
        else
        	textView.append("invokeConnectClient() FAILED!\n");
    }

    private void timerMethod()
    {
    	this.runOnUiThread(timerAction);
    }

    private Runnable timerAction = new Runnable()
	    {
	    	public void run()
	    	{
	    		String messages = BaseJni.popMessages();

	    		if (!messages.isEmpty())
	    		{
		    		textView.append(messages);
		    		textView.append("\n");
	    		}
	    	}
	    };

	public void onDestroy()
	{
		textView.append("onDestroy()\n\n");

		super.onDestroy();
	}
}
