/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/cv/detector/orbpatterngenerator/win/ORBPatternGenerator.h"

#include "ocean/base/String.h"

#include "ocean/cv/detector/FASTFeatureDetector.h"
#include "ocean/cv/detector/ORBFeatureDescriptor.h"
#include "ocean/cv/detector/ORBFeatureOrientation.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/math/SquareMatrix2.h"

ORBPatternGenerator::ORBPatternGenerator()
{
	const int subWindowSizeHalf = 2u;
	const int patchWindowSizeHalf = 15u;
	const int initialX = -patchWindowSizeHalf + subWindowSizeHalf;
	const int endX = patchWindowSizeHalf - subWindowSizeHalf + 1;
	const int endY = patchWindowSizeHalf - subWindowSizeHalf + 1;

	PatternTest test(-patchWindowSizeHalf + subWindowSizeHalf, -patchWindowSizeHalf + subWindowSizeHalf, -patchWindowSizeHalf + subWindowSizeHalf, -patchWindowSizeHalf + subWindowSizeHalf);

	binaryTests_.reserve(240856u);
	while (test.y1() != endY)
	{
		while (test.x1() != endX)
		{
			while (test.y2() != endY)
			{
				while (test.x2() != endX)
				{
					if (NumericT<int>::secureAbs(test.x1() - test.x2()) >= 5u || NumericT<int>::secureAbs(test.y1() - test.y2()) >= 5u)
					{
						binaryTests_.push_back(test);
					}

					test.x2()++;
				}

				test.y2()++;
				test.x2() = initialX;
			}

			test.x1()++;
			test.x2() = test.x1();
			test.y2() = test.y1();
		}

		test.y1()++;
		test.x1() = initialX;
	}
}

void ORBPatternGenerator::generateTests(Frames& frames)
{
	for (unsigned int i = 0u; i < frames.size(); i++)
	{
		std::cout << "Calculate tests for the "<< i + 1 << ". image" << std::endl;

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(frames[i], FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		const Frame linedIntegralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrame);
		ocean_assert(linedIntegralFrame.isContinuous());

		CV::Detector::FASTFeatures featurePointsFAST;
		constexpr unsigned int threshold = 35u;

		CV::Detector::FASTFeatureDetector::detectFeatures(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), threshold, false, false, featurePointsFAST, yFrame.paddingElements());
		CV::Detector::ORBFeatures featurePoints = CV::Detector::ORBFeature::features2ORBFeatures(featurePointsFAST, yFrame.width(), yFrame.height());
		CV::Detector::ORBFeatureOrientation::determineFeatureOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), featurePoints);

		calculateTests(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), featurePoints);
	}
}

void ORBPatternGenerator::calculateTests(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, const CV::Detector::ORBFeatures& featurePoints)
{
	ocean_assert(linedIntegralFrame != nullptr);
	ocean_assert(width > 0 && height > 0);

	constexpr unsigned int linedIntegralFramePaddingElements = 0u;

	for (unsigned int i = 0; i < binaryTests_.size(); i++)
	{
		binaryTests_[i].testVector().resize(countFeaturePoints_ + featurePoints.size());

		for (unsigned int j = 0; j < featurePoints.size(); j++)
		{
			const Scalar angle = featurePoints[j].orientation() * Numeric::deg2rad(5);
			const SquareMatrix2 rotationMatrix = SquareMatrix2(Scalar(Numeric::cos(angle)), Scalar(Numeric::sin(angle)), Scalar(-Numeric::sin(angle)), Scalar(Numeric::cos(angle)));

			Vector2 point1 = Vector2(Scalar(binaryTests_[i].x1()), Scalar(binaryTests_[i].y1()));
			Vector2 point2 = Vector2(Scalar(binaryTests_[i].x2()), Scalar(binaryTests_[i].y2()));
			point1 = rotationMatrix * point1;
			point2 = rotationMatrix * point2;

			const Scalar x = Scalar(featurePoints[j].observation().x() + 0.5);
			const Scalar y = Scalar(featurePoints[j].observation().y() + 0.5);

			const Scalar intensity1 = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, Vector2(x + point1.x(), y + point1.y()), CV::PC_CENTER, 5u, 5u);
			const Scalar intensity2 = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, Vector2(x + point2.x(), y + point2.y()), CV::PC_CENTER, 5u, 5u);

			if (intensity1 < intensity2)
			{
				binaryTests_[i].binaryTestSum()++;
				binaryTests_[i].testVector()[countFeaturePoints_ + j] = true;
			}
			else
			{
				binaryTests_[i].testVector()[countFeaturePoints_ + j] = false;
			}
		}
	}

	countFeaturePoints_ += (unsigned int)featurePoints.size();
}

void ORBPatternGenerator::searchBestTests(Worker* worker)
{
	for (unsigned int i = 0; i < binaryTests_.size(); i++)
	{
		binaryTests_[i].setAverage(Scalar(binaryTests_[i].binaryTestSum()) / Scalar(countFeaturePoints_));
	}

	std::sort(binaryTests_.begin(), binaryTests_.end());

	result_.reserve(256);
	result_.clear();
	result_.push_back(binaryTests_[0]);

	Scalar threshold = Scalar(0.01);

	if (worker && countFeaturePoints_ > 2000)
	{
		Lock lock;

		do
		{
			std::cout << std::endl << "threshold: " << threshold << std::endl;
			for (unsigned int i = 1u; i < binaryTests_.size(); i++)
			{
				if (binaryTests_[i].maxCorrelation() > threshold)
				{
					continue;
				}

				for (unsigned int j = binaryTests_[i].correlationIndex(); j < result_.size(); j++)
				{
					Scalar sum1 = Scalar(0.0);
					Scalar sum2 = Scalar(0.0);
					Scalar sum3 = Scalar(0.0);

					worker->executeFunction(Worker::Function::createStatic(ORBPatternGenerator::calculateSumForCorrelation, this, i, j, &sum1, &sum2, &sum3, &lock, 0u, 0u), 0u, countFeaturePoints_, 7, 8);

					const Scalar correlation = (Numeric::isEqualEps(sum2) || Numeric::isEqualEps(sum3)) ? Scalar(0.0) : Numeric::abs( sum1 / Numeric::sqrt(sum2 * sum3) );

					if (correlation > binaryTests_[i].maxCorrelation())
						binaryTests_[i].setMaxCorrelation(correlation);

					binaryTests_[i].correlationIndex()++;

					if (correlation > threshold)
					{
						break;
					}
				}

				if (binaryTests_[i].maxCorrelation() < threshold)
				{
					result_.push_back(binaryTests_[i]);
					std::cout << "add " << result_.size() << ". test (" << i << "):  mean: " << binaryTests_[i].average() << "  |  correlation: " << binaryTests_[i].maxCorrelation() << std::endl;
				}

				if (result_.size() == 256)
				{
					break;
				}
			}

			threshold += Scalar(0.01);
		}
		while (result_.size() < 256);
	}
	else
	{
		do
		{
			std::cout << std::endl << "threshold: " << threshold << std::endl;
			for (unsigned int i = 1u; i < binaryTests_.size(); i++)
			{
				if (binaryTests_[i].maxCorrelation() > threshold)
				{
					continue;
				}

				for (unsigned int j = binaryTests_[i].correlationIndex(); j < result_.size(); j++)
				{
					Scalar sum1 = Scalar(0.0);
					Scalar sum2 = Scalar(0.0);
					Scalar sum3 = Scalar(0.0);

					for(unsigned int k = 0; k < countFeaturePoints_; k++)
					{
						const Scalar binaryTestsVector_i = binaryTests_[i].testVector()[k] ? Scalar(1.0) : Scalar(0.0);
						const Scalar resultVector_i = result_[j].testVector()[k] ? Scalar(1.0) : Scalar(0.0);

						sum1 += (binaryTestsVector_i - binaryTests_[i].average()) * (resultVector_i - result_[j].average());
						sum2 += (binaryTestsVector_i - binaryTests_[i].average()) * (binaryTestsVector_i- binaryTests_[i].average());
						sum3 += (resultVector_i - result_[j].average()) * (resultVector_i - result_[j].average());
					}
					const Scalar correlation = (Numeric::isEqualEps(sum2) || Numeric::isEqualEps(sum3)) ? Scalar(0.0) : Numeric::abs( sum1 / Numeric::sqrt(sum2 * sum3) );

					if (correlation > binaryTests_[i].maxCorrelation())
					{
						binaryTests_[i].setMaxCorrelation(correlation);
					}

					binaryTests_[i].correlationIndex()++;

					if (correlation > threshold)
					{
						break;
					}
				}

				if (binaryTests_[i].maxCorrelation() < threshold)
				{
					result_.push_back(binaryTests_[i]);
					std::cout << "add " << result_.size() << ". test (" << i << "):  mean: " << binaryTests_[i].average() << "  |  correlation: " << binaryTests_[i].maxCorrelation() << std::endl;
				}

				if (result_.size() == 256)
				{
					break;
				}
			}

			threshold += Scalar(0.01);
		}
		while (result_.size() < 256);
	}
}

void ORBPatternGenerator::calculateSumForCorrelation(ORBPatternGenerator* object, const unsigned int indexBinaryTests, const unsigned int indexResult, Scalar* sum1, Scalar* sum2, Scalar* sum3, Lock* lock, const unsigned int startIndex, const unsigned int range)
{
	ocean_assert(object);
	ocean_assert(sum1 && sum2 && sum3);
	ocean_assert(indexBinaryTests < object->binaryTests_.size());
	ocean_assert(indexResult < object->binaryTests_.size());
	ocean_assert(object->binaryTests_[indexBinaryTests].testVector().size() < startIndex + range);
	ocean_assert(object->binaryTests_[indexResult].testVector().size() < startIndex + range);

	Scalar tempSum1 = Scalar(0.0);
	Scalar tempSum2 = Scalar(0.0);
	Scalar tempSum3 = Scalar(0.0);

	for (unsigned int i = startIndex; i < startIndex + range; i++)
	{
		const Scalar binaryTestsVector_i = object->binaryTests_[indexBinaryTests].testVector()[i] ? Scalar(1.0) : Scalar(0.0);
		const Scalar resultVector_i = object->result_[indexResult].testVector()[i] ? Scalar(1.0) : Scalar(0.0);

		tempSum1 += (binaryTestsVector_i - object->binaryTests_[indexBinaryTests].average()) * (resultVector_i - object->result_[indexResult].average());
		tempSum2 += (binaryTestsVector_i - object->binaryTests_[indexBinaryTests].average()) * (binaryTestsVector_i- object->binaryTests_[indexBinaryTests].average());
		tempSum3 += (resultVector_i - object->result_[indexResult].average()) * (resultVector_i - object->result_[indexResult].average());
	}

	const OptionalScopedLock scopedLock(lock);

	*sum1 += tempSum1;
	*sum2 += tempSum2;
	*sum3 += tempSum3;
}
