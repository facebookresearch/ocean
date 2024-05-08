/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_EXPLICIT_RECORDER_H
#define META_OCEAN_MEDIA_EXPLICIT_RECORDER_H

#include "ocean/media/Media.h"
#include "ocean/media/Recorder.h"

#include "ocean/base/SmartObjectRef.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class ExplicitRecorder;

/**
 * Definition of a object reference holding an explicit recorder.
 * @see SmartObjectRef, ExplicitRecorder.
 * @ingroup media
 */
typedef Ocean::SmartObjectRef<ExplicitRecorder, Recorder> ExplicitRecorderRef;

/**
 * This class is the base class for all explicit media recorder.
 * An explicit recorder has to be started explicitly to start the recording process.<br>
 * Instead, an implicit recorder may be started indirect by an external request.<br>
 * @see ImplicitRecorder.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT ExplicitRecorder : virtual public Recorder
{
	public:

		/**
		 * Sets the recorder.
		 * @return True, the recorder started successfully
		 * @see stop().
		 */
		virtual bool start() = 0;

		/**
		 * Stops the recorder.
		 * @return True, the recorder stopped successfully
		 * @see start().
		 */
		virtual bool stop() = 0;

	protected:

		/**
		 * Creates a new explicit recorder.
		 */
		ExplicitRecorder();

		/**
		 * Destructs a explicit recorder.
		 */
		~ExplicitRecorder() override;
};

}

}

#endif // META_OCEAN_MEDIA_EXPLICIT_RECORDER_H
