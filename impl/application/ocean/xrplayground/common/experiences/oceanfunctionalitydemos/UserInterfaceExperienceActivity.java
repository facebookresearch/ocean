// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.xrplayground.android;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

public class UserInterfaceExperienceActivity extends Activity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		Log.i("Ocean", "UserInterfaceExperienceActivity::onCreate()");

		super.onCreate(savedInstanceState);

		LinearLayout layout = new LinearLayout(this);
		layout.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));
		layout.setOrientation(LinearLayout.VERTICAL);
		layout.setGravity(Gravity.CENTER);

		LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
		layoutParams.setMargins(0, 50, 0, 50);

		TextView textView = new TextView(this);
		textView.setLayoutParams(layoutParams);
		textView.setText("Please make a selection");
		textView.setTextSize(25);
		layout.addView(textView);

		buttonA_ = new Button(this);
		buttonA_.setLayoutParams(layoutParams);
		buttonA_.setText("Selection A");
		layout.addView(buttonA_);

		buttonB_ = new Button(this);
		buttonB_.setLayoutParams(layoutParams);
		buttonB_.setText("Selection B");
		layout.addView(buttonB_);

		OnClickListener onClickListener = new OnClickListener() {
			@Override
			public void onClick(View view)
			{
				if (view == buttonA_)
				{
					onSelection("A");
				}
				else
				{
					onSelection("B");
				}

				finish();
			}
		};

	 	buttonA_.setOnClickListener(onClickListener);
		buttonB_.setOnClickListener(onClickListener);

		setContentView(layout);
	}

	@Override
	public void onResume()
	{
		Log.d("Ocean", "UserInterfaceExperienceActivity::onResume()");

		super.onResume();
	}

	@Override
	protected void onDestroy()
	{
		Log.d("Ocean", "UserInterfaceExperienceActivity::onDestroy()");

		super.onDestroy();
	}

	/**
	 * Native event function when a selection has been made.
	 * @param selection The selection which has been made
	 */
	protected static native void onSelection(String selection);

	/// The button with selection A.
	protected Button buttonA_;

	/// The button with selection B.
	protected Button buttonB_;
};
