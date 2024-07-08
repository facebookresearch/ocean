/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Memory.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/math/Matrix.h"

#include <functional>
#include <numeric>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a frame converter test for the function of the basic frame converter.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameConverter : protected CV::FrameConverter
{
	public:

		/**
		 * This class is a helper class simply offering random values which are constant during process execution.
		 */
		class ValueProvider : public Singleton<ValueProvider>
		{
			friend class Singleton<ValueProvider>;

			public:

				/**
				 * Returns the random (but constant during process execution) alpha value.
				 * @return The alpha value, with range [0, infinity)
				 */
				inline uint8_t alphaValue() const;

				/**
				 * Returns the random (but constant during process execution) gamma value.
				 * @return The gamma value, with range [0.4, 1.0]
				 */
				inline float gammaValue() const;

			protected:

				/**
				 * Protected constructor.
				 */
				inline ValueProvider();

			protected:

				/// The alpha value.
				uint8_t alphaValue_;

				/// The gamma value.
				float gammaValue_;
		};

		/**
		 * This class is a wrapper for function pointers.
		 */
		class FunctionWrapper
		{
			public:

				/**
				 * Definition of individual types of conversion functions.
				 */
				enum FunctionType : uint32_t
				{
					/// And invalid function type.
					FT_INVALID = 0u,
					/// 1-plane uint8 to 1-plane uint8 conversion function.
					FT_1_UINT8_TO_1_UINT8,
					/// 1-plane uint8 to 1-plane uint16 conversion function.
					FT_1_UINT8_TO_1_UINT16,
					/// 1-plane uint8 plus constant gamma to 1-plane uint8 conversion function.
					FT_1_UINT8_GAMMA_TO_1_UINT8,
					/// 1-plane uint8 to 1-plane plus constant alpha uint8 conversion function.
					FT_1_UINT8_TO_1_UINT8_ALPHA,
					/// 1-plane uint8 to 1-plane uint8 plus constant black level, white balance, and gamma conversion function.
					FT_1_UINT8_TO_1_UINT8_BLACKLEVEL_WHITEBALANCE_GAMMA,
					/// 1-plane uint16 to 1-plane uint8 conversion function.
					FT_1_UINT16_TO_1_UINT8,
					/// 1-plane uint16 to 1-plane uint16 conversion function.
					FT_1_UINT16_TO_1_UINT16,
					/// 1-plane uint32 to 1-plane uint8 conversion function.
					FT_1_UINT32_TO_1_UINT8,
					/// 1-plane uint32 to 1-plane uint16 conversion function.
					FT_1_UINT32_TO_1_UINT16,
					/// 1-plane uint8 to 2-plane uint8 conversion function.
					FT_1_UINT8_TO_2_UINT8,
					/// 1-plane uint8 to 3-plane uint8 conversion function.
					FT_1_UINT8_TO_3_UINT8,
					/// 2-plane uint8 to 1-plane uint8 conversion function.
					FT_2_UINT8_TO_1_UINT8_ALPHA,
					/// 2-plane uint8 to 1-plane plus constant alpha uint8 conversion function.
					FT_2_UINT8_TO_1_UINT8,
					/// 2-plane uint8 to 3-plane plus constant alpha uint8 conversion function.
					FT_2_UINT8_TO_3_UINT8,
					/// 3-plane uint8 to 1-plane uint8 conversion function.
					FT_3_UINT8_TO_1_UINT8,
					/// 3-plane uint8 to 3-plane uint8 conversion function.
					FT_3_UINT8_TO_3_UINT8,
					/// 3-plane uint8 to 1-plane plus constant alpha uint8 conversion function.
					FT_3_UINT8_TO_1_UINT8_ALPHA
				};

				/**
				 * Definition of a function pointer to a conversion function with one source plane and one target plane.
				 */
				template <typename TSource, typename TTarget>
				using OneSourceOneTargetConversionFunction = void(*)(const TSource* source, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with one source plane and one target plane plus constant alpha.
				 */
				template <typename TSource, typename TTarget>
				using OneSourceGammaOneTargetConversionFunction = void(*)(const TSource* source, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const float gamma, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with one source plane and one target plane plus constant alpha.
				 */
				template <typename TSource, typename TTarget>
				using OneSourceOneTargetAlphaConversionFunction = void(*)(const TSource* source, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements, const TTarget alpha, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with one source plane and one target plane plus constant black level, white balance, and gamma.
				 */
				template <typename TSource, typename TTarget>
				using OneSourceOneTargetBlackLevelWhiteBalanceGammaConversionFunction = void(*)(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const uint16_t blackLevel, const float* whiteBalance, const float gamma, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with one source plane and two target planes.
				 */
				template <typename TSource, typename TTarget>
				using OneSourceTwoTargetsConversionFunction = void(*)(const TSource* source, TTarget* target0, TTarget* target1, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t sourcePaddingElements, const uint32_t target0PaddingElements, const uint32_t target1PaddingElements, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with one source plane and three target planes.
				 */
				template <typename TSource, typename TTarget>
				using OneSourceThreeTargetsConversionFunction = void(*)(const TSource* source, TTarget* target0, TTarget* target1, TTarget* target2, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t sourcePaddingElements, const uint32_t target0PaddingElements, const uint32_t target1PaddingElements, const uint32_t target2PaddingElements, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with two source planes and one target plane.
				 */
				template <typename TSource, typename TTarget>
				using TwoSourcesOneTargetConversionFunction = void(*)(const TSource* source0, const TSource* source1, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t targetPaddingElements, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with two source planes and three target planes.
				 */
				template <typename TSource, typename TTarget>
				using TwoSourcesThreeTargetConversionFunction = void(*)(const TSource* source0, const TSource* source1, TTarget* target0, TTarget* target1, TTarget* target2, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t targetPaddingElements0, const uint32_t targetPaddingElements1, const uint32_t targetPaddingElements2, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with two source planes and one target plane plus constant alpha.
				 */
				template <typename TSource, typename TTarget>
				using TwoSourcesOneTargetAlphaConversionFunction = void(*)(const TSource* source0, const TSource* source1, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t targetPaddingElements, const TTarget alpha, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with three source planes and one target plane.
				 */
				template <typename TSource, typename TTarget>
				using ThreeSourcesOneTargetConversionFunction = void(*)(const TSource* source0, const TSource* source1, const TSource* source2, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t source2PaddingElements, const uint32_t targetPaddingElements, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with three source planes and three target planes.
				 */
				template <typename TSource, typename TTarget>
				using ThreeSourcesThreeTargetConversionFunction = void(*)(const TSource* source0, const TSource* source1, const TSource* source2, TTarget* target0, TTarget* target1, TTarget* target2, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t source2PaddingElements, const uint32_t targetPaddingElements0, const uint32_t targetPaddingElements1, const uint32_t targetPaddingElements2, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with three source planes and one target plane plus constant alpha.
				 */
				template <typename TSource, typename TTarget>
				using ThreeSourcesOneTargetAlphaConversionFunction = void(*)(const TSource* source0, const TSource* source1, const TSource* source2, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t source2PaddingElements, const uint32_t targetPaddingElements, const TTarget alpha, Worker* worker);

			public:

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT8_TO_1_UINT8 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceOneTargetConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT8_TO_1_UINT16 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceOneTargetConversionFunction<uint8_t, uint16_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT8_GAMMA_TO_1_UINT8 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceGammaOneTargetConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT8_TO_1_UINT8_ALPHA function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceOneTargetAlphaConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT8_TO_1_UINT8_BLACKLEVEL_WHITEBALANCE_GAMMA function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceOneTargetBlackLevelWhiteBalanceGammaConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT16_TO_1_UINT8 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceOneTargetConversionFunction<uint16_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT16_TO_1_UINT16 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceOneTargetConversionFunction<uint16_t, uint16_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT32_TO_1_UINT8 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceOneTargetConversionFunction<uint32_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT32_TO_1_UINT16 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceOneTargetConversionFunction<uint32_t, uint16_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT8_TO_2_UINT8 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceTwoTargetsConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT8_TO_3_UINT8 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const OneSourceThreeTargetsConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_2_UINT8_TO_1_UINT8 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const TwoSourcesOneTargetConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_2_UINT8_TO_3_UINT8 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const TwoSourcesThreeTargetConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_2_UINT8_TO_1_UINT8_ALPHA function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const TwoSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_3_UINT8_TO_1_UINT8 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const ThreeSourcesOneTargetConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_3_UINT8_TO_3_UINT8 function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const ThreeSourcesThreeTargetConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Creates a new wrapper object and stores a function pointer to a FT_3_UINT8_TO_1_UINT8_ALPHA function.
				 * @param function The pointer to the conversion function, must be valid
				 */
				FunctionWrapper(const ThreeSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> function);

				/**
				 * Calls the conversion function for a source and target frame.
				 * @param source The source frame, must be valid
				 * @param target The target frame, must be valid
				 * @param conversionFlag The conversion flag to be used
				 * @param options Optional parameters that can be provided to the wrapped conversion function, must be `nullptr` to be ignored or be valid and have the expected number of elements for that specific function
				 * @param worker Optional worker object to be used
				 * @return True, if succeeded
				 */
				bool invoke(const Frame& source, Frame& target, const FrameConverter::ConversionFlag conversionFlag, const void* options, Worker* worker) const;

			protected:

				/// The function pointer of the conversion function.
				const void* function_;

				/// The type of the conversion function.
				const FunctionType functionType_;
		};

	public:

		/**
		 * Definition of a function pointer to a pixel extraction function.
		 * @param frame The frame from which the pixel will be extracted, must be valid
		 * @param x The horizontal pixel location within the frame, with range [0, width - 1]
		 * @param y The vertical pixel location within the frame, with range [0, height - 1]
		 * @param conversionFlag The conversion flag that will be applied, must be valid
		 * @return The pixel color values, a vector for a trivial case; a matrix in case each channel needs to be handled individually
		 */
		using FunctionPixelValue = std::function<MatrixD(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)>;

	public:

		/**
		 * Tests all frame converter functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if all tests succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the comfort convert function for a Frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComfortConvert(const double testDuration);

		/**
		 * Tests the comfort convert and copy function for a Frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComfortConvertAndCopy(const double testDuration);

		/**
		 * Tests the comfort change function for a Frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComfortChange(const double testDuration);

		/**
		 * Tests the cast function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCast(const double testDuration);

		/**
		 * Tests the normalized cast function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNormalizedCast(const double testDuration);

		/**
		 * Tests the sub frame function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSubFrame(const double testDuration);

		/**
		 * Tests the sub frame function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSubFrameMask(const double testDuration);

		/**
		 * Tests the patch creator.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchFrame(const double testDuration);

		/**
		 * Tests the patch creator with mirrored border.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchFrameMirroredBorder(const double testDuration);

		/**
		 * Test the 1-row-based converter for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3, with 6 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration);

		/**
		 * Test the 1-row-based converter for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3, with 10 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3, with 10 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3, with 10 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3, with 7 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 3 planes and a 2x2 downsampling of channel 2 and 3, with 7 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 3 planes and 1 channel to 1 plane and 3 channels.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 3 channels, 1 planes and a 2x1 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 3 channels, 1 planes and a 2x1 downsampling of channel 1 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 2-row-based mapper for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based converter for pixel formats with 3 channels, 3 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 3 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 3 planes and a 2x2 downsampling of channel 2 and 3 adding a new target channel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 3 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 2-row-based mapper for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based convert 3-plane to zipped 3-channel function, with 6 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration);

		/**
		 * Test the 1-row-based convert zipped 3-channel with 2x1 downsampling to 3-channel function, with 10 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Test the 1-row-based convert zipped 3-channel with 2x1 downsampling to 3-channel function, with 10 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Tests the conversion of frames with one pixel format to another pixel format for functions supporting padding.
		 * @param sourcePixelFormat The pixel format of the source frame, must be valid
		 * @param targetPixelFormat The pixel format of the target frame, must be valid
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param functionWrapper The wrapper around the conversion function to be tested, must be valid
		 * @param conversionFlag The conversion type to be used
		 * @param functionSourcePixelValue The function pointer which to extract one pixel from the source image, must be valid
		 * @param functionTargetPixelValue The function pointer which to extract one pixel from the target image, must be valid
		 * @param transformationMatrix The transformation matrix defining the conversion (an affine transformation)
		 * @param minimalGroundTruthValue The minimal ground truth value for value clamping, with range (-infinity, maximalGroundTruthValue)
		 * @param maximalGroundTruthValue The maximal ground truth value for value clamping, with range (minimalGroundTruthValue, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param thresholdMaximalErrorToInteger The maximal allowed error between the ground truth integer and the resulting integer value, with range [0, infinity)
		 * @param options Optional parameters that will be provided to the function wrapper when it is invoked, must be `nullptr` to be ignored or be valid and have the expected number of elements for that specific function
		 * @return True, if succeeded
		 */
		static bool testFrameConversion(const FrameType::PixelFormat& sourcePixelFormat, const FrameType::PixelFormat& targetPixelFormat, const unsigned int width, const unsigned int height, const FunctionWrapper& functionWrapper, const CV::FrameConverter::ConversionFlag conversionFlag, const FunctionPixelValue functionSourcePixelValue, const FunctionPixelValue functionTargetPixelValue, const MatrixD& transformationMatrix, const double minimalGroundTruthValue, const double maximalGroundTruthValue, const double testDuration, Worker& worker, const unsigned int thresholdMaximalErrorToInteger = 3u, const void* options = nullptr);

		/**
		 * Tests the color space conversion matrices.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConversionMatrices(const double testDuration);

		/**
		 * Validates the color space conversion from a source pixel format to a target pixel format.
		 * @param sourceFrame The source frame, must be valid
		 * @param targetFrame The target frame, must be valid
		 * @param functionSourcePixelValue The function pointer which to extract one pixel from the source image, must be valid
		 * @param functionTargetPixelValue The function pointer which to extract one pixel from the target image, must be valid
		 * @param transformationMatrix The transformation matrix defining the conversion (an affine transformation)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param averageAbsErrorToFloat Resulting average absolute error between the converted result and the ground truth floating point result, with range [0, 256)
		 * @param averageAbsErrorToInteger Resulting average absolute error between the converted result and the ground truth integer result (rounded result), with range [0, 256)
		 * @param maximalAbsErrorToFloat maximal absolute error between the converted result and the ground truth floating point result, with range [0, 256)
		 * @param maximalAbsErrorToInteger Resulting maximal absolute error between the converted result and the ground truth integer result (rounded result), with range [0, 256)
		 * @param minimalGroundTruthValue The minimal ground truth value for value clamping, with range (-infinity, maximalGroundTruthValue)
		 * @param maximalGroundTruthValue The maximal ground truth value for value clamping, with range (minimalGroundTruthValue, infinity)
		 * @param skipPlausibilityCheck True, to skip the plausibility check ensuring that the resulting pixel values are in a plausible value range; False, to apply the plausibility check
		 * @return True, if succeeded
		 */
		static bool validateConversion(const Frame& sourceFrame, const Frame& targetFrame, const FunctionPixelValue functionSourcePixelValue, const FunctionPixelValue functionTargetPixelValue, const MatrixD& transformationMatrix, const CV::FrameConverter::ConversionFlag conversionFlag, double* averageAbsErrorToFloat, double* averageAbsErrorToInteger, double* maximalAbsErrorToFloat, unsigned int* maximalAbsErrorToInteger, const double minimalGroundTruthValue, const double maximalGroundTruthValue, const bool skipPlausibilityCheck = false);

		/**
		 * Extracts one pixel from a generic frame (e.g,. with pixel format BGR24, RGB24, YUV24, ...).
		 * @param frame The frame from which the pixel will be extracted, must be valid
		 * @param x The horizontal pixel location within the frame, with range [0, frame.width() - 1]
		 * @param y The vertical pixel location within the frame, with range [0, frame.height() - 1]
		 * @param conversionFlag The conversion flag that will be applied, must be valid
		 * @return The vector holding the frame's color value at the specified location
		 */
		static MatrixD functionGenericPixel(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag);

	protected:

		/**
		 * Tests the sub frame function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the elements to be used
		 */
		template <typename T>
		static bool testSubFrame(const double testDuration);

		/**
		 * Tests the cast function for from 'unsigned char' to a specified data type.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param channels The number of channels the frame has, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the target frame
		 */
		template <typename T>
		static bool testCast(const unsigned int width, const unsigned int height, const unsigned int channels);

		/**
		 * Tests the cast function for from 'unsigned char' to a specified data type.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param channels The number of channels the frame has, with range [1, infinity)
		 * @param normalization The normalization parameter that is applied
		 * @param offset The offset that is added to all values
		 * @return True, if succeeded
		 * @tparam T The data type of the target frame
		 */
		template <typename T>
		static bool testNormalizedCast(const unsigned int width, const unsigned int height, const unsigned int channels, const T normalization, const T offset);

		/**
		 * Tests the patch creator.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The element type to be used
		 */
		template <typename T>
		static bool testPatchFrame(const double testDuration);

		/**
		 * Tests the patch creator with mirrored border.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The element type to be used
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool testPatchFrameMirroredBorder(const double testDuration);

		/**
		 * Validates the sub-frame function.
		 * @param channels Number of data channels of both frames, with range [1, infinity)
		 * @param source The source frame from which the image content has been copied, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param target The target frame to which the image content has been copied, must be valid
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourceLeft Horizontal start position of the sub-frame within the source frame, with range [0, sourceWidth - 1]
		 * @param sourceTop Vertical start position of the sub-frame within the source frame, with range [0, sourceHeight - 1]
		 * @param targetLeft Horizontal start position of the sub-frame within the target frame, with range [0, targetWidth - 1]
		 * @param targetTop Vertical start position of the sub-frame within the target frame, with range [0, targetHeight - 1]
		 * @param width The width of the sub-frame in pixel, with range [1, min(sourceWidth - sourceLeft, targetWidth - targetLeft)]
		 * @param height The height of the sub-frame in pixel, with range [1, min(sourceHeight - sourceTop, targetHeight - targetTop)]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each element
		 */
		template <typename T>
		static bool validateSubFrame(const unsigned int channels, const T* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const T* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the sub-frame function supporting a mask.
		 * @param channels Number of data channels of both frames, with range [1, infinity)
		 * @param source The source frame from which the image content has been copied, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param originalTarget The copy of the target frame BEFORE the image content has been copied to it, must be valid
		 * @param target The target frame to which the image content has been copied, must be valid
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param mask The binary mask that is used to indicate which source pixels to copy to the target frame, must be valid, have one channel, and have the same size as the source frame
		 * @param sourceLeft Horizontal start position of the sub-frame within the source frame, with range [0, sourceWidth - 1]
		 * @param sourceTop Vertical start position of the sub-frame within the source frame, with range [0, sourceHeight - 1]
		 * @param targetLeft Horizontal start position of the sub-frame within the target frame, with range [0, targetWidth - 1]
		 * @param targetTop Vertical start position of the sub-frame within the target frame, with range [0, targetHeight - 1]
		 * @param subFrameWidth Width of the sub-frame in pixel, with range [1, min(sourceWidth - sourceLeft, targetWidth - targetLeft)]
		 * @param subFrameHeight Height of the sub-frame in pixel, with range [1, min(sourceHeight - sourceTop, targetHeight - targetTop)]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, with range [0, infinity)
		 * @param maskPaddingElements Optional number of padding elements at the end of each source mask row, with range [0, infinity)
		 * @param maskValue Optional value which indicates which pixel value should be interpreted as the foreground (and copied)
		 * @return True, if succeeded
		 * @tparam T The data type of each element
		 */
		template <typename T>
		static bool validateSubFrameMask(const unsigned int channels, const T* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const T* originalTarget, const T* target, const unsigned int targetWidth, const unsigned int targetHeight, const uint8_t* mask, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int maskPaddingElements, const uint8_t maskValue);
};

inline TestFrameConverter::ValueProvider::ValueProvider() :
	alphaValue_(uint8_t(RandomI::random(0u, 255u))),
	gammaValue_(0.0f)
{
	const std::vector<float> gammaValues = {0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};

	gammaValue_ = RandomI::random(gammaValues);
}

inline uint8_t TestFrameConverter::ValueProvider::alphaValue() const
{
	return alphaValue_;
}

inline float TestFrameConverter::ValueProvider::gammaValue() const
{
	return gammaValue_;
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_H
