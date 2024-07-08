/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/base/Memory.h"

#include "ocean/cv/FrameFilterSobel.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

bool HarrisCornerDetector::PreciseCornerPosition::precisePosition(const unsigned int x, const unsigned int y, const int32_t strength, Scalar& preciseX, Scalar& preciseY, int32_t& preciseStrength)
{
	ocean_assert(frameData_ != nullptr);
	ocean_assert(frameWidth_ >= 10u);
	ocean_assert(x >= 3u && y >= 3u && x < frameWidth_ - 3u && y < frameHeight_ - 3u);

	// overall we need 3x3 Harris votes, which are based on 5x5 sobel responses

	const unsigned int frameStrideElements = frameWidth_ + framePaddingElements_;

	constexpr unsigned int numberSobelResponses = 8u;
	ocean_assert(numberSobelResponses + 2u <= frameWidth_);

	// although we need 5 sobel responses only (in horizontal direction), our functions below will at least create eight responses (in horizontal direction)
	const unsigned int firstSobelResponseLeft8 = min(x - 3u, frameWidth_ - numberSobelResponses - 1u);
	ocean_assert(firstSobelResponseLeft8 <= x - 3u);
	ocean_assert(firstSobelResponseLeft8 + numberSobelResponses < frameWidth_);

	const uint8_t* const yRowTop = frameData_ + (y - 2u) * frameStrideElements + firstSobelResponseLeft8;

	constexpr unsigned int responseBufferStride = numberSobelResponses + 1u; // +1: we add one additional response element as the NEON implementation below needs 4 instead of 3 responses

	int16_t responsesXX[responseBufferStride * 5u]; // all responses: 270 bytes
	int16_t responsesYY[responseBufferStride * 5u];
	int16_t responsesXY[responseBufferStride * 5u];

#ifdef OCEAN_DEBUG
	memset(responsesXX, 0x00, sizeof(responsesXX));
	memset(responsesYY, 0x00, sizeof(responsesYY));
	memset(responsesXY, 0x00, sizeof(responsesXY));
#endif

	for (unsigned int nRow = 0u; nRow < 5u; ++nRow)
	{
		ocean_assert(nRow + y - 2u >= 1u);
		ocean_assert(nRow + y - 2u < frameHeight_ - 1u);

		FrameFilterSobel::filterHorizontalVertical3Squared1Channel8BitRow(yRowTop + frameStrideElements * nRow, frameWidth_, numberSobelResponses, framePaddingElements_, responsesXX + responseBufferStride * nRow, responsesYY + responseBufferStride * nRow, responsesXY + responseBufferStride * nRow);
	}

#ifdef OCEAN_DEBUG
	for (unsigned int nRow = 0u; nRow < 5u; ++nRow)
	{
		// we ensure that our dummy +1 response is still zero (we did not overwrite the values)
		ocean_assert(responsesXX[nRow * responseBufferStride + 8u] == 0);
		ocean_assert(responsesYY[nRow * responseBufferStride + 8u] == 0);
		ocean_assert(responsesXY[nRow * responseBufferStride + 8u] == 0);
	}
#endif

	// we calculate the horizontal position of our 3 responses of interest (within our array of 8 responses)
	const unsigned int firstSobelResponseOffset = (x - 3u) - firstSobelResponseLeft8;
	ocean_assert(firstSobelResponseOffset <= 5u);

	int32_t harrisVotes[3][4]; // we actually need 3x3 votes, +1 for NEON

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	// the responses have one additional response element so that we cannot run of memory

	for (unsigned int nRow = 1u; nRow < 4u; ++nRow)
	{
		const int16_t* const topResponseXX = responsesXX + responseBufferStride * (nRow - 1u) + firstSobelResponseOffset;
		const int16_t* const topResponseYY = responsesYY + responseBufferStride * (nRow - 1u) + firstSobelResponseOffset;
		const int16_t* const topResponseXY = responsesXY + responseBufferStride * (nRow - 1u) + firstSobelResponseOffset;

		const int32x4_t sumsXX_32x4 = NEON::sum16Bit4Blocks3x3(topResponseXX + responseBufferStride * 0u, topResponseXX + responseBufferStride * 1u, topResponseXX + responseBufferStride * 2u);
		const int32x4_t sumsYY_32x4 = NEON::sum16Bit4Blocks3x3(topResponseYY + responseBufferStride * 0u, topResponseYY + responseBufferStride * 1u, topResponseYY + responseBufferStride * 2u);
		const int32x4_t sumsXY_32x4 = NEON::sum16Bit4Blocks3x3(topResponseXY + responseBufferStride * 0u, topResponseXY + responseBufferStride * 1u, topResponseXY + responseBufferStride * 2u);

		ocean_assert(nRow - 1u < 3u);
		determine4VotesNEON(sumsXX_32x4, sumsYY_32x4, sumsXY_32x4, harrisVotes[nRow - 1u]);
	}

#else

	for (unsigned int nRow = 1u; nRow < 4u; ++nRow)
	{
		for (unsigned int xS = firstSobelResponseOffset; xS < firstSobelResponseOffset + 3u; ++xS)
		{
			const int16_t* const topResponseXX = responsesXX + responseBufferStride * (nRow - 1u) + xS;
			const int16_t* const topResponseYY = responsesYY + responseBufferStride * (nRow - 1u) + xS;
			const int16_t* const topResponseXY = responsesXY + responseBufferStride * (nRow - 1u) + xS;

			const int32_t vote = harrisVote(topResponseXX + responseBufferStride * 0u, topResponseXX + responseBufferStride * 1u, topResponseXX + responseBufferStride * 2u,
												topResponseYY + responseBufferStride * 0u, topResponseYY + responseBufferStride * 1u, topResponseYY + responseBufferStride * 2u,
												topResponseXY + responseBufferStride * 0u, topResponseXY + responseBufferStride * 1u, topResponseXY + responseBufferStride * 2u);

			ocean_assert(nRow - 1u < 3u);
			ocean_assert(xS - firstSobelResponseOffset < 3u);

			harrisVotes[nRow - 1u][xS - firstSobelResponseOffset] = vote;
		}
	}

#endif

	const int32_t& vote00 = harrisVotes[0][0];
	const int32_t& vote01 = harrisVotes[0][1];
	const int32_t& vote02 = harrisVotes[0][2];

	const int32_t& vote10 = harrisVotes[1][0];
	const int32_t& vote11 = harrisVotes[1][1];
	const int32_t& vote12 = harrisVotes[1][2];

	const int32_t& vote20 = harrisVotes[2][0];
	const int32_t& vote21 = harrisVotes[2][1];
	const int32_t& vote22 = harrisVotes[2][2];

	ocean_assert(vote11 == strength); // we have determine this vote already when determining the pixel-precise location of this corner

	ocean_assert(vote11 >= vote00 && vote11 >= vote01 && vote11 >= vote02);
	ocean_assert(vote11 >= vote10 && vote11 > vote12);
	ocean_assert(vote11 >= vote20 && vote11 > vote21 && vote11 > vote22);

	preciseX = Scalar(x);
	preciseY = Scalar(y);

	// [-1 0 1] * 1/2
	const Scalar dx = Scalar(vote12 - vote10) * Scalar(0.5);
	const Scalar dy = Scalar(vote21 - vote01) * Scalar(0.5);

	// [1 -2 1] * 1/1
	const Scalar dxx = Scalar(vote12 + vote10 - vote11 * 2);
	const Scalar dyy = Scalar(vote21 + vote01 - vote11 * 2);

	// [ 1  0 -1 ]
	// [ 0  0  0 ] * 1/4
	// [-1  0  1 ]

	const Scalar dxy = Scalar(vote22 + vote00 - vote20 - vote02) * Scalar(0.25);

	const Scalar denominator = dxx * dyy - dxy * dxy;
	const Scalar factor = Numeric::isEqualEps(denominator) ? 0 : Scalar(1) / denominator;

	const Scalar offsetX = (dyy * dx - dxy * dy) * factor;
	const Scalar offsetY = (dxx * dy - dxy * dx) * factor;

	// check for invalid maximum
	if (offsetX >= -1 && offsetX <= 1 && offsetY >= -1 && offsetY <= 1)
	{
		// **TODO** do we need to add an explicit offset of (0.5, 0.5), due to the center of a pixel?
		preciseX = Scalar(x) - offsetX;
		preciseY = Scalar(y) - offsetY;
	}

	preciseStrength = strength;
	return true;
}

bool HarrisCornerDetector::detectCorners(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const unsigned int subFrameLeft, const unsigned int subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int threshold, const bool frameIsUndistorted, HarrisCorners& corners, const bool determineExactPosition, Worker* worker)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(threshold <= 512);
	ocean_assert(subFrameLeft + subFrameWidth <= width);
	ocean_assert(subFrameTop + subFrameHeight <= height);

	if (!yFrame || width < 10u || height < 7u || threshold > 512u || subFrameWidth < 10u || subFrameHeight < 7u || subFrameLeft + subFrameWidth > width || subFrameTop + subFrameHeight > height)
	{
		return false;
	}

	ocean_assert(corners.empty());

	Worker* useWorker = (worker && subFrameWidth * subFrameHeight >= 200u * 200u) ? worker : nullptr;

	const int32_t internalThreshold = determineInternalThreshold(threshold);
	const HarrisCorner::DistortionState distortionState = frameIsUndistorted ? HarrisCorner::DS_UNDISTORTED : HarrisCorner::DS_DISTORTED;

	NonMaximumSuppressionVote nonMaximumSuppression(width, subFrameHeight, subFrameTop);

	if (useWorker != nullptr)
	{
		useWorker->executeFunction(Worker::Function::createStatic(HarrisCornerDetector::detectCornerCandidatesSubset, yFrame, width, height, yFramePaddingElements, internalThreshold, &nonMaximumSuppression, subFrameLeft, subFrameWidth, 0u, 0u), subFrameTop, subFrameHeight, 8u, 9u, 10u);
	}
	else
	{
		detectCornerCandidatesSubset(yFrame, width, height, yFramePaddingElements, internalThreshold, &nonMaximumSuppression, subFrameLeft, subFrameWidth, subFrameTop, subFrameHeight);
	}

	ocean_assert(subFrameLeft + 3u < width);
	ocean_assert(subFrameWidth - 6u <= width && subFrameLeft + 3u + subFrameWidth - 6u <= width);
	ocean_assert(subFrameTop + 3u < height);
	ocean_assert(subFrameHeight - 6u <= height && subFrameTop + 3u + subFrameHeight - 6u <= height);

	if (determineExactPosition)
	{
		PreciseCornerPosition precisePositionObject(yFrame, width, height, yFramePaddingElements);

		const NonMaximumSuppressionVote::PositionCallback<Scalar, int32_t> callbackFunction(NonMaximumSuppressionVote::PositionCallback<Scalar, int32_t>::create(precisePositionObject, &PreciseCornerPosition::precisePosition));
		const NonMaximumSuppressionVote::StrengthPositions<Scalar, int32_t> strengthPositions(nonMaximumSuppression.suppressNonMaximum<Scalar, int32_t, false /*tStrictMaximum*/>(subFrameLeft + 3u, subFrameWidth - 6u, subFrameTop + 3u, subFrameHeight - 6u, useWorker, &callbackFunction));

		corners.reserve(strengthPositions.size());
		for (NonMaximumSuppressionVote::StrengthPositions<Scalar, int32_t>::const_iterator i = strengthPositions.begin(); i != strengthPositions.end(); ++i)
		{
			corners.emplace_back(*i, distortionState, Scalar(i->strength()));
		}
	}
	else
	{
		const NonMaximumSuppressionVote::StrengthPositions<unsigned int, int32_t> strengthPositions(nonMaximumSuppression.suppressNonMaximum<unsigned int, int32_t, false /*tStrictMaximum*/>(subFrameLeft + 3u, subFrameWidth - 6u, subFrameTop + 3u, subFrameHeight - 6u, useWorker, nullptr));

		corners.reserve(strengthPositions.size());
		for (NonMaximumSuppressionVote::StrengthPositions<unsigned int, int32_t>::const_iterator i = strengthPositions.begin(); i != strengthPositions.end(); ++i)
		{
			corners.emplace_back(Vector2(Scalar(i->x()), Scalar(i->y())), distortionState, Scalar(i->strength()));
		}
	}

	return true;
}

void HarrisCornerDetector::harrisVotesFrame(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, int32_t* votes, const unsigned int votesPaddingElements, Worker* worker, const bool setBorderPixels)
{
	ocean_assert(yFrame != nullptr && votes != nullptr);

	ocean_assert(width >= 10u && height >= 7u);
	if (!yFrame || !votes || width < 10u || height < 7u)
	{
		return;
	}

	Frame filterResponse(FrameType(width, height, FrameType::genericPixelFormat<int8_t, 2u>(), FrameType::ORIGIN_UPPER_LEFT));
	CV::FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int8_t, 1u>(yFrame, filterResponse.data<int8_t>(), width, height, yFramePaddingElements, filterResponse.paddingElements(), worker);

	harrisVotesFrameSobelResponse(filterResponse.constdata<int8_t>(), width, height, filterResponse.paddingElements(), votes, votesPaddingElements, worker, setBorderPixels);
}

void HarrisCornerDetector::harrisVotesFrameSobelResponse(const int8_t* sobelResponse, const unsigned int width, const unsigned int height, const unsigned int sobelResponsePaddingElements, int32_t* votes, const unsigned int votesPaddingElements, Worker* worker, const bool setBorderPixels)
{
	ocean_assert(sobelResponse != nullptr && votes != nullptr);
	ocean_assert(width >= 10u && height >= 5u);

	if (!sobelResponse || !votes || width < 10u || height < 5u)
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&harrisVotesByResponseSubset, sobelResponse, width, height, sobelResponsePaddingElements, votes, votesPaddingElements, 0u, 0u), 0u, height, 6u, 7u, 40u);
	}
	else
	{
		harrisVotesByResponseSubset(sobelResponse, width, height, sobelResponsePaddingElements, votes, votesPaddingElements, 0u, height);
	}

	if (setBorderPixels)
	{
		constexpr int32_t neutralResponse = 0;

#ifdef OCEAN_DEBUG
		Frame debugZeroFrame(FrameType(11u, 11u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		debugZeroFrame.setValue(0x00u);

		const int32_t debugNeutralResponse = harrisVotePixel(debugZeroFrame.constdata<uint8_t>(), debugZeroFrame.width(), 5u, 5u, debugZeroFrame.paddingElements());

		ocean_assert(neutralResponse == debugNeutralResponse);
#endif

		const unsigned int votesStrideElements = width + votesPaddingElements;

		// top 2 rows
		memset(votes, neutralResponse, sizeof(int32_t) * width);
		memset(votes + votesStrideElements, neutralResponse, sizeof(int32_t) * width);

		for (unsigned int y = 2u; y < height - 2u; ++y)
		{
			// first two pixels

			int32_t* const votesRow = votes + y * votesStrideElements;

			votesRow[0] = neutralResponse;
			votesRow[1] = neutralResponse;

			votesRow[width - 2u] = neutralResponse;
			votesRow[width - 1u] = neutralResponse;
		}

		// bottom 2 rows
		memset(votes + votesStrideElements * (height - 2u), neutralResponse, sizeof(int32_t) * width);
		memset(votes + votesStrideElements * (height - 1u), neutralResponse, sizeof(int32_t) * width);
	}
}

void HarrisCornerDetector::harrisVotes(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const PixelPosition* positions, const size_t numberPositions, int32_t* votes, Worker* worker)
{
	ocean_assert(yFrame != nullptr && positions != nullptr && votes != nullptr);

	ocean_assert(width >= 10u && height >= 5u);
	if (!yFrame || !votes || width < 10u || height < 5u)
	{
		return;
	}

	constexpr unsigned int filterResponsePaddingElements = 0u;

	Frame filterResponse(FrameType(width, height, FrameType::genericPixelFormat<int8_t, 2u>(), FrameType::ORIGIN_UPPER_LEFT), filterResponsePaddingElements);
	CV::FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int8_t, 1u>(yFrame, filterResponse.data<int8_t>(), width, height, yFramePaddingElements, filterResponse.paddingElements(), worker);

	const int8_t* const sobelResponse = filterResponse.constdata<int8_t>();

	for (size_t n = 0; n < numberPositions; ++n)
	{
		const PixelPosition& position = positions[n];

		ocean_assert((position.x() - 2u < width - 4u && position.y() - 2u < height - 4u) == (position.x() >= 2u && position.y() >= 2u && position.x() < width - 2u && position.y() < height - 2u));

		if (positions[n].x() - 2u < width - 4u && position.y() - 2u < height - 4u)
		{
			votes[n] = harrisVotePixel(sobelResponse + 2u * (position.y() * width + position.x()), width, filterResponsePaddingElements);
		}
		else
		{
			votes[n] = NumericT<int32_t>::minValue();
			ocean_assert(votes[n] < 0);
		}
	}
}

void HarrisCornerDetector::harrisVotesByResponseSubset(const int8_t* response, const unsigned int width, const unsigned int height, const unsigned int responsePaddingElements, int32_t* votes, const unsigned int votesPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(response && votes);
	ocean_assert(firstRow + numberRows <= height);

	const unsigned int responseStrideElements = width * 2u + responsePaddingElements;
	const unsigned int votesStrideElements = width + votesPaddingElements;

	const unsigned int beginResponseRow = max(0, int(firstRow) - 2) + 2;
	const unsigned int endResponseRow = min(firstRow + numberRows + 2, height) - 2;

	const int8_t* response0 = response + (beginResponseRow - 1) * responseStrideElements + 2u * 2u;
	const int8_t* response1 = response0 + responseStrideElements;
	const int8_t* response2 = response1 + responseStrideElements;

	int32_t* vote = votes + beginResponseRow * votesStrideElements + 2 - 1;

	const int8_t* const response0End = response + responseStrideElements * endResponseRow + 2u * 2u;

	while (response0 != response0End)
	{
		ocean_assert(response0 < response0End);
		ocean_assert((response0 - response) % responseStrideElements == 2u * 2u);

		const int8_t* const response0EndRow = response0 + responseStrideElements - 8u;

		while (response0 != response0EndRow)
		{
			ocean_assert(response0 < response0End);
			ocean_assert(response0 < response0EndRow);

			const uint32_t Ixx = sqr(*(response0 - 2)) + sqr(*(response0 + 0)) + sqr(*(response0 + 2))
									+ sqr(*(response1 - 2)) + sqr(*(response1 + 0)) + sqr(*(response1 + 2))
									+ sqr(*(response2 - 2)) + sqr(*(response2 + 0)) + sqr(*(response2 + 2));

			const uint32_t Iyy = sqr(*(response0 - 1)) + sqr(*(response0 + 1)) + sqr(*(response0 + 3))
									+ sqr(*(response1 - 1)) + sqr(*(response1 + 1)) + sqr(*(response1 + 3))
									+ sqr(*(response2 - 1)) + sqr(*(response2 + 1)) + sqr(*(response2 + 3));

			const int32_t Ixy = *(response0 - 2) * *(response0 - 1) + *(response0 + 0) * *(response0 + 1) + *(response0 + 2) * *(response0 + 3)
									+ *(response1 - 2) * *(response1 - 1) + *(response1 + 0) * *(response1 + 1) + *(response1 + 2) * *(response1 + 3)
									+ *(response2 - 2) * *(response2 - 1) + *(response2 + 0) * *(response2 + 1) + *(response2 + 2) * *(response2 + 3);

			const int32_t determinant = int32_t((Ixx / 8u) * (Iyy / 8u)) - int32_t(sqr(Ixy / 8));
			const uint32_t sqrTrace = sqr((Ixx + Iyy) / 8u);

			ocean_assert(NumericT<int32_t>::isInsideValueRange(int64_t(sqrTrace) * 3ll));
			ocean_assert(NumericT<int32_t>::isInsideValueRange(int64_t(determinant) - int64_t(sqrTrace) * 3ll / 64ll));

			*++vote = determinant - int32_t((sqrTrace * 3u) / 64u);

			response0 += 2;
			response1 += 2;
			response2 += 2;
		}

		response0 += responsePaddingElements + 8u;
		response1 += responsePaddingElements + 8u;
		response2 += responsePaddingElements + 8u;

		vote += votesPaddingElements + 4u;
	}
}

void HarrisCornerDetector::harrisVotesSubPixelSubset(const uint8_t* yFrame, const unsigned int width, const unsigned int yFramePaddingElements, const Vector2* positions, int32_t* votes, const unsigned int firstPosition, const unsigned int numberPositions)
{
	ocean_assert(yFrame != nullptr && width >= 7u);
	ocean_assert(positions && votes);

	for (unsigned int n = firstPosition; n < firstPosition + numberPositions; ++n)
	{
		votes[n] = harrisVoteSubPixel(yFrame, width, positions[n].x(), positions[n].y(), yFramePaddingElements);
	}
}

int32_t HarrisCornerDetector::harrisVotePixel(const uint8_t* yFrame, const unsigned int width, const unsigned int positionX, const unsigned int positionY, const unsigned int yFramePaddingElements)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 10u);

	ocean_assert(positionX >= 2u && positionX + 2u < width);
	ocean_assert(positionY >= 2u);

	const unsigned int strideElements = width + yFramePaddingElements;

	yFrame += strideElements * (positionY - 1u) + positionX - 1u;

	int32_t IxxIyyIxy[3 * 3 * 3];

	for (unsigned int y = 0u; y < 3u; y++)
	{
		for (unsigned int x = 0u; x < 3u; x++)
		{
			int32_t* const pointer = IxxIyyIxy + y * 9u + x * 3u;
			ocean_assert(pointer < IxxIyyIxy + 3 * 3 * 3);

			CV::FrameFilterSobel::filterPixelCoreHorizontalVertical3Squared1Channel8Bit<int32_t, 8>(yFrame + y * strideElements + x, width, pointer, yFramePaddingElements);
		}
	}

	// select the third response triple in each 'row' (due to the application of already verified code)
	const int32_t* r0 = IxxIyyIxy + 0u * 9u + 6u;
	const int32_t* r1 = IxxIyyIxy + 1u * 9u + 6u;
	const int32_t* r2 = IxxIyyIxy + 2u * 9u + 6u;

	const uint32_t Ixx = uint32_t(*(r0 - 6) + *(r0 - 3) + *(r0 + 0)
							+ *(r1 - 6) + *(r1 - 3) + *(r1 + 0)
							+ *(r2 - 6) + *(r2 - 3) + *(r2 + 0));

	const uint32_t Iyy = uint32_t(*(r0 - 5) + *(r0 - 2) + *(r0 + 1)
							+ *(r1 - 5) + *(r1 - 2) + *(r1 + 1)
							+ *(r2 - 5) + *(r2 - 2) + *(r2 + 1));

	const int32_t Ixy = *(r0 - 4) + *(r0 - 1) + *(r0 + 2)
							+ *(r1 - 4) + *(r1 - 1) + *(r1 + 2)
							+ *(r2 - 4) + *(r2 - 1) + *(r2 + 2);

	const int32_t determinant = int32_t((Ixx / 8u) * (Iyy / 8u)) - int32_t(sqr((Ixy / 8)));
	const uint32_t sqrTrace = sqr((Ixx + Iyy) / 8u);

	ocean_assert(NumericT<int32_t>::isInsideValueRange(int64_t(sqrTrace) * 3ll));
	ocean_assert(NumericT<int32_t>::isInsideValueRange(int64_t(determinant) - int64_t(sqrTrace) * 3ll));
	ocean_assert(NumericT<int32_t>::isInsideValueRange(int64_t(determinant) - int64_t(sqrTrace) * 3ll / 64ll));

	return determinant - int32_t((sqrTrace * 3u) / 64u);
}

int32_t HarrisCornerDetector::harrisVoteSubPixel(const uint8_t* yFrame, const unsigned int width, const Scalar x, const Scalar y, const unsigned int yFramePaddingElements)
{
	ocean_assert(yFrame != nullptr && width >= 7u);
	ocean_assert(x >= Scalar(3) && y >= Scalar(3) && x < Scalar(width - 3u));

	// the center of a pixel is located with an offset of (0.5, 0.5)

	const unsigned int left = (unsigned int)(x - Scalar(0.5));
	const unsigned int top = (unsigned int)(y - Scalar(0.5));

	const int32_t harrisTopLeft = harrisVotePixel(yFrame, width, left, top, yFramePaddingElements);
	const int32_t harrisTopRight = harrisVotePixel(yFrame, width, left + 1u, top, yFramePaddingElements);
	const int32_t harrisBottomLeft = harrisVotePixel(yFrame, width, left, top + 1u, yFramePaddingElements);
	const int32_t harrisBottomRight = harrisVotePixel(yFrame, width, left + 1u, top + 1u, yFramePaddingElements);

	const Scalar _tx = x - (Scalar(left) + Scalar(0.5));
	ocean_assert(_tx >= 0 && _tx <= 1);

	const Scalar _ty = y - (Scalar(top) + Scalar(0.5));
	ocean_assert(_ty >= 0 && _ty <= 1);

	const Scalar tx = Scalar(1) - _tx;
	const Scalar ty = Scalar(1) - _ty;

	const Scalar harrisTop = tx * Scalar(harrisTopLeft) + _tx * Scalar(harrisTopRight);
	const Scalar harrisBottom = tx * Scalar(harrisBottomLeft) + _tx * Scalar(harrisBottomRight);

	return Numeric::round32(ty * harrisTop + _ty * harrisBottom);
}

std::vector<int32_t> HarrisCornerDetector::harrisVotesSubPixel(const uint8_t* yFrame, const unsigned int width, const Vectors2& positions, const unsigned int yFramePaddingElements, Worker* worker)
{
	ocean_assert(yFrame != nullptr && width >= 7u);

	std::vector<int32_t> results(positions.size());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&HarrisCornerDetector::harrisVotesSubPixelSubset, yFrame, width, yFramePaddingElements, positions.data(), results.data(), 0u, 0u), 0u, (unsigned int)(positions.size()), 5u, 6u, 50u);
	}
	else
	{
		harrisVotesSubPixelSubset(yFrame, width, yFramePaddingElements, positions.data(), results.data(), 0u, (unsigned int)(positions.size()));
	}

	return results;
}

void HarrisCornerDetector::detectCornerCandidatesSubset(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const int32_t internalThreshold, NonMaximumSuppressionVote* nonMaximumSuppression, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(yFrame != nullptr && nonMaximumSuppression != nullptr);
	ocean_assert_and_suppress_unused(width >= 10u && height >= 7u, height);

	ocean_assert(firstRow + numberRows <= height);
	ocean_assert(firstColumn + numberColumns <= width);

	ocean_assert(firstRow >= nonMaximumSuppression->yOffset());
	ocean_assert(firstRow + numberRows <= nonMaximumSuppression->yOffset() + nonMaximumSuppression->height());

	const unsigned int frameStrideElements = width + yFramePaddingElements;

	const unsigned int responseCore = numberColumns - 2u;

	Memory responsesMemory = Memory::create<int16_t>((numberColumns - 2u) * 3u * 3u);

	int16_t* response0 = responsesMemory.data<int16_t>();
	int16_t* response1 = response0 + (numberColumns - 2u) * 3u;
	int16_t* response2 = response1 + (numberColumns - 2u) * 3u;

	const unsigned int beginHarrisRow = (unsigned int)(max(int(nonMaximumSuppression->yOffset()), int(firstRow) - 2)) + 2u;
	const unsigned int endHarrisRow = min(firstRow + numberRows + 2u, nonMaximumSuppression->height() + nonMaximumSuppression->yOffset()) - 2u;

	ocean_assert(beginHarrisRow < endHarrisRow);
	if (beginHarrisRow >= endHarrisRow)
	{
		return;
	}

	/// apply sobel filter for the first 2 responses

	// response for row0 (will be swapped in the loop)
	int16_t* response = response1;
	const uint8_t* yFrameRow = yFrame + (beginHarrisRow - 1u) * frameStrideElements + firstColumn;

	FrameFilterSobel::filterHorizontalVertical3Squared1Channel8BitRow(yFrameRow, width, responseCore, yFramePaddingElements, response + responseCore * 0u, response + responseCore * 1u, response + responseCore * 2u);

	// response for row1 (will be swapped in the loop)
	response = response2;
	yFrameRow = yFrame + beginHarrisRow * frameStrideElements + firstColumn;
	FrameFilterSobel::filterHorizontalVertical3Squared1Channel8BitRow(yFrameRow, width, responseCore, yFramePaddingElements, response + responseCore * 0u, response + responseCore * 1u, response + responseCore * 2u);

	unsigned int y = beginHarrisRow;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	int32_t votesNEON[4];
#endif

	yFrameRow = yFrame + (beginHarrisRow + 1u) * frameStrideElements + firstColumn;
	const uint8_t* const yFrameEnd = yFrame + (endHarrisRow + 1u) * frameStrideElements + firstColumn;
	while (yFrameRow != yFrameEnd)
	{
		assert(yFrameRow < yFrameEnd);

		/// response1 -> response0
		std::swap(response0, response1);
		/// response2 -> response1
		std::swap(response1, response2);
		/// response2 can be reused now

		FrameFilterSobel::filterHorizontalVertical3Squared1Channel8BitRow(yFrameRow, width, responseCore, yFramePaddingElements, response2 + responseCore * 0u, response2 + responseCore * 1u, response2 + responseCore * 2u);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		const unsigned int filterCore = numberColumns - 4u;

		for (unsigned int x = 0u; x < filterCore; x += 4u)
		{
			if (x + 4u > filterCore)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

				ocean_assert(x >= 4u && filterCore > 4u);
				const unsigned int newX = filterCore - 4u;

				ocean_assert(x > newX);

				x = newX;

				nonMaximumSuppression->removeCandidatesRightFrom(firstColumn + 2u + x, y);

				// the for loop will stop after this iteration
				ocean_assert(!(x + 4u < filterCore));
			}

			const int32x4_t sumsXX_32x4 = NEON::sum16Bit4Blocks3x3(response0 + responseCore * 0u + x, response1 + responseCore * 0u + x, response2 + responseCore * 0u + x);
			const int32x4_t sumsYY_32x4 = NEON::sum16Bit4Blocks3x3(response0 + responseCore * 1u + x, response1 + responseCore * 1u + x, response2 + responseCore * 1u + x);
			const int32x4_t sumsXY_32x4 = NEON::sum16Bit4Blocks3x3(response0 + responseCore * 2u + x, response1 + responseCore * 2u + x, response2 + responseCore * 2u + x);

			determine4VotesNEON(sumsXX_32x4, sumsYY_32x4, sumsXY_32x4, votesNEON);

			for (unsigned int n = 0u; n < 4u; ++n)
			{
				if (votesNEON[n] >= internalThreshold)
				{
					nonMaximumSuppression->addCandidate(firstColumn + 2u + x + n, y, votesNEON[n]);
				}
			}
		}

#else

		const unsigned int filterCore = numberColumns - 4u;

		for (unsigned int x = 0u; x < filterCore; ++x)
		{
			const int32_t vote = harrisVote(response0 + responseCore * 0u + x, response1 + responseCore * 0u + x, response2 + responseCore * 0u + x,
										response0 + responseCore * 1u + x, response1 + responseCore * 1u + x, response2 + responseCore * 1u + x,
										response0 + responseCore * 2u + x, response1 + responseCore * 2u + x, response2 + responseCore * 2u + x);

			if (vote >= internalThreshold)
			{
				nonMaximumSuppression->addCandidate(firstColumn + 2u + x, y, vote);
			}
		}

#endif

		yFrameRow += frameStrideElements;
		y++;
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

void HarrisCornerDetector::determine4VotesNEON(const int32x4_t& Ixx_s_32x4, const int32x4_t& Iyy_s_32x4, const int32x4_t& Ixy_s_32x4, int32_t* votes)
{
	ocean_assert(votes != nullptr);

	// determinant = (Ixx >> 3) * (Iyy >> 3) - sqr((Ixy >> 3));
	// sqrTrace = sqr((Ixx + Iyy) >> 3);
	// determinant - ((sqrTrace * 3) >> 6);

	const int32x4_t constant_s_three_32x4 = vdupq_n_s32(3);

	const int32x4_t Ixx_s3_32x4 = vrshrq_n_s32(Ixx_s_32x4, 3);
	const int32x4_t Iyy_s3_32x4 = vrshrq_n_s32(Iyy_s_32x4, 3);
	const int32x4_t Ixy_s3_32x4 = vrshrq_n_s32(Ixy_s_32x4, 3);

	const int32x4_t determinant_s_32x4 = vsubq_s32(vmulq_s32(Ixx_s3_32x4, Iyy_s3_32x4), vmulq_s32(Ixy_s3_32x4, Ixy_s3_32x4));

	const int32x4_t Ixx_yy_s3_32x4 = vaddq_s32(Ixx_s3_32x4, Iyy_s3_32x4);
	const int32x4_t sqrTrace_s_32x4 = vmulq_s32(Ixx_yy_s3_32x4, Ixx_yy_s3_32x4);

	const int32x4_t vote_s_32x4 = vsubq_s32(determinant_s_32x4, vrshrq_n_s32(vmulq_s32(sqrTrace_s_32x4, constant_s_three_32x4), 6));

	vst1q_s32(votes, vote_s_32x4);
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}

}
