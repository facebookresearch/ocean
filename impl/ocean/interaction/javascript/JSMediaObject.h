/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_MEDIA_OBJECT_H
#define META_OCEAN_INTERACTION_JS_MEDIA_OBJECT_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript MediaObject object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSMediaObject : public JSObject<JSMediaObject, Media::MediumRef>
{
	friend class JSObject<JSMediaObject, Media::MediumRef>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			/// The accessor for the url property, a string value.
			AI_URL
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			FI_DURATION,
			FI_LOOP,
			FI_FRAME_HEIGHT,
			FI_FRAME_WIDTH,
			FI_FRAME_PIXEL_FORMAT,
			FI_FRAME_PIXEL_ORIGIN,
			FI_FRAME_FREQUENCY,
			FI_FRAME_TIMESTAMP,
			FI_HAS_FRAME,
			FI_PREFERRED_FRAME_HEIGHT,
			FI_PREFERRED_FRAME_WIDTH,
			FI_PREFERRED_FRAME_FREQUENCY,
			FI_PREFERRED_FRAME_PIXEL_FORMAT,
			FI_NORMAL_DURATION,
			FI_POSITION,
			FI_SPEED,

			FI_HAS_SOUND,
			FI_SOUND_CHANNELS,
			FI_SOUND_FREQUENCY,
			FI_SOUND_BITS_PER_SAMPLE,
			FI_SOUND_VOLUME,
			FI_SOUND_MUTE,
			FI_PREFERRED_SOUND_CHANNELS,
			FI_PREFERRED_SOUND_FREQUENCY,
			FI_PREFERRED_SOUND_BITS_PER_SAMPLE,

			FI_SET_LOOP,
			FI_SET_POSITION,
			FI_SET_SPEED,
			FI_SET_PREFERRED_FRAME_DIMENSION,
			FI_SET_PREFERRED_FRAME_FREQUENCY,
			FI_SET_PREFERRED_FRAME_PIXEL_FORMAT,
			FI_SET_PREFERRED_SOUND_BITS_PER_SAMPLE,
			FI_SET_PREFERRED_SOUND_CHANNELS,
			FI_SET_PREFERRED_SOUND_FREQUENCY,
			FI_SET_SOUND_VOLUME,
			FI_SET_SOUND_MUTE,

			FI_IS_VALID,
			FI_IS_EXCLUSIVE,
			FI_IS_INVALID,

			FI_START,
			FI_RESTART,
			FI_PAUSE,
			FI_STOP,

			FI_IS_STARTED
		};

	public:

		/**
		 * Returns the JavaScript name of this object.
		 * @return The object's JavaScript name
		 */
		static inline const char* objectName();

	protected:

		/**
		 * Creates the function template and object template for this object.
		 */
		static void createFunctionTemplate();
};

inline const char* JSMediaObject::objectName()
{
	return "MediaObject";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_MEDIA_OBJECT_H
