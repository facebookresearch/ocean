// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "application/ocean/demo/cv/detector/qrcodes/ApplicationDemoCVDetectorQRCodes.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Worker.h"

#include "ocean/io/File.h"

/**
 * @ingroup applicationdemocvdetectorqrcodes
 * @defgroup applicationdemocvdetectorqrcodesdetectorrecordingprocessor QR Codes, Recording Processor
 * @{
 * The demo application implements a processor for VRS recordings.
 * @}
 */

using namespace Ocean;

/**
 * Processes a subset of all VRS files.
 * @param vrsFiles The VRS files to process, must be valid
 * @param outputFiles The output files, one for each VRS file
 * @param framesPerSeconds The number of frames that the output video should be encoded with, range: [1, infinity)
 * @param justVideo If true it will create just a video of the recording, the detector is not executed
 * @param useOldDetector If true it will use the old QR code detector, otherwise the current one will be used
 * @param worker Optional worker to distribute the computation
 * @return The number of successful recordings
 * @ingroup applicationdemocvdetectorqrcodesdetectorrecordingprocessor
 */
static size_t processRecordings(const IO::Files& vrsFiles, const IO::Files& outputFiles, const unsigned int framesPerSecond, const bool justVideo, const bool useOldDetector, Worker* worker);

/**
 * Processes a subset of all VRS files.
 * @param vrsFiles The VRS files to process, must be valid
 * @param outputFiles The output files, one for each VRS file
 * @param framesPerSeconds The number of frames that the output video should be encoded with, range: [1, infinity)
 * @param justVideo If true it will create just a video of the recording, the detector is not executed
 * @param useOldDetector If true it will use the old QR code detector, otherwise the current one will be used
 * @param index The current index, with range [0, size)
 * @param successful The resulting number of successful recordings, with range [0, size]
 * @param size The number of VRS files, with range [1, infinity)
 * @param lock Lock object to access the current index, must be valid
 * @param firstIndex Unused first index
 * @param numberIndices Unused number of indices
 * @ingroup applicationdemocvdetectorqrcodesdetectorrecordingprocessor
 */
static void processRecordingsSubset(const IO::File* vrsFiles, const IO::File* outputFiles, const unsigned int framesPerSecond, const bool justVideo, const bool useOldDetector, size_t* index, size_t* successful, const size_t size, Lock* lock, const unsigned int firstIndex, const unsigned int numberIndices);

/**
 * Processes a single VRS recording.
 * @param vrsFile The VRS file to process, must be valid
 * @param outputFile The output file
 * @param framesPerSeconds The number of frames that the output video should be encoded with, range: [1, infinity)
 * @param justVideo If true it will create just a video of the recording, the detector is not executed
 * @param useOldDetector If true it will use the old QR code detector, otherwise the current one will be used
 * @return True, if succeeded
 * @ingroup applicationdemocvdetectorqrcodesdetectorrecordingprocessor
 */
static bool processRecording(const IO::File& vrsFile, const IO::File& outputFile, const unsigned int framesPerSecond, const bool justVideo, const bool useOldDetector);
