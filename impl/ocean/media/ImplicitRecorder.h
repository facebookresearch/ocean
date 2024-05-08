/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMPLICIT_RECORDER_H
#define META_OCEAN_MEDIA_IMPLICIT_RECORDER_H

#include "ocean/media/Media.h"
#include "ocean/media/Recorder.h"

#include "ocean/base/SmartObjectRef.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class ImplicitRecorder;

/**
 * Definition of a object reference holding an explicit recorder.
 * @see SmartObjectRef, ExplicitRecorder.
 * @ingroup media
 */
typedef Ocean::SmartObjectRef<ImplicitRecorder, Recorder> ImplicitRecorderRef;

/**
 * This class is the base class for all implicit media recorder.
 * An implicit recorder may be started indirect by an external request.<br>
 * Instead, an explicit recorder has to be started explicitly to start the recording process.<br>
 * @see ExplicitRecorder.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT ImplicitRecorder : virtual public Recorder
{
	public:

		/**
		 * Returns whether this recorder is currently enabled.
		 * @return True, if so
		 */
		virtual bool isEnabled() const = 0;

		/**
		 * Enables the streaming recorder.
		 * @return True, if succeeded
		 */
		virtual bool enable() = 0;

		/**
		 * Disables the streaming recorder.
		 * @return True, if succeeded
		 */
		virtual bool disable() = 0;

	protected:

		/**
		 * Creates a new implicit recorder.
		 */
		ImplicitRecorder();

		/**
		 * Destructs a implicit recorder.
		 */
		~ImplicitRecorder() override;
};

}

}

#endif // META_OCEAN_MEDIA_IMPLICIT_RECORDER_H
