/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_DISCRETE_COSINE_TRANSFORM_WIN_DISCRETE_COSINE_TRANSFORM_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_DISCRETE_COSINE_TRANSFORM_WIN_DISCRETE_COSINE_TRANSFORM_MAIN_WINDOW_H

#include "ocean/base/Frame.h"

#include "ocean/math/DiscreteCosineTransform.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window of the demo application.
 * @ingroup applicationdemomathdiscretecosinetransformwin
 */
class DiscreteCosineTransformMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param mediaFile Media file that will be applied
		 */
		DiscreteCosineTransformMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& mediaFile);

		/**
		 * Destructs the main window.
		 */
		~DiscreteCosineTransformMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

	protected:

		/// A window showing the DCT result.
		BitmapWindow dctWindow_;

		/// Media file that is used to apply the DCT.
		std::string windowMediaFile_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_DISCRETE_COSINE_TRANSFORM_WIN_DISCRETE_COSINE_TRANSFORM_MAIN_WINDOW_H
