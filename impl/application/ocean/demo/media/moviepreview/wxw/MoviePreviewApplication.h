// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_APPLICATION_H

#include "application/ocean/demo/media/moviepreview/wxw/MoviePreview.h"

/**
 * This class implements the application.
 * @ingroup applicationdemomediamoviepreviewwxw
 */
class MoviePreviewApplication : public wxApp
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

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_APPLICATION_H
