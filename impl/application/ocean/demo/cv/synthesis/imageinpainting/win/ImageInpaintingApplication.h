/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_APPLICATION_H

#include "application/ocean/demo/cv/synthesis/imageinpainting/win/ImageInpainting.h"

/**
 * This class implements the application.
 * @ingroup applicationdemocvsynthesisimageinpaintingwxw
 */
class App : public wxApp
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

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_APPLICATION_H
