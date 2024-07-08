/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/rmv/RMVFeatureDetector.h"

#include "ocean/cv/detector/FASTFeatureDetector.h"
#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/media/Utilities.h"

#include <algorithm>

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

Vectors2 RMVFeatureDetector::detectFeatures(const Frame& frame, const DetectorType detectorType, const Scalar threshold, const bool frameIsUndistorted, Worker* worker)
{
	ocean_assert(frame.isValid() && threshold >= 0);

	switch (detectorType)
	{
		case DT_FAST_FEATURE:
		{
			CV::Detector::FASTFeatures fastFeatures;
			CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(frame, (unsigned int)(threshold), frameIsUndistorted, true, fastFeatures, worker);
			std::sort(fastFeatures.begin(), fastFeatures.end());

			return CV::Detector::FASTFeature::features2imagePoints(fastFeatures);
		}

		case DT_HARRIS_FEATURE:
		{
			CV::Detector::HarrisCorners harrisCorners;
			CV::Detector::HarrisCornerDetector::detectCorners(frame, (unsigned int)(threshold), frameIsUndistorted, harrisCorners, true, worker);
			std::sort(harrisCorners.begin(), harrisCorners.end());

			return CV::Detector::HarrisCorner::corners2imagePoints(harrisCorners);
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid detector type!");
	return Vectors2();
}

Vectors2 RMVFeatureDetector::detectFeatures(const Frame& frame, const Box2& boundingBox, const DetectorType detectorType, const Scalar threshold, const bool frameIsUndistorted, Worker* worker)
{
	ocean_assert(frame && threshold >= 0);

	if (!boundingBox.isValid())
	{
		return detectFeatures(frame, detectorType, threshold, frameIsUndistorted, worker);
	}

	const unsigned int left = (unsigned int)(max(0, int(boundingBox.lower().x())));
	const unsigned int right = (unsigned int)(min(int(boundingBox.higher().x()), int(frame.width() - 1)));

	const unsigned int top = (unsigned int)(max(0, int(boundingBox.lower().y())));
	const unsigned int bottom = (unsigned int)(min(int(boundingBox.higher().y()), int(frame.height() - 1)));

	ocean_assert(left <= right && right < frame.width());
	ocean_assert(top <= bottom && bottom < frame.height());

	if (right <= left || bottom <= top)
	{
		return Vectors2();
	}

	switch (detectorType)
	{
		case DT_FAST_FEATURE:
		{
			CV::Detector::FASTFeatures fastFeatures;
			CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(frame, left, top, right - left, bottom - top, (unsigned int)threshold, frameIsUndistorted, true, fastFeatures, worker);
			std::sort(fastFeatures.begin(), fastFeatures.end());

			return CV::Detector::FASTFeature::features2imagePoints(fastFeatures);
		}

		case DT_HARRIS_FEATURE:
		{
			CV::Detector::HarrisCorners harrisCorners;
			CV::Detector::HarrisCornerDetector::detectCorners(frame, left, top, right - left, bottom - top, (unsigned int)threshold, frameIsUndistorted, harrisCorners, true, worker);
			std::sort(harrisCorners.begin(), harrisCorners.end());

			return CV::Detector::HarrisCorner::corners2imagePoints(harrisCorners);
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid detector type!");
	return Vectors2();
}

Vectors2 RMVFeatureDetector::detectFeatures(const Frame& frame, const DetectorType detectorType, const Scalar approximatedThreshold, const size_t numberFeatures, const bool frameIsUndistorted, Worker* worker)
{
	ocean_assert(frame && approximatedThreshold >= 0 && numberFeatures > 0);

	switch (detectorType)
	{
		case DT_FAST_FEATURE:
		{
			CV::Detector::FASTFeatures fastFeatures;
			CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(frame, (unsigned int)(approximatedThreshold + Scalar(0.5)), frameIsUndistorted, true, fastFeatures, worker);

			if (fastFeatures.size() > numberFeatures * 110 / 100)
			{
				for (unsigned int threshold = (unsigned int)(approximatedThreshold + Scalar(0.5)); threshold < 255; threshold += 5)
				{
					CV::Detector::FASTFeatures newFastFeatures;
					CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(frame, threshold, frameIsUndistorted, true, newFastFeatures, worker);

					if (newFastFeatures.size() < numberFeatures * 110 / 100)
						break;

					fastFeatures = std::move(newFastFeatures);
				}
			}
			else if (fastFeatures.size() < numberFeatures)
			{
				for (int threshold = int(approximatedThreshold); threshold >= 5; threshold -= 5)
				{
					fastFeatures.clear();
					CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(frame, (unsigned int)(threshold), frameIsUndistorted, true, fastFeatures, worker);

					if (fastFeatures.size() >= numberFeatures)
					{
						break;
					}
				}
			}

			std::sort(fastFeatures.begin(), fastFeatures.end());
			if (fastFeatures.size() > numberFeatures)
			{
				fastFeatures.resize(numberFeatures);
			}

			return CV::Detector::FASTFeature::features2imagePoints(fastFeatures);
		}

		case DT_HARRIS_FEATURE:
		{
			CV::Detector::HarrisCorners harrisCorners;
			CV::Detector::HarrisCornerDetector::detectCorners(frame, (unsigned int)(approximatedThreshold + Scalar(0.5)), frameIsUndistorted, harrisCorners, true, worker);

			if (harrisCorners.size() < numberFeatures)
			{
				for (int threshold = int(approximatedThreshold); threshold >= 5; threshold -= 5)
				{
					harrisCorners.clear();
					CV::Detector::HarrisCornerDetector::detectCorners(frame, (unsigned int)(threshold), frameIsUndistorted, harrisCorners, true, worker);

					if (harrisCorners.size() >= numberFeatures)
					{
						break;
					}
				}
			}

			std::sort(harrisCorners.begin(), harrisCorners.end());
			if (harrisCorners.size() > numberFeatures)
			{
				harrisCorners.resize(numberFeatures);
			}

			return CV::Detector::HarrisCorner::corners2imagePoints(harrisCorners);
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid detector type!");
	return Vectors2();
}

Vectors2 RMVFeatureDetector::detectFeatures(const Frame& frame, const Box2& boundingBox, const DetectorType detectorType, const Scalar approximatedThreshold, const size_t numberFeatures, const bool frameIsUndistorted, Worker* worker)
{
	ocean_assert(frame && approximatedThreshold >= 0 && numberFeatures > 0);

	if (!boundingBox.isValid())
	{
		return detectFeatures(frame, detectorType, approximatedThreshold, numberFeatures, frameIsUndistorted, worker);
	}

	const unsigned int left = (unsigned int)(max(0, int(boundingBox.lower().x())));
	const unsigned int right = (unsigned int)(min(int(boundingBox.higher().x()), int(frame.width() - 1)));

	const unsigned int top = (unsigned int)(max(0, int(boundingBox.lower().y())));
	const unsigned int bottom = (unsigned int)(min(int(boundingBox.higher().y()), int(frame.height() - 1)));

	ocean_assert(left <= right && right < frame.width());
	ocean_assert(top <= bottom && bottom < frame.height());

	if (right <= left || bottom <= top)
		return Vectors2();

	switch (detectorType)
	{
		case DT_FAST_FEATURE:
		{
			CV::Detector::FASTFeatures fastFeatures;
			CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(frame, left, top, right - left, bottom - top, (unsigned int)approximatedThreshold, frameIsUndistorted, true, fastFeatures, worker);

			if (fastFeatures.size() > numberFeatures * 110 / 100)
			{
				for (unsigned int threshold = (unsigned int)(approximatedThreshold + Scalar(0.5)); threshold < 255; threshold += 5)
				{
					CV::Detector::FASTFeatures newFastFeatures;
					CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(frame, left, top, right - left, bottom - top, threshold, frameIsUndistorted, true, newFastFeatures, worker);

					if (newFastFeatures.size() <= numberFeatures * 110 / 100)
					{
						if (newFastFeatures.size() > numberFeatures)
							fastFeatures = newFastFeatures;

						break;
					}
				}
			}
			else if (fastFeatures.size() < numberFeatures)
			{
				for (int threshold = int(approximatedThreshold); threshold >= 5; threshold -= 5)
				{
					fastFeatures.clear();
					CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(frame, left, top, right - left, bottom - top, (unsigned int)(threshold), frameIsUndistorted, true, fastFeatures, worker);

					if (fastFeatures.size() >= numberFeatures)
					{
						break;
					}
				}
			}

			std::sort(fastFeatures.begin(), fastFeatures.end());
			if (fastFeatures.size() > numberFeatures)
			{
				fastFeatures.resize(numberFeatures);
			}

			return CV::Detector::FASTFeature::features2imagePoints(fastFeatures);
		}

		case DT_HARRIS_FEATURE:
		{
			CV::Detector::HarrisCorners harrisCorners;
			CV::Detector::HarrisCornerDetector::detectCorners(frame, left, top, right - left, bottom - top, (unsigned int)approximatedThreshold, frameIsUndistorted, harrisCorners, true, worker);

			if (harrisCorners.size() < numberFeatures)
			{
				for (int threshold = int(approximatedThreshold); threshold >= 5; threshold -= 5)
				{
					harrisCorners.clear();
					CV::Detector::HarrisCornerDetector::detectCorners(frame, left, top, right - left, bottom - top, (unsigned int)threshold, frameIsUndistorted, harrisCorners, true, worker);

					if (harrisCorners.size() >= numberFeatures)
					{
						break;
					}
				}
			}

			std::sort(harrisCorners.begin(), harrisCorners.end());
			if (harrisCorners.size() > numberFeatures)
			{
				harrisCorners.resize(numberFeatures);
			}

			return CV::Detector::HarrisCorner::corners2imagePoints(harrisCorners);
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid detector type!");
	return Vectors2();
}

}

}

}
