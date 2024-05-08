/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMEDIA_TEST_MOVIE_H
#define META_OCEAN_TEST_TESTMEDIA_TEST_MOVIE_H

#include "ocean/test/testmedia/TestMedia.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/io/Directory.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

/**
 * This class implements a test for Movie objects.
 * @ingroup testmedia
 */
class OCEAN_TEST_MEDIA_EXPORT TestMovie
{
	protected:

		/// The number of bits to be encoded in each movie.
		static constexpr unsigned int numberBits_ = 16u;

		/// The size of the bits in pixel.
		static constexpr unsigned int bitSize_ = 15u;

		/// The vertical location of the color strip.
		static constexpr unsigned int yColorStrip_ = 200u;

		/**
		 * This class implements a verifier for movies.
		 */
		class OCEAN_TEST_MEDIA_EXPORT MovieVerifier
		{
			public:

				/**
				 * Creates a new verifier object.
				 * @param expectedWidth The expected width of the movie, in pixel, with range [1, infinity)
				 * @param expectedHeight The expected height of the movie, in pixel, with range [1, infinity)
				 * @param expectedNumberFrames The expected number of frames the movie has, with range [1, infinity)
				 * @param expectedFps The expected number of frames per seconds the movie has, with range (0, infinity)
				 */
				MovieVerifier(const unsigned int expectedWidth, const unsigned int expectedHeight, const unsigned int expectedNumberFrames, const double expectedFps);

				/**
				 * Event function for a new movie frame.
				 * @param frame The new movie frame, will be valid
				 * @param camera The camera profile associated with the frame, invalid if unknown
				 */
				void onFrame(const Frame& frame, const SharedAnyCamera& camera);

				/**
				 * Returns the timestamp when this verifier has been updated the last time.
				 * @return The verifier's timestamp
				 */
				Timestamp lastUpdateTimestamp() const;

				/**
				 * Returns whether the verifier has verified all frames.
				 * @return True, if so
				 */
				bool succeeded() const;

				/**
				 * Parses the frame index encoded in the frame.
				 * @param rgbFrame The frame to parse, must be valid, must have pixel format FORMAT_RGB24
				 * @param frameIndex The resulting parsed frame index, with range [0, infinity)
				 * @return True, if succeeded
				 */
				static bool parseFrameIndex(const Frame& rgbFrame, unsigned int& frameIndex);

				/**
				 * Verifies the color of the color stripe in the frame.
				 * @param rgbFrame The frame to verify, must be valid, must have pixel format FORMAT_RGB24
				 * @param frameIndex The frame index for which the color will be verified, with range [0, infinity)
				 * @return True, if succeeded
				 */
				static bool verifyColor(const Frame& rgbFrame, const unsigned int frameIndex);

			protected:

				/// The expected width of the movie, in pixel, with range [1, infinity).
				const unsigned int expectedWidth_ = 0u;

				/// The expected height of the movie, in pixel, with range [1, infinity).
				const unsigned int expectedHeight_ = 0u;

				/// The expected number of frames the movie has, with range [1, infinity).
				const unsigned int expectedNumberFrames_ = 0u;

				/// The expected number of frames per seconds the movie has, with range (0, infinity).
				const double expectedFps_ = 0.0;

				/// The number of received frames.
				unsigned int numberReceivedFrames_ = 0u;

				/// The timestamp this verifier was updated the last time.
				Timestamp lastUpdateTimestamp_;

				/// True, if the verifier has seen an error.
				bool hasError_ = false;

				/// The verifier's lock.
				mutable Lock lock_;
		};

	public:

		/**
		 * Invokes all tests that are defined.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the encoder and decoder of movies.
		 * @return True, if succeeded
		 */
		static bool testEncodeDecode();

		/**
		 * Tests the loop functionality of movies.
		 * @return True, if succeeded
		 */
		static bool testLoop();

		/**
		 * Tests the pause functionality of movies.
		 * @return True, if succeeded
		 */
		static bool testPause();

		/**
		 * Registers all necessary media libraries.
		 */
		static void registerMediaLibraries();

		/**
		 * Unregisters all media libraries.
		 */
		static void unregisterMediaLibraries();

	protected:

		/**
		 * Returns the names of media libraries support movie encoding.
		 * @return The names of all available libraries
		 */
		static std::vector<std::string> libraryNamesEncoder();

		/**
		 * Returns the names of media libraries support movie decoding.
		 * @return The names all available libraries
		 */
		static std::vector<std::string> libraryNamesDecoder();

		/**
		 * Creates a movie and writes it to a file.
		 * @param directory The directory in which the movie will be written, must exist
		 * @param width The width of the movie, in pixel, with range [1, infinity)
		 * @param height The height of the movie, in pixel, with range [1, infinity)
		 * @param numberFrames The number of frames the movie will have, with range [1, infinity)
		 * @param fps The frames per second the movie will have, with range (0, infinity)
		 * @param libraryName The name of the library to be used, empty to use any
		 * @return The file of the movie, if succeeded
		 */
		static IO::File writeMovie(const IO::Directory& directory, const unsigned int width, const unsigned int height, const unsigned int numberFrames, const double fps, const std::string& libraryName = std::string());

		/**
		 * Reads a movie from a file and verifies whether the movie has the correct properties.
		 * @param file The file to the movie, must exist
		 * @param width The expected width of the movie, in pixel, with range [1, infinity)
		 * @param height The expected height of the movie, in pixel, with range [1, infinity)
		 * @param numberFrames The expected number of frames the movie has, with range [1, infinity)
		 * @param fps The expected frames per second the movie has, with range (0, infinity)
		 * @param libraryName The name of the library to be used, empty to use any
		 * @return True, if the movie could be read and if the properties are as expected
		 */
		static bool readMovie(const IO::File& file, const unsigned int width, const unsigned int height, const unsigned int numberFrames, const double fps, const std::string& libraryName = std::string());

		/**
		 * Returns a unique id.
		 * @return The unique id
		 */
		static unsigned int uniqueId();

		/**
		 * Returns a unique RGB24 color for a frame index.
		 * @param frameIndex The index of the frame for which the color will be returned, with range [0, infinity)
		 * @return The unique color
		 */
		static const uint8_t* uniqueColor(const unsigned int frameIndex);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMEDIA_TEST_MOVIE_H
