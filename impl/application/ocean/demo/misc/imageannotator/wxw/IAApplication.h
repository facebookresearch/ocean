// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IMAGE_ANNONATOR_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IMAGE_ANNONATOR_APPLICATION_H

#include "application/ocean/demo/misc/imageannotator/wxw/ImageAnnotator.h"

/**
 * This class implements the application.
 * @ingroup applicationdemomiscimageannoatoarwxw
 */
class IAApplication : public wxApp
{
	private:

		/**
		 * Initialization event function.
		 * @return True, if succeeded
		 */
		bool OnInit() override;

		/**
		 * Clean up function.
		 */
		void CleanUp() override;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IMAGE_ANNONATOR_APPLICATION_H
