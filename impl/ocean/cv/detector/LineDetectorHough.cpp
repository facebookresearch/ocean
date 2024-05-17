/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/LineDetectorHough.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameFilterScharrMagnitude.h"
#include "ocean/cv/FrameFilterSobelMagnitude.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/geometry/NonLinearOptimizationLine.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

bool LineDetectorHough::InfiniteLine::isParallel(const InfiniteLine& line, const Scalar cosAngle) const
{
	const Scalar lineCosAngle(normal_ * line.normal_);
	return Numeric::abs(lineCosAngle) >= cosAngle;
}

bool LineDetectorHough::InfiniteLine::isSimilar(const InfiniteLine& line, const Scalar distance, const Scalar cosAngle, const bool halfOrientationPrecision) const
{
	ocean_assert(distance >= 0);

	ocean_assert(Numeric::isEqual(normal_.length(), 1));
	ocean_assert(Numeric::isEqual(line.normal_.length(), 1));

	const Scalar lineCosAngle(normal_ * line.normal_);

	return (lineCosAngle > 0 && lineCosAngle > cosAngle && Numeric::abs(distance_ - line.distance_) < distance)
					|| (halfOrientationPrecision && lineCosAngle < 0 && -lineCosAngle > cosAngle && Numeric::abs(distance_ + line.distance_) < distance);
}

LineDetectorHough::Accumulator::LookupManager::~LookupManager()
{
	for (AngleLookupMap::iterator i = angleLookupMap_.begin(); i != angleLookupMap_.end(); ++i)
	{
		delete [] i->second;
	}

	for (AngleLookupMap::iterator i = angleLookupMapDiagonal_.begin(); i != angleLookupMapDiagonal_.end(); ++i)
	{
		delete [] i->second;
	}

	for (DirectionLookupMap::iterator i = directionLookupMap_.begin(); i != directionLookupMap_.end(); ++i)
	{
		delete [] i->second;
	}
}

const LineDetectorHough::Accumulator::AngleLookupData* LineDetectorHough::Accumulator::LookupManager::angleLookupData8BitResponse16BitRequest(const unsigned int angleBins, const bool halfOrientationPrecision)
{
	const ScopedLock scopedLock(lock_);

	const AngleLookupMap::const_iterator i = angleLookupMap_.find(MapPair(angleBins, halfOrientationPrecision));
	if (i != angleLookupMap_.end())
	{
		return i->second;
	}

	AngleLookupData* angleLookupData = new AngleLookupData[256 * 256];
	ocean_assert(angleLookupData != nullptr);

	for (int h = -128; h <= 127; ++h)
	{
		for (int v = -128; v <= 127; ++v)
		{
			Scalar angle = (v == 0 && h == 0) ? Scalar(0) : Numeric::atan2(Scalar(v), Scalar(h));
			ocean_assert(angle >= -Numeric::pi() && angle <= Numeric::pi());

			unsigned int aBin;

			if (halfOrientationPrecision)
			{
				if (angle < -Numeric::pi_2())
				{
					angle = Numeric::pi() + angle;
				}
				else if (angle > Numeric::pi_2())
				{
					angle = angle - Numeric::pi();
				}

				ocean_assert(angle >= -Numeric::pi_2() && angle <= Numeric::pi_2());

				if (angle <= -Numeric::pi_2())
				{
					angle = Numeric::pi_2();
				}

				ocean_assert(angle > -Numeric::pi_2() && angle <= Numeric::pi_2());

				aBin = (unsigned int)((angle + Numeric::pi_2()) * Scalar(angleBins - 1u) / Numeric::pi());
				ocean_assert(aBin < angleBins);
			}
			else
			{
				if (angle <= -Numeric::pi())
				{
					angle = Numeric::pi();
				}

				ocean_assert(angle > -Numeric::pi() && angle <= Numeric::pi());

				aBin = (unsigned int)((angle + Numeric::pi()) * Scalar(angleBins - 1u) / Numeric::pi2());
				ocean_assert(aBin < angleBins);
			}

			const unsigned int weight = (unsigned int)(Numeric::sqrt(Scalar(v * v) + Scalar(h * h)));
			const AngleLookupData data(aBin, weight);

			const unsigned int pos = uint8_t(h) | (uint8_t(v) << 8);

			angleLookupData[pos] = data;
		}
	}

	angleLookupMap_.insert(std::make_pair(MapPair(angleBins, halfOrientationPrecision), angleLookupData));

	return angleLookupData;
}

const LineDetectorHough::Accumulator::AngleLookupData* LineDetectorHough::Accumulator::LookupManager::angleLookupDataDiagonal8BitResponse16BitRequest(const unsigned int angleBins, const bool halfOrientationPrecision)
{
	const ScopedLock scopedLock(lock_);

	const AngleLookupMap::const_iterator i = angleLookupMapDiagonal_.find(MapPair(angleBins, halfOrientationPrecision));
	if (i != angleLookupMapDiagonal_.end())
	{
		return i->second;
	}

	AngleLookupData* angleLookupData = new AngleLookupData[256 * 256];
	ocean_assert(angleLookupData != nullptr);

	for (int r45 = -128; r45 <= 127; ++r45)
	{
		for (int r135 = -128; r135 <= 127; ++r135)
		{
			const Vector2 normal = Vector2(Scalar(r45) - Scalar(r135), Scalar(r45) + Scalar(r135));
			Scalar angle = normal.isNull() ? Scalar(0) : Numeric::atan2(normal.y(), normal.x());
			ocean_assert(angle >= -Numeric::pi() && angle <= Numeric::pi());

			unsigned int aBin;

			if (halfOrientationPrecision)
			{
				if (angle < -Numeric::pi_2())
				{
					angle = Numeric::pi() + angle;
				}
				else if (angle > Numeric::pi_2())
				{
					angle = angle - Numeric::pi();
				}

				ocean_assert(angle >= -Numeric::pi_2() && angle <= Numeric::pi_2());

				if (angle <= -Numeric::pi_2())
				{
					angle = Numeric::pi_2();
				}

				ocean_assert(angle > -Numeric::pi_2() && angle <= Numeric::pi_2());

				aBin = (unsigned int)((angle + Numeric::pi_2()) * Scalar(angleBins - 1u) / Numeric::pi());
				ocean_assert(aBin < angleBins);
			}
			else
			{
				if (angle <= -Numeric::pi())
				{
					angle = Numeric::pi();
				}

				ocean_assert(angle > -Numeric::pi() && angle <= Numeric::pi());

				aBin = (unsigned int)((angle + Numeric::pi()) * Scalar(angleBins - 1u) / Numeric::pi2());
				ocean_assert(aBin < angleBins);
			}

			const unsigned int weight = (unsigned int)(normal.length());
			const AngleLookupData data(aBin, weight);

			const unsigned int pos = uint8_t(r45) | (uint8_t(r135) << 8);

			angleLookupData[pos] = data;
		}
	}

	angleLookupMapDiagonal_.insert(std::make_pair(MapPair(angleBins, halfOrientationPrecision), angleLookupData));

	return angleLookupData;
}

const LineDetectorHough::Accumulator::DirectionLookupData* LineDetectorHough::Accumulator::LookupManager::directionLookupData(const unsigned int angleBins, const unsigned int distanceBins, const bool halfOrientationPrecision)
{
	const ScopedLock scopedLock(lock_);

	const DirectionLookupMap::const_iterator i = directionLookupMap_.find(MapTriple(angleBins, distanceBins, halfOrientationPrecision));
	if (i != directionLookupMap_.end())
	{
		return i->second;
	}

	DirectionLookupData* directionLookupData = new DirectionLookupData[angleBins];
	ocean_assert(directionLookupData != nullptr);

	for (unsigned int n = 0u; n < angleBins; ++n)
	{
		const unsigned int invN = angleBins - n - 1u;

		Scalar angle;

		if (halfOrientationPrecision)
		{
			angle =  Numeric::pi_2() - Numeric::pi() * Scalar(invN) / Scalar(angleBins);
			ocean_assert(angle > -Numeric::pi_2() && angle <= Numeric::pi_2());
		}
		else
		{
			angle =  Numeric::pi() - Numeric::pi2() * Scalar(invN) / Scalar(angleBins);
			ocean_assert(angle > -Numeric::pi() && angle <= Numeric::pi());
		}

		Vector2 normal = Vector2(cos(angle), sin(angle));
		ocean_assert(Numeric::isEqual(normal.length(), 1));

		normal *= Scalar(distanceBins);

		const int dX = Numeric::round32(normal.x());
		const int dY = Numeric::round32(normal.y());

		directionLookupData[n] = DirectionLookupData(dX, dY);
	}

	directionLookupMap_.insert(std::make_pair(MapTriple(angleBins, distanceBins, halfOrientationPrecision), directionLookupData));
	return directionLookupData;
}

LineDetectorHough::Accumulator::Accumulator(const unsigned int width, const unsigned int height, const unsigned int distanceBins, const unsigned int angleBins, const unsigned int mirroredAngleBins, const bool halfOrientationPrecision) :
	accumulatorMirroredAngleBins_(mirroredAngleBins),
	accumulatorImageWidth_(width),
	accumulatorImageHeight_(height),
	accumulatorImageWidthHalf_(width / 2u),
	accumulatorImageHeightHalf_(height / 2u),
	accumulatorHalfOrientationPrecision_(halfOrientationPrecision)
{
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(mirroredAngleBins < angleBins / 2u);

	const unsigned int accumulatorWidth = distanceBins + ((distanceBins % 2u) != 1u);
	const unsigned int accumulatorHeightCore = angleBins + (angleBins % 2u);

	ocean_assert(accumulatorWidth % 2 == 1);

	ocean_assert(accumulatorWidth > 0);
	ocean_assert(accumulatorHeightCore > 0);

	const unsigned int accumulatorHeight = accumulatorHeightCore + 2u * accumulatorMirroredAngleBins_;

	// sets the dimension of the accumulator
	if (!accumulatorFrame_.set(FrameType(accumulatorWidth, accumulatorHeight, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	accumulatorFrame_.setValue(0x00);

	accumulatorMaximalDistance_ = Numeric::round32(Numeric::sqrt(Scalar(width * width) + Scalar(height * height)) * Scalar(0.5));

	accumulatorDistanceBinsHalf_ = accumulatorWidth / 2u;
	ocean_assert(accumulatorDistanceBinsHalf_ * 2u + 1u == accumulatorWidth);

	angleLookupTable_ = LookupManager::get().angleLookupData8BitResponse16BitRequest(accumulatorHeightCore, halfOrientationPrecision);
	angleLookupTableDiagonal_ = LookupManager::get().angleLookupDataDiagonal8BitResponse16BitRequest(accumulatorHeightCore, halfOrientationPrecision);

	directionLookupTable_ = LookupManager::get().directionLookupData(accumulatorHeightCore, accumulatorWidth, halfOrientationPrecision);
}

void LineDetectorHough::Accumulator::accumulate(const unsigned int x, const unsigned int y, const int8_t* responses, const unsigned int angleNeigbors)
{
	ocean_assert(angleLookupTable_ && directionLookupTable_);

	ocean_assert(x < accumulatorImageWidth_ && y < accumulatorImageHeight_);
	ocean_assert(angleNeigbors < angleBinsCore());

	const int normalizedX = x - accumulatorImageWidthHalf_;
	const int normalizedY = y - accumulatorImageHeightHalf_;

	ocean_assert(size_t(responses) % 2 == 0); // need 2-byte alignment

	const AngleLookupData& angleLookupData = *(angleLookupTable_ + *(uint16_t*)(responses));

	const unsigned int accumulatorFrameStrideElements = accumulatorFrame_.strideElements();
	uint32_t* const accumulatorFrameData = accumulatorFrame_.data<uint32_t>();

	for (int n = -int(angleNeigbors); n <= int(angleNeigbors); ++n)
	{
		const int a = int(angleLookupData.angleBin_) + n;

		const unsigned int angleBin = (a >= 0) ? (a < int(angleBinsCore()) ? a : a - int(angleBinsCore())) : (int(angleBinsCore()) + a);
		ocean_assert(angleBin < angleBinsCore());

		const DirectionLookupData& directionLookupData = *(directionLookupTable_ + angleBin);

		const int distance = normalizedX * directionLookupData.directionX_ + normalizedY * directionLookupData.directionY_;
		const unsigned int distanceBin = accumulatorDistanceBinsHalf_ + (distance + NumericT<int>::copySign(accumulatorMaximalDistance_, distance)) / (2 * accumulatorMaximalDistance_);
		ocean_assert(distanceBin < distanceBins());

		const unsigned int localWeight = angleNeigbors + 1u - abs(n);
		ocean_assert(localWeight > 0 && localWeight <= angleNeigbors + 1);

		ocean_assert((angleBin + accumulatorMirroredAngleBins_) * accumulatorFrame_.width() + distanceBin < accumulatorFrame_.pixels());
		accumulatorFrameData[(angleBin + accumulatorMirroredAngleBins_) * accumulatorFrameStrideElements + distanceBin] += localWeight * angleLookupData.weight_;
	}
}

void LineDetectorHough::Accumulator::clear()
{
	accumulatorFrame_.setValue(0x00);
}

void LineDetectorHough::Accumulator::accumulateDiagonal(const unsigned int x, const unsigned int y, const int8_t* responses, const unsigned int angleNeigbors)
{
	ocean_assert(angleLookupTableDiagonal_ && directionLookupTable_);

	ocean_assert(x < accumulatorImageWidth_ && y < accumulatorImageHeight_);
	ocean_assert(angleNeigbors < angleBinsCore());

	const int normalizedX = x - accumulatorImageWidthHalf_;
	const int normalizedY = y - accumulatorImageHeightHalf_;

	ocean_assert(size_t(responses) % 2 == 0); // need 2-byte alignment

	const AngleLookupData& angleLookupData = *(angleLookupTableDiagonal_ + *(uint16_t*)(responses));

	const unsigned int accumulatorFrameStrideElements = accumulatorFrame_.strideElements();
	uint32_t* accumulatorFrameData = accumulatorFrame_.data<uint32_t>();

	for (int n = -int(angleNeigbors); n <= int(angleNeigbors); ++n)
	{
		const int a = int(angleLookupData.angleBin_) + n;

		const unsigned int angleBin = a >= 0 ? (a < int(angleBinsCore()) ? a : a - int(angleBinsCore())) : (int(angleBinsCore()) + a);
		ocean_assert(angleBin < angleBinsCore());

		const DirectionLookupData& directionLookupData = *(directionLookupTable_ + angleBin);

		const int distance = normalizedX * directionLookupData.directionX_ + normalizedY * directionLookupData.directionY_;
		const unsigned int distanceBin = accumulatorDistanceBinsHalf_ + (distance + NumericT<int>::copySign(accumulatorMaximalDistance_, distance)) / (2 * accumulatorMaximalDistance_);
		ocean_assert(distanceBin < distanceBins());

		const unsigned int localWeight = angleNeigbors + 1 - abs(n);
		ocean_assert(localWeight > 0 && localWeight <= angleNeigbors + 1);

		accumulatorFrameData[(angleBin + accumulatorMirroredAngleBins_) * accumulatorFrameStrideElements + distanceBin] += localWeight * angleLookupData.weight_;
	}
}

void LineDetectorHough::Accumulator::createMirroredAngleBins()
{
	ocean_assert(accumulatorFrame_.isContinuous());

	if (accumulatorHalfOrientationPrecision_)
	{
		// the left top accumulator pixel 'belongs' to the bottom right accumulator pixel

		uint32_t* accumulatorFrameData = accumulatorFrame_.data<uint32_t>();

		// fill top mirrored angle bins
		for (unsigned int n = 0; n < accumulatorMirroredAngleBins_; ++n)
		{
			uint32_t* target = accumulatorFrameData + n * accumulatorFrame_.width() - 1;
			const uint32_t* source = accumulatorFrameData + (n + angleBinsCore() + 1) * accumulatorFrame_.width();

			const uint32_t* const targetEnd = target + accumulatorFrame_.width();

			while (target != targetEnd)
			{
				*++target = *--source;
			}
		}

		// fill bottom mirrored angle bins
		for (unsigned int n = 0; n < accumulatorMirroredAngleBins_; ++n)
		{
			uint32_t* target = accumulatorFrameData + (n + accumulatorFrame_.height() - accumulatorMirroredAngleBins_) * accumulatorFrame_.width() - 1;
			const uint32_t* source = accumulatorFrameData + (accumulatorMirroredAngleBins_ + n + 1) * accumulatorFrame_.width();

			const uint32_t* const targetEnd = target + accumulatorFrame_.width();

			while (target != targetEnd)
			{
				*++target = *--source;
			}
		}
	}
	else
	{
		// the left top accumulator pixel 'belongs' to the bottom right accumulator pixel

		uint32_t* accumulatorFrameData = accumulatorFrame_.data<uint32_t>();

		// fill top mirrored angle bins
		memcpy(accumulatorFrameData, accumulatorFrameData + (angleBinsCore() + 1u) * accumulatorFrame_.width(), sizeof(uint32_t) * accumulatorFrame_.width() * accumulatorMirroredAngleBins_);
		memcpy(accumulatorFrameData + (accumulatorFrame_.height() - accumulatorMirroredAngleBins_) * accumulatorFrame_.width(), accumulatorFrameData + (accumulatorMirroredAngleBins_ + 1u) * accumulatorFrame_.width(), sizeof(uint32_t) * accumulatorFrame_.width() * accumulatorMirroredAngleBins_);
	}

#ifdef OCEAN_DEBUG
	debugMirroredAngleBinsCreated_ = true;
#endif
}

void LineDetectorHough::Accumulator::detectPeaks(InfiniteLines& lines, const unsigned int voteThreshold, const bool determineExactPeakMaximum, Worker* worker, const bool smoothAccumulator)
{
	ocean_assert(debugMirroredAngleBinsCreated_);

	if (smoothAccumulator)
	{
		CV::FrameFilterGaussian::filter(accumulatorFrame_, 3u, worker);
	}

#if 0
	{
		// the following code can be used to visualize the Hough transformation
		Frame normalizedAccumulatorFrame;
		CV::FrameNormalizer::normalizeToUint8(accumulatorFrame, normalizedAccumulatorFrame);
		Platform::Win::Utilities::desktopFrameOutput(0, 0, normalizedAccumulatorFrame);
	}
#endif

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &LineDetectorHough::Accumulator::detectPeaksSubset, voteThreshold, determineExactPeakMaximum, &lock, &lines, 0u, 0u), 0u, angleBinsCore(), 4u, 5u, 20u);
	}
	else
		detectPeaksSubset(voteThreshold, determineExactPeakMaximum, nullptr, &lines, 0u, angleBinsCore());
}

void LineDetectorHough::Accumulator::detectAdaptivePeaks(InfiniteLines& lines, const Scalar adaptiveVoteThresholdFactor, const unsigned int border, const bool determineExactPeakMaximum, Worker* worker, const bool smoothAccumulator)
{
	ocean_assert(debugMirroredAngleBinsCreated_);

	ocean_assert(border > 1 && border == accumulatorMirroredAngleBins_);

	if (smoothAccumulator)
	{
		CV::FrameFilterGaussian::filter(accumulatorFrame_, 3u, worker);
	}

#if 0
	{
		// the following code can be used to visualize the Hough transformation
		Frame normalizedAccumulatorFrame;
		CV::FrameNormalizer::normalizeToUint8(accumulatorFrame, normalizedAccumulatorFrame);
		Platform::Win::Utilities::desktopFrameOutput(0, 0, normalizedAccumulatorFrame);
	}
#endif

	const unsigned int window = 2u * border + 1u;

	Frame integralAccumulator(FrameType(accumulatorFrame_.width() + window, accumulatorFrame_.height() + window, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	CV::IntegralImage::createBorderedImage<uint32_t, uint32_t, 1u>(accumulatorFrame_.constdata<uint32_t>(), integralAccumulator.data<uint32_t>(), accumulatorFrame_.width(), accumulatorFrame_.height(), border, accumulatorFrame_.paddingElements(), integralAccumulator.paddingElements());

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &LineDetectorHough::Accumulator::detectAdaptivePeaksSubset, integralAccumulator.constdata<uint32_t>(), adaptiveVoteThresholdFactor, border, determineExactPeakMaximum, &lock, &lines, 0u, 0u), 0u, angleBinsCore(), 6u, 7u, 20u);
	}
	else
	{
		detectAdaptivePeaksSubset(integralAccumulator.constdata<uint32_t>(), adaptiveVoteThresholdFactor, border, determineExactPeakMaximum, nullptr, &lines, 0u, angleBinsCore());
	}
}

void LineDetectorHough::Accumulator::joinTwo(Accumulator* accumulators, Worker* worker)
{
	ocean_assert(accumulators);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&LineDetectorHough::Accumulator::joinTwo, accumulators, 0u, 0u), 0u, accumulators[0].angleBinsCore(), 1u, 2u);
	}
	else
	{
		joinTwo(accumulators, 0u, accumulators[0].angleBinsCore());
	}
}

void LineDetectorHough::Accumulator::joinFour(Accumulator* accumulators, Worker* worker)
{
	ocean_assert(accumulators);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&LineDetectorHough::Accumulator::joinFour, accumulators, 0u, 0u), 0u, accumulators[0].angleBinsCore(), 1u, 2u);
	}
	else
	{
		joinFour(accumulators, 0, accumulators[0].angleBinsCore());
	}
}

void LineDetectorHough::Accumulator::detectPeaksSubset(const unsigned int voteThreshold, const bool determineExactPeakMaximum, Lock* lock, InfiniteLines* lines, const unsigned int firstAngleBin, const unsigned int numberAngleBins)
{
	ocean_assert(accumulatorFrame_.isContinuous());

	ocean_assert(lines != nullptr);
	ocean_assert(accumulatorFrame_.width() >= 3u && accumulatorFrame_.height() >= 3u);
	ocean_assert(firstAngleBin + numberAngleBins <= angleBinsCore());

	ocean_assert(angleBinsCore() + 2u <= angleBins());

	ocean_assert(int(firstAngleBin + accumulatorMirroredAngleBins_) - 1 >= 0);

	uint32_t* accumulatorFrameData = accumulatorFrame_.data<uint32_t>();

	const uint32_t* vote0 = accumulatorFrameData + (firstAngleBin + accumulatorMirroredAngleBins_ - 1u) * accumulatorFrame_.width() + 1u;
	const uint32_t* vote1 = vote0 + accumulatorFrame_.width();
	const uint32_t* vote2 = vote1 + accumulatorFrame_.width();

	const uint32_t* const vote1End = vote1 + numberAngleBins * accumulatorFrame_.width();

	unsigned int angle = firstAngleBin;
	unsigned int distance = 1u;

	InfiniteLines localLines;
	localLines.reserve(100);

	while (vote1 != vote1End)
	{
		ocean_assert(vote1 < vote1End);
		ocean_assert(angle < firstAngleBin + numberAngleBins);

		const uint32_t* const vote1EndRow = vote1 + accumulatorFrame_.width() - 2u;
		distance = 1u;

		while (vote1 != vote1EndRow)
		{
			ocean_assert(vote1 < vote1EndRow);
			ocean_assert(vote1 < vote1End);

			if (*vote1 >= voteThreshold
				&& *vote1 > *(vote0 - 1) && *vote1 > *(vote0 + 0) && *vote1 > *(vote0 + 1)
				&& *vote1 > *(vote2 - 1) && *vote1 > *(vote2 + 0) && *vote1 > *(vote2 + 1)
				&& *vote1 > *(vote1 - 1) && *vote1 > *(vote1 + 1))
			{
				Scalar correctedD = Scalar(distance);
				Scalar correctedA = Scalar(angle);

				if (determineExactPeakMaximum)
				{
					/**
					 * x' = x - (d^2f(x) / d^2x)^-1 * df(x) / dx
					 *
					 * df(x) / dx = [ ddistance, dangle ]^T = [d, a]^T
					 *
					 * d^2f(x) / d^2x = [ d^2f(x) / (ddistance * ddistance), d^2f(x) / (ddistance * dangle) ] = [ dd, da ]
					 *                  [ d^2f(x) / (dangle * ddistance),    d^2f(x) / (dangle * dangle)    ]   [ da, aa ]
					 *
					 * [ a b ]^-1   [  d  -b ]
					 * [ c d ]    = [ -c   a ] * 1/det
					 *
					 * det = a * d - b * c
					 */

					// [-1 0 1] * 1/2
					const Scalar d = Scalar(int(*(vote1 + 1)) - int(*(vote1 - 1))) * Scalar(0.5);
					const Scalar a = Scalar(int(*(vote2)) - int(*(vote0))) * Scalar(0.5);

					// [1 -2 1] * 1/1
					const Scalar dd = Scalar(int(*(vote1 + 1) + *(vote1 - 1)) - int(*(vote1) * 2));
					const Scalar aa = Scalar(int(*(vote2) + *(vote0)) - int(*(vote1) * 2));

					// [ 1  0 -1 ]
					// [ 0  0  0 ] * 1/4
					// [-1  0  1 ]

					const Scalar da = Scalar(int(*(vote2 + 1) + *(vote0 - 1)) - int(*(vote2 - 1)) - int(*(vote0 + 1))) * Scalar(0.25);

					// 1/det = 1/(dd * aa - da * da)

					const Scalar denominator = dd * aa - da * da;
					ocean_assert(Numeric::isNotEqualEps(denominator));

					const Scalar factor = 1 / denominator;

					// [ dd da ]^-1   [ d ]       [  aa  -da ]       [ d ]       [  aa * f   -da * f ]   [ d ]
					// [ da aa ]    * [ a ]   =   [ -da   dd ] * f * [ a ]   =   [ -da * f    dd * f ] * [ a ]

					// [ offsetD ]   [ (aa * d - da * a) * f ]
					// [ offsetA ] = [ (dd * a - da * d) * f ]

					const Scalar offsetD = (aa * d - da * a) * factor;
					const Scalar offsetA = (dd * a - da * d) * factor;

					// check for invalid maximum
					if (offsetD >= -2 && offsetD <= 2 && offsetA >= -2 && offsetA <= 2)
					{
						correctedD = Scalar(distance) - offsetD;
						correctedA = Scalar(angle) - offsetA;
					}
				}

				const Scalar lineAngle = accumulatorHalfOrientationPrecision_ ? Numeric::angleAdjustNull((Scalar(correctedA) * Numeric::pi()) / Scalar(angleBinsCore() - 1u) - Numeric::pi_2())
																				: Numeric::angleAdjustNull((Scalar(correctedA) * Numeric::pi2()) / Scalar(angleBinsCore() - 1u) - Numeric::pi());

				const Scalar lineDistance = ((correctedD - Scalar(accumulatorDistanceBinsHalf_)) * Scalar(2) * Scalar(accumulatorMaximalDistance_)) / Scalar(distanceBins());
				ocean_assert(lineDistance <= Scalar(accumulatorMaximalDistance_));

				const Vector2 normal(Numeric::cos(lineAngle), Numeric::sin(lineAngle));
				ocean_assert(Numeric::isEqual(normal.length(), 1));

				localLines.emplace_back(normal, lineAngle, lineDistance, Scalar(*vote1));
			}

			++vote0;
			++vote1;
			++vote2;

			++distance;
		}

		vote0 += 2;
		vote1 += 2;
		vote2 += 2;

		++angle;
	}

	const OptionalScopedLock optionalScopedLock(lock);

	if (lock && lines->empty())
	{
		lines->reserve(localLines.size() * 4);
	}

	lines->insert(lines->end(), localLines.begin(), localLines.end());
}

void LineDetectorHough::Accumulator::detectAdaptivePeaksSubset(const uint32_t* borderedIntegralAccumulator, const Scalar adaptiveVoteThresholdFactor, const unsigned int border, const bool determineExactPeakMaximum, Lock* lock, InfiniteLines* lines, const unsigned int firstAngleBin, const unsigned int numberAngleBins)
{
	ocean_assert(accumulatorFrame_.isContinuous());

	ocean_assert(accumulatorFrame_.width() >= 3u && accumulatorFrame_.height() >= 3u);
	ocean_assert(firstAngleBin + numberAngleBins <= angleBinsCore());
	ocean_assert(lines);

	ocean_assert(angleBinsCore() + 2u <= angleBins());

	ocean_assert(int(firstAngleBin + accumulatorMirroredAngleBins_) - 1 >= 0);

	const uint32_t* const accumulatorFrameData = accumulatorFrame_.constdata<uint32_t>();

	const uint32_t* vote0 = accumulatorFrameData + (firstAngleBin + accumulatorMirroredAngleBins_ - 1u) * accumulatorFrame_.width() + 1u;
	const uint32_t* vote1 = vote0 + accumulatorFrame_.width();
	const uint32_t* vote2 = vote1 + accumulatorFrame_.width();

	const uint32_t* const vote1End = vote1 + numberAngleBins * accumulatorFrame_.width();

	unsigned int angle = firstAngleBin;
	unsigned int distance = 1u;

	const unsigned int window = 2u * border + 1u;
	const unsigned int integralWidth = accumulatorFrame_.width() + window;

	const uint32_t* integralVoteTop = borderedIntegralAccumulator + (border + firstAngleBin) * integralWidth + 1u;
	const uint32_t* integralVoteBottom = integralVoteTop + window * integralWidth;

	ocean_assert(window != 0u);
	const Scalar normalization = adaptiveVoteThresholdFactor / Scalar(window * window);

	InfiniteLines localLines;
	localLines.reserve(100);

	while (vote1 != vote1End)
	{
		ocean_assert(vote1 < vote1End);
		ocean_assert(angle < firstAngleBin + numberAngleBins);

		ocean_assert((vote0 - accumulatorFrameData) % accumulatorFrame_.width() == 1u);
		ocean_assert((vote1 - accumulatorFrameData) % accumulatorFrame_.width() == 1u);
		ocean_assert((vote2 - accumulatorFrameData) % accumulatorFrame_.width() == 1u);

		ocean_assert((integralVoteTop - borderedIntegralAccumulator) % integralWidth == 1u);
		ocean_assert((integralVoteBottom - borderedIntegralAccumulator) % integralWidth == 1u);
		ocean_assert(integralVoteTop + integralWidth * window == integralVoteBottom);

		const uint32_t* const vote1EndRow = vote1 + accumulatorFrame_.width() - 2u;
		distance = 1u;

		while (vote1 != vote1EndRow)
		{
			ocean_assert(vote1 < vote1EndRow);
			ocean_assert(vote1 < vote1End);

			// we determine the sum of all votes inside the surrounding window
			const unsigned int sumWindowVotes = *integralVoteTop - *(integralVoteTop + window) - *integralVoteBottom + *(integralVoteBottom + window);

			if (*vote1 >= 1u && *vote1 >= (unsigned int)(Scalar(sumWindowVotes) * normalization)
				&& *vote1 > *(vote0 - 1) && *vote1 > *(vote0 + 0) && *vote1 > *(vote0 + 1)
				&& *vote1 > *(vote2 - 1) && *vote1 > *(vote2 + 0) && *vote1 > *(vote2 + 1)
				&& *vote1 > *(vote1 - 1) && *vote1 > *(vote1 + 1))
			{
				Scalar correctedD = Scalar(distance);
				Scalar correctedA = Scalar(angle);

				if (determineExactPeakMaximum)
				{
					/**
					 * x' = x - (d^2f(x) / d^2x)^-1 * df(x) / dx
					 *
					 * df(x) / dx = [ ddistance, dangle ]^T = [d, a]^T
					 *
					 * d^2f(x) / d^2x = [ d^2f(x) / (ddistance * ddistance), d^2f(x) / (ddistance * dangle) ] = [ dd, da ]
					 *                  [ d^2f(x) / (dangle * ddistance),    d^2f(x) / (dangle * dangle)    ]   [ da, aa ]
					 *
					 * [ a b ]^-1   [  d  -b ]
					 * [ c d ]    = [ -c   a ] * 1/det
					 *
					 * det = a * d - b * c
					 */

					// [-1 0 1] * 1/2
					const Scalar d = Scalar(int(*(vote1 + 1)) - int(*(vote1 - 1))) * Scalar(0.5);
					const Scalar a = Scalar(int(*(vote2)) - int(*(vote0))) * Scalar(0.5);

					// [1 -2 1] * 1/1
					const Scalar dd = Scalar(int(*(vote1 + 1) + *(vote1 - 1)) - int(*(vote1) * 2));
					const Scalar aa = Scalar(int(*(vote2) + *(vote0)) - int(*(vote1) * 2));

					// [ 1  0 -1 ]
					// [ 0  0  0 ] * 1/4
					// [-1  0  1 ]

					const Scalar da = Scalar(int(*(vote2 + 1) + *(vote0 - 1)) - int(*(vote2 - 1)) - int(*(vote0 + 1))) * Scalar(0.25);

					// 1/det = 1/(dd * aa - da * da)

					const Scalar denominator = dd * aa - da * da;
					if (Numeric::isNotEqualEps(denominator))
					{
						const Scalar factor = Scalar(1) / denominator;

						// [ dd da ]^-1   [ d ]       [  aa  -da ]       [ d ]       [  aa * f   -da * f ]   [ d ]
						// [ da aa ]    * [ a ]   =   [ -da   dd ] * f * [ a ]   =   [ -da * f    dd * f ] * [ a ]

						// [ offsetD ]   [ (aa * d - da * a) * f ]
						// [ offsetA ] = [ (dd * a - da * d) * f ]

						const Scalar offsetD = (aa * d - da * a) * factor;
						const Scalar offsetA = (dd * a - da * d) * factor;

						// check for invalid maximum
						if (offsetD >= -2 && offsetD <= 2 && offsetA >= -2 && offsetA <= 2)
						{
							correctedD = Scalar(distance) - offsetD;
							correctedA = Scalar(angle) - offsetA;
						}
					}
				}

				const Scalar lineAngle = accumulatorHalfOrientationPrecision_ ? Numeric::angleAdjustNull((Scalar(correctedA) * Numeric::pi()) / Scalar(angleBinsCore() - 1u) - Numeric::pi_2())
																				: Numeric::angleAdjustNull((Scalar(correctedA) * Numeric::pi2()) / Scalar(angleBinsCore() - 1u) - Numeric::pi());

				const Scalar lineDistance = ((correctedD - Scalar(accumulatorDistanceBinsHalf_)) * Scalar(2) * Scalar(accumulatorMaximalDistance_)) / Scalar(distanceBins());
				ocean_assert(lineDistance <= Scalar(accumulatorMaximalDistance_));

				const Vector2 normal(Numeric::cos(lineAngle), Numeric::sin(lineAngle));
				ocean_assert(Numeric::isEqual(normal.length(), 1));

				localLines.emplace_back(normal, lineAngle, lineDistance, Scalar(*vote1));
			}

			++vote0;
			++vote1;
			++vote2;

			++integralVoteTop;
			++integralVoteBottom;

			++distance;
		}

		vote0 += 2;
		vote1 += 2;
		vote2 += 2;

		integralVoteTop += 2u + window;
		integralVoteBottom += 2u + window;

		++angle;
	}

	const OptionalScopedLock optionalScopedLock(lock);

	if (lock && lines->empty())
	{
		lines->reserve(localLines.size() * 4);
	}

	lines->insert(lines->end(), localLines.begin(), localLines.end());
}

void LineDetectorHough::Accumulator::join(Accumulator* accumulators, const unsigned int number, Worker* worker)
{
	if (number <= 1u)
	{
		return;
	}

	ocean_assert(accumulators);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&LineDetectorHough::Accumulator::join, accumulators, number, 0u, 0u), 0, accumulators[0].angleBinsCore(), 2u, 3u);
	}
	else
	{
		join(accumulators, number, 0u, accumulators[0].angleBinsCore());
	}
}

void LineDetectorHough::Accumulator::joinTwo(Accumulator* accumulators, const unsigned int firstAngleBin, const unsigned int numberAngleBins)
{
	ocean_assert(accumulators != nullptr);
	ocean_assert(firstAngleBin + numberAngleBins <= accumulators[0].angleBinsCore());

	Frame& frame0(accumulators[0].accumulatorFrame_);
	const Frame& frame1(accumulators[1].accumulatorFrame_);

	ocean_assert(frame0.isContinuous() && frame1.isContinuous());
	ocean_assert(frame0.size() == frame1.size());

	// we only join the accumulator core, the additional mirrored accumulator rows will be calculated afterwards
	uint32_t* a0 = frame0.data<uint32_t>() + (firstAngleBin + accumulators[0].mirroredAngleBins()) * frame0.width() - 1u;
	const uint32_t* a1 = frame1.constdata<uint32_t>() + (firstAngleBin + accumulators[0].mirroredAngleBins()) * frame0.width() - 1u;

	const uint32_t* const a0End = a0 + numberAngleBins * frame0.width();

	while (a0 != a0End)
	{
		*++a0 += *++a1;
	}
}

void LineDetectorHough::Accumulator::joinFour(Accumulator* accumulators, const unsigned int firstAngleBin, const unsigned int numberAngleBins)
{
	ocean_assert(accumulators != nullptr);
	ocean_assert(firstAngleBin + numberAngleBins <= accumulators[0].angleBinsCore());

	Frame& frame0(accumulators[0].accumulatorFrame_);
	const Frame& frame1(accumulators[1].accumulatorFrame_);
	const Frame& frame2(accumulators[2].accumulatorFrame_);
	const Frame& frame3(accumulators[3].accumulatorFrame_);

	ocean_assert(frame0.isContinuous() && frame1.isContinuous());
	ocean_assert(frame2.isContinuous() && frame3.isContinuous());
	ocean_assert(frame0.size() == frame1.size());
	ocean_assert(frame1.size() == frame2.size());
	ocean_assert(frame2.size() == frame3.size());

	// we only join the accumulator core, the additional mirrored accumulator rows will be calculated afterwards
	uint32_t* a0 = frame0.data<uint32_t>() + (firstAngleBin + accumulators[0].mirroredAngleBins()) * frame0.width() - 1u;
	const uint32_t* a1 = frame1.constdata<uint32_t>() + (firstAngleBin + accumulators[0].mirroredAngleBins()) * frame0.width() - 1u;
	const uint32_t* a2 = frame2.constdata<uint32_t>() + (firstAngleBin + accumulators[0].mirroredAngleBins()) * frame0.width() - 1u;
	const uint32_t* a3 = frame3.constdata<uint32_t>() + (firstAngleBin + accumulators[0].mirroredAngleBins()) * frame0.width() - 1u;

	const uint32_t* const a0End = a0 + numberAngleBins * frame0.width();

	while (a0 != a0End)
	{
		*++a0 += *++a1 + *++a2 + *++a3;
	}
}

void LineDetectorHough::Accumulator::join(Accumulator* accumulators, const unsigned int number, const unsigned int firstAngleBin, const unsigned int numberAngleBins)
{
	ocean_assert(accumulators != nullptr);
	ocean_assert(firstAngleBin + numberAngleBins <= accumulators[0].angleBinsCore());
	ocean_assert(number > 1u);

	ocean_assert(accumulators[0].accumulatorFrame_.isContinuous());

	const unsigned int width = accumulators[0].accumulatorFrame_.width();
	const uint32_t** pointers = new const uint32_t*[number - 1u];

	// we only join the accumulator core, the additional mirrored accumulator rows will be calculated afterwards
	for (unsigned int n = 0u; n < number - 1u; ++n)
	{
		ocean_assert(accumulators[n + 1u].accumulatorFrame_.isContinuous());
		pointers[n] = accumulators[n + 1u].accumulatorFrame_.constdata<uint32_t>() + (firstAngleBin + accumulators[0].mirroredAngleBins()) * width - 1u;
	}

	uint32_t* target = accumulators[0].accumulatorFrame_.data<uint32_t>() + (firstAngleBin + accumulators[0].mirroredAngleBins()) * width;
	const uint32_t* const targetEnd = target + numberAngleBins * width;

	while (target != targetEnd)
	{
		for (unsigned int n = 0; n < number - 1u; ++n)
		{
			*target += *++(pointers[n]);
		}

		++target;
	}

	delete [] pointers;
}

bool LineDetectorHough::internalDetectLines(const Frame& frame, const FilterType filterType, const FilterResponse filterResponse, InfiniteLines& infiniteLines, FiniteLines2* finiteLines, const bool optimizeLines, const Scalar thresholdParameter, const unsigned int adaptiveThresholdWindowHalf, const unsigned int voteThreshold, const unsigned int angleNeigbors, const bool determineExactPeakMaximum, Worker* worker, const unsigned int anglePrecision, const unsigned int distancePrecision, const bool halfOrientationPrecision, const Scalar similarDistance, const Scalar similarAngle)
{
	ocean_assert(frame);

	if (!frame.isValid())
	{
		return false;
	}

	ocean_assert(anglePrecision > 0u && anglePrecision < 36000u);
	ocean_assert(distancePrecision != 0u);

	const unsigned int width = frame.width();
	const unsigned int height = frame.height();

	Frame response;
	unsigned int maximalFilterMagnitude = 0u;
	unsigned int voteNumber = 0u;

	// first, we determine the individual edge filter responses, either Sobel responses or Scharr responses
	// we use all data channels of the provided frame but keep one filter response for each pixel only (the filter response with largest magnitude)
	// however, although we use the magnitude we keep the original filter response and not the magnitude

	switch (filterResponse)
	{
		case FR_HORIZONTAL_VERTICAL:
		{
			if (!response.set(FrameType(frame, FrameType::genericPixelFormat<int8_t, 2u>()), true, true))
			{
				return false;
			}

			voteNumber = 1u;

			switch (filterType)
			{
				case FT_SCHARR:
					CV::FrameFilterScharrMagnitude::Comfort::filterHorizontalVerticalAs1Channel(frame, response, worker);
					maximalFilterMagnitude = 128u;
					break;

				case FT_SOBEL:
					CV::FrameFilterSobelMagnitude::Comfort::filterHorizontalVerticalAs1Channel(frame, response, worker);
					maximalFilterMagnitude = 128u;
					break;

				default:
					return false;
			}

			break;
		}

		case FR_DIAGONAL:
		{
			if (!response.set(FrameType(frame, FrameType::genericPixelFormat<int8_t, 2u>()), true, true))
			{
				return false;
			}

			voteNumber = 1u;

			switch (filterType)
			{
				case FT_SCHARR:
					CV::FrameFilterScharrMagnitude::Comfort::filterDiagonalAs1Channel(frame, response, worker);
					maximalFilterMagnitude = 128u;
					break;

				case FT_SOBEL:
					CV::FrameFilterSobelMagnitude::Comfort::filterDiagonalAs1Channel(frame, response, worker);
					maximalFilterMagnitude = 128u;
					break;

				default:
					return false;
			}

			break;
		}

		case FR_HORIZONTAL_VERTICAL_DIAGONAL:
		{
			if (!response.set(FrameType(frame, FrameType::genericPixelFormat<int8_t, 4u>()), true, true))
			{
				return false;
			}

			voteNumber = 2u;

			switch (filterType)
			{
				case FT_SCHARR:
					CV::FrameFilterScharrMagnitude::Comfort::filterAs1Channel(frame, response, worker);
					maximalFilterMagnitude = 128u;
					break;

				case FT_SOBEL:
					CV::FrameFilterSobelMagnitude::Comfort::filterAs1Channel(frame, response, worker);
					maximalFilterMagnitude = 128u;
					break;

				default:
					return false;
			}

			break;
		}

		default:
			return false;
	}

	const unsigned int neighborVotes = min(angleNeigbors, anglePrecision / 2u);

	ocean_assert(maximalFilterMagnitude > 0u && maximalFilterMagnitude < 256u * 256u);
	ocean_assert(voteNumber > 0u);

	const unsigned int diagonalHalf = distancePrecision == (unsigned int)(-1) ? (unsigned int)(Numeric::sqrt(Scalar(width * width + height * height))) / 2u : distancePrecision;

	ocean_assert(adaptiveThresholdWindowHalf == 0u || adaptiveThresholdWindowHalf >= 2u);

	// we need one mirrored angle bin in the non-adaptive threshold mode as we want to use the 3x3 non-suppression area without the need to handle the upper and lower row of the accumulation buffer explicitly
	// for the adaptive threshold mode we need half of the size of the surrounding window (with the same argument)
	const unsigned int mirroredAngleBins = max(1u, adaptiveThresholdWindowHalf);

	if (worker && *worker)
	{
		Worker::Functions functions;
		Accumulator accumulators[2] =
		{
			Accumulator(width, height, diagonalHalf, anglePrecision, mirroredAngleBins, halfOrientationPrecision),
			Accumulator(width, height, diagonalHalf, anglePrecision, mirroredAngleBins, halfOrientationPrecision)
		};

		const unsigned int height_2 = height / 2u;

		switch (filterResponse)
		{
			case FR_HORIZONTAL_VERTICAL:
				functions.emplace_back(Worker::Function::createStatic(&LineDetectorHough::createVotesHorizontalVerticalSubset, response.constdata<int8_t>(), accumulators + 0, neighborVotes, voteThreshold, 0u * height_2, height_2)); // 0 -> height_2
				functions.emplace_back(Worker::Function::createStatic(&LineDetectorHough::createVotesHorizontalVerticalSubset, response.constdata<int8_t>(), accumulators + 1, neighborVotes, voteThreshold, 1u * height_2, height_2)); // height_2 -> height

				break;

			case FR_DIAGONAL:
				functions.emplace_back(Worker::Function::createStatic(&LineDetectorHough::createVotesDiagonalSubset, response.constdata<int8_t>(), accumulators + 0, neighborVotes, voteThreshold, 0u * height_2, height_2));
				functions.emplace_back(Worker::Function::createStatic(&LineDetectorHough::createVotesDiagonalSubset, response.constdata<int8_t>(), accumulators + 1, neighborVotes, voteThreshold, 1u * height_2, height_2));

				break;

			case FR_HORIZONTAL_VERTICAL_DIAGONAL:
				functions.emplace_back(Worker::Function::createStatic(&LineDetectorHough::createVotesHorizontalVerticalDiagonalSubset, response.constdata<int8_t>(), accumulators + 0, neighborVotes, voteThreshold, 0u * height_2, height_2));
				functions.emplace_back(Worker::Function::createStatic(&LineDetectorHough::createVotesHorizontalVerticalDiagonalSubset, response.constdata<int8_t>(), accumulators + 1, neighborVotes, voteThreshold, 1u * height_2, height_2));

				break;

			default:
				ocean_assert(false && "This should never happen!");
		}

		ocean_assert(!functions.empty());
		worker->executeFunctions(functions);
		Accumulator::joinTwo(accumulators, worker);

		accumulators[0].createMirroredAngleBins();

		if (adaptiveThresholdWindowHalf == 0u)
		{
			// number of votes * maximal filter magnitude * center weighting * two votes (0-90, 45-135)
			const unsigned int internalThreshold = (unsigned int)thresholdParameter * (maximalFilterMagnitude / 2u) * (neighborVotes + 1u) * max(1u, (neighborVotes / 10u)) * voteNumber;
			ocean_assert(internalThreshold > 0u);

			accumulators[0].detectPeaks(infiniteLines, internalThreshold, determineExactPeakMaximum, worker);
		}
		else
		{
			accumulators[0].detectAdaptivePeaks(infiniteLines, thresholdParameter, adaptiveThresholdWindowHalf, determineExactPeakMaximum, worker);
		}

		if (optimizeLines)
		{
			InfiniteLines optimizedLines(infiniteLines);
			worker->executeFunction(Worker::Function::createStatic(&LineDetectorHough::optimizeInfiniteLinesSubset, (const InfiniteLine*)infiniteLines.data(), infiniteLines.size(), response.constdata<int8_t>(), response.width(), response.height(), filterResponse, 5u, (const Accumulator*)accumulators, halfOrientationPrecision, optimizedLines.data(), 0u, 0u), 0u, (unsigned int)infiniteLines.size());
			infiniteLines = optimizedLines;
		}

		if (Numeric::isNotEqualEps(similarDistance) && Numeric::isNotEqualEps(similarAngle))
		{
			InfiniteLines filteredInfiniteLines;
			filterLines(infiniteLines, similarDistance, similarAngle, filteredInfiniteLines, halfOrientationPrecision);
			infiniteLines = filteredInfiniteLines;
		}

		if (finiteLines)
		{
			const Accumulator::AngleLookupData* horizontalLookup = Accumulator::LookupManager::get().angleLookupData8BitResponse16BitRequest(accumulators[0].angleBinsCore(), halfOrientationPrecision);
			const Accumulator::AngleLookupData* diagonalLookup = Accumulator::LookupManager::get().angleLookupDataDiagonal8BitResponse16BitRequest(accumulators[0].angleBinsCore(), halfOrientationPrecision);

			Lock lock;
			worker->executeFunction(Worker::Function::createStatic(&detectFiniteLinesSubset, (const InfiniteLines*)&infiniteLines, response.constdata<int8_t>(), response.width(), response.height(), filterResponse, accumulators[0].angleBinsCore(), horizontalLookup, diagonalLookup, halfOrientationPrecision, &lock, finiteLines, 0u, 0u), 0, (unsigned int)(infiniteLines.size()), 11u, 12u, 10u);
		}
	}
	else // no worker object
	{
		Accumulator accumulator(width, height, diagonalHalf, anglePrecision, mirroredAngleBins, halfOrientationPrecision);

		switch (filterResponse)
		{
			case FR_HORIZONTAL_VERTICAL:
				createVotesHorizontalVerticalSubset(response.constdata<int8_t>(), &accumulator, neighborVotes, voteThreshold, 0u, height);
				break;

			case FR_DIAGONAL:
				createVotesDiagonalSubset(response.constdata<int8_t>(), &accumulator, neighborVotes, voteThreshold, 0u, height);
				break;

			case FR_HORIZONTAL_VERTICAL_DIAGONAL:
				createVotesHorizontalVerticalDiagonalSubset(response.constdata<int8_t>(), &accumulator, neighborVotes, voteThreshold, 0u, height);
				break;

			default:
				ocean_assert(false && "This should never happen!");
		}

		accumulator.createMirroredAngleBins();

		if (adaptiveThresholdWindowHalf == 0u)
		{
			// number of votes * maximal filter magnitude * center weighting * (two votes) (0-90, 45-135)
			const unsigned int internalThreshold = (unsigned int)thresholdParameter * (maximalFilterMagnitude / 2u) * (neighborVotes + 1u) * max(1u, (neighborVotes / 10u));// * voteNumber; // **TODO** why uncommented?
			ocean_assert(internalThreshold > 0);

			accumulator.detectPeaks(infiniteLines, internalThreshold, determineExactPeakMaximum, nullptr);
		}
		else
		{
			accumulator.detectAdaptivePeaks(infiniteLines, thresholdParameter, adaptiveThresholdWindowHalf, determineExactPeakMaximum, nullptr);
		}

		if (optimizeLines && !infiniteLines.empty())
		{
			InfiniteLines optimizedLines(infiniteLines);
			optimizeInfiniteLinesSubset(infiniteLines.data(), infiniteLines.size(), response.constdata<int8_t>(), response.width(), response.height(), filterResponse, 5u, &accumulator, halfOrientationPrecision, optimizedLines.data(), 0u, (unsigned int)infiniteLines.size());
			infiniteLines = optimizedLines;
		}

		if (Numeric::isNotEqualEps(similarDistance) && Numeric::isNotEqualEps(similarAngle))
		{
			InfiniteLines filteredInfiniteLines;
			filterLines(infiniteLines, similarDistance, similarAngle, filteredInfiniteLines, halfOrientationPrecision);
			infiniteLines = filteredInfiniteLines;
		}

		if (finiteLines)
		{
			const Accumulator::AngleLookupData* horizontalLookup = Accumulator::LookupManager::get().angleLookupData8BitResponse16BitRequest(accumulator.angleBinsCore(), halfOrientationPrecision);
			const Accumulator::AngleLookupData* diagonalLookup = Accumulator::LookupManager::get().angleLookupDataDiagonal8BitResponse16BitRequest(accumulator.angleBinsCore(), halfOrientationPrecision);
			detectFiniteLinesSubset(&infiniteLines,  response.constdata<int8_t>(), width, height, filterResponse, accumulator.angleBinsCore(), horizontalLookup, diagonalLookup, halfOrientationPrecision, nullptr, finiteLines, 0u, (unsigned int)(infiniteLines.size()));
		}
	}

	return true;
}

void LineDetectorHough::filterLines(const InfiniteLines& lines, const Scalar minDistance, const Scalar minAngle, InfiniteLines& filteredLines, const bool halfOrientationPrecision)
{
	ocean_assert(filteredLines.empty());

	filteredLines.reserve(lines.size());

	const Scalar minCosAngle = Numeric::cos(minAngle);

	for (unsigned int n = 0; n < lines.size(); ++n)
	{
		const InfiniteLine& line = lines[n];

		bool doNotUse = false;
		for (unsigned int i = 0; i < lines.size(); ++i)
		{
			if (i != n && line.isSimilar(lines[i], minDistance, minCosAngle, halfOrientationPrecision) && line.strength() < lines[i].strength())
			{
				doNotUse = true;
				break;
			}
		}

		if (!doNotUse)
		{
			filteredLines.push_back(line);
		}
	}
}

void LineDetectorHough::parallelLines(const InfiniteLines& lines, const Scalar maxAngle, InfiniteLineGroups& parallelGroups, const unsigned int minimalSetSize, const bool noDuplicates)
{
	if (lines.empty())
	{
		return;
	}

	ocean_assert(parallelGroups.empty());

	const Scalar maxCosAngle = Numeric::cos(maxAngle);

	IndexSetVector indexSets;
	indexSets.resize(lines.size());

	for (size_t n0 = 0; n0 < lines.size() - 1; ++n0)
	{
		indexSets[n0].insert((unsigned int)n0);

		for (size_t n1 = n0 + 1; n1 < lines.size(); ++n1)
		{
			if (lines[n0].isParallel(lines[n1], maxCosAngle))
			{
				indexSets[n0].insert((unsigned int)n1);
				indexSets[n1].insert((unsigned int)n0);
			}
		}
	}

	std::sort(indexSets.begin(), indexSets.end(), compare);

	if (noDuplicates)
	{
		IndexSet32 duplicateSet;

		for (IndexSetVector::const_iterator i = indexSets.begin(); i != indexSets.end(); ++i)
		{
			if (i->size() > 1)
			{
				InfiniteLines newLines;
				newLines.reserve(i->size());

				for (IndexSet32::const_iterator iS = i->begin(); iS != i->end(); ++iS)
				{
					if (duplicateSet.find(*iS) == duplicateSet.end())
					{
						newLines.push_back(lines[*iS]);
						duplicateSet.insert(*iS);
					}
				}

				if (newLines.size() > 1 && (minimalSetSize == 0 || newLines.size() >= minimalSetSize))
				{
					parallelGroups.push_back(newLines);
				}
			}
			else
			{
				// the following index sets will have not more than one element
				break;
			}
		}
	}
	else
	{
		for (IndexSetVector::const_iterator i = indexSets.begin(); i != indexSets.end(); ++i)
		{
			if (i->size() > 1)
			{
				InfiniteLines newLines;
				newLines.reserve(i->size());

				for (IndexSet32::const_iterator iS = i->begin(); iS != i->end(); ++iS)
				{
					newLines.push_back(lines[*iS]);
				}

				if (minimalSetSize == 0 || newLines.size() >= minimalSetSize)
				{
					parallelGroups.push_back(newLines);
				}
			}
			else
			{
				// the following index sets will have not more than one element
				break;
			}
		}
	}
}

bool LineDetectorHough::parallelLines(const InfiniteLines& lines, const Scalar minAngle, InfiniteLines& parallels)
{
	ocean_assert(minAngle >= 0);
	ocean_assert(parallels.empty());

	const Scalar minAngleCos = Numeric::cos(minAngle);

	InfiniteLines sortedLines(lines);
	sortLinesAccordingDistance(sortedLines);

	Scalar maxAngleCosValue = 0;
	unsigned int maxValidNeighbors = 0u;

	for (size_t i = 0; i < lines.size(); ++i)
	{
		unsigned int validNeighbors = 0u;
		Scalar minAngleCosValue = 0;

		const InfiniteLine* line = &sortedLines[i];

		InfiniteLines newParalleLines;
		newParalleLines.reserve(lines.size());

		newParalleLines.push_back(*line);

		// left test
		for (size_t n = i - 1; n != size_t(-1); --n)
		{
			const InfiniteLine& testLine = sortedLines[n];

			ocean_assert(Numeric::isEqual(line->normal().length(), 1));
			ocean_assert(Numeric::isEqual(testLine.normal().length(), 1));

			const Scalar cosValue = line->normal() * testLine.normal();

			if (Numeric::abs(cosValue) > minAngleCos)
			{
				++validNeighbors;
				minAngleCosValue = min(Numeric::abs(cosValue), minAngleCosValue);

				newParalleLines.push_back(testLine);
				line = &testLine;
			}
		}

		line = &sortedLines[i];

		// right test
		for (size_t n = i + 1; n < lines.size(); ++n)
		{
			const InfiniteLine& testLine = sortedLines[n];

			ocean_assert(Numeric::isEqual(line->normal().length(), 1));
			ocean_assert(Numeric::isEqual(testLine.normal().length(), 1));

			const Scalar cosValue = line->normal() * testLine.normal();

			if (Numeric::abs(cosValue) > minAngleCos)
			{
				++validNeighbors;
				minAngleCosValue = min(Numeric::abs(cosValue), minAngleCosValue);

				newParalleLines.push_back(testLine);
				line = &testLine;
			}
		}

		if (validNeighbors > maxValidNeighbors || (validNeighbors == maxValidNeighbors && maxAngleCosValue < minAngleCosValue))
		{
			maxValidNeighbors = validNeighbors;
			maxAngleCosValue = minAngleCosValue;

			parallels = newParalleLines;
		}
	}

	return true;
}

void LineDetectorHough::createVotesHorizontalVerticalSubset(const int8_t* response, Accumulator* accumulator, const unsigned int angleNeigbors, const unsigned int voteThreshold, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(response != nullptr && accumulator != nullptr);
	ocean_assert(angleNeigbors && angleNeigbors < accumulator->angleBinsCore());

	const unsigned int width = accumulator->width();
	const unsigned int height = accumulator->height();

	const unsigned int firstFilterRow = max(0, int(firstRow) - 1) + 1;
	const unsigned int endFilterRow = (unsigned int)(min(firstRow + numberRows + 1u, height) - 1u);

	const int8_t lowerThreshold = -int8_t(voteThreshold);
	const int8_t upperThreshold = int8_t(voteThreshold);

	const int8_t* filterResponses = response + 2u * width * firstFilterRow + 2u;

	for (unsigned int y = firstFilterRow; y < endFilterRow; ++y)
	{
		ocean_assert((filterResponses - response) % (2u * width) == 2u);

		for (unsigned int x = 1u; x < width - 1u; ++x)
		{
			// responses for filter 0 and 90 degree
			if (filterResponses[0] <= lowerThreshold || filterResponses[0] >= upperThreshold || filterResponses[1] <= lowerThreshold || filterResponses[1] >= upperThreshold)
			{
				accumulator->accumulate(x, y, filterResponses, angleNeigbors);
			}

			filterResponses += 2;
		}

		filterResponses += 4;
	}
}

void LineDetectorHough::createVotesDiagonalSubset(const int8_t* response, Accumulator* accumulator, const unsigned int angleNeigbors, const unsigned int voteThreshold, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(response != nullptr && accumulator != nullptr);
	ocean_assert(angleNeigbors && angleNeigbors < accumulator->angleBinsCore());

	const unsigned int width = accumulator->width();
	const unsigned int height = accumulator->height();

	const unsigned int firstFilterRow = max(0, int(firstRow) - 1) + 1;
	const unsigned int endFilterRow = (unsigned int)(min(firstRow + numberRows + 1u, height) - 1u);

	const int8_t lowerThreshold = -int8_t(voteThreshold);
	const int8_t upperThreshold = int8_t(voteThreshold);

	const int8_t* filterResponses = response + 2u * width * firstFilterRow + 2u;

	for (unsigned int y = firstFilterRow; y < endFilterRow; ++y)
	{
		ocean_assert((filterResponses - response) % (2u * width) == 2u);

		for (unsigned int x = 1u; x < width - 1u; ++x)
		{
			// responses for filter 45 and 135 degree
			if (filterResponses[0] <= lowerThreshold || filterResponses[0] >= upperThreshold || filterResponses[1] <= lowerThreshold || filterResponses[1] >= upperThreshold)
			{
				accumulator->accumulateDiagonal(x, y, filterResponses, angleNeigbors);
			}

			filterResponses += 2;
		}

		filterResponses += 4;
	}
}

void LineDetectorHough::createVotesHorizontalVerticalDiagonalSubset(const int8_t* response, Accumulator* accumulator, const unsigned int angleNeigbors, const unsigned int voteThreshold, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(response != nullptr && accumulator != nullptr);
	ocean_assert(angleNeigbors && angleNeigbors < accumulator->angleBinsCore());

	const unsigned int width = accumulator->width();
	const unsigned int height = accumulator->height();

	const unsigned int firstFilterRow = max(0, int(firstRow) - 1) + 1;
	const unsigned int endFilterRow = (unsigned int)(min(firstRow + numberRows + 1u, height) - 1u);

	const int8_t lowerThreshold = -int8_t(voteThreshold);
	const int8_t upperThreshold = int8_t(voteThreshold);

	const int8_t* filterResponses = response + 4u * width * firstFilterRow + 4u;

	for (unsigned int y = firstFilterRow; y < endFilterRow; ++y)
	{
		ocean_assert((filterResponses - response) % (4u * width) == 4u);

		for (unsigned int x = 1u; x < width - 1u; ++x)
		{
			// responses for filter 0 and 90 degree
			if (filterResponses[0] <= lowerThreshold || filterResponses[0] >= upperThreshold || filterResponses[1] <= lowerThreshold || filterResponses[1] >= upperThreshold)
			{
				accumulator->accumulate(x, y, filterResponses, angleNeigbors);
			}

			// responses for filter 45 and 135 degree
			if (filterResponses[2] <= lowerThreshold || filterResponses[2] >= upperThreshold || filterResponses[3] <= lowerThreshold || filterResponses[3] >= upperThreshold)
			{
				accumulator->accumulateDiagonal(x, y, filterResponses + 2, angleNeigbors);
			}

			filterResponses += 4;
		}

		filterResponses += 8;
	}
}

void LineDetectorHough::detectFiniteLinesSubset(const InfiniteLines* infiniteLines, const int8_t* response, const unsigned int width, const unsigned int height, const FilterResponse filterResponse, const unsigned int angleBins, const Accumulator::AngleLookupData* horizontalAngleLookup, const Accumulator::AngleLookupData* diagonalAngleLookup, const bool halfOrientationPrecision, Lock* lock, FiniteLines2* finiteLines, const unsigned int firstLine, const unsigned int numberLines)
{
	ocean_assert(infiniteLines != nullptr && response != nullptr && finiteLines != nullptr );
	ocean_assert(firstLine + numberLines <= infiniteLines->size());

	FiniteLines2 localLines;
	localLines.reserve(100);

	for (unsigned int n = firstLine; n < firstLine + numberLines; ++n)
	{
		detectFiniteLines((*infiniteLines)[n], response, width, height, filterResponse, angleBins, horizontalAngleLookup, diagonalAngleLookup, halfOrientationPrecision, localLines);
	}

	const OptionalScopedLock optionalScopedLock(lock);

	if (lock && finiteLines->empty())
	{
		finiteLines->reserve(localLines.size() * 4);
	}

	finiteLines->insert(finiteLines->end(), localLines.begin(), localLines.end());
}

void LineDetectorHough::detectFiniteLines(const InfiniteLine& infiniteLine, const int8_t* response, const unsigned int width, const unsigned int height, const FilterResponse filterResponse, const unsigned int angleBins, const Accumulator::AngleLookupData* horizontalAngleLookup, const Accumulator::AngleLookupData* diagonalAngleLookup, const bool halfOrientationPrecision, FiniteLines2& finiteLines)
{
	ocean_assert(response != nullptr);

	const unsigned int minimalLength = 10u;
	const unsigned int maximalSpace = 10u;

	const Scalar width2 = Scalar(width) * Scalar(0.5);
	const Scalar height2 = Scalar(height) * Scalar(0.5);

	const int8_t responseThreshold = 8;

	const Line2 lineInPixelCoordinate(infiniteLine.point() + Vector2(width2, height2), infiniteLine.direction());

	int x0, y0, x1, y1;
	if (!CV::Bresenham::borderIntersection(lineInPixelCoordinate, 2, 2, int(width) - 3, int(height) - 3, x0, y0, x1, y1))
	{
		return;
	}

	CV::Bresenham bresenham(x0, y0, x1, y1);

	int angleBin = halfOrientationPrecision ? int((infiniteLine.angle() + Numeric::pi_2()) * Scalar(angleBins - 1) / Numeric::pi() + Scalar(0.5))
												: int((infiniteLine.angle() + Numeric::pi()) * Scalar(angleBins - 1) / Numeric::pi2() + Scalar(0.5));

	if (angleBin < 0)
	{
		angleBin = angleBins + angleBin;
	}
	else if (angleBin >= int(angleBins))
	{
		angleBin = angleBin - int(angleBins);
	}
	ocean_assert(angleBin >= 0 && angleBin < int(angleBins));

	const int lowerAngleBin = angleBin - 3;
	const int higherAngleBin = angleBin + 3;
	int lowBorderBin = -1;
	int highBorderBin = angleBins;

	if (lowerAngleBin < 0)
	{
		highBorderBin = angleBins + lowerAngleBin;
	}
	if (higherAngleBin >= int(angleBins))
	{
		lowBorderBin = higherAngleBin - angleBins;
	}

	unsigned int startX = (unsigned int)(-1);
	unsigned int startY = (unsigned int)(-1);
	unsigned int endX = (unsigned int)(-1);
	unsigned int endY = (unsigned int)(-1);

	bool started = false;
	unsigned int noLinePointSince = 0u;
	unsigned int lineLength = 0u;

	bool matchingResponse = false;

	while (x0 != x1 && y0 != y1)
	{
		switch (filterResponse)
		{
			case FR_HORIZONTAL_VERTICAL:
				ocean_assert(false && "Missing implementation!");
				break;

			case FR_DIAGONAL:
				ocean_assert(false && "Missing implementation!");
				break;

			case FR_HORIZONTAL_VERTICAL_DIAGONAL:
			{
				const int8_t* pixelResponse = response + 4 * (y0 * int(width) + x0);
				const unsigned int lookupBinHorizontal = horizontalAngleLookup[*(const uint16_t*)pixelResponse].angleBin_;
				const unsigned int lookupBinDiagonal = diagonalAngleLookup[*(((const uint16_t*)pixelResponse) + 1)].angleBin_;

				// if the response is intensive enough
				matchingResponse = (pixelResponse[0] > responseThreshold || pixelResponse[0] < -responseThreshold
									|| pixelResponse[1] > responseThreshold || pixelResponse[1] < -responseThreshold)
								// if the response values corresponds to the given line
								&& ((int(lookupBinHorizontal) >= lowerAngleBin && int(lookupBinHorizontal) <= higherAngleBin)
										|| (int(lookupBinDiagonal) >= lowerAngleBin && int(lookupBinDiagonal) <= higherAngleBin)
										|| int(lookupBinHorizontal) <= lowBorderBin || int(lookupBinHorizontal) >= highBorderBin // additional border check
										|| int(lookupBinDiagonal) <= lowBorderBin || int(lookupBinDiagonal) >= highBorderBin);

				// test direct neighbors if the response matches not to the line
				if (!matchingResponse)
					switch (bresenham.orientation())
					{
						case CV::Bresenham::ORIENTATION_0:
						case CV::Bresenham::ORIENTATION_3:
						case CV::Bresenham::ORIENTATION_4:
						case CV::Bresenham::ORIENTATION_7:
						{
							const int8_t* pixelResponseTop0 = response + 4 * ((y0 - 1) * int(width) + x0);
							const unsigned int lookupBinHorizontalTop0 = horizontalAngleLookup[*(const uint16_t*)pixelResponseTop0].angleBin_;
							const unsigned int lookupBinDiagonalTop0 = diagonalAngleLookup[*(((const uint16_t*)pixelResponseTop0) + 1)].angleBin_;

							const int8_t* pixelResponseBottom0 = response + 4 * ((y0 + 1) * int(width) + x0);
							const unsigned int lookupBinHorizontalBottom0 = horizontalAngleLookup[*(const uint16_t*)pixelResponseBottom0].angleBin_;
							const unsigned int lookupBinDiagonalBottom0 = diagonalAngleLookup[*(((const uint16_t*)pixelResponseBottom0) + 1)].angleBin_;

							matchingResponse = ((pixelResponseTop0[0] > responseThreshold || pixelResponseTop0[0] < -responseThreshold
														|| pixelResponseTop0[1] > responseThreshold || pixelResponseTop0[1] < -responseThreshold)
											// if the response values corresponds to the given line
											&& ((int(lookupBinHorizontalTop0) >= lowerAngleBin && int(lookupBinHorizontalTop0) <= higherAngleBin)
													|| (int(lookupBinDiagonalTop0) >= lowerAngleBin && int(lookupBinDiagonalTop0) <= higherAngleBin)
													|| int(lookupBinHorizontalTop0) <= lowBorderBin || int(lookupBinHorizontalTop0) >= highBorderBin // additional border check
													|| int(lookupBinDiagonalTop0) <= lowBorderBin || int(lookupBinDiagonalTop0) >= highBorderBin))

													|| ((pixelResponseBottom0[0] > responseThreshold || pixelResponseBottom0[0] < -responseThreshold
														|| pixelResponseBottom0[1] > responseThreshold || pixelResponseBottom0[1] < -responseThreshold)
											// if the response values corresponds to the given line
											&& ((int(lookupBinHorizontalBottom0) >= lowerAngleBin && int(lookupBinHorizontalBottom0) <= higherAngleBin)
													|| (int(lookupBinDiagonalBottom0) >= lowerAngleBin && int(lookupBinDiagonalBottom0) <= higherAngleBin)
													|| int(lookupBinHorizontalBottom0) <= lowBorderBin || int(lookupBinHorizontalBottom0) >= highBorderBin // additional border check
													|| int(lookupBinDiagonalBottom0) <= lowBorderBin || int(lookupBinDiagonalBottom0) >= highBorderBin));

							if (!matchingResponse)
							{
								const int8_t* pixelResponseTop1 = response + 4 * ((y0 - 2) * int(width) + x0);
								const unsigned int lookupBinHorizontalTop1 = horizontalAngleLookup[*(const uint16_t*)pixelResponseTop1].angleBin_;
								const unsigned int lookupBinDiagonalTop1 = diagonalAngleLookup[*(((const uint16_t*)pixelResponseTop1) + 1)].angleBin_;

								const int8_t* pixelResponseBottom1 = response + 4 * ((y0 + 2) * int(width) + x0);
								const unsigned int lookupBinHorizontalBottom1 = horizontalAngleLookup[*(const uint16_t*)pixelResponseBottom1].angleBin_;
								const unsigned int lookupBinDiagonalBottom1 = diagonalAngleLookup[*(((const uint16_t*)pixelResponseBottom1) + 1)].angleBin_;

								matchingResponse = ((pixelResponseTop1[0] > responseThreshold || pixelResponseTop1[0] < -responseThreshold
															|| pixelResponseTop1[1] > responseThreshold || pixelResponseTop1[1] < -responseThreshold)
												// if the response values corresponds to the given line
												&& ((int(lookupBinHorizontalTop1) >= lowerAngleBin && int(lookupBinHorizontalTop1) <= higherAngleBin)
														|| (int(lookupBinDiagonalTop1) >= lowerAngleBin && int(lookupBinDiagonalTop1) <= higherAngleBin)
														|| int(lookupBinHorizontalTop1) <= lowBorderBin || int(lookupBinHorizontalTop1) >= highBorderBin // additional border check
														|| int(lookupBinDiagonalTop1) <= lowBorderBin || int(lookupBinDiagonalTop1) >= highBorderBin))

														|| ((pixelResponseBottom1[0] > responseThreshold || pixelResponseBottom1[0] < -responseThreshold
															|| pixelResponseBottom1[1] > responseThreshold || pixelResponseBottom1[1] < -responseThreshold)
												// if the response values corresponds to the given line
												&& ((int(lookupBinHorizontalBottom1) >= lowerAngleBin && int(lookupBinHorizontalBottom1) <= higherAngleBin)
														|| (int(lookupBinDiagonalBottom1) >= lowerAngleBin && int(lookupBinDiagonalBottom1) <= higherAngleBin)
														|| int(lookupBinHorizontalBottom1) <= lowBorderBin || int(lookupBinHorizontalBottom1) >= highBorderBin // additional border check
														|| int(lookupBinDiagonalBottom1) <= lowBorderBin || int(lookupBinDiagonalBottom1) >= highBorderBin));
							}

							break;
						}

						case CV::Bresenham::ORIENTATION_1:
						case CV::Bresenham::ORIENTATION_2:
						case CV::Bresenham::ORIENTATION_5:
						case CV::Bresenham::ORIENTATION_6:
						{
							const int8_t* pixelResponseLeft0 = response + 4 * (y0 * int(width) + x0 - 1);
							const unsigned int lookupBinHorizontalLeft0 = horizontalAngleLookup[*(const uint16_t*)pixelResponseLeft0].angleBin_;
							const unsigned int lookupBinDiagonalLeft0 = diagonalAngleLookup[*(((const uint16_t*)pixelResponseLeft0) + 1)].angleBin_;

							const int8_t* pixelResponseRight0 = response + 4 * (y0 * int(width) + x0 + 1);
							const unsigned int lookupBinHorizontalRight0 = horizontalAngleLookup[*(const uint16_t*)pixelResponseRight0].angleBin_;
							const unsigned int lookupBinDiagonalRight0 = diagonalAngleLookup[*(((const uint16_t*)pixelResponseRight0) + 1)].angleBin_;

							matchingResponse = ((pixelResponseLeft0[0] > responseThreshold || pixelResponseLeft0[0] < -responseThreshold
														|| pixelResponseLeft0[1] > responseThreshold || pixelResponseLeft0[1] < -responseThreshold)
											// if the response values corresponds to the given line
											&& ((int(lookupBinHorizontalLeft0) >= lowerAngleBin && int(lookupBinHorizontalLeft0) <= higherAngleBin)
													|| (int(lookupBinDiagonalLeft0) >= lowerAngleBin && int(lookupBinDiagonalLeft0) <= higherAngleBin)
													|| int(lookupBinHorizontalLeft0) <= lowBorderBin || int(lookupBinHorizontalLeft0) >= highBorderBin // additional border check
													|| int(lookupBinDiagonalLeft0) <= lowBorderBin || int(lookupBinDiagonalLeft0) >= highBorderBin))

													|| ((pixelResponseRight0[0] > responseThreshold || pixelResponseRight0[0] < -responseThreshold
														|| pixelResponseRight0[1] > responseThreshold || pixelResponseRight0[1] < -responseThreshold)
											// if the response values corresponds to the given line
											&& ((int(lookupBinHorizontalRight0) >= lowerAngleBin && int(lookupBinHorizontalRight0) <= higherAngleBin)
													|| (int(lookupBinDiagonalRight0) >= lowerAngleBin && int(lookupBinDiagonalRight0) <= higherAngleBin)
													|| int(lookupBinHorizontalRight0) <= lowBorderBin || int(lookupBinHorizontalRight0) >= highBorderBin // additional border check
													|| int(lookupBinDiagonalRight0) <= lowBorderBin || int(lookupBinDiagonalRight0) >= highBorderBin));

							if (!matchingResponse)
							{
								const int8_t* pixelResponseLeft1 = response + 4 * (y0 * width + x0 - 2);
								const unsigned int lookupBinHorizontalLeft1 = horizontalAngleLookup[*(const uint16_t*)pixelResponseLeft1].angleBin_;
								const unsigned int lookupBinDiagonalLeft1 = diagonalAngleLookup[*(((const uint16_t*)pixelResponseLeft1) + 1)].angleBin_;

								const int8_t* pixelResponseRight1 = response + 4 * (y0 * width + x0 + 2);
								const unsigned int lookupBinHorizontalRight1 = horizontalAngleLookup[*(const uint16_t*)pixelResponseRight1].angleBin_;
								const unsigned int lookupBinDiagonalRight1 = diagonalAngleLookup[*(((const uint16_t*)pixelResponseRight1) + 1)].angleBin_;

								// if the response is intensive enough
								matchingResponse = ((pixelResponseLeft1[0] > responseThreshold || pixelResponseLeft1[0] < -responseThreshold
														|| pixelResponseLeft1[1] > responseThreshold || pixelResponseLeft1[1] < -responseThreshold)
											// if the response values corresponds to the given line
											&& ((int(lookupBinHorizontalLeft1) >= lowerAngleBin && int(lookupBinHorizontalLeft1) <= higherAngleBin)
													|| (int(lookupBinDiagonalLeft1) >= lowerAngleBin && int(lookupBinDiagonalLeft1) <= higherAngleBin)
													|| int(lookupBinHorizontalLeft1) <= lowBorderBin || int(lookupBinHorizontalLeft1) >= highBorderBin // additional border check
													|| int(lookupBinDiagonalLeft1) <= lowBorderBin || int(lookupBinDiagonalLeft1) >= highBorderBin))

													|| ((pixelResponseRight1[0] > responseThreshold || pixelResponseRight1[0] < -responseThreshold
														|| pixelResponseRight1[1] > responseThreshold || pixelResponseRight1[1] < -responseThreshold)
											// if the response values corresponds to the given line
											&& ((int(lookupBinHorizontalRight1) >= lowerAngleBin && int(lookupBinHorizontalRight1) <= higherAngleBin)
													|| (int(lookupBinDiagonalRight1) >= lowerAngleBin && int(lookupBinDiagonalRight1) <= higherAngleBin)
													|| int(lookupBinHorizontalRight1) <= lowBorderBin || int(lookupBinHorizontalRight1) >= highBorderBin // additional border check
													|| int(lookupBinDiagonalRight1) <= lowBorderBin || int(lookupBinDiagonalRight1) >= highBorderBin));

							}

							break;
						}

						default:
							ocean_assert(false && "This should never happen!");
					}

				break;
			}

			default:
				ocean_assert(false && "This should never happen!");
		}

		if (matchingResponse)
		{
			if (started)
			{
				ocean_assert(startX != (unsigned int)(-1));
				ocean_assert(startY != (unsigned int)(-1));

				endX = x0;
				endY = y0;
				++lineLength;
			}
			else
			{
				ocean_assert(startX == (unsigned int)(-1));
				ocean_assert(startY == (unsigned int)(-1));

				startX = x0;
				startY = y0;
				lineLength = 0u;
				started = true;
			}

			noLinePointSince = 0u;
		}
		else // if the response values correspond to no line
		{
			++noLinePointSince;

			// check whether a line could end
			if (started && noLinePointSince >= maximalSpace)
			{
				// check whether the ended line is long enough
				if (lineLength >= minimalLength)
				{
					finiteLines.emplace_back(Vector2(Scalar(startX), Scalar(startY)), Vector2(Scalar(endX), Scalar(endY)));
				}

				started = false;

#ifdef OCEAN_DEBUG
				startX = (unsigned int)(-1);
				startY = (unsigned int)(-1);
				endX = (unsigned int)(-1);
				endY = (unsigned int)(-1);
#endif
			}
		}

		bresenham.findNext(x0, y0);
	}

	if (started && lineLength >= minimalLength)
	{
		finiteLines.emplace_back(Vector2(Scalar(startX), Scalar(startY)), Vector2(Scalar(endX), Scalar(endY)));
	}
}

void LineDetectorHough::optimizeInfiniteLinesSubset(const InfiniteLine* infiniteLines, const size_t number, const int8_t* response, const unsigned int width, const unsigned int height, const FilterResponse filterResponse, const unsigned int radius, const Accumulator* accumulator, const bool halfOrientationPrecision, InfiniteLine* optimizedLines, const unsigned int firstLine, const unsigned int numberLines)
{
	ocean_assert(infiniteLines != nullptr && optimizedLines != nullptr);
	ocean_assert_and_suppress_unused(firstLine + numberLines <= number, number);

	const unsigned int sampingSteps = 10u;

	const Scalar width2 = Scalar(width) * Scalar(0.5);
	const Scalar height2 = Scalar(height) * Scalar(0.5);

	const int8_t responseThreshold = 8;
	const Accumulator::AngleLookupData* horizontalLookup = Accumulator::LookupManager::get().angleLookupData8BitResponse16BitRequest(accumulator->angleBinsCore(), halfOrientationPrecision);
	const Accumulator::AngleLookupData* diagonalLookup = Accumulator::LookupManager::get().angleLookupDataDiagonal8BitResponse16BitRequest(accumulator->angleBinsCore(), halfOrientationPrecision);

	const Scalar invPI = Scalar(1) / Numeric::pi();
	const Scalar invPI2 = Scalar(1) / Numeric::pi2();

	for (unsigned int n = firstLine; n < firstLine + numberLines; ++n)
	{
		const unsigned int angleBin = min((halfOrientationPrecision ? (unsigned int)((infiniteLines[n].angle() + Numeric::pi_2()) * Scalar(accumulator->angleBinsCore() - 1u) * invPI + Scalar(0.5))
															: (unsigned int)((infiniteLines[n].angle() + Numeric::pi()) * Scalar(accumulator->angleBinsCore() - 1u) * invPI2 + Scalar(0.5))), accumulator->angleBinsCore() - 1u);

		const Line2 lineInPixelCoordinate(infiniteLines[n].point() + Vector2(width2, height2), infiniteLines[n].direction());

		int x0, y0, x1, y1;
		if (!CV::Bresenham::borderIntersection(lineInPixelCoordinate, 2, 2, int(width) - 3, int(height) - 3, x0, y0, x1, y1))
		{
			continue;
		}

		const Vector2 lineNormal(infiniteLines[n].direction().perpendicular() * Scalar(radius));
		const VectorI2 normalPlus(Numeric::round32(lineNormal.x()), Numeric::round32(lineNormal.y()));
		const VectorI2 normalMinus(Numeric::round32(-lineNormal.x()), Numeric::round32(-lineNormal.y()));

		Vectors2 imagePoints;

		CV::Bresenham bresenham(x0, y0, x1, y1);

		unsigned int iteration = 0u;

		while (x0 != x1 && y0 != y1)
		{
			if (iteration++ % sampingSteps == 0u)
			{
				const VectorI2 position(x0, y0);

				VectorI2 start(position + normalMinus);
				const VectorI2 stop(position + normalPlus);

				CV::Bresenham localBresenham(start.x(), start.y(), stop.x(), stop.y());

				VectorI2 highestResponsePosition(-1000, -1000);
				unsigned int highestResponse = 0u;

				while (true)
				{
					if ((unsigned int)start.x() < width && (unsigned int)start.y() < height)
					{
						switch (filterResponse)
						{
							case FR_HORIZONTAL_VERTICAL:
							{
								const int8_t* pixelResponse = response + 2u * (start.y() * width + start.x());
								const unsigned int lookupBinHorizontal = horizontalLookup[*(const uint16_t*)pixelResponse].angleBin_;
								ocean_assert(lookupBinHorizontal < accumulator->angleBinsCore());

								// if the response is intensive enough
								if ((pixelResponse[0] > responseThreshold || pixelResponse[0] < -responseThreshold
										|| pixelResponse[1] > responseThreshold || pixelResponse[1] < -responseThreshold)
										// if the response values corresponds to the given line
										&& ringDistance(angleBin, lookupBinHorizontal, accumulator->angleBinsCore()) < accumulator->angleBinsCore() * 15u / 360u)
								{
									const unsigned int responseValue = max(abs(pixelResponse[0]), abs(pixelResponse[1]));

									if (responseValue > highestResponse)
									{
										highestResponsePosition = start;
										highestResponse = responseValue;
									}
									else if (responseValue == highestResponse && position.sqrDistance(start) < position.sqrDistance(highestResponsePosition))
									{
										highestResponsePosition = start;
									}
								}

								break;
							}

							case FR_DIAGONAL:
								ocean_assert(false && "Missing implementation!");
								break;

							case FR_HORIZONTAL_VERTICAL_DIAGONAL:
							{
								const int8_t* pixelResponse = response + 4u * (start.y() * width + start.x());
								const unsigned int lookupBinHorizontal = horizontalLookup[*(const uint16_t*)pixelResponse].angleBin_;
								const unsigned int lookupBinDiagonal = diagonalLookup[*(((const uint16_t*)pixelResponse) + 1)].angleBin_;

								// if the response is intensive enough
								if (((pixelResponse[0] > responseThreshold || pixelResponse[0] < -responseThreshold
										|| pixelResponse[1] > responseThreshold || pixelResponse[1] < -responseThreshold
										|| pixelResponse[2] > responseThreshold || pixelResponse[2] < -responseThreshold
										|| pixelResponse[3] > responseThreshold || pixelResponse[3] < -responseThreshold)
										// if the response values corresponds to the given line
										&& ringDistance(angleBin, lookupBinHorizontal, accumulator->angleBinsCore()) < accumulator->angleBinsCore() * 15u / 360u)
										|| ringDistance(angleBin, lookupBinDiagonal, accumulator->angleBinsCore()) < accumulator->angleBinsCore() * 15u / 360u)
								{
									const unsigned int responseValue = max(max(abs(pixelResponse[0]), abs(pixelResponse[1])),
																			max(abs(pixelResponse[2]), abs(pixelResponse[3])));

									if (responseValue > highestResponse)
									{
										highestResponsePosition = start;
										highestResponse = responseValue;
									}
									else if (responseValue == highestResponse && position.sqrDistance(start) < position.sqrDistance(highestResponsePosition))
									{
										highestResponsePosition = start;
									}
								}

								break;
							}

							default:
								ocean_assert(false && "Invalid filter response!");
								break;
						}
					}

					if (start == stop)
					{
						break;
					}

					localBresenham.findNext(start.x(), start.y());
				}

				if (highestResponse != 0u)
				{
					imagePoints.emplace_back(Scalar(highestResponsePosition.x()), Scalar(highestResponsePosition.y()));
				}
			}

			bresenham.findNext(x0, y0);
		}

		if (imagePoints.size() < 5)
		{
			continue;
		}

		// transform the image points into points with origin in the center of the image

		for (Vectors2::iterator i = imagePoints.begin(); i != imagePoints.end(); ++i)
		{
			*i = *i - Vector2(width2, height2);
		}

		Scalar initialSqrError, finalSqrError;
		Line2 internalOptimizedLine;
		if (!Geometry::NonLinearOptimizationLine::optimizeLine(infiniteLines[n], ConstArrayAccessor<Vector2>(imagePoints), internalOptimizedLine, 10u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), &initialSqrError, &finalSqrError))
		{
			continue;
		}

		const Vector2 optimizedNormal(-internalOptimizedLine.direction().y(), internalOptimizedLine.direction().x());
		const Scalar optimizedAngle(Numeric::atan2(optimizedNormal.y(), optimizedNormal.x()));

		optimizedLines[n] = InfiniteLine(optimizedNormal, optimizedAngle, internalOptimizedLine.point() * optimizedNormal, infiniteLines[n].strength());
	}
}

}

}

}
