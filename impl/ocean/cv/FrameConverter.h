/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_H
#define META_OCEAN_CV_FRAME_CONVERTER_H

#include "ocean/cv/CV.h"
#include "ocean/cv/CVUtilities.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Matrix.h"

namespace Ocean
{

namespace CV
{

OCEAN_DISABLE_DOCUMENTATION_DIAGNOSTIC // Clang has a bug and need to be disabled for FrameConverter

/**
 * This is the base class for all frame converter classes.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverter
{
	public:

		/**
		 * Definition of individual conversion flags.
		 */
		enum ConversionFlag : uint32_t
		{
			/**
			 * Normal conversion, neither flips nor mirrors the image.
			 * The following pattern shows the conversion for an image with resolution 2x2:
			 * <pre>
			 * Input:    Output:
			 * | A B |   | A B |
			 * | C D |   | C D |
			 * </pre>
			 */
			CONVERT_NORMAL,

			/**
			 * Flipped conversion, exchanges top and bottom of the image (flipping around the x-axis).
			 * The following pattern shows the conversion for an image with resolution 2x2:
			 * <pre>
			 * Input:    Output:
			 * | A B |   | C D |
			 * | C D |   | A B |
			 * </pre>
			 */
			CONVERT_FLIPPED,

			/**
			 * Mirrored conversion, exchanges left and right of the image (like in a mirror, mirroring around the y-axis).
			 * The following pattern shows the conversion for an image with resolution 2x2:
			 * <pre>
			 * Input:    Output:
			 * | A B |   | B A |
			 * | C D |   | D C |
			 * </pre>
			 */
			CONVERT_MIRRORED,

			/**
			 * Rotated conversion, rotates the image by 180.0 degrees with anchor in the center of the image.
			 * The following pattern shows the conversion for an image with resolution 2x2:
			 * <pre>
			 * Input:    Output:
			 * | A B |   | D C |
			 * | C D |   | B A |
			 * </pre>
			 */
			CONVERT_FLIPPED_AND_MIRRORED
		};

		/**
		 * Definition of a vector holding conversion flags.
		 */
		using ConversionFlags = std::vector<ConversionFlag>;

		/**
		 * Definition of a boolean enum for copy preferences (to improve code readability).
		 */
		enum CopyPreference : bool
		{
			/// Tries to avoid copying the frame data whenever possible.
			CP_AVOID_COPY_IF_POSSIBLE = false,
			/// Forces a copy of the frame data in any case.
			CP_ALWAYS_COPY = true
		};

		/**
		 * Definition of a class storing options for frame conversions.
		 */
		class Options
		{
			public:

				/**
				 * Definition of individual types of options.
				 */
				enum OptionsType : uint32_t
				{
					/// Default conversion.
					OT_DEFAULT = 0u,
					/// Conversion with explicit alpha channel target value.
					OT_ALPHA_CHANNEL_TARGET_VALUE = 1u << 0u,
					/// Conversion with gamma correction.
					OT_GAMMA_CORRECTION = 1u << 1u,
					/// Conversion with black level, white balance, and gamma encoding
					OT_BLACKLEVEL_WHITEBALANCE_GAMMA = 1u << 2u,
					/// Approximated conversion.
					OT_APPROXIMATED = 1u << 2u,
				};

			public:

				/**
				 * Default constructor.
				 * @param allowApproximation True, to allow an approximated conversion if available
				 */
				explicit inline Options(const bool allowApproximation = false);

				/**
				 * Creates options for source image without alpha channel but a target image with alpha channel.
				 * @param alphaChannelTargetValue The uint8_t alpha channel value for the target image if the source image does not contain an alpha channel; ignored if the source image contains an alpha channel, with range [0, 255]
				 * @param allowApproximation True, to allow an approximated conversion if available
				 */
				explicit inline Options(const uint8_t alphaChannelTargetValue, const bool allowApproximation = false);

				/**
				 * Creates options for a conversion applying gamma correction.
				 * @param gamma The gamma value to be applied, with range (0, 2)
				 * @param allowApproximation True, to allow an approximated conversion if available
				 */
				explicit inline Options(const float gamma, const bool allowApproximation = false);

				/**
				 * Creates options for a conversion applying black level subtraction, white balance, and gamma encoding.
				 * @param blackLevel The black level value that is subtracted from each element of the raw image before any other operation, with range [0, 1024)
				 * @param whiteBalanceRed The white balancing scalar of the red channel, with range [0, infinity)
				 * @param whiteBalanceGreen The white balancing scalar of the green channel, with range [0, infinity)
				 * @param whiteBalanceBlue The white balancing scalar of the blue channel, with range [0, infinity)
				 * @param gamma The gamma value to be applied, with range (0, infinity)
				 * @param allowApproximation True, to allow an allowApproximation conversion if available
				 * @sa FrameConverterRGGB10_Packed::convertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT()
				 */
				explicit inline Options(const uint16_t blackLevel, const float whiteBalanceRed, const float whiteBalanceGreen, const float whiteBalanceBlue, const float gamma, const bool allowApproximation = false);

				/**
				 * Returns the options type.
				 * @return The options' type
				 */
				inline OptionsType optionsType() const;

				/**
				 * Returns the uint8_t alpha channel value for the target image if the source image does not contain an alpha channel; ignored if the source image contains an alpha channel.
				 * @return The alpha value for the target channel, with range [0, 255]
				 */
				inline uint8_t alphaChannelTargetValue() const;

				/**
				 * Returns the gamma value for a conversion with gamma correction/encoding.
				 * @return The gamma value, with range (0, 2) (OT_GAMMA_CORRECTION) or [0, infinity) (OT_BLACKLEVEL_WHITEBALANCE_GAMMA)
				 */
				inline float gamma() const;

				/**
				 * Returns the black level value for a conversion with black level correction.
				 * @return The black level value, with range [0, 1024)
				 */
				inline uint16_t blackLevel() const;

				/**
				 * Returns the white balance values for a conversion with white balance correction.
				 * @return The white balance values for the red, green, and blue channels, with range [0, infinity)
				 */
				inline const float* whiteBalance() const;

				/**
				 * Returns whether the conversion can be approximated.
				 * @return True, if so
				 */
				inline bool allowApproximation() const;

			protected:

				/// The options type.
				OptionsType optionsType_ = OT_DEFAULT;

				/// The alpha channel value for the target image if the source image does not contain an alpha channel, with range [0, 255]
				uint8_t alphaChannelTargetValue_ = 0xFFu;

				/// The gamma value for a conversion with gamma correction/encoding, with range (0, 2) (OT_GAMMA_CORRECTION) or [0, infinity) (OT_BLACKLEVEL_WHITEBALANCE_GAMMA)
				float gamma_ = 1.0f;

				/// The black level value that is subtracted from each element of the raw image before any other operation, with range [0, 1024)
				uint16_t blackLevel_ = 0u;

				/// The white balancing scalars of the red, green, and blue channels (in that order), with range [0, infinity)
				float whiteBalance_[3] = { 1.0f, 1.0f, 1.0f };
		};

	protected:

		/**
		 * This class implements a singleton-based map for function pointers of conversion functions.
		 */
		class OCEAN_CV_EXPORT ConversionFunctionMap : public Singleton<ConversionFunctionMap>
		{
			friend class Singleton<ConversionFunctionMap>;

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
					/// 1-plane uint8 with constant gamma correction to 1-plane uint8 conversion function.
					FT_1_UINT8_GAMMA_TO_1_UINT8,
					/// 1-plane uint8 to 1-plane uint8 with constant alpha channel conversion function.
					FT_1_UINT8_TO_1_UINT8_ALPHA,
					/// 1-plane uint8 to 1-plane uint8 conversion function with constant black level, white balance, and gamma values.
					FT_1_UINT8_TO_1_UINT8_BLACKLEVEL_WHITEBALANCE_GAMMA,
					/// 1-plane uint8 to 1-plane uint16 conversion function.
					FT_1_UINT8_TO_1_UINT16,
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
					FT_2_UINT8_TO_1_UINT8,
					/// 2-plane uint8 to 1-plane uint8 with constant alpha channel conversion function.
					FT_2_UINT8_TO_1_UINT8_ALPHA,
					/// 2-plane uint8 to 3-plane uint8 conversion function.
					FT_2_UINT8_TO_3_UINT8,
					/// 3-plane uint8 to 1-plane uint8 conversion function.
					FT_3_UINT8_TO_1_UINT8,
					/// 3-plane uint8 to 1-plane uint8 with constant alpha channel conversion function.
					FT_3_UINT8_TO_1_UINT8_ALPHA,
					/// 3-plane uint8 to 3-plane uint8 conversion function.
					FT_3_UINT8_TO_3_UINT8
				};

				/**
				 * Definition of a function pointer to a conversion function with one source plane and one target plane.
				 */
				template <typename TSource, typename TTarget>
				using OneSourceOneTargetConversionFunction = void(*)(const TSource* source, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with one source plane with gamma correction and one target plane.
				 */
				template <typename TSource, typename TTarget>
				using OneSourceGammaOneTargetConversionFunction = void(*)(const TSource* source, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const float gamma, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with one source plane and one target plane with constant alpha value.
				 */
				template <typename TSource, typename TTarget>
				using OneSourceOneTargetAlphaConversionFunction = void(*)(const TSource* source, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements, const uint8_t alphaValue, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with one source plane and one target plane with constant values for black level, white balance (red, green, blue), and gamma.
				 */
				template <typename TSource, typename TTarget>
				using OneSourceOneTargetBlackLevelWhiteBalanceGammaConversionFunction = void(*)(const TSource* source, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint16_t blackLevelValue, const float* whiteBalanceValues, const float gamma, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements, Worker* worker);

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
				 * Definition of a function pointer to a conversion function with two source planes and one target plane with constant alpha.
				 */
				template <typename TSource, typename TTarget>
				using TwoSourcesOneTargetAlphaConversionFunction = void(*)(const TSource* source0, const TSource* source1, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t targetPaddingElements, const uint8_t alphaValue, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with two source planes and three target planes.
				 */
				template <typename TSource, typename TTarget>
				using TwoSourcesThreeTargetConversionFunction = void(*)(const TSource* source0, const TSource* source1, TTarget* target0, TTarget* target1, TTarget* target2, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t targetPaddingElements0, const uint32_t targetPaddingElements1, const uint32_t targetPaddingElements2, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with three source planes and one target plane.
				 */
				template <typename TSource, typename TTarget>
				using ThreeSourcesOneTargetConversionFunction = void(*)(const TSource* source0, const TSource* source1, const TSource* source2, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t source2PaddingElements, const uint32_t targetPaddingElements, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with three source planes and one target plane with constant alpha value.
				 */
				template <typename TSource, typename TTarget>
				using ThreeSourcesOneTargetAlphaConversionFunction = void(*)(const TSource* source0, const TSource* source1, const TSource* source2, TTarget* target, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t source2PaddingElements, const uint32_t targetPaddingElements, const uint8_t alphaValue, Worker* worker);

				/**
				 * Definition of a function pointer to a conversion function with three source planes and three target planes.
				 */
				template <typename TSource, typename TTarget>
				using ThreeSourcesThreeTargetConversionFunction = void(*)(const TSource* source0, const TSource* source1, const TSource* source2, TTarget* target0, TTarget* target1, TTarget* target2, const uint32_t width, const uint32_t height, const ConversionFlag conversionFlag, const uint32_t source0PaddingElements, const uint32_t source1PaddingElements, const uint32_t source2PaddingElements, const uint32_t targetPaddingElements0, const uint32_t targetPaddingElements1, const uint32_t targetPaddingElements2, Worker* worker);

			protected:

				/**
				 * This class combines source pixel format, target pixel format, and options types.
				 */
				class ConversionTriple
				{
					public:

						/**
						 * Helper class for a hash function.
						 * The separate struct is necessary for compilers like GCC.
						 */
						struct Hash
						{
							/**
							 * Hash function.
							 * @param conversionTriple The conversion triple for which the hash will be determined
							 * @return The hash value
							 */
							inline size_t operator()(const ConversionTriple& conversionTriple) const;
						};

					public:

						/**
						 * Default constructor.
						 */
						ConversionTriple() = default;

						/**
						 * Creates a new object.
						 * @param sourcePixelFormat The pixel format of the source frame, must be valid
						 * @param targetPixelFormat The pixel format of the target frame, must be valid
						 * @param optionsType The type of the options for which the conversion is defined
						 */
						inline ConversionTriple(const FrameType::PixelFormat& sourcePixelFormat, const FrameType::PixelFormat& targetPixelFormat, const Options::OptionsType optionsType = Options::OT_DEFAULT);

						/**
						 * Returns whether two objects are identical.
						 * @param conversionTriple The second object
						 * @return True, if so
						 */
						inline bool operator==(const ConversionTriple& conversionTriple) const;

					public:

						/// The pixel format of the source frame, must be valid
						FrameType::PixelFormat sourcePixelFormat_ = FrameType::FORMAT_UNDEFINED;

						/// The pixel format of the target frame, must be valid
						FrameType::PixelFormat targetPixelFormat_ = FrameType::FORMAT_UNDEFINED;

						/// The type of the options for which the conversion is defined
						Options::OptionsType optionsType_ = Options::OT_DEFAULT;
				};

				/**
				 * This class is a wrapper for function pointers.
				 */
				class FunctionWrapper
				{
					friend class ConversionFunctionMap;

					public:

						/**
						 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT8_TO_1_UINT8 function.
						 * @param function The pointer to the conversion function, must be valid
						 */
						FunctionWrapper(const OneSourceOneTargetConversionFunction<uint8_t, uint8_t> function);

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
						 * Creates a new wrapper object and stores a function pointer to a FT_1_UINT8_TO_1_UINT16 function.
						 * @param function The pointer to the conversion function, must be valid
						 */
						FunctionWrapper(const OneSourceOneTargetConversionFunction<uint8_t, uint16_t> function);

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
						 * Creates a new wrapper object and stores a function pointer to a FT_2_UINT8_TO_1_UINT8_ALPHA function.
						 * @param function The pointer to the conversion function, must be valid
						 */
						FunctionWrapper(const TwoSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> function);

						/**
						 * Creates a new wrapper object and stores a function pointer to a FT_2_UINT8_TO_3_UINT8 function.
						 * @param function The pointer to the conversion function, must be valid
						 */
						FunctionWrapper(const TwoSourcesThreeTargetConversionFunction<uint8_t, uint8_t> function);

						/**
						 * Creates a new wrapper object and stores a function pointer to a FT_3_UINT8_TO_1_UINT8 function.
						 * @param function The pointer to the conversion function, must be valid
						 */
						FunctionWrapper(const ThreeSourcesOneTargetConversionFunction<uint8_t, uint8_t> function);

						/**
						 * Creates a new wrapper object and stores a function pointer to a FT_3_UINT8_TO_1_UINT8_ALPHA function.
						 * @param function The pointer to the conversion function, must be valid
						 */
						FunctionWrapper(const ThreeSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> function);

						/**
						 * Creates a new wrapper object and stores a function pointer to a FT_3_UINT8_TO_3_UINT8 function.
						 * @param function The pointer to the conversion function, must be valid
						 */
						FunctionWrapper(const ThreeSourcesThreeTargetConversionFunction<uint8_t, uint8_t> function);

					protected:

						/// The function pointer of the conversion function.
						const void* function_;

						/// The type of the conversion function.
						const FunctionType functionType_;
				};

				/**
				 * Definition of a map mapping pairs or pixel formats to function pointers.
				 */
				typedef std::unordered_map<ConversionTriple, FunctionWrapper, ConversionTriple::Hash> FormatPair2FunctionWrapperMap;

			public:

				/**
				 * Returns the function pointer for a source and target pixel format.
				 * @param sourcePixelFormat The pixel format of the source frame, must be valid
				 * @param targetPixelFormat The pixel format of the target frame, must be valid
				 * @param functionType The resulting type of the conversion function
				 * @param options The options for the conversion
				 * @return The function pointer, nullptr if the combination of source and target pixel format is not supported
				 */
				const void* function(const FrameType::PixelFormat& sourcePixelFormat, const FrameType::PixelFormat& targetPixelFormat, FunctionType& functionType, const Options& options) const;

			protected:

				/**
				 * Creates a new map object and initializes all function pointers.
				 */
				ConversionFunctionMap();

			protected:

				/// The map mapping pairs or pixel formats to function pointers.
				FormatPair2FunctionWrapperMap formatPair2FunctionWrapperMap_;
		};

	protected:

		/**
		 * Definition of a function pointer to a function able to convert one image row from one generic pixel format to another generic pixel format.
		 * @param sourceRow The row in the source frame, must be valid
		 * @param targetRow The row in the target frame, must be valid
		 * @param width The number of pixels to convert, with range [1, infinity)
		 * @param options Optional options which are necessary for the conversion, otherwise nullptr
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t' or 'float'
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t' or 'float'
		 */
		template <typename TSource, typename TTarget>
		using RowConversionFunction = void (*)(const TSource* sourceRow, TTarget* targetRow, const size_t width, const void* options);

		/**
		 * Definition of a function pointer to a function able to convert multiple image row from an arbitrary pixel format to another arbitrary pixel format.
		 * @param sources The memory pointers defining the source rows, must be valid
		 * @param targets The memory pointers defining the target rows, must be valid
		 * @param multipleRowIndex The index of the rows to be converted, with range [0, height / multipleRowsPerIteration - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options Optional options which are necessary for the conversion, otherwise nullptr
		 */
		using MultipleRowsConversionFunction = void (*)(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Definition of a function pointer to a function able to reverse the order of pixels in an image row with a generic pixel format.
		 * @param inputRow The row to reverse, must be valid
		 * @param targetRow The row receiving the reversed pixels, must be different from 'inputRow', must be valid
		 * @param width The number of pixels to reverse, with range [1, infinity)
		 * @tparam T The data type of each pixel channel, e.g., 'uint8_t' or 'float'
		 */
		template <typename T>
		using RowReversePixelOrderFunction = void (*)(const T* inputRow, T* targetRow, const size_t width);

		/**
		 * Definition of a function pointer to a function able to reverse the order of pixels in an image row with a generic pixel format in-place.
		 * @param row The row to reverse, must be valid
		 * @param width The number of pixels to reverse, with range [1, infinity)
		 * @tparam T The data type of each pixel channel, e.g., 'uint8_t' or 'float'
		 */
		template <typename T>
		using RowReversePixelOrderInPlaceFunction = void (*)(T* row, const size_t width);

		/**
		 * Definition of the parameters used by the function for row-wise conversion of RGGB14_PACKED to RGB24/BGR24
		 */
		struct RGGB10ToRGB24AdvancedOptions
		{
			/// The black level that needs to be subtracted from the unpacked pixel values, with range [0, 1024)
			uint16_t blackLevel = 0u;

			/// The white balance factors for the red, green, and blue channel as 7 bit fixed-point numbers; the order of the channels is the same as in the target frame
			unsigned int whiteBalance7[3] = { 128u, 128u, 128u };

			/// Pointer to the gamma lookup-table, cf. `FrameConverterY10_Packed::LookupTableManager`
			const uint8_t* gammaLookupValues = nullptr;

			/// The number of padding elements of the source frame
			unsigned int sourcePaddingElements = 0u;

			/// The number of padding elements of the target frame
			unsigned int targetPaddingElements = 0u;
		 };

	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_EXPORT Comfort
		{
			public:

				/**
				 * Returns whether the convert function of this class supports the conversion of a frame with one pixel format to a new frame with other pixel format.
				 * @param sourceType The frame type of the source frame, must be valid
				 * @param targetPixelFormat The pixel format of the target frame, must be valid
				 * @param options The options to be used for conversion
				 * @return True, if so
				 * @see convert().
				 */
				static bool isSupported(const FrameType& sourceType, const FrameType::PixelFormat targetPixelFormat, const Options& options = Options());

				/**
				 * Converts a frame with arbitrary dimension, pixel format and pixel origin into a frame with the same dimension, but different pixel format or pixel origin.
				 * @param source The source frame to convert, must be valid
				 * @param targetPixelFormat The pixel format of the target frame, must be valid
				 * @param targetPixelOrigin The pixel origin of the target frame, must be valid
				 * @param target The resulting target frame, the frame will be modified if the frame type is not compatible, or if the target frame is not owner of the frame data, or if the target frame is a read-only frame, can be invalid
				 * @param forceCopy True, if the resulting target image is expected to be the owner of the image data, otherwise the source frame will be the owner of the image data if possible
				 * @param worker Optional worker object to distribute the conversion computation to different CPU cores
				 * @param options The options to be used for conversion
				 * @return True, if the frame type conversion is supported and succeeded
				 *
				 * Here is an example showing how to use this function:
				 * @code
				 * bool function(const Frame& anyFrame)
				 * {
				 *     // we do not know which pixel format (and pixel origin) the given frame has
				 *     // however, we know that we need e.g., a grayscale frame with 8 bit and pixel origin in the upper left corner of the target frame
				 *
				 *     Frame yFrame;
				 *     if (!FrameConverter::Comfort::convert(anyFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, FrameConverter::CP_AVOID_COPY_IF_POSSIBLE)) // we try to avoid a copy if possible
				 *     {
				 *         // the given frame could not be converted into a Y8 frame, so we stop here
				 *         return false;
				 *     }
				 *
				 *     // from now on we have access to a Y8 frame, it may be
				 *     // - a frame not owning the frame data but referencing the memory only (in case 'anyFrame' provided a plain Y8 block)
				 *     // - a frame owning the frame data if the given image was converted to a Y8 frame
				 *
				 *     // we can use the memory as long as anyFrame exists
				 *     const uint8_t* data = yFrame.constdata<uint8_t>();
				 *
				 *     // do something here
				 *
				 *     return true;
				 * }
				 * @endcode
				 * @see isSupported(), convertAndCopy().
				 */
				static bool convert(const Frame& source, const FrameType::PixelFormat targetPixelFormat, const FrameType::PixelOrigin targetPixelOrigin, Frame& target, const bool forceCopy = true, Worker* worker = nullptr, const Options& options = Options());

				/**
				 * Converts a frame with arbitrary dimension, pixel format and pixel origin into a frame with the same dimension and pixel origin, but different pixel format.
				 * @param source The source frame to convert, must be valid
				 * @param targetPixelFormat The pixel format of the target frame, must be valid
				 * @param target The resulting target frame, the frame will be modified if the frame type is not compatible, or if the target frame is not owner of the frame data, or if the target frame is a read-only frame, can be invalid
				 * @param forceCopy True, if the resulting target image is expected to be the owner of the image data, otherwise the source frame will be the owner of the image data if possible
				 * @param worker Optional worker object to distribute the conversion computation to different CPU cores
				 * @param options The options to be used for conversion
				 * @return True, if the frame type conversion is supported and succeeded
				 *
				 * Here is an example showing how to use this function:
				 * @code
				 * bool function(const Frame& anyFrame)
				 * {
				 *     // we do not know which pixel format (and pixel origin) the given frame has
				 *     // however, we know that we need e.g., a grayscale frame with 8 bit with any pixel origin
				 *
				 *     Frame yFrame;
				 *     if (!FrameConverter::Comfort::convert(anyFrame, FrameType::FORMAT_Y8, yFrame, FrameConverter::CP_AVOID_COPY_IF_POSSIBLE)) // we try to avoid a copy if possible
				 *     {
				 *         // the given frame could not be converted into a Y8 frame, so we stop here
				 *         return false;
				 *     }
				 *
				 *     // from now on we have access to a Y8 frame, it may be
				 *     // - a frame not owning the frame data but referencing the memory only (in case 'anyFrame' provided a plain Y8 block)
				 *     // - a frame owning the frame data if the given image was converted to a Y8 frame
				 *
				 *     // we can use the memory as long as anyFrame exists
				 *     const uint8_t* data = yFrame.constdata<uint8_t>();
				 *
				 *     // do something here
				 *
				 *     return true;
				 * }
				 * @endcode
				 * @see isSupported(), convertAndCopy().
				 */
				static inline bool convert(const Frame& source, const FrameType::PixelFormat targetPixelFormat, Frame& target, const bool forceCopy = true, Worker* worker = nullptr, const Options& options = Options());

				/**
				 * Converts a frame with arbitrary dimension, pixel format and pixel origin into a frame with the same dimension and pixel format, but different pixel origin.
				 * @param source The source frame to convert, must be valid
				 * @param targetPixelOrigin The pixel origin of the target frame, must be valid
				 * @param target The resulting target frame, the frame will be modified if the frame type is not compatible, or if the target frame is not owner of the frame data, or if the target frame is a read-only frame, can be invalid
				 * @param forceCopy True, if the resulting target image is expected to be the owner of the image data, otherwise the source frame will be the owner of the image data if possible
				 * @param worker Optional worker object to distribute the conversion computation to different CPU cores
				 * @param options The options to be used for conversion
				 * @return True, if the frame type conversion is supported and succeeded
				 * @see isSupported(), convertAndCopy().
				 */
				static inline bool convert(const Frame& source, const FrameType::PixelOrigin targetPixelOrigin, Frame& target, const bool forceCopy = true, Worker* worker = nullptr, const Options& options = Options());

				/**
				 * Converts a frame with arbitrary dimension, pixel format and pixel origin into a frame with the same dimension but different pixel format or pixel origin.
				 * This function does always copy the memory to the already existing memory of the target frame.
				 * @param source The source frame to convert, must be valid
				 * @param target The target frame which will receive the converted source image information, must contain writable memory, must be valid
				 * @param worker Optional worker object to distribute the conversion computation to different CPU cores
				 * @param options The options to be used for conversion
				 * @return True, if the frame type conversion is supported and succeeded
				 *
				 * Here is an example showing how to use this function:
				 * @code
				 * bool function(const Frame& anySourceFrame)
				 * {
				 *     uint8_t* targetMemoryRGB24 = ...;
				 *     const unsigned int targetMemoryPaddingElements = ...;
				 *
				 *     const FrameType targetFrameType(anySourceFrame.width(), anySourceFrame.height(), FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);
				 *
				 *     Frame targetFrame(targetFrameType, targetMemoryRGB24, Frame::CM_USE_KEEP_LAYOUT, targetMemoryPaddingElements);
				 *
				 *     if (!FrameConverter::Comfort::convertAndCopy(anySourceFrame, targetFrame))
				 *     {
				 *         // there is no converter from the source pixel format to the target pixel format, so we stop here
				 *         return false;
				 *     }
				 *
				 *     // do something here with the RGB memory in `targetMemoryRGB24`
				 *
				 *     return true;
				 * }
				 * @endcode
				 * @see isSupported(), convert().
				 */
				static bool convertAndCopy(const Frame& source, Frame& target, Worker* worker = nullptr, const Options& options = Options());

				/**
				 * Converts / changes a frame with arbitrary dimension, pixel format and pixel origin into a frame with the same dimension but different pixel format or pixel origin.
				 * @param frame The frame to convert, must be valid
				 * @param targetPixelFormat The pixel format of the target frame, must be valid
				 * @param targetPixelOrigin The pixel origin of the target frame, must be valid
				 * @param forceCopy True, if the resulting target image is expected to are the owner of the image data, otherwise the source frame will be the owner of the image data if possible
				 * @param worker Optional worker object to distribute the conversion computation to different CPU cores
				 * @param options The options to be used for conversion
				 * @return True, if the frame type conversion is supported and succeeded
				 */
				static inline bool change(Frame& frame, const FrameType::PixelFormat targetPixelFormat, const FrameType::PixelOrigin targetPixelOrigin, const bool forceCopy = true, Worker* worker = nullptr, const Options& options = Options());

				/**
				 * Converts / changes a frame with arbitrary dimension, pixel format and pixel origin into a frame with the same dimension and same pixel origin but different pixel format.
				 * @param frame The frame to convert, must be valid
				 * @param targetPixelFormat The pixel format of the target frame, must be valid
				 * @param forceCopy True, if the resulting target image is expected to are the owner of the image data, otherwise the source frame will be the owner of the image data if possible
				 * @param worker Optional worker object to distribute the conversion computation to different CPU cores
				 * @param options The options to be used for conversion
				 * @return True, if the frame type conversion is supported and succeeded
				 */
				static inline bool change(Frame& frame, const FrameType::PixelFormat targetPixelFormat, const bool forceCopy = true, Worker* worker = nullptr, const Options& options = Options());

				/**
				 * Converts / changes a frame with arbitrary dimension, pixel format and pixel origin into a frame with the same dimension and same pixel format but different pixel origin.
				 * @param frame The frame to convert, must be valid
				 * @param targetPixelOrigin The pixel origin of the target frame, must be valid
				 * @param forceCopy True, if the resulting target image is expected to are the owner of the image data, otherwise the source frame will be the owner of the image data if possible
				 * @param worker Optional worker object to distribute the conversion computation to different CPU cores
				 * @param options The options to be used for conversion
				 * @return True, if the frame type conversion is supported and succeeded
				 */
				static inline bool change(Frame& frame, const FrameType::PixelOrigin targetPixelOrigin, const bool forceCopy = true, Worker* worker = nullptr, const Options& options = Options());
		};

		/**
		 * Casts the pixel values from one frame type to another frame type.
		 * The source frame must be a zipped frame e.g., FrameType::FORMAT_Y8, FrameType::FORMAT_RGB24, ...<br>
		 * Beware: This function does not handle any out of range issues and does not apply rounding.<br>
		 * This function mainly does the following:
		 * @code
		 * for each pixel and channel:
		 *     targetValue = TTarget(sourceValue)
		 * @endcode
		 * @param source The source frame to be casted, must be valid
		 * @param target The target frame receiving the casted pixel values, must be valid (and not overlap the source frame)
		 * @param width The width of the source (and target frame) in pixel, with range [1, infinity)
		 * @param height The height of the source (and target frame) in pixel, with range [1, infinity)
		 * @param channels The number of channels the source frame (and target frame) has, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each pixel channel of the source frame, e.g., 'uint8_t', 'int', 'float', ...
		 * @tparam TTarget The data type of each pixel channel of the target frame, e.g., 'uint8_t', 'int', 'float', ...
		 *
		 * Here is an example how to use this function:
		 * @code
		 * void function()
		 * {
		 *     // we have a source frame e.g., with pixel format RGB 24 bit, 'uint8_t' for each pixel channel
		 *     Frame sourceFrame(FrameType(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		 *
		 *     // set the pixel values of sourceFrame here ...
		 *
		 *     // we want to cast this frame to a frame with 32 bit floating point values
		 *     // we use the frame type of the source as pattern and change the pixel format only
		 *     Frame targetFrame(FrameType(sourceFrame, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, 3u)));
		 *
		 *     // now we simply cast the values from the source frame to the target frame
		 *     FrameConverter::cast<uint8_t, float>(sourceFrame.constdata<uint8_t>(), targetFrame.data<float>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels());
		 *
		 *     // now we can access the floating point pixel values of target
		 *     const float* floatRGBValues = targetFrame.constdata<float>();
		 * }
		 * @endcode
		 * @see normalizedCast().
		 */
		template <typename TSource, typename TTarget>
		static void cast(const TSource* __restrict source, TTarget* __restrict target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Casts the pixel values from one frame type to another frame type but also normalizes the casted source values before assigning them (by scaling and offsetting).
		 * The source frame must be a zipped frame e.g., FrameType::FORMAT_Y8, FrameType::FORMAT_RGB24, ...<br>
		 * Beware: This function does not handle any out of range issues and does not apply rounding.<br>
		 * This function mainly does the following:
		 * @code
		 * for each pixel and channel:
		 *     targetValue = TTarget(sourceValue) * multiplicationFactor + offset
		 * @endcode
		 * @param source The source frame to be casted, must be valid
		 * @param target The target frame receiving the casted pixel values, must be valid (and not overlap the source frame)
		 * @param width The width of the source (and target frame) in pixel, with range [1, infinity)
		 * @param height The height of the source (and target frame) in pixel, with range [1, infinity)
		 * @param channels The number of channels the source frame (and target frame) has, with range [1, infinity)
		 * @param multiplicationFactor The multiplication factor (with data type TTarget) which will be multiplied with each source values before the value is assigned, should not be zero
		 * @param offset The offset (with data type TTarget) that is added to each value after the conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each pixel channel of the source frame, e.g., 'uint8_t', 'int', 'float', ...
		 * @tparam TTarget The data type of each pixel channel of the target frame, e.g., 'uint8_t', 'int', 'float', ...
		 *
		 * Here is an example how to use this function:
		 * @code
		 * void function()
		 * {
		 *     // we have a source frame e.g., with pixel format RGB 24 bit, 'uint8_t' for each pixel channel
		 *     Frame sourceFrame(FrameType(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		 *
		 *     // set the pixel values of sourceFrame here ...
		 *
		 *     // we want to cast this frame to a frame with 32 bit floating point values
		 *     // we use the frame type of the source as pattern and change the pixel format only
		 *     Frame targetFrame(FrameType(sourceFrame, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, 3u)));
		 *
		 *     // now we normalize the source values by 1/255 and assign the values - so that we get floating point values with range [0, 1]
		 *     FrameConverter::normalizedCast<uint8_t, float>(sourceFrame.constdata<uint8_t>(), targetFrame.data<float>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), 1.0f / 255.0f, 0);
		 *
		 *     // now we can access the normalized floating point pixel values of target
		 *     const float* floatRGBValues = targetFrame.constdata<float>();
		 * }
		 * @endcode
		 * @see cast().
		 */
		template <typename TSource, typename TTarget>
		static void normalizedCast(const TSource* __restrict source, TTarget* __restrict target, const unsigned int width, const unsigned int height, const unsigned int channels, const TTarget multiplicationFactor, const TTarget offset, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Copies a sub-frame of a given frame into a second frame while both frames might have an individual number of padding elements at the end of each row.
		 * The dimension of the sub-frame must fit into the source and target frame.
		 * @param source The source frame from which the sub-frame will be copied, must be valid
		 * @param target The target frame to which the sub-frame will be copied, must be valid
		 * @param sourceWidth Width of the entire source frame in pixels, with range [1, infinity)
		 * @param sourceHeight Height of the entire source frame in pixels, with range [1, infinity)
		 * @param targetWidth Width of the entire target frame in pixels, with range [1, infinity)
		 * @param targetHeight Height of the entire target frame in pixels, with range [1, infinity)
		 * @param channels Number of data channels of the given source (and target) frame, with range [1, infinity)
		 * @param sourceLeft Horizontal start position of the sub-frame inside the source frame in pixels, with range [0, sourceWidth - 1]
		 * @param sourceTop Vertical start position of the sub-frame inside the source frame in pixels, with range [0, sourceHeight - 1]
		 * @param targetLeft Horizontal start position of the sub-frame inside the target frame in pixels, with range [0, targetWidth -1]
		 * @param targetTop Vertical start position of the sub-frame inside the target frame in pixels, with range [0, targetHeight - 1]
		 * @param width The width of the sub-frame in pixel, with range [1, min(sourceWidth - sourceLeft, targetWidth - targetLeft)]
		 * @param height The height of the sub-frame in pixel, with range [1, min(sourceHeight - sourceTop, targetHeight - targetTop)]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each element
		 */
		template <typename T>
		static bool subFrame(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Copies pixels from one sub-frame to another if the pixels are part of a mask; input may use padding.
		 * The behavior of this function can be described as:
		 * <pre>
		 * target[i] = mask[i] == maskValue ? source[i] : target[i]
		 * </pre>
		 * The dimension of the sub-frame must fit into the source and target frame. The mask must have the same size as the sub-frame.
		 * @param sourceFrame The source frame from which the sub-frame will be copied, must be valid
		 * @param targetFrame The target frame to which the sub-frame will be copied, must be valid
		 * @param maskFrame The binary mask that is used to indicate which source pixels to copy to the target frame, must be valid, have one channel, and have the same size of the region that is copied (`subFrameWidth` x `subFrameHeight`)
		 * @param sourceLeft Horizontal start position of the sub-frame inside the source frame in pixels, with range [0, sourceWidth - 1]
		 * @param sourceTop Vertical start position of the sub-frame inside the source frame in pixels, with range [0, sourceHeight - 1]
		 * @param targetLeft Horizontal start position of the sub-frame inside the target frame in pixels, with range [0, targetWidth -1]
		 * @param targetTop Vertical start position of the sub-frame inside the target frame in pixels, with range [0, targetHeight - 1]
		 * @param subFrameWidth Width of the sub-frame in pixel, with range [0, min(sourceWidth - sourceLeft, targetWidth - targetLeft)]
		 * @param subFrameHeight Height of the sub-frame in pixel, with range [1, min(sourceHeight - sourceTop, targetHeight - targetTop)]
		 * @param maskValue Optional value which indicates which pixel value should be interpreted as the foreground (and copied)
		 * @return True, if succeeded
		 * @tparam T The data type of the elements of the source and target frames
		 */
		template <typename T>
		static bool subFrameMask(const Frame& sourceFrame, Frame& targetFrame, const Frame& maskFrame, const uint32_t sourceLeft, const uint32_t sourceTop, const uint32_t targetLeft, const uint32_t targetTop, const uint32_t subFrameWidth, const uint32_t subFrameHeight, const uint8_t maskValue = 0u);

		/**
		 * Copies pixels from one sub-frame to another if the pixels are part of a mask; input may use padding.
		 * The behavior of this function can be described as:
		 * <pre>
		 * target[i] = mask[i] == maskValue ? source[i] : target[i]
		 * </pre>
		 * The dimension of the sub-frame must fit into the source and target frame. The mask must have the same size as the sub-frame.
		 * @param source The source frame from which the sub-frame will be copied, must be valid
		 * @param target The target frame to which the sub-frame will be copied, must be valid
		 * @param mask The binary mask that is used to indicate which source pixels to copy to the target frame, must be valid, have one channel, and have the same size of the region that is copied (`subFrameWidth` x `subFrameHeight`)
		 * @param sourceWidth Width of the entire source frame in pixels, with range [1, infinity)
		 * @param sourceHeight Height of the entire source frame in pixels, with range [1, infinity)
		 * @param targetWidth Width of the entire target frame in pixels, with range [1, infinity)
		 * @param targetHeight Height of the entire target frame in pixels, with range [1, infinity)
		 * @param channels Number of data channels of the given source (and target) frame, with range [1, infinity)
		 * @param sourceLeft Horizontal start position of the sub-frame inside the source frame in pixels, with range [0, sourceWidth - 1]
		 * @param sourceTop Vertical start position of the sub-frame inside the source frame in pixels, with range [0, sourceHeight - 1]
		 * @param targetLeft Horizontal start position of the sub-frame inside the target frame in pixels, with range [0, targetWidth -1]
		 * @param targetTop Vertical start position of the sub-frame inside the target frame in pixels, with range [0, targetHeight - 1]
		 * @param subFrameWidth Width of the sub-frame in pixel, with range [1, min(sourceWidth - sourceLeft, targetWidth - targetLeft)]
		 * @param subFrameHeight Height of the sub-frame in pixel, with range [1, min(sourceHeight - sourceTop, targetHeight - targetTop)]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, with range [0, infinity)
		 * @param maskPaddingElements Optional number of padding elements at the end of each source row, with range [0, infinity)
		 * @param maskValue Optional value which indicates which pixel value should be interpreted as the foreground (and copied)
		 * @return True, if succeeded
		 * @tparam T The data type of the elements of the source and target frames
		 */
		template <typename T>
		static bool subFrameMask(const T* source, T* target, const uint8_t* mask, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int maskPaddingElements, const uint8_t maskValue = 0u);

		/**
		 * Copies a small patch area of a given frame into a buffer holding only the entire patch.
		 * @param source The source frame from which the patch will be copied, must be valid
		 * @param buffer The target buffer to which the frame content will be copied, must be valid
		 * @param width The width of the source frame in pixels, with range [patchSize, infinity)
		 * @param channels Number of data channels of the given source frame, with range [1, infinity)
		 * @param x Horizontal center position of the patch to be copied in pixel, with range [patchSize/2, width - patchSize/2 - 1]
		 * @param y Vertical center position of the patch to be copied in pixel, with range [patchSize/2, height - patchSize/2 - 1]
		 * @param patchSize The side length of the patch to be copied in pixel, with range [1, infinity), must be odd
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param bufferPaddingElements The number of padding elements at the end of each buffer row, in elements, with range [0, infinity)
		 * @tparam T The data type of the elements of the source frame and the target buffer, e.g., 'uint8_t', or 'float'
		 */
		template <typename T>
		static inline void patchFrame(const T* source, T* buffer, const unsigned int width, const unsigned int channels, const unsigned int x, const unsigned int y, const unsigned int patchSize, const unsigned int sourcePaddingElements, const unsigned int bufferPaddingElements);

		/**
		 * Copies a small patch area of a frame into a buffer holding only the entire patch.
		 * Pixels in the patch mapping to positions outside the frame are mirrored into the frame.<br>
		 * @param source The source frame from which the patch will be copied, must be valid
		 * @param buffer The target buffer to which the frame content will be copied, must be valid
		 * @param width The width of the source frame in pixels, with range [patchSize/2+1, infinity)
		 * @param height The height of the source frame in pixels, with range [patchSize/2+1, infinity)
		 * @param x Horizontal center position of the patch to be copied in pixel, with range [0, width - 1]
		 * @param y Vertical center position of the patch to be copied in pixel, with range [0, height - 1]
		 * @param patchSize The side length of the patch to be copied in pixel, with range [1, infinity), must be odd
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param bufferPaddingElements The number of padding elements at the end of each buffer row, in elements, with range [0, infinity)
		 * @tparam T The data type of the elements of the source frame and the target buffer, e.g., 'uint8_t', or 'float'
		 * @tparam tChannels Number of data channels of the given source frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void patchFrameMirroredBorder(const T* source, T* buffer, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const unsigned int patchSize, const unsigned int sourcePaddingElements, const unsigned int bufferPaddingElements);

		/**
		 * Returns the 3x4 color space transformation matrix from full range RGB24 to full range YUV24 using BT.601, analog RGB to (analog) YPbPr.
		 * Below the precise transformation matrices are given:
		 * <pre>
		 * RGB input value range:  [0, 255]x[0, 255]x[0, 255]
		 * YUV output value range: [0, 255]x[0, 255]x[0, 255]
		 *
		 * | Y |   |  0.299       0.587       0.114        0 |   | R |
		 * | U | = | -0.168736   -0.331264    0.5        128 | * | G |
		 * | V |   |  0.5        -0.418688   -0.081312   128 |   | B |
		 *                                                       | 1 |
		 * Approximation with 7 bit precision:
		 *       | Y |     |  38     75    15      0   128 |   | R |
		 * 128 * | U |  =  | -22    -42    64    128 * 128 | * | G |
		 *       | V |     |  64    -54   -10    128 * 128 |   | B |
		 *                                                     | 1 |
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrix_FullRangeRGB24_To_FullRangeYVU24_BT601().
		 */
		static MatrixD transformationMatrix_FullRangeRGB24_To_FullRangeYUV24_BT601();

		/**
		 * Returns the 3x4 color space transformation matrix from full range RGB24 to full range YVU24 using BT.601, analog RGB to (analog) YPbPr.
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrix_FullRangeRGB24_To_FullRangeYUV24_BT601().
		 */
		static MatrixD transformationMatrix_FullRangeRGB24_To_FullRangeYVU24_BT601();

		/**
		 * Returns the 3x4 color space transformation matrix from full range RGB24 to limited range YUV24 using BT.601, analog RGB to (digital) YCbCr.
		 * Below the precise transformation matrices are given:
		 * <pre>
		 * RGB input value range:  [0, 255]x[0, 255]x[0, 255]
		 * YUV output value range: [16, 235]x[16, 240]x[16, 240]
		 *
		 * | Y |   |  0.2578125   0.5039063   0.09765625  16.0  |   | R |
		 * | U | = | -0.1484375  -0.2890625   0.4375      128.0 | * | G |
		 * | V |   |  0.4375     -0.3671875  -0.0703125   128.0 |   | B |
		 *                                                          | 1 |
		 * Approximation with 7 bit precision:
		 *       | Y |     |  33     64    13     16 * 128 |   | R |
		 * 128 * | U |  =  | -19    -37    56    128 * 128 | * | G |
		 *       | V |     |  56    -47   -9     128 * 128 |   | B |
		 *                                                     | 1 |
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrix_FullRangeRGB24_To_LimitedRangeYVU24_BT601().
		 */
		static MatrixD transformationMatrix_FullRangeRGB24_To_LimitedRangeYUV24_BT601();

		/**
		 * Returns the 3x4 color space transformation matrix from full range RGB24 to limited range YVU24 using BT.601, analog RGB to (digital) YCbCr.
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrix_FullRangeRGB24_To_LimitedRangeYUV24_BT601().
		 */
		static MatrixD transformationMatrix_FullRangeRGB24_To_LimitedRangeYVU24_BT601();

		/**
		 * Returns the color space transformation matrix from full range YUV24 to full range BGR24 using BT.601, (analog) YPbPr to analog BGR.
		 * Below the precise transformation matrices are given:
		 * <pre>
		 * YUV input value range:  [0, 255]x[0, 255]x[0, 255]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 *
		 * | B |     | 1.0     1.772      0.0       -226.816   |   | Y |
		 * | G |  =  | 1.0    -0.34414   -0.71414    135.45984 | * | U |
		 * | R |     | 1.0     0.0        1.402     -179.456   |   | V |
		 *                                                         | 1 |
		 *
		 * Approximation with 6 bit precision:
		 *       | B |     | 64   113     0 |   |    Y    |
		 *  64 * | G |  =  | 64   -22   -46 | * | U - 128 |
		 *       | R |     | 64    0     90 |   | V - 128 |
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_Android().
		 */
		static MatrixD transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_BT601();

		/**
		 * Returns the color space transformation matrix from full range YUV24 to full range RGB24 using BT.601, (analog) YPbPr to analog RGB.
		 * Below the precise transformation matrices are given:
		 * <pre>
		 * YUV input value range:  [0, 255]x[0, 255]x[0, 255s]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 *
		 * | R |     | 1.0     0.0        1.402     -179.456   |   | Y |
		 * | G |  =  | 1.0    -0.34414   -0.71414    135.45984 | * | U |
		 * | B |     | 1.0     1.772      0.0       -226.816   |   | V |
		 *                                                         | 1 |
		 *
		 * Approximation with 6 bit precision:
		 *       | R |     | 64    0     90 |   |    Y    |
		 *  64 * | G |  =  | 64   -22   -46 | * | U - 128 |
		 *       | B |     | 64   113     0 |   | V - 128 |
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_Android().
		 */
		static MatrixD transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601();

		/**
		 * Returns the color space transformation matrix from full range YUV24 to full range BGR24 similar to BT.601, (analog) YPbPr to analog BGR.
		 * This transformation matrix is close to the official BT.601 standard and used on Android for conversion from Y'UV420sp (NV21).<br>
		 * @see transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_Android().
		 * @return The 3x4 transformation matrix
		 */
		static MatrixD transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_Android();

		/**
		 * Returns the color space transformation matrix from full range YUV24 to full range RGB24 similar to BT.601, (analog) YPbPr to analog RGB.
		 * This transformation matrix is close to the official BT.601 standard and used on Android for conversion from Y'UV420sp (NV21).<br>
		 * The conversion can be found in Android's source code: /media/libstagefright/yuv/YUVImage.cpp<br>
		 * Below the precise transformation matrices are given:
		 * <pre>
		 * YUV input value range:  [0, 255]x[0, 255]x[0, 255s]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 *
		 * | R |     | 1.0     0.0         1.370705  |   |    Y    |     | 1.0     0.0         1.370705   -175.45024  |   | Y |
		 * | G |  =  | 1.0    -0.337633   -0.698001  | * | U - 128 |  =  | 1.0    -0.337633   -0.698001    132.561152 | * | U |
		 * | B |     | 1.0     1.732446    0.0       |   | V - 128 |     | 1.0     1.732446    0.0        -221.753088 |   | V |
		 *                                                                                                                | 1 |
		 *
		 * Approximation with 6 bit precision:
		 *       | R |     | 64    0     88 |   |    Y    |
		 *  64 * | G |  =  | 64   -22   -45 | * | U - 128 |
		 *       | B |     | 64   111     0 |   | V - 128 |
		 * </pre>
		 * @see transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601(), transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_Android().
		 * @return The 3x4 transformation matrix
		 */
		static MatrixD transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_Android();

		/**
		 * Returns the color space transformation matrix from full range YVU24 to full range BGR24 similar to BT.601, (analog) YPbPr to analog BGR.
		 * This transformation matrix is close to the official BT.601 standard and used on Android for conversion from Y'UV420sp (NV21).<br>
		 * @see transformationMatrix_FullRangeYVU24_To_FullRangeRGB24_Android().
		 * @return The 3x4 transformation matrix
		 */
		static MatrixD transformationMatrix_FullRangeYVU24_To_FullRangeBGR24_Android();

		/**
		 * Returns the color space transformation matrix from full range YVU24 to full range RGB24 similar to BT.601, (analog) YPbPr to analog RGB.
		 * This transformation matrix is close to the official BT.601 standard and used on Android for conversion from Y'UV420sp (NV21).<br>
		 * The conversion can be found in Android's source code: /media/libstagefright/yuv/YUVImage.cpp<br>
		 * Below the precise transformation matrices are given:
		 * <pre>
		 * YUV input value range:  [0, 255]x[0, 255]x[0, 255s]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 *
		 * | R |     | 1.0     1.370705    0.0      |   |    Y    |     | 1.0    1.370705    0.0        -175.45024  |   | Y |
		 * | G |  =  | 1.0    -0.698001   -0.337633 | * | V - 128 |  =  | 1.0   -0.698001   -0.337633    132.561152 | * | V |
		 * | B |     | 1.0     0.0         1.732446 |   | U - 128 |     | 1.0    0.0         1.732446   -221.753088 |   | U |
		 *                                                                                                              | 1 |
		 *
		 * Approximation with 6 bit precision:
		 *       | R |     | 64   88   0  |   |    Y    |
		 *  64 * | G |  =  | 64  -45  -22 | * | V - 128 |
		 *       | B |     | 64    0  111 |   | U - 128 |
		 * </pre>
		 * @return The 3x4 transformation matrix
		 */
		static MatrixD transformationMatrix_FullRangeYVU24_To_FullRangeRGB24_Android();

		/**
		 * Returns the color space transformation matrix from limited range YUV24 to full range RGB24 using BT.601, (digital) YCbCr to analog RGB.
		 * Below the precise transformation matrices are given:
		 * <pre>
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 *
		 * | R |     | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | Y |
		 * | G |  =  | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
		 * | B |     | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | V |
		 *                                                                                   | 1 |
		 *
		 * Approximation with 13 bit precision:
		 *        | R |     | 9535     0         13074 |   | Y -  16 |
		 * 8192 * | G |  =  | 9535    -3203     -6660  | * | U - 128 |
		 *        | B |     | 9535     16531     0     |   | V - 128 |
		 *
		 * Approximation with 10 bit precision:
		 *        | R |     | 1192     0        1634     -223 * 1024 |   | Y |
		 * 1024 * | G |  =  | 1192    -400     -833       135 * 1024 | * | U |
		 *        | B |     | 1192     2066     0        -277 * 1024 |   | V |
		 *                                                               | 1 |
		 *
		 * Approximation with 8 bit precision:
		 *       | R |     | 298     0       409  |   | Y -  16 |
		 * 256 * | G |  =  | 298    -409    -208  | * | U - 128 |
		 *       | B |     | 298     516     0    |   | V - 128 |
		 *
		 * Approximation with 6 bit precision:
		 *      | R |     | 75    0     102 |   | Y -  16 |
		 * 64 * | G |  =  | 75   -25   -52  | * | U - 128 |
		 *      | B |     | 75   128     0  |   | V - 128 |
		 * </pre>
		 * @return The 3x4 transformation matrix
		 */
		static MatrixD transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601();

		/**
		 * Returns the color space transformation matrix from full range BGR24 to limited range YUV24 using BT.601, analog RGB to (digital) YCbCr.
		 * <pre>
		 * BGR input value range:  [0, 255]x[0, 255]x[0, 255]
		 * YUV output value range: [16, 235]x[16, 240]x[16, 240]
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrixFullRangeRGB24ToLimitedRangeYUV24_BT601().
		 */
		static MatrixD transformationMatrix_FullRangeBGR24_To_LimitedRangeYUV24_BT601();

		/**
		 * Returns the 3x4 color space transformation matrix from full range BGR24 to full range YUV24 using BT.601, analog BGR to (analog) YPbPr.
		 * Below the precise transformation matrices are given:
		 * <pre>
		 * BGR input value range:  [0, 255]x[0, 255]x[0, 255]
		 * YUV output value range: [0, 255]x[0, 255]x[0, 255]
		 *
		 * | Y |   |  0.114       0.587       0.299        0 |   | B |
		 * | U | = |  0.5        -0.331264   -0.168736   128 | * | G |
		 * | V |   | -0.081312   -0.418688    0.5        128 |   | R |
		 *                                                       | 1 |
		 * Approximation with 7 bit precision:
		 *       | Y |     |  15     75    38      0   128 |   | B |
		 * 128 * | U |  =  |  64    -42   -22    128 * 128 | * | G |
		 *       | V |     | -10    -54    64    128 * 128 |   | R |
		 *                                                     | 1 |
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrix_FullRangeRGB24_To_FullRangeYUV24_BT601().
		 */
		static MatrixD transformationMatrix_FullRangeBGR24_To_FullRangeYUV24_BT601();

		/**
		 * Returns the 3x4 color space transformation matrix from full range BGR24 to full range YVU24 using BT.601, analog BGR to (analog) YPbPr.
		 * Below the precise transformation matrices are given:
		 * <pre>
		 * BGR input value range:  [0, 255]x[0, 255]x[0, 255]
		 * YVU output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrix_FullRangeBGR24_To_FullRangeYUV24_BT601().
		 */
		static MatrixD transformationMatrix_FullRangeBGR24_To_FullRangeYVU24_BT601();

		/**
		 * Returns the color space transformation matrix from limited range YUV24 to full range BGR24 using BT.601, (digital) YCbCr to analog BGR.
		 * <pre>
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * BGR output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrixLimitedRangeYUV24ToFullRangeRGB24_BT601().
		 */
		static MatrixD transformationMatrix_LimitedRangeYUV24_To_FullRangeBGR24_BT601();

		/**
		 * Returns the color space transformation matrix from limited range YVU24 to full range BGR24 using BT.601, (digital) YCrCb to analog BGR.
		 * <pre>
		 * YVU input value range:  [16, 235]x[16, 240]x[16, 240]
		 * BGR output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrixLimitedRangeYUV24ToFullRangeRGB24_BT601().
		 */
		static MatrixD transformationMatrix_LimitedRangeYVU24_To_FullRangeBGR24_BT601();

		/**
		 * Returns the color space transformation matrix from limited range YVU24 to full range RGB24 using BT.601, (digital) YCrCb to analog RGB.
		 * <pre>
		 * YVU input value range:  [16, 235]x[16, 240]x[16, 240]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrixLimitedRangeYUV24ToFullRangeRGB24_BT601().
		 */
		static MatrixD transformationMatrix_LimitedRangeYVU24_To_FullRangeRGB24_BT601();

		/**
		 * Returns the color space transformation matrix from full range YVU24 to full range RGB24 using BT.601, (digital) YCrCb to analog RGB.
		 * <pre>
		 * YVU input value range:  [0, 255]x[0, 255]x[0, 255]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601().
		 */
		static MatrixD transformationMatrix_FullRangeYVU24_To_FullRangeRGB24_BT601();

		/**
		 * Returns the color space transformation matrix from full range YVU24 to full range BGR24 using BT.601, (digital) YCrCb to analog BGR.
		 * <pre>
		 * YVU input value range:  [0, 255]x[0, 255]x[0, 255]
		 * BGR output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @return The 3x4 transformation matrix
		 * @see transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_BT601().
		 */
		static MatrixD transformationMatrix_FullRangeYVU24_To_FullRangeBGR24_BT601();

		/**
		 * Returns a vector holding all possible conversion flags.
		 * @return The vector with conversion flags, will be four.
		 */
		static const ConversionFlags& conversionFlags();

		/**
		 * Translates a given conversion flag to a string.
		 * @param conversionFlag The conversion flag to be translated
		 * @return The resulting string containing the flag
		 */
		static std::string translateConversionFlag(const ConversionFlag conversionFlag);

	protected:

		/**
		 * Casts 16 successive elements from one data type to another data type.
		 * @param source The 16 source elements that will be casted, must be valid
		 * @param target The 16 target elements receiving the casted pixel values, must be valid
		 * @tparam TSource The data type of each pixel channel of the source frame, e.g., 'uint8_t', 'int', 'float', ...
		 * @tparam TTarget The data type of each pixel channel of the target frame, e.g., 'uint8_t', 'int', 'float', ...
		 */
		template <typename TSource, typename TTarget>
		static inline void cast16Elements(const TSource* const source, TTarget* const target);

		/**
		 * Converts a frame with generic pixel format (e.g., RGBA32, BGR24, YUV24, ...) to a frame with generic pixel format (e.g., RGB24, Y8).
		 * This function needs a function pointer that is able to convert one row, and to reverse the order of pixels in one row in the target frame.
		 * @param source The source frame with generic pixel format, must be valid
		 * @param target The target frame with generic pixel format, must be valid
		 * @param width The width of the frame, with range [1, infinity)
		 * @param height The height of the frame, with range [1, infinity)
		 * @param sourceStrideElements Number of horizontal elements between the start of two source rows, in elements, with range [width * elementsPerSourcePixel, infinity)
		 * @param targetStrideElements Number of horizontal elements between the start of two target rows, in elements, with range [width * elementsPerTargetPixel, infinity)
		 * @param flag Determining the type of conversion
		 * @param rowConversionFunction The function able to convert one row, must be valid
		 * @param targetReversePixelOrderInPlaceFunction The function able to reverse the pixel order in one target row, must be valid if 'flag == CONVERT_MIRRORED || CONVERT_FLIPPED_AND_MIRRORED', can be nullptr otherwise
		 * @param areContinuous True, if source and target frame have continuous memory (without padding); False, otherwise
		 * @param options Optional options which are necessary in the row conversion function, otherwise nullptr
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 * @tparam TSource The data type of each source pixel element, e.g., 'uint8_t' or 'float'
		 * @tparam TTarget The data type of each target pixel element, e.g., 'uint8_t' or 'float'
		 */
		template <typename TSource, typename TTarget>
		static inline void convertGenericPixelFormat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourceStrideElements, const unsigned int targetStrideElements, const ConversionFlag flag, const RowConversionFunction<TSource, TTarget> rowConversionFunction, const RowReversePixelOrderInPlaceFunction<TTarget> targetReversePixelOrderInPlaceFunction, const bool areContinuous, const void* options, Worker* worker);

		/**
		 * Converts a frame with arbitrary pixel format (e.g., Y_UV12, Y_VU12, YUYV16, ...) to a frame with arbitrary pixel format.
		 * This function needs a function pointer that is able to convert multiple rows.
		 * @param sources The memory pointers defining the source frame, e.g., several individual points to individual blocks in memory, must be valid
		 * @param targets The memory pointers defining the target frame, e.g., several individual points to individual blocks in memory, must be valid
		 * @param width The width of the frame, with range [1, infinity)
		 * @param height The height of the frame, with range [multipleRowsPerIteration, infinity), must be a multiple of 'multipleRowsPerIteration'
		 * @param flag The conversion type to be applied
		 * @param multipleRowsPerIteration The number of rows, the specified rows-conversion-functions 'multipleRowsConversionFunction' can handle within one iteration, with range [1, infinity)
		 * @param multipleRowsConversionFunction The function able to convert several row, must be valid
		 * @param options Optional options which are necessary in the rows conversion function, otherwise nullptr
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertArbitraryPixelFormat(const void** sources, void** targets, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int multipleRowsPerIteration, const MultipleRowsConversionFunction multipleRowsConversionFunction, const void* options, Worker* worker);

		/**
		 * Converts a subset of a frame with generic pixel format (e.g., RGBA32, BGR24, YUV24, ...) to a frame with generic pixel format (e.g., Y8).
		 * This function needs a function pointer that is able to convert one row, and to reverse the order of pixels in one row in the target frame.
		 * @param source The source frame with generic pixel format, must be valid
		 * @param target The target frame with generic pixel format, must be valid
		 * @param width The width of the frame, with range [1, infinity)
		 * @param height The height of the frame, with range [1, infinity)
		 * @param sourceStrideBytes Number of bytes between the start of two source rows, in bytes, with range [width * elementsPerSourcePixel * sizeof(channelElement), infinity)
		 * @param targetStrideBytes Number of bytes between the start of two target rows, in bytes, with range [width * elementsPerTargetPixel * sizeof(channelElement), infinity)
		 * @param flag Determining the type of conversion
		 * @param rowConversionFunction The function able to convert one row, must be valid
		 * @param targetReversePixelOrderInPlaceFunction The function able to reverse the pixel order in one target row, must be valid
		 * @param areContinuous True, if source and target frame have continuous memory (without padding); False, otherwise
		 * @param options Optional options which are necessary in the row conversion function, otherwise nullptr
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @see convertGenericPixelFormat<T>().
		 */
		static void convertGenericPixelFormatSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourceStrideBytes, const unsigned int targetStrideBytes, const ConversionFlag flag, const RowConversionFunction<uint8_t, uint8_t> rowConversionFunction, const RowReversePixelOrderInPlaceFunction<uint8_t> targetReversePixelOrderInPlaceFunction, const bool areContinuous, const void* options, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Converts a subset of a frame with arbitrary pixel format (e.g., Y_UV12, Y_VU12, YUYV16, ...) to a frame with arbitrary pixel format.
		 * @param sources The memory pointers defining the source frame, e.g., several individual points to individual blocks in memory, must be valid
		 * @param targets The memory pointers defining the target frame, e.g., several individual points to individual blocks in memory, must be valid
		 * @param width The width of the frame, with range [1, infinity)
		 * @param height The height of the frame, with range [multipleRowsPerIteration, infinity), must be a multiple of 'multipleRowsPerIteration'
		 * @param flag The conversion type to be applied
		 * @param multipleRowsPerIteration The number of rows, the specified rows-conversion-functions 'multipleRowsConversionFunction' can handle within one iteration, with range [1, infinity)
		 * @param multipleRowsConversionFunction The function able to convert several row, must be valid
		 * @param options Optional options which are necessary in the rows conversion function, otherwise nullptr
		 * @param firstMultipleRow The first multiple-row to be handled, with range [0, height / multipleRowsPerIteration - 1]
		 * @param numberMultipleRows The number of multiple-rows to be handled, with range [1, height / multipleRowsPerIteration]
		 * @see convertArbitraryPixelFormat().
		 */
		static void convertArbitraryPixelFormatSubset(const void** sources, void** targets, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int multipleRowsPerIteration, const MultipleRowsConversionFunction multipleRowsConversionFunction, const void* options, const unsigned int firstMultipleRow, const unsigned int numberMultipleRows);

		/**
		 * This function is not used anymore due to the corresponding 2-row function.
		 *
		 * Converts one row of an image with e.g., a Y_UV12 pixel format to one row of an image with e.g., RGB24 pixel format with 6 bit precision.
		 * This function needs one plane with the first channel and another plane/block of 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_UV12 image looks like this:
		 * <pre>
		 *  source0:        source1:
		 *  ---------       ---------
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |      ---------
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the target image of e.g., a RGB24 image looks like this:
		 * <pre>
		 *  target:
		 *  ----------------------------
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 *  ----------------------------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: targetPlanePaddingElements
		 *
		 * options[ 3] int32_t: f00
		 * options[ 4] int32_t: f10
		 * options[ 5] int32_t: f20
		 * options[ 6] int32_t: f01
		 * options[  ] ...
		 * options[11] int32_t: f22
		 *
		 * options[12] int32_t: b0
		 * options[13] int32_t: b1
		 * options[14] int32_t: b2
		 *
		 * with transformation:
		 * t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
		 * t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
		 * t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)
		 * </pre>
		 * @param sources The pointer to the first and second memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height 1]
		 * @param width The width of the frame in pixel, with range [1, infinity), must be even
		 * @param height The height of the frame in pixel, with range [1, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 15 options parameters: 3 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 * @see convertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit().
		 */
		static void convertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * This function is not used anymore due to the corresponding 2-row function.
		 *
		 * Converts one row of an image with e.g., a Y_UV12 pixel format to one row of an image with e.g., RGB24 pixel format with 10 bit precision.
		 * This function needs one plane with the first channel and another plane/block of 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_UV12 image looks like this:
		 * <pre>
		 *  source0:        source1:
		 *  ---------       ---------
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |      ---------
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the target image of e.g., a RGB24 image looks like this:
		 * <pre>
		 *  target:
		 *  ----------------------------
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 *  ----------------------------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: targetPlanePaddingElements
		 *
		 * options[ 3] int32_t: f00
		 * options[ 4] int32_t: f10
		 * options[ 5] int32_t: f20
		 * options[ 6] int32_t: f01
		 * options[  ] ...
		 * options[11] int32_t: f22
		 *
		 * options[12] int32_t: b0
		 * options[13] int32_t: b1
		 * options[14] int32_t: b2
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * @param sources The pointer to the first and second memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height 1]
		 * @param width The width of the frame in pixel, with range [1, infinity), must be even
		 * @param height The height of the frame in pixel, with range [1, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 15 options parameters: 3 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 * @see convertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit().
		 */
		static void convertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with e.g., a Y_UV12 pixel format to two rows of an image with e.g., an RGB24 pixel format with 6 bit precision.
		 * This function needs one plane with the first channel and another plane/block of 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_UV12 image looks like this:
		 * <pre>
		 *  source0:        source1:
		 *  ---------       ---------
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |      ---------
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the target image of e.g., a RGB24 image looks like this:
		 * <pre>
		 *  target:
		 *  ----------------------------
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 *  ----------------------------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: targetPlanePaddingElements
		 *
		 * options[ 3] int32_t: f00
		 * options[ 4] int32_t: f10
		 * options[ 5] int32_t: f20
		 * options[ 6] int32_t: f01
		 * options[  ] ...
		 * options[11] int32_t: f22
		 *
		 * options[12] int32_t: b0
		 * options[13] int32_t: b1
		 * options[14] int32_t: b2
		 *
		 * with transformation:
		 * t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
		 * t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
		 * t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)
		 * </pre>
		 * @param sources The pointer to the first and second memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / 2 - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 15 options parameters: 3 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 */
		static void convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with e.g., a Y_UV12 pixel format to two rows of an image with e.g., an RGB24 pixel format with 10 bit precision.
		 * This function needs one plane with the first channel and another plane/block of 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_UV12 image looks like this:
		 * <pre>
		 *  source0:        source1:
		 *  ---------       ---------
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |      ---------
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the target image of e.g., a RGB24 image looks like this:
		 * <pre>
		 *  target:
		 *  ----------------------------
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 *  ----------------------------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: targetPlanePaddingElements
		 *
		 * options[ 3] int32_t: f00
		 * options[ 4] int32_t: f10
		 * options[ 5] int32_t: f20
		 * options[ 6] int32_t: f01
		 * options[  ] ...
		 * options[11] int32_t: f22
		 *
		 * options[12] int32_t: b0
		 * options[13] int32_t: b1
		 * options[14] int32_t: b2
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * @param sources The pointer to the first and second memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / 2 - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 15 options parameters: 3 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 */
		static void convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with e.g., a RGB pixel format to two rows of an image with e.g., an Y_UV12 pixel format with 7 bit precision.
		 * This function needs a source image with one plane and a target image with two planes.
		 * The layout of the source image of e.g., a RGB24 image looks like this:
		 * <pre>
		 *  source:
		 *  ----------------------------
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 *  ----------------------------
		 * </pre>
		 *
		 * The layout of the target image of e.g., an Y_UV12 image looks like this:
		 * <pre>
		 *  target0:        target1:
		 *  ---------       ---------
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |      ---------
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlanePaddingElements
		 * options[ 1] uint32_t: targetPlane0PaddingElements
		 * options[ 2] uint32_t: targetPlane1PaddingElements
		 *
		 * options[ 3] int32_t: f00
		 * options[ 4] int32_t: f10
		 * options[ 5] int32_t: f20
		 * options[ 6] int32_t: f01
		 * options[  ] ...
		 * options[11] int32_t: f22
		 *
		 * options[12] int32_t: b0
		 * options[13] int32_t: b1
		 * options[14] int32_t: b2
		 *
		 * with transformation:
		 * t0 = clamp(0, f00 * s0 + f01 * s1 + f02 * s2 + b0, 255)
		 * t1 = clamp(0, f10 * s0 + f11 * s1 + f12 * s2 + b1, 255)
		 * t2 = clamp(0, f20 * s0 + f21 * s1 + f22 * s2 + b2, 255)
		 * </pre>
		 * @param sources The pointer to the source plane, must be valid
		 * @param targets The pointers to the first and second target plane, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / 2 - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 15 options parameters: 3 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 */
		static void convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with e.g., a RGB pixel format to two rows of an image with e.g., an Y_U_V12 pixel format with 7 bit precision.
		 * This function needs a source image with one plane and a target image with three planes.
		 * The layout of the source image of e.g., a RGB24 image looks like this:
		 * <pre>
		 *  source:
		 *  ----------------------------
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 *  ----------------------------
		 * </pre>
		 *
		 * The layout of the target image of e.g., an Y_U_V12 image looks like this:
		 * <pre>
		 *  target0:        target1:    target2:
		 *  ---------       -----	    -----
		 * | Y Y Y Y |     | U U |	   | V V |
		 * | Y Y Y Y |     | U U |	   | V V |
		 * | Y Y Y Y |      -----	    -----
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlanePaddingElements
		 * options[ 1] uint32_t: targetPlane0PaddingElements
		 * options[ 2] uint32_t: targetPlane1PaddingElements
		 * options[ 3] uint32_t: targetPlane2PaddingElements
		 *
		 * options[ 4] int32_t: f00
		 * options[ 5] int32_t: f10
		 * options[ 6] int32_t: f20
		 * options[ 7] int32_t: f01
		 * options[  ] ...
		 * options[12] int32_t: f22
		 *
		 * options[13] int32_t: b0
		 * options[14] int32_t: b1
		 * options[15] int32_t: b2
		 *
		 * with transformation:
		 * t0 = clamp(0, f00 * s0 + f01 * s1 + f02 * s2 + b0, 255)
		 * t1 = clamp(0, f10 * s0 + f11 * s1 + f12 * s2 + b1, 255)
		 * t2 = clamp(0, f20 * s0 + f21 * s1 + f22 * s2 + b2, 255)
		 * </pre>
		 * @param sources The pointer to the source plane, must be valid
		 * @param targets The pointers to the first, second, and third target planes, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / 2 - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 16 options parameters: 4 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 */
		static void convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts (maps) one row of an image with e.g., a Y_U_V24 pixel format to one row of an image with e.g., an YUV24 or YVU24 pixel format.
		 * This function needs three source planes each holding one channel.<br>
		 * The layout of the source image of e.g., an Y_U_V24 image looks like this:
		 * <pre>
		 *  source0:        source1:        source2:
		 *  ---------       ---------       ---------
		 * | Y Y Y Y |     | U U U U |     | V V V V |
		 * | Y Y Y Y |     | U U U U |     | V V V V |
		 * | Y Y Y Y |     | U U U U |     | V V V V |
		 * | Y Y Y Y |     | U U U U |     | V V V V |
		 *  ---------       ---------       ---------
		 * </pre>
		 *
		 * The layout of the target image of e.g., an YUV24 image looks like this:
		 * <pre>
		 *  target:
		 *  ----------------------------
		 * | Y U V  Y U V  Y U V  Y U V |
		 * | Y U V  Y U V  Y U V  Y U V |
		 * | Y U V  Y U V  Y U V  Y U V |
		 * | Y U V  Y U V  Y U V  Y U V |
		 *  ----------------------------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePlane0PaddingElements
		 * options[1] uint32_t: sourcePlane1PaddingElements
		 * options[2] uint32_t: sourcePlane2PaddingElements
		 * options[2] uint32_t: targetPlanePaddingElements
		 *
		 * with transformation:
		 * t0 = tSourceChannelIndex0 == 0 ? s0, or tSourceChannelIndex0 == 1 ? s1, or tSourceChannelIndex0 == 2 ? s2
		 * t1 = tSourceChannelIndex1 == 0 ? s0, or tSourceChannelIndex1 == 1 ? s1, or tSourceChannelIndex1 == 2 ? s2
		 * t2 = tSourceChannelIndex2 == 0 ? s0, or tSourceChannelIndex2 == 1 ? s1, or tSourceChannelIndex2 == 2 ? s2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 4 options parameters: 4 padding parameters, with ranges [0, infinity), must be valid
		 * @tparam tSourceChannelIndex0 The index of the source channels which will be mapped to the first target channel, with range [0, 2]
		 * @tparam tSourceChannelIndex1 The index of the source channels which will be mapped to the second target channel, with range [0, 2]
		 * @tparam tSourceChannelIndex2 The index of the source channels which will be mapped to the third target channel, with range [0, 2]
		 */
		template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
		static void mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts (matches) one row of an image with e.g., a Y_UV12 pixel format to one row of an image with e.g., an YUV24 or YVU24 pixel format.
		 * This function needs one plane with the first channel and another plane/block of 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_UV12 image looks like this:
		 * <pre>
		 *  source0:        source1:
		 *  ---------       ---------
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |      ---------
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the target image of e.g., an YUV24 image looks like this:
		 * <pre>
		 *  target:
		 *  ----------------------------
		 * | Y U V  Y U V  Y U V  Y U V |
		 * | Y U V  Y U V  Y U V  Y U V |
		 * | Y U V  Y U V  Y U V  Y U V |
		 * | Y U V  Y U V  Y U V  Y U V |
		 *  ----------------------------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePlane0PaddingElements
		 * options[1] uint32_t: sourcePlane1PaddingElements
		 * options[2] uint32_t: targetPlanePaddingElements
		 *
		 * with transformation:
		 * t0 = tSourceChannelIndex0 == 0 ? s0, or tSourceChannelIndex0 == 1 ? s1, or tSourceChannelIndex0 == 2 ? s2
		 * t1 = tSourceChannelIndex1 == 0 ? s0, or tSourceChannelIndex1 == 1 ? s1, or tSourceChannelIndex1 == 2 ? s2
		 * t2 = tSourceChannelIndex2 == 0 ? s0, or tSourceChannelIndex2 == 1 ? s1, or tSourceChannelIndex2 == 2 ? s2
		 * </pre>
		 * @param sources The pointer to the first and second memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 3 options parameters: 3 padding parameters, with ranges [0, infinity), must be valid
		 * @tparam tSourceChannelIndex0 The index of the source channels which will be mapped to the first target channel, with range [0, 2]
		 * @tparam tSourceChannelIndex1 The index of the source channels which will be mapped to the second target channel, with range [0, 2]
		 * @tparam tSourceChannelIndex2 The index of the source channels which will be mapped to the third target channel, with range [0, 2]
		 */
		template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
		static void mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts (matches) two rows of an image with e.g., a Y_UV12 pixel format to two rows of an image with e.g., an YUV24 or YVU24 pixel format.
		 * This function needs one plane with the first channel and another plane/block of 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_UV12 image looks like this:
		 * <pre>
		 *  source0:        source1:
		 *  ---------       ---------
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |     | U V U V |
		 * | Y Y Y Y |      ---------
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePlanePaddingElements
		 * options[1] uint32_t: sourceZippedPaddingElements
		 * options[2] uint32_t: targetZippedPaddingElements
		 *
		 * with transformation:
		 * t0 = tSourceChannelIndex0 == 0 ? s0, or tSourceChannelIndex0 == 1 ? s1, or tSourceChannelIndex0 == 2 ? s2
		 * t1 = tSourceChannelIndex1 == 0 ? s0, or tSourceChannelIndex1 == 1 ? s1, or tSourceChannelIndex1 == 2 ? s2
		 * t2 = tSourceChannelIndex2 == 0 ? s0, or tSourceChannelIndex2 == 1 ? s1, or tSourceChannelIndex2 == 2 ? s2
		 * </pre>
		 * @param sources The pointer to the first and second memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / 2 - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 3 options parameters: 3 padding parameters, with ranges [0, infinity), must be valid
		 * @tparam tSourceChannelIndex0 The index of the source channels which will be mapped to the first target channel, with range [0, 2]
		 * @tparam tSourceChannelIndex1 The index of the source channels which will be mapped to the second target channel, with range [0, 2]
		 * @tparam tSourceChannelIndex2 The index of the source channels which will be mapped to the third target channel, with range [0, 2]
		 */
		template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
		static void mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts one row of an image with e.g., a Y_U_V12 pixel format to one row of an image with e.g., an RGB24 pixel format with 10 bit precision.
		 * This function needs one plane with the first channel, and two additional planes with 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_U_V12 image looks like this:
		 * <pre>
		 *  source0:        source1:    source2:
		 *  ---------       -----       -----
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |      -----       -----
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: sourcePlane2PaddingElements
		 * options[ 3] uint32_t: targetZippedPaddingElements
		 *
		 * options[ 4] int32_t: f00
		 * options[ 5] int32_t: f10
		 * options[ 6] int32_t: f20
		 * options[ 7] int32_t: f01
		 * options[  ] ...
		 * options[12] int32_t: f22
		 *
		 * options[13] int32_t: b0
		 * options[14] int32_t: b1
		 * options[15] int32_t: b2
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 16 options parameters: 4 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 */
		static void convertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with e.g., a Y_U_V12 pixel format to two rows of an image with e.g., an RGB24 pixel format with 10 bit precision.
		 * This function needs one plane with the first channel, and two additional planes with 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_U_V12 image looks like this:
		 * <pre>
		 *  source0:        source1:    source2:
		 *  ---------       -----       -----
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |      -----       -----
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: sourcePlane2PaddingElements
		 * options[ 3] uint32_t: targetZippedPaddingElements
		 *
		 * options[ 4] int32_t: f00
		 * options[ 5] int32_t: f10
		 * options[ 6] int32_t: f20
		 * options[ 7] int32_t: f01
		 * options[  ] ...
		 * options[12] int32_t: f22
		 *
		 * options[13] int32_t: b0
		 * options[14] int32_t: b1
		 * options[15] int32_t: b2
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / 2 - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 16 options parameters: 4 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 */
		static void convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts one row of an image with e.g., a Y_U_V12 pixel format to one row of an image with e.g., an YUV24 or YVU24 pixel format.
		 * This function needs one plane with the first channel, and two additional planes with 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_U_V12 image looks like this:
		 * <pre>
		 *  source0:        source1:    source2:
		 *  ---------       -----       -----
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |      -----       -----
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePlane0PaddingElements
		 * options[1] uint32_t: sourcePlane1PaddingElements
		 * options[2] uint32_t: sourcePlane2PaddingElements
		 * options[3] uint32_t: targetZippedPaddingElements
		 *
		 * with transformation:
		 * t0 = tSourceChannelIndex0 == 0 ? s0, or tSourceChannelIndex0 == 1 ? s1, or tSourceChannelIndex0 == 2 ? s2
		 * t1 = tSourceChannelIndex1 == 0 ? s0, or tSourceChannelIndex1 == 1 ? s1, or tSourceChannelIndex1 == 2 ? s2
		 * t2 = tSourceChannelIndex2 == 0 ? s0, or tSourceChannelIndex2 == 1 ? s1, or tSourceChannelIndex2 == 2 ? s2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 4 options parameters: 4 padding parameters, must be valid
		 * @tparam tSourceChannelIndex0 The index of the source channels which will be mapped to the first target channel, with range [0, infinity)
		 * @tparam tSourceChannelIndex1 The index of the source channels which will be mapped to the second target channel, with range [0, infinity)
		 * @tparam tSourceChannelIndex2 The index of the source channels which will be mapped to the third target channel, with range [0, infinity)
		 * @see mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel().
		 */
		template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
		static void mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with e.g., a Y_U_V12 pixel format to two rows of an image with e.g., an YUV24 or YVU24 pixel format.
		 * This function needs one plane with the first channel, and two additional planes with 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_U_V12 image looks like this:
		 * <pre>
		 *  source0:        source1:    source2:
		 *  ---------       -----       -----
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |      -----       -----
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 *
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePlane0PaddingElements
		 * options[1] uint32_t: sourcePlane1PaddingElements
		 * options[2] uint32_t: sourcePlane2PaddingElements
		 * options[3] uint32_t: targetZippedPaddingElements
		 *
		 * with transformation:
		 * t0 = tSourceChannelIndex0 == 0 ? s0, or tSourceChannelIndex0 == 1 ? s1, or tSourceChannelIndex0 == 2 ? s2
		 * t1 = tSourceChannelIndex1 == 0 ? s0, or tSourceChannelIndex1 == 1 ? s1, or tSourceChannelIndex1 == 2 ? s2
		 * t2 = tSourceChannelIndex2 == 0 ? s0, or tSourceChannelIndex2 == 1 ? s1, or tSourceChannelIndex2 == 2 ? s2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / 2 - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 4 options parameters: 4 padding parameters, must be valid
		 * @tparam tSourceChannelIndex0 The index of the source channels which will be mapped to the first target channel, with range [0, infinity)
		 * @tparam tSourceChannelIndex1 The index of the source channels which will be mapped to the second target channel, with range [0, infinity)
		 * @tparam tSourceChannelIndex2 The index of the source channels which will be mapped to the third target channel, with range [0, infinity)
		 * @see mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel().
		 */
		template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
		static void mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with e.g., a YUV24 pixel format to two rows of an image with e.g., an Y_U_V12 or Y_V_U12 pixel format.
		 * This function is mainly the reverse conversion function of mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel().<br>
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourceZippedPaddingElements
		 * options[1] uint32_t: targetPlane0PaddingElements
		 * options[2] uint32_t: targetPlane1PaddingElements
		 * options[3] uint32_t: targetPlane2PaddingElements
		 *
		 * with transformation:
		 * t0 = tSourceChannelIndex0 == 0 ? s0, or tSourceChannelIndex0 == 1 ? s1, or tSourceChannelIndex0 == 2 ? s2
		 * t1 = tSourceChannelIndex1 == 0 ? s0, or tSourceChannelIndex1 == 1 ? s1, or tSourceChannelIndex1 == 2 ? s2
		 * t2 = tSourceChannelIndex2 == 0 ? s0, or tSourceChannelIndex2 == 1 ? s1, or tSourceChannelIndex2 == 2 ? s2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / 2 - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 4 options parameters: 4 padding parameters, must be valid
		 * @tparam tSourceChannelIndex0 The index of the source channels which will be mapped to the first target channel, with range [0, infinity)
		 * @tparam tSourceChannelIndex1 The index of the source channels which will be mapped to the second target channel, with range [0, infinity)
		 * @tparam tSourceChannelIndex2 The index of the source channels which will be mapped to the third target channel, with range [0, infinity)
		 * @see mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel().
		 */
		template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
		static void mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts one row of an image with e.g., a Y_U_V24 pixel format to one rows of an image with e.g., an RGB24 pixel format.
		 * This function needs three source planes/blocks with three individual channels.
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: sourcePlane2PaddingElements
		 * options[ 3] uint32_t: targetZippedPaddingElements
		 *
		 * options[ 4] int32_t: f00
		 * options[ 5] int32_t: f10
		 * options[ 6] int32_t: f20
		 * options[ 7] int32_t: f01
		 * options[  ] ...
		 * options[12] int32_t: f22
		 *
		 * options[13] int32_t: b0
		 * options[14] int32_t: b1
		 * options[15] int32_t: b2
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 16 options parameters: 4 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 */
		static void convertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts one row of an image with e.g., a Y_U_V24 pixel format to one rows of an image with e.g., an RGBA32 pixel format.
		 * This function needs three source planes/blocks with three individual channels.
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: sourcePlane2PaddingElements
		 * options[ 3] uint32_t: targetZippedPaddingElements
		 *
		 * options[ 4] int32_t: f00
		 * options[ 5] int32_t: f10
		 * options[ 6] int32_t: f20
		 * options[ 7] int32_t: f01
		 * options[  ] ...
		 * options[12] int32_t: f22
		 *
		 * options[13] int32_t: b0
		 * options[14] int32_t: b1
		 * options[15] int32_t: b2
		 *
		 * options[16] uint32_t: channelValue3
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * t3 = channelValue3
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 17 options parameters: 4 padding parameters, 9 multiplication parameters, and 3 bias parameters, one constant channel value, must be valid
		 */
		static void convertOneRow_3Planes1Channel_To_1Plane4Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts one row of an image with e.g., a Y_U_V12 pixel format to one row of an image with e.g., an RGBA32 pixel format.
		 * This function needs one plane with the first channel, and two additional planes with 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_U_V12 image looks like this:
		 * <pre>
		 *  source0:        source1:    source2:
		 *  ---------       -----       -----
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |      -----       -----
		 * | Y Y Y Y |
		 *  ---------
		 * </pre>
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: sourcePlane2PaddingElements
		 * options[ 3] uint32_t: targetZippedPaddingElements
		 *
		 * options[ 4] int32_t: f00
		 * options[ 5] int32_t: f10
		 * options[ 6] int32_t: f20
		 * options[ 7] int32_t: f01
		 * options[  ] ...
		 * options[12] int32_t: f22
		 *
		 * options[13] int32_t: b0
		 * options[14] int32_t: b1
		 * options[15] int32_t: b2
		 *
		 * options[16] int32_t: channelValue3
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * t3 = channelValue3
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 17 options parameters: 4 padding parameters, 9 multiplication parameters, and 3 bias parameters, one constant channel value, must be valid
		 */
		static void convertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with e.g., a Y_U_V12 pixel format to two rows of an image with e.g., an RGB24 pixel format.
		 * This function needs one plane with the first channel, and two additional planes with 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_U_V12 image looks like this:
		 * <pre>
		 *  source0:        source1:    source2:
		 *  ---------       -----       -----
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |      -----       -----
		 * | Y Y Y Y |
		 *  ---------
		 *
		 * The layout of the target image of e.g., a RGB24 image looks like this:
		 * <pre>
		 *  target:
		 *  ----------------------------
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 * | R G B  R G B  R G B  R G B |
		 *  ----------------------------
		 * </pre>
		 *
		 * </pre>
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: sourcePlane2PaddingElements
		 * options[ 3] uint32_t: targetPlanePaddingElements
		 *
		 * options[ 4] int32_t: f00
		 * options[ 5] int32_t: f10
		 * options[ 6] int32_t: f20
		 * options[ 7] int32_t: f01
		 * options[  ] ...
		 * options[12] int32_t: f22
		 *
		 * options[13] int32_t: b0
		 * options[14] int32_t: b1
		 * options[15] int32_t: b2
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * t3 = channelValue3
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height/2 - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [2, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 16 options parameters: 4 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 */
		static void convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with e.g., a Y_U_V12 pixel format to two rows of an image with e.g., an RGBA32 pixel format.
		 * This function needs one plane with the first channel, and two additional planes with 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the source image of e.g., an Y_U_V12 image looks like this:
		 * <pre>
		 *  source0:        source1:    source2:
		 *  ---------       -----       -----
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |     | U U |     | V V |
		 * | Y Y Y Y |      -----       -----
		 * | Y Y Y Y |
		 *  ---------
		 *
		 * The layout of the target image of e.g., a RGBA24 image looks like this:
		 * <pre>
		 *  target:
		 *  ------------------------------------
		 * | R G B A  R G B A  R G B A  R G B A |
		 * | R G B A  R G B A  R G B A  R G B A |
		 * | R G B A  R G B A  R G B A  R G B A |
		 * | R G B A  R G B A  R G B A  R G B A |
		 *  ------------------------------------
		 * </pre>
		 *
		 * </pre>
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePlane0PaddingElements
		 * options[ 1] uint32_t: sourcePlane1PaddingElements
		 * options[ 2] uint32_t: sourcePlane2PaddingElements
		 * options[ 3] uint32_t: targetPlanePaddingElements
		 *
		 * options[ 4] int32_t: f00
		 * options[ 5] int32_t: f10
		 * options[ 6] int32_t: f20
		 * options[ 7] int32_t: f01
		 * options[  ] ...
		 * options[12] int32_t: f22
		 *
		 * options[13] int32_t: b0
		 * options[14] int32_t: b1
		 * options[15] int32_t: b2
		 *
		 * options[16] uint32_t: channelValue3
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * t3 = channelValue3
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height/2 - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [2, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 17 options parameters: 4 padding parameters, 9 multiplication parameters, and 3 bias parameters, one constant channel value, must be valid
		 */
		static void convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts one row of an image with e.g., a YUYV16 pixel format to one row of an image with e.g., an RGB24 or BGR2424 pixel format.
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePaddingElements
		 * options[ 1] uint32_t: targetPaddingElements
		 *
		 * options[ 2] int32_t: f00
		 * options[ 3] int32_t: f10
		 * options[ 4] int32_t: f20
		 * options[ 5] int32_t: f01
		 * options[  ] ...
		 * options[10] int32_t: f22
		 *
		 * options[11] int32_t: b0
		 * options[12] int32_t: b1
		 * options[13] int32_t: b2
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [1, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 14 options parameters: 2 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 */
		static void convertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts one row of an image with e.g., a UYVY16 pixel format to one row of an image with e.g., an RGB24 or BGR2424 pixel format.
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[ 0] uint32_t: sourcePaddingElements
		 * options[ 1] uint32_t: targetPaddingElements
		 *
		 * options[ 2] int32_t: f00
		 * options[ 3] int32_t: f10
		 * options[ 4] int32_t: f20
		 * options[ 5] int32_t: f01
		 * options[  ] ...
		 * options[10] int32_t: f22
		 *
		 * options[11] int32_t: b0
		 * options[12] int32_t: b1
		 * options[13] int32_t: b2
		 *
		 * with transformation:
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [1, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 14 options parameters: 2 padding parameters, 9 multiplication parameters, and 3 bias parameters, must be valid
		 */
		static void convertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts one row of an image with e.g., a YUYV16 pixel format to one row of an image with e.g., an YUV24 or YVU24 pixel format.
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePaddingElements
		 * options[1] uint32_t: targetPaddingElements
		 *
		 * with transformation:
		 * t0 = tSourceChannelIndex0 == 0 ? s0, or tSourceChannelIndex0 == 1 ? s1, or tSourceChannelIndex0 == 2 ? s2
		 * t1 = tSourceChannelIndex1 == 0 ? s0, or tSourceChannelIndex1 == 1 ? s1, or tSourceChannelIndex1 == 2 ? s2
		 * t2 = tSourceChannelIndex2 == 0 ? s0, or tSourceChannelIndex2 == 1 ? s1, or tSourceChannelIndex2 == 2 ? s2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 2 options parameters: 2 padding parameters, must be valid
		 * @tparam tSourceChannelIndex0 The index of the source channels which will be mapped to the first target channel, with range [0, infinity)
		 * @tparam tSourceChannelIndex1 The index of the source channels which will be mapped to the second target channel, with range [0, infinity)
		 * @tparam tSourceChannelIndex2 The index of the source channels which will be mapped to the third target channel, with range [0, infinity)
		 * @see mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel().
		 */
		template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
		static void mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts one row of an image with e.g., a UYVY16 pixel format to one row of an image with e.g., an YUV24 or YVU24 pixel format.
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePaddingElements
		 * options[1] uint32_t: targetPaddingElements
		 *
		 * with transformation:
		 * t0 = tSourceChannelIndex0 == 0 ? s0, or tSourceChannelIndex0 == 1 ? s1, or tSourceChannelIndex0 == 2 ? s2
		 * t1 = tSourceChannelIndex1 == 0 ? s0, or tSourceChannelIndex1 == 1 ? s1, or tSourceChannelIndex1 == 2 ? s2
		 * t2 = tSourceChannelIndex2 == 0 ? s0, or tSourceChannelIndex2 == 1 ? s1, or tSourceChannelIndex2 == 2 ? s2
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 2 options parameters: 2 padding parameters, must be valid
		 * @tparam tSourceChannelIndex0 The index of the source channels which will be mapped to the first target channel, with range [0, infinity)
		 * @tparam tSourceChannelIndex1 The index of the source channels which will be mapped to the second target channel, with range [0, infinity)
		 * @tparam tSourceChannelIndex2 The index of the source channels which will be mapped to the third target channel, with range [0, infinity)
		 */
		template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
		static void mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with 3-channel Bayer mosaic pixel format with packed 10-bit pixel values to an image with 3-channel un-packed 8-bit pixel format.
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePaddingElements
		 * options[1] uint32_t: targetPaddingElements
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height/2 - 1]
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param conversionFlag The conversion to be applied
		 * @param options The 2 options parameters: 2 padding parameters
		 * @tparam tIndexRed The index of red channel in the target image, with range [0, 2]
		 * @tparam tIndexGreen The index of green channel in the target image, with range [0, 2]
		 * @tparam tIndexBlue The index of blue channel in the target image, with range [0, 2]
		 */
		template <unsigned int tIndexRed, unsigned int tIndexGreen, unsigned int tIndexBlue>
		static void convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with 3-channel Bayer mosaic pixel format with packed 10-bit pixel values to an image with 3-channel un-packed 8-bit pixel format and applies black level subtraction, white balance, and gamma encoding
		 * The layout of the parameters, `options`, is defined in the struct `RGGB10ToRGB24AdvancedOptions`.
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height/2 - 1]
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param conversionFlag The conversion to be applied
		 * @param options The 2 options parameters: 2 padding parameters
		 * @tparam tIndexRed The index of red channel in the target image, with range [0, 2]
		 * @tparam tIndexGreen The index of green channel in the target image, with range [0, 2]
		 * @tparam tIndexBlue The index of blue channel in the target image, with range [0, 2]
		 */
		template <unsigned int tIndexRed, unsigned int tIndexGreen, unsigned int tIndexBlue>
		static void convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8BitAdvanced(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Maps one row of a 1-plane, 2-channel image to two planes with 1 channels.
		 * @param sources The pointer to the plane of the source image, must be valid
		 * @param targets The pointer to the first and second plane of the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 3 options parameters: 3 padding parameters
		 */
		static void mapOneRow_1Plane2Channels_To_2Planes1Channel_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Unpacks 5 elements from a row in a packed Bayer mosaic to 4 pixels values
		 * The required memory layout of the input: `A B C D X ...`
		 * Bytes marked as `X` store two bits for each of the previous four elements: `X = AABB CCDD`.
		 * The memory layout of the output will be: `A B C D` (16 bits per element but only the lowest 10 bits are used)
		 * This function is compatible with pixel formats like `FrameType::FORMAT_RGGB10_PACKED` or `FrameType::FORMAT_Y10_PACKED`.
		 * @param packed The packed 5 elements, must be valid
		 * @param unpacked The resulting 4 unpacked elements, must be valid
		 */
		static OCEAN_FORCE_INLINE void unpack5ElementsBayerMosaicPacked10Bit(const uint8_t* const packed, uint16_t* unpacked);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Unpacks 15 elements from a row in a packed Bayer mosaic to 12 pixels values
		 * The required memory layout of the input: `A B C D X  A B C D X  A B C D X  A ...`
		 * Bytes marked as `X` store two bits for each of the previous four elements: `X = AABB CCDD`.
		 * The memory layout of the output will be: `A B C D  A B C D  A B C D` (16 bits per element but only the lowest 10 bits are used)
		 * This function is compatible with pixel formats like `FrameType::FORMAT_RGGB10_PACKED` or `FrameType::FORMAT_Y10_PACKED`.
		 * @param packed The packed 15 elements, must be valid
		 * @param unpackedAB_u_16x8 The resulting first 8 uint16_t values
		 * @param unpackedC_u_16x4 The resulting last 4 uint16_t values
		 * @tparam tAllowLastOverlappingElement True, to allow reading 16 elements from `packed` (if the memory is large enough); False, to force reading only 15 elements
		 */
		template <bool tAllowLastOverlappingElement>
		static OCEAN_FORCE_INLINE void unpack15ElementsBayerMosaicPacked10BitNEON(const uint8_t* const packed, uint16x8_t& unpackedAB_u_16x8, uint16x4_t& unpackedC_u_16x4);

#endif // OCEAN_HARDWARE_NEON_VERSION
};

inline FrameConverter::Options::Options(const bool allowApproximation)
{
	if (allowApproximation)
	{
		optionsType_ = OptionsType(optionsType_ | OT_APPROXIMATED);
	}
}

inline FrameConverter::Options::Options(const uint8_t alphaChannelTargetValue, const bool allowApproximation) :
	optionsType_(OT_ALPHA_CHANNEL_TARGET_VALUE),
	alphaChannelTargetValue_(alphaChannelTargetValue)
{
	if (allowApproximation)
	{
		optionsType_ = OptionsType(optionsType_ | OT_APPROXIMATED);
	}
}

inline FrameConverter::Options::Options(const float gamma, const bool allowApproximation) :
	optionsType_(OT_GAMMA_CORRECTION),
	gamma_(gamma)
{
	ocean_assert(gamma_ >= 0.0f && gamma_ <= 2.0f);

	if (allowApproximation)
	{
		optionsType_ = OptionsType(optionsType_ | OT_APPROXIMATED);
	}
}

inline FrameConverter::Options::Options(const uint16_t blackLevel, const float whiteBalanceRed, const float whiteBalanceGreen, const float whiteBalanceBlue, const float gamma, const bool allowApproximation) :
	optionsType_(OT_BLACKLEVEL_WHITEBALANCE_GAMMA),
	gamma_(gamma),
	blackLevel_(blackLevel)
{
	ocean_assert(blackLevel_ < 1024u);
	ocean_assert(whiteBalanceRed >= 0.0f && whiteBalanceGreen >= 0.0f && whiteBalanceBlue >= 0.0f);
	ocean_assert(gamma_ >= 0.0f);

	whiteBalance_[0] = whiteBalanceRed;
	whiteBalance_[1] = whiteBalanceGreen;
	whiteBalance_[2] = whiteBalanceBlue;

	if (allowApproximation)
	{
		optionsType_ = OptionsType(optionsType_ | OT_APPROXIMATED);
	}
}

inline FrameConverter::Options::OptionsType FrameConverter::Options::optionsType() const
{
	return optionsType_;
}

inline uint8_t FrameConverter::Options::alphaChannelTargetValue() const
{
	ocean_assert(optionsType_ & OT_ALPHA_CHANNEL_TARGET_VALUE);
	return alphaChannelTargetValue_;
}

inline float FrameConverter::Options::gamma() const
{
	ocean_assert((optionsType_ & OT_GAMMA_CORRECTION) || (optionsType_ & OT_BLACKLEVEL_WHITEBALANCE_GAMMA));
	return gamma_;
}

inline uint16_t FrameConverter::Options::blackLevel() const
{
	ocean_assert(optionsType_ & OT_BLACKLEVEL_WHITEBALANCE_GAMMA);
	return blackLevel_;
}

inline const float* FrameConverter::Options::whiteBalance() const
{
	ocean_assert(optionsType_ & OT_BLACKLEVEL_WHITEBALANCE_GAMMA);
	return whiteBalance_;
}

inline bool FrameConverter::Options::allowApproximation() const
{
	return (optionsType_ & OT_APPROXIMATED) == OT_APPROXIMATED;
}

inline size_t FrameConverter::ConversionFunctionMap::ConversionTriple::Hash::operator()(const ConversionTriple& conversionTriple) const
{
	return size_t(conversionTriple.sourcePixelFormat_ ^ (conversionTriple.targetPixelFormat_ << uint64_t(1u)) ^ (uint64_t(conversionTriple.optionsType_) << uint64_t(2u)));
}

inline FrameConverter::ConversionFunctionMap::ConversionTriple::ConversionTriple(const FrameType::PixelFormat& sourcePixelFormat, const FrameType::PixelFormat& targetPixelFormat, const Options::OptionsType optionsType) :
	sourcePixelFormat_(sourcePixelFormat),
	targetPixelFormat_(targetPixelFormat),
	optionsType_(optionsType)
{
	// nothing to do here
}

inline bool FrameConverter::ConversionFunctionMap::ConversionTriple::operator==(const ConversionTriple& conversionTriple) const
{
	return sourcePixelFormat_ == conversionTriple.sourcePixelFormat_ && targetPixelFormat_ == conversionTriple.targetPixelFormat_ && optionsType_ == conversionTriple.optionsType_;
}

inline bool FrameConverter::Comfort::convert(const Frame& source, const FrameType::PixelFormat targetPixelFormat, Frame& target, const bool forceCopy, Worker* worker, const Options& options)
{
	return convert(source, targetPixelFormat, source.pixelOrigin(), target, forceCopy, worker, options);
}

inline bool FrameConverter::Comfort::convert(const Frame& source, const FrameType::PixelOrigin targetPixelOrigin, Frame& target, const bool forceCopy, Worker* worker, const Options& options)
{
	return convert(source, source.pixelFormat(), targetPixelOrigin, target, forceCopy, worker, options);
}

inline bool FrameConverter::Comfort::change(Frame& frame, const FrameType::PixelFormat targetPixelFormat, const FrameType::PixelOrigin targetPixelOrigin, const bool forceCopy, Worker* worker, const Options& options)
{
	ocean_assert(frame.isValid());
	ocean_assert(targetPixelFormat != FrameType::FORMAT_UNDEFINED && targetPixelOrigin != FrameType::ORIGIN_INVALID);

	if (!frame.isValid())
	{
		return false;
	}

	if (frame.pixelFormat() == targetPixelFormat && frame.pixelOrigin() == targetPixelOrigin)
	{
		return true;
	}

	Frame tmpFrame;
	if (!convert(frame, targetPixelFormat, targetPixelOrigin, tmpFrame, forceCopy, worker, options))
	{
		return false;
	}

	// if the intermediate frame could be created without copying the frame data we have to copy the frame data instead
	if (frame.isOwner() && !tmpFrame.isOwner())
	{
		frame.copy(tmpFrame);
	}
	else
	{
		frame = std::move(tmpFrame);
	}

	return true;
}

inline bool FrameConverter::Comfort::change(Frame& frame, const FrameType::PixelFormat targetPixelFormat, const bool forceCopy, Worker* worker, const Options& options)
{
	return change(frame, targetPixelFormat, frame.pixelOrigin(), forceCopy, worker, options);
}

inline bool FrameConverter::Comfort::change(Frame& frame, const FrameType::PixelOrigin targetPixelOrigin, const bool forceCopy, Worker* worker, const Options& options)
{
	return change(frame, frame.pixelFormat(), targetPixelOrigin, forceCopy, worker, options);
}

OCEAN_RE_ENABLE_DOCUMENTATION_DIAGNOSTIC

template <typename TSource, typename TTarget>
void FrameConverter::cast(const TSource* __restrict source, TTarget* __restrict target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);

	// we will have a small performance benefit when applying as less as possible non-16-block iterations
	// thus, we distinguish between an execution with and without padding values

	if (sourcePaddingElements == 0u && targetPaddingElements == 0u)
	{
		if (std::is_same<TSource, TTarget>::value)
		{
			memcpy(target, source, size_t(width * height * channels) * sizeof(TSource));
		}
		else
		{
			const unsigned int elementsPerFrame = width * height * channels;
			const unsigned int blocksPerFrame_16 = elementsPerFrame / 16u;

			const unsigned int remainingElementsPerFrame = elementsPerFrame - blocksPerFrame_16 * 16u;

			for (unsigned int n = 0u; n < blocksPerFrame_16; ++n)
			{
				cast16Elements<TSource, TTarget>(source, target);

				source += 16;
				target += 16;
			}

			for (unsigned int i = 0u; i < remainingElementsPerFrame; ++i)
			{
				target[i] = TTarget(source[i]);
			}
		}
	}
	else
	{
		if (std::is_same<TSource, TTarget>::value)
		{
			const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
			const unsigned int targetStrideElements = width * channels + targetPaddingElements;

			const size_t bytesPerRowToCopy = size_t(width * channels) * sizeof(TSource);

			for (unsigned int y = 0u; y < height; ++y)
			{
				memcpy(target, source, bytesPerRowToCopy);

				source += sourceStrideElements;
				target += targetStrideElements;
			}
		}
		else
		{
			const unsigned int elementsPerRow = width * channels;
			const unsigned int blocksPerRow_16 = elementsPerRow / 16u;

			const unsigned int remainingElementsPerRow = elementsPerRow - blocksPerRow_16 * 16u;

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < blocksPerRow_16; ++x)
				{
					cast16Elements<TSource, TTarget>(source, target);

					source += 16;
					target += 16;
				}

				for (unsigned int i = 0u; i < remainingElementsPerRow; ++i)
				{
					target[i] = TTarget(source[i]);
				}

				source += remainingElementsPerRow + sourcePaddingElements;
				target += remainingElementsPerRow + targetPaddingElements;
			}
		}
	}
}

template <typename TSource, typename TTarget>
void FrameConverter::normalizedCast(const TSource* __restrict source, TTarget* __restrict target, const unsigned int width, const unsigned int height, const unsigned int channels, const TTarget multiplicationFactor, const TTarget offset, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);

	// we will have a small performance benefit when applying as little as possible non-16-block iterations
	// thus, we distinguish between an execution with and without padding values

	if (sourcePaddingElements == 0u && targetPaddingElements == 0u)
	{
		const unsigned int elementsPerFrame = width * height * channels;
		const unsigned int blocksPerFrame_16 = elementsPerFrame / 16u;

		const unsigned int remainingElementsPerFrame = elementsPerFrame - blocksPerFrame_16 * 16u;

		for (unsigned int n = 0u; n < blocksPerFrame_16; ++n)
		{
			for (unsigned int i = 0u; i < 16u; ++i)
			{
				target[i] = TTarget(source[i]) * multiplicationFactor + offset;
			}

			source += 16;
			target += 16;
		}

		for (unsigned int i = 0u; i < remainingElementsPerFrame; ++i)
		{
			target[i] = TTarget(source[i]) * multiplicationFactor + offset;
		}
	}
	else
	{
		const unsigned int elementsPerRow = width * channels;
		const unsigned int blocksPerRow_16 = elementsPerRow / 16u;

		const unsigned int remainingElementsPerRow = elementsPerRow - blocksPerRow_16 * 16u;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocksPerRow_16; ++n)
			{
				for (unsigned int i = 0u; i < 16u; ++i)
				{
					target[i] = TTarget(source[i]) * multiplicationFactor + offset;
				}

				source += 16;
				target += 16;
			}

			for (unsigned int i = 0u; i < remainingElementsPerRow; ++i)
			{
				target[i] = TTarget(source[i]) * multiplicationFactor + offset;
			}

			source += remainingElementsPerRow + sourcePaddingElements;
			target += remainingElementsPerRow + targetPaddingElements;
		}
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
OCEAN_FORCE_INLINE void FrameConverter::cast16Elements<uint8_t, float>(const uint8_t* const source, float* const target)
{
	const uint8x16_t source_8x16 = vld1q_u8(source);

	const uint16x8_t source_16x8_0 = vmovl_u8(vget_low_u8(source_8x16));
	const uint16x8_t source_16x8_1 = vmovl_u8(vget_high_u8(source_8x16));

	const uint32x4_t source_32x4_0 = vmovl_u16(vget_low_u16(source_16x8_0));
	const uint32x4_t source_32x4_1 = vmovl_u16(vget_high_u16(source_16x8_0));
	const uint32x4_t source_32x4_2 = vmovl_u16(vget_low_u16(source_16x8_1));
	const uint32x4_t source_32x4_3 = vmovl_u16(vget_high_u16(source_16x8_1));

	const float32x4_t target_32x4_0 = vcvtq_f32_u32(source_32x4_0);
	const float32x4_t target_32x4_1 = vcvtq_f32_u32(source_32x4_1);
	const float32x4_t target_32x4_2 = vcvtq_f32_u32(source_32x4_2);
	const float32x4_t target_32x4_3 = vcvtq_f32_u32(source_32x4_3);

	vst1q_f32(target +  0, target_32x4_0);
	vst1q_f32(target +  4, target_32x4_1);
	vst1q_f32(target +  8, target_32x4_2);
	vst1q_f32(target + 12, target_32x4_3);
}

template <>
OCEAN_FORCE_INLINE void FrameConverter::cast16Elements<uint8_t, uint16_t>(const uint8_t* const source, uint16_t* const target)
{
	const uint8x16_t source_8x16 = vld1q_u8(source);

	const uint16x8_t source_16x8_0 = vmovl_u8(vget_low_u8(source_8x16));
	const uint16x8_t source_16x8_1 = vmovl_u8(vget_high_u8(source_8x16));

	vst1q_u16(target + 0, source_16x8_0);
	vst1q_u16(target + 8, source_16x8_1);
}

template <>
OCEAN_FORCE_INLINE void FrameConverter::cast16Elements<uint8_t, int16_t>(const uint8_t* const source, int16_t* const target)
{
	const uint8x16_t source_8x16 = vld1q_u8(source);

	const uint16x8_t source_16x8_0 = vmovl_u8(vget_low_u8(source_8x16));
	const uint16x8_t source_16x8_1 = vmovl_u8(vget_high_u8(source_8x16));

	vst1q_s16(target + 0, vreinterpretq_s16_u16(source_16x8_0));
	vst1q_s16(target + 8, vreinterpretq_s16_u16(source_16x8_1));
}

template <>
OCEAN_FORCE_INLINE void FrameConverter::cast16Elements<uint8_t, uint32_t>(const uint8_t* const source, uint32_t* const target)
{
	const uint8x16_t source_8x16 = vld1q_u8(source);

	const uint16x8_t source_16x8_0 = vmovl_u8(vget_low_u8(source_8x16));
	const uint16x8_t source_16x8_1 = vmovl_u8(vget_high_u8(source_8x16));

	const uint32x4_t source_32x4_0 = vmovl_u16(vget_low_u16(source_16x8_0));
	const uint32x4_t source_32x4_1 = vmovl_u16(vget_high_u16(source_16x8_0));
	const uint32x4_t source_32x4_2 = vmovl_u16(vget_low_u16(source_16x8_1));
	const uint32x4_t source_32x4_3 = vmovl_u16(vget_high_u16(source_16x8_1));

	vst1q_u32(target +  0, source_32x4_0);
	vst1q_u32(target +  4, source_32x4_1);
	vst1q_u32(target +  8, source_32x4_2);
	vst1q_u32(target + 12, source_32x4_3);
}

template <>
OCEAN_FORCE_INLINE void FrameConverter::cast16Elements<uint8_t, int32_t>(const uint8_t* const source, int32_t* const target)
{
	const uint8x16_t source_8x16 = vld1q_u8(source);

	const uint16x8_t source_16x8_0 = vmovl_u8(vget_low_u8(source_8x16));
	const uint16x8_t source_16x8_1 = vmovl_u8(vget_high_u8(source_8x16));

	const uint32x4_t source_32x4_0 = vmovl_u16(vget_low_u16(source_16x8_0));
	const uint32x4_t source_32x4_1 = vmovl_u16(vget_high_u16(source_16x8_0));
	const uint32x4_t source_32x4_2 = vmovl_u16(vget_low_u16(source_16x8_1));
	const uint32x4_t source_32x4_3 = vmovl_u16(vget_high_u16(source_16x8_1));

	vst1q_s32(target +  0, vreinterpretq_s32_u32(source_32x4_0));
	vst1q_s32(target +  4, vreinterpretq_s32_u32(source_32x4_1));
	vst1q_s32(target +  8, vreinterpretq_s32_u32(source_32x4_2));
	vst1q_s32(target + 12, vreinterpretq_s32_u32(source_32x4_3));
}

template <>
OCEAN_FORCE_INLINE void FrameConverter::cast16Elements<float, uint8_t>(const float* const source, uint8_t* const target)
{
	const float32x4_t source_32x4_0 = vld1q_f32(source +  0);
	const float32x4_t source_32x4_1 = vld1q_f32(source +  4);
	const float32x4_t source_32x4_2 = vld1q_f32(source +  8);
	const float32x4_t source_32x4_3 = vld1q_f32(source + 12);

	const uint32x4_t target_32x4_0 = vcvtq_u32_f32(source_32x4_0);
	const uint32x4_t target_32x4_1 = vcvtq_u32_f32(source_32x4_1);
	const uint32x4_t target_32x4_2 = vcvtq_u32_f32(source_32x4_2);
	const uint32x4_t target_32x4_3 = vcvtq_u32_f32(source_32x4_3);

	const uint16x8_t target_16x8_0 = vcombine_u16(vmovn_u32(target_32x4_0), vmovn_u32(target_32x4_1));
	const uint16x8_t target_16x8_1 = vcombine_u16(vmovn_u32(target_32x4_2), vmovn_u32(target_32x4_3));

	const uint8x16_t target_8x16 = vcombine_u8(vmovn_u16(target_16x8_0), vmovn_u16(target_16x8_1));

	vst1q_u8(target, target_8x16);
}

#endif // #if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <typename TSource, typename TTarget>
OCEAN_FORCE_INLINE void FrameConverter::cast16Elements(const TSource* const source, TTarget* const target)
{
	for (unsigned int i = 0u; i < 16u; ++i)
	{
		target[i] = TTarget(source[i]);
	}
}

template <typename T>
bool FrameConverter::subFrame(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned  int targetHeight, const unsigned int channels, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);

	if (sourceLeft + width > sourceWidth || sourceTop + height > sourceHeight || targetLeft + width > targetWidth || targetTop + height > targetHeight)
	{
		return false;
	}

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	const T* subSource = source + sourceStrideElements * sourceTop + sourceLeft * channels;
	T* subTarget = target + targetStrideElements * targetTop + targetLeft * channels;

	if (sourcePaddingElements == 0u && targetPaddingElements == 0u && width == sourceWidth && sourceWidth == targetWidth)
	{
		memcpy(subTarget, subSource, height * width * channels * sizeof(T));
	}
	else
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			memcpy(subTarget, subSource, width * channels * sizeof(T));

			subTarget += targetStrideElements;
			subSource += sourceStrideElements;
		}
	}

	return true;
}

template <typename T>
bool FrameConverter::subFrameMask(const Frame& sourceFrame, Frame& targetFrame, const Frame& maskFrame, const uint32_t sourceLeft, const uint32_t sourceTop, const uint32_t targetLeft, const uint32_t targetTop, const uint32_t subFrameWidth, const uint32_t subFrameHeight, const uint8_t maskValue)
{
	if (subFrameWidth == 0u || subFrameHeight == 0u)
	{
		return true;
	}

	if (!sourceFrame.isValid() || !targetFrame.isValid() || !maskFrame.isValid() || sourceFrame.numberPlanes() != 1u || targetFrame.numberPlanes() != 1u || maskFrame.numberPlanes() != 1u || FrameType::dataType<T>() != sourceFrame.dataType() || FrameType::dataType<T>() != targetFrame.dataType() || maskFrame.dataType() != FrameType::dataType<uint8_t>() || sourceFrame.channels() != targetFrame.channels() || maskFrame.width() != subFrameWidth || maskFrame.height() != subFrameHeight)
	{
		ocean_assert(false && "Invalid input");
		return false;
	}

	return subFrameMask<T>(sourceFrame.constdata<T>(), targetFrame.data<T>(), maskFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.channels(), sourceLeft, sourceTop, targetLeft, targetTop, subFrameWidth, subFrameHeight, sourceFrame.paddingElements(), targetFrame.paddingElements(), maskFrame.paddingElements(), maskValue);
}

template <typename T>
bool FrameConverter::subFrameMask(const T* source, T* target, const uint8_t* mask, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int maskPaddingElements, const uint8_t maskValue)
{
	ocean_assert(source != nullptr && target != nullptr && mask != nullptr);

	if (sourceLeft + subFrameWidth > sourceWidth || sourceTop + subFrameHeight > sourceHeight || targetLeft + subFrameWidth > targetWidth || targetTop + subFrameHeight > targetHeight)
	{
		ocean_assert(false && "Invalid input");
		return false;
	}

	const unsigned int maskStrideElements = subFrameWidth + maskPaddingElements;

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	for (unsigned int y = 0u; y < subFrameHeight; ++y)
	{
		const uint8_t* maskRow = mask + maskStrideElements * y;

		const T* subSource = source + sourceStrideElements * (sourceTop + y) + sourceLeft * channels;
		T* subTarget = target + targetStrideElements * (targetTop + y) + targetLeft * channels;

		for (unsigned int x = 0u; x < subFrameWidth; ++x)
		{
			if (*maskRow == maskValue)
			{
				for (unsigned int c = 0u; c < channels; ++c)
				{
					subTarget[c] = subSource[c];
				}
			}

			maskRow++;

			subSource += channels;
			subTarget += channels;
		}
	}

	return true;
}

template <typename T>
void FrameConverter::patchFrame(const T* source, T* buffer, const unsigned int width, const unsigned int channels, const unsigned int x, const unsigned int y, const unsigned int patchSize, const unsigned int sourcePaddingElements, const unsigned int bufferPaddingElements)
{
	ocean_assert(source != nullptr && buffer != nullptr);
	ocean_assert(width >= patchSize && channels >= 1u);

	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);
	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(x >= patchSize_2 && y >= patchSize_2);
	ocean_assert(x + patchSize_2 < width);

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
	const unsigned int bufferStrideElements = patchSize * channels + bufferPaddingElements;

	const unsigned int sourceLeft = x - patchSize_2;
	const unsigned int sourceTop = y - patchSize_2;

	source += sourceTop * sourceStrideElements + sourceLeft * channels;

	for (unsigned int row = 0u; row < patchSize; ++row)
	{
		memcpy(buffer, source, channels * patchSize * sizeof(T));

		source += sourceStrideElements;
		buffer += bufferStrideElements;
	}
}

template <typename T, unsigned int tChannels>
void FrameConverter::patchFrameMirroredBorder(const T* source, T* buffer, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const unsigned int patchSize, const unsigned int sourcePaddingElements, const unsigned int bufferPaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid number of color channels!");

	ocean_assert(source != nullptr && buffer != nullptr);

	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);
	const unsigned int patchSize_2 = patchSize / 2u;

	const unsigned int widthPatchSize1 = width - (patchSize - 1u);
	ocean_assert(widthPatchSize1 == width - patchSize_2 * 2u);

	ocean_assert(width >= patchSize_2 + 1u && height >= patchSize_2 + 1u);

	ocean_assert(x < width && y < height);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;

	for (int top = int(y - patchSize_2); top <= int(y + patchSize_2); ++top)
	{
		const T* sourceRow = source + CVUtilities::mirrorIndex(top, height) * int(sourceStrideElements);

		for (int left = int(x - patchSize_2); left <= int(x + patchSize_2); ++left)
		{
			if ((unsigned int)(left) - patchSize_2 < widthPatchSize1)
			{
				ocean_assert(left >= int(patchSize_2) && left < int(width - patchSize_2));

				const T* sourcePixel = sourceRow + left * tChannels;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					buffer[n] = sourcePixel[n];
				}
			}
			else
			{
				ocean_assert((unsigned int)(left) <= patchSize_2 || (unsigned int)(left) >= width - patchSize_2);

				const T* sourcePixel = sourceRow + CVUtilities::mirrorIndex(left, width) * tChannels;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					buffer[n] = sourcePixel[n];
				}
			}

			buffer += tChannels;
		}

		buffer += bufferPaddingElements;
	}
}

template <typename TSource, typename TTarget>
inline void FrameConverter::convertGenericPixelFormat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourceStrideElements, const unsigned int targetStrideElements, const ConversionFlag flag, const RowConversionFunction<TSource, TTarget> rowConversionFunction, const RowReversePixelOrderInPlaceFunction<TTarget> targetReversePixelOrderInPlaceFunction, const bool areContinuous, const void* options, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(sourceStrideElements >= width && targetStrideElements >= width);
	ocean_assert(rowConversionFunction != nullptr);
	ocean_assert(flag == CONVERT_NORMAL || flag == CONVERT_FLIPPED || targetReversePixelOrderInPlaceFunction != nullptr);

	// the internal subset conversion function needs bytes instead of elements

	const unsigned int sourceStrideBytes = sourceStrideElements * sizeof(TSource);
	const unsigned int targetStrideBytes = targetStrideElements * sizeof(TTarget);

	if (worker && height >= 200u)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameConverter::convertGenericPixelFormatSubset, (const uint8_t*)(source), (uint8_t*)(target), width, height, sourceStrideBytes, targetStrideBytes, flag, (const RowConversionFunction<uint8_t, uint8_t>)rowConversionFunction, (const RowReversePixelOrderInPlaceFunction<uint8_t>)targetReversePixelOrderInPlaceFunction, areContinuous, options, 0u, 0u), 0u, height, 11u, 12u, 20u);
	}
	else
	{
		convertGenericPixelFormatSubset((const uint8_t*)(source), (uint8_t*)(target), width, height, sourceStrideBytes, targetStrideBytes, flag, (const RowConversionFunction<uint8_t, uint8_t>)rowConversionFunction, (const RowReversePixelOrderInPlaceFunction<uint8_t>)targetReversePixelOrderInPlaceFunction, areContinuous, options, 0u, height);
	}
}

inline void FrameConverter::convertArbitraryPixelFormat(const void** sources, void** targets, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int multipleRowsPerIteration, const MultipleRowsConversionFunction multipleRowsConversionFunction, const void* options, Worker* worker)
{
	ocean_assert(multipleRowsPerIteration != 0u && height % multipleRowsPerIteration == 0u);

	const unsigned int rowIterations = height / multipleRowsPerIteration;

	if (worker && rowIterations >= 200u)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameConverter::convertArbitraryPixelFormatSubset, sources, targets, width, height, flag, multipleRowsPerIteration, multipleRowsConversionFunction, options, 0u, 0u), 0u, rowIterations, 8u, 9u, 20u);
	}
	else
	{
		convertArbitraryPixelFormatSubset(sources, targets, width, height, flag, multipleRowsPerIteration, multipleRowsConversionFunction, options, 0u, rowIterations);
	}
}

OCEAN_FORCE_INLINE void FrameConverter::unpack5ElementsBayerMosaicPacked10Bit(const uint8_t* const packed, uint16_t* unpacked)
{
	ocean_assert(packed != nullptr);
	ocean_assert(unpacked != nullptr);

	unpacked[0] = uint16_t(uint16_t(packed[0]) << uint16_t(2) | (uint16_t(packed[4]) & uint16_t(0b00000011)));
	unpacked[1] = uint16_t(uint16_t(packed[1]) << uint16_t(2) | ((uint16_t(packed[4]) & uint16_t(0b00001100)) >> uint16_t(2)));
	unpacked[2] = uint16_t(uint16_t(packed[2]) << uint16_t(2) | ((uint16_t(packed[4]) & uint16_t(0b00110000)) >> uint16_t(4)));
	unpacked[3] = uint16_t(uint16_t(packed[3]) << uint16_t(2) | (uint16_t(packed[4]) >> uint16_t(6)));
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <bool tAllowLastOverlappingElement>
OCEAN_FORCE_INLINE void FrameConverter::unpack15ElementsBayerMosaicPacked10BitNEON(const uint8_t* const packed, uint16x8_t& unpackedAB_u_16x8, uint16x4_t& unpackedC_u_16x4)
{
	constexpr uint8x8_t shuffleC_u_8x8 = {6u, 2u, 6u, 3u, 6u, 4u, 6u, 5u};

	constexpr int8x16_t leftShifts_s_8x16 = {6, 0, 4, 0, 2, 0, 0, 0, 6, 0, 4, 0, 2, 0, 0, 0};
	constexpr int16x8_t rightShifts_s_16x8 = {-6, -6, -6, -6, -6, -6, -6, -6};

	const uint8x16_t packed_u_8x16 = tAllowLastOverlappingElement ? vld1q_u8(packed) : vcombine_u8(vld1_u8(packed), vext_u8(vld1_u8(packed + 7), shuffleC_u_8x8, 1)); // shuffleC_u_8x8 is just a dummy value

	// F E D C B A 9 8 7 6 5 4 3 2 1 0
	// 8 9 7 9 6 9 5 9 3 4 2 4 1 4 0 4

#ifdef __aarch64__
	constexpr uint8x16_t shuffle_u_8x16 = {4u, 0u, 4u, 1u, 4u, 2u, 4u, 3u, 9u, 5u, 9u, 6u, 9u, 7u, 9u, 8u};
	const uint8x16_t intermediateAB_u_8x16 = vqtbl1q_u8(packed_u_8x16, shuffle_u_8x16);
#else
	const uint8x8_t packedA_u_8x8 = vget_low_u8(packed_u_8x16);
	const uint8x8_t packedB_u_8x8 = vget_low_u8(vextq_u8(packed_u_8x16, packed_u_8x16, 5));

	constexpr uint8x8_t shuffleAB_u_8x8 = {4u, 0u, 4u, 1u, 4u, 2u, 4u, 3u};
	const uint8x16_t intermediateAB_u_8x16 = vcombine_u8(vtbl1_u8(packedA_u_8x8, shuffleAB_u_8x8), vtbl1_u8(packedB_u_8x8, shuffleAB_u_8x8));
#endif // __aarch64__


	// 7 6 5 4 3 2 1 0
	// 5 6 4 6 3 6 2 6
	const uint8x8_t intermediateC_u_8x8 = vtbl1_u8(vget_high_u8(packed_u_8x16), shuffleC_u_8x8);


	// ... XXXXXX99 33333333 44XXXXXX 22222222 XX44XXXX 11111111 XXXX44XX 00000000 XXXXXX44
	// ... 99------ 33333333 44------ 22222222 44------ 11111111 44------ 00000000 44------
	const uint16x8_t intermediateAB_u_16x8 = vreinterpretq_u16_u8(vshlq_u8(intermediateAB_u_8x16, leftShifts_s_8x16));

	const uint16x4_t intermediateC_u_16x4 = vreinterpret_u16_u8(vshl_u8(intermediateC_u_8x8, vget_low_u8(leftShifts_s_8x16)));


	// ... 99------ 33333333 44------ 22222222 44------ 11111111 44------ 00000000 44------
	// ... 55555599 ------33 33333344 ------22 22222244 ------11 11111144 ------00 00000044
	unpackedAB_u_16x8 = vshlq_u16(intermediateAB_u_16x8, rightShifts_s_16x8);

	unpackedC_u_16x4 = vshl_u16(intermediateC_u_16x4, vget_low_u8(rightShifts_s_16x8));
}

#endif // OCEAN_HARDWARE_NEON_VERSION

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_H
