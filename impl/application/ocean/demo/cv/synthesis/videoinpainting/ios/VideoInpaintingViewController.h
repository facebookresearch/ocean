// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/synthesis/videoinpainting/ios/VideoInpaintingOSX.h"
#include "application/ocean/demo/cv/synthesis/videoinpainting/ios/OpenGLViewController.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/cv/advanced/HomographyTracker.h"

#include "ocean/cv/synthesis/MappingF1.h"

#include "ocean/math/PinholeCamera.h"

/**
 * The ViewController implementing the actual functionality of the Video Inpainting demo application.
 * @ingroup applicationdemocvsynthesisvideoinpaintingios
 */
@interface VideoInpaintingViewController : OpenGLViewController

@end

/**
 * This class implemenets simple helper functions.
 * **TODO** **JH** This class needs a significant reengineering.<br>
 * **DOCUMENT** **JH** This class needs to be documented.
 * applicationdemocvsynthesisvideoinpaintingios
 */
class VideoInpainting : public Singleton<VideoInpainting>
{
	friend class Singleton<VideoInpainting>;

	protected:

		/**
		 * **TODO** use template to support individual channels
		 */
		class PositionDelta : public Ocean::Vector2
		{
			public:
				/**
				 * Constructor
				 * @param position TODO
				 * @param value0 TODO
				 * @param value1 TODO
				 * @param value2 TODO
				 */
				inline PositionDelta(const Ocean::Vector2& position, const Ocean::Scalar value0, const Ocean::Scalar value1, const Ocean::Scalar value2);

				/**
				 * Constructor
				 * @param position TODO
				 * @param valuesA TODO
				 * @param valuesB TODO
				 */
				inline PositionDelta(const Ocean::Vector2& position, const unsigned char* valuesA, const unsigned char* valuesB);

				/**
				 * Constructor
				 * @param deltaA TODO
				 * @param deltaB TODO
				 */
				inline PositionDelta(const PositionDelta& deltaA, const PositionDelta& deltaB);

			public:

				Ocean::Scalar delta0;
				Ocean::Scalar delta1;
				Ocean::Scalar delta2;
		};

		typedef std::vector<PositionDelta> PositionDeltas;

	public:

		bool onFrame(const LegacyFrame& inputFrame, LegacyFrame& outputFrame);

		void defineMask(const LegacyFrame& yFrame, LegacyFrame& yuvFrame, const CV::PixelPosition newMaskPoint, Worker* worker);

		void inpaint(const LegacyFrame& yFrame, LegacyFrame& yuvFrame, const CV::PixelPosition newMaskPoint, Worker* worker);

		void addMaskPoint(const Vector2& point);

		void reset();

		void startRemove();

		static Vector2 view2video(const double viewWidth, const double viewHeight, const Scalar viewFovX, const PinholeCamera& videoCamera, const Vector2& point);

	protected:

		VideoInpainting();

		void createReferenceFrame(const LegacyFrame& yuvFrame, const Ocean::CV::PixelBoundingBox& trackingMaskBoundingBox, const CV::PixelPositions& contourPoints, Worker* worker);

	protected:

		CV::PixelPosition inpaintingNewMaskPoint;

		Vectors2 inpaintingHomographyContourPoints;

		CV::Advanced::HomographyTracker inpaintingHomographyTracker;

		RandomGenerator inpaintingRandomGenerator;

		LegacyFrame inpaintingTrackingMask;

		bool inpaintingRemoving;

		SquareMatrix3 inpaintingTotalHomography;

		LegacyFrame inpaintingInpaintFrame;

		LegacyFrame inpaintingInpaintFrame_4;

		LegacyFrame inpaintingReferenceFrame;

		CV::Synthesis::MappingF1 inpaintingPreviousMapping;

		LegacyFrame inpaintingYUVFrame_4;

		Lock inpaintingLock;

		Lock inpaintingMaskPointLock;
};

/// \cond DOXYGEN_DO_NOT_DOCUMENT

VideoInpainting::PositionDelta::PositionDelta(const Ocean::Vector2& position, const Ocean::Scalar value0, const Ocean::Scalar value1, const Ocean::Scalar value2) :
	Ocean::Vector2(position),
	delta0(value0),
	delta1(value1),
	delta2(value2)
{
	// nothing to do here
}

VideoInpainting::PositionDelta::PositionDelta(const Ocean::Vector2& position, const unsigned char* valuesA, const unsigned char* valuesB) :
	Ocean::Vector2(position),
	delta0(Ocean::Scalar(int(valuesB[0]) - int(valuesA[0]))),
	delta1(Ocean::Scalar(int(valuesB[1]) - int(valuesA[1]))),
	delta2(Ocean::Scalar(int(valuesB[2]) - int(valuesA[2])))
{
	// nothing to do here
}

VideoInpainting::PositionDelta::PositionDelta(const PositionDelta& positionA, const PositionDelta& positionB) :
	Ocean::Vector2((positionA + positionB) * Ocean::Scalar(0.5)),
	delta0((positionA.delta0 + positionB.delta0) * Ocean::Scalar(0.5)),
	delta1((positionA.delta1 + positionB.delta1) * Ocean::Scalar(0.5)),
	delta2((positionA.delta2 + positionB.delta2) * Ocean::Scalar(0.5))
{
	// nothing to do here
}

/// \endcond
