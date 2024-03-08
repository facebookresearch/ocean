// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_HISTOGRAM_WXWIDGETS_HISTOGRAM_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_HISTOGRAM_WXWIDGETS_HISTOGRAM_APPLICATION_H

#include "application/ocean/demo/cv/histogram/wxw/Histogram.h"

/**
 * This class implements the application.
 * @ingroup applicationdemocvhistogramwxw
 */
class HistogramApplication : public wxApp
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

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_HISTOGRAM_WXWIDGETS_HISTOGRAM_APPLICATION_H
