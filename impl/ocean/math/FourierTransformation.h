/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_FOURIER_TRANSFORMATION_H
#define META_OCEAN_MATH_FOURIER_TRANSFORMATION_H

#include "ocean/math/Math.h"
#include "ocean/math/Complex.h"
#include "ocean/math/Numeric.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

/**
 * This class implements Fourier transformation functions.
 * @ingroup math
 */
class OCEAN_MATH_EXPORT FourierTransformation
{
	public:

		/**
		 * This class provides the implementation of the Fourier transformation of the Ocean framework.
		 */
		class NaiveImplementation
		{
			public:

				/**
				 * Applies a forward Fourier transformation for a given 2D spatial signal.
				 * The spatial signal may be composed of several channels (element-wise interleaved).<br>
				 * However only one channel will be transformed to one joined block of frequencies at once.<br>
				 * @param spatial 2D spatial signal that will be transformed
				 * @param width The width of the 2D signal in elements
				 * @param height height of the 2D signal in elements
				 * @param frequency Resulting frequency analysis for the given signal, make sure that the provided buffer is large enough
				 * @param worker Optional worker object to distribute the computation
				 * @tparam T Data type of the spatial and frequency signals
				 */
				template <typename T>
				static void spatialToFrequency2(const std::complex<T>* spatial, const unsigned int width, const unsigned int height, std::complex<T>* frequency, Worker* worker = nullptr);

				/**
				 * Applies a backward Fourier transformation for a given 2D frequency signal.
				 * The spatial signal may be composed of several channels (element-wise interleaved).<br>
				 * However only one joined block of frequencies will be transformed to one channel at once.<br>
				 * @param frequency 2D frequency signal that will be transformed
				 * @param width The width of the 2D signal in elements
				 * @param height height of the 2D signal in elements
				 * @param spatial Resulting spatial frequency for the given signal, make sure that the provided buffer is large enough
				 * @param worker Optional worker object to distribute the computation
				 * @tparam T Data type of the spatial and frequency signals
				 */
				template <typename T>
				static void frequencyToSpatial2(const std::complex<T>* frequency, const unsigned int width, const unsigned int height, std::complex<T>* spatial, Worker* worker = nullptr);

			protected:

				/**
				 * Applies a horizontal forward Fourier transformation for a subset of a given 2D spatial signal.
				 * @param spatial 2D spatial signal that will be transformed
				 * @param width The width of the 2D signal in elements
				 * @param height height of the 2D signal in elements
				 * @param frequencyHorizontal Resulting horizontal frequency analysis for the given signal, make sure that the provided buffer is large enough
				 * @param firstRow First row to be handled
				 * @param numberRows Number of rows to be handled
				 */
				template <typename T>
				static void spatialToFrequencyHorizontalSubset2(const std::complex<T>* spatial, const unsigned int width, const unsigned int height, std::complex<T>* frequencyHorizontal, const unsigned int firstRow, const unsigned int numberRows);

				/**
				 * Applies a vertical forward Fourier transformation for a subset of a given 2D spatial signal.
				 * @param spatial 2D spatial signal that will be transformed
				 * @param width The width of the 2D signal in elements
				 * @param height height of the 2D signal in elements
				 * @param frequencyVertical Resulting vertical frequency analysis for the given signal, make sure that the provided buffer is large enough
				 * @param firstColumn First column to be handled
				 * @param numberColumns Number of columns to be handled
				 */
				template <typename T>
				static void spatialToFrequencyVerticalSubset2(const std::complex<T>* spatial, const unsigned int width, const unsigned int height, std::complex<T>* frequencyVertical, const unsigned int firstColumn, const unsigned int numberColumns);

				/**
				 * Applies a horizontal backward Fourier transformation for subset of a given 2D frequency signal.
				 * @param frequency 2D frequency signal that will be transformed
				 * @param width The width of the 2D signal in elements
				 * @param height height of the 2D signal in elements
				 * @param spatialHorizontal Resulting spatial frequency for the given signal, make sure that the provided buffer is large enough
				 * @param firstRow First row to be handled
				 * @param numberRows Number of rows to be handled
				 */
				template <typename T>
				static void frequencyToSpatialHorizontalSubset2(const std::complex<T>* frequency, const unsigned int width, const unsigned int height, std::complex<T>* spatialHorizontal, const unsigned int firstRow, const unsigned int numberRows);

				/**
				 * Applies a vertical backward Fourier transformation for a subset of a given 2D frequency signal.
				 * @param frequency 2D frequency signal that will be transformed
				 * @param width The width of the 2D signal in elements
				 * @param height height of the 2D signal in elements
				 * @param spatialVertical Resulting vertical spatial frequency for the given signal, make sure that the provided buffer is large enough
				 * @param firstColumn First column to be handled
				 * @param numberColumns Number of columns to be handled
				 */
				template <typename T>
				static void frequencyToSpatialVerticalSubset2(const std::complex<T>* frequency, const unsigned int width, const unsigned int height, std::complex<T>* spatialVertical, const unsigned int firstColumn, const unsigned int numberColumns);
		};

	public:

		/**
		 * Direct Fourier Transformation (OpenCV-compatible interface)
		 * @sa cv::dft()
		 * @param source Input array that could be real or complex. Must be valid.
		 * @param target Output array whose size and type depends on the flags. Size depends on `flags`.
		 * @param flags Transformation flags, representing a combination of the cv::DftFlags
		 * @param nonzero_rows Number of nonzero rows. All rows after that will be ignored.
		 * @return True on success, otherwise false
		 */
		static bool dft0(const Frame& source, Frame& target, int flags, int nonzero_rows);

		/**
		 * Direct Fourier Transformation (OpenCV-compatible interface)
		 * @sa cv::dft()
		 * @param source Input array that could be real or complex, must be a valid pointer
		 * @param width The width of the input and output arrays, range: [1, infinity)
		 * @param height The height of the input and output arrays, range: [1, infinity)
		 * @param sourceChannels Number of channels of the input array, range: [1, 2]
		 * @param target Output array, this must be initialized before calling this function, size depends on `flags`, must be a valid pointer
		 * @param targetChannels Number of channels of the output array, depends on `flags`, range: [1, 2]
		 * @param dataType Data type of the input and output (must be the same), valid values: FrameType::DT_SIGNED_FLOAT_32 or FrameType::DT_SIGNED_FLOAT_64
		 * @param flags Transformation flags, representing a combination of the cv::DftFlags
		 * @param nonzero_rows Number of nonzero rows. All rows after that will be ignored
		 * @param sourcePaddingElements Number of padding elements used in the input array, range: [0, infinity)
		 * @param targetPaddingElements Number of padding elements used in the output array, range: [0, infinity)
		 * @return True on success, otherwise false
		 */
		static bool dft0(const void* source, const unsigned int width, const unsigned int height, const unsigned int sourceChannels, void* target, const unsigned int targetChannels, const Ocean::FrameType::DataType dataType, int flags, int nonzero_rows, const unsigned int sourcePaddingElements = 0u, const unsigned int targetPaddingElements = 0u);

		/**
		 * Returns the optimal DFT size for a given vector size
		 * @sa cv::getOptimalDFTSize()
		 * @param size The size of the vector
		 * @return Optimal size for the DFT
		 */
		static int getOptimalDFTSize0(int size);

		/**
		 * Applies a forward Fourier transformation for a given 2D (real) spatial signal.
		 * @param spatial The real 2D spatial signal that will be transformed, must be valid
		 * @param width The width of the 2D signal in elements, with range [1, infinity)
		 * @param height height of the 2D signal in elements, with range [1, infinity)
		 * @param complexFrequency Resulting complex frequency analysis for the given signal, must be valid
		 * @param spatialPaddingElements The number of padding elements at the end of each row of the spatial signal, in elements with respect to `T` (not `complex<T>`), with range [0, infinity)
		 * @param frequencyPaddingElements The number of padding elements at the end of each row of the frequency analysis, in elements (not `complex<T>`), with range [0, infinity)
		 * @tparam T The element data type of the spatial and frequency signals, either 'float' or 'double'
		 */
		template <typename T>
		static void spatialToFrequency2(const T* spatial, const unsigned int width, const unsigned int height, T* complexFrequency, const unsigned int spatialPaddingElements = 0u, const unsigned int frequencyPaddingElements = 0u);

		/**
		 * Applies a forward Fourier transformation for a given 2D (complex) spatial signal.
		 * @param complexSpatial The complex 2D spatial signal that will be transformed, must be valid
		 * @param width The width of the 2D signal in elements, with range [1, infinity)
		 * @param height height of the 2D signal in elements, with range [1, infinity)
		 * @param complexFrequency Resulting complex frequency analysis for the given signal, must be valid
		 * @param spatialPaddingElements The number of padding elements at the end of each row of the spatial signal, in elements with respect to `T` (not `complex<T>`), with range [0, infinity)
		 * @param frequencyPaddingElements The number of padding elements at the end of each row of the frequency analysis, in elements (not `complex<T>`), with range [0, infinity)
		 * @tparam T Data type of the spatial and frequency signals, either 'float' or 'double'
		 */
		template <typename T>
		static void complexSpatialToFrequency2(const T* complexSpatial, const unsigned int width, const unsigned int height, T* complexFrequency, const unsigned int spatialPaddingElements = 0u, const unsigned int frequencyPaddingElements = 0u);

		/**
		 * Applies a backward Fourier transformation for a given 2D frequency signal.
		 * @param complexFrequency The complex 2D frequency signal that will be transformed, must be valid
		 * @param width The width of the 2D signal in elements, with range [1, infinity)
		 * @param height height of the 2D signal in elements, with range [1, infinity)
		 * @param spatial Resulting (real) spatial frequency for the given signal, make sure that the provided buffer is large enough, must be valid
		 * @param frequencyPaddingElements The number of padding elements at the end of each row of the frequency analysis, in elements (not `complex<T>`), with range [0, infinity)
		 * @param spatialPaddingElements The number of padding elements at the end of each row of the spatial signal, in elements with respect to `T` (not `complex<T>`), with range [0, infinity)
		 * @tparam T Data type of the spatial and frequency signals, either 'float' or 'double'
		 */
		template <typename T>
		static void frequencyToSpatial2(const T* complexFrequency, const unsigned int width, const unsigned int height, T* spatial, const unsigned int frequencyPaddingElements = 0u, const unsigned int spatialPaddingElements = 0u);

		/**
		 * Applies a backward Fourier transformation for a given 2D frequency signal.
		 * @param complexFrequency The complex 2D frequency signal that will be transformed, must be valid
		 * @param width The width of the 2D signal in elements, with range [1, infinity)
		 * @param height height of the 2D signal in elements, with range [1, infinity)
		 * @param complexSpatial Resulting complex spatial frequency for the given signal, make sure that the provided buffer is large enough, must be valid
		 * @param frequencyPaddingElements The number of padding elements at the end of each row of the frequency analysis, in elements (not `complex<T>`), with range [0, infinity)
		 * @param spatialPaddingElements The number of padding elements at the end of each row of the spatial signal, in elements with respect to `T` (not `complex<T>`), with range [0, infinity)
		 * @tparam T Data type of the spatial and frequency signals, either 'float' or 'double'
		 */
		template <typename T>
		static void frequencyToComplexSpatial2(const T* complexFrequency, const unsigned int width, const unsigned int height, T* complexSpatial, const unsigned int frequencyPaddingElements = 0u, const unsigned int spatialPaddingElements = 0u);

		/**
		 * Converts scalar values to complex values.
		 * @param source Scalar source values
		 * @param target Resulting complex values, make sure that the provided memory is large enough
		 * @param number Number of elements that will be converted
		 * @tparam TScalar Data type of the real signal
		 * @tparam TComplex Data type of the complex signal
		 */
		template <typename TScalar, typename TComplex>
		static inline void scalarToComplex(const TScalar* source, std::complex<TComplex>* target, const size_t number);

		/**
		 * Converts the real components of complex values to scalar values.
		 * @param source Complex source values
		 * @param target Resulting scalar values, make sure that the provided memory is large enough
		 * @param number Number of elements that will be converted
		 * @tparam TComplex Data type of the complex signal
		 * @tparam TScalar Data type of the real signal
		 */
		template <typename TComplex, typename TScalar>
		static inline void realToScalar(const std::complex<TComplex>* source, TScalar* target, const size_t number);

		/**
		 * Converts the imaginary components of complex values to scalar values.
		 * @param source Complex source values
		 * @param target Resulting scalar values, make sure that the provided memory is large enough
		 * @param number Number of elements that will be converted
		 * @tparam TComplex Data type of the complex signal
		 * @tparam TScalar Data type of the real signal
		 */
		template <typename TComplex, typename TScalar>
		static inline void imaginaryToScalar(const std::complex<TComplex>* source, TScalar* target, const size_t number);

		/**
		 * Converts complex values to magnitude (or absolute) values.
		 * The magnitude of a complex value is determined by the square root of the squared sum of the real and the imaginary element.<br>
		 * @param source Complex source values
		 * @param target Resulting magnitude values, make sure that the provided memory is large enough
		 * @param number Number of elements that will be converted
		 * @tparam TComplex Data type of the complex signal
		 * @tparam TScalar Data type of the real signal
		 */
		template <typename TComplex, typename TScalar>
		static inline void complexToMagnitude(const std::complex<TComplex>* source, TScalar* target, const size_t number);

		/**
		 * Shifts a given signal by half of the width and the height.
		 * @param source The source signal that will be shifted
		 * @param width The width of the signal in elements, must be even
		 * @param height The height of the signal in elements, must be even
		 * @param target The target signal that will receive the shifted signal
		 * @tparam T Data type of the signal
		 */
		template <typename T>
		static void shiftHalfDimension2(const T* source, unsigned int width, const unsigned int height, T* target);

		/**
		 * Shifts a given signal by half of the width and the height.
		 * @param data Signal that will be shifted
		 * @param width The width of the signal in elements, must be even
		 * @param height The height of the signal in elements, must be even
		 * @tparam T Data type of the signal
		 */
		template <typename T>
		static void shiftHalfDimension2(T* data, unsigned int width, const unsigned int height);

		/**
		 * Returns the center position for shift operations that corresponds with the first Fourier element.
		 * @param size Size of the signal in bins, with range (0, infinity)
		 * @return Corresponding center position of the first bin
		 */
		static inline unsigned int shiftCenter(const unsigned int size);

		/**
		 * Elementwise multiplication of two 2D complex Fourier spectrums (one channel spectrums).
		 * @param complexSourceA The pointer of the first complex source spectrum, must be valid
		 * @param complexSourceB The pointer of the second complex source spectrum, must be valid
		 * @param complexTarget The pointer of the target spectrum receiving the elementwise multiplication result, must be valid
		 * @param width The width of the 2D spectrum (the number of complex elements in horizontal direction), with range [1, infinity)
		 * @param height The height of the 2D specturm (the number of complex elements in vertical direction), with range [1, infinity)
		 * @param horizontalPaddingSourceAElements Optional horizontal padding at the end of each row of the first source specturm, in elements with respect to `T` (not `complex<T>`), with range [0, infinity)
		 * @param horizontalPaddingSourceBElements Optional horizontal padding at the end of each row of the second source specturm, in elements with respect to `T` (not `complex<T>`), with range [0, infinity)
		 * @param horizontalPaddingTargetElements Optional horizontal padding at the end of each row of the target specturm, in elements with respect to `T` (not `complex<T>`), with range [0, infinity)
		 * @tparam T The data type of the real and imaginary part of the complex number, e.g., 'float' or 'double'
		 * @tparam tComplexConjugateA True, to multiply the complex conjugated values of the first spectrum; False, to multiply the first spectrum without modification
		 * @tparam tComplexConjugateB True, to multiply the complex conjugated values of the first spectrum; False, to multiply the second spectrum without modification
		 */
		template <typename T, bool tComplexConjugateA, bool tComplexConjugateB>
		static void elementwiseMultiplication2(const T* complexSourceA, const T* complexSourceB, T* complexTarget, const unsigned int width, const unsigned int height, const unsigned int horizontalPaddingSourceAElements = 0u, const unsigned int horizontalPaddingSourceBElements = 0u, const unsigned int horizontalPaddingTargetElements = 0u);

		/**
		 * Multiplication of two 2D complex Fourier spectrums (one channel spectrums) in packed complex conjugate-symmetric format or CCS-packed format.
		 * For 2D one-channel data, the packed conjugate-symmetric format (CCS-packed format) is defined as follows:
		 *
		 * 1. Even width, \f$w = 2L\f$, and even height, \f$h = 2K\f$
		 *
		 * \f[\begin{bmatrix}
		 *     R_{0,0}   & R_{0,1}    & I_{0,1}    & R_{0,2}    & \cdots & I_{0,L-1}    & R_{0,L}   \\
		 *     R_{1,0}   & R_{1,1}    & I_{1,1}    & R_{1,2}    & \cdots & I_{1,L-1}    & R_{1,L}   \\
		 *     I_{1,0}   & R_{2,1}    & I_{2,1}    & R_{2,2}    & \cdots & I_{2,L-1}    & I_{1,L}   \\
		 *     R_{2,0}   & R_{3,1}    & I_{3,1}    & R_{3,2}    & \cdots & I_{3,L-1}    & R_{2,L}   \\
		 *     \cdots    & \cdots     & \cdots     & \cdots     & \cdots & \cdots       & \cdots    \\
		 *     I_{K-1,0} & R_{2K-2,1} & I_{2K-2,1} & R_{2K-2,2} & \cdots & I_{2K-2,L-1} & I_{K-1,L} \\
		 *     R_{K,0}   & R_{2K-1,1} & I_{2K-1,1} & R_{2K-1,2} & \cdots & I_{2K-1,L-1} & R_{K-1,L} \\
		 * \end{bmatrix}\in\mathbb{R}^{h \times w}\f]
		 *
		 * 2. Even width, \f$w = 2L\f$, and odd height, \f$h = 2K + 1\f$
		 *
		 * \f[\begin{bmatrix}
		 *     R_{0,0}   & R_{0,1}    & I_{0,1}    & R_{0,2}    & \cdots & I_{0,L-1}    & R_{0,L}   \\
		 *     R_{1,0}   & R_{1,1}    & I_{1,1}    & R_{1,2}    & \cdots & I_{1,L-1}    & R_{1,L}   \\
		 *     I_{1,0}   & R_{2,1}    & I_{2,1}    & R_{2,2}    & \cdots & I_{2,L-1}    & I_{1,L}   \\
		 *     R_{2,0}   & R_{3,1}    & I_{3,1}    & R_{3,2}    & \cdots & I_{3,L-1}    & R_{2,L}   \\
		 *     \cdots    & \cdots     & \cdots     & \cdots     & \cdots & \cdots       & \cdots    \\
		 *     I_{K-1,0} & R_{2K-2,1} & I_{2K-2,1} & R_{2K-2,2} & \cdots & I_{2K-2,L-1} & I_{K-1,L} \\
		 *     R_{K,0}   & R_{2K-1,1} & I_{2K-1,1} & R_{2K-1,2} & \cdots & I_{2K-1,L-1} & R_{K-1,L} \\
		 *     I_{K,0}   & R_{2K,1}   & I_{2K,1}   & R_{2K,2}   & \cdots & I_{2K,L-1}   & I_{K,L}   \\
		 * \end{bmatrix}\in\mathbb{R}^{h \times w}\f]
		 *
		 * 3. Odd width, \f$w = 2L + 1\f$, and even height, \f$h = 2K\f$
		 *
		 * \f[\begin{bmatrix}
		 *     R_{0,0}   & R_{0,1}    & I_{0,1}    & R_{0,2}    & \cdots & I_{0,L-1}    & R_{0,L}    & I_{0,L}    \\
		 *     R_{1,0}   & R_{1,1}    & I_{1,1}    & R_{1,2}    & \cdots & I_{1,L-1}    & R_{1,L}    & I_{1,L}    \\
		 *     I_{1,0}   & R_{2,1}    & I_{2,1}    & R_{2,2}    & \cdots & I_{2,L-1}    & R_{2,L}    & I_{2,L}    \\
		 *     R_{2,0}   & R_{3,1}    & I_{3,1}    & R_{3,2}    & \cdots & I_{3,L-1}    & R_{3,L}    & I_{3,L}    \\
		 *     \cdots    & \cdots     & \cdots     & \cdots     & \cdots & \cdots       & \cdots     & \cdots     \\
		 *     I_{K-1,0} & R_{2K-2,1} & I_{2K-2,1} & R_{2K-2,2} & \cdots & I_{2K-2,L-1} & R_{2K-2,L} & I_{2K-2,L} \\
		 *     R_{K,0}   & R_{2K-1,1} & I_{2K-1,1} & R_{2K-1,2} & \cdots & I_{2K-1,L-1} & R_{2K-1,L} & I_{2K-1,L} \\
		 * \end{bmatrix}\in\mathbb{R}^{h \times w}\f]
		 *
		 * 4. Odd width, \f$w = 2L + 1\f$, and odd height, \f$h = 2K + 1\f$
		 *
		 * \f[\begin{bmatrix}
		 *     R_{0,0}   & R_{0,1}    & I_{0,1}    & R_{0,2}    & \cdots & I_{0,L-1}    & R_{0,L}    & I_{0,L}    \\
		 *     R_{1,0}   & R_{1,1}    & I_{1,1}    & R_{1,2}    & \cdots & I_{1,L-1}    & R_{1,L}    & I_{1,L}    \\
		 *     I_{1,0}   & R_{2,1}    & I_{2,1}    & R_{2,2}    & \cdots & I_{2,L-1}    & R_{2,L}    & I_{2,L}    \\
		 *     R_{2,0}   & R_{3,1}    & I_{3,1}    & R_{3,2}    & \cdots & I_{3,L-1}    & R_{3,L}    & I_{3,L}    \\
		 *     \cdots    & \cdots     & \cdots     & \cdots     & \cdots & \cdots       & \cdots     & \cdots     \\
		 *     I_{K-1,0} & R_{2K-2,1} & I_{2K-2,1} & R_{2K-2,2} & \cdots & I_{2K-2,L-1} & R_{2K-2,L} & I_{2K-2,L} \\
		 *     R_{K,0}   & R_{2K-1,1} & I_{2K-1,1} & R_{2K-1,2} & \cdots & I_{2K-1,L-1} & R_{2K-1,L} & I_{2K-1,L} \\
		 *     I_{K,0}   & R_{2K,1}   & I_{2K,1}   & R_{2K,2}   & \cdots & I_{2K,L-1}  & R_{2K,L}    & I_{2K,L}   \\
		 * \end{bmatrix}\in\mathbb{R}^{h \times w}\f]
		 *
		 * For 1D one-channel data, the format is defined as:
		 *
		 * 1. Even length, \f$l = \textbf{max}(w, h) = 2L\f$
		 *
		 * \f[\begin{bmatrix}
		 *     R_{0} & R_{1} & I_{1} & R_{2} & \cdots & I_{L-1} & R_{L}
		 * \end{bmatrix}\in\mathbb{R}^{l}\f]
		 *
		 * 2. Odd length, \f$l = \textbf{max}(w, h) = 2L + 1\f$
		 *
		 * \f[\begin{bmatrix}
		 *     R_{0} & R_{1} & I_{1} & R_{2} & \cdots & I_{L-1} & R_{L} & I_{L}
		 * \end{bmatrix}\in\mathbb{R}^{l}\f]
		 *
		 * The input data is expected to be in the CCS-packed format. The output is guaranteed to be in the CCS-packed format.
		 *
		 * More resources about the complex conjugate-symmetric format:
		 * - https://www.comp.nus.edu.sg/~cs4243/doc/ipl.pdf, page 157
		 * - https://software.intel.com/en-us/mkl-developer-reference-c-dfti-packed-format, DFTI_CCS_FORMAT for One/Two-dimensional Transforms
		 *
		 * @note For 1D and 2D one-channel input this function produces the same result as OpenCV's `cv::multspectums()` (requires that `TIntermediate=double`). For 2D two-channel input refer to the above function `elementwiseMultiplication2()`.
		 * @note For 1D and 2D one-channel data this function can take the output of OpenCV's `cv::dft()`
		 * @param sourceA The pointer of the first source spectrum, must be valid
		 * @param sourceB The pointer of the second source spectrum, must be valid
		 * @param target The pointer of the targt spectrum receiving the elementwise multiplication result, must be valid
		 * @param width The width of the input and output spectra, with range [1, infinity)
		 * @param height The height of the input and output spectra, with range [1, infinity)
		 * @param horizontalPaddingSourceAElements Optional horizontal padding at the end of each row of the first source specturm (number of complex elements), with range [0, infinity)
		 * @param horizontalPaddingSourceBElements Optional horizontal padding at the end of each row of the second source specturm (number of complex elements), with range [0, infinity)
		 * @param horizontalPaddingTargetElements Optional horizontal padding at the end of each row of the target specturm (number of complex elements), with range [0, infinity)
		 * @tparam TComplex Type of the real and imaginary parts of the complex numbers
		 * @tparam tComplexConjugateA If true, the conjugated values of `sourceA` will be used in the multiplication, otherwise they will be used as is
		 * @tparam tComplexConjugateB If true, the conjugated values of `sourceB` will be used in the multiplication, otherwise they will be used as is
		 * @tparam TIntermediate Type that is used internally for the computation. Size of this can be larger than `TComplex` which helps reduce float rounding errors.
		 */
		template <typename TComplex, bool tComplexConjugateA, bool tComplexConjugateB, typename TIntermediate=double>
		static void elementwiseMultiplicationCCS(const TComplex* sourceA, const TComplex* sourceB, TComplex* target, const unsigned int width, const unsigned int height, const unsigned int horizontalPaddingSourceAElements = 0u, const unsigned int horizontalPaddingSourceBElements = 0u, const unsigned int horizontalPaddingTargetElements = 0u);

		/**
		 * Elementwise division of two 2D complex Fourier spectrums (one channel spectrums).
		 * This function allows the definition of padding elements at the end of each row to support sub-specturms.
		 * @param complexSourceA The pointer of the first source spectrum, must be valid
		 * @param complexSourceB The pointer of the second source spectrum, must be valid
		 * @param complexTarget The pointer of the targt spectrum receiving the elementwise multiplication result, must be valid
		 * @param width The width of the 2D spectrum (the number of complex elements in horizontal direction), with range [1, infinity)
		 * @param height The height of the 2D specturm (the number of complex elements in vertical direction), with range [1, infinity)
		 * @param horizontalPaddingSourceAElements Optional horizontal padding at the end of each row of the first source specturm, in elements with respect to `T` (not `complex<T>`), with range [0, infinity)
		 * @param horizontalPaddingSourceBElements Optional horizontal padding at the end of each row of the second source specturm, in elements with respect to `T` (not `complex<T>`), with range [0, infinity)
		 * @param horizontalPaddingTargetElements Optional horizontal padding at the end of each row of the target specturm, in elements with respect to `T` (not `complex<T>`), with range [0, infinity)
		 * @tparam T The data type of the real and imaginary part of the complex number, e.g., 'float' or 'double'
		 */
		template <typename T>
		static void elementwiseDivision2(const T* complexSourceA, const T* complexSourceB, T* complexTarget, const unsigned int width, const unsigned int height, const unsigned int horizontalPaddingSourceAElements = 0u, const unsigned int horizontalPaddingSourceBElements = 0u, const unsigned int horizontalPaddingTargetElements = 0u);
};

template <typename T>
inline void FourierTransformation::NaiveImplementation::spatialToFrequency2(const std::complex<T>* spatial, const unsigned int width, const unsigned int height, std::complex<T>* frequency, Worker* worker)
{
	ocean_assert(spatial && frequency);
	std::vector< std::complex<T> > frequencyHorizontal(width * height);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(spatialToFrequencyHorizontalSubset2<T>, spatial, width, height, frequencyHorizontal.data(), 0u, 0u), 0u, height);
		worker->executeFunction(Worker::Function::createStatic(spatialToFrequencyVerticalSubset2<T>, (const std::complex<T>*)frequencyHorizontal.data(), width, height, frequency, 0u, 0u), 0u, width);
	}
	else
	{
		spatialToFrequencyHorizontalSubset2<T>(spatial, width, height, frequencyHorizontal.data(), 0u, height);
		spatialToFrequencyVerticalSubset2<T>(frequencyHorizontal.data(), width, height, frequency, 0u, width);
	}
}

template <typename T>
inline void FourierTransformation::NaiveImplementation::frequencyToSpatial2(const std::complex<T>* frequency, const unsigned int width, const unsigned int height, std::complex<T>* spatial, Worker* worker)
{
	ocean_assert(frequency && spatial);
	std::vector< std::complex<T> > spatialHorizontal(width * height);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(frequencyToSpatialHorizontalSubset2<T>, frequency, width, height, spatialHorizontal.data(), 0u, 0u), 0u, height);
		worker->executeFunction(Worker::Function::createStatic(frequencyToSpatialVerticalSubset2<T>, (const std::complex<T>*)spatialHorizontal.data(), width, height, spatial, 0u, 0u), 0u, width);
	}
	else
	{
		frequencyToSpatialHorizontalSubset2<T>(frequency, width, height, spatialHorizontal.data(), 0u, height);
		frequencyToSpatialVerticalSubset2<T>(spatialHorizontal.data(), width, height, spatial, 0u, width);
	}
}

template <typename T>
void FourierTransformation::NaiveImplementation::spatialToFrequencyHorizontalSubset2(const std::complex<T>* spatial, const unsigned int width, const unsigned int height, std::complex<T>* frequencyHorizontal, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(spatial && frequencyHorizontal);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);
	ocean_assert(width != 0u);

	const T pi2_width_1 = NumericT<T>::pi2() / T(width);

	std::complex<T> value;

	for (unsigned int r = firstRow; r < firstRow + numberRows; ++r)
	{
		const std::complex<T>* const signal = spatial + r * width;
		std::complex<T>* const spectrum = frequencyHorizontal + r * width;

		for (unsigned int k = 0u; k < width; ++k)
		{
			value = Complex(0, 0);

			for (unsigned int n = 0u; n < width; ++n)
			{
				const T angle = pi2_width_1 * T(n * k);
				value += signal[n] * std::complex<T>(NumericT<T>::cos(angle), -NumericT<T>::sin(angle));
			}

			spectrum[k] = value;
		}
	}
}

template <typename T>
void FourierTransformation::NaiveImplementation::spatialToFrequencyVerticalSubset2(const std::complex<T>* spatial, const unsigned int width, const unsigned int height, std::complex<T>* frequencyVertical, const unsigned int firstColumn, const unsigned int numberColumns)
{
	ocean_assert(spatial && frequencyVertical);
	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert(height != 0u);

	const T pi2_height_1 = NumericT<T>::pi2() / T(height);

	std::complex<T> value;

	for (unsigned int c = firstColumn; c < firstColumn + numberColumns; ++c)
	{
		const std::complex<T>* const signal = spatial + c;
		std::complex<T>* const spectrum = frequencyVertical + c;

		for (unsigned int k = 0u; k < height; ++k)
		{
			value = Complex(0, 0);

			for (unsigned int n = 0u; n < height; ++n)
			{
				const T angle = pi2_height_1 * T(n * k);
				value += signal[n * width] * std::complex<T>(NumericT<T>::cos(angle), -NumericT<T>::sin(angle));
			}

			spectrum[k * width] = value;
		}
	}
}

template <typename T>
void FourierTransformation::NaiveImplementation::frequencyToSpatialHorizontalSubset2(const std::complex<T>* frequency, const unsigned int width, const unsigned int height, std::complex<T>* spatialHorizontal, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(frequency && spatialHorizontal);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);
	ocean_assert(width != 0u);

	const T pi2_width_1 = NumericT<T>::pi2() / T(width);
	const T normalization = T(1) / T(width);

	std::complex<T> value;

	for (unsigned int r = firstRow; r < firstRow + numberRows; ++r)
	{
		const std::complex<T>* const spectrum = frequency + r * width;
		std::complex<T>* const signal = spatialHorizontal + r * width;

		for (unsigned int n = 0; n < width; ++n)
		{
			value = Complex(0, 0);

			for (unsigned int k = 0; k < width; ++k)
			{
				const T angle = pi2_width_1 * T(k * n);
				value += spectrum[k] * std::complex<T>(NumericT<T>::cos(angle), NumericT<T>::sin(angle));
			}

			signal[n] = value * normalization;
		}
	}
}

template <typename T>
void FourierTransformation::NaiveImplementation::frequencyToSpatialVerticalSubset2(const std::complex<T>* frequency, const unsigned int width, const unsigned int height, std::complex<T>* spatialVertical, const unsigned int firstColumn, const unsigned int numberColumns)
{
	ocean_assert(frequency && spatialVertical);
	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert(height != 0u);

	const T pi2_height_1 = NumericT<T>::pi2() / T(height);
	const T normalization = T(1) / T(height);

	std::complex<T> value;

	for (unsigned int c = firstColumn; c < firstColumn + numberColumns; ++c)
	{
		const std::complex<T>* const spectrum = frequency + c;
		std::complex<T>* const signal = spatialVertical + c;

		for (unsigned int n = 0u; n < height; ++n)
		{
			value = Complex(0, 0);

			for (unsigned int k = 0; k < height; ++k)
			{
				const T angle = pi2_height_1 * T(k * n);
				value += spectrum[k * width] * std::complex<T>(NumericT<T>::cos(angle), NumericT<T>::sin(angle));
			}

			signal[n * width] = value * normalization;
		}
	}
}

template <typename TScalar, typename TComplex>
inline void FourierTransformation::scalarToComplex(const TScalar* source, std::complex<TComplex>* target, const size_t number)
{
	ocean_assert(source && target);

	for (size_t n = 0; n < number; ++n)
		target[n] = Complex(source[n], 0);
}

template <typename TComplex, typename TScalar>
inline void FourierTransformation::realToScalar(const std::complex<TComplex>* source, TScalar* target, const size_t number)
{
	ocean_assert(source && target);

	for (size_t n = 0; n < number; ++n)
		target[n] = source[n].real();
}

template <typename TComplex, typename TScalar>
inline void FourierTransformation::imaginaryToScalar(const std::complex<TComplex>* source, TScalar* target, const size_t number)
{
	ocean_assert(source && target);

	for (size_t n = 0; n < number; ++n)
		target[n] = source[n].imag();
}

template <typename TComplex, typename TScalar>
inline void FourierTransformation::complexToMagnitude(const std::complex<TComplex>* source, TScalar* target, const size_t number)
{
	ocean_assert(source && target);

	for (size_t n = 0; n < number; ++n)
		target[n] = std::abs(source[n]);
}

template <typename T>
void FourierTransformation::shiftHalfDimension2(const T* source, unsigned int width, const unsigned int height, T* target)
{
	ocean_assert(source && target);
	ocean_assert(width % 2u == 0u);
	ocean_assert(height % 2u == 0u);

	const unsigned int width2 = width / 2u;
	const unsigned int height2 = height / 2u;

	for (unsigned int y = 0u; y < height; ++y)
		for (unsigned int x = 0u; x < width; ++x)
			target[((y + height2) % height) * width + ((x + width2) % width)] = *source++;
}

template <typename T>
void FourierTransformation::shiftHalfDimension2(T* data, unsigned int width, const unsigned int height)
{
	ocean_assert(data);

	// if the frame dimension is a multiple of two
	if (width % 2u == 0u && height % 2u == 0u)
	{
		const unsigned int width_2 = width / 2u;
		const unsigned int height_2 = height / 2u;

		for (unsigned int y = 0u; y < height_2; ++y)
			for (unsigned int x = 0u; x < width_2; ++x)
			{
				// swap top left with bottom right
				std::swap(data[y * width + x], data[(y + height_2) * width + x + width_2]);

				// swap bottom left with top right
				std::swap(data[(y + height_2) * width + x], data[y * width + x + width_2]);
			}
	}
	else
	{
		Memory tmpMemory = Memory::create<T>(width * height);
		T* tmp = tmpMemory.data<T>();

		ocean_assert(tmp != nullptr);

		memcpy(tmp, data, width * height * sizeof(T));

		const unsigned int width_2 = width / 2u;
		const unsigned int height_2 = height / 2u;

		const unsigned int extraX = width % 2u;
		const unsigned int extraY = height % 2u;

		for (unsigned int y = 0u; y < height_2 + extraY; ++y)
		{
			// top left to bottom right
			memcpy(data + (y + height_2) * width + width_2, tmp + y * width, (width_2 + extraX) * sizeof(T));

			// top right to bottom left
			memcpy(data + (y + height_2) * width, tmp + y * width + width_2 + extraX, width_2 * sizeof(T));
		}

		for (unsigned int y = 0u; y < height_2; ++y)
		{
			// bottom right to top left
			memcpy(data + y * width, tmp + (y + height_2 + extraY) * width + width_2 + extraX, width_2 * sizeof(T));

			// bottom left to top right
			memcpy(data + y * width + width_2 + extraX, tmp + (y + height_2 + extraY) * width, (width_2 + extraX) * sizeof(T));
		}

		ocean_assert(data[shiftCenter(height) * width + shiftCenter(width)] == tmp[0]);
	}
}

inline unsigned int FourierTransformation::shiftCenter(const unsigned int size)
{
	return size / 2u;
}

template <typename T, bool tComplexConjugateA, bool tComplexConjugateB>
void FourierTransformation::elementwiseMultiplication2(const T* complexSourceA, const T* complexSourceB, T* complexTarget, const unsigned int width, const unsigned int height, const unsigned int horizontalPaddingSourceAElements, const unsigned int horizontalPaddingSourceBElements, const unsigned int horizontalPaddingTargetElements)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Invalid data type!");
	static_assert(sizeof(std::complex<T>) == sizeof(T) * 2, "Invalid data type!");

	ocean_assert(complexSourceA != nullptr && complexSourceB != nullptr && complexTarget != nullptr);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int complexSourceAStrideElements = width * 2u + horizontalPaddingSourceAElements;
	const unsigned int complexSourceBStrideElements = width * 2u + horizontalPaddingSourceBElements;
	const unsigned int complexTargetStrideElements = width * 2u + horizontalPaddingTargetElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		const std::complex<T>* const sourceARow = (const std::complex<T>*)(complexSourceA + y * complexSourceAStrideElements);
		const std::complex<T>* const sourceBRow = (const std::complex<T>*)(complexSourceB + y * complexSourceBStrideElements);
		std::complex<T>* const targetRow = (std::complex<T>*)(complexTarget + y * complexTargetStrideElements);

		if constexpr (tComplexConjugateA && tComplexConjugateB)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				targetRow[x] = std::conj(sourceARow[x]) * std::conj(sourceBRow[x]);
			}
		}
		else if constexpr (tComplexConjugateA && !tComplexConjugateB)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				targetRow[x] = std::conj(sourceARow[x]) * sourceBRow[x];
			}
		}
		else if constexpr (!tComplexConjugateA && tComplexConjugateB)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				targetRow[x] = sourceARow[x] * std::conj(sourceBRow[x]);
			}
		}
		else
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				targetRow[x] = sourceARow[x] * sourceBRow[x];
			}
		}
	}
}

template <typename TComplex, bool tComplexConjugateA, bool tComplexConjugateB, typename TIntermediate>
void FourierTransformation::elementwiseMultiplicationCCS(const TComplex* sourceA, const TComplex* sourceB, TComplex* target, const unsigned int width, const unsigned int height, const unsigned int horizontalPaddingSourceAElements, const unsigned int horizontalPaddingSourceBElements, const unsigned int horizontalPaddingTargetElements)
{
	ocean_assert(sourceA && sourceB && target);
	ocean_assert(width != 0u && height != 0u);

	const size_t sourceAStrideElements = width + horizontalPaddingSourceAElements;
	const size_t sourceBStrideElements = width + horizontalPaddingSourceBElements;
	const size_t targetStrideElements = width + horizontalPaddingTargetElements;

	const bool isOneDimensionalData = width == 1u || height == 1u;

	if (isOneDimensionalData)
	{
		// First element
		target[0] = (TComplex)((TIntermediate)sourceA[0] * (TIntermediate)sourceB[0]);

		// Middle elements
		const unsigned int elementsCount = std::max(width, height);
		const unsigned int lastElement = elementsCount - 1u;

		for (unsigned int j = 1u; j < elementsCount; j += 2)
		{
			const TIntermediate realA = (TIntermediate)sourceA[j];
			const TIntermediate imaginaryA = (TIntermediate)(tComplexConjugateA ? -sourceA[j + 1u] : sourceA[j + 1u]);

			const TIntermediate realB = (TIntermediate)sourceB[j];
			const TIntermediate imaginaryB = (TIntermediate)(tComplexConjugateB ? -sourceB[j + 1u] : sourceB[j + 1u]);

			target[j] = (TComplex)((realA * realB) - (imaginaryA * imaginaryB));
			target[j + 1u] = (TComplex)((imaginaryA * realB) + (realA * imaginaryB));
		}

		// Last element
		if (elementsCount % 2u == 0u)
		{
			target[lastElement] = (TComplex)((TIntermediate)sourceA[lastElement] * (TIntermediate)sourceB[lastElement]);
		}
	}
	else
	{
		const unsigned int lastRowIndex = height - 1u;
		const unsigned int lastColumnIndex = width - 1u;
		const bool isWidthEven = width % 2u == 0u;
		const bool isHeightEven = height % 2u == 0u;

		// Left-most column
		target[0] = (TComplex)((TIntermediate)sourceA[0] * (TIntermediate)sourceB[0]);

		for (unsigned int row = 1u; row < lastRowIndex; row += 2u)
		{
			const TIntermediate realA = (TIntermediate)sourceA[row * sourceAStrideElements];
			const TIntermediate imaginaryA = (TIntermediate)(tComplexConjugateA ? -sourceA[(row + 1u) * sourceAStrideElements] : sourceA[(row + 1u) * sourceAStrideElements]);

			const TIntermediate realB = (TIntermediate)sourceB[row * sourceBStrideElements];
			const TIntermediate imaginaryB = (TIntermediate)(tComplexConjugateB ? -sourceB[(row + 1u) * sourceBStrideElements] : sourceB[(row + 1u) * sourceBStrideElements]);

			target[row * targetStrideElements] = (TComplex)((realA * realB) - (imaginaryA * imaginaryB));
			target[(row + 1u) * targetStrideElements] = (TComplex)((imaginaryA * realB) + (realA * imaginaryB));
		}

		// Bottom-left element, if height is even
		if (isHeightEven)
		{
			target[lastRowIndex * targetStrideElements] = (TComplex)((TIntermediate)sourceA[lastRowIndex * sourceAStrideElements] * (TIntermediate)sourceB[lastRowIndex * sourceBStrideElements]);
		}

		// Right-most column, if width is even
		if (isWidthEven)
		{
			target[lastColumnIndex] = sourceA[lastColumnIndex] * sourceB[lastColumnIndex];

			for (unsigned int row = 1u; row < lastRowIndex; row += 2u)
			{
				const TIntermediate realA = (TIntermediate)sourceA[row * sourceAStrideElements + lastColumnIndex];
				const TIntermediate imaginaryA = (TIntermediate)(tComplexConjugateA ? -sourceA[(row + 1u) * sourceAStrideElements + lastColumnIndex] : sourceA[(row + 1u) * sourceAStrideElements + lastColumnIndex]);

				const TIntermediate realB = (TIntermediate)sourceB[row * sourceBStrideElements + lastColumnIndex];
				const TIntermediate imaginaryB = (TIntermediate)(tComplexConjugateB ? -sourceB[(row + 1u) * sourceBStrideElements + lastColumnIndex] : sourceB[(row + 1u) * sourceBStrideElements + lastColumnIndex]);

				target[row * targetStrideElements + lastColumnIndex] = (TComplex)((realA * realB) - (imaginaryA * imaginaryB));
				target[(row + 1u) * targetStrideElements + lastColumnIndex] = (TComplex)((imaginaryA * realB) + (realA * imaginaryB));
			}

			// Bottom-right element, if height is even
			if (isHeightEven)
			{
				target[lastRowIndex * targetStrideElements + lastColumnIndex] = (TComplex)((TIntermediate)sourceA[lastRowIndex * sourceAStrideElements + lastColumnIndex] * (TIntermediate)sourceB[lastRowIndex * sourceBStrideElements + lastColumnIndex]);
			}
		}

		// Middle columns
		const unsigned int columnEnd = width - (isWidthEven ? 1u : 0u);

		for (unsigned int row = 0u; row < height; ++row)
		{
			for (unsigned int column = 1u; column < columnEnd; column += 2u)
			{
				const TIntermediate realA = (TIntermediate)sourceA[column];
				const TIntermediate imaginaryA = (TIntermediate)(tComplexConjugateA ? -sourceA[column + 1u] : sourceA[column + 1u]);

				const TIntermediate realB = (TIntermediate)sourceB[column];
				const TIntermediate imaginaryB = (TIntermediate)(tComplexConjugateB ? -sourceB[column + 1u] : sourceB[column + 1u]);

				target[column] = (TComplex)((realA * realB) - (imaginaryA * imaginaryB));
				target[column + 1u] = (TComplex)((imaginaryA * realB) + (realA * imaginaryB));
			}

			sourceA += sourceAStrideElements;
			sourceB += sourceBStrideElements;
			target += targetStrideElements;
		}
	}
}

template <typename T>
void FourierTransformation::elementwiseDivision2(const T* complexSourceA, const T* complexSourceB, T* complexTarget, const unsigned int width, const unsigned int height, const unsigned int horizontalPaddingSourceAElements, const unsigned int horizontalPaddingSourceBElements, const unsigned int horizontalPaddingTargetElements)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Invalid data type!");
	static_assert(sizeof(std::complex<T>) == sizeof(T) * 2, "Invalid data type!");

	ocean_assert(complexSourceA != nullptr && complexSourceB != nullptr && complexTarget != nullptr);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int complexSourceAStrideElements = width * 2u + horizontalPaddingSourceAElements;
	const unsigned int complexSourceBStrideElements = width * 2u + horizontalPaddingSourceBElements;
	const unsigned int complexTargetStrideElements = width * 2u + horizontalPaddingTargetElements;

	// (a + bi) / (c + di)
	// = [(ac + bd) + i(bc - ad)] / (c^2 + d^2)

	for (unsigned int y = 0u; y < height; ++y)
	{
		const std::complex<T>* const sourceARow = (const std::complex<T>*)(complexSourceA + y * complexSourceAStrideElements);
		const std::complex<T>* const sourceBRow = (const std::complex<T>*)(complexSourceB + y * complexSourceBStrideElements);
		std::complex<T>* const targetRow = (std::complex<T>*)(complexTarget + y * complexTargetStrideElements);

		for (unsigned int x = 0u; x < width; ++x)
		{
			const T denominator = sourceBRow[x].real() * sourceBRow[x].real() + sourceBRow[x].imag() * sourceBRow[x].imag();
			ocean_assert(NumericT<T>::isNotEqualEps(denominator));

			const T invDenominator = T(1.0) / denominator;

			targetRow[x] = std::complex<T>((sourceARow[x].real() * sourceBRow[x].real() + sourceARow[x].imag() * sourceBRow[x].imag()) * invDenominator,
												(sourceARow[x].imag() * sourceBRow[x].real() - sourceARow[x].real() * sourceBRow[x].imag()) * invDenominator);
		}
	}
}

} // namespace Ocean

#endif // META_OCEAN_MATH_FOURIER_TRANSFORMATION_H
