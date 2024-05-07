// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TRACKING_PATTERN_PATTERN_TRACKER_CORE_6DOF_H
#define META_OCEAN_TRACKING_PATTERN_PATTERN_TRACKER_CORE_6DOF_H

#include "ocean/tracking/pattern/Pattern.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/SubRegion.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/io/Bitstream.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

#include "ocean/tracking/VisualTracker.h"

#include "ocean/tracking/blob/Blob.h"
#include "ocean/tracking/blob/FeatureMap.h"
#include "ocean/tracking/uvtexturemapping/UVTextureMapping.h"

namespace Ocean
{

namespace Tracking
{

namespace Pattern
{

/**
 * This class implements the core of the 6DOF feature tracker for planar patterns.
 * This 'core' class is separated from the general PatternTracker6DOF class to avoid any virtual functions.<br>
 * Virtual functions may increase the binary size as the compiler/linker may not be able to identify unused virtual functions as 'dead', and thus the linker will not be able to strip such functions.<br>
 * However, if the PatternTracker6DOF's object oriented capability is not needed anyway the usage of this core class is recommended (especially if the binary size matters).
 * @see PatternTracker6DOF.
 * @ingroup trackingpattern
 */
class OCEAN_TRACKING_PATTERN_EXPORT PatternTrackerCore6DOF
{
	public:

		/**
		 * Set of configurable parameters for the tracker.
		 */
		class Options
		{
			public:

				/**
				 * Creates a new options object.
				 */
				inline Options() noexcept;

			public:

				/// The maximal number of patterns that can be visible concurrently, with range [1, infinity).
				/// If equal to zero, then no limit will be used.
				unsigned int maxConcurrentlyVisiblePattern = 1u;

				/// Maximum number of features to extract from a given input frame during recognition.
				/// If equal to zero, then no limit will be used.
				unsigned int maxNumberFeatures = 0u;

				/// The maximal time used for pattern recognition for each frame in seconds, with range (0, infinity).
				/// If the provided value is <= 0 when the tracker is created, then a default value will be selected.
				double maxRecognitionTime = 0.0;

				/// Optional random seed to use internally for, e.g., RANSAC.
				/// If unset when the tracker is created, a time-based seed is used.
				const unsigned int* randomSeed = nullptr;

				/// Time in seconds to wait between recognition attempts when at least one pattern is currently being tracked.
				/// If the value is <= 0, a default value will be chosen.
				double recognitionCadenceWithTrackedPatterns = 0.5;

				/// Time in seconds to wait between recognition attempts when no patterns are currently being tracked.
				/// If the provided value is < 0, it is ignored and set to zero.
				double recognitionCadenceWithoutTrackedPatterns = 0.0;

				/// The number of iterations to run RANSAC when attempting to verify a newly recognized target.
				unsigned int recognitionRansacIterations = 50u;

				/// True, to skip frame-to-frame tracking and to apply a full re-detection for every frame.
				bool noFrameToFrameTracking = false;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

				/// True, to apply a downsampling on Android devices to improve performance on low end devices.
				bool downsampleInputImageOnAndroid = true;
#endif
		};

	protected:

		/**
		 * This class stores the information necessary for one tracking pattern.
		 */
		class Pattern
		{
			protected:

				/**
				 * Definition of a vector holding 2D feature positions.
				 */
				typedef std::vector<Vectors2> PointLayers;

			public:

				/**
				 * Creates a new invalid pattern object.
				 */
				inline Pattern();

				/**
				 * Creates a new pattern object by a given frame and pattern dimension.
				 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking pattern, must be valid
				 * @param width The width of the given grayscale frame in pixel, with range [1, infinity)
				 * @param height The height of the given grayscale frame in pixel, with range [1, infinity)
				 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
				 * @param dimension The dimension of the tracking pattern, with range (0, infinity)x(0, infinity)
				 * @param worker Optional worker object to distribute the computation
				 */
				Pattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, Worker* worker = nullptr);

				/**
				 * Creates a new pattern object by a given frame and pattern dimension.
				 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking pattern, must be valid
				 * @param width The width of the given grayscale frame in pixel, with range [1, infinity)
				 * @param height The height of the given grayscale frame in pixel, with range [1, infinity)
				 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
				 * @param representativeFeatures Representative features that should be used for an initial pattern recognition instead of all features from the entire pattern, must be at least 7
				 * @param dimension The dimension of the tracking pattern, with range (0, infinity)x(0, infinity)
				 * @param worker Optional worker object to distribute the computation
				 */
				Pattern(const uint8_t* yFrame, const unsigned int width, unsigned int height, const unsigned int yFramePaddingElements, CV::Detector::Blob::BlobFeatures&& representativeFeatures, const Vector2& dimension, Worker* worker = nullptr);

				/**
				 * Creates a new cylinder-type tracking pattern (an image of a flattened cylinder).
				 * For more information, see FeatureMap::CylinderUVTextureMapping.
				 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking pattern, must be valid
				 * @param width The width of the given grayscale frame in pixel, with range [1, infinity)
				 * @param height The height of the given grayscale frame in pixel, with range [1, infinity)
				 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
				 * @param cylinderUVTextureMapping The mapping from the provided image into 3D coordinates
				 * @param worker Optional worker object to distribute the computation
				 */
				Pattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const UVTextureMapping::CylinderUVTextureMapping& cylinderUVTextureMapping, Worker* worker = nullptr);

				/**
				 * Creates a new cone-type tracking pattern (an image of a flattened cone).
				 * For more information, see FeatureMap::ConeUVTextureMapping.
				 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking pattern, must be valid
				 * @param width The width of the given grayscale frame in pixel, with range [1, infinity)
				 * @param height The height of the given grayscale frame in pixel, with range [1, infinity)
				 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
				 * @param coneUVTextureMapping The mapping from the provided image into 3D coordinates
				 * @param worker Optional worker object to distribute the computation
				 */
				Pattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const UVTextureMapping::ConeUVTextureMapping& coneUVTextureMapping, Worker* worker = nullptr);

				/**
				 * Returns the Blob feature map of this pattern.
				 * @return Blob feature map
				 */
				inline const Blob::FeatureMap& featureMap() const;

				/**
				 * Returns the frame pyramid of the image defining the tracking pattern.
				 * @return Pattern frame pyramid
				 */
				inline const CV::FramePyramid& pyramid() const;

				/**
				 * Returns the dimension of the tracking object defined in the tracker coordinate system.
				 * @return Pattern dimension
				 */
				inline const Vector2& dimension() const;

				/**
				 * Returns the first 3D corner position of the tracking pattern in the tracker coordinate system. Only to be used for planar patterns.
				 * @return The upper left corner of the tracking pattern (for a planar pattern, identical with the origin of the world coordinate system).
				 */
				inline Vector3 corner0() const;

				/**
				 * Returns the second 3D corner position of the tracking pattern in the tracker coordinate system. Only to be used for planar patterns.
				 * @return The lower left corner of the tracking pattern
				 */
				inline Vector3 corner1() const;

				/**
				 * Returns the third 3D corner position of the tracking pattern in the tracker coordinate system. Only to be used for planar patterns.
				 * @return The lower right corner of the tracking pattern
				 */
				inline Vector3 corner2() const;

				/**
				 * Returns the fourth 3D corner position of the tracking pattern in the tracker coordinate system. Only to be used for planar patterns.
				 * @return The upper right corner of the tracking pattern
				 */
				inline Vector3 corner3() const;

				/**
				 * Returns the two 3D triangles covering the tracking area of this tracking pattern.
				 * @return Two 3D triangles
				 */
				inline Triangles3 triangles3() const;

				/**
				 * Returns the two projected 2D triangles specifying the tracking pattern in the camera frame as seen with the previous pose (which is stored in this object).
				 * Beware: The previous pose must be valid.<br>
				 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
				 * @return The two projected 2D triangles
				 */
				inline Triangles2 triangles2(const PinholeCamera& pinholeCamera) const;

				/**
				 * Returns the two projected 2D triangles specifying the tracking pattern in the camera frame as seen with a given pose.
				 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
				 * @param pose The pose defining the camera position and orientation, must be valid
				 * @return The two projected 2D triangles
				 */
				inline Triangles2 triangles2(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose) const;

				/**
				 * Returns the previous camera pose from which this tracking pattern has been seen.
				 * @return Previous camera pose, if any
				 */
				inline const HomogenousMatrix4& previousPose() const;

				/**
				 * Returns the previous camera pose from which this tracking pattern has been seen.
				 * @return Previous camera pose, if any
				 */
				inline HomogenousMatrix4& previousPose();

				/**
				 * Returns the 3D object points of this pattern which have been used in the previous (or current) tracking iteration.
				 * @return 3D object points lying in the pattern plane, each point corresponds with one image point
				 * @see imagePoints().
				 */
				inline const Vectors3& objectPoints() const;

				/**
				 * Returns the 3D object points of this pattern which have been used in the previous (or current) tracking iteration.
				 * @return 3D object points lying in the pattern plane, each point corresponds with one image point
				 * @see imagePoints().
				 */
				inline Vectors3& objectPoints();

				/**
				 * Returns the 2D image points of this pattern which have been used in the previous (or current) tracking iteration.
				 * @return 2D image points, each point corresponds with one object point
				 * @see objectPoints().
				 */
				inline const Vectors2& imagePoints() const;

				/**
				 * Returns the 2D image points of this pattern which have been used in the previous (or current) tracking iteration.
				 * @return 2D image points, each point corresponds with one object point
				 * @see objectPoints().
				 */
				inline Vectors2& imagePoints();

				/**
				 * Returns the 2D feature points from the pyramid frame of the pattern image.
				 * @param layer The index of the pyramid layer, with range [0, layer())
				 * @return The 2D feature points of the specified layer
				 * @see layer().
				 */
				inline const Vectors2& referencePoints(const unsigned int layer) const;

				/**
				 * Returns the number of pyramid layers of the pattern image.
				 * @return The number of layers
				 * @see referencePoints().
				 */
				inline unsigned int layers() const;

				/**
				 * Returns whether this pattern holds representative feature points.
				 * Representative feature points are a subset of all possible feature points but with high reliability and proven to be good recognizable.
				 * @return True, if so
				 */
				inline bool hasRepresentativeFeatures() const;

				/**
				 * Returns whether this pattern holds a valid/useful rough guess of the camera pose.
				 * @param poseGuess The resulting rough pose guess, if existing
				 * @param maximalAge The maximal age of the rough pose guess in seconds, with range [0, 2]
				 * @return True, if so
				 */
				inline bool hasPoseGuess(HomogenousMatrix4& poseGuess, const double maximalAge = 0.5);

				/**
				 * Returns a guess of the current camera pose for this pattern.
				 * @param timestamp Optional timestamp of the pose guess
				 * @return pose The camera pose guess, if any
				 */
				inline const HomogenousMatrix4& poseGuess(Timestamp* timestamp = nullptr);

				/**
				 * Sets a guess of the current (or next) camera pose for this pattern.
				 * @param pose guess The rough pose guess
				 * @param timestamp The timestamp of this guess
				 */
				inline void setPoseGuess(const HomogenousMatrix4& pose, const Timestamp& timestamp);

				/**
				 * Tries to recognize this pattern based on a set of live camera features.
				 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
				 * @param features The live camera features used to recognize this pattern, at least 7
				 * @param randomGenerator Random number generator
				 * @param recognitionPose The resulting 6-DOF camera pose if the pattern could be recognized, will not be very accurate
				 * @param worker Optional worker to distribute the computation
				 * @return True, if the pattern could be recognized
				 */
				bool recognizePattern(const PinholeCamera& pinholeCamera, const CV::Detector::Blob::BlobFeatures& features, RandomGenerator& randomGenerator, HomogenousMatrix4& recognitionPose, Worker* worker);

				/**
				 * Returns whether this tracking pattern object is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns whether the underlying shape for this pattern is a plane.
				 * @return True, if so
				 */
				inline bool isPlanar() const;

				/**
				 * Returns whether the underlying shape for this pattern is a cylinder.
				 * @return True, if so
				 */
				inline bool isCylindrical() const;

				/**
				 * Returns whether the underlying shape for this pattern is a cone.
				 * @return True, if so
				 */
				inline bool isConical() const;

				/**
				 * Resets the internal recognition states of this pattern while the actual feature map is untouched.
				 */
				void reset();

				/**
				 * Returns whether this tracking pattern object is valid.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			protected:

				/// The Blob feature map of this pattern.
				Blob::FeatureMap patternFeatureMap;

				/// Optional representative feature points.
				CV::Detector::Blob::BlobFeatures patternRepresentativeFeatures;

				/// The frame pyramid of the image specifying the pattern.
				CV::FramePyramid patternPyramid;

				/// The dimension of the tracking pattern defined in the tracker coordinate system as (x-axis, z-axis).
				Vector2 patternDimension;

				/// The previous camera pose for this tacking pattern, if any
				HomogenousMatrix4 patternPreviousPose;

				/// The 3D object points which have been used in the previous (or current) tracking iteration.
				Vectors3 patternObjectPoints;

				/// The 2D image points which have been used in the previous (or current) tracking iteration.
				Vectors2 patternImagePoints;

				/// The point pyramid of the pattern image storing reference feature points for individual pattern resolutions.
				PointLayers patternPyramidReferencePoints;

				/// A rough guess of the camera pose for this pattern, if any.
				HomogenousMatrix4 patternPoseGuess;

				/// The timestamp of the rough camera pose.
				Timestamp patternPoseGuessTimestamp;

		};

		/**
		 * Definition of a map holding pattern objects.
		 */
		typedef std::map<unsigned int, Pattern> PatternMap;

	public:

		/**
		 * Creates a new feature tracker object.
		 * @param options Set of parameters for the tracker
		 */
		explicit PatternTrackerCore6DOF(const Options& options = Options());

		/**
		 * Destructs a feature tracker object.
		 */
		~PatternTrackerCore6DOF();

		/**
		 * Adds a new 2D tracking pattern (an image) to the tracker.
		 * The origin of the pattern will be located in the upper left corner of the given frame.<br>
		 * The pattern lies inside the x-z-plane with y-axis as up-vector.
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking pattern, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [1, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [1, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @param dimension The dimension of the tracking pattern in the tracker coordinate system, with range (0, infinity)x(-infinity, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return The id of the tracking pattern, -1 if the pattern could not be added
		 * @see removePattern(), numberPattern(), setMaxConcurrentlyVisiblePattern().
		 */
		unsigned int addPattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, Worker* worker = nullptr);

		/**
		 * Adds a new 2D tracking pattern (an image) to the tracker.
		 * The origin of the pattern will be located in the upper left corner of the given frame.<br>
		 * The pattern lies inside the x-z-plane with y-axis as up-vector.<br>
		 * This function takes a file in which the pattern is defined, the file can be a simple image or a feature map storing an additional hierarchy of feature points.
		 * @param filename The filename of the file storing the pattern information, must be valid
		 * @param dimension The dimension of the tracking pattern in the tracker coordinate system, with range (0, infinity)x(-infinity, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return The id of the tracking pattern, -1 if the pattern could not be added
		 * @see removePattern(), numberPattern(), setMaxConcurrentlyVisiblePattern().
		 */
		unsigned int addPattern(const std::string& filename, const Vector2& dimension, Worker* worker = nullptr);

		/**
		 * Adds a new cylinder-type tracking pattern (an image of a flattened cylinder) to the tracker.
		 * For more information, see FeatureMap::CylinderUVTextureMapping.
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking pattern, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [1, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [1, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @param cylinderUVTextureMapping The mapping from the provided image into 3D coordinates
		 * @param worker Optional worker object to distribute the computation
		 * @return The id of the tracking pattern, -1 if the pattern could not be added
		 */
		unsigned int addCylinderPattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const UVTextureMapping::CylinderUVTextureMapping& cylinderUVTextureMapping, Worker* worker = nullptr);

		/**
		 * Adds a new cone-type tracking pattern (an image of a flattened cone) to the tracker.
		 * For more information, see FeatureMap::ConeUVTextureMapping.
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking pattern, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [1, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [1, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @param coneUVTextureMapping The mapping from the provided image into 3D coordinates
		 * @param worker Optional worker object to distribute the computation
		 * @return The id of the tracking pattern, -1 if the pattern could not be added
		 */
		unsigned int addConePattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const UVTextureMapping::ConeUVTextureMapping& coneUVTextureMapping, Worker* worker = nullptr);

		/**
		 * Removes a pattern from this tracker.
		 * @param patternId The id of the pattern to be removed
		 * @return True, if the defined pattern could be removed; False, if e.g., the pattern id is invalid
		 * @see removePatterns().
		 */
		bool removePattern(const unsigned int patternId);

		/**
		 * Removes all patterns from this tracker.
		 * @return True, if all existing patterns could be removed
		 * @see removePattern().
		 */
		bool removePatterns();

		/**
		 * Executes the 6DOF tracking for a given frame.
		 * Beware: The frame type of the input image must not change between successive calls, reset the tracker in case the image resolution changes.
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) to be used for tracking (the frame's width and hight will be extracted from the camera profile), must be valid
		 * @param pinholeCamera The pinhole camera object defining the project, with same dimension as the given frame, must be valid
		 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus feature must not be undistorted explicitly
		 * @param timestamp The timestamp of the given frame, must be valid
		 * @param transformations Resulting 6DOF poses combined with the tracking ids
		 * @param world_R_camera Optional absolute orientation of the camera in the moment the frame was taken, defined in a coordinate system not related with the tracking objects, an invalid object otherwise
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		inline bool determinePoses(const uint8_t* yFrame, const PinholeCamera& pinholeCamera, const unsigned int yFramePaddingElements, const bool frameIsUndistorted, const Timestamp& timestamp, VisualTracker::TransformationSamples& transformations, const Quaternion& world_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Executes the 6DOF tracking for a given frame and optionally skips recognition.
		 * Beware: The frame type of the input image must not change between successive calls, reset the tracker in case the image resolution changes.
		 * @param allowRecognition If false, skip feature extraction and matching for this frame
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) to be used for tracking (the frame's width and hight will be extracted from the camera profile), must be valid
		 * @param pinholeCamera The pinhole camera object defining the project, with same dimension as the given frame, must be valid
		 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus feature must not be undistorted explicitly
		 * @param timestamp The timestamp of the given frame, must be valid
		 * @param transformations Resulting 6DOF poses combined with the tracking ids
		 * @param world_R_camera Optional absolute orientation of the camera in the moment the frame was taken, defined in a coordinate system not related with the tracking objects, an invalid object otherwise
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePoses(const bool allowRecognition, const uint8_t* yFrame, const PinholeCamera& pinholeCamera, const unsigned int yFramePaddingElements, const bool frameIsUndistorted, const Timestamp& timestamp, VisualTracker::TransformationSamples& transformations, const Quaternion& world_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Returns the number of registered/added pattern.
		 * @return The number of existing pattern, with range [0, infinity)
		 * @see addPattern(), setMaxConcurrentlyVisiblePattern().
		 */
		inline unsigned int numberPattern() const;

		/**
		 * Returns the maximal number of pattern that can be tracked concurrently within one frame.
		 * @return The number of pattern that can be tracked concurrently, with range [0, numberPattern()], 0 to track as much as possible, 1 by default
		 * @see setMaxConcurrentlyVisiblePattern(), numberPattern().
		 */
		inline unsigned int maxConcurrentlyVisiblePattern() const;

		/**
		 * Sets the maximal number of pattern that can be tracked concurrently within one frame.
		 * @param maxConcurrentlyVisiblePattern The number of pattern that can be tracked concurrently, with range [0, infinity), 0 to track as much as possible
		 * @see maxConcurrentlyVisiblePattern(), numberPattern().
		 */
		inline void setMaxConcurrentlyVisiblePattern(const unsigned int maxConcurrentlyVisiblePattern);

		/**
		 * Resets the tracker's states but keeps all registered pattern.
		 * This function should be used e.g., whenever the resolution of the input image changes.<br>
		 * The tracker is simply reset to a state before the first call of determinePoses().
		 */
		void reset();

		/**
		 * Converts a known camera pose determined by this pattern tracker to a corresponding camera pose based on a difference camera profile.
		 * Beware: The conversion is an approximation only and does not reflect a mathematic perfect solution.
		 * @param newCamera The new camera profile for which the new pose will be calculated, must be valid
		 * @param referenceCamera The camera profile for which the known (reference) pose has been determined by this pattern tracker, must be valid
		 * @param referencePose The known (reference) pose which will be converted based on the new camera profile, must be valid
		 * @param newPose The resulting new pose matching with the new camera profile
		 * @return True, if succeeded
		 */
		static bool convertPoseForCamera(const PinholeCamera& newCamera, const PinholeCamera& referenceCamera, const HomogenousMatrix4& referencePose, HomogenousMatrix4& newPose);

		/**
		 * Writes "Version 4" quantized blob features to a bitstream.
		 * V4 is suitable for geometric verification, where we only need (x, y) observation points; all other values, such as a scale, are not serialized.
		 * All descriptor elements are quantized from floating point with range [-1,1] to the range [0,256] and then rounded down to an 8-bit integer (note that 1 also maps to 255).
		 * All descriptors must have the same number of elements.
		 * @param features The feature to be written, not more than 64 million
		 * @param bitstream The output bitstream to which the information will be written
		 * @return True, if succeeded
		 */
		static bool writeQuantizedFeaturesForGeometricVerification(const CV::Detector::Blob::BlobFeatures& features, IO::OutputBitstream& bitstream);

		/**
		 * Writes blob features to a bitstream.
		 * @param features The feature to be written, not more than 64 million
		 * @param bitstream The output bitstream to which the information will be written
		 * @return True, if succeeded
		 */
		static bool writeFeatures(const CV::Detector::Blob::BlobFeatures& features, IO::OutputBitstream& bitstream);

		/**
		 * Reads blob features from a bitstream.
		 * @param bitstream The input bitstream from which the information will be read
		 * @param features The resulting features received from the bitstream
		 * @param version Optional out parameter to return the version parsed out of the bitstream
		 * @return True, if succeeded
		 */
		static bool readFeatures(IO::InputBitstream& bitstream, CV::Detector::Blob::BlobFeatures& features, uint64_t* version = nullptr);

		/**
		 * Writes a file containing a feature map of a 2D image pattern and optional including a subset of feature points providing a compact representation of the pattern.
		 * @param filename The name of the resulting feature map file, must be valid
		 * @param pattern The 2D image pattern for which the feature map will be written, must be valid
		 * @param representativeFeatures Optional set of features providing a good representation of the pattern, with positions defined in the pixel domain of the pattern
		 * @return True, if succeeded
		 */
		static bool writeFeatureMap(const std::string& filename, const Frame& pattern, const CV::Detector::Blob::BlobFeatures& representativeFeatures);

		/**
		 * Reads a feature map of a 2D image pattern from a file while the file may also include a subset of feature points providing a compact representation of the 2D pattern.
		 * @param filename The name of the file providing the feature map
		 * @param pattern The resulting 2D image pattern encapsulated in the file
		 * @param representativeFeatures Optional resulting set of features providing a good representation of the pattern, with positions defined in the pixel domain of the pattern
		 * @return True, if succeeded
		 */
		static bool readFeatureMap(const std::string& filename, Frame& pattern, CV::Detector::Blob::BlobFeatures& representativeFeatures);

		/**
		 * Returns the 2D image points of this pattern which have been used in the previous (or current) tracking iteration.
		 * @return 2D image points, each point corresponds with one object point
		 * @see objectPoints().
		 */
		const Vectors2* trackedImagePoints(unsigned int patternId) const;

		/**
			* Returns the 3D object points of this pattern which have been used in the previous (or current) tracking iteration.
			* @return 3D object points, each point corresponds with one tracked image point
			*/
		const Vectors3* trackedObjectPoints(unsigned int patternId) const;

	protected:

		/**
		 * Computes the maximum allowed time between recognition attempts, which may depend on whether or not any targets are currently being tracked.
		 * @return Maximum allowed duration in seconds, with range [0, infinity)
		 */
		inline double maximumDurationBetweenRecognitionAttempts() const;

		/**
		 * Determines the 6DOF tracking for a given frame.
		 * @param allowRecognition If false, skip feature extraction and matching for this frame
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format) to be used for tracking, must be valid
		 * @param pinholeCamera The pinhole camera object defining the projection, with same dimension as the given frame
		 * @param previousCamera_R_camera Optional relative orientation between the previous frame and the current frame, if known
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePoses(const bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Determines the 6DOF tracking for a given frame which has been downsampled.
		 * @param allowRecognition If false, skip feature extraction and matching for this frame
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format) to be used for tracking, must be valid
		 * @param pinholeCamera The pinhole camera object defining the projection, with same dimension as the given frame
		 * @param previousCamera_R_camera Optional relative orientation between the previous frame and the current frame, if known
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePosesWithDownsampledResolution(const bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Determines the 6DOF poses for the registered patterns without any a-priori information.
		 * @param pinholeCamera The pinhole camera object associated with the frame
		 * @param yFrame The current camera frame with grayscale pixel format (Y8), must be valid
		 * @param currentFramePyramid The frame pyramid of the current frame
		 * @param previousCamera_R_camera Optional relative orientation between the previous frame and the current frame, if known
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePosesWithoutKnowledge(const PinholeCamera& pinholeCamera, const Frame& yFrame, const CV::FramePyramid& currentFramePyramid, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Creates the integral image of the given frame.
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format) from which the integral frame will be created, must be valid
		 * @return Pointer to the integral image data
		 */
		const unsigned int* createIntegralImage(const Frame& yFrame);

		/**
		 * Counts the number of currently visible pattern.
		 * @return The number of visible pattern
		 */
		inline unsigned int internalNumberVisiblePattern() const;

		/**
		 * Returns the maximal number of pattern that can be tracked concurrently within one frame.
		 * This function is an internal helper function providing the number of pattern that are allowed to be visible concurrently.
		 * @return The number of pattern that can be tracked concurrently, with range [1, numberPattern()]
		 */
		inline unsigned int internalMaxConcurrentlyVisiblePattern() const;

		/**
		 * Determine the camera pose for the current camera frame and for a given pattern by application of 2D/3D feature points correspondences determined for the previous camera frame.
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param previousFramePyramid The frame pyramid of the previous frame, must be valid
		 * @param currentFramePyramid The frame pyramid of the current camera frame, with same frame type and number of layers as 'previousFramePyramid', must be valid
		 * @param pattern The tracking pattern object which is needs to be re-found (re-tracked) in the current camera frame
		 * @param previousCamera_R_camera Optional relative orientation between the previous frame and the current frame, if known
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the pattern could be tracked reliably
		 */
		static bool determinePoseWithPreviousCorrespondences(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, Pattern& pattern, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Determines the camera pose for extreme camera motion for a given pattern for which a pose guess is known.
		 * The resulting pose will not be free of drift errors as the pose is determine between successive camera frames only.
		 * However, the resulting pose will be better than no pose (in most situations).
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param previousFramePyramid The frame pyramid of the previous frame, must be valid
		 * @param currentFramePyramid The frame pyramid of the current camera frame, with same frame type and number of layers as 'previousFramePyramid', must be valid
		 * @param pattern The tracking pattern object which is needs to be re-found (re-tracked) in the current camera frame
		 * @param previousCamera_R_camera Optional relative orientation between the previous frame and the current frame, if known
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the pattern could be tracked with a drift-error-based pose
		 */
		static bool determinePoseWithDriftErrors(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, Pattern& pattern, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Tracks a set of 2D/3D points correspondences from the previous frame to the current frame (in a specified pyramid layer).
		 * @param pinholeCamera The pinhole camera profile to be used
		 * @param previousFramePyramid The frame pyramid of the previous frame
		 * @param currentFramePyramid The frame pyramid of the current frame
		 * @param trackingLayer The pyramid layer which is used for tracking
		 * @param previousPose The camera pose for the previous frame
		 * @param previousObjectPoints The 3D object points which have been used to determine the previous pose
		 * @param previousImagePoints The 2D image points which have been used to determine the previous pose, each points has a corresponding 3D object points, the points are defined in the finest pyramid layer
		 * @param roughPose Resulting pose determined for tracked points in the specified pyramid layer 'trackingLayer'
		 * @param worker Optional worker object to distribute the computation
		 * @param numberFeatures The number of feature points which will be used for tracking, with range [3, infinity)
		 * @param maxError The maximal square error between bidirectional tracked feature points
		 * @return True, if succeeded
		 */
		static bool trackFrame2FrameHierarchy(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const unsigned int trackingLayer, const HomogenousMatrix4& previousPose, const Vectors3& previousObjectPoints, const Vectors2& previousImagePoints, HomogenousMatrix4& roughPose, Worker* worker = nullptr, const unsigned int numberFeatures = 20u, const Scalar maxError = Scalar(0.9 * 0.9));

		/**
		 * Tracks a set of 2D/3D points correspondences from the previous frame to the current frame.
		 * @param pinholeCamera The pinhole camera profile to be used
		 * @param previousFramePyramid The frame pyramid of the previous frame
		 * @param currentFramePyramid The frame pyramid of the current frame
		 * @param previousPose The camera pose for the previous frame
		 * @param previousObjectPoints The 3D object points which have been used to determine the previous pose, and resulting 3D object points which could be tracked
		 * @param previousImagePoints The 2D image points which have been used to determine the previous pose, each points has a corresponding 3D object points, and resulting 2D image points which could be tracked
		 * @param currentImagePoints The resulting 2D image points which have been tracked in the current frame
		 * @param currentPose The resulting current camera pose
		 * @param roughCurrentPose Optional rough current camera pose which can be used to improve the tracking performance
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool trackFrame2Frame(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const HomogenousMatrix4& previousPose, Vectors3& previousObjectPoints, Vectors2& previousImagePoints, Vectors2& currentImagePoints, HomogenousMatrix4& currentPose, const HomogenousMatrix4& roughCurrentPose = HomogenousMatrix4(false), Worker* worker = nullptr);

		/**
		 * Optimizes a given camera pose by rectification of the current camera frame so that it matches with the original tracking pattern.
		 * @param pinholeCamera The pinhole camera profile to be used
		 * @param currentFramePyramid The frame pyramid of the current camera frame, with pixel format FORMAT_Y8
		 * @param roughPose The rough camera pose which will be optimized
		 * @param pattern The tracking pattern object which is needs to be re-found (re-tracked) in the current camera frame
		 * @param optimizedPose Resulting optimized camera pose
		 * @param worker Optional worker object to distribute the computation
		 * @param occlusionArray Optional resulting occlusion array specifying which parts of the rectified camera frame matched with the tracking pattern and which parts are expected to be occluded by different objects
		 * @return True, if the given rough pose could be optimized
		 */
		static bool optimizePoseByRectification(const PinholeCamera& pinholeCamera, const CV::FramePyramid& currentFramePyramid, const HomogenousMatrix4& roughPose, const Pattern& pattern, HomogenousMatrix4& optimizedPose, Worker* worker = nullptr, Geometry::SpatialDistribution::OccupancyArray* occlusionArray = nullptr);

		/**
		 * Returns a sub region based on a set of given 2D triangles.
		 * Due to numerical instability, triangles may be invalid in extreme tracking situations (especially if 32 bit floating point values are used).<br>
		 * Thus, this function uses a backup sub region which is used whenever one triangle is invalid.<br>
		 * The backup sub region is defined by width and height, with origin at (0, 0).
		 * @param triangles The triangles defining the sub region, at least one triangle
		 * @param backupWidth The width of the backup sub region, with range (0, infinity)
		 * @param backupHeight The height of the backup sub region, with range (0, infinity)
		 * @return The resulting sub region
		 */
		static CV::SubRegion triangles2subRegion(const Triangles2& triangles, const unsigned int backupWidth, const unsigned int backupHeight);

		/**
		 * Reads blob features from a bitstream using format version 1.
		 * Version 1 stores 64 floating point values with 64 bit precision.
		 * @param bitstream The input bitstream from which the information will be read
		 * @param features The resulting features received from the bitstream
		 * @return True, if succeeded
		 */
		static bool readFeatures_V1(IO::InputBitstream& bitstream, CV::Detector::Blob::BlobFeatures& features);

		/**
		 * Reads blob features from a bitstream using format version 2.
		 * Version 2 stores 64 floating point values with 32 bit precision and now also stores the orientation type.
		 * @param bitstream The input bitstream from which the information will be read
		 * @param features The resulting features received from the bitstream
		 * @return True, if succeeded
		 */
		static bool readFeatures_V2(IO::InputBitstream& bitstream, CV::Detector::Blob::BlobFeatures& features);

		/**
		 * Reads blob features from a bitstream using format version 3.
		 * Version 3 stores 36 floating point values with 32 bit precision and stores the orientation type.
		 * @param bitstream The input bitstream from which the information will be read
		 * @param features The resulting features received from the bitstream
		 * @return True, if succeeded
		 */
		static bool readFeatures_V3(IO::InputBitstream& bitstream, CV::Detector::Blob::BlobFeatures& features);

		/**
		 * Reads blob features from a bitstream using format version 4.
		 * Version 4 stores 36 floating point values with 8 bit precision and otherwise only stores feature (x, y) observations.
		 * @param bitstream The input bitstream from which the information will be read
		 * @param features The resulting features received from the bitstream
		 * @return True, if succeeded
		 */
		static bool readFeatures_V4(IO::InputBitstream& bitstream, CV::Detector::Blob::BlobFeatures& features);

		/**
		 * @return The unique tag for the features.
		 */
		static const IO::Tag& trackerTagFeatures();

		/**
		 * @return The unique tag for the feature maps.
		 */
		static const IO::Tag& trackerTagFeatureMap();

	protected:

		/// Set of options for this tracker.
		Options options_;

		/// Frame pyramid of the current tracking frame.
		CV::FramePyramid trackerCurrentFramePyramid;

		/// Frame pyramid of the previous tracking frame.
		CV::FramePyramid trackerPreviousFramePyramid;

		/// The map holding all registered pattern object.
		PatternMap trackerPatternMap;

		/// Optional absolute orientation for the previous camera frame (as provided from outside this tracker, e.g., via an IMU sensor).
		Quaternion world_R_previousCamera;

		/// Random generator object.
		RandomGenerator trackerRandomGenerator;

		/// A counter providing unique pattern ids.
		unsigned int trackerPatternMapIdCounter;

		/// Tracker lock object.
		mutable Lock trackerLock;

		/// Integral image for the most recent frame (used to avoid frame buffer re-allocations).
		Frame trackerIntegralImage;

		/// The timestamp of the previous frame.
		Timestamp trackerTimestampPreviousFrame;

		/// The last timestamp at which we started an attempt to recognize a new pattern (this timestamp is prior to feature extraction).
		Timestamp lastRecognitionAttemptTimestamp_;

		/// The id of the pattern that has been tried to recognized last.
		unsigned int trackerLastRecognitionPatternId;
};

inline PatternTrackerCore6DOF::Options::Options() noexcept
{
	// nothing to do here
}

inline PatternTrackerCore6DOF::Pattern::Pattern() :
	patternDimension(0, 0),
	patternPreviousPose(false),
	patternPoseGuess(false)
{
	// nothing to do here
}

inline const Blob::FeatureMap& PatternTrackerCore6DOF::Pattern::featureMap() const
{
	return patternFeatureMap;
}

inline const CV::FramePyramid& PatternTrackerCore6DOF::Pattern::pyramid() const
{
	return patternPyramid;
}

inline const Vector2& PatternTrackerCore6DOF::Pattern::dimension() const
{
	return patternDimension;
}

inline Vector3 PatternTrackerCore6DOF::Pattern::corner0() const
{
	ocean_assert(isValid());
	return Vector3(0, 0, 0);
}

inline Vector3 PatternTrackerCore6DOF::Pattern::corner1() const
{
	ocean_assert(isValid());
	return Vector3(0, 0, patternDimension.y());
}

inline Vector3 PatternTrackerCore6DOF::Pattern::corner2() const
{
	ocean_assert(isValid());
	return Vector3(patternDimension.x(), 0, patternDimension.y());
}

inline Vector3 PatternTrackerCore6DOF::Pattern::corner3() const
{
	ocean_assert(isValid());
	return Vector3(patternDimension.x(), 0, 0);
}

inline Triangles3 PatternTrackerCore6DOF::Pattern::triangles3() const
{
	Triangles3 result(2);
	result[0] = Triangle3(corner0(), corner1(), corner2());
	result[1] = Triangle3(corner0(), corner2(), corner3());

	return result;
}

inline Triangles2 PatternTrackerCore6DOF::Pattern::triangles2(const PinholeCamera& pinholeCamera) const
{
	ocean_assert(patternPreviousPose.isValid());

	return triangles2(pinholeCamera, patternPreviousPose);
}

inline Triangles2 PatternTrackerCore6DOF::Pattern::triangles2(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose) const
{
	ocean_assert(pose.isValid());

	Triangles2 triangles;

	if (isPlanar())
	{
		triangles =
		{
			pinholeCamera.projectToImage<true>(pose, Triangle3(corner0(), corner1(), corner2()), pinholeCamera.hasDistortionParameters()),
			pinholeCamera.projectToImage<true>(pose, Triangle3(corner0(), corner2(), corner3()), pinholeCamera.hasDistortionParameters())
		};
	}
	else if (isCylindrical() || isConical())
	{
		ocean_assert(patternFeatureMap.cylinder().isValid() || patternFeatureMap.cone().isValid());

		// Project an approximation of the cone's surface that has been precomputed in the feature map.
		const Triangles3& triangles3D = patternFeatureMap.triangles3();
		triangles.reserve(triangles3D.size() / 2 + 1);

		for (const Triangle3& triangle3D : triangles3D)
		{
			const Triangle2 triangle2D = pinholeCamera.projectToImage<true>(pose, triangle3D, pinholeCamera.hasDistortionParameters());

			// Check that the triangle is front-facing: Assuming the triangle's normal faces outward from
			// the surface of the object, a visible triangle will have a normal that points towards the
			// camera. After projection, this still holds -- if we take the cross product n of the two
			// triangle legs in the z=0 plane, a visible triangle will have n.z < 0.
			const Vector2 segment10 = triangle2D.point0() - triangle2D.point1();
			const Vector2 segment12 = triangle2D.point2() - triangle2D.point1();
			if (segment12.x() * segment10.y() - segment12.y() * segment10.x() < Scalar(0.))
			{
				triangles.push_back(triangle2D);
			}
		}
	}
	else
	{
		ocean_assert(false && "Not supported!");
	}

	return triangles;
}

inline const HomogenousMatrix4& PatternTrackerCore6DOF::Pattern::previousPose() const
{
	return patternPreviousPose;
}

inline HomogenousMatrix4& PatternTrackerCore6DOF::Pattern::previousPose()
{
	return patternPreviousPose;
}

inline const Vectors3& PatternTrackerCore6DOF::Pattern::objectPoints() const
{
	return patternObjectPoints;
}

inline Vectors3& PatternTrackerCore6DOF::Pattern::objectPoints()
{
	return patternObjectPoints;
}

inline const Vectors2& PatternTrackerCore6DOF::Pattern::imagePoints() const
{
	return patternImagePoints;
}

inline Vectors2& PatternTrackerCore6DOF::Pattern::imagePoints()
{
	return patternImagePoints;
}

inline const Vectors2& PatternTrackerCore6DOF::Pattern::referencePoints(const unsigned int layer) const
{
	ocean_assert(layer < patternPyramid.layers() && layer < patternPyramidReferencePoints.size());
	return patternPyramidReferencePoints[layer];
}

inline unsigned int PatternTrackerCore6DOF::Pattern::layers() const
{
	return (unsigned int)patternPyramidReferencePoints.size();
}

inline bool PatternTrackerCore6DOF::Pattern::hasRepresentativeFeatures() const
{
	return patternRepresentativeFeatures.size() >= 7;
}

inline bool PatternTrackerCore6DOF::Pattern::hasPoseGuess(HomogenousMatrix4& poseGuess, const double maximalAge)
{
	ocean_assert(maximalAge >= 0.0 && maximalAge <= 2.0);

	if (patternPoseGuess.isValid() && NumericD::abs(double(Timestamp(true) - patternPoseGuessTimestamp)) <= maximalAge)
	{
		poseGuess = patternPoseGuess;
		return true;
	}

	return false;
}

inline const HomogenousMatrix4& PatternTrackerCore6DOF::Pattern::poseGuess(Timestamp* timestamp)
{
	if (timestamp)
	{
		*timestamp = patternPoseGuessTimestamp;
	}

	return patternPoseGuess;
}

inline void PatternTrackerCore6DOF::Pattern::setPoseGuess(const HomogenousMatrix4& pose, const Timestamp& timestamp)
{
	patternPoseGuess = pose;
	patternPoseGuessTimestamp = timestamp;
}

inline bool PatternTrackerCore6DOF::Pattern::isValid() const
{
	return patternPyramid.isValid();
}

inline bool PatternTrackerCore6DOF::Pattern::isPlanar() const
{
	return patternFeatureMap.isPlanar();
}

inline bool PatternTrackerCore6DOF::Pattern::isCylindrical() const
{
	return patternFeatureMap.isCylindrical();
}

inline bool PatternTrackerCore6DOF::Pattern::isConical() const
{
	return patternFeatureMap.isConical();
}

inline PatternTrackerCore6DOF::Pattern::operator bool() const
{
	return isValid();
}

inline bool PatternTrackerCore6DOF::determinePoses(const uint8_t* yFrame, const PinholeCamera& pinholeCamera, const unsigned int yFramePaddingElements, const bool frameIsUndistorted, const Timestamp& timestamp, VisualTracker::TransformationSamples& transformations, const Quaternion& world_R_camera, Worker* worker)
{
	return determinePoses(/* allowRecognition */ true, yFrame, pinholeCamera, yFramePaddingElements, frameIsUndistorted, timestamp, transformations, world_R_camera, worker);
}

inline unsigned int PatternTrackerCore6DOF::numberPattern() const
{
	const ScopedLock scopedLock(trackerLock);

	return (unsigned int)(trackerPatternMap.size());
}

inline unsigned int PatternTrackerCore6DOF::maxConcurrentlyVisiblePattern() const
{
	const ScopedLock scopedLock(trackerLock);

	return options_.maxConcurrentlyVisiblePattern;
}

inline void PatternTrackerCore6DOF::setMaxConcurrentlyVisiblePattern(const unsigned int maxConcurrentlyVisiblePattern)
{
	const ScopedLock scopedLock(trackerLock);

	options_.maxConcurrentlyVisiblePattern = maxConcurrentlyVisiblePattern;
}

inline double PatternTrackerCore6DOF::maximumDurationBetweenRecognitionAttempts() const
{
	return (internalNumberVisiblePattern() == 0) ? options_.recognitionCadenceWithoutTrackedPatterns : options_.recognitionCadenceWithTrackedPatterns;
}

inline unsigned int PatternTrackerCore6DOF::internalNumberVisiblePattern() const
{
	unsigned int number = 0u;

	for (PatternMap::const_iterator i = trackerPatternMap.begin(); i != trackerPatternMap.end(); ++i)
	{
		if (i->second.previousPose().isValid())
		{
			number++;
		}
	}

	return number;
}

inline unsigned int PatternTrackerCore6DOF::internalMaxConcurrentlyVisiblePattern() const
{
	if (options_.maxConcurrentlyVisiblePattern == 0u)
	{
		return (unsigned int)trackerPatternMap.size();
	}
	else
	{
		return min(options_.maxConcurrentlyVisiblePattern, (unsigned int)trackerPatternMap.size());
	}
}

}

}

}

#endif // META_OCEAN_TRACKING_PATTERN_PATTERN_TRACKER_CORE_6DOF_H
