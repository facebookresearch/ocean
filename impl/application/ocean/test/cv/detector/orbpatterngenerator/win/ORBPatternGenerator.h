/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_CV_DETECTOR_ORB_DETECTOR_PATTERN_GENERATOR_WIN_ORB_DETECTOR_PATTERN_GENERATOR_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_CV_DETECTOR_ORB_DETECTOR_PATTERN_GENERATOR_WIN_ORB_DETECTOR_PATTERN_GENERATOR_H

#include "application/ocean/test/cv/detector/ApplicationTestCVDetector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/ORBFeature.h"

#include "ocean/math/Math.h"

using namespace Ocean;

/**
 * This class implements the ORB pattern generator.
 * @ingroup applicationtestcvdetectororbpatterngeneratorwin
 */
class ORBPatternGenerator
{
	public:

		/**
		 * This Subclass implements a pattern test which is necessary for the pattern generation.
		 */
		class PatternTest
		{
			public:

				/**
				 * Creates a PatternTest object width zeros values.
				 */
				PatternTest() = default;

				/**
				 * Creates a Test object by given four coordinates for the two points.
				 * @param x1 X coordinate of the first point
				 * @param y1 Y coordinate of the first point
				 * @param x2 X coordinate of the second point
				 * @param y2 Y coordinate of the second point
				 */
				inline PatternTest(const int x1, const int y1, const int x2, const int y2);

				/**
				 * Returns the x coordinate of the first point.
				 * @return X coordinate of first point
				 */
				inline const int& x1() const;

				/**
				 * Returns the x coordinate of the first point.
				 * @return X coordinate of first point
				 */
				inline int& x1();

				/**
				 * Returns the y coordinate of the first point.
				 * @return Y coordinate of first point
				 */
				inline const int& y1() const;

				/**
				 * Returns the y coordinate of the first point.
				 * @return Y coordinate of first point
				 */
				inline int& y1();

				/**
				 * Returns the x coordinate of the second point.
				 * @return X coordinate of second point
				 */
				inline const int& x2() const;

				/**
				 * Returns the x coordinate of the second point.
				 * @return X coordinate of second point
				 */
				inline int& x2();

				/**
				 * Returns the y coordinate of the second point.
				 * @return Y coordinate of second point
				 */
				inline const int& y2() const;

				/**
				 * Returns the y coordinate of the second point.
				 * @return Y coordinate of second point
				 */
				inline int& y2();

				/**
				 * Returns the sum of ones of the binary test.
				 * @return Sum of binary test
				 */
				inline const unsigned int& binaryTestSum() const;

				/**
				 * Returns the sum of ones of the binary test.
				 * @return Sum of binary test
				 */
				inline unsigned int& binaryTestSum();

				/**
				 * Returns the average of the results from the test.
				 * @return Average of binary test
				 */
				inline Scalar average() const;

				/**
				 * Set the average of the results from the test.
				 * @param value Average value
				 */
				inline void setAverage(const Scalar value);

				/**
				 * Returns the correlation index of this element in result until the correlation are tested so far.
				 * @return Correlation index
				 */
				inline const unsigned int& correlationIndex() const;

				/**
				 * Returns the correlation index of this element in result until the correlation are tested so far.
				 * @return Correlation index
				 */
				inline unsigned int& correlationIndex();

				/**
				 * Returns the maximal correlation to all tests in the result so far.
				 * @return Maximal correlation
				 */
				inline Scalar maxCorrelation() const;

				/**
				 * Set the maximal correlation to all tests in the result so far.
				 * @param value Maximal correlation to set, Range: [0, 1]
				 */
				inline void setMaxCorrelation(const Scalar value);

				/**
				 * Returns the test vector, holds all test results from all feature points in all tested images.
				 * @return Test vector
				 */
				inline const std::vector<bool>& testVector() const;

				/**
				 * Returns the test vector, holds all test results from all feature points in all tested images.
				 * @return Test vector
				 */
				inline std::vector<bool>& testVector();

				/**
				 * Returns true, if this test is less then the given test.
				 * @param compare Test to compare width
				 * @return True, if so
				 */
				inline bool operator<(const PatternTest& compare) const;

			private:

				// The four values for the x and y coordinates of the two test points
				int values_[4] = {0, 0, 0, 0};

				// Sum of all ones of this test
				unsigned int binaryTestSumValue_ = 0u;

				// Average value from this test, Range [0, 1]
				Scalar averageValue_ = Scalar(0);

				// Index for this element in result until the correlation are tested so far
				unsigned int correlationIndexValue_ = 0u;

				// Maximal correlation to all tests in the result variable so far
				Scalar maxCorrelationValue_ = Scalar(0);

				// Holds all test results from all feature points in all tested images
				std::vector<bool> testVectorValues_;
		};

	public:

		/**
		 * Creates a ORBPatternGenerator object.
		 */
		ORBPatternGenerator();

		/**
		 * Generates the tests for the images.
		 * @param frames The frames which will tested
		 */
		void generateTests(Frames& frames);

		/**
		 * Calculate the test results of all possible tests with all detected feature points.
		 * The results are stored in the binaryTests testVector
		 * @param linedIntegralFrame The lined integral frame of the tested image, must be valid
		 * @param width The width of the tested image
		 * @param height The height of the tested image
		 * @param featurePoints Feature points, that are detected in the tested image
		 */
		void calculateTests(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, const CV::Detector::ORBFeatures& featurePoints);

		/**
		 * Search the best tests from all calculated tests.
		 * The results are stored in the result variable.
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 */
		void searchBestTests(Worker* worker = nullptr);

		/**
		 * Returns the Results from the best test searching.
		 * @return Best tests
		 */
		inline std::vector<PatternTest> getResult() const;

	private:

		/**
		 * Calculating the sum's that are needed for calculating the correlation between the tests.
		 * @param object The object of the ORBBinaryTestsCalculation
		 * @param indexBinaryTests Index from the binary test
		 * @param indexResult Index for the result to calculate the sum with
		 * @param sum1 Pointer to the first sum
		 * @param sum2 Pointer to the second sum
		 * @param sum3 Pointer to the third sum
		 * @param lock The lock for thread save addition of the sums
		 * @param startIndex First index of the test vector to be handled
		 * @param range Number of elements of the test vector to be handled
		 */
		static void calculateSumForCorrelation(ORBPatternGenerator* object, const unsigned int indexBinaryTests, const unsigned int indexResult, Scalar* sum1, Scalar* sum2, Scalar* sum3, Lock* lock, const unsigned int startIndex, const unsigned int range);

	private:

		// All possible binary tests
		std::vector<PatternTest> binaryTests_;

		// Number of feature points from all tested images
		unsigned int countFeaturePoints_ = 0u;

		// Result, store the best 256 tests
		std::vector<PatternTest> result_;
};

inline ORBPatternGenerator::PatternTest::PatternTest(const int x1, const int y1, const int x2, const int y2) :
	binaryTestSumValue_(0u),
	averageValue_(Scalar(0.0)),
	correlationIndexValue_(0u),
	maxCorrelationValue_(Scalar(0.0))
{
	values_[0] = x1;
	values_[1] = y1;
	values_[2] = x2;
	values_[3] = y2;
}

inline const int& ORBPatternGenerator::PatternTest::x1() const
{
	return values_[0];
}

inline int& ORBPatternGenerator::PatternTest::x1()
{
	return values_[0];
}

inline const int& ORBPatternGenerator::PatternTest::y1() const
{
	return values_[1];
}

inline int& ORBPatternGenerator::PatternTest::y1()
{
	return values_[1];
}

inline const int& ORBPatternGenerator::PatternTest::x2() const
{
	return values_[2];
}

inline int& ORBPatternGenerator::PatternTest::x2()
{
	return values_[2];
}

inline const int& ORBPatternGenerator::PatternTest::y2() const
{
	return values_[3];
}

inline int& ORBPatternGenerator::PatternTest::y2()
{
	return values_[3];
}

inline const unsigned int& ORBPatternGenerator::PatternTest::binaryTestSum() const
{
	return binaryTestSumValue_;
}

inline unsigned int& ORBPatternGenerator::PatternTest::binaryTestSum()
{
	return binaryTestSumValue_;
}

inline Scalar ORBPatternGenerator::PatternTest::average() const
{
	return averageValue_;
}

inline void ORBPatternGenerator::PatternTest::setAverage(const Scalar value)
{
	ocean_assert(value >= 0 && value <= 1);
	averageValue_ = value;
}

inline const unsigned int& ORBPatternGenerator::PatternTest::correlationIndex() const
{
	return correlationIndexValue_;
}

inline unsigned int& ORBPatternGenerator::PatternTest::correlationIndex()
{
	return correlationIndexValue_;
}

inline Scalar ORBPatternGenerator::PatternTest::maxCorrelation() const
{
	return maxCorrelationValue_;
}

inline void ORBPatternGenerator::PatternTest::setMaxCorrelation(const Scalar value)
{
	ocean_assert(value >= 0 && value <= 1);
	maxCorrelationValue_ = value;
}

inline const std::vector<bool>& ORBPatternGenerator::PatternTest::testVector() const
{
	return testVectorValues_;
}

inline std::vector<bool>& ORBPatternGenerator::PatternTest::testVector()
{
	return testVectorValues_;
}

inline bool ORBPatternGenerator::PatternTest::operator<(const ORBPatternGenerator::PatternTest& compare) const
{
	return Numeric::abs(average() - Scalar(0.5)) < Numeric::abs(compare.average() - Scalar(0.5));
}

inline std::vector<ORBPatternGenerator::PatternTest> ORBPatternGenerator::getResult() const
{
	return result_;
}

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_CV_DETECTOR_ORB_DETECTOR_PATTERN_GENERATOR_WIN_ORB_DETECTOR_PATTERN_GENERATOR_H
