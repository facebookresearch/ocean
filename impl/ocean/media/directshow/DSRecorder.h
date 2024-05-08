/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_RECORDER_H
#define META_OCEAN_MEDIA_DS_RECORDER_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSGraphObject.h"

#include "ocean/media/Recorder.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class is the base class for all DirectShow recorders.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSRecorder :
	virtual public DSGraphObject,
	virtual public Recorder
{
	protected:

		/**
		 * Creates a new DirectShow recorder.
		 */
		DSRecorder();

		/**
		 * Destructs a DirectShow recorder.
		 */
		~DSRecorder() override;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_RECORDER_H
