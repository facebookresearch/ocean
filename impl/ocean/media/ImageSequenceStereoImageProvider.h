/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGE_SEQUENCE_STEREO_IMAGE_PROVIDER_H
#define META_OCEAN_MEDIA_IMAGE_SEQUENCE_STEREO_IMAGE_PROVIDER_H

#include "ocean/media/Media.h"
#include "ocean/media/ImageSequence.h"
#include "ocean/media/StereoImageProviderInterface.h"

#include "ocean/math/SampleMap.h"

namespace Ocean
{

namespace Media
{

/**
 * This class implements a provider for stereo images which are extracted from two separate image sequences and poses/calibration file.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT ImageSequenceStereoImageProvider : public StereoImageProviderInterface
{
	public:

		/**
		 * Definition of a sample map holding 6-DOF transformations.
		 */
		typedef SampleMap<HomogenousMatrixD4> TransformationMap;

	public:

		/**
		 * Creates an invalid provider object.
		 */
		ImageSequenceStereoImageProvider();

		/**
		 * Move constructor.
		 * @param provider The provider to be moved
		 */
		ImageSequenceStereoImageProvider(ImageSequenceStereoImageProvider&& provider);

		/**
		 * Creates a provider object for two image sequences and a poses/calibration file.
		 * @param imageSequenceFilenameA The filename of the first image of the first image sequence (the image sequence of the first fisheye camera), must be valid
		 * @param imageSequenceFilenameB The filename of the first image of the second image sequence (the image sequence of the second fisheye camera), must be valid
		 * @param transformationsFilename The filename of the file containing the camera calibration of both fisheye cameras followed by the individual camera transformations
		 * @param framesPerSecond The number of frames per second, with range (0, infinity)
		 */
		ImageSequenceStereoImageProvider(const std::string& imageSequenceFilenameA, const std::string& imageSequenceFilenameB, const std::string& transformationsFilename, const double framesPerSecond = 30.0);

		/**
		 * Returns the next stereo images from the provider, further optional transformations between device and world, and between cameras and device will be provided.
		 * @see StereoImageProviderInterface::nextStereoImages(), hasDeviceTransformations(), hasCameraTransformations().
		 */
		bool nextStereoImages(FisheyeCamera& fisheyeCameraA, FisheyeCamera& fisheyeCameraB, Frame& imageA, Frame& imageB, HomogenousMatrixD4* world_T_device = nullptr, HomogenousMatrixD4* device_T_cameraA = nullptr, HomogenousMatrixD4* device_T_cameraB = nullptr, HomogenousMatricesD4* world_T_leftHandJoints = nullptr, HomogenousMatricesD4* world_T_rightHandJoints = nullptr, bool* abort = nullptr) override;

		/**
		 * Restarts with the first pair of stereo images.
		 * @return True, if succeeded
		 */
		bool restart() override;

		/**
		 * Returns whether the provider has access to the transformations between device and world (world_T_device).
		 * @return True, if so
		 */
		bool hasDeviceTransformations() const override;

		/**
		 * Returns whether the provider has access to the transformations between cameras and device (device_T_cameraA and device_T_cameraB).
		 * @return True, if so
		 */
		bool hasCameraTransformations() const override;

		/**
		 * Returns whether this provider can provide valid stereo images (e.g., whether the underlying resources are initialized and whether the source exists).
		 * @return True, if so
		 */
		bool isValid() const override;

		/**
		 * Releases all owned resources like e.g., the image sequence objects.
		 */
		void release() override;

		/**
		 * Move operator.
		 * @param provider The provider to be moved
		 * @return Reference to this object
		 */
		ImageSequenceStereoImageProvider& operator=(ImageSequenceStereoImageProvider&& provider);

		/**
		 * Extracts the camera calibrations, the device transformations, and the camera transformations from a transformations file.
		 *
		 * The expected format can be outline as follows:
		 *
		 * ```
		 * intrinsics of camera A
		 * intrinsics of camera B
		 * frame 1: world_T_device device_T_cameraA device_T_cameraB
		 * frame 2: world_T_device device_T_cameraA device_T_cameraB
		 * frame 3: world_T_device device_T_cameraA device_T_cameraB
		 * ...
		 * ```
		 *
		 * or, more verbosely,
		 *
		 * ```
		 * widthA heightA focalLengthXA focalLengthYA principalPointXA principalPointYA radialDistortionA[0] radialDistortionA[1] radialDistortionA[2] radialDistortionA[3] radialDistortionA[4] radialDistortionA[5] tangentialDistortionA[0] tangentialDistortionA[1]
		 * widthB heightB focalLengthXB focalLengthYB principalPointXB principalPointYB radialDistortionB[0] radialDistortionB[1] radialDistortionB[2] radialDistortionB[3] radialDistortionB[4] radialDistortionB[5] tangentialDistortionB[0] tangentialDistortionB[1]
		 * [wTd0] [wTd1] [wTd2] [wTd3] [wTd4] [wTd5] [wTd6] [wTd7] [wTd8] [wTd9] [wTd10] [wTd11] [wTd12] [wTd13] [wTd14] [wTd15] [dTa0] [dTa1] [dTa2] [dTa3] [dTa4] [dTa5] [dTa6] [dTa7] [dTa8] [dTa9] [dTa10] [dTa11] [dTa12] [dTa13] [dTa14] [dTa15] [dTb0] [dTb1] [dTb2] [dTb3] [dTb4] [dTb5] [dTb6] [dTb7] [dTb8] [dTb9] [dTb10] [dTb11] [dTb12] [dTb13] [dTb14] [dTb15]
		 * [wTd0] [wTd1] [wTd2] [wTd3] [wTd4] [wTd5] [wTd6] [wTd7] [wTd8] [wTd9] [wTd10] [wTd11] [wTd12] [wTd13] [wTd14] [wTd15] [dTa0] [dTa1] [dTa2] [dTa3] [dTa4] [dTa5] [dTa6] [dTa7] [dTa8] [dTa9] [dTa10] [dTa11] [dTa12] [dTa13] [dTa14] [dTa15] [dTb0] [dTb1] [dTb2] [dTb3] [dTb4] [dTb5] [dTb6] [dTb7] [dTb8] [dTb9] [dTb10] [dTb11] [dTb12] [dTb13] [dTb14] [dTb15]
		 * [wTd0] [wTd1] [wTd2] [wTd3] [wTd4] [wTd5] [wTd6] [wTd7] [wTd8] [wTd9] [wTd10] [wTd11] [wTd12] [wTd13] [wTd14] [wTd15] [dTa0] [dTa1] [dTa2] [dTa3] [dTa4] [dTa5] [dTa6] [dTa7] [dTa8] [dTa9] [dTa10] [dTa11] [dTa12] [dTa13] [dTa14] [dTa15] [dTb0] [dTb1] [dTb2] [dTb3] [dTb4] [dTb5] [dTb6] [dTb7] [dTb8] [dTb9] [dTb10] [dTb11] [dTb12] [dTb13] [dTb14] [dTb15]
		 * ...
		 * ```
		 *
		 * where the first two rows contain the intrinsics of the two fisheye cameras, and starting with the third row each row
		 * stores three homogeneous 4-by-4 matrices which correspond to one frame. The matrix elements of each transformation are
		 * stored in column-major order, i.e.,
		 *
		 * ```
		 * m = [m0 m4 m8  m12]
		 *     [m1 m5 m9  m13]
		 *     [m2 m6 m10 m14]
		 *     [m3 m7 m11 m15]
		 * ```
		 *
		 * @param transformationsFilename The filename of the transformations file, must be valid
		 * @param fisheyeCameraA The resulting profile of the first fisheye camera
		 * @param fisheyeCameraB The resulting profile of the second fisheye camera
		 * @param map_world_T_device The resulting sample map transforming device to world
		 * @param map_device_T_cameraA The resulting sample map transforming the first camera to device
		 * @param map_device_T_cameraB The resulting sample map transforming the second camera to device
		 * @param framesPerSecond The number of frames per second, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool readTransformationsFile(const std::string& transformationsFilename, FisheyeCamera& fisheyeCameraA, FisheyeCamera& fisheyeCameraB, TransformationMap& map_world_T_device, TransformationMap& map_device_T_cameraA, TransformationMap& map_device_T_cameraB, const double framesPerSecond = 30.0);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param provider The provider which would have been copied
		 */
		ImageSequenceStereoImageProvider(const ImageSequenceStereoImageProvider& provider) = delete;

		/**
		 * Disabled assign operator.
		 * @param provider The provider which would have been assigned
		 * @return Reference to this object
		 */
		ImageSequenceStereoImageProvider& operator=(const ImageSequenceStereoImageProvider& provider) = delete;

    protected:

        /// The first image sequence providing the images for the first stereo camera.
		ImageSequenceRef imageSequenceA_;

		/// The second image sequence providing the images for the first stereo camera.
		ImageSequenceRef imageSequenceB_;

        /// The fisheye camera profile of the first camera.
		FisheyeCamera fisheyeCameraA_;

		/// The fisheye camera profile of the second camera.
		FisheyeCamera fisheyeCameraB_;

		/// The sample map transforming device to world.
		TransformationMap map_world_T_device_;

		/// The sample map transforming the first camera to device.
		TransformationMap map_device_T_cameraA_;

		/// The sample map transforming the second camera to device.
		TransformationMap map_device_T_cameraB_;

		/// The number of frames per second, with range (0, infinity)
		double framesPerSecond_;
};

}

}

#endif // META_OCEAN_MEDIA_IMAGE_SEQUENCE_STEREO_IMAGE_PROVIDER_H
