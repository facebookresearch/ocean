// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.platform.android.application;

import android.app.Activity;
import android.os.Bundle;

/**
 * This class implements a simple Activity holding a single scrollable text view asynchronously receiving text lines from the messaging system of Ocean.
 * The class is intended for e.g., console demo application or test application simply providing text information as output.
 * @ingroup platformandroid
 */
public class TextActivity extends Activity
{
	/// The text view showing the message.
	private MessengerView textView;

	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		textView = new MessengerView(this, false);
		setContentView(textView);
	}
}
