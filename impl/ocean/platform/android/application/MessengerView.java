/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.platform.android.application;

import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.content.Context;
import android.widget.ScrollView;
import android.widget.TextView;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.*;

/**
 * This class implements a simple scroll view synchronously receiving text lines from the messaging system of Ocean.
 * @ingroup platformandroid
 */
public class MessengerView extends ScrollView
{
	/**
	 * Creates a new message view object
	 * @param context The context of new view object
	 * @param lastMessageOnly True, to display the most recent message only; False, to display all messages
	 */
	public MessengerView(Context context, boolean lastMessageOnly)
	{
		super(context);

		lastMessageOnly_ = lastMessageOnly;

		textView_ = new TextView(context);
		addView(textView_);

		timer_ = new Timer();
		timer_.schedule(new TimerTask()
		{
			public void run()
			{
				onTimer();
			}
		}, 0, 50);
	}

	/**
	 * Sets whether the message view shows the most recent message only or whether all messages will be displayed.
	 * @param value True, to display the most recent message only; False, to display all messages
	 */
	public void setLastMessageOnly(boolean value)
	{
		lastMessageOnly_ = value;
	}

	/**
	 * Sets the text color and the optional shadow color of the internal TextView.
	 * @param textColor The color of the text to be used
	 * @param shadowRadius The radius of the text shadow, in pixels, 0 to disable text shadow, with range [0, infinity)
	 * @param shadowColor The color of the text shadow to be used
	 * @see android.widget.TextView.setTextColor(), android.widget.TextView.setShadowLayer().
	 */
	public void setTextColor(int textColor, float shadowRadius, int shadowColor)
	{
		textView_.setTextColor(textColor);
		textView_.setShadowLayer(shadowRadius, 0.0f, 0.0f, shadowColor);
	}

	/**
	 * The timer event function.
	 */
	private void onTimer()
	{
		Activity activity = (Activity)getContext();

		if (activity != null)
		{
			// we need to ensure that the UI is accessed in the main thread
			activity.runOnUiThread(timerAction);
		}
	}

	/// Runnable object for the main thread.
	private Runnable timerAction = new Runnable()
	{
		public void run()
		{
			int remainingMessages = 50;

			while (remainingMessages-- > 0)
			{
				String messages = BaseJni.popMessages();

				if (messages.isEmpty())
				{
					break;
				}

				if (lastMessageOnly_)
				{
					textView_.setText(messages);
				}
				else
				{
					textView_.append(messages);
					textView_.append("\n");
				}
			}
		}
	};

	/// The timer necessary for the update of the text view.
	private Timer timer_;

	/// The text view showing the message.
	private TextView textView_;

	/// True, to display the most recent message only; False, to display all messages.
	private boolean lastMessageOnly_;
}
