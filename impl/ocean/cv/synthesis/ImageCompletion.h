// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_SYNTHESIS_IMAGE_COMPLECTION_H
#define META_OCEAN_CV_SYNTHESIS_IMAGE_COMPLECTION_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/ImageCompletionLayer.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Box2.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements an image completion algorithm using several pyramid image completion layers.
 * Further the implementation increases speed and image coherence because of the usage of information from previous images.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT ImageCompletion
{
	public:

		/**
		 * Creates a new image completion object.
		 * @param worker Explicit worker object to be used, otherwise an own worker object will be used
		 */
		explicit ImageCompletion(Worker* worker = nullptr);

		/**
		 * Destructs an image completion object.
		 */
		virtual ~ImageCompletion();

		/**
		 * Calculates the image completion result and creates an intermediate grayscale image internally.
		 * @param color The color frame to apply the image completion for
		 * @param mask The mask image used for image completion
		 * @param boundingBox Bounding box which bounds the area to be filled
		 * @param result Resulting image completion image
		 * @return True, if succeeded
		 */
		bool completion(const Frame& color, const Frame& mask, const Box2 boundingBox, Frame& result);

		/**
		 * Calculates the image completion result.
		 * @param color The color frame to apply the image completion for
		 * @param gray The gray scale frame of the color frame
		 * @param mask The mask image used for image completion
		 * @param boundingBox Bounding box which bounds the area to be filled
		 * @param result Resulting image completion image
		 * @return True, if succeeded
		 */
		bool completion(const Frame& color, const Frame& gray, const Frame& mask, const Box2 boundingBox, Frame& result);

	protected:

		/// Finest image completion layer from previous frame.
		ImageCompletionLayer previousLayer_;

		/// Worker object to distribute the computation.
		Worker* worker_;

		/// State determining whether this object is owner of the worker object.
		bool workerOwner_;
};

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_IMAGE_COMPLECTION_H
