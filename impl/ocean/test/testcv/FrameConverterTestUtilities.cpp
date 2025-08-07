/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/FrameConverterTestUtilities.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/math/Random.h"

#include <array>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint8_t, uint16_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT16)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceGammaOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_GAMMA_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetAlphaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT8_ALPHA)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetBlackLevelWhiteBalanceGammaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT8_BLACKLEVEL_WHITEBALANCE_GAMMA)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint16_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT16_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint16_t, uint16_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT16_TO_1_UINT16)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint32_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT32_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint32_t, uint16_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT32_TO_1_UINT16)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceTwoTargetsConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_2_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const OneSourceThreeTargetsConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_3_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const TwoSourcesOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_2_UINT8_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const TwoSourcesThreeTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_2_UINT8_TO_3_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const TwoSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_2_UINT8_TO_1_UINT8_ALPHA)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const TwoSourcesTwoTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_2_UINT8_TO_2_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const ThreeSourcesOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_3_UINT8_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const ThreeSourcesThreeTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_3_UINT8_TO_3_UINT8)
{
	// nothing to do here
}

FrameConverterTestUtilities::FunctionWrapper::FunctionWrapper(const ThreeSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_3_UINT8_TO_1_UINT8_ALPHA)
{
	// nothing to do here
}

bool FrameConverterTestUtilities::FunctionWrapper::invoke(const Frame& source, Frame& target, const FrameConverter::ConversionFlag conversionFlag, const void* options, Worker* worker) const
{
	if (function_ == nullptr || source.width() != target.width() || source.height() != target.height())
	{
		ocean_assert(false && "This must never happen!");
		return false;
	}

	switch (functionType_)
	{
		case FT_1_UINT8_TO_1_UINT8:
			((OneSourceOneTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT8_TO_1_UINT16:
			((OneSourceOneTargetConversionFunction<uint8_t, uint16_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint16_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT8_GAMMA_TO_1_UINT8:
			((OneSourceGammaOneTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, ValueProvider::get().gammaValue(), source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT8_TO_1_UINT8_ALPHA:
			((OneSourceOneTargetAlphaConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), ValueProvider::get().alphaValue(), worker);
			return true;

		case FT_1_UINT8_TO_1_UINT8_BLACKLEVEL_WHITEBALANCE_GAMMA:
		{
			// floatOptions[0] - black level, uint16_t, range: [0, 1024)
			// floatOptions[1] - white balance red, float, range: [0, infinity)
			// floatOptions[2] - white balance green, float, range: [0, infinity)
			// floatOptions[3] - white balance blue, float, range: [0, infinity)
			// floatOptions[4] - gamma, float, range: (0, 2)
			const float* floatOptions = reinterpret_cast<const float*>(options);

			const uint16_t blackLevel = floatOptions ? uint16_t(floatOptions[0] + 0.5f) : uint16_t(0u);

			ocean_assert(floatOptions[1] >= 0.0f && floatOptions[2] >= 0.0f && floatOptions[3] >= 0.0f);
			const float whiteBalance[3] =
			{
				floatOptions[1],
				floatOptions[2],
				floatOptions[3],
			};

			const float gamma = floatOptions ? floatOptions[4] : 1.0f;

			((OneSourceOneTargetBlackLevelWhiteBalanceGammaConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, blackLevel, whiteBalance, gamma, source.paddingElements(0u), target.paddingElements(0u), worker);

			return true;
		}

		case FT_1_UINT16_TO_1_UINT8:
			((OneSourceOneTargetConversionFunction<uint16_t, uint8_t>)(function_))(source.constdata<uint16_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT16_TO_1_UINT16:
			((OneSourceOneTargetConversionFunction<uint16_t, uint16_t>)(function_))(source.constdata<uint16_t>(0u), target.data<uint16_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT32_TO_1_UINT8:
			((OneSourceOneTargetConversionFunction<uint32_t, uint8_t>)(function_))(source.constdata<uint32_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT32_TO_1_UINT16:
			((OneSourceOneTargetConversionFunction<uint32_t, uint16_t>)(function_))(source.constdata<uint32_t>(0u), target.data<uint16_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT8_TO_2_UINT8:
			((OneSourceTwoTargetsConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), target.paddingElements(1u), worker);
			return true;

		case FT_1_UINT8_TO_3_UINT8:
			((OneSourceThreeTargetsConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), target.data<uint8_t>(2u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), target.paddingElements(1u), target.paddingElements(2u), worker);
			return true;

		case FT_2_UINT8_TO_1_UINT8:
			((TwoSourcesOneTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), target.paddingElements(0u), worker);
			return true;

		case FT_2_UINT8_TO_2_UINT8:
			((TwoSourcesTwoTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), target.paddingElements(0u), target.paddingElements(1u), worker);
			return true;

		case FT_2_UINT8_TO_3_UINT8:
			((TwoSourcesThreeTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), target.data<uint8_t>(2u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), target.paddingElements(0u), target.paddingElements(1u), target.paddingElements(2u), worker);
			return true;

		case FT_2_UINT8_TO_1_UINT8_ALPHA:
			((TwoSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), target.paddingElements(0u), ValueProvider::get().alphaValue(), worker);
			return true;

		case FT_3_UINT8_TO_1_UINT8:
			((ThreeSourcesOneTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), source.constdata<uint8_t>(2u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), source.paddingElements(2u), target.paddingElements(0u), worker);
			return true;

		case FT_3_UINT8_TO_3_UINT8:
			((ThreeSourcesThreeTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), source.constdata<uint8_t>(2u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), target.data<uint8_t>(2u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), source.paddingElements(2u), target.paddingElements(0u), target.paddingElements(1u), target.paddingElements(2u), worker);
			return true;

		case FT_3_UINT8_TO_1_UINT8_ALPHA:
			((ThreeSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), source.constdata<uint8_t>(2u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), source.paddingElements(2u), target.paddingElements(0u), ValueProvider::get().alphaValue(), worker);
			return true;

		default:
			break;
	}

	ocean_assert(false && "Invalid function type!");
	return false;
}

bool FrameConverterTestUtilities::testFrameConversion(const FrameType::PixelFormat& sourcePixelFormat, const FrameType::PixelFormat& targetPixelFormat, const unsigned int width, const unsigned int height, const FunctionWrapper& functionWrapper, const CV::FrameConverter::ConversionFlag conversionFlag, const FunctionPixelValue functionSourcePixelValue, const FunctionPixelValue functionTargetPixelValue, const MatrixD& transformationMatrix, const double minimalGroundTruthValue, const double maximalGroundTruthValue, const double testDuration, Worker& worker, const unsigned int thresholdMaximalErrorToInteger, const void* options)
{
	ocean_assert(functionSourcePixelValue != nullptr && functionTargetPixelValue != nullptr);
	ocean_assert(testDuration > 0.0);
	ocean_assert(thresholdMaximalErrorToInteger < 20u);

	bool allSucceeded = true;

	Log::info() << "... " << translateConversionFlag(conversionFlag) << ":";

	const unsigned int widthMultiple = std::max(FrameType::widthMultiple(sourcePixelFormat), FrameType::widthMultiple(targetPixelFormat));
	const unsigned int heightMultiple = std::max(FrameType::heightMultiple(sourcePixelFormat), FrameType::heightMultiple(targetPixelFormat));

	if (widthMultiple % FrameType::widthMultiple(sourcePixelFormat) != 0u || widthMultiple % FrameType::widthMultiple(targetPixelFormat) != 0u
			|| heightMultiple % FrameType::heightMultiple(sourcePixelFormat) != 0u || heightMultiple % FrameType::heightMultiple(targetPixelFormat) != 0u)
	{
		ocean_assert(false && "We need to determine the least common multiple!");
		return false;
	}

	RandomGenerator randomGenerator;

	double averageErrorToFloat = 0.0;
	double averageErrorToInteger = 0.0;
	double maximalErrorToFloat = 0.0;
	unsigned int maximalErrorToInteger = 0u;

	unsigned int measurements = 0u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (const bool& benchmarkIteration : {true, false})
			{

#ifdef OCEAN_USE_GTEST
				const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(randomGenerator, 1u, 1001u) * widthMultiple;
				const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(randomGenerator, 1u, 41u) * heightMultiple;
#else
				const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(randomGenerator, 1u, 1000u) * widthMultiple;
				const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(randomGenerator, 1u, 1000u) * heightMultiple;
#endif

				const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, targetPixelFormat), &randomGenerator);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(benchmarkIteration);

				if (!functionWrapper.invoke(sourceFrame, targetFrame, conversionFlag, options, useWorker))
				{
					allSucceeded = false;
				}

				performance.stopIf(benchmarkIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				double localAverageErrorToFloat;
				double localAverageErrorToInteger;
				double localMaximalErrorToFloat;
				unsigned int localMaximalErrorToInteger;
				if (!validateConversion(sourceFrame, targetFrame, functionSourcePixelValue, functionTargetPixelValue, transformationMatrix, conversionFlag, &localAverageErrorToFloat, &localAverageErrorToInteger, &localMaximalErrorToFloat, &localMaximalErrorToInteger, minimalGroundTruthValue, maximalGroundTruthValue))
				{
					allSucceeded = false;
				}

				averageErrorToFloat += localAverageErrorToFloat;
				averageErrorToInteger += localAverageErrorToInteger;
				maximalErrorToFloat = std::max(maximalErrorToFloat, localMaximalErrorToFloat);
				maximalErrorToInteger = std::max(maximalErrorToInteger, localMaximalErrorToInteger);

				++measurements;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
		Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
	}

	ocean_assert(measurements != 0u);

	OCEAN_SUPPRESS_UNUSED_WARNING(averageErrorToFloat);

	averageErrorToFloat /= double(measurements);
	averageErrorToInteger /= double(measurements);

	if (maximalErrorToInteger > thresholdMaximalErrorToInteger)
	{
		Log::info() << "Validation FAILED: max error: " << maximalErrorToInteger << ", average error: " << String::toAString(averageErrorToInteger, 2u);
		allSucceeded = false;
	}
	else
	{
		Log::info() << "Validation succeeded: max error: " << maximalErrorToInteger << ", average error: " << String::toAString(averageErrorToInteger, 2u);
	}

	return allSucceeded;
}

bool FrameConverterTestUtilities::validateConversion(const Frame& sourceFrame, const Frame& targetFrame, const FunctionPixelValue functionSourcePixelValue, const FunctionPixelValue functionTargetPixelValue, const MatrixD& transformationMatrix, const CV::FrameConverter::ConversionFlag conversionFlag, double* averageAbsErrorToFloat, double* averageAbsErrorToInteger, double* maximalAbsErrorToFloat, unsigned int* maximalAbsErrorToInteger, const double minimalGroundTruthValue, const double maximalGroundTruthValue, const bool skipPlausibilityCheck)
{
	ocean_assert(sourceFrame.isValid() && targetFrame.isValid());
	ocean_assert(functionSourcePixelValue != nullptr && functionTargetPixelValue != nullptr);

	double localAverageErrorToFloat = 0.0;
	double localAverageErrorToInteger = 0.0;
	double localMaximalAbsErrorToFloat = 0.0;
	unsigned int localMaximalErrorToInteger = 0u;

	if (sourceFrame.width() != targetFrame.width() || sourceFrame.height() != targetFrame.height())
	{
		ocean_assert(false && "Invalid parameters!");
		return false;
	}

	if (sourceFrame.channels() != (unsigned int)(transformationMatrix.columns()) && sourceFrame.channels() + 1u != (unsigned int)(transformationMatrix.columns()))
	{
		ocean_assert(false && "Invalid parameters!");
		return false;
	}

	if (targetFrame.channels() != (unsigned int)(transformationMatrix.rows()))
	{
		ocean_assert(false && "Invalid parameters!");
		return false;
	}

	unsigned long long measurements = 0ull;

	for (unsigned int y = 0; y < sourceFrame.height(); ++y)
	{
		for (unsigned int x = 0; x < sourceFrame.width(); ++x)
		{
			const MatrixD targetVector = functionTargetPixelValue(targetFrame, x, y, conversionFlag);
			ocean_assert(targetVector.rows() >= 1 && targetVector.columns() == 1);

			MatrixD sourceVector = functionSourcePixelValue(sourceFrame, x, y, CV::FrameConverter::CONVERT_NORMAL);

			if (!targetVector || !sourceVector)
			{
				ocean_assert(false && "Invalid color vector/matrix!");
				return false;
			}

			if (sourceVector.columns() == 1)
			{
				if (sourceVector.rows() + 1 == transformationMatrix.columns())
				{
					// making the vector a homogeneous vector

					sourceVector = MatrixD(transformationMatrix.columns(), 1, sourceVector, 0, 0, 1.0);
				}
			}
			else
			{
				ocean_assert(sourceVector.columns() >= 2);

				if (sourceVector.columns() != targetFrame.channels())
				{
					ocean_assert(false && "Invalid matrix!");
					return false;
				}

				if (sourceVector.rows() + 1 == transformationMatrix.columns())
				{
					// making the matrix a homogeneous matrix

					sourceVector = MatrixD(transformationMatrix.columns(), sourceVector.columns(), sourceVector, 0, 0, 1.0);
				}
			}

			const MatrixD groundTruthTargetVector = transformationMatrix * sourceVector;

			for (unsigned int nChannel = 0u; nChannel < targetFrame.channels(); ++nChannel)
			{
				double groundTruthValueFloat;

				if (groundTruthTargetVector.columns() == 1)
				{
					groundTruthValueFloat = groundTruthTargetVector(nChannel, 0);
				}
				else
				{
					// in case the source vector was actually a source matrix, each column of that matrix is holding a different channel

					ocean_assert(nChannel < groundTruthTargetVector.columns());
					groundTruthValueFloat = groundTruthTargetVector(nChannel, nChannel);
				}

				if (minimalGroundTruthValue < maximalGroundTruthValue)
				{
					groundTruthValueFloat = minmax(minimalGroundTruthValue, groundTruthValueFloat, maximalGroundTruthValue);
				}
				else
				{
					ocean_assert(groundTruthValueFloat >= minimalGroundTruthValue && groundTruthValueFloat < maximalGroundTruthValue + 0.5);
				}

				const double valueFloat =  targetVector(nChannel, 0);

				const double absError = NumericD::abs(groundTruthValueFloat - valueFloat);

				ocean_assert_and_suppress_unused(skipPlausibilityCheck || absError <= 10.0, skipPlausibilityCheck);

				localAverageErrorToFloat += absError;

				localAverageErrorToInteger += std::abs(int(groundTruthValueFloat) - int(valueFloat));
				localMaximalAbsErrorToFloat = max(localMaximalAbsErrorToFloat, NumericD::abs(groundTruthValueFloat - valueFloat));
				localMaximalErrorToInteger = max(localMaximalErrorToInteger, (unsigned int)(std::abs(int(groundTruthValueFloat) - int(valueFloat))));

				measurements++;
			}
		}
	}

	ocean_assert(measurements != 0ull);

	if (averageAbsErrorToFloat)
	{
		*averageAbsErrorToFloat = localAverageErrorToFloat / double(measurements);
	}

	if (averageAbsErrorToInteger)
	{
		*averageAbsErrorToInteger = localAverageErrorToInteger / double(measurements);
	}

	if (maximalAbsErrorToFloat)
	{
		*maximalAbsErrorToFloat = localMaximalAbsErrorToFloat;
	}

	if (maximalAbsErrorToInteger)
	{
		*maximalAbsErrorToInteger = localMaximalErrorToInteger;
	}

	return true;
}

MatrixD FrameConverterTestUtilities::functionGenericPixel(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(FrameType::formatIsGeneric(frame.pixelFormat()));

	ocean_assert(x < frame.width() && y < frame.height());

	unsigned int xAdjusted = x;
	unsigned int yAdjusted = y;

	switch (conversionFlag)
	{
		case CV::FrameConverter::CONVERT_NORMAL:
			break;

		case CV::FrameConverter::CONVERT_FLIPPED:
			yAdjusted = frame.height() - y - 1u;
			break;

		case CV::FrameConverter::CONVERT_MIRRORED:
			xAdjusted = frame.width() - x - 1u;
			break;

		case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
			xAdjusted = frame.width() - x - 1u;
			yAdjusted = frame.height() - y - 1u;
			break;

		default:
			ocean_assert(false && "Not supported conversion flag.");
	}

	const unsigned int channels = frame.channels();

	MatrixD pixelColors(channels, 1, 0.0);

	switch (frame.dataType())
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<uint8_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_SIGNED_INTEGER_8:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<int8_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_16:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<uint16_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_SIGNED_INTEGER_16:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<int16_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_32:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<uint32_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_SIGNED_INTEGER_32:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<int32_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_64:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<uint64_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_SIGNED_INTEGER_64:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<int64_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		default:
			ocean_assert(false && "Invalid data type!");
			break;
	}

	return pixelColors;
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
