/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_ADVANCED_POISSONBLENDING_WXWIDGETS_POISSONBLENDING_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_ADVANCED_POISSONBLENDING_WXWIDGETS_POISSONBLENDING_APPLICATION_H

#include "application/ocean/demo/cv/advanced/poissonblending/wxw/PoissonBlending.h"

/**
 * This class implements the application.
 * @ingroup applicationdemocvPoissonBlendingwxw
 */
class PoissonBlendingApplication : public wxApp
{
	private:

		/**
		 * Initialization event function.
		 * @return True, if succeeded
		 */
		bool OnInit() override;

		/**
		 * Application clean up.
		 */
		void CleanUp() override;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_ADVANCED_POISSONBLENDING_WXWIDGETS_POISSONBLENDING_APPLICATION_H
