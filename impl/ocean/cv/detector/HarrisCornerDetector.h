// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_DETECTOR_HARRIS_CORNER_DETECTOR_H
#define META_OCEAN_CV_DETECTOR_HARRIS_CORNER_DETECTOR_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/FeatureDetector.h"
#include "ocean/cv/detector/HarrisCorner.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/NEON.h"
#include "ocean/cv/NonMaximumSuppression.h"

#include "ocean/math/Vector2.h"

#include <numeric>

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class implements the Harris corner detector.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT HarrisCornerDetector
{
	public:

		/**
		 * Definition of a boolean enum for frame un-/distortion properties (to improve code readability).
		 */
		enum FrameDistortion
		{
			/// The provided frame is distorted so that all resulting feature locations are distorted.
			FD_FRAME_IS_DISTORTED = false,
			/// The provided frame is undistorted so that all resulting feature locations are also undistorted.
			FD_FRAME_IS_UNDISTORTED = true,
		};

		/**
		 * Definition of a boolean enum for precision properties (to improve code readability).
		 */
		enum PositionPrecision
		{
			/// The resulting position of the feature point will have a precision with pixel accuracy.
			PP_PIXEL_ACCURACY = false,
			/// The resulting position of the feature point will have a precision with sub-pixel accuracy.
			PP_SUBPIXEL_ACCURACY = true
		};

	private:

		/**
		 * Definition of a maximum suppression object holding integer strength parameters.
		 */
		typedef NonMaximumSuppression<int> NonMaximumSuppressionVote;

		/**
		 * This class implements a helper object allowing to determine the precise 2D position of Harris corners.
		 */
		class PreciseCornerPosition
		{
			public:

				/**
				 * Creates a new object.
				 * @param frame The 8 bit frame on which the Harris corners are detected, must be valid
				 * @param width The width of the given frame in pixel, with range [7, infinity)
				 * @param height The height of the given frame in pixel, with range [7, infinity)
				 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 */
				inline PreciseCornerPosition(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements);

				/**
				 * Determines the precise position of a given (rough) Harris corner.
				 * @param x Horizontal position of the Harris corner, with range [3, frameWidth - 4]
				 * @param y Vertical position of the Harris corner, with range [3, frameHeight - 4]
				 * @param strength The strength value of the Harris corner
				 * @param preciseX The resulting horizontal position of the Harris corner with sub-pixel accuracy, with range [0, frameWidth_)
				 * @param preciseY The resulting vertical position of the Harris corner with sub-pixel accuracy, with range [0, frameHeight_)
				 * @param preciseStrength The resulting strength value of the precise Harris corner
				 * @return True, if succeeded
				 */
				bool precisePosition(const unsigned int x, const unsigned int y, const int strength, Scalar& preciseX, Scalar& preciseY, int& preciseStrength);

			protected:

				/// The frame in which the Harris corners are detected.
				const uint8_t* const frameData_;

				/// The frame width in pixel, with range [7, infinity)
				const unsigned int frameWidth_;

				/// The frame height in pixel, with range [7, infinity)
				const unsigned int frameHeight_;

				/// The optional number of padding elements at the end of each row, with range [0, infinity)
				const unsigned int framePaddingElements_;
		};

	public:

		/**
		 * Detects Harris corners inside a given 8 bit grayscale image.
		 * @param yFrame The 8 bit grayscale frame in which the Harris corners will be detected, must be valid
		 * @param width The width of the frame in pixel, with range [10, infinity)
		 * @param height The height of the frame in pixel, with range [7, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param threshold Minimal strength value all detected corners must exceed to count as corner, with range [0, 512]
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param corners Resulting Harris corners detected inside the given frame
		 * @param determineExactPosition True, to force the subpixel interpolation to determine the exact corner position
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @return True, if succeeded
		 */
		static inline bool detectCorners(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const unsigned int threshold, const bool frameIsUndistorted, HarrisCorners& corners, const bool determineExactPosition = false, Worker* worker = nullptr);

		/**
		 * Detects Harris corners inside a sub-frame of a given 8 bit grayscale image.
		 * @param yFrame 8 bit grayscale frame to be used for corner detection with pixel origin in the upper left corner
		 * @param width The width of the frame in pixel, with range [10, infinity)
		 * @param height The height of the frame in pixel, with range [7, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param subFrameLeft Left position of the sub frame defined in the original image in pixel, with range [0, width - 7]
		 * @param subFrameTop Top position of the sub frame defined in the original image in pixel, with range [0, height - 7]
		 * @param subFrameWidth Width of the sub frame in pixel, with range [7, width - subFrameLeft]
		 * @param subFrameHeight Height of the sub frame in pixel, with range [7, height - subFrameTop]
		 * @param threshold Minimal strength value all detected corners must exceed to count as corner, with range [0, 512]
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param corners Resulting Harris corners detected inside the given frame
		 * @param determineExactPosition True, to force the subpixel interpolation to determine the exact corner position
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @return True, if succeeded
		 */
		static bool detectCorners(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const unsigned int subFrameLeft, const unsigned int subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int threshold, const bool frameIsUndistorted, HarrisCorners& corners, const bool determineExactPosition = false, Worker* worker = nullptr);

		/**
		 * Detects Harris corners inside a given frame.
		 * If the given frame is not an 8 bit grayscale frame with pixel origin in the upper left corner, the frame will be converted internally.
		 * @param frame The frame to be used for corner detection, with resolution [10, infinity)x[7, infinity), must be valid
		 * @param threshold Minimal strength value all detected corners must exceed to count as corner, with range [0, 512]
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param corners Resulting Harris corners detected inside the given frame
		 * @param determineExactPosition True, to force the subpixel interpolation to determine the exact corner position
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @return True, if succeeded
		 */
		static inline bool detectCorners(const Frame& frame, const unsigned int threshold, const bool frameIsUndistorted, HarrisCorners& corners, const bool determineExactPosition = false, Worker* worker = nullptr);

		/**
		 * Detects Harris corners inside a sub-region of a given frame.
		 * If the given frame is not an 8 bit grayscale frame with pixel origin in the upper left corner, the frame will be converted internally.
		 * @param frame The frame to be used for corner detection, with resolution [10, infinity)x[7, infinity), must be valid
		 * @param subFrameLeft Left position of the sub frame defined in the original image in pixel, with range [0, frame.width() - 7]
		 * @param subFrameTop Top position of the sub frame defined in the original image in pixel, with range [0, frame.height() - 7]
		 * @param subFrameWidth Width of the sub frame in pixel, with range [10, frame.width() - subFrameLeft]
		 * @param subFrameHeight Height of the sub frame in pixel, with range [7, frame.height() - subFrameTop]
		 * @param threshold Minimal strength value all detected corners must exceed to count as corner, with range [0, 512]
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param corners Resulting Harris corners detected inside the given frame
		 * @param determineExactPosition True, to force the subpixel interpolation to determine the exact corner position
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @return True, if succeeded
		 */
		static inline bool detectCorners(const Frame& frame, const unsigned int subFrameLeft, const unsigned int subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int threshold, const bool frameIsUndistorted, HarrisCorners& corners, const bool determineExactPosition = false, Worker* worker = nullptr);

		/**
		 * Creates the Harris corner votes for an entire frame (and therefore for each pixel) without applying a maximum suppression.
		 * The resulting votes may have an invalid 2 pixel wide frame border (depending on 'setBorderPixels').
		 * @param yFrame The 8 bit (grayscale) frame to be used for Harris application, with minimal size 7x7
		 * @param width The width of the given frame in pixel, with range [10, infinity)
		 * @param height The height of the given frame in pixel, with range [7, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param votes Resulting Harris votes values, make sure that the buffer is large enough
		 * @param votesPaddingElements The number of padding elements at the end of each votes row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @param setBorderPixels True, to set the border pixels to a neutral value; False, to keep random memory values
		 */
		static void harrisVotesFrame(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, int32_t* votes, const unsigned int votesPaddingElements, Worker* worker = nullptr, const bool setBorderPixels = false);

		/**
		 * Creates the Harris corner votes for the horizontal and vertical sobel responses for an entire frame (and therefore for each pixel of the original frame) without applying a maximum suppression.
		 * The resulting votes may have an invalid 2 pixel wide frame border (depending on 'setBorderPixels').
		 * @param sobelResponse 16 bit sobel filter responses (8 bit for the horizontal response and 8 bit for the vertical response) to be used for Harris application, with minimal size 5x5
		 * @param width The width of the original frame in pixel, with range [5, infinity)
		 * @param height The height of the original frame in pixel, with range [5, infinity)
		 * @param sobelResponsePaddingElements The number of padding elements at the end of each sobel response row, in elements, with range [0, infinity)
		 * @param votes Resulting Harris votes values, make sure that the buffer is large enough
		 * @param votesPaddingElements Optional padding at the end of each votes row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @param setBorderPixels True, to set the border pixels to a neutral value; False, to keep random memory values
		 */
		static void harrisVotesFrameSobelResponse(const int8_t* sobelResponse, const unsigned int width, const unsigned int height, const unsigned int sobelResponsePaddingElements, int32_t* votes, const unsigned int votesPaddingElements, Worker* worker = nullptr, const bool setBorderPixels = false);

		/**
		 * Calculates the Harris corner votes for several given positions in a frame only.
		 * @param yFrame The 8 bit (grayscale) frame to be used for Harris application, with minimal size 5x5
		 * @param width The width of the given frame in pixel, with range [10, infinity)
		 * @param height The height of the given frame in pixel, with range [7, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param positions Pixel positions inside the given frame to determine the responses for
		 * @param numberPositions Number of given pixel positions
		 * @param votes Resulting Harris votes values, one for each given position in the same order, make sure that enough buffer is available
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 */
		static void harrisVotes(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const PixelPosition* positions, const size_t numberPositions, int* votes, Worker* worker = nullptr);

		/**
		 * Calculates the Harris corner vote for one specific pixel from an 8 bit grayscale frame.
		 * @param yFrame The 8 bit grayscale frame that is used to determine the vote, must be valid
		 * @param width The width of the given frame in pixel, with range [10, infinity)
		 * @param x Horizontal position in pixel, with range [2, width - 2)
		 * @param y Vertical position in pixel, with range [2, height - 2)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @return Resulting Harris vote
		 */
		static int harrisVotePixel(const uint8_t* yFrame, const unsigned int width, const unsigned int x, const unsigned int y, const unsigned int yFramePaddingElements);

		/**
		 * Calculates the Harris corner vote for one specific sub-pixel position from an 8 bit grayscale frame.
		 * @param yFrame The 8 bit grayscale frame that is used to determine the vote, must be valid
		 * @param width The width of the given frame in pixel, with range [10, infinity)
		 * @param x Horizontal position in pixel, with range [3, width - 3)
		 * @param y Vertical position in pixel, with range [3, height - 3)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @return Resulting Harris vote
		 */
		static int harrisVoteSubPixel(const uint8_t* yFrame, const unsigned int width, const Scalar x, const Scalar y, const unsigned int yFramePaddingElements);

		/**
		 * Calculates the Harris corner votes for specified sub-pixel positions from an 8 bit grayscale frame.
		 * @param yFrame The 8 bit grayscale frame that is used to determine the vote
		 * @param width The width of the given frame in pixel, with range [10, infinity)
		 * @param positions The sub-pixel positions for which the Harris votes will be determined
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting Harris votes, one vote for each position
		 */
		static std::vector<int> harrisVotesSubPixel(const uint8_t* yFrame, const unsigned int width, const Vectors2& positions, const unsigned int yFramePaddingElements, Worker* worker = nullptr);

		/**
		 * Calculates one Harris Corner vote for one specific pixel from a frame storing sobel responses (Sx, Sy).
		 * @param sobelResponses The 2 x 8bit sobel filter responses (8 bit for the horizontal response and 8 bit for the vertical response) to be used for Harris application pointing to the pixel of interest
		 * @param width The width of the given response frame in pixel, with range [5, infinity)
		 * @param sobelResponsesPaddingElements The number of padding elements at the end of each response row, in elements, with range [0, infinity)
		 * @return Resulting Harris vote
		 */
		static inline int harrisVotePixel(const int8_t* sobelResponses, const unsigned int width, const unsigned int sobelResponsesPaddingElements);

		/**
		 * Calculates one Harris Corner vote for one specific pixel for a 3x3 region from a buffer storing interleaved squared sobel responses (Ixx, Iyy, Ixy).
		 * @param squaredSobelResponses 3x32 bit sobel filter responses (32 bit for the squared horizontal response, 32 bit for the square vertical response and 32 bit for the product of horizontal and vertical response) to be used for Harris application pointing to the pixel of interest
		 * @param width The width of the given response frame in pixel, with range [3, infinity)
		 * @param squaredSobelResponsesPaddingElements The number of padding elements at the end of each response row, in elements, with range [0, infinity)
		 * @return Resulting Harris vote
		 */
		static inline int harrisVotePixel(const int* squaredSobelResponses, const unsigned int width, const unsigned int squaredSobelResponsesPaddingElements);

		/**
		 * Calculates one Harris Corner vote for a 3x3 region from three buffers storing Sobel responses products Ixx, Iyy, and Ixy.
		 * @param responsesXX0 The first row of squared horizontal Sobel responses (Ixx), at least three, must be valid
		 * @param responsesXX1 The second row of squared horizontal Sobel responses (Ixx), at least three, must be valid
		 * @param responsesXX2 The third row of squared horizontal Sobel responses (Ixx), at least three, must be valid
		 * @param responsesYY0 The first row of squared vertical Sobel responses (Iyy), at least three, must be valid
		 * @param responsesYY1 The second row of squared vertical Sobel responses (Iyy), at least three, must be valid
		 * @param responsesYY2 The third row of squared vertical Sobel responses (Iyy), at least three, must be valid
		 * @param responsesXY0 The first row of products of horizontal and vertical Sobel responses (Ixy), at least three, must be valid
		 * @param responsesXY1 The second row of products of horizontal and vertical Sobel responses (Ixy), at least three, must be valid
		 * @param responsesXY2 The third row of products of horizontal and vertical Sobel responses (Ixy), at least three, must be valid
		 * @return Resulting Harris vote
		 */
		static inline int harrisVote(const int16_t* const responsesXX0, const int16_t* const responsesXX1, const int16_t* const responsesXX2, const int16_t* const responsesYY0, const int16_t* const responsesYY1, const int16_t* const responsesYY2, const int16_t* const responsesXY0, const int16_t* const responsesXY1, const int16_t* const responsesXY2);

		/**
		 * Returns the threshold used internally for a given threshold.
		 * Due to internal optimizations, we used an adjusted threshold internally.<br>
		 * <pre>
		 * internalThreshold = ((threshold ^ 2) / 8) ^ 2
		 * </pre>
		 * @param threshold The (external) threshold for which the internal threshold will be returned, with range [0, 512]
		 * @return The corresponding threshold which is used internally when comparing the Harris votes, with range [0, infinity)
		 * @see determineThreshold().
		 */
		static constexpr int32_t determineInternalThreshold(const unsigned int threshold);

		/**
		 * Determines the (external) threshold corresponding to an (internal) Harris vote.
		 * This function mainly determines the reverse of determineInternalThreshold().
		 * @param vote The (internal) Harris vote for which the corresponding threshold will be returned, with range (-infinity, infinity)
		 * @return The resulting threshold, with range [0, infinity)
		 * @tparam T The data type of the vote
		 * @see determineInternalThreshold().
		 */
		template <typename T>
		static inline T determineThreshold(const T vote);

	private:

		/**
		 * Determines Harris votes inside a sub-frame of a given buffer holding the filter responses.
		 * @param response Buffer holding the normalized 8 bit horizontal and vertical filter responses
		 * @param width The width of the source frame in pixel, with range [10, infinity)
		 * @param height The height of the source frame in pixel, with range [7, infinity)
		 * @param responsePaddingElements The number of padding elements at the end of each response row, in elements, with range [0, infinity)
		 * @param votes Buffer receiving the resulting corner votes
		 * @param votesPaddingElements The number of padding elements at the end of each votes row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1u, height - firstRow]
		 */
		static void harrisVotesByResponseSubset(const int8_t* response, const unsigned int width, const unsigned int height, const unsigned int responsePaddingElements, int* votes, const unsigned int votesPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Creates the Harris corner votes for a subset of specified sub-pixel positions from an 8 bit grayscale frame.
		 * @param yFrame The 8 bit grayscale frame that is used to determine the vote
		 * @param width The width of the given frame in pixel, with range [10, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param positions The sub-pixel positions for which the Harris votes will be determined
		 * @param votes The resulting votes, one vote for each position
		 * @param firstPosition The first position to handle
		 * @param numberPositions The number of positions to handle
		 */
		static void harrisVotesSubPixelSubset(const uint8_t* yFrame, const unsigned int width, const unsigned int yFramePaddingElements, const Vector2* positions, int* votes, const unsigned int firstPosition, const unsigned int numberPositions);

		/**
		 * Detects Harris corners inside a sub-frame of a given frame.
		 * @param yFrame Grayscale 8 bit frame, must be valid
		 * @param width The width of the source frame in pixel, with range [10, infinity)
		 * @param height The height of the source frame in pixel, with range [7, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param internalThreshold Minimal threshold a Harris corner must exceed, with range (-infinity, infinity)
		 * @param nonMaximumSuppression Non maximum suppression buffer holding all votes stronger than the specified threshold
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number o f columns to be handled
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 */
		static void detectCornerCandidatesSubset(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const int internalThreshold, NonMaximumSuppressionVote* nonMaximumSuppression, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Calculates four Harris Corner votes for 3x3 regions from sums of (squared) sobel responses.
		 * @param Ixx_s_32x4 The four individual sums of squared horizontal Sobel responses (Ixx)
		 * @param Iyy_s_32x4 The four individual sums of squared vertical Sobel responses (Iyy)
		 * @param Ixy_s_32x4 The four individual sums of products of horizontal and vertical Sobel responses (Ixy)
		 * @param votes The resulting four individual Harris corner votes, must be vlaid
		 */
		static void determine4VotesNEON(const int32x4_t& Ixx_s_32x4, const int32x4_t& Iyy_s_32x4, const int32x4_t& Ixy_s_32x4, int* votes);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Returns the square value.
		 * @param value The value to be squared
		 * @return Square value
		 */
		static constexpr int sqr(const int value);

		/**
		 * Returns the square value.
		 * @param value The value to be squared
		 * @return Square value
		 */
		static constexpr unsigned int sqr(const unsigned int value);

		/**
		 * Returns the square value.
		 * @param value The value to be squared
		 * @return Square value
		 */
		static constexpr long long sqr(const long long value);
};

inline HarrisCornerDetector::PreciseCornerPosition::PreciseCornerPosition(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements) :
	frameData_(frame),
	frameWidth_(width),
	frameHeight_(height),
	framePaddingElements_(framePaddingElements)
{
	ocean_assert(frameData_ != nullptr);
	ocean_assert(frameWidth_ >= 7u && frameHeight_ >= 7u);
}

inline bool HarrisCornerDetector::detectCorners(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const unsigned int threshold, const bool frameIsUndistorted, HarrisCorners& corners, const bool determineExactPosition, Worker* worker)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 10u && height >= 7u);

	return detectCorners(yFrame, width, height, yFramePaddingElements, 0u, 0u, width, height, threshold, frameIsUndistorted, corners, determineExactPosition, worker);
}

inline bool HarrisCornerDetector::detectCorners(const Frame& frame, const unsigned int threshold, const bool frameIsUndistorted, HarrisCorners& corners, const bool determineExactPosition, Worker* worker)
{
	ocean_assert(frame.isValid());

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	return detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), threshold, frameIsUndistorted, corners, determineExactPosition, worker);
}

inline bool HarrisCornerDetector::detectCorners(const Frame& frame, const unsigned int subFrameLeft, const unsigned int subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int threshold, const bool frameIsUndistorted, HarrisCorners& corners, const bool determineExactPosition, Worker* worker)
{
	ocean_assert(frame.isValid());

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	return detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, threshold, frameIsUndistorted, corners, determineExactPosition, worker);
}

inline int HarrisCornerDetector::harrisVotePixel(const int8_t* sobelResponse, const unsigned int width, const unsigned int sobelResponsesPaddingElements)
{
	ocean_assert(sobelResponse);
	ocean_assert(width >= 5u);

	const unsigned int sobelResponsesStrideElements = width * 2u + sobelResponsesPaddingElements;

	const int8_t* const response0 = sobelResponse - sobelResponsesStrideElements;
	const int8_t* const response1 = sobelResponse;
	const int8_t* const response2 = sobelResponse + sobelResponsesStrideElements;

	const unsigned int Ixx = (unsigned int)sqr(*(response0 - 2)) + (unsigned int)sqr(*(response0 + 0)) + (unsigned int)sqr(*(response0 + 2))
						+ (unsigned int)sqr(*(response1 - 2)) + (unsigned int)sqr(*(response1 + 0)) + (unsigned int)sqr(*(response1 + 2))
						+ (unsigned int)sqr(*(response2 - 2)) + (unsigned int)sqr(*(response2 + 0)) + (unsigned int)sqr(*(response2 + 2));

	const unsigned int Iyy = (unsigned int)sqr(*(response0 - 1)) + (unsigned int)sqr(*(response0 + 1)) + (unsigned int)sqr(*(response0 + 3))
						+ (unsigned int)sqr(*(response1 - 1)) + (unsigned int)sqr(*(response1 + 1)) + (unsigned int)sqr(*(response1 + 3))
						+ (unsigned int)sqr(*(response2 - 1)) + (unsigned int)sqr(*(response2 + 1)) + (unsigned int)sqr(*(response2 + 3));

	const int Ixy = *(response0 - 2) * *(response0 - 1) + *(response0 + 0) * *(response0 + 1) + *(response0 + 2) * *(response0 + 3)
						+ *(response1 - 2) * *(response1 - 1) + *(response1 + 0) * *(response1 + 1) + *(response1 + 2) * *(response1 + 3)
						+ *(response2 - 2) * *(response2 - 1) + *(response2 + 0) * *(response2 + 1) + *(response2 + 2) * *(response2 + 3);

	const int determinant = int((Ixx >> 3u) * (Iyy >> 3u)) - sqr((Ixy / 8));
	const unsigned int sqrTrace = sqr((Ixx + Iyy) >> 3u);

	ocean_assert(((long long)sqrTrace) * 3ll >= (long long)(NumericT<int>::minValue())
				&& ((long long)sqrTrace) * 3ll <= (long long)(NumericT<int>::maxValue()));

	return determinant - ((sqrTrace * 3u) >> 6u);
}

inline int HarrisCornerDetector::harrisVotePixel(const int* squaredSobelResponses, const unsigned int width, const unsigned int squaredSobelResponsesPaddingElements)
{
	ocean_assert(squaredSobelResponses);
	ocean_assert(width >= 3u);

	const unsigned int squaredSobelResponsesStrideElements = width * 3u + squaredSobelResponsesPaddingElements;

	const int* const response0 = squaredSobelResponses - squaredSobelResponsesStrideElements;
	const int* const response1 = squaredSobelResponses;
	const int* const response2 = squaredSobelResponses + squaredSobelResponsesStrideElements;

	const unsigned int Ixx = (unsigned int)*(response0 - 3) + (unsigned int)*(response0 + 0) + (unsigned int)*(response0 + 3)
						+ (unsigned int)*(response1 - 3) + (unsigned int)*(response1 + 0) + (unsigned int)*(response1 + 3)
						+ (unsigned int)*(response2 - 3) + (unsigned int)*(response2 + 0) + (unsigned int)*(response2 + 3);

	const unsigned int Iyy = (unsigned int)*(response0 - 2) + (unsigned int)*(response0 + 1) + (unsigned int)*(response0 + 4)
						+ (unsigned int)*(response1 - 2) + (unsigned int)*(response1 + 1) + (unsigned int)*(response1 + 4)
						+ (unsigned int)*(response2 - 2) + (unsigned int)*(response2 + 1) + (unsigned int)*(response2 + 4);

	const int Ixy = *(response0 - 1) + *(response0 + 2) + *(response0 + 5)
						+ *(response1 - 1) + *(response1 + 2) + *(response1 + 5)
						+ *(response2 - 1) + *(response2 + 2) + *(response2 + 5);

	const int determinant = int((Ixx >> 3u) * (Iyy >> 3u)) - sqr((Ixy / 8));
	const unsigned int sqrTrace = sqr((Ixx + Iyy) >> 3u);

	ocean_assert(((long long)sqrTrace) * 3ll >= (long long)(NumericT<int>::minValue())
				&& ((long long)sqrTrace) * 3ll <= (long long)(NumericT<int>::maxValue()));

	return determinant - ((sqrTrace * 3u) >> 6u);
}

inline int HarrisCornerDetector::harrisVote(const int16_t* const responsesXX0, const int16_t* const responsesXX1, const int16_t* const responsesXX2, const int16_t* const responsesYY0, const int16_t* const responsesYY1, const int16_t* const responsesYY2, const int16_t* const responsesXY0, const int16_t* const responsesXY1, const int16_t* const responsesXY2)
{
	ocean_assert(responsesXX0 != nullptr && responsesXX1 != nullptr && responsesXX2 != nullptr);
	ocean_assert(responsesYY0 != nullptr && responsesYY1 != nullptr && responsesYY2 != nullptr);
	ocean_assert(responsesXY0 != nullptr && responsesXY1 != nullptr && responsesXY2 != nullptr);

	ocean_assert(responsesXX0[0] >= 0 && responsesXX0[1] >= 0 && responsesXX0[2] >= 0);
	ocean_assert(responsesXX1[0] >= 0 && responsesXX1[1] >= 0 && responsesXX1[2] >= 0);
	ocean_assert(responsesXX2[0] >= 0 && responsesXX2[1] >= 0 && responsesXX2[2] >= 0);

	ocean_assert(responsesYY0[0] >= 0 && responsesYY0[1] >= 0 && responsesYY0[2] >= 0);
	ocean_assert(responsesYY1[0] >= 0 && responsesYY1[1] >= 0 && responsesYY1[2] >= 0);
	ocean_assert(responsesYY2[0] >= 0 && responsesYY2[1] >= 0 && responsesYY2[2] >= 0);

	const unsigned int Ixx = (unsigned int)(responsesXX0[0]) + (unsigned int)(responsesXX0[1]) + (unsigned int)(responsesXX0[2])
						+ (unsigned int)(responsesXX1[0]) + (unsigned int)(responsesXX1[1]) + (unsigned int)(responsesXX1[2])
						+ (unsigned int)(responsesXX2[0]) + (unsigned int)(responsesXX2[1]) + (unsigned int)(responsesXX2[2]);

	const unsigned int Iyy = (unsigned int)(responsesYY0[0]) + (unsigned int)(responsesYY0[1]) + (unsigned int)(responsesYY0[2])
						+ (unsigned int)(responsesYY1[0]) + (unsigned int)(responsesYY1[1]) + (unsigned int)(responsesYY1[2])
						+ (unsigned int)(responsesYY2[0]) + (unsigned int)(responsesYY2[1]) + (unsigned int)(responsesYY2[2]);

	const int Ixy = (unsigned int)(responsesXY0[0]) + (unsigned int)(responsesXY0[1]) + (unsigned int)(responsesXY0[2])
						+ (unsigned int)(responsesXY1[0]) + (unsigned int)(responsesXY1[1]) + (unsigned int)(responsesXY1[2])
						+ (unsigned int)(responsesXY2[0]) + (unsigned int)(responsesXY2[1]) + (unsigned int)(responsesXY2[2]);

	const int determinant = int((Ixx >> 3u) * (Iyy >> 3u)) - sqr((Ixy / 8));
	const unsigned int sqrTrace = sqr((Ixx >> 3u) + (Iyy >> 3u));

	ocean_assert(((long long)sqrTrace) * 3ll >= (long long)(NumericT<int>::minValue())
				&& ((long long)sqrTrace) * 3ll <= (long long)(NumericT<int>::maxValue()));

	return determinant - int((sqrTrace * 3u) >> 6u);
}

constexpr int32_t HarrisCornerDetector::determineInternalThreshold(const unsigned int threshold)
{
	ocean_assert(threshold <= 512u);

	return int32_t(sqr(threshold * threshold / 8u));
}

template <>
inline float HarrisCornerDetector::determineThreshold(const float vote)
{
	return NumericF::pow(NumericF::abs(float(vote)) * 64.0f, 0.25f);
}

template <>
inline double HarrisCornerDetector::determineThreshold(const double vote)
{
	return NumericD::pow(NumericD::abs(double(vote)) * 64.0, 0.25);
}

template <typename T>
inline T HarrisCornerDetector::determineThreshold(const T vote)
{
	static_assert(!std::is_floating_point<T>::value, "Invalid data type!");

	const double result = determineThreshold<double>(double(vote));
	ocean_assert(result <= double(NumericT<T>::maxValue()));

	return T(result);
}

constexpr int HarrisCornerDetector::sqr(const int value)
{
	return value * value;
}

constexpr unsigned int HarrisCornerDetector::sqr(const unsigned int value)
{
	return value * value;
}

constexpr long long HarrisCornerDetector::sqr(const long long value)
{
	return value * value;
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_HARRIS_CORNER_DETECTOR_H
