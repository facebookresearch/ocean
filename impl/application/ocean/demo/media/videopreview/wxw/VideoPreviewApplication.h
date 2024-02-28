// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEO_PREVIEW_WXW_VIDEO_PREVIEW_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEO_PREVIEW_WXW_VIDEO_PREVIEW_APPLICATION_H

#include "application/ocean/demo/media/videopreview/wxw/VideoPreview.h"

/**
 * This class implements the application.
 * @ingroup applicationdemomediavideopreviewwxwidgets
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

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEO_PREVIEW_WXW_VIDEO_PREVIEW_APPLICATION_H
