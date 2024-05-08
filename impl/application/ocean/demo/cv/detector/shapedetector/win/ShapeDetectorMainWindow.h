/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_SHAPEDETECTOR_WIN_SHAPE_DETECTOR_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_SHAPEDETECTOR_WIN_SHAPE_DETECTOR_MAIN_WINDOW_H

#include "application/ocean/demo/cv/detector/shapedetector/win/ShapeDetectorMain.h"

#include "ocean/io/File.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

#include "ocean/test/testcv/testdetector/TestShapeDetector.h"

using namespace Ocean;

/**
 * This class implements the main window for the shape detector demo app.
 * @ingroup applicationdemocvdetectorshapedetectorwin
 */
class ShapeDetectorMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow,
	protected Test::TestCV::TestDetector::TestShapeDetector
{
	public:

		/**
		 * Definition of individual shape detectors.
		 */
		enum ShapeDetector : uint32_t
		{
			/// An gradient-based shape detector.
			SD_GRADIENT_BASED = 0u,
			/// An variance-based shape detector.
			SD_VARIANCE_BASED,
			/// An variance-based shape detector.
			SD_GRADIENT_VARIANCE_BASED,
			/// A modified variance-based shape detector.
			SD_MODIFIED_GRADIENT_VARIANCE_BASED,
			/// An helper enum holding the number of detectors.
			SD_END
		};

	protected:

		/**
		 * This class combines all parameters for the gradient-based detector.
		 */
		class GradientBasedParameters
		{
			public:

				/**
				 * Initializes all parameters.
				 */
				GradientBasedParameters();

				/**
				 * Event function for a pressed key.
				 * @param keyString The string of the pressed key
				 * @return True, if the event was handled
				 */
				bool onKeyDown(const std::string& keyString);

				/**
				 * Translates the edge response strategy to a string.
				 * @param edgeResponseStrategy Strategy to translate
				 * @return The translated string
				 */
				static std::string translateEdgeResponseStrategy(const GradientBasedDetector::EdgeResponseStrategy edgeResponseStrategy);

				/**
				 * Translates the minimal response strategy to a string.
				 * @param minimalResponseStrategy Strategy to translate
				 * @return The translated string
				 */
				static std::string translateMinimalResponseStrategy(const GradientBasedDetector::MinimalResponseStrategy minimalResponseStrategy);

				/**
				 * Translates the penalty usage strategy to a string.
				 * @param penaltyUsage Strategy to translate
				 * @return The translated string
				 */
				static std::string translatePenaltyUsage(const GradientBasedDetector::PenaltyUsage penaltyUsage);

			public:

				/// The factor mulitplied with each response.
				double detectorResponseFactor_;

				/// The detection threshold.
				double detectorThreshold_;

				/// The expected shape sign.
				int shapeSign_;

				/// The penalty factor.
				double detectorPenaltyFactor_;

				/// The edge response strategy to apply.
				GradientBasedDetector::EdgeResponseStrategy detectorEdgeResponseStrategy_;

				/// The minimal responses strategy to apply.
				GradientBasedDetector::MinimalResponseStrategy detectorMinimalResponseStrategy_;

				/// The usage of penalty.
				GradientBasedDetector::PenaltyUsage detectorPenaltyUsage_;
		};

		/**
		 * This class combines all parameters for the variance-based detector.
		 */
		class VarianceBasedParameters
		{
			public:

				/**
				 * Initializes all parameters.
				 */
				VarianceBasedParameters();

				/**
				 * Event function for a pressed key.
				 * @param keyString The string of the pressed key
				 * @return True, if the event was handled
				 */
				bool onKeyDown(const std::string& keyString);

				/**
				 * Translates the threshold strategy to a string.
				 * @param detectorThresholdStrategy Strategy to translate
				 * @return The translated string
				 */
				static std::string translateThresholdStrategy(const VarianceBasedDetector::ThresholdStrategy detectorThresholdStrategy);

				/**
				 * Translates the gradient response strategy to a string.
				 * @param gradientResponseStrategy Strategy to translate
				 * @return The translated string
				 */
				static std::string translateGradientResponseStrategy(const VarianceBasedDetector::GradientResponseStrategy gradientResponseStrategy);

				/**
				 * Translates the band strategy to a string.
				 * @param bandStrategy Strategy to translate
				 * @return The translated string
				 */
				static std::string translateBandStrategy(const VarianceBasedDetector::BandStrategy bandStrategy);

			public:

				/// The factor mulitplied with each response.
				double detectorResponseFactor_;

				/// The detection threshold.
				double detectorThreshold_;

				/// The minimal variance value each response must have.
				double detectorMinimalVariance_;

				/// The maximal ration between vertical and horizontal responses.
				double detectorMaximalRatio_;

				/// The multiplication factor for the foreground variance.
				double detectorForegroundVarianceFactor_;

				/// The threshold strategy to be used.
				VarianceBasedDetector::ThresholdStrategy detectorThresholdStrategy_;

				/// The gradient response strategy to be used.
				VarianceBasedDetector::GradientResponseStrategy detectorGradientResponseStrategy_;

				/// The band strategy to be used.
				VarianceBasedDetector::BandStrategy detectorBandStrategy_;
		};

		/**
		 * This class combines all parameters for the gradient&variance-based detector.
		 */
		class GradientVarianceBasedParameters
		{
			public:

				/**
				 * Initializes all parameters.
				 */
				GradientVarianceBasedParameters();

				/**
				 * Event function for a pressed key.
				 * @param keyString The string of the pressed key
				 * @return True, if the event was handled
				 */
				bool onKeyDown(const std::string& keyString);

				/**
				 * Translates the band strategy to a string.
				 * @param bandStrategy Strategy to translate
				 * @return The translated string
				 */
				static std::string translateBandStrategy(const GradientVarianceBasedDetector::BandStrategy bandStrategy);

				/**
				 * Translates the optimization strategy to a string.
				 * @param optimizationStrategy Strategy to translate
				 * @return The translated string
				 */
				static std::string translateOptimizationStrategy(const GradientVarianceBasedDetector::OptimizationStrategy optimizationStrategy);

			public:

				/// The factor mulitplied with each response.
				double detectorResponseFactor_;

				/// The detection threshold.
				double detectorThreshold_;

				/// The expected shape sign.
				int shapeSign_;

				/// The maximal ration between vertical and horizontal responses.
				double detectorMaximalRatio_;

				/// The band strategy to be used.
				GradientVarianceBasedDetector::BandStrategy detectorBandStrategy_;

				/// The optimization strategy to be used.
				GradientVarianceBasedDetector::OptimizationStrategy detectorOptimizationStrategy_;
		};

		/**
		 * This class combines all parameters for the modified gradient&variance-based detector.
		 */
		class ModifiedGradientVarianceBasedParameters
		{
			public:

				/**
				 * Initializes all parameters.
				 */
				ModifiedGradientVarianceBasedParameters();

				/**
				 * Event function for a pressed key.
				 * @param keyString The string of the pressed key
				 * @return True, if the event was handled
				 */
				bool onKeyDown(const std::string& keyString);

				/**
				 * Translates the band strategy to a string.
				 * @param bandStrategy Strategy to translate
				 * @return The translated string
				 */
				static std::string translateBandStrategy(const GradientVarianceBasedDetector::BandStrategy bandStrategy);

			public:

				/// The factor mulitplied with each response.
				double detectorResponseFactor_;

				/// The detection threshold.
				double detectorThreshold_;

				/// The expected shape sign.
				int shapeSign_;

				/// The maximal ration between vertical and horizontal responses.
				double detectorMaximalRatio_;

				/// The band strategy to be used.
				GradientVarianceBasedDetector::BandStrategy detectorBandStrategy_;
		};

	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name name of the main window
		 * @param filenames The filenames of all images to be loaded
		 */
		ShapeDetectorMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::string>& filenames);

		/**
		 * Destructs the main window.
		 */
		virtual ~ShapeDetectorMainWindow();

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function for keyboard button down events.
		 * @see Window::onKeyDonw().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Event functions for drag&drop events for files.
		 * @see Window::onDragAndDrop().
		 */
		void onDragAndDrop(const Files& files) override;

		/**
		 * Resets the parameters.
		 */
		void resetParameters();

		/**
		 * Update the image to be analyzed.
		 * @param filename The filename of the new image, must be valid
		 * @return True, if the image could be updated/loaded
		 */
		bool updateImage(const std::string& filename);

		/**
		 * Applies shape detector and updates the visualization.
		 */
		void invokeShapeDetector();

		/**
		 * Translates the shape detector to a string.
		 * @param shapeDetector Shape detector to translate
		 * @return The translated string
		 */
		static std::string translateShapeDetector(const ShapeDetector shapeDetector);

		/**
		 * Translates the response type to a string.
		 * @param responseType Response type to translate
		 * @return The translated string
		 */
		static std::string translateResponseType(const ResponseType responseType);

		/**
		 * Translates the response visualization to a string.
		 * @param responseVisualization Response visualization to translate
		 * @return The translated string
		 */
		static std::string translateResponseVisualization(const ResponseVisualization responseVisualization);

	protected:

		/// The filenames of the images to analyze.
		IO::Files inputFiles_;

		/// The index of the current image to be analyzed.
		size_t currentInputIndex_;

		/// The shape detector which is currently used.
		ShapeDetector shapeDetector_;

		/// Window for top-down responses.
		Platform::Win::BitmapWindow windowResponsesTopDown_;

		/// Window for bottom-up responses.
		Platform::Win::BitmapWindow windowResponsesBottomUp_;

		/// Window to display the parameters.
		Platform::Win::BitmapWindow windowParameters_;

		/// The image to be analyzed.
		Frame yImage_;

		/// Linear gain for visibility.
		double visibilityLinearGain_;

		/// True, to show the response images; False, to show the input image.
		bool showResponses_;

		/// The orientation angle to be applied, in radian.
		double rotation_;

		/// The width of the shape, in pixel.
		unsigned int shapeWidth_;

		/// The height of the shape, in pixel.
		unsigned int shapeHeight_;

		/// The step size of the shape, in pixel.
		unsigned int shapeStepSize_;

		/// The size of the top band, in pixel.
		unsigned int shapeTopBand_;

		/// The size of the bottom band, in pixel.
		unsigned int shapeBottomBand_;

		/// The minimal edge response.
		double detectorMinimalEdgeResponse_;

		/// The radius in which the non-maximum suppression is applied.
		double detectorNonMaximumSuppressionRadius_;

		/// True, to apply the detector to the transposed image; False, to apply the detector to the image without transposing it
		bool transposed_;

		/// The response type to be used.
		ResponseType responseType_;

		/// The response visualization method to be used.
		ResponseVisualization responseVisualization_;

		/// The parameters of the gradient-based detector.
		GradientBasedParameters gradientBasedParameters_;

		/// The parameters of the variance-based detector.
		VarianceBasedParameters varianceBasedParameters_;

		/// The parameters of the variance-based detector.
		GradientVarianceBasedParameters gradientVarianceBasedParameters_;

		/// The parameters of the modified variance-based detector.
		ModifiedGradientVarianceBasedParameters modifiedGradientVarianceBasedParameters_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_SHAPEDETECTOR_WIN_SHAPE_DETECTOR_MAIN_WINDOW_H
