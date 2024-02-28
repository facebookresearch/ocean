// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/synthesis/ImageCompletion.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterErosion.h"
#include "ocean/cv/FrameInterpolatorBicubic.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

ImageCompletion::ImageCompletion(Worker* worker) :
	worker_(worker),
	workerOwner_(false)
{
	workerOwner_ = worker == nullptr;

	if (workerOwner_)
	{
		worker_ = new Worker();
		ocean_assert(worker_ != nullptr);
	}
}

ImageCompletion::~ImageCompletion()
{
	if (workerOwner_)
	{
		delete worker_;
	}
}

bool ImageCompletion::completion(const Frame& color, const Frame& mask, const Box2 boundingBox, Frame& result)
{
	Frame gray;
	if (!FrameConverter::Comfort::convert(color, FrameType(color, FrameType::FORMAT_Y8), gray, false, worker_))
	{
		return false;
	}

	return completion(color, gray, mask, boundingBox, result);
}

bool ImageCompletion::completion(const Frame& color, const Frame& gray, const Frame& mask, const Box2 boundingBox, Frame& result)
{
	const Frame& gray0 = gray;
	const Frame& mask0 = mask;
	Frame result0(gray0.frameType());

	ImageCompletionLayer completionLayer0;

	const unsigned int left = (unsigned int)(max(0, int(boundingBox.lower().x() - 1)));
	const unsigned int right = (unsigned int)(min(int(color.width()), int(boundingBox.higher().x() + 1)));
	const unsigned int top = (unsigned int)(max(0, int(boundingBox.lower().y() - 1)));
	const unsigned int bottom = (unsigned int)(min(int(color.height()), int(boundingBox.higher().y() + 1)));

	const unsigned int left0 = left;
	const unsigned int right0 = min(right, color.width() - 1);
	const unsigned int top0 = top;
	const unsigned int bottom0 = min(bottom, color.height() - 1);

	if (!previousLayer_)
	{
		// make pyramid frames with size: 1/2
		FrameType type1(mask0.width() / 2u, mask0.height() / 2u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT);
		Frame gray1(type1);
		Frame mask1(type1);
		Frame result1(type1);
		FrameInterpolatorBicubic::resize8BitPerChannel<1u>(gray0.constdata<uint8_t>(), gray1.data<uint8_t>(), gray0.width(), gray0.height(), gray1.width(), gray1.height(), gray0.paddingElements(), gray1.paddingElements(), worker_);
		FrameInterpolatorNearestPixel::Comfort::resize(mask0, mask1, worker_);

		// make pyramid frames with size: 1/4
		FrameType type2(mask0.width() / 4u, mask0.height() / 4u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT);
		Frame gray2(type2);
		Frame mask2(type2);
		Frame result2(type2);
		FrameInterpolatorBicubic::resize8BitPerChannel<1u>(gray1.constdata<uint8_t>(), gray2.data<uint8_t>(), gray1.width(), gray1.height(), gray2.width(), gray2.height(), gray0.paddingElements(), gray2.paddingElements(), worker_);
		FrameInterpolatorNearestPixel::Comfort::resize(mask1, mask2, worker_);

		// make pyramid frames with size: 1/16
		FrameType type4(mask0.width() / 16u, mask0.height() / 16u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT);
		Frame gray4(type4);
		Frame mask4(type4);
		Frame result4(type4);
		FrameInterpolatorBicubic::resize8BitPerChannel<1u>(gray2.data<uint8_t>(), gray4.data<uint8_t>(), gray2.width(), gray2.height(), gray4.width(), gray4.height(), gray2.paddingElements(), gray4.paddingElements(), worker_);
		FrameInterpolatorNearestPixel::Comfort::resize(mask2, mask4, worker_);


		// erase hole with erosion filter in the smallest frame
		gray4.makeContinuous();
		Frame mask4copy(mask4, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
		FrameFilterErosion::shrinkMask8BitPerChannel4Neighbor<1u, false>(gray4.data<uint8_t>(), mask4copy.data<uint8_t>(), gray4.width(), gray4.height(), gray4.paddingElements(), mask4copy.paddingElements());

		const unsigned int left4 = left / 16u;
		const unsigned int right4 = right / 16u;
		const unsigned int top4 = top / 16u;
		const unsigned int bottom4 = bottom / 16u;

		// iterations on level 4
		ImageCompletionLayer completionLayer4(gray4.width(), gray4.height());
		//completion4.initializeRandomY8(gray4.data<uint8_t>(), mask4.data<uint8_t>(), 7);
		completionLayer4.initializeNull(mask4.data<uint8_t>(), 7);

		for (unsigned int n = 0; n < 3; ++n)
		{
			ocean_assert(gray4.isContinuous() && mask4.isContinuous());

			completionLayer4.improveRandom8Bit(gray4.constdata<uint8_t>(), mask4.data<uint8_t>(), 7, 3, *worker_, left4, right4 - left4 + 1, top4, bottom4 - top4 + 1);

			result4.copy(gray4);
			completionLayer4.coherenceImage8Bit(gray4.constdata<uint8_t>(), mask4.data<uint8_t>(), result4.data<uint8_t>(), 7);

			gray4.copy(result4);
		}



		// propagation from level 4 level to level 2

		const unsigned int left2 = left / 4;
		const unsigned int right2 = right / 4;
		const unsigned int top2 = top / 4;
		const unsigned int bottom2 = bottom / 4;

		ImageCompletionLayer completionLayer2(gray2.width(), gray2.height(), mask2.data<uint8_t>(), 13, completionLayer4);
		completionLayer2.coherenceImage8Bit(gray2.data<uint8_t>(), mask2.data<uint8_t>(), result2.data<uint8_t>(), 13);

		gray2.copy(result2);

		for (unsigned int n = 0; n < 3; ++n)
		{
			ocean_assert(gray2.isContinuous() && mask2.isContinuous());

			completionLayer2.improveRandom8Bit(gray2.data<uint8_t>(), mask2.data<uint8_t>(), 13, 3, *worker_, left2 , right2 - left2 + 1, top2, bottom2 - top2 + 1);
			result2.copy(gray2);

			completionLayer2.coherenceImage8Bit(gray2.data<uint8_t>(), mask2.data<uint8_t>(), result2.data<uint8_t>(), 13);
			gray2.copy(result2);
		}

		completionLayer0 = ImageCompletionLayer(gray0.width(), gray0.height(), mask0.constdata<uint8_t>(), 25, completionLayer2);
		completionLayer0.coherenceImage8Bit(gray0.constdata<uint8_t>(), mask0.constdata<uint8_t>(), result0.data<uint8_t>(), 25);
	}
	else
	{
		ocean_assert(gray0.isContinuous() && mask0.isContinuous());

		completionLayer0.clearAndAdopt(gray0.width(), gray0.height(), mask0.constdata<uint8_t>(), 27, previousLayer_, *worker_, left0, right0 - left0 + 1, top0, bottom0 - top0 + 1);

		result.set(color, true, true);
		completionLayer0.coherenceImage24Bit(color.constdata<uint8_t>(), mask0.constdata<uint8_t>(), result.data<uint8_t>(), 27, 3);

		if (!FrameConverter::Comfort::convert(result, result0, result0, true, worker_))
		{
			return false;
		}
	}

	for (unsigned int n = 0; n < 1; ++n)
	{
		ocean_assert(result0.isContinuous() && mask0.isContinuous());

		completionLayer0.improveRandom8Bit(result0.data<uint8_t>(), mask0.constdata<uint8_t>(), 25, 1, *worker_, left0, right0 - left0 + 1, top0, bottom0 - top0 + 1);
	}

	previousLayer_ = completionLayer0;
	return true;
}

}

}

}
