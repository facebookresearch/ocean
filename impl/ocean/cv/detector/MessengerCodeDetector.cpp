/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/MessengerCodeDetector.h"

#include "ocean/base/StaticVector.h"

#include "ocean/cv/Canvas.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/geometry/Homography.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

void MessengerCodeDetector::DebugInformation::draw(Frame& frame) const
{
	ocean_assert(frame.isValid());

	const uint8_t* const redColor = CV::Canvas::red(frame.pixelFormat());
	const uint8_t* const greenColor = CV::Canvas::green(frame.pixelFormat());
	const uint8_t* const blueColor = CV::Canvas::blue(frame.pixelFormat());

	// first we draw all detected bullseyes (also those not belonging to a valid code)

	for (const Bullseye& bullseye : detectedBullseyes_)
	{
		drawBullseye(frame, bullseye, redColor);
	}

	for (const DetectedCode& detectedCode : detectedCodes_)
	{
		// Draw the connection between the bullseyes
		ocean_assert(detectedCode.bullseyes_.size() == 4u);

		for (size_t i = 0; i < detectedCode.bullseyes_.size(); ++i)
		{
			const Bullseye& currentBullseye = detectedCode.bullseyes_[i];
			const Bullseye& nextBullseye = detectedCode.bullseyes_[((i + 1) % detectedCode.bullseyes_.size())];

			CV::Canvas::line<3u>(frame, currentBullseye.position().x(), currentBullseye.position().y(), nextBullseye.position().x(), nextBullseye.position().y(), blueColor);
		}

		Vector2 codeCenter(Scalar(0), Scalar(0));

		// Draw bullseyes and the approximated center of the code
		for (const Bullseye& bullseye : detectedCode.bullseyes_)
		{
			drawBullseye(frame, bullseye, greenColor);
			codeCenter += bullseye.position();
		}

		codeCenter /= Scalar(detectedCode.bullseyes_.size());

		CV::Canvas::ellipse(frame, CV::PixelPosition((unsigned int)Numeric::round32(codeCenter.x()), (unsigned int)Numeric::round32(codeCenter.y())), 11u, 11u, redColor);

		// Draw the bits of the current messenger code
		ocean_assert(detectedCode.codebits_.size() == detectedCode.codeBitsLocationFrame_.size());

		constexpr unsigned int size = 7u;
		for (size_t codeIndex = 0; codeIndex < detectedCode.codebits_.size(); ++codeIndex)
		{
			const Vector2& codeBitLocation = detectedCode.codeBitsLocationFrame_[codeIndex];

			if (detectedCode.codebits_[codeIndex] == false)
			{
				CV::Canvas::point<size>(frame, codeBitLocation, redColor);
			}
			else
			{
				const Vector2 direction = (codeBitLocation - codeCenter).normalized() * Scalar(size) * Scalar(0.5);
				const Vector2 startLocation = codeBitLocation - direction;
				const Vector2 endLocation = codeBitLocation + direction;

				CV::Canvas::line<3u>(frame, startLocation.x(), startLocation.y(), endLocation.x(), endLocation.y(), greenColor);
			}
		}
	}
}

void MessengerCodeDetector::DebugInformation::drawBullseye(Frame& frame, const Bullseye& bullseye, const uint8_t* color)
{
	ocean_assert(frame);

	const Vector2 center = bullseye.position();
	const Scalar radius = bullseye.radius();

	CV::Canvas::line<3u>(frame, center.x() - radius, center.y(), center.x() + radius, center.y(), color);
	CV::Canvas::line<3u>(frame, center.x(), center.y() - radius, center.x(), center.y() + radius, color);
};

MessengerCodeDetector::Codes MessengerCodeDetector::detectMessengerCodes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, Worker* worker)
{
	return detectMessengerCodes<false>(yFrame, width, height, yFramePaddingElements, nullptr, worker);
}

MessengerCodeDetector::Codes MessengerCodeDetector::detectMessengerCodesWithDebugInformation(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, DebugInformation& debugInformation, const unsigned int yFramePaddingElements, Worker* worker)
{
	return detectMessengerCodes<true>(yFrame, width, height, yFramePaddingElements, &debugInformation, worker);
}

template <bool tCreateDebugInformation>
MessengerCodeDetector::Codes MessengerCodeDetector::detectMessengerCodes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, DebugInformation* debugInformation, Worker* worker)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(height >= 21u && width >= 21u);

	ocean_assert(tCreateDebugInformation == false || debugInformation != nullptr);

	// first, we detect all bullseyes in the given image

	MessengerCodeDetector::Bullseyes bullseyes = MessengerCodeDetector::detectBullseyes(yFrame, width, height, yFramePaddingElements, worker);

	// if no codes have been found, check if "dark mode" is enabled - we re-run the above function on the inverted input image, yFrame

	Frame yFrameInverted;

	if (bullseyes.size() < 4)
	{
		bullseyes.clear();

		if (!yFrameInverted.set(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return Codes();
		}

		CV::FrameInverter::invert8BitPerChannel(yFrame, yFrameInverted.data<uint8_t>(), width, height, 1u, yFramePaddingElements, yFrameInverted.paddingElements(), nullptr /* no worker, as too fast already*/);

		bullseyes = MessengerCodeDetector::detectBullseyes(yFrameInverted.constdata<uint8_t>(), width, height, yFrameInverted.paddingElements(), worker);

		if (bullseyes.empty())
		{
			return Codes();
		}
	}

	const uint8_t* const yFrameUsed = yFrameInverted ? yFrameInverted.constdata<uint8_t>() : yFrame;
	const unsigned int paddingElementsUsed = yFrameInverted ? yFrameInverted.paddingElements() : yFramePaddingElements;

	// we may have detected several individual locations for the same bullseye - so that we have to keep only one, **TODO** do it smarter in detectBullseyes()

	MessengerCodeDetector::Bullseyes filteredBullseyes;
	for (const CV::Detector::MessengerCodeDetector::Bullseye& bullseye : bullseyes)
	{
		bool foundClosePosition = false;

		for (size_t n = 0; !foundClosePosition && n < filteredBullseyes.size(); ++n)
		{
			foundClosePosition = bullseye.position().sqrDistance(filteredBullseyes[n].position()) < Numeric::sqr(10);
		}

		if (!foundClosePosition)
		{
			filteredBullseyes.push_back(bullseye);
		}
	}
	bullseyes = std::move(filteredBullseyes);

	if (tCreateDebugInformation && debugInformation != nullptr)
	{
		debugInformation->detectedBullseyes_ = bullseyes;
	}

	// a Messenger Code is composed of four bullseyes, so we can stop in case we have less
	if (bullseyes.size() < 4)
	{
		return Codes();
	}

	Codes detectedCodes;

	Vectors2 positions(bullseyes.size());
	Scalars radii(bullseyes.size());

	for (size_t n = 0; n < bullseyes.size(); ++n)
	{
		positions[n] = Vector2(bullseyes[n].position());
		radii[n] = Scalar(bullseyes[n].radius());
	}

	// let's find any valid combination of bullseyes that can potentially represent a Messenger Code

	const MessengerCodeDetector::IndexQuartets indexQuartets = MessengerCodeDetector::extractCodeCandidates(positions.data(), radii.data(), positions.size());

	for (const MessengerCodeDetector::IndexQuartet& indexQuartet : indexQuartets)
	{
		const Vector2 bullseyePositions[4] =
		{
			Vector2(bullseyes[indexQuartet[0]].position()),
			Vector2(bullseyes[indexQuartet[1]].position()),
			Vector2(bullseyes[indexQuartet[2]].position()),
			Vector2(bullseyes[indexQuartet[3]].position())
		};

		Scalar codeSize;
		SquareMatrix3 homography;

		if (MessengerCodeDetector::determineHomographyForBullseyeQuartet(bullseyePositions, homography, codeSize))
		{
			if (MessengerCodeDetector::isCodeInsideFrame(width, height, homography, codeSize))
			{
				if (MessengerCodeDetector::correctRotation(yFrameUsed, width, height, bullseyePositions, codeSize, homography, paddingElementsUsed))
				{
					MessengerCodeDetector::CodeBits codeBits;

					const unsigned int averageBullseyeGrayThreshold = (bullseyes[indexQuartet[0]].grayThreshold() + bullseyes[indexQuartet[1]].grayThreshold() + bullseyes[indexQuartet[2]].grayThreshold() + bullseyes[indexQuartet[3]].grayThreshold() + 2u) / 4u;
					ocean_assert(averageBullseyeGrayThreshold < 256u);

					Vectors2 codeBitsLocationFrame;
					if (MessengerCodeDetector::extractCodeBits<tCreateDebugInformation>(yFrameUsed, width, height, codeSize, homography, codeBits, averageBullseyeGrayThreshold, paddingElementsUsed, &codeBitsLocationFrame))
					{
						detectedCodes.push_back(std::move(codeBits));

						if (tCreateDebugInformation && debugInformation != nullptr)
						{
							DebugInformation::DetectedCode newDetectedCode;

							newDetectedCode.bullseyes_ = {bullseyes[indexQuartet[0]], bullseyes[indexQuartet[1]], bullseyes[indexQuartet[2]], bullseyes[indexQuartet[3]]};
							newDetectedCode.codebits_ = detectedCodes.back();
							newDetectedCode.codeBitsLocationFrame_ = std::move(codeBitsLocationFrame);

							debugInformation->detectedCodes_.emplace_back(std::move(newDetectedCode));
						}
					}
				}
			}
		}
	}

	return detectedCodes;
}

void MessengerCodeDetector::detectBullseyesSubset(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, Bullseyes* bullseyes, Lock* multiThreadLock, const unsigned int yFramePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 21u && height >= 21u);
	ocean_assert(firstRow >= 10u && numberRows <= height - 20u);
	ocean_assert(bullseyes != nullptr);

	Bullseyes localBullseyes;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		detectBullseyesInRow(yFrame, width, height, y, localBullseyes, yFramePaddingElements);
	}

	const OptionalScopedLock scopedLock(multiThreadLock);

	bullseyes->insert(bullseyes->end(), localBullseyes.cbegin(), localBullseyes.cend());
}

MessengerCodeDetector::IndexQuartets MessengerCodeDetector::extractCodeCandidates(const Vector2* bullseyes, const Scalar* radii, const size_t size, const Scalar radiusScaleTolerance, const Scalar distanceScaleTolerance)
{
	ocean_assert(bullseyes != nullptr);
	ocean_assert(radii != nullptr);
	ocean_assert(size >= 4);

	ocean_assert(radiusScaleTolerance >= Scalar(0) && radiusScaleTolerance <= Scalar(0.5));

	// Determine the maximum allowed value of distanceScaleTolerance (t) in order to ensure that the range of valid short distances (shortDistance*Sqr) and the range of valid long distances (longDistance*Sqr) do not overlap, i.e.:
	//
	//     radius2bullseyesDistanceShort(r) * maximalDistanceScaleFactor < radius2bullseyesDistanceLong(r) * minimalDistanceScaleFactor
	// <=> s(r)                             * (1 + t)                    < l(r)                            * (1 - t)
	// <=> (sqrt(2) / 2) * l(r)             * (1 + t)                    < l(r)                            * (1 - t)
	// <=> (1 + (sqrt(2) / 2)) * t < (1 - (sqrt(2) / 2))
	// <=> t < 3 - (2 * sqrt(2)) = 0.17157287525381
	ocean_assert(distanceScaleTolerance >= Scalar(0) && distanceScaleTolerance < Scalar(0.17157287525381));

	IndexQuartets indexQuartets;

	const Scalar minimalRadiusScaleFactor = Scalar(1) - radiusScaleTolerance;
	const Scalar maximalRadiusScaleFactor = Scalar(1) + radiusScaleTolerance;

	const Scalar minimalDistanceScaleFactor = Scalar(1) - distanceScaleTolerance;
	const Scalar maximalDistanceScaleFactor = Scalar(1) + distanceScaleTolerance;

	typedef StaticVector<unsigned int, 2> StaticVector;

	// we simply iterate over every possible combination of bullseyes while using geometric constraints to keep the list of candidates as small as possible

	for (size_t a = 0; a < size - 3; ++a)
	{
		const Scalar radiusMinimalA = radii[a] * minimalRadiusScaleFactor;
		const Scalar radiusMaximalA = radii[a] * maximalRadiusScaleFactor;

		for (size_t b = a + 1; b < size - 2; ++b)
		{
			if (radii[b] < radiusMinimalA || radii[b] > radiusMaximalA)
			{
				continue;
			}

			const Scalar averageRadiusAB = Scalar(0.5) * (radii[a] + radii[b]);

			const Scalar radiusMinimalAB = averageRadiusAB * minimalRadiusScaleFactor;
			const Scalar radiusMaximalAB = averageRadiusAB * maximalRadiusScaleFactor;

			const Scalar minSqrDistanceAB = Numeric::sqr(radius2bullseyesDistanceShort(averageRadiusAB) * minimalDistanceScaleFactor);
			const Scalar maxSqrDistanceAB = Numeric::sqr(radius2bullseyesDistanceLong(averageRadiusAB) * maximalDistanceScaleFactor);

			ocean_assert(minSqrDistanceAB < maxSqrDistanceAB);

			const Scalar sqrDistanceAB = bullseyes[a].sqrDistance(bullseyes[b]);

			if (sqrDistanceAB < minSqrDistanceAB || maxSqrDistanceAB < sqrDistanceAB)
			{
				continue;
			}

			for (size_t c = b + 1; c < size - 1; ++c)
			{
				if (radii[c] < radiusMinimalAB || radii[c] > radiusMaximalAB)
				{
					continue;
				}

				const Scalar averageRadiusABC = Scalar(1.0 / 3.0) * (radii[a] + radii[b] + radii[c]);

				const Scalar radiusMinimalABC = averageRadiusABC * minimalRadiusScaleFactor;
				const Scalar radiusMaximalABC = averageRadiusABC * maximalRadiusScaleFactor;

				const Scalar minSqrDistanceABC = Numeric::sqr(radius2bullseyesDistanceShort(averageRadiusABC) * minimalDistanceScaleFactor);
				const Scalar maxSqrDistanceABC = Numeric::sqr(radius2bullseyesDistanceLong(averageRadiusABC) * maximalDistanceScaleFactor);

				ocean_assert(minSqrDistanceABC < maxSqrDistanceABC);

				const Scalar sqrDistanceAC = bullseyes[a].sqrDistance(bullseyes[c]);

				if (sqrDistanceAC < minSqrDistanceABC || maxSqrDistanceABC < sqrDistanceAC)
				{
					continue;
				}

				for (size_t d = c + 1; d < size; ++d)
				{
					if (radii[d] < radiusMinimalABC || radii[d] > radiusMaximalABC)
					{
						continue;
					}

					// recompute the band of the short distances and the long distances based on the average radius of the current four bullseyes, then check if they form a valid candidate

					const Scalar averageRadiusABCD = Scalar(0.25) * (radii[a] + radii[b] + radii[c] + radii[d]);

					const Scalar shortDistanceMinimalSqr = Numeric::sqr(radius2bullseyesDistanceShort(averageRadiusABCD) * minimalDistanceScaleFactor);
					const Scalar shortDistanceMaximalSqr = Numeric::sqr(radius2bullseyesDistanceShort(averageRadiusABCD) * maximalDistanceScaleFactor);

					const Scalar longDistanceMinimalSqr = Numeric::sqr(radius2bullseyesDistanceLong(averageRadiusABCD) * minimalDistanceScaleFactor);
					const Scalar longDistanceMaximalSqr = Numeric::sqr(radius2bullseyesDistanceLong(averageRadiusABCD) * maximalDistanceScaleFactor);

					ocean_assert(shortDistanceMaximalSqr < longDistanceMinimalSqr);

					StaticVector shortDistances;
					StaticVector longDistances;

					const Scalar sqrDistanceAD = bullseyes[a].sqrDistance(bullseyes[d]);

					if (shortDistanceMinimalSqr <= sqrDistanceAD && sqrDistanceAD <= shortDistanceMaximalSqr)
					{
						shortDistances.pushBack((unsigned int)d);
					}
					else if (longDistanceMinimalSqr <= sqrDistanceAD && sqrDistanceAD <= longDistanceMaximalSqr)
					{
						longDistances.pushBack((unsigned int)d);
					}
					else
					{
						continue;
					}

					if (shortDistanceMinimalSqr <= sqrDistanceAC && sqrDistanceAC <= shortDistanceMaximalSqr)
					{
						shortDistances.pushBack((unsigned int)c);
					}
					else if (longDistanceMinimalSqr <= sqrDistanceAC && sqrDistanceAC <= longDistanceMaximalSqr && longDistances.empty())
					{
						longDistances.pushBack((unsigned int)c);
					}
					else
					{
						continue;
					}

					if (shortDistanceMinimalSqr <= sqrDistanceAB && sqrDistanceAB <= shortDistanceMaximalSqr && shortDistances.size() == 1)
					{
						shortDistances.pushBack((unsigned int)b);
					}
					else if (longDistanceMinimalSqr <= sqrDistanceAB && sqrDistanceAB <= longDistanceMaximalSqr && longDistances.empty())
					{
						longDistances.pushBack((unsigned int)b);
					}
					else
					{
						continue;
					}

					ocean_assert(shortDistances.size() + longDistances.size() == 3);
					ocean_assert(shortDistances.size() == 2 && longDistances.size() == 1);

					// sort the four candidates in counter-clockwise order

					ocean_assert(bullseyes[a] != bullseyes[shortDistances[0]]);
					ocean_assert(bullseyes[a] != bullseyes[shortDistances[1]]);
					ocean_assert(bullseyes[a] != bullseyes[longDistances[0]]);
					ocean_assert(bullseyes[shortDistances[0]] != bullseyes[shortDistances[1]]);
					ocean_assert(bullseyes[shortDistances[0]] != bullseyes[longDistances[0]]);
					ocean_assert(bullseyes[shortDistances[1]] != bullseyes[longDistances[0]]);

					IndexQuartet indexQuartet;
					indexQuartet[0] = (unsigned int)a;
					indexQuartet[2] = longDistances[0];

					if ((bullseyes[shortDistances[0]] - bullseyes[a]).cross(bullseyes[shortDistances[1]] - bullseyes[a]) < 0)
					{
						indexQuartet[1] = shortDistances[0];
						indexQuartet[3] = shortDistances[1];
					}
					else
					{
						indexQuartet[1] = shortDistances[1];
						indexQuartet[3] = shortDistances[0];
					}

					// check whether the remaining two short edges and the one remaining long edge match

					const Scalar shortSqrDistance12 = bullseyes[indexQuartet[1]].sqrDistance(bullseyes[indexQuartet[2]]);
					const Scalar shortSqrDistance23 = bullseyes[indexQuartet[2]].sqrDistance(bullseyes[indexQuartet[3]]);
					const Scalar longSqrDistance13 = bullseyes[indexQuartet[1]].sqrDistance(bullseyes[indexQuartet[3]]);

					if (shortDistanceMinimalSqr <= shortSqrDistance12 && shortSqrDistance12 <= shortDistanceMaximalSqr
						&& shortDistanceMinimalSqr <= shortSqrDistance23 && shortSqrDistance23 <= shortDistanceMaximalSqr
						&& longDistanceMinimalSqr <= longSqrDistance13 && longSqrDistance13 <= longDistanceMaximalSqr)
					{
						indexQuartets.push_back(indexQuartet);
					}
				}
			}
		}
	}

	return indexQuartets;
}

bool MessengerCodeDetector::determineHomographyForBullseyeQuartet(const Vector2* bullseyes, SquareMatrix3& homography, Scalar& codeSize)
{
	ocean_assert(bullseyes != nullptr);

	// all given points must be different
	ocean_assert(bullseyes[0] != bullseyes[1] && bullseyes[0] != bullseyes[2] && bullseyes[0] != bullseyes[3]);
	ocean_assert(bullseyes[1] != bullseyes[2] && bullseyes[1] != bullseyes[3]);
	ocean_assert(bullseyes[2] != bullseyes[3]);

	// we expect a counter-clockwise order
	ocean_assert((bullseyes[2] - bullseyes[1]).cross(bullseyes[0] - bullseyes[1]) <= 0);
	ocean_assert((bullseyes[3] - bullseyes[2]).cross(bullseyes[1] - bullseyes[2]) <= 0);
	ocean_assert((bullseyes[0] - bullseyes[3]).cross(bullseyes[2] - bullseyes[3]) <= 0);
	ocean_assert((bullseyes[1] - bullseyes[0]).cross(bullseyes[3] - bullseyes[0]) <= 0);

	const Scalar averageDistanceBullseyes = (bullseyes[0].distance(bullseyes[1]) + bullseyes[1].distance(bullseyes[2]) + bullseyes[2].distance(bullseyes[3]) + bullseyes[3].distance(bullseyes[0])) * Scalar(0.25);

	const Scalar boxEdgeLength = averageDistanceBullseyes * Scalar(1.41421356237309505); // = aDB * 2 / sqrt(2)
	const Scalar boxEdgeLength_2 = boxEdgeLength * Scalar(0.5);

	// we add an explicit offset to cover the entire Messenger Code (not only the bullseyes)
	const Scalar boxOffset107 = boxEdgeLength * Scalar(0.107);

	const Vector2 rectifiedBullseyes[4] =
	{
		Vector2(boxEdgeLength_2, boxOffset107),
		Vector2(boxOffset107, boxEdgeLength_2),
		Vector2(boxEdgeLength_2, boxEdgeLength - boxOffset107),
		Vector2(boxEdgeLength - boxOffset107, boxEdgeLength_2),
	};

	if (!Geometry::Homography::homographyMatrixLinearWithoutOptimations(rectifiedBullseyes, bullseyes, 4, homography))
	{
		return false;
	}

	ocean_assert((std::is_same<Scalar, double>::value == false || bullseyes[0].isEqual(homography * rectifiedBullseyes[0], Scalar(0.01))));
	ocean_assert((std::is_same<Scalar, double>::value == false || bullseyes[1].isEqual(homography * rectifiedBullseyes[1], Scalar(0.01))));
	ocean_assert((std::is_same<Scalar, double>::value == false || bullseyes[2].isEqual(homography * rectifiedBullseyes[2], Scalar(0.01))));
	ocean_assert((std::is_same<Scalar, double>::value == false || bullseyes[3].isEqual(homography * rectifiedBullseyes[3], Scalar(0.01))));

	codeSize = boxEdgeLength;

	return true;
}

bool MessengerCodeDetector::isCodeInsideFrame(const unsigned int width, const unsigned int height, const SquareMatrix3& homography, const Scalar codeSize)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(!homography.isSingular());

	const Box2 frameBoundingBox(Scalar(0), Scalar(0), Scalar(width - 1u), Scalar(height - 1u));

	const Scalar codeRadius = codeSize * Scalar(0.475);
	const Vector2 codeCenter = Vector2(codeSize, codeSize) * Scalar(0.5);

	for (unsigned int n = 0u; n < 18u; ++n)
	{
		const Vector2 codePostition = codeCenter + (Quaternion(Vector3(0, 0, 1), Numeric::deg2rad(Scalar(n * 20u))) * Vector3(codeRadius, 0, 0)).xy();
		const Vector2 framePosition = homography * codePostition;

		if (!frameBoundingBox.isInside(framePosition))
		{
			return false;
		}
	}

	return true;
}

template <bool tCreateDebugInformation>
bool MessengerCodeDetector::extractCodeBits(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const Scalar codeSize, const SquareMatrix3& homography, CodeBits& codeBits, const unsigned int grayThreshold, const unsigned int framePaddingElements, Vectors2* codeBitsLocationFrame)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(tCreateDebugInformation == false || codeBitsLocationFrame != nullptr);

	const unsigned int frameStrideElements = width + framePaddingElements;

	const Scalar codeSize_2 = codeSize * Scalar(0.5);
	const Vector2 codeCenter = Vector2(codeSize_2, codeSize_2);

	const Scalar codeRadius = codeSize * Scalar(0.46);

	const Vectors2& locations = codeBitCoordinates();

	Memory lookupValuesMemory = Memory::create<uint8_t>(codeBits.size());
	uint8_t* lookupValues = lookupValuesMemory.data<uint8_t>();
	memset(lookupValues, 0x00, sizeof(uint8_t) * codeBits.size());

	if (tCreateDebugInformation && codeBitsLocationFrame != nullptr)
	{
		codeBitsLocationFrame->clear();
		codeBitsLocationFrame->reserve(locations.size());

		for (size_t n = 0; n < locations.size(); ++n)
		{
			const Vector2 samplePositionCode = codeCenter + locations[n] * codeRadius;
			codeBitsLocationFrame->emplace_back(homography * samplePositionCode);
		}
	}

	for (size_t n = 0; n < locations.size(); ++n)
	{
		const Vector2 samplePositionCode = codeCenter + locations[n] * codeRadius;
		const Vector2 samplePositionFrame = homography * samplePositionCode;

		const unsigned int x = (unsigned int)(samplePositionFrame.x() + Scalar(0.5));
		const unsigned int y = (unsigned int)(samplePositionFrame.y() + Scalar(0.5));

		unsigned int lookupRadius = codeSize > Scalar(100) ? 3u : 2u;

		if (x <= lookupRadius || y <= lookupRadius || x >= width - lookupRadius || y >= height - lookupRadius)
		{
			ocean_assert(false && "Should never happen!");
			return false;
		}

		uint8_t minLookupValue = yFrame[y * frameStrideElements + x];

		for (unsigned int r = 1u; r < lookupRadius; ++r)
		{
			minLookupValue = std::min(minLookupValue, yFrame[y * frameStrideElements + x + r]);
			minLookupValue = std::min(minLookupValue, yFrame[y * frameStrideElements + x - r]);

			minLookupValue = std::min(minLookupValue, yFrame[(y + r) * frameStrideElements + x]);
			minLookupValue = std::min(minLookupValue, yFrame[(y - r) * frameStrideElements + x]);
		}

		lookupValues[n] = minLookupValue;
	}

	// the average lookup value will be our threshold between black and white pixels

	ocean_assert(grayThreshold < 256u);
	for (size_t n = 0; n < locations.size(); ++n)
	{
		codeBits[n] = lookupValues[n] < grayThreshold;
	}

	return true;
}

bool MessengerCodeDetector::correctRotation(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const Vector2* bullseyes, const Scalar codeSize, SquareMatrix3& homography, const unsigned int framePaddingElements)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert_and_suppress_unused(bullseyes != nullptr, bullseyes);
	ocean_assert(codeSize > Scalar(0));
	ocean_assert(!homography.isSingular());

	// we determine several samples between the inner core of the Messenger Code and the first ring containing the bit information
	// one sample area should have a significant difference from the other three sample areas

	const unsigned int frameStrideElements = width + framePaddingElements;

	constexpr unsigned int samples = 12u;
	const Scalar sampleArcAngle = Numeric::deg2rad(160);

	const Scalar sampleRadiusCore = codeSize * Scalar(0.320); // codeSize / 2 - codeSize * 18%
	const Scalar sampleRadiusLogoInner = codeSize * Scalar(0.055);
	const Scalar sampleRadiusLogoOuter = codeSize * Scalar(0.075);

	const Vector2 codeCenter = Vector2(codeSize, codeSize) * Scalar(0.5);

	int sampleSumDifference[4] = {0, 0, 0, 0};

	for (unsigned int sampleIteration = 0u; sampleIteration < 4u; ++sampleIteration)
	{
		const Scalar samplePositionAngleCenter = Numeric::deg2rad(Scalar(45) + Scalar(90) * Scalar(sampleIteration));

		const HomogenousMatrix4 sampleCoordinateSystemLogo(HomogenousMatrix4(Vector3(codeCenter, 0)) * HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), samplePositionAngleCenter)) * HomogenousMatrix4(Vector3(sampleRadiusCore, 0, 0)));

		const Scalar sampleAngleStart = -sampleArcAngle * Scalar(0.5);

		static_assert(samples >= 3u, "Invalid sample number!");
		const Scalar sampleAngleSteps = sampleArcAngle / Scalar(samples - 1u);

		for (unsigned int n = 0u; n < samples; ++n)
		{
			const Scalar samplePositionAngle = sampleAngleStart + sampleAngleSteps * Scalar(n);

			const HomogenousMatrix4 sampleCoordinateSystem = sampleCoordinateSystemLogo * HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), samplePositionAngle));

			const Vector2 samplePositionFrameInner = homography * (sampleCoordinateSystem * Vector3(sampleRadiusLogoInner, 0, 0)).xy();

			const unsigned int xInner = (unsigned int)(samplePositionFrameInner.x() + Scalar(0.5));
			const unsigned int yInner = (unsigned int)(samplePositionFrameInner.y() + Scalar(0.5));

			if (xInner >= width || yInner >= height) // negative values always >= width
			{
				return false;
			}

			const Vector2 samplePositionFrameOuter = homography * (sampleCoordinateSystem * Vector3(sampleRadiusLogoOuter, 0, 0)).xy();

			const unsigned int xOuter = (unsigned int)(samplePositionFrameOuter.x() + Scalar(0.5));
			const unsigned int yOuter = (unsigned int)(samplePositionFrameOuter.y() + Scalar(0.5));

			if (xOuter >= width || yOuter >= height) // negative values always >= width
			{
				return false;
			}

			const int difference = int(yFrame[yOuter * frameStrideElements + xOuter]) - int(yFrame[yInner * frameStrideElements + xInner]);

			sampleSumDifference[sampleIteration] += difference;
		}
	}

	// we sort sampleSums

	unsigned int bestDifferenceIndex = 0u;

	int bestSumDifference = sampleSumDifference[0];
	int secondBestSumDifference = NumericT<int>::minValue();

	for (unsigned int n = 1u; n < 4u; ++n)
	{
		if (sampleSumDifference[n] > bestSumDifference)
		{
			secondBestSumDifference = bestSumDifference;

			bestSumDifference = sampleSumDifference[n];
			bestDifferenceIndex = n;
		}
		else if (sampleSumDifference[n] < secondBestSumDifference)
		{
			secondBestSumDifference = sampleSumDifference[n];
		}
	}

	const int average = (sampleSumDifference[0] + sampleSumDifference[1] + sampleSumDifference[2] + sampleSumDifference[3]) / 4;

	// we have a valid Messenger Code if the first difference is very high, while all other differences are quite low

	if (!(bestSumDifference - int(samples) * 10 >= secondBestSumDifference && bestSumDifference > average && secondBestSumDifference < average))
	{
		return false;
	}


	// we need to rotate the code so that the best index matches with index 0
	// 0 ->    0 deg
	// 1 ->  -90 deg
	// 2 -> -180 deg
	// 3 -> -270 deg

	const Scalar rotationAngle = Scalar(bestDifferenceIndex) * -Numeric::pi_2();

	const SquareMatrix3 translationCenterForward(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(codeCenter, 1));
	const SquareMatrix3 translationCenterBackward(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(-codeCenter, 1));
	ocean_assert(translationCenterForward == translationCenterBackward.inverted());

	const SquareMatrix3 rotationMatrix = translationCenterForward * SquareMatrix3(Quaternion(Vector3(0, 0, -1), rotationAngle)) * translationCenterBackward;

	homography = homography * rotationMatrix;

	return true;
}

void MessengerCodeDetector::detectBullseyesInRow(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int y, Bullseyes& bullseyes, const unsigned int yFramePaddingElements)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 21u && height >= 21u);
	ocean_assert(y >= 10u && y < height - 10u);

	const unsigned int yFrameStrideElements = width + yFramePaddingElements;

	const uint8_t* const yRow = yFrame + yFrameStrideElements * y;

	// we are looking for the following pattern (white, black, white, black, white, black, white):
	// W B W B B B W B W

	// the first segment must be black

	unsigned int segment_1_start_black = (unsigned int)(-1);
	unsigned int segment_2_start_white = (unsigned int)(-1);

	unsigned int x = 1u;

	// start segment 1: we search for the start of the first black segment (with white pixel to the left)

	TransitionHistory history;
	while (x < width && !isTransitionToBlack(yRow + x, history))
	{
		++x;
	}

	if (x == width)
	{
		return;
	}

	ocean_assert(segment_1_start_black == (unsigned int)(-1));
	segment_1_start_black = x;

	while (x < width)
	{
		// start segment 2: we search for the start of the first white segment
		if (segment_2_start_white == (unsigned int)(-1))
		{
			history.reset();
			while (x < width && !isTransitionToWhite(yRow + x, history))
			{
				++x;
			}

			if (x == width)
			{
				break;
			}

			ocean_assert(segment_2_start_white == (unsigned int)(-1));
			segment_2_start_white = x;
		}

		ocean_assert(segment_1_start_black < segment_2_start_white);
		const unsigned int segment_1_size = segment_2_start_white - segment_1_start_black;

		const unsigned int blackRingSegmentMin = max(1u, segment_1_size * 384u / 512u); // 0.75 ~ 384/512
		const unsigned int blackRingSegmentMax = max(segment_1_size + 3u, segment_1_size * 640u / 512u); // 1.25 ~ 640/512

		// start segment 3: we search for the start of the second black segment (the center dot)

		history.reset();
		while (x < width && !isTransitionToBlack(yRow + x, history))
		{
			++x;
		}

		if (x == width)
		{
			break;
		}

		ocean_assert(segment_2_start_white < x);
		const unsigned int segment_2_size = x - segment_2_start_white;

		// now we can check whether the first black and first white segment had almost identical sizes

		ocean_assert(segment_1_start_black < segment_2_start_white);
		ocean_assert(segment_2_start_white < x);

		const unsigned int whiteRingSegmentMin = max(1u, segment_1_size * 384u / 512u); // 0.75 ~ 384/512
		const unsigned int whiteRingSegmentMax = max(segment_1_size + 6u, segment_1_size * 640u / 512u); // 1.25 ~ 640/512

		if (segment_2_size < whiteRingSegmentMin || segment_2_size > whiteRingSegmentMax)
		{
			// the first two segments are too different, so we discard the first two segments
			// and we can use the current third segment (black) as new first segment

			segment_1_start_black = x;
			segment_2_start_white = (unsigned int)(-1);

			// x stays untouched

			continue;
		}

		const unsigned int segment_3_start_black = x;

		// start segment 4: we search for the start of the second white segment

		history.reset();
		while (x < width && !isTransitionToWhite(yRow + x, history))
		{
			++x;
		}

		if (x == width)
		{
			break;
		}

		ocean_assert(segment_3_start_black < x);
		const unsigned int segment_3_size = x - segment_3_start_black;

		const unsigned int segment_4_start_white = x;

		// now we can check whether the third segment has the same size as the first two segments (or is slightly smaller)

		ocean_assert(segment_1_start_black < segment_2_start_white);
		ocean_assert(segment_2_start_white < segment_3_start_black);
		ocean_assert(segment_3_start_black < segment_4_start_white);

		const unsigned int dotSegmentMin = max((unsigned int)max(1, int(blackRingSegmentMin) - 3), blackRingSegmentMin * 384u / 512u); // 0.75 ~ 384/512
		const unsigned int dotSegmentMax = blackRingSegmentMax;

		if (segment_3_size < dotSegmentMin || segment_3_size > dotSegmentMax)
		{
			// the size of the center block does not match with the size of the first two blocks
			// so we discard the first two segments and we use the center segment as new first black segment
			// and the start of the forth segment as start of the second segment

			segment_1_start_black = segment_3_start_black;
			segment_2_start_white = segment_4_start_white;

			// we also have to reset x
			x = segment_2_start_white;

			continue;
		}

		// start segment 5: we search for the start of the last black segment

		history.reset();
		while (x < width && !isTransitionToBlack(yRow + x, history))
		{
			++x;
		}

		if (x == width)
		{
			break;
		}

		ocean_assert(segment_4_start_white < x);
		const unsigned int segment_4_size = x - segment_4_start_white;

		if (segment_4_size < whiteRingSegmentMin || segment_4_size > whiteRingSegmentMax)
		{
			// the forth segment (the last white segment) does not have the correct size (not 1/3 of the third size)

			// in case the forth segment has same size as the third segment, we take the third and forth segment as new first and second
			// otherwise, we take the start of the fifth segment as very first black segment

			if (segment_4_size < segment_3_size * 384u / 512u || segment_4_size > segment_3_size * 640u / 512u) // 0.75 ~ 384/512,   1.25 ~ 640/512
			{
				// third and forth segment don't match

				segment_1_start_black = x;
				segment_2_start_white = (unsigned int)(-1);

				// x stays untouched
			}
			else
			{
				// third and forth segment match

				segment_1_start_black = segment_3_start_black;
				segment_2_start_white = segment_4_start_white;

				// we also have to reset x
				x = segment_2_start_white;
			}

			continue;
		}

		const unsigned int segment_5_start_black = x;

		// start 'segment 6': we search for the start of the next white segment (the end of the last black segment + 1 pixel)

		history.reset();
		while (x < width && !isTransitionToWhite(yRow + x, history))
		{
			++x;
		}

		if (x == width)
		{
			break;
		}

		ocean_assert(segment_5_start_black < x);
		const unsigned int segment_5_size = x - segment_5_start_black;

		// finally we can check whether also the last black segment has the correct size

		if (segment_5_size >= blackRingSegmentMin && segment_5_size <= blackRingSegmentMax)
		{
			// we have a valid combination of segments

			const unsigned int xCenter = (segment_3_start_black + segment_4_start_white + 1u) / 2u;

			const unsigned int grayThreshold = determineThreshold(yRow + segment_1_start_black, segment_1_size, segment_2_size, segment_3_size, segment_4_size, segment_5_size);

			// let's ensure that the center pixel actually fits with the determined gray threshold
			if (grayThreshold < 255u && yFrame[y * yFrameStrideElements + xCenter] <= grayThreshold)
			{
				// the diameter of the bullseye including the left and right black segment
				const unsigned int diameter = x - segment_1_start_black;
				ocean_assert(diameter >= 5u);

				const unsigned int diameter3_4 = (diameter * 3u + 2u) / 4u;

				// now we check whether we can find the same bullseye pattern in vertical direction

				if (xCenter >= diameter3_4 && y >= diameter3_4 && xCenter < width - diameter3_4 && y < height - diameter3_4
						&& checkBullseyeInColumn(yFrame, yFrameStrideElements, height, xCenter, y, grayThreshold, blackRingSegmentMin, blackRingSegmentMax, whiteRingSegmentMin, whiteRingSegmentMax, dotSegmentMin, dotSegmentMax))
				{
					const float whiteRingRadius = float(segment_3_size) * 0.5f + float(segment_2_size + segment_4_size) * 0.25f;
					const float blackRingRadius = whiteRingRadius + float(segment_2_size + segment_4_size) * 0.25f + float(segment_1_size + segment_5_size) * 0.25f;
					const float whiteBorderRadius = blackRingRadius * 1.5f;

					if (checkBullseyeInNeighborhood(yFrame, width, height, xCenter, y, grayThreshold, whiteRingRadius, blackRingRadius, whiteBorderRadius, yFramePaddingElements))
					{
						Vector2 location;
						if (determineAccurateBullseyeLocation(yFrame, width, height, xCenter, y, grayThreshold, location, yFramePaddingElements))
						{
							const float radius = float(diameter) * 0.5f;

							ocean_assert(location.x() >= Scalar(radius) && location.y() >= Scalar(radius));
							ocean_assert(location.x() < Scalar(width) - Scalar(radius) && location.y() < Scalar(height) - Scalar(radius));

							bullseyes.emplace_back(location, radius, grayThreshold);
						}
					}
				}
			}
		}

		// in any case (either if the last segment does not have the correct size, or if we found a valid segment combination)
		// we use the last black segment as new first black segment

		segment_1_start_black = segment_3_start_black;
		segment_2_start_white = segment_4_start_white;

		// we also have to reset x
		x = segment_2_start_white;
	}
}

bool MessengerCodeDetector::checkBullseyeInColumn(const uint8_t* const yFrame, const unsigned int frameStrideElements, const unsigned int height, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const unsigned int blackRingSegmentMin, const unsigned int blackRingSegmentMax, const unsigned int whiteRingSegmentMin, const unsigned int whiteRingSegmentMax, const unsigned int dotSegmentMin, const unsigned int dotSegmentMax)
{
	ocean_assert(yFrame != nullptr);

	ocean_assert(xCenter < frameStrideElements && yCenter < height); // xCenter < width, but we do not have 'width'

	const uint8_t* yCenterPointer = yFrame + yCenter * frameStrideElements + xCenter;

	// the black center dot

	const unsigned int dotSegmentMaxHalf = (dotSegmentMax + 1u) / 2u;
	const unsigned int dotSegmentSymmetricTolerance = max(1u, dotSegmentMax * 179u / 512u); // ~ 35%

	unsigned int topRows;
	unsigned int bottomRows;
	if (!findNextUpperPixel<false>(yCenterPointer, yCenter, dotSegmentMaxHalf + 1u, threshold, frameStrideElements, topRows)
		|| !findNextLowerPixel<false>(yCenterPointer, yCenter, height, dotSegmentMaxHalf + 1u, threshold, frameStrideElements, bottomRows)
		|| NumericT<unsigned int>::isNotEqual(topRows, bottomRows, dotSegmentSymmetricTolerance)
		|| topRows + bottomRows - 1u < dotSegmentMin // (topRows + bottomRows - 1): as top and bottom include the center pixel
		|| topRows + bottomRows - 1u > dotSegmentMax)
	{
		return false;
	}

	// the inner white ring

	unsigned int upperStart = yCenter - topRows;
	unsigned int lowerStart = yCenter + bottomRows;

	const uint8_t* yTopPointer = yCenterPointer - topRows * frameStrideElements;
	const uint8_t* yBottomPointer = yCenterPointer + bottomRows * frameStrideElements;

	if (!findNextUpperPixel<true>(yTopPointer, upperStart, whiteRingSegmentMax + 1u, threshold, frameStrideElements, topRows)
		|| !findNextLowerPixel<true>(yBottomPointer, lowerStart, height, whiteRingSegmentMax + 1u, threshold, frameStrideElements, bottomRows)
		|| topRows < whiteRingSegmentMin || bottomRows < whiteRingSegmentMin)
	{
		return false;
	}

	ocean_assert(topRows >= whiteRingSegmentMin && topRows <= whiteRingSegmentMax + 1u);
	ocean_assert(bottomRows >= whiteRingSegmentMin && bottomRows <= whiteRingSegmentMax + 1u);

	// the outer black ring

	upperStart -= topRows;
	lowerStart += bottomRows;

	yTopPointer -= topRows * frameStrideElements;
	yBottomPointer += bottomRows * frameStrideElements;

	if (!findNextUpperPixel<false>(yTopPointer, upperStart, blackRingSegmentMax + 1u, threshold, frameStrideElements, topRows)
		|| !findNextLowerPixel<false>(yBottomPointer, lowerStart, height, blackRingSegmentMax + 1u, threshold, frameStrideElements, bottomRows)
		|| topRows < blackRingSegmentMin || bottomRows < blackRingSegmentMin)
	{
		return false;
	}

	ocean_assert(topRows >= blackRingSegmentMin && topRows <= blackRingSegmentMax + 1u);
	ocean_assert(bottomRows >= blackRingSegmentMin && bottomRows <= blackRingSegmentMax + 1u);

	return true;
}

bool MessengerCodeDetector::checkBullseyeInNeighborhood(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const float whiteRingRadius, const float blackRingRadius, const float whiteBorderRadius, const unsigned int framePaddingElements)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert_and_suppress_unused(width >= 21u && height >= 21u, height);

	ocean_assert(float(xCenter) + whiteBorderRadius + 0.5f < float(width));
	ocean_assert(float(xCenter) - whiteBorderRadius + 0.5f > 0.0f);

	ocean_assert(float(yCenter) + whiteBorderRadius + 0.5f < float(height));
	ocean_assert(float(yCenter) - whiteBorderRadius + 0.5f > 0.0f);

	ocean_assert(whiteRingRadius >= 1.0f);
	ocean_assert(whiteRingRadius + 1.0f <= blackRingRadius);
	ocean_assert(blackRingRadius + 1.0f <= whiteBorderRadius);

	constexpr unsigned int numberLookupLocations = 3u;

	const VectorT2<unsigned int> offsets1024[numberLookupLocations] =
	{
		VectorT2<unsigned int>(392u, 946u), // = (0.3826834323, 0.9238795325) * 1024, 22.5 deg
		VectorT2<unsigned int>(724u, 724u), // = (0.7071067811, 0.7071067811) * 1024, 45.0 deg
		VectorT2<unsigned int>(946u, 392u), // = (0.9238795325, 0.3826834323) * 1024, 67.5 deg
	};

	const unsigned int whiteRingRadius1024 = (unsigned int)(whiteRingRadius * 1024.0f + 0.5f);
	const unsigned int blackRingRadius1024 = (unsigned int)(blackRingRadius * 1024.0f + 0.5f);
	const unsigned int whiteBorderRadius1024 = (unsigned int)(whiteBorderRadius * 1024.0f + 0.5f);

	const unsigned int frameStrideElements = width + framePaddingElements;

	// inner white ring
	for (unsigned int n = 0u; n < numberLookupLocations; ++n)
	{
		if (yFrame[(yCenter - (whiteRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter - (whiteRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] < threshold)
			return false;

		if (yFrame[(yCenter + (whiteRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter - (whiteRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] < threshold)
			return false;

		if (yFrame[(yCenter - (whiteRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter + (whiteRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] < threshold)
			return false;

		if (yFrame[(yCenter + (whiteRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter + (whiteRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] < threshold)
			return false;
	}

	// black ring
	for (unsigned int n = 0u; n < numberLookupLocations; ++n)
	{
		if (yFrame[(yCenter - (blackRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter - (blackRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] > threshold)
			return false;

		if (yFrame[(yCenter + (blackRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter - (blackRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] > threshold)
			return false;

		if (yFrame[(yCenter - (blackRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter + (blackRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] > threshold)
			return false;

		if (yFrame[(yCenter + (blackRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter + (blackRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] > threshold)
			return false;
	}

	// outer white border
	for (unsigned int n = 0u; n < numberLookupLocations; ++n)
	{
		if (yFrame[(yCenter - (whiteBorderRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter - (whiteBorderRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] < threshold)
			return false;

		if (yFrame[(yCenter + (whiteBorderRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter - (whiteBorderRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] < threshold)
			return false;

		if (yFrame[(yCenter - (whiteBorderRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter + (whiteBorderRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] < threshold)
			return false;

		if (yFrame[(yCenter + (whiteBorderRadius1024 * offsets1024[n].y() + 524288u) / 1048576u) * frameStrideElements + xCenter + (whiteBorderRadius1024 * offsets1024[n].x() + 524288u) / 1048576u] < threshold)
			return false;
	}

	return true;
}

unsigned int MessengerCodeDetector::determineThreshold(const uint8_t* yPosition, const unsigned int segmentSize1, const unsigned int segmentSize2, const unsigned int segmentSize3, const unsigned int segmentSize4, const unsigned int segmentSize5)
{
	unsigned int sumBlack = 0u;
	unsigned int sumWhite = 0u;

	sumWhite += *(yPosition - 1);

	for (unsigned int n = 0u; n < segmentSize1; ++n)
		sumBlack += *yPosition++;

	for (unsigned int n = 0u; n < segmentSize2; ++n)
		sumWhite += *yPosition++;

	for (unsigned int n = 0u; n < segmentSize3; ++n)
		sumBlack += *yPosition++;

	for (unsigned int n = 0u; n < segmentSize4; ++n)
		sumWhite += *yPosition++;

	for (unsigned int n = 0u; n < segmentSize5; ++n)
		sumBlack += *yPosition++;

	sumWhite += *yPosition;

	const unsigned int averageBlack = sumBlack / (segmentSize1 + segmentSize3 + segmentSize5);
	const unsigned int averageWhite = sumWhite / (segmentSize2 + segmentSize4 + 2u);

	if (averageBlack + 2u >= averageWhite)
	{
		// the separate between bright and dark pixels is not strong enough
		return (unsigned int)(-1);
	}

	return (averageBlack + averageWhite + 1u) / 2u;
}

bool MessengerCodeDetector::determineAccurateBullseyeLocation(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int xBullseye, const unsigned int yBullseye, const unsigned int threshold, Vector2& location, const unsigned int framePaddingElements)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 21u && height >= 21u);
	ocean_assert(xBullseye < width && yBullseye < height);

	const unsigned int frameStrideElements = width + framePaddingElements;

	ocean_assert(yFrame[yBullseye * frameStrideElements + xBullseye] <= threshold);

	// first, we identify the first left, right, top, and bottom pixel that do not match with the threshold anymore

	unsigned int bullseyeLeft = xBullseye - 1u; // exclusive location

	while (bullseyeLeft < width && yFrame[yBullseye * frameStrideElements + bullseyeLeft] <= threshold)
	{
		--bullseyeLeft;
	}

	if (bullseyeLeft >= width)
	{
		return false;
	}

	unsigned int bullseyeRight = xBullseye + 1u; // exclusive location

	while (bullseyeRight < width && yFrame[yBullseye * frameStrideElements + bullseyeRight] <= threshold)
	{
		++bullseyeRight;
	}

	if (bullseyeRight >= width)
	{
		return false;
	}

	unsigned int bullseyeTop = yBullseye - 1u; // exclusive location

	while (bullseyeTop < height && yFrame[bullseyeTop * frameStrideElements + xBullseye] <= threshold)
	{
		--bullseyeTop;
	}

	if (bullseyeTop >= height)
	{
		return false;
	}

	unsigned int bullseyeBottom = yBullseye + 1u; // exclusive location

	while (bullseyeBottom < height && yFrame[bullseyeBottom * frameStrideElements + xBullseye] <= threshold)
	{
		++bullseyeBottom;
	}

	if (bullseyeBottom >= height)
	{
		return false;
	}

	// now, we determine the sub-pixel borders:
	// (out - in) / 1 == (out - threshold) / x
	// x = (out - threshold) / (out - in)

	// left border
	const float leftIn = float(yFrame[yBullseye * frameStrideElements + bullseyeLeft + 1u]);
	const float leftOut = float(yFrame[yBullseye * frameStrideElements + bullseyeLeft + 0u]);
	ocean_assert(leftIn <= float(threshold) && leftOut > float(threshold));

	ocean_assert(leftOut - leftIn >= NumericF::eps());
	const float leftBorder = float(bullseyeLeft) + (leftOut - float(threshold)) / (leftOut - leftIn);


	// right border
	const float rightIn = float(yFrame[yBullseye * frameStrideElements + bullseyeRight - 1u]);
	const float rightOut = float(yFrame[yBullseye * frameStrideElements + bullseyeRight + 0u]);
	ocean_assert(rightIn <= float(threshold) && rightOut > float(threshold));

	ocean_assert(rightOut - rightIn >= NumericF::eps());
	const float rightBorder = float(bullseyeRight) - (rightOut - float(threshold)) / (rightOut - rightIn);


	// top border
	const float topIn = float(yFrame[(bullseyeTop + 1u) * frameStrideElements + xBullseye]);
	const float topOut = float(yFrame[(bullseyeTop + 0u) * frameStrideElements + xBullseye]);
	ocean_assert(topIn <= float(threshold) && topOut > float(threshold));

	ocean_assert(topOut - topIn >= NumericF::eps());
	const float topBorder = float(bullseyeTop) + (topOut - float(threshold)) / (topOut - topIn);


	// bottom border
	const float bottomIn = float(yFrame[(bullseyeBottom - 1u) * frameStrideElements + xBullseye]);
	const float bottomOut = float(yFrame[(bullseyeBottom + 0u) * frameStrideElements + xBullseye]);
	ocean_assert(bottomIn <= float(threshold) && bottomOut > float(threshold));

	ocean_assert(bottomOut - bottomIn >= NumericF::eps());
	const float bottomBorder = float(bullseyeBottom) - (bottomOut - float(threshold)) / (bottomOut - bottomIn);

	ocean_assert(leftBorder <= float(xBullseye) && float(xBullseye) <= rightBorder);
	ocean_assert(topBorder <= float(yBullseye) && float(yBullseye) <= bottomBorder);

	location = Vector2(Scalar(leftBorder + rightBorder) * Scalar(0.5), Scalar(topBorder + bottomBorder) * Scalar(0.5));

	return true;
}

const Vectors2& MessengerCodeDetector::codeBitCoordinates()
{
	const static Vectors2 values(calculateBitCoordiantes());

	ocean_assert(values.size() == numberCodeBits);

	return values;
};

Vectors2 MessengerCodeDetector::calculateBitCoordiantes()
{
	Vectors2 coordinates;
	coordinates.reserve(260);

	calculateRingBitCoordinates(68u, IndexSet32({0u, 1u, 16u, 17u, 18u, 23u, 24u, 25u, 26u, 27u, 28u, 33u, 34u, 35u, 50u, 51u, 52u, 67u}), Scalar(0.783), coordinates); // 44/56 = 0.785714286, we chose slightly more correct
	calculateRingBitCoordinates(74u, IndexSet32({0u, 1u, 17u, 18u, 19u, 20u, 26u, 27u, 28u, 29u, 36u, 37u, 38u, 54u, 55u, 56u, 57u, 73u}), Scalar(0.855), coordinates); // 48/56 = 0.857142857, we chose slightly more correct
	calculateRingBitCoordinates(80u, IndexSet32({0u, 1u, 19u, 20u, 21u, 39u, 40u, 41u, 59u, 60u, 61u, 79u}), Scalar(52) / Scalar(56), coordinates);
	calculateRingBitCoordinates(86u, IndexSet32(), Scalar(1.0), coordinates);

	return coordinates;
}

void MessengerCodeDetector::calculateRingBitCoordinates(const unsigned int bits, const IndexSet32& bitsToSkip, const Scalar radius, Vectors2& coordinates)
{
	ocean_assert(bits >= 1u);
	ocean_assert(bitsToSkip.size() < (unsigned int)(bits));

	ocean_assert(radius > Numeric::eps());

	for (unsigned int n = 0u; n < bits; ++n)
	{
		if (bitsToSkip.find(n) == bitsToSkip.cend())
		{
			const Scalar angle = Scalar(n) * Numeric::pi2() / Scalar(bits);

			const Scalar x = Numeric::sin(angle) * radius;
			const Scalar y = -Numeric::cos(angle) * radius;

			coordinates.emplace_back(x, y);
		}
	}
}

}

}

}
