/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/HomographyImageAlignmentDense.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameFilterGradient.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/geometry/Homography.h"
#include "ocean/geometry/Jacobian.h"
#include "ocean/geometry/NonLinearOptimization.h"
#include "ocean/geometry/Utilities.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements the base class for the advanced optimization provider.
 * @tparam tChannels The number of color channels of the frames to align, with range [1, 4]
 */
template <unsigned int tChannels>
class HomographyImageAlignmentDense::HomographyProvider : public Geometry::NonLinearOptimization::AdvancedDenseOptimizationProvider
{
	public:

		/**
		 * Definition of a specific consistency data object.
		 */
		class ConsistencyObject : public HomographyImageAlignmentDense::ConsistencyData
		{
			friend class HomographyProvider;

			public:

				/**
				 * Creates a new object.
				 * @owner The owner of this consistency object
				 */
				explicit inline ConsistencyObject(HomographyProvider<tChannels>& owner);

				/**
				 * Moves the data from this consistency object to the provider.
				 * @param provider The provider to which the data of this object will be moved
				 */
				virtual void moveToProvider(HomographyProvider<tChannels>& provider);

				/**
				 * Moves the data from the owning provider to this consistency object.
				 * @aram provider The owning provider from which the data will be moved
				 */
				virtual void moveFromProvider(HomographyProvider<tChannels>& provider);

				/**
				 * Returns whether this object currently does not hold any consistency information.
				 * @see ConsistencyData::isEmpty().
				 */
				bool isEmpty() override;

			protected:

				/// The mean value for the template frame.
				Scalar templateFrameMean_[tChannels];

#ifdef OCEAN_DEBUG
				/// The pointer to the template frame.
				const Frame* debugTemplateFrame_ = nullptr;
#endif
		};

	public:

		/**
		 * Creates a new provider object by the given parameters and moves some consistency data from the object, if provided.
		 * @param homography The already rough homography to be optimized, transforming points defined in the template frame to points defined in the current frame
		 * @param templateFrame The template frame representing the ground truth data
		 * @param templateSubRegion The sub-region within the template frame specifying the planar area of interest
		 * @param currentFrame The current frame for which the homography needs to be optimized
		 * @param homographyParameters The number of parameters used to define the homography, with range [8, 9]
		 * @param zeroMean True, to subtract the color intensities from the corresponding mean intensity before determining the error; False, to determine the error directly
		 * @param abort Optional abort statement allowing to abort the frame type request at any time; set the value True to abort the request
		 */
		inline HomographyProvider(SquareMatrix3& homography, const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const unsigned int homographyParameters, const bool zeroMean, bool* abort = nullptr);

		/**
		 * Determines the error for the current model candidate (not the actual model).
		 * @see AdvancedProviderBase::determineError().
		 */
		inline Scalar determineError();

		/**
		 * Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedProviderBase::acceptCorrection().
		 */
		inline void acceptCorrection();

		/**
		 * Returns whether the optimization process should stop e.g., due to an external event.
		 * @see AdvancedProviderBase::shouldStop().
		 */
		inline bool shouldStop();

	protected:

		/**
		 * Determines the error for the current model candidate (not the actual model) without respecting the mean color intensities.
		 * @see determineError().
		 */
		template <bool tUseMeans>
		Scalar determineError();

	protected:

		/// The homography to be optimized.
		SquareMatrix3& homography_;

		/// The homography candidate.
		SquareMatrix3 homographyCandidate_;

		/// The template frame.
		const Frame& templateFrame_;

		/// The sub-region defined in the template frame.
		const CV::SubRegion& templateSubRegion_;

		/// The integer-accurate bounding box of the sub-region.
		CV::PixelBoundingBox templateSubRegionBoundingBox_;

		/// The current frame.
		const Frame& currentFrame_;

		/// The homography which has been used to create the transformed current frame (so that a second creation can be skipped).
		SquareMatrix3 usedHomographyForCurrent_;

		/// The re-usable memory/frame so that it can hold the transformed current frame so that it corresponds to the template frame.
		Frame reusableTransformedCurrentFrame_;

		/// The re-usable memory/frame so that it can hold the transformed mask of the current frame.
		Frame reusableTransformedCurrentMask_;

		/// The number of previous iterations which did not change the overall result anymore.
		unsigned int previousIterationsWithSmallImpact_ = 0u;

		/// The mean value for the template frame.
		Scalar templateFrameMean_[tChannels];

		/// True, forcing the provider to determine the error with respect to the individual mean color intensities.
		const bool zeroMean_ = false;

		/// The number of parameters used to define the homography, with range [8, 9].
		const unsigned int homographyParameters_ = 0u;

		/// The optional abort statement to explicitly stop the provider by an extern call.
		bool* abort_ = nullptr;

		///  Optional consistency data object to improve the performance of this provider
		ConsistencyObject* consistencyObject_ = nullptr;
};

template <unsigned int tChannels>
inline HomographyImageAlignmentDense::HomographyProvider<tChannels>::ConsistencyObject::ConsistencyObject(HomographyProvider<tChannels>& owner)
{
#ifdef OCEAN_DEBUG
	debugTemplateFrame_ = &owner.templateFrame_;
#endif

	OCEAN_SUPPRESS_UNUSED_WARNING(owner);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		templateFrameMean_[n] = Numeric::maxValue();
	}

	ocean_assert(isEmpty());
}

template <unsigned int tChannels>
inline void HomographyImageAlignmentDense::HomographyProvider<tChannels>::ConsistencyObject::moveToProvider(HomographyProvider<tChannels>& provider)
{
	ocean_assert(debugTemplateFrame_ == &provider.templateFrame_);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		provider.templateFrameMean_[n] = templateFrameMean_[n];
		templateFrameMean_[n] = Numeric::maxValue();
	}
}

template <unsigned int tChannels>
inline void HomographyImageAlignmentDense::HomographyProvider<tChannels>::ConsistencyObject::moveFromProvider(HomographyProvider<tChannels>& provider)
{
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		templateFrameMean_[n] = provider.templateFrameMean_[n];
	}
}

template <unsigned int tChannels>
bool HomographyImageAlignmentDense::HomographyProvider<tChannels>::ConsistencyObject::isEmpty()
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	return templateFrameMean_[0] == Numeric::maxValue();
}

template <unsigned int tChannels>
inline HomographyImageAlignmentDense::HomographyProvider<tChannels>::HomographyProvider(SquareMatrix3& homography, const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const unsigned int homographyParameters, const bool zeroMean, bool* abort) :
	homography_(homography),
	homographyCandidate_(homography),
	templateFrame_(templateFrame),
	templateSubRegion_(templateSubRegion),
	currentFrame_(currentFrame),
	usedHomographyForCurrent_(false),
	previousIterationsWithSmallImpact_(0u),
	zeroMean_(zeroMean),
	homographyParameters_(homographyParameters),
	abort_(abort),
	consistencyObject_(nullptr)
{
	static_assert(tChannels >= 1u && tChannels <= 4u, "Invalid channel number!");

	ocean_assert(Numeric::isEqual(homography_[8], 1));

	ocean_assert(FrameType::formatIsGeneric(templateFrame_.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, tChannels));
	ocean_assert(templateFrame_.pixelFormat() == currentFrame_.pixelFormat());
	ocean_assert(templateFrame_.pixelOrigin() == currentFrame_.pixelOrigin());
	ocean_assert(templateSubRegion_);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		templateFrameMean_[n] = Numeric::maxValue();
	}

	if (!reusableTransformedCurrentFrame_.set(templateFrame_.frameType(), true /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	if (!reusableTransformedCurrentMask_.set(FrameType(templateFrame_.frameType(), FrameType::FORMAT_Y8), true /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	ocean_assert(templateSubRegion_.boundingBox().isValid());

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!templateSubRegion_.boundingBox().box2integer(templateFrame_.width(), templateFrame_.height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		ocean_assert(false && "This should never happen!");
	}

	templateSubRegionBoundingBox_ = CV::PixelBoundingBox(CV::PixelPosition(boundingBoxLeft, boundingBoxTop), boundingBoxWidth, boundingBoxHeight);

	ocean_assert(reusableTransformedCurrentFrame_.width() >= templateSubRegionBoundingBox_.width() && reusableTransformedCurrentFrame_.height() >= templateSubRegionBoundingBox_.height());
	ocean_assert(reusableTransformedCurrentMask_.width() >= templateSubRegionBoundingBox_.width() && reusableTransformedCurrentMask_.height() >= templateSubRegionBoundingBox_.height());
}

template <unsigned int tChannels>
inline Scalar HomographyImageAlignmentDense::HomographyProvider<tChannels>::determineError()
{
	const Vector2 transformedCorners[] =
	{
		this->homographyCandidate_ * Vector2(Scalar(this->templateSubRegionBoundingBox_.left()), Scalar(this->templateSubRegionBoundingBox_.top())),
		this->homographyCandidate_ * Vector2(Scalar(this->templateSubRegionBoundingBox_.left()), Scalar(this->templateSubRegionBoundingBox_.bottomEnd())),
		this->homographyCandidate_ * Vector2(Scalar(this->templateSubRegionBoundingBox_.rightEnd()), Scalar(this->templateSubRegionBoundingBox_.bottomEnd())),
		this->homographyCandidate_ * Vector2(Scalar(this->templateSubRegionBoundingBox_.rightEnd()), Scalar(this->templateSubRegionBoundingBox_.top()))
	};

	// if the convex bounding box is transformed to a non-convex polygon the homography must be irregular

	if (!Geometry::Utilities::isPolygonConvex(transformedCorners, 4))
	{
		return Numeric::maxValue();
	}

	if (zeroMean_)
	{
		return determineError<true>();
	}
	else
	{
		return determineError<false>();
	}
}

template <unsigned int tChannels>
template <bool tUseMeans>
Scalar HomographyImageAlignmentDense::HomographyProvider<tChannels>::determineError()
{
	ocean_assert(Numeric::isEqual(homographyCandidate_[8], 1));
	ocean_assert(templateSubRegionBoundingBox_);

	if (!templateSubRegionBoundingBox_)
	{
		return Numeric::maxValue();
	}

	// we define a target frame and a target mask for the homography result, we simply use the already existing frame buffer, the frames will have the same dimension as the bounding box of the template frame

	ocean_assert(templateSubRegionBoundingBox_.width() <= reusableTransformedCurrentFrame_.width());
	ocean_assert(templateSubRegionBoundingBox_.height() <= reusableTransformedCurrentFrame_.height());

	Frame transformedCurrentSubFrame = reusableTransformedCurrentFrame_.subFrame(0u, 0u, templateSubRegionBoundingBox_.width(), templateSubRegionBoundingBox_.height(), Frame::CM_USE_KEEP_LAYOUT); // just re-using the memory of reusableTransformedCurrentFrame_
	Frame transformedCurrentSubMask = reusableTransformedCurrentMask_.subFrame(0u, 0u, templateSubRegionBoundingBox_.width(), templateSubRegionBoundingBox_.height(), Frame::CM_USE_KEEP_LAYOUT);

	// we check whether the tracking region leaves the image content so that we have to use a mask telling which pixel is valid and which pixel lies outside
	const bool needCoverageMask = !CV::FrameInterpolatorBilinear::coversHomographyInputFrame(currentFrame_.width(), currentFrame_.height(), transformedCurrentSubFrame.width(), transformedCurrentSubFrame.height(), homographyCandidate_, int(templateSubRegionBoundingBox_.left()), int(templateSubRegionBoundingBox_.top()));

	if (homographyCandidate_ != usedHomographyForCurrent_)
	{
		if (needCoverageMask)
		{
			if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(currentFrame_, transformedCurrentSubFrame, transformedCurrentSubMask, homographyCandidate_, WorkerPool::get().scopedWorker()(), 0xFF, CV::PixelPositionI(int(templateSubRegionBoundingBox_.left()), int(templateSubRegionBoundingBox_.top()))))
			{
				return Numeric::maxValue();
			}
		}
		else
		{
			if (!CV::FrameInterpolatorBilinear::Comfort::homography(currentFrame_, transformedCurrentSubFrame, homographyCandidate_, nullptr, WorkerPool::get().scopedWorker()(), CV::PixelPositionI(int(templateSubRegionBoundingBox_.left()), int(templateSubRegionBoundingBox_.top()))))
			{
				return Numeric::maxValue();
			}
		}
	}

	usedHomographyForCurrent_ = homographyCandidate_;

#ifdef OCEAN_INTENSIVE_DEBUG
	{
		// we ensure that the sub-frame is identical to the sub-region of the entire frame

		Frame transformedCurrentFrameLarge(templateFrame_.frameType());
		Frame transformedCurrentMaskLarge(FrameType(templateFrame_, FrameType::FORMAT_Y8));

		if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(currentFrame_, transformedCurrentFrameLarge, transformedCurrentMaskLarge, homographyCandidate_, WorkerPool::get().scopedWorker()(), 0xFFu))
		{
			ocean_assert(false && "This must never happen!");
			return Numeric::maxValue();
		}

		for (unsigned int y = 0u; y < templateSubRegionBoundingBox_.height(); ++y)
		{
			for (unsigned int x = 0u; x < templateSubRegionBoundingBox_.width(); ++x)
			{
				if (needCoverageMask)
				{
					ocean_assert(*transformedCurrentSubMask.constpixel<uint8_t>(x, y) == *transformedCurrentMaskLarge.constpixel<uint8_t>(x + templateSubRegionBoundingBox_.left(), y + templateSubRegionBoundingBox_.top()));

					ocean_assert(*transformedCurrentSubMask.constpixel<uint8_t>(x, y) == 0x00u || *transformedCurrentSubMask.constpixel<uint8_t>(x, y));
				}

				if (!needCoverageMask || *transformedCurrentSubMask.constpixel<uint8_t>(x, y) == 0xFFu)
				{
					typedef typename DataType<uint8_t, tChannels>::Type PixelType;

					const PixelType* subFramePixel = (const PixelType*)(transformedCurrentSubFrame.constpixel<uint8_t>(x, y));
					const PixelType* largePixel = (const PixelType*)(transformedCurrentFrameLarge.constpixel<uint8_t>(x + templateSubRegionBoundingBox_.left(), y + templateSubRegionBoundingBox_.top()));

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						ocean_assert(std::abs(int(subFramePixel->values_[n]) - int(largePixel->values_[n])) <= 1);
					}
				}
			}
		}
	}
#endif

	Scalar transformedCurrentFrameMean[tChannels];

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		transformedCurrentFrameMean[n] = Numeric::maxValue();
	}
#endif

	if constexpr (tUseMeans)
	{
		if (templateFrameMean_[0] == Numeric::maxValue())
		{
			if (!determineMeans8BitPerChannel<tChannels, true>(templateFrame_, templateSubRegion_, transformedCurrentSubFrame, needCoverageMask ? transformedCurrentSubMask : Frame(), templateFrameMean_, transformedCurrentFrameMean, WorkerPool::get().scopedWorker()()))
			{
				return Numeric::maxValue();
			}
		}
		else
		{
			if (!determineMeans8BitPerChannel<tChannels, false>(templateFrame_, templateSubRegion_, transformedCurrentSubFrame, needCoverageMask ? transformedCurrentSubMask : Frame(), templateFrameMean_, transformedCurrentFrameMean, WorkerPool::get().scopedWorker()()))
			{
				return Numeric::maxValue();
			}
		}
	}

	const Scalar errorValue = determineError8BitPerChannel<tChannels, tUseMeans>(templateFrame_, templateSubRegion_, transformedCurrentSubFrame, needCoverageMask ? transformedCurrentSubMask : Frame(), templateFrameMean_, transformedCurrentFrameMean, WorkerPool::get().scopedWorker()());

#ifdef OCEAN_INTENSIVE_DEBUG
	const Scalar slowError = HomographyImageAlignmentDense::slowDetermineError8BitPerChannel<tChannels>(templateFrame_, templateSubRegion_, currentFrame_, homographyCandidate_, tUseMeans);
	ocean_assert(Numeric::isWeakEqual(errorValue, slowError));
#endif

	return errorValue;
}

template <unsigned int tChannels>
inline void HomographyImageAlignmentDense::HomographyProvider<tChannels>::acceptCorrection()
{
	ocean_assert(Numeric::isEqual(homographyCandidate_[8], 1));
	homography_ = homographyCandidate_;
}

template <unsigned int tChannels>
inline bool HomographyImageAlignmentDense::HomographyProvider<tChannels>::shouldStop()
{
	return previousIterationsWithSmallImpact_ >= 5u || (abort_ != nullptr && *abort_ == true);
}

/**
 * Implements an advanced optimization provider for dense image alignment by an additive delta approach.
 * @tparam tChannels The number of color channels of the frames to align, with range [1, 4]
 */
template <unsigned int tChannels>
class HomographyImageAlignmentDense::DenseAdditiveHomographyProvider : public HomographyProvider<tChannels>
{
	public:

		/**
		 * Creates a new provider object by the given parameters and moves some consistency data from the object, if provided.
		 * @param homography The already rough homography to be optimized, transforming points defined in the template frame to points defined in the current frame
		 * @param templateFrame The template frame representing the ground truth data
		 * @param templateSubRegion The sub-region within the template frame specifying the planar area of interest
		 * @param currentFrame The current frame for which the homography needs to be optimized
		 * @param homographyParameters The number of parameters used to define the homography, with range [8, 9]
		 * @param zeroMean True, to subtract the color intensities from the corresponding mean intensity before determining the error; False, to determine the error directly
		 * @param consistencyData Optional consistency data object to improve the performance of this provider
		 * @param abort Optional abort statement allowing to abort the frame type request at any time; set the value True to abort the request
		 */
		inline DenseAdditiveHomographyProvider(SquareMatrix3& homography, const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const unsigned int homographyParameters, const bool zeroMean, ConsistencyDataRef* consistencyData = nullptr, bool* abort = nullptr);

		/**
		 * Destructs the provider object and moves some consistency data back to the consistency object, if provided
		 */
		inline ~DenseAdditiveHomographyProvider() // need to be defined here in the class as Clang will complain otherwise
		{
			if (this->consistencyObject_)
			{
				// we now move the information from the provider to the consistency data object
				this->consistencyObject_->moveFromProvider(*this);
			}
		}

		/**
		 * Determines the Hessian matrix and the Error-Jacobian vector (the transposed Jacobian multiplied with the individual errors).
		 * @see AdvancedProviderBase::determineHessianAndErrorJacobian().
		 */
		inline bool determineHessianAndErrorJacobian(Matrix& hessian, Matrix& jacobianError);

		/**
		 * Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedProviderBase::applyCorrection().
		 */
		inline void applyCorrection(const Matrix& deltas);

	protected:

		/**
		 * Determines the Hessian matrix and the Error-Jacobian vector (the transposed Jacobian multiplied with the individual errors).
		 * @see determineHessianAndErrorJacobian().
		 */
		template <bool tUseMeans>
		bool determineHessianAndErrorJacobian(Matrix& hessian, Matrix& jacobianError);

	protected:

		/// The re-usable memory/frame so that it can hold the gradient data of the current frame.
		Frame reusableGradientCurrentFrame_;
};

template <unsigned int tChannels>
inline HomographyImageAlignmentDense::DenseAdditiveHomographyProvider<tChannels>::DenseAdditiveHomographyProvider(SquareMatrix3& homography, const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const unsigned int homographyParameters, const bool zeroMean, ConsistencyDataRef* consistencyData, bool* abort) :
	HomographyProvider<tChannels>(homography, templateFrame, templateSubRegion, currentFrame, homographyParameters, zeroMean, abort)
{
	static_assert(tChannels >= 1u && tChannels <= 4u, "Invalid channel number!");

	if (!reusableGradientCurrentFrame_.set(FrameType(currentFrame, FrameType::genericPixelFormat<int16_t, tChannels * 2u>()), true /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	if (consistencyData)
	{
		if (consistencyData->isNull())
		{
			this->consistencyObject_ = new typename HomographyProvider<tChannels>::ConsistencyObject(*this);
			*consistencyData = ConsistencyDataRef(this->consistencyObject_);
		}
		else
		{
			this->consistencyObject_ = dynamic_cast<typename HomographyProvider<tChannels>::ConsistencyObject*>(consistencyData->pointer());
			ocean_assert(this->consistencyObject_ != nullptr);

			// we now move/copy the information form the consistency data object to the provider
			this->consistencyObject_->moveToProvider(*this);
		}
	}
}

template <unsigned int tChannels>
inline bool HomographyImageAlignmentDense::DenseAdditiveHomographyProvider<tChannels>::determineHessianAndErrorJacobian(Matrix& hessian, Matrix& jacobianError)
{
	if (this->zeroMean_)
	{
		return determineHessianAndErrorJacobian<true>(hessian, jacobianError);
	}
	else
	{
		return determineHessianAndErrorJacobian<false>(hessian, jacobianError);
	}
}

template <unsigned int tChannels>
inline void HomographyImageAlignmentDense::DenseAdditiveHomographyProvider<tChannels>::applyCorrection(const Matrix& deltas)
{
	ocean_assert((deltas.rows() == 8u || deltas.rows() == 9u) && deltas.columns() == 1u);

	for (unsigned int n = 0u; n < deltas.rows(); ++n)
	{
		this->homographyCandidate_[n] = this->homography_[n] - deltas(n);
	}

	Geometry::Homography::normalizeHomography(this->homographyCandidate_);

	const Box2 boundingBox = this->templateSubRegion_.boundingBox();
	const Box2 enlargedBoundingBox(boundingBox.center(), boundingBox.width() * 2, boundingBox.height() * 2);

	const Vector2 corners[4] =
	{
		Vector2(enlargedBoundingBox.left(), enlargedBoundingBox.top()),
		Vector2(enlargedBoundingBox.right(), enlargedBoundingBox.top()),
		Vector2(enlargedBoundingBox.right(), enlargedBoundingBox.bottom()),
		Vector2(enlargedBoundingBox.left(), enlargedBoundingBox.bottom())
	};

	Scalar maxOffset = 0;
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		maxOffset = max(maxOffset, (this->homography_ * corners[n]).sqrDistance(this->homographyCandidate_ * corners[n]));
	}

	if (maxOffset < 0.5 * 0.5)
	{
		this->previousIterationsWithSmallImpact_++;
	}
	else
	{
		this->previousIterationsWithSmallImpact_ = 0u;
	}
}

template <unsigned int tChannels>
template <bool tUseMeans>
bool HomographyImageAlignmentDense::DenseAdditiveHomographyProvider<tChannels>::determineHessianAndErrorJacobian(Matrix& hessian, Matrix& jacobianError)
{
	ocean_assert(Numeric::isEqual(this->homographyCandidate_[8], 1));

	Frame transformedCurrentSubFrame = this->reusableTransformedCurrentFrame_.subFrame(0u, 0u, this->templateSubRegionBoundingBox_.width(), this->templateSubRegionBoundingBox_.height(), Frame::CM_USE_KEEP_LAYOUT);
	Frame transformedCurrentSubMask = this->reusableTransformedCurrentMask_.subFrame(0u, 0u, this->templateSubRegionBoundingBox_.width(), this->templateSubRegionBoundingBox_.height(), Frame::CM_USE_KEEP_LAYOUT);

	// we check whether the tracking region leaves the image content so that we have to use a mask telling which pixel is valid and which pixel lies outside
	const bool needCoverageMask = !CV::FrameInterpolatorBilinear::coversHomographyInputFrame(this->currentFrame_.width(), this->currentFrame_.height(), transformedCurrentSubFrame.width(), transformedCurrentSubFrame.height(), this->homographyCandidate_, int(this->templateSubRegionBoundingBox_.left()), int(this->templateSubRegionBoundingBox_.top()));

	if (this->homographyCandidate_ != this->usedHomographyForCurrent_)
	{
		if (needCoverageMask)
		{
			if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(this->currentFrame_, transformedCurrentSubFrame, transformedCurrentSubMask, this->homographyCandidate_, WorkerPool::get().scopedWorker()(), 0xFF, CV::PixelPositionI(int(this->templateSubRegionBoundingBox_.left()), int(this->templateSubRegionBoundingBox_.top()))))
			{
				return false;
			}
		}
		else
		{
			if (!CV::FrameInterpolatorBilinear::Comfort::homography(this->currentFrame_, transformedCurrentSubFrame, this->homographyCandidate_, nullptr, WorkerPool::get().scopedWorker()(), CV::PixelPositionI(int(this->templateSubRegionBoundingBox_.left()), int(this->templateSubRegionBoundingBox_.top()))))
			{
				return false;
			}
		}
	}

	this->usedHomographyForCurrent_ = this->homographyCandidate_;

#ifdef OCEAN_INTENSIVE_DEBUG
	{
		// we ensure that the sub-frame is identical to the sub-region of the entire frame

		Frame transformedCurrentFrameLarge(this->templateFrame_.frameType());
		Frame transformedCurrentMaskLarge(FrameType(this->templateFrame_, FrameType::FORMAT_Y8));

		if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(this->currentFrame_, transformedCurrentFrameLarge, transformedCurrentMaskLarge, this->homographyCandidate_, WorkerPool::get().scopedWorker()(), 0xFFu))
		{
			ocean_assert(false && "This must never happen!");
			return false;
		}

		for (unsigned int y = 0u; y < this->templateSubRegionBoundingBox_.height(); ++y)
		{
			for (unsigned int x = 0u; x < this->templateSubRegionBoundingBox_.width(); ++x)
			{
				if (needCoverageMask)
				{
					ocean_assert(*transformedCurrentSubMask.constpixel<uint8_t>(x, y) == *transformedCurrentMaskLarge.constpixel<uint8_t>(x + this->templateSubRegionBoundingBox_.left(), y + this->templateSubRegionBoundingBox_.top()));
				}

				if (*transformedCurrentSubMask.constpixel<uint8_t>(x, y) == 0xFFu)
				{
					typedef typename DataType<uint8_t, tChannels>::Type PixelType;

					const PixelType* subFramePixel = (const PixelType*)(transformedCurrentSubFrame.constpixel<uint8_t>(x, y));
					const PixelType* largePixel = (const PixelType*)(transformedCurrentFrameLarge.constpixel<uint8_t>(x + this->templateSubRegionBoundingBox_.left(), y + this->templateSubRegionBoundingBox_.top()));

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						ocean_assert(std::abs(int(subFramePixel->values_[n]) - int(largePixel->values_[n])) <= 1);
					}
				}
			}
		}
	}
#endif

	Scalar transformedCurrentFrameMean[tChannels];

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		transformedCurrentFrameMean[n] = Numeric::maxValue();
	}
#endif

	if constexpr (tUseMeans)
	{
		if (!determineMeans8BitPerChannel<tChannels, false>(this->templateFrame_, this->templateSubRegion_, transformedCurrentSubFrame, needCoverageMask ? transformedCurrentSubMask : Frame(), this->templateFrameMean_, transformedCurrentFrameMean, WorkerPool::get().scopedWorker()()))
		{
			return false;
		}
	}

	const Vector2 transformedCorners[] =
	{
		this->homographyCandidate_ * Vector2(Scalar(this->templateSubRegionBoundingBox_.left()), Scalar(this->templateSubRegionBoundingBox_.top())),
		this->homographyCandidate_ * Vector2(Scalar(this->templateSubRegionBoundingBox_.left()), Scalar(this->templateSubRegionBoundingBox_.bottomEnd())),
		this->homographyCandidate_ * Vector2(Scalar(this->templateSubRegionBoundingBox_.rightEnd()), Scalar(this->templateSubRegionBoundingBox_.bottomEnd())),
		this->homographyCandidate_ * Vector2(Scalar(this->templateSubRegionBoundingBox_.rightEnd()), Scalar(this->templateSubRegionBoundingBox_.top()))
	};

	const bool isConvex = Geometry::Utilities::isPolygonConvex(transformedCorners, 4);
	ocean_assert_and_suppress_unused(isConvex, isConvex);

	Box2 transformedBoundingBox(transformedCorners, 4);
	transformedBoundingBox = Box2(transformedBoundingBox.lower(), transformedBoundingBox.higher() + Vector2(1, 1));

	unsigned int transformedBoundingBoxLeft = 0u, transformedBoundingBoxTop = 0u, transformedBoundingBoxWidth = 0u, transformedBoundingBoxHeight = 0u;
	if (!transformedBoundingBox.box2integer(this->currentFrame_.width(), this->currentFrame_.height(), transformedBoundingBoxLeft, transformedBoundingBoxTop, transformedBoundingBoxWidth, transformedBoundingBoxHeight))
	{
		return false;
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	{
		const unsigned int templateWidth = this->templateFrame_.width();
		const unsigned int templateHeight = this->templateFrame_.height();

		for (unsigned int y = this->templateSubRegionBoundingBox_.top(); y < this->templateSubRegionBoundingBox_.top() + this->templateSubRegionBoundingBox_.height(); y++)
		{
			for (unsigned int x = this->templateSubRegionBoundingBox_.left(); x < this->templateSubRegionBoundingBox_.left() + this->templateSubRegionBoundingBox_.width(); ++x)
			{
				if (this->templateSubRegion_.isInside(Vector2(Scalar(x), Scalar(y))))
				{
					const Vector2 transformedPoint(this->homographyCandidate_ * Vector2(Scalar(x), Scalar(y)));

					if (transformedPoint.x() >= 0 && transformedPoint.y() >= 0 && transformedPoint.x() <= Scalar(templateWidth - 1u) && transformedPoint.y() <= Scalar(templateHeight - 1u))
					{
						ocean_assert(transformedBoundingBox.isInside(transformedPoint));

						ocean_assert(Scalar(transformedBoundingBoxLeft) <= transformedPoint.x());
						ocean_assert(Scalar(transformedBoundingBoxTop) <= transformedPoint.y());

						ocean_assert(transformedPoint.x() <= Scalar(transformedBoundingBoxLeft + transformedBoundingBoxWidth - 1u));
						ocean_assert(transformedPoint.y() <= Scalar(transformedBoundingBoxTop + transformedBoundingBoxHeight - 1u));
					}
				}
			}
		}
	}
#endif

	// we create the gradient image but only use the memory which is very efficient
	Frame gradientCurrentFrame = this->reusableGradientCurrentFrame_.subFrame(0u, 0u, transformedBoundingBoxWidth, transformedBoundingBoxHeight, Frame::CM_USE_KEEP_LAYOUT);

	CV::FrameFilterGradient::filterHorizontalVerticalSubFrame<uint8_t, int16_t, tChannels, false>(this->currentFrame_.template constdata<uint8_t>(), this->currentFrame_.width(), this->currentFrame_.height(), this->currentFrame_.paddingElements(), transformedBoundingBoxLeft, transformedBoundingBoxTop, gradientCurrentFrame.data<int16_t>(), gradientCurrentFrame.width(), gradientCurrentFrame.height(), gradientCurrentFrame.paddingElements(), 1, WorkerPool::get().scopedWorker()());

#ifdef OCEAN_INTENSIVE_DEBUG
	{
		// we ensure that the (integer) gradient result of the sub-frame is equal to the (float) gradient result of the entire frame

		Frame gradientCurrentFrameLarge(FrameType(this->currentFrame_, FrameType::genericPixelFormat<float, tChannels * 2u>()));
		CV::FrameFilterGradient::filterHorizontalVertical<uint8_t, float, tChannels, true>(this->currentFrame_.template constdata<uint8_t>(), gradientCurrentFrameLarge.data<float>(), this->currentFrame_.width(), this->currentFrame_.height(), this->currentFrame_.paddingElements(), gradientCurrentFrameLarge.paddingElements(), 1.0f / 255.0f, WorkerPool::get().scopedWorker()());

		for (unsigned int y = 0u; y < transformedBoundingBoxHeight; ++y)
		{
			for (unsigned int x = 0u; x < transformedBoundingBoxWidth; ++x)
			{
				const int16_t* const gradientSmall = gradientCurrentFrame.constpixel<int16_t>(x, y);
				const float* const gradientLarge = gradientCurrentFrameLarge.constpixel<float>(x, y);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					const int16_t smallX = gradientSmall[2u * n + 0u];
					const int16_t smallY = gradientSmall[2u * n + 1u];

					const float largeX = gradientLarge[2u * n + 0u];
					const float largeY = gradientLarge[2u * n + 1u];

					const float floatSmallX = float(smallX) * 0.5f * (1.0f / 255.0f);
					const float floatSmallY = float(smallY) * 0.5f * (1.0f / 255.0f);

					ocean_assert(NumericF::isEqual(floatSmallX, largeX) && NumericF::isEqual(floatSmallY, largeY));
				}
			}
		}
	}
#endif

	if (this->homographyParameters_ == 8u)
	{
		determineHessianAndErrorJacobian8BitPerChannel<8u, tChannels, tUseMeans>(this->templateFrame_, this->templateSubRegion_, transformedCurrentSubFrame, needCoverageMask ? transformedCurrentSubMask : Frame(), transformedBoundingBoxLeft, transformedBoundingBoxTop, transformedBoundingBoxWidth, transformedBoundingBoxHeight, gradientCurrentFrame, this->homographyCandidate_, this->templateFrameMean_, transformedCurrentFrameMean, hessian, jacobianError, WorkerPool::get().scopedWorker()());
	}
	else
	{
		determineHessianAndErrorJacobian8BitPerChannel<9u, tChannels, tUseMeans>(this->templateFrame_, this->templateSubRegion_, transformedCurrentSubFrame, needCoverageMask ? transformedCurrentSubMask : Frame(), transformedBoundingBoxLeft, transformedBoundingBoxTop, transformedBoundingBoxWidth, transformedBoundingBoxHeight, gradientCurrentFrame, this->homographyCandidate_, this->templateFrameMean_, transformedCurrentFrameMean, hessian, jacobianError, WorkerPool::get().scopedWorker()());
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	{
		Matrix debugHessian, debugJacobianError;

		if (this->homographyParameters_ == 8u)
		{
			HomographyImageAlignmentDense::slowDetermineHessianAndErrorJacobian<8u>(this->templateFrame_, this->templateSubRegion_, this->currentFrame_, this->homographyCandidate_, tUseMeans, debugHessian, debugJacobianError);
		}
		else
		{
			HomographyImageAlignmentDense::slowDetermineHessianAndErrorJacobian<9u>(this->templateFrame_, this->templateSubRegion_, this->currentFrame_, this->homographyCandidate_, tUseMeans, debugHessian, debugJacobianError);
		}

		const Scalar subRegionSize = this->templateSubRegion_.size();
		const Scalar jacobianErrorEpsilon = Scalar(1e-6) * subRegionSize;
		const Scalar hessianErrorEpsilon = Scalar(1e-4) * subRegionSize;

		ocean_assert(debugJacobianError.isEqual(jacobianError, jacobianErrorEpsilon));
		ocean_assert(debugHessian.isEqual(hessian, hessianErrorEpsilon));
	}
#endif

	return true;
}

bool HomographyImageAlignmentDense::optimizeAlignmentAdditive(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& roughHomography, const unsigned int homographyParameters, const bool zeroMean, SquareMatrix3& homography, const unsigned int iterations, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors, ConsistencyDataRef* externalConsistencyData, bool* abort)
{
	ocean_assert(templateFrame.pixelFormat() == currentFrame.pixelFormat());
	ocean_assert(templateFrame.pixelOrigin() == currentFrame.pixelOrigin());

	ocean_assert(templateFrame.numberPlanes() == 1u);
	ocean_assert(templateFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(templateSubRegion);

	if (templateFrame.pixelFormat() != currentFrame.pixelFormat() || templateFrame.numberPlanes() != 1u || templateFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	ocean_assert(iterations != 0u);
	ocean_assert(&roughHomography != &homography);

	homography = roughHomography;

	switch (FrameType::formatGenericNumberChannels(currentFrame.pixelFormat()))
	{
		case 1u:
		{
			DenseAdditiveHomographyProvider<1u> provider(homography, templateFrame, templateSubRegion, currentFrame, homographyParameters, zeroMean, externalConsistencyData, abort);
			return Geometry::NonLinearOptimization::advancedDenseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case 2u:
		{
			DenseAdditiveHomographyProvider<2u> provider(homography, templateFrame, templateSubRegion, currentFrame, homographyParameters, zeroMean, externalConsistencyData, abort);
			return Geometry::NonLinearOptimization::advancedDenseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case 3u:
		{
			DenseAdditiveHomographyProvider<3u> provider(homography, templateFrame, templateSubRegion, currentFrame, homographyParameters, zeroMean, externalConsistencyData, abort);
			return Geometry::NonLinearOptimization::advancedDenseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case 4u:
		{
			DenseAdditiveHomographyProvider<4u> provider(homography, templateFrame, templateSubRegion, currentFrame, homographyParameters, zeroMean, externalConsistencyData, abort);
			return Geometry::NonLinearOptimization::advancedDenseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

/**
 * Implements an advanced optimization provider for dense image alignment by an inverse compositional delta approach.
 * @tparam tChannels The number of color channels of the frames to align, with range [1, 4]
 */
template <unsigned int tChannels>
class HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider : public HomographyProvider<tChannels>
{
	public:

		/**
		 * Definition of a specific consistency data object.
		 */
		class InverseCompositionalConsistencyObject : public HomographyProvider<tChannels>::ConsistencyObject
		{
			friend class DenseInverseCompositionalHomographyProvider;

			public:

				/**
				 * Creates a new object.
				 * @owner The owner of this consistency object
				 */
				explicit inline InverseCompositionalConsistencyObject(DenseInverseCompositionalHomographyProvider& owner);

				/**
				 * Moves the data from this consistency object to the provider.
				 * @param provider The provider to which the data of this object will be moved
				 */
				void moveToProvider(HomographyProvider<tChannels>& provider) override;

				/**
				 * Moves the data from the owning provider to this consistency object.
				 * @aram provider The owning provider from which the data will be moved
				 */
				void moveFromProvider(HomographyProvider<tChannels>& provider) override;

			protected:

				/// The (approximated) Hessian matrix that depends on the template frame only (not on the current frame).
				Matrix hessian_;

				/// The Jacobian matrix rows that depends on the template frame only, one row for each pixel and channel.
				Scalars jacobianRows_;
		};

	public:

		/**
		 * Creates a new provider object by the given parameters and moves some consistency data from the object, if provided.
		 * @param homography The already rough homography to be optimized, transforming points defined in the template frame to points defined in the current frame
		 * @param templateFrame The template frame representing the ground truth data
		 * @param templateSubRegion The sub-region within the template frame specifying the planar area of interest
		 * @param currentFrame The current frame for which the homography needs to be optimized
		 * @param homographyParameters The number of parameters used to define the homography, with range [8, 9]
		 * @param zeroMean True, to subtract the color intensities from the corresponding mean intensity before determining the error; False, to determine the error directly
		 * @param consistencyData Optional consistency data object to improve the performance of this provider
		 * @param abort Optional abort statement allowing to abort the frame type request at any time; set the value True to abort the request
		 */
		inline DenseInverseCompositionalHomographyProvider(SquareMatrix3& homography, const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const unsigned int homographyParameters, const bool zeroMean, ConsistencyDataRef* consistencyData = nullptr, bool* abort = nullptr);

		/**
		 * Destructs the provider object and moves some consistency data back to the consistency object, if provided
		 */
		inline ~DenseInverseCompositionalHomographyProvider() // need to be defined here in the class as Clang will complain otherwise
		{
			if (this->consistencyObject_)
			{
				// we now move the information from the provider to the consistency data object
				this->consistencyObject_->moveFromProvider(*this);
			}
		}

		/**
		 * Determines the Hessian matrix and the Error-Jacobian vector (the transposed Jacobian multiplied with the individual errors).
		 * @see AdvancedProviderBase::determineHessianAndErrorJacobian().
		 */
		inline bool determineHessianAndErrorJacobian(Matrix& hessian, Matrix& jacobianError);

		/**
		 * Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedProviderBase::applyCorrection().
		 */
		inline void applyCorrection(const Matrix& deltas);

		/**
		 * Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedProviderBase::acceptCorrection().
		 */
		inline void acceptCorrection();

		/**
		 * Returns whether the optimization process should stop e.g., due to an external event.
		 * @see AdvancedProviderBase::shouldStop().
		 */
		inline bool shouldStop();

	protected:

		/**
		 * Determines the Hessian matrix and the Error-Jacobian vector (the transposed Jacobian multiplied with the individual errors).
		 * @see determineHessianAndErrorJacobian().
		 */
		template <bool tUseMeans>
		bool determineHessianAndErrorJacobian(Matrix& hessian, Matrix& jacobianError);

		/**
		 * Determines the constant Hessian matrix and constant Jacobian vector.
		 * @tparam tParameters The number of parameters describing the homography and used to optimize it, with range [8, 9]
		 */
		template <unsigned int tParameters>
		bool determineHessianAndJacobian();

	protected:

		/// The constant Hessian matrix for the template frame.
		Matrix hessian_;

		/// The constant Jacobian matrix rows for the template frame, one row for each pixel and channel.
		Scalars jacobianRows_;
};

template <unsigned int tChannels>
inline HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider<tChannels>::InverseCompositionalConsistencyObject::InverseCompositionalConsistencyObject(DenseInverseCompositionalHomographyProvider& owner) :
	HomographyProvider<tChannels>::ConsistencyObject(owner)
{
	// nothing to do here
}

template <unsigned int tChannels>
inline void HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider<tChannels>::InverseCompositionalConsistencyObject::moveToProvider(HomographyProvider<tChannels>& provider)
{
	HomographyProvider<tChannels>::ConsistencyObject::moveToProvider(provider);

	DenseInverseCompositionalHomographyProvider<tChannels>& specializedProvider = static_cast<DenseInverseCompositionalHomographyProvider<tChannels>&>(provider);

	specializedProvider.hessian_ = std::move(hessian_);
	specializedProvider.jacobianRows_ = std::move(jacobianRows_);
}

template <unsigned int tChannels>
inline void HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider<tChannels>::InverseCompositionalConsistencyObject::moveFromProvider(HomographyProvider<tChannels>& provider)
{
	HomographyProvider<tChannels>::ConsistencyObject::moveFromProvider(provider);

	DenseInverseCompositionalHomographyProvider<tChannels>& specializedProvider = static_cast<DenseInverseCompositionalHomographyProvider<tChannels>&>(provider);

	hessian_ = std::move(specializedProvider.hessian_);
	jacobianRows_ = std::move(specializedProvider.jacobianRows_);
}

template <unsigned int tChannels>
inline HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider<tChannels>::DenseInverseCompositionalHomographyProvider(SquareMatrix3& homography, const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const unsigned int homographyParameters, const bool zeroMean, ConsistencyDataRef* consistencyData, bool* abort) :
	HomographyProvider<tChannels>(homography, templateFrame, templateSubRegion, currentFrame, homographyParameters, zeroMean, abort)
{
	static_assert(tChannels >= 1u && tChannels <= 4u, "Invalid channel number!");

	ocean_assert(Numeric::isEqual(this->homography_[8], 1));

	ocean_assert(FrameType::formatIsGeneric(this->templateFrame_.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, tChannels));
	ocean_assert(this->templateFrame_.pixelFormat() == this->currentFrame_.pixelFormat());
	ocean_assert(this->templateFrame_.pixelOrigin() == this->currentFrame_.pixelOrigin());
	ocean_assert(this->templateSubRegion_);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		this->templateFrameMean_[n] = Numeric::maxValue();
	}

	if (!this->reusableTransformedCurrentFrame_.set(this->templateFrame_.frameType(), true /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	if (!this->reusableTransformedCurrentMask_.set(FrameType(this->templateFrame_.frameType(), FrameType::FORMAT_Y8), true /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	ocean_assert(this->templateSubRegion_.boundingBox().isValid());

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!this->templateSubRegion_.boundingBox().box2integer(this->templateFrame_.width(), this->templateFrame_.height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		ocean_assert(false && "This should never happen!");
	}

	this->templateSubRegionBoundingBox_ = CV::PixelBoundingBox(CV::PixelPosition(boundingBoxLeft, boundingBoxTop), boundingBoxWidth, boundingBoxHeight);

	ocean_assert(this->reusableTransformedCurrentFrame_.width() >= this->templateSubRegionBoundingBox_.width() && this->reusableTransformedCurrentFrame_.height() >= this->templateSubRegionBoundingBox_.height());
	ocean_assert(this->reusableTransformedCurrentMask_.width() >= this->templateSubRegionBoundingBox_.width() && this->reusableTransformedCurrentMask_.height() >= this->templateSubRegionBoundingBox_.height());

	if (consistencyData)
	{
		if (consistencyData->isNull())
		{
			this->consistencyObject_ = new InverseCompositionalConsistencyObject(*this);
			*consistencyData = ConsistencyDataRef(this->consistencyObject_);
		}
		else
		{
			this->consistencyObject_ = dynamic_cast<typename HomographyProvider<tChannels>::ConsistencyObject*>(consistencyData->pointer());
			ocean_assert(this->consistencyObject_ != nullptr);

			// we now move/copy the information form the consistency data object to the provider
			this->consistencyObject_->moveToProvider(*this);
		}
	}
}

template <unsigned int tChannels>
inline bool HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider<tChannels>::determineHessianAndErrorJacobian(Matrix& hessian, Matrix& jacobianError)
{
	if (this->zeroMean_)
	{
		return determineHessianAndErrorJacobian<true>(hessian, jacobianError);
	}
	else
	{
		return determineHessianAndErrorJacobian<false>(hessian, jacobianError);
	}
}

template <unsigned int tChannels>
inline void HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider<tChannels>::applyCorrection(const Matrix& deltas)
{
	ocean_assert((deltas.rows() == 8u || deltas.rows() == 9u) && deltas.columns() == 1u);

	SquareMatrix3 deltaHomography(true);

	for (unsigned int n = 0u; n < deltas.rows(); ++n)
	{
		deltaHomography[n] += deltas(n);
	}

	// f(p+1) = f(f(delta)^-1, p)
	this->homographyCandidate_ = this->homography_ * deltaHomography.inverted();

	Geometry::Homography::normalizeHomography(this->homographyCandidate_);

	const Box2 boundingBox = this->templateSubRegion_.boundingBox();
	const Box2 enlargedBoundingBox(boundingBox.center(), boundingBox.width() * 2, boundingBox.height() * 2);

	const Vector2 corners[4] =
	{
		Vector2(enlargedBoundingBox.left(), enlargedBoundingBox.top()),
		Vector2(enlargedBoundingBox.right(), enlargedBoundingBox.top()),
		Vector2(enlargedBoundingBox.right(), enlargedBoundingBox.bottom()),
		Vector2(enlargedBoundingBox.left(), enlargedBoundingBox.bottom())
	};

	Scalar maxOffset = 0;
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		maxOffset = max(maxOffset, (this->homography_ * corners[n]).sqrDistance(this->homographyCandidate_ * corners[n]));
	}

	if (maxOffset < 0.5 * 0.5)
	{
		this->previousIterationsWithSmallImpact_++;
	}
	else
	{
		this->previousIterationsWithSmallImpact_ = 0u;
	}
}

template <unsigned int tChannels>
inline void HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider<tChannels>::acceptCorrection()
{
	ocean_assert(Numeric::isEqual(this->homographyCandidate_[8], 1));
	this->homography_ = this->homographyCandidate_;
}

template <unsigned int tChannels>
inline bool HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider<tChannels>::shouldStop()
{
	return this->previousIterationsWithSmallImpact_ >= 5u || (this->abort_ != nullptr && *this->abort_ == true);
}

template <unsigned int tChannels>
template <bool tUseMeans>
bool HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider<tChannels>::determineHessianAndErrorJacobian(Matrix& hessian, Matrix& jacobianError)
{
	ocean_assert(Numeric::isEqual(this->homographyCandidate_[8], 1));

	Frame transformedCurrentSubFrame = this->reusableTransformedCurrentFrame_.subFrame(0u, 0u, this->templateSubRegionBoundingBox_.width(), this->templateSubRegionBoundingBox_.height(), Frame::CM_USE_KEEP_LAYOUT);
	Frame transformedCurrentSubMask = this->reusableTransformedCurrentMask_.subFrame(0u, 0u, this->templateSubRegionBoundingBox_.width(), this->templateSubRegionBoundingBox_.height(), Frame::CM_USE_KEEP_LAYOUT);

	// we check whether the tracking region leaves the image content so that we have to use a mask telling which pixel is valid and which pixel lies outside
	const bool needCoverageMask = !CV::FrameInterpolatorBilinear::coversHomographyInputFrame(this->currentFrame_.width(), this->currentFrame_.height(), transformedCurrentSubFrame.width(), transformedCurrentSubFrame.height(), this->homographyCandidate_, int(this->templateSubRegionBoundingBox_.left()), int(this->templateSubRegionBoundingBox_.top()));

	if (this->homographyCandidate_ != this->usedHomographyForCurrent_)
	{
		if (needCoverageMask)
		{
			if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(this->currentFrame_, transformedCurrentSubFrame, transformedCurrentSubMask, this->homographyCandidate_, WorkerPool::get().scopedWorker()(), 0xFF, CV::PixelPositionI(int(this->templateSubRegionBoundingBox_.left()), int(this->templateSubRegionBoundingBox_.top()))))
			{
				return false;
			}
		}
		else
		{
			if (!CV::FrameInterpolatorBilinear::Comfort::homography(this->currentFrame_, transformedCurrentSubFrame, this->homographyCandidate_, nullptr, WorkerPool::get().scopedWorker()(), CV::PixelPositionI(int(this->templateSubRegionBoundingBox_.left()), int(this->templateSubRegionBoundingBox_.top()))))
			{
				return false;
			}
		}
	}

	this->usedHomographyForCurrent_ = this->homographyCandidate_;

#ifdef OCEAN_INTENSIVE_DEBUG
	{
		// we ensure that the sub-frame is identical to the sub-region of the entire frame

		Frame transformedCurrentFrameLarge(this->templateFrame_.frameType());
		Frame transformedCurrentMaskLarge(FrameType(this->templateFrame_, FrameType::FORMAT_Y8));

		if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(this->currentFrame_, transformedCurrentFrameLarge, transformedCurrentMaskLarge, this->homographyCandidate_, WorkerPool::get().scopedWorker()(), 0xFFu))
		{
			ocean_assert(false && "This must never happen!");
			return false;
		}

		for (unsigned int y = 0u; y < this->templateSubRegionBoundingBox_.height(); ++y)
		{
			for (unsigned int x = 0u; x < this->templateSubRegionBoundingBox_.width(); ++x)
			{
				if (needCoverageMask)
				{
					ocean_assert(*transformedCurrentSubMask.constpixel<uint8_t>(x, y) == *transformedCurrentMaskLarge.constpixel<uint8_t>(x + this->templateSubRegionBoundingBox_.left(), y + this->templateSubRegionBoundingBox_.top()));
				}

				if (!needCoverageMask || *transformedCurrentSubMask.constpixel<uint8_t>(x, y) == 0xFFu)
				{
					typedef typename DataType<uint8_t, tChannels>::Type PixelType;

					ocean_assert(*((const PixelType*)(transformedCurrentSubFrame.constpixel<uint8_t>(x, y))) == *((const PixelType*)(transformedCurrentFrameLarge.constpixel<uint8_t>(x + this->templateSubRegionBoundingBox_.left(), y + this->templateSubRegionBoundingBox_.top()))));
				}
			}
		}
	}
#endif

	Scalar transformedCurrentFrameMean[tChannels];

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		transformedCurrentFrameMean[n] = Numeric::maxValue();
	}
#endif

	if constexpr (tUseMeans)
	{
		if (!determineMeans8BitPerChannel<tChannels, false>(this->templateFrame_, this->templateSubRegion_, transformedCurrentSubFrame, needCoverageMask ? transformedCurrentSubMask : Frame(), this->templateFrameMean_, transformedCurrentFrameMean, WorkerPool::get().scopedWorker()()))
		{
			return false;
		}
	}

	if (!hessian_)
	{
		if (this->homographyParameters_ == 8u)
		{
			determineHessianAndJacobian<8u>();
		}
		else
		{
			determineHessianAndJacobian<9u>();
		}
	}

	ocean_assert(this->hessian_.rows() == this->homographyParameters_ && this->hessian_.columns() == this->homographyParameters_);

	// **TODO** use homography mask

	if (this->homographyParameters_ == 8u)
	{
		HomographyImageAlignmentDense::determineErrorJacobianInverseCompositional8BitPerChannel<8u, tChannels, tUseMeans>(this->templateFrame_, this->templateSubRegion_, transformedCurrentSubFrame, Frame(), this->templateFrameMean_, transformedCurrentFrameMean, this->jacobianRows_.data(), jacobianError, WorkerPool::get().scopedWorker()());
	}
	else
	{
		HomographyImageAlignmentDense::determineErrorJacobianInverseCompositional8BitPerChannel<9u, tChannels, tUseMeans>(this->templateFrame_, this->templateSubRegion_, transformedCurrentSubFrame, Frame(), this->templateFrameMean_, transformedCurrentFrameMean, this->jacobianRows_.data(), jacobianError, WorkerPool::get().scopedWorker()());
	}

	// we simply copy the constant Hessian
	hessian = this->hessian_;

#ifdef OCEAN_INTENSIVE_DEBUG

	{
		Frame gradientTemplateFrame(FrameType(this->templateFrame_, FrameType::genericPixelFormat<int16_t, tChannels * 2u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::FrameFilterGradient::filterHorizontalVertical<uint8_t, int16_t, tChannels, false>(this->templateFrame_.template constdata<uint8_t>(), gradientTemplateFrame.data<int16_t>(), this->templateFrame_.width(), this->templateFrame_.height(), this->templateFrame_.paddingElements(), gradientTemplateFrame.paddingElements(),1, WorkerPool::get().scopedWorker()());

		Frame transformedCurrentFrameLarge(this->templateFrame_.frameType());
		Frame unusedTransformedCurrentMaskLarge(FrameType(this->templateFrame_, FrameType::FORMAT_Y8));

		if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(this->currentFrame_, transformedCurrentFrameLarge, unusedTransformedCurrentMaskLarge, this->homographyCandidate_, WorkerPool::get().scopedWorker()(), 0xFFu))
		{
			ocean_assert(false && "This must never happen!");
			return false;
		}

		Matrix debugHessian(this->homographyParameters_, this->homographyParameters_, false);
		Matrix debugJacobianError(this->homographyParameters_, 1, false);

		Matrix homographyJacobian(2, this->homographyParameters_);
		Matrix gradient(1, 2);
		Matrix intermediate(1, this->homographyParameters_);

		constexpr Scalar gradientNormalization = Scalar(0.5) / Scalar(255);

		for (unsigned int y = this->templateSubRegionBoundingBox_.top(); y < this->templateSubRegionBoundingBox_.bottomEnd(); ++y)
		{
			for (unsigned int x = this->templateSubRegionBoundingBox_.left(); x < this->templateSubRegionBoundingBox_.rightEnd(); ++x)
			{
				if (this->templateSubRegion_.isInside(Vector2(Scalar(x), Scalar(y))))
				{
					if (this->homographyParameters_ == 8u)
					{
						Geometry::Jacobian::calculateIdentityHomographyJacobian2x8(homographyJacobian[0], homographyJacobian[1], Scalar(x), Scalar(y));
					}
					else
					{
						Geometry::Jacobian::calculateIdentityHomographyJacobian2x9(homographyJacobian[0], homographyJacobian[1], Scalar(x), Scalar(y));
					}

					Scalar channelError;

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						gradient(0, 0) = Scalar(gradientTemplateFrame.constpixel<int16_t>(x, y)[n * 2u + 0u]) * gradientNormalization;
						gradient(0, 1) = Scalar(gradientTemplateFrame.constpixel<int16_t>(x, y)[n * 2u + 1u]) * gradientNormalization;

						intermediate = gradient * homographyJacobian;
						debugHessian += intermediate.transposed() * intermediate;

						if constexpr (tUseMeans)
						{
							channelError = ((Scalar(transformedCurrentFrameLarge.constpixel<uint8_t>(x, y)[n]) - transformedCurrentFrameMean[n]) - (Scalar(this->templateFrame_.template constpixel<uint8_t>(x, y)[n]) - this->templateFrameMean_[n])) * Scalar(1.0 / 255.0);
						}
						else
						{
							channelError = Scalar(int(transformedCurrentFrameLarge.constpixel<uint8_t>(x, y)[n]) - int(this->templateFrame_.template constpixel<uint8_t>(x, y)[n])) * Scalar(1.0 / 255.0);
						}

						const unsigned int jacobianIndex = tChannels * ((y - this->templateSubRegionBoundingBox_.top()) * this->templateSubRegionBoundingBox_.width() + x - this->templateSubRegionBoundingBox_.left()) + n;

						for (unsigned int i = 0u; i < this->homographyParameters_; ++i)
						{
							ocean_assert(Numeric::isWeakEqual(intermediate(i), this->jacobianRows_[jacobianIndex * this->homographyParameters_ + i] * gradientNormalization));
						}

						intermediate *= channelError;

						debugJacobianError += intermediate.transposed();
					}
				}
			}
		}

		const Scalar subRegionSize = this->templateSubRegion_.size();
		const Scalar hessianErrorEpsilon = Scalar(1e-3) * subRegionSize;

		for (unsigned int n = 0u; n < this->homographyParameters_ * this->homographyParameters_; ++n)
		{
			const Scalar value = hessian(n);
			const Scalar debugValue = debugHessian(n);

			ocean_assert(Numeric::isEqual(value, debugValue, hessianErrorEpsilon));
		}

		/*const Scalar jacobianErrorEpsilon = Scalar(1e-6) * subRegionSize; // disabled until the mask is included

		for (unsigned int n = 0u; n < this->homographyParameters_; ++n)
		{
			const Scalar value = jacobianError(n);
			const Scalar debugValue = debugJacobianError(n);

			ocean_assert(Numeric::isEqual(value, debugValue, jacobianErrorEpsilon));
		}*/
	}

#endif // OCEAN_INTENSIVE_DEBUG

	return true;
}

template <unsigned int tChannels>
template <unsigned int tParameters>
bool HomographyImageAlignmentDense::DenseInverseCompositionalHomographyProvider<tChannels>::determineHessianAndJacobian()
{
	static_assert(tParameters == 8u || tParameters == 9u, "Invalid parameter number!");

	ocean_assert(this->templateSubRegionBoundingBox_.isValid());

	Frame gradientTemplateSubFrame(FrameType(this->templateSubRegionBoundingBox_.width(), this->templateSubRegionBoundingBox_.height(), FrameType::genericPixelFormat<int16_t, tChannels * 2u>(), FrameType::ORIGIN_UPPER_LEFT));
	CV::FrameFilterGradient::filterHorizontalVerticalSubFrame<uint8_t, int16_t, tChannels, false>(this->templateFrame_.template constdata<uint8_t>(), this->templateFrame_.width(), this->templateFrame_.height(), this->templateFrame_.paddingElements(), this->templateSubRegionBoundingBox_.left(), this->templateSubRegionBoundingBox_.top(), gradientTemplateSubFrame.data<int16_t>(), gradientTemplateSubFrame.width(), gradientTemplateSubFrame.height(), gradientTemplateSubFrame.paddingElements(), 1, WorkerPool::get().scopedWorker()());

	ocean_assert(gradientTemplateSubFrame.width() == this->templateSubRegionBoundingBox_.width() && gradientTemplateSubFrame.height() == this->templateSubRegionBoundingBox_.height());
	const int16_t* const gradientSubFrameData = gradientTemplateSubFrame.constdata<int16_t>();
	ocean_assert(gradientTemplateSubFrame.isContinuous());

#ifdef OCEAN_DEBUG
	Frame debugGradientTemplateFrame(FrameType(this->templateFrame_, FrameType::genericPixelFormat<int16_t, tChannels * 2u>(), FrameType::ORIGIN_UPPER_LEFT));
	CV::FrameFilterGradient::filterHorizontalVertical<uint8_t, int16_t, tChannels, false>(this->templateFrame_.template constdata<uint8_t>(), debugGradientTemplateFrame.data<int16_t>(), this->templateFrame_.width(), this->templateFrame_.height(), this->templateFrame_.paddingElements(), debugGradientTemplateFrame.paddingElements(), 1, WorkerPool::get().scopedWorker()());
#endif

	StaticMatrix<Scalar, tParameters, tParameters> hessian(false);
	Scalars localJacobianRows(tChannels * tParameters * this->templateSubRegionBoundingBox_.size());

	StaticMatrix<Scalar, 2, tParameters> homographyJacobian;
	StaticMatrix<Scalar, 1, 2> gradient;
	StaticMatrix<Scalar, 1, tParameters> intermediate;

	// **TODO** multicore-implementation, integrate homography mask

	unsigned int measurements = 0u;

	for (unsigned int y = this->templateSubRegionBoundingBox_.top(); y < this->templateSubRegionBoundingBox_.bottomEnd(); ++y)
	{
		for (unsigned int x = this->templateSubRegionBoundingBox_.left(); x < this->templateSubRegionBoundingBox_.rightEnd(); ++x)
		{
			if (this->templateSubRegion_.isInside(Vector2(Scalar(x), Scalar(y))))
			{
				if constexpr (tParameters == 8u)
				{
					Geometry::Jacobian::calculateIdentityHomographyJacobian2x8(homographyJacobian.template row<0>(), homographyJacobian.template row<1>(), Scalar(x), Scalar(y));
				}
				else
				{
					Geometry::Jacobian::calculateIdentityHomographyJacobian2x9(homographyJacobian.template row<0>(), homographyJacobian.template row<1>(), Scalar(x), Scalar(y));
				}

				const unsigned int index = tChannels * ((y - this->templateSubRegionBoundingBox_.top()) * this->templateSubRegionBoundingBox_.width() + x - this->templateSubRegionBoundingBox_.left());

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					ocean_assert(debugGradientTemplateFrame.constpixel<int16_t>(x, y)[n * 2u + 0u] == gradientSubFrameData[(index + n) * 2u + 0u]);
					ocean_assert(debugGradientTemplateFrame.constpixel<int16_t>(x, y)[n * 2u + 1u] == gradientSubFrameData[(index + n) * 2u + 1u]);

					gradient.element<0u, 0u>() = Scalar(gradientSubFrameData[(index + n) * 2u + 0u]);
					gradient.element<0u, 1u>() = Scalar(gradientSubFrameData[(index + n) * 2u + 1u]);

					gradient.multiply(homographyJacobian, intermediate);
					intermediate.multiplyWithTransposedLeftAndAdd(hessian);

					// missing the normalization 0.5 / 255, which will be done if the final error-Jacobian is determined
					memcpy(localJacobianRows.data() + (index + n) * tParameters, intermediate.data(), sizeof(Scalar) * tParameters);
				}

				measurements++;
			}
		}
	}

	if (measurements == 0u)
	{
		return false;
	}

	const Scalar hessianNormalization = Scalar(0.5 * 0.5) / Scalar(255 * 255);

	hessian_.resize(tParameters, tParameters);
	for (size_t n = 0; n < tParameters * tParameters; ++n)
	{
		hessian_(n) = hessian[n] * hessianNormalization;
	}

	jacobianRows_ = std::move(localJacobianRows);

	return true;
}

bool HomographyImageAlignmentDense::optimizeAlignmentInverseCompositional(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& roughHomography, const unsigned int homographyParameters, const bool zeroMean, SquareMatrix3& homography, const unsigned int iterations, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors, ConsistencyDataRef* externalConsistencyData, bool* abort)
{
	ocean_assert(templateFrame.pixelFormat() == currentFrame.pixelFormat());
	ocean_assert(templateFrame.pixelOrigin() == currentFrame.pixelOrigin());

	ocean_assert(templateFrame.numberPlanes() == 1u);
	ocean_assert(templateFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(templateSubRegion);

	if (templateFrame.pixelFormat() != currentFrame.pixelFormat() || templateFrame.numberPlanes() != 1u || templateFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	ocean_assert(iterations != 0u);
	ocean_assert(&roughHomography != &homography);

	homography = roughHomography;

	switch (FrameType::formatGenericNumberChannels(currentFrame.pixelFormat()))
	{
		case 1u:
		{
			DenseInverseCompositionalHomographyProvider<1u> provider(homography, templateFrame, templateSubRegion, currentFrame, homographyParameters, zeroMean, externalConsistencyData, abort);
			return Geometry::NonLinearOptimization::advancedDenseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case 2u:
		{
			DenseInverseCompositionalHomographyProvider<2u> provider(homography, templateFrame, templateSubRegion, currentFrame, homographyParameters, zeroMean, externalConsistencyData, abort);
			return Geometry::NonLinearOptimization::advancedDenseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case 3u:
		{
			DenseInverseCompositionalHomographyProvider<3u> provider(homography, templateFrame, templateSubRegion, currentFrame, homographyParameters, zeroMean, externalConsistencyData, abort);
			return Geometry::NonLinearOptimization::advancedDenseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case 4u:
		{
			DenseInverseCompositionalHomographyProvider<4u> provider(homography, templateFrame, templateSubRegion, currentFrame, homographyParameters, zeroMean, externalConsistencyData, abort);
			return Geometry::NonLinearOptimization::advancedDenseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool HomographyImageAlignmentDense::optimizeAlignmentMultiResolution(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const unsigned int numberPyramidLayers, const unsigned int homographyParameters, const bool additiveAlignment, const bool levenbergMarquardtOptimization, const bool zeroMean, const SquareMatrix3& roughHomography, SquareMatrix3& homography, const unsigned int coarseIterations, const unsigned int fineIterations, const CV::FramePyramid::DownsamplingMode downsamplingMode)
{
	ocean_assert(templateFrame.isValid() && currentFrame.isValid());
	ocean_assert(templateFrame.frameType() == currentFrame.frameType());
	ocean_assert(templateSubRegion);

	ocean_assert(numberPyramidLayers >= 1u);
	if (numberPyramidLayers == 0u)
	{
		return false;
	}

	ocean_assert(Numeric::isEqual(roughHomography[8], 1));
	ocean_assert(homographyParameters == 8u || homographyParameters == 9u);

	ocean_assert(coarseIterations >= 1u && fineIterations >= 1u);

	const CV::FramePyramid templateFramePyramid(templateFrame, downsamplingMode, numberPyramidLayers, false /*copyFirstLayer*/, WorkerPool::get().scopedWorker()());
	const CV::FramePyramid currentFramePyramid(currentFrame, downsamplingMode, numberPyramidLayers, false /*copyFirstLayer*/, WorkerPool::get().scopedWorker()());

	const unsigned int layers = min(templateFramePyramid.layers(), currentFramePyramid.layers());

	std::vector<CV::SubRegion> templateSubRegions;
	templateSubRegions.reserve(layers);

	for (unsigned int layerIndex = 0u; layerIndex < layers; ++layerIndex)
	{
		templateSubRegions.emplace_back(templateSubRegion * (Scalar(1) / Scalar(templateFramePyramid.sizeFactor((unsigned int)(layerIndex)))));
	}

	return optimizeAlignmentMultiResolution(templateFramePyramid, templateSubRegions, currentFramePyramid, layers, homographyParameters, additiveAlignment, levenbergMarquardtOptimization, zeroMean, roughHomography, homography, coarseIterations, fineIterations);
}

bool HomographyImageAlignmentDense::optimizeAlignmentMultiResolution(const CV::FramePyramid& templateFramePyramid, const std::vector<CV::SubRegion>& templateSubRegions, const CV::FramePyramid& currentFramePyramid, const unsigned int layers, const unsigned int homographyParameters, const bool additiveAlignment, const bool levenbergMarquardtOptimization, const bool zeroMean, const SquareMatrix3& roughHomography, SquareMatrix3& homography, const unsigned int coarseIterations, const unsigned int fineIterations, std::vector<ConsistencyDataRef>* consistencyDatas)
{
	ocean_assert(templateFramePyramid.frameType() == currentFramePyramid.frameType());
	ocean_assert(layers >= 1u);

	ocean_assert(templateFramePyramid.layers() >= layers);
	ocean_assert(currentFramePyramid.layers() >= layers);
	ocean_assert(templateSubRegions.size() >= layers);

	ocean_assert(Numeric::isEqual(roughHomography[8], 1));
	ocean_assert(homographyParameters == 8u || homographyParameters == 9u);

	ocean_assert(coarseIterations >= 1u && fineIterations >= 1u);

	const unsigned int coarsestLayerIndex = layers - 1u;

	const Scalar sizeFactor = Scalar(templateFramePyramid.sizeFactor(coarsestLayerIndex));
	const SquareMatrix3 scaleMatrix(Vector3(sizeFactor, sizeFactor, 1));

	// we have to calculate the previous homography for the coarsest layer
	SquareMatrix3 intermediateHomography = scaleMatrix.inverted() * roughHomography * scaleMatrix;
	ocean_assert(Numeric::isEqual(intermediateHomography[8], 1));

	const Scalar lambda = levenbergMarquardtOptimization ? Scalar(10) : Scalar(0);
	const Scalar lambdaFactor = levenbergMarquardtOptimization ? Scalar(10) : Scalar(1);

	if (consistencyDatas && consistencyDatas->size() < layers)
	{
		consistencyDatas->resize(layers);
	}

	for (int layerIndex = int(coarsestLayerIndex); layerIndex >= 0; --layerIndex)
	{
		const Frame& templateFrame = templateFramePyramid[layerIndex];
		const Frame& currentFrame = currentFramePyramid[layerIndex];

		const CV::SubRegion& templateSubRegion = templateSubRegions[layerIndex];

		ConsistencyDataRef* consistencyData = consistencyDatas ? &(*consistencyDatas)[layerIndex] : nullptr;

		const unsigned int iterations = max(1u, (layerIndex * coarseIterations + (coarsestLayerIndex - layerIndex) * fineIterations) / (coarsestLayerIndex));

		if (additiveAlignment)
		{
			if (!optimizeAlignmentAdditive(templateFrame, templateSubRegion, currentFrame, intermediateHomography, homographyParameters, zeroMean, homography, iterations, lambda, lambdaFactor, nullptr, nullptr, nullptr, consistencyData))
			{
				return false;
			}
		}
		else
		{
			if (!optimizeAlignmentInverseCompositional(templateFrame, templateSubRegion, currentFrame, intermediateHomography, homographyParameters, zeroMean, homography, iterations, lambda, lambdaFactor, nullptr, nullptr, nullptr, consistencyData))
			{
				return false;
			}
		}

		ocean_assert(Numeric::isEqual(homography[8], 1));

		if (layerIndex != 0)
		{
			// we have to transformed the determined homography for the next pyramid layer
			const SquareMatrix3 layerScaleMatrix(Vector3(2, 2, 1));
			intermediateHomography = layerScaleMatrix * homography * layerScaleMatrix.inverted();
		}
	}

	return true;
}

Scalar HomographyImageAlignmentDense::slowDetermineError(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& homographyCandidate, const bool zeroMean)
{
	ocean_assert(templateFrame.pixelFormat() == currentFrame.pixelFormat());
	ocean_assert(templateFrame.pixelOrigin() == currentFrame.pixelOrigin());

	ocean_assert(templateFrame.numberPlanes() == 1u);
	ocean_assert(templateFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(templateSubRegion);

	if (templateFrame.pixelFormat() != currentFrame.pixelFormat() || templateFrame.numberPlanes() != 1u || templateFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return Numeric::maxValue();
	}

	switch (FrameType::formatGenericNumberChannels(currentFrame.pixelFormat()))
	{
		case 1u:
			return slowDetermineError8BitPerChannel<1u>(templateFrame, templateSubRegion, currentFrame, homographyCandidate, zeroMean);

		case 2u:
			return slowDetermineError8BitPerChannel<2u>(templateFrame, templateSubRegion, currentFrame, homographyCandidate, zeroMean);

		case 3u:
			return slowDetermineError8BitPerChannel<3u>(templateFrame, templateSubRegion, currentFrame, homographyCandidate, zeroMean);

		case 4u:
			return slowDetermineError8BitPerChannel<4u>(templateFrame, templateSubRegion, currentFrame, homographyCandidate, zeroMean);
	}

	ocean_assert(false && "Invalid pixel format!");
	return Numeric::maxValue();
}

template <unsigned int tChannels>
Scalar HomographyImageAlignmentDense::slowDetermineError8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& homography, const bool zeroMean)
{
	static_assert(tChannels >= 1u && tChannels <= 4u, "Invalid channel number!");

	ocean_assert(FrameType::formatIsGeneric(templateFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, tChannels));
	ocean_assert(templateFrame.pixelFormat() == currentFrame.pixelFormat());
	ocean_assert(templateFrame.pixelOrigin() == currentFrame.pixelOrigin());
	ocean_assert(templateSubRegion);

	ocean_assert(Numeric::isEqual(homography[8], 1));

	const Box2 boundingBox = templateSubRegion.boundingBox();

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!boundingBox.box2integer(templateFrame.width(), templateFrame.height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		return Numeric::maxValue();
	}

	Frame transformedCurrentFrame(templateFrame.frameType());
	Frame transformedCurrentMask(FrameType(templateFrame, FrameType::FORMAT_Y8));

	if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(currentFrame, transformedCurrentFrame, transformedCurrentMask, homography, WorkerPool::get().scopedWorker()(), 0xFF))
	{
		return Numeric::maxValue();
	}

	const unsigned int transformedCurrentFrameStrideElements = transformedCurrentFrame.strideElements();
	const unsigned int transformedCurrentMaskStrideElements = transformedCurrentMask.strideElements();

	const uint8_t* const transformedCurrentData = transformedCurrentFrame.constdata<uint8_t>();
	const uint8_t* const transformedCurrentMaskData = transformedCurrentMask.constdata<uint8_t>();

	const unsigned int templateWidth = templateFrame.width();
	const unsigned int templateHeight = templateFrame.height();

	unsigned int measurements = 0u;
	Scalar sqrErrorSum = Scalar(0);

	constexpr Scalar inv255 = Scalar(1.0 / 255.0);

	if (zeroMean)
	{
		// first we determine the mean color values within the specified sub-region(s)

		Scalar transformedCurrentFrameMean[tChannels];
		Scalar templateFrameMean[tChannels];

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			transformedCurrentFrameMean[n] = Scalar(0);
			templateFrameMean[n] = Scalar(0);
		}

		unsigned int templateMeasurements = 0u;
		unsigned int currentMeasurements = 0u;

		for (unsigned int y = boundingBoxTop; y < boundingBoxTop + boundingBoxHeight; ++y)
		{
			for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
			{
				if (templateSubRegion.isInside(Vector2(Scalar(x), Scalar(y))))
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						templateFrameMean[n] += Scalar(templateFrame.constpixel<uint8_t>(x, y)[n]);
					}

					templateMeasurements++;

					// we need to ensure we have a valid pixel in the transformed current frame (that pixel must not lie outside the current frame)
					// we can ensure that a corresponding current pixel exists if: homography * (x, y)^T is inside current frame
					// however the Hessian determination needs the (bilinear) interpolated gradient value we have to ensure that our transformed pixel has a valid 2x2 neighborhood

					if (transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x] == 0xFFu
							&& (x == templateWidth - 1u || transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu)
							&& (y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x] == 0xFFu)
							&& (x == templateWidth - 1u || y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu))
					{
						ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							transformedCurrentFrameMean[n] += Scalar(transformedCurrentData[y * transformedCurrentFrameStrideElements + tChannels * x + n]);
						}

						currentMeasurements++;
					}
				}
			}
		}

		if (currentMeasurements == 0u)
		{
			return Numeric::maxValue();
		}

		ocean_assert(templateMeasurements != 0u);

		const Scalar invTemplateMeasurements = Scalar(1) / Scalar(templateMeasurements);

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			templateFrameMean[n] *= invTemplateMeasurements;
		}

		const Scalar invCurrentMeasurements = Scalar(1) / Scalar(currentMeasurements);

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			transformedCurrentFrameMean[n] *= invCurrentMeasurements;
		}

		// now we determine the error

		measurements = 0u;
		sqrErrorSum = Scalar(0);

		for (unsigned int y = boundingBoxTop; y < boundingBoxTop + boundingBoxHeight; ++y)
		{
			for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
			{
				if (templateSubRegion.isInside(Vector2(Scalar(x), Scalar(y))))
				{
					// checking the 2x2 pixels necessary for the interpolation of the gradient values
					if (transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x] == 0xFFu
							&& (x == templateWidth - 1u || transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu)
							&& (y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x] == 0xFFu)
							&& (x == templateWidth - 1u || y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu))
					{
						ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							const Scalar errorValue = (Scalar((transformedCurrentData[y * transformedCurrentFrameStrideElements + tChannels * x + n]) - transformedCurrentFrameMean[n])
													- (Scalar(templateFrame.constpixel<uint8_t>(x, y)[n]) - templateFrameMean[n])) * inv255;
							sqrErrorSum += errorValue * errorValue;
						}

						measurements++;
					}
				}
			}
		}
	}
	else
	{
		// we determine the error directly

		measurements = 0u;
		sqrErrorSum = Scalar(0);

		for (unsigned int y = boundingBoxTop; y < boundingBoxTop + boundingBoxHeight; ++y)
		{
			for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
			{
				if (templateSubRegion.isInside(Vector2(Scalar(x), Scalar(y))))
				{
					// checking the 2x2 pixels necessary for the interpolation of the gradient values
					if (transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x] == 0xFFu
							&& (x == templateWidth - 1u || transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu)
							&& (y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x] == 0xFFu)
							&& (x == templateWidth - 1u || y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu))
					{
						ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							const Scalar errorValue = (Scalar(transformedCurrentData[y * transformedCurrentFrameStrideElements + tChannels * x + n]) - Scalar(templateFrame.constpixel<uint8_t>(x, y)[n])) * inv255;
							sqrErrorSum += errorValue * errorValue;
						}

						measurements++;
					}
				}
			}
		}
	}

	if (measurements == 0u)
	{
		return Numeric::maxValue();
	}

	return sqrErrorSum / Scalar(measurements * tChannels);
}

template <unsigned int tParameters>
inline bool HomographyImageAlignmentDense::slowDetermineHessianAndErrorJacobian(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& homography, const bool zeroMean, Matrix& hessian, Matrix& jacobianError)
{
	static_assert(tParameters == 8u || tParameters == 9u, "Invalid parameter number!");

	ocean_assert(templateFrame.pixelFormat() == currentFrame.pixelFormat());
	ocean_assert(templateFrame.pixelOrigin() == currentFrame.pixelOrigin());

	ocean_assert(templateFrame.numberPlanes() == 1u);
	ocean_assert(templateFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(templateSubRegion);

	if (templateFrame.pixelFormat() != currentFrame.pixelFormat() || templateFrame.numberPlanes() != 1u || templateFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	switch (FrameType::formatGenericNumberChannels(currentFrame.pixelFormat()))
	{
		case 1u:
			return slowDetermineHessianAndErrorJacobian8BitPerChannel<tParameters, 1u>(templateFrame, templateSubRegion, currentFrame, homography, zeroMean, hessian, jacobianError);

		case 2u:
			return slowDetermineHessianAndErrorJacobian8BitPerChannel<tParameters, 2u>(templateFrame, templateSubRegion, currentFrame, homography, zeroMean, hessian, jacobianError);

		case 3u:
			return slowDetermineHessianAndErrorJacobian8BitPerChannel<tParameters, 3u>(templateFrame, templateSubRegion, currentFrame, homography, zeroMean, hessian, jacobianError);

		case 4u:
			return slowDetermineHessianAndErrorJacobian8BitPerChannel<tParameters, 4u>(templateFrame, templateSubRegion, currentFrame, homography, zeroMean, hessian, jacobianError);
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

template <unsigned int tParameters, unsigned int tChannels>
bool HomographyImageAlignmentDense::slowDetermineHessianAndErrorJacobian8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& homography, const bool zeroMean, Matrix& hessian, Matrix& jacobianError)
{
	static_assert(tParameters == 8u || tParameters == 9u, "Invalid parameter number!");
	static_assert(tChannels >= 1u && tChannels <= 4u, "Invalid channel number!");

	ocean_assert(FrameType::formatIsGeneric(templateFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, tChannels));
	ocean_assert(templateFrame.pixelFormat() == currentFrame.pixelFormat());
	ocean_assert(templateFrame.pixelOrigin() == currentFrame.pixelOrigin());
	ocean_assert(templateSubRegion);

	ocean_assert(Numeric::isEqual(homography[8], 1));

	const Box2 boundingBox = templateSubRegion.boundingBox();

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!boundingBox.box2integer(templateFrame.width(), templateFrame.height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		return false;
	}

	Frame transformedCurrentFrame(templateFrame.frameType());
	Frame transformedCurrentMask(FrameType(templateFrame, FrameType::FORMAT_Y8));

	if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(currentFrame, transformedCurrentFrame, transformedCurrentMask, homography, WorkerPool::get().scopedWorker()(), 0xFF))
	{
		return false;
	}

	const unsigned int transformedCurrentFrameStrideElements = transformedCurrentFrame.strideElements();
	const unsigned int transformedCurrentMaskStrideElements = transformedCurrentMask.strideElements();

	const uint8_t* const transformedCurrentData = transformedCurrentFrame.constdata<uint8_t>();
	const uint8_t* const transformedCurrentMaskData = transformedCurrentMask.constdata<uint8_t>();

	const unsigned int templateWidth = templateFrame.width();
	const unsigned int templateHeight = templateFrame.height();

	const unsigned int currentWidth = currentFrame.width();
	const unsigned int currentHeight = currentFrame.height();

	constexpr unsigned int gradientCurrentPaddingElements = 0u;

	Frame gradientCurrentFrame(FrameType(currentFrame, FrameType::genericPixelFormat<Scalar, tChannels * 2u>()), gradientCurrentPaddingElements);
	CV::FrameFilterGradient::filterHorizontalVertical<uint8_t, Scalar, tChannels, true>(currentFrame.constdata<uint8_t>(), gradientCurrentFrame.data<Scalar>(), currentFrame.width(), currentFrame.height(), currentFrame.paddingElements(), gradientCurrentFrame.paddingElements(), Scalar(1.0 / 255.0), WorkerPool::get().scopedWorker()());

	const Scalar* const gradientCurrentData = gradientCurrentFrame.constdata<Scalar>();

	StaticMatrix<Scalar, 1, 2> gradient;
	StaticMatrix<Scalar, 2, tParameters> localJacobian;
	StaticMatrix<Scalar, 1, tParameters> intermediate;

	StaticMatrix<Scalar, tParameters, tParameters> localHessian(false);
	StaticMatrix<Scalar, 1, tParameters> transposedJacobianError(false);

	const Scalar inv255 = Scalar(1.0 / 255.0);

	if (zeroMean)
	{
		// first we determine the mean color values within the specified sub-region(s)

		unsigned int templateMeasurements = 0u;
		unsigned int currentMeasurements = 0u;

		Scalar transformedCurrentFrameMean[tChannels];
		Scalar templateFrameMean[tChannels];

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			transformedCurrentFrameMean[n] = Scalar(0);
			templateFrameMean[n] = Scalar(0);
		}

		for (unsigned int y = boundingBoxTop; y < boundingBoxTop + boundingBoxHeight; ++y)
		{
			for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
			{
				if (templateSubRegion.isInside(Vector2(Scalar(x), Scalar(y))))
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						templateFrameMean[n] += Scalar(templateFrame.constpixel<uint8_t>(x, y)[n]);
					}

					templateMeasurements++;

					// checking the 2x2 pixels necessary for the interpolation of the gradient values
					if (transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x] == 0xFFu
							&& (x == templateWidth - 1u || transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu)
							&& (y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x] == 0xFFu)
							&& (x == templateWidth - 1u || y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu))
					{
						ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							transformedCurrentFrameMean[n] += Scalar(transformedCurrentData[y * transformedCurrentFrameStrideElements + tChannels * x + n]);
						}

						currentMeasurements++;
					}
				}
			}
		}

		if (currentMeasurements == 0u)
		{
			return false;
		}

		ocean_assert(templateMeasurements != 0u);

		const Scalar invTemplateMeasurements = Scalar(1) / Scalar(templateMeasurements);
		const Scalar invCurrentMeasurements = Scalar(1) / Scalar(currentMeasurements);

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			transformedCurrentFrameMean[n] *= invCurrentMeasurements;
			templateFrameMean[n] *= invTemplateMeasurements;
		}

		// now we determine the jacobian(s), the Hessian and the error(s)

		Scalar interpolatedGradientValues[tChannels * 2u];

		for (unsigned int y = boundingBoxTop; y < boundingBoxTop + boundingBoxHeight; ++y)
		{
			for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
			{
				if (templateSubRegion.isInside(Vector2(Scalar(x), Scalar(y))))
				{
					// checking the 2x2 pixels necessary for the interpolation of the gradient values
					if (transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x] == 0xFFu
							&& (x == templateWidth - 1u || transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu)
							&& (y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x] == 0xFFu)
							&& (x == templateWidth - 1u || y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu))
					{
						const Vector2 transformedPoint(homography * Vector2(Scalar(x), Scalar(y)));

						CV::FrameInterpolatorBilinear::interpolatePixel<Scalar, Scalar, tChannels * 2u>(gradientCurrentData, currentWidth, currentHeight, gradientCurrentPaddingElements, transformedPoint, interpolatedGradientValues);

						if constexpr (tParameters == 8u)
						{
							Geometry::Jacobian::calculateHomographyJacobian2x8(localJacobian.template row<0>(), localJacobian.template row<1>(), Scalar(x), Scalar(y), homography);
						}
						else
						{
							Geometry::Jacobian::calculateHomographyJacobian2x9(localJacobian.template row<0>(), localJacobian.template row<1>(), Scalar(x), Scalar(y), homography);
						}

						for (unsigned int n = 0u; n < tChannels; n++)
						{
							gradient.element<0u, 0u>() = interpolatedGradientValues[n * 2u + 0u];
							gradient.element<0u, 1u>() = interpolatedGradientValues[n * 2u + 1u];

							intermediate = gradient * localJacobian;
							intermediate.multiplyWithTransposedLeftAndAdd(localHessian);

							ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

							const Scalar errorValue = (Scalar((transformedCurrentData[y * transformedCurrentFrameStrideElements + tChannels * x + n]) - transformedCurrentFrameMean[n])
														- (Scalar(templateFrame.constpixel<uint8_t>(x, y)[n]) - templateFrameMean[n])) * inv255;

							transposedJacobianError += intermediate * errorValue;
						}
					}
				}
			}
		}
	}
	else
	{
		// now we determine the jacobian(s), the Hessian and the error(s)

		Scalar interpolatedGradientValues[tChannels * 2u];

		for (unsigned int y = boundingBoxTop; y < boundingBoxTop + boundingBoxHeight; ++y)
		{
			for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
			{
				if (templateSubRegion.isInside(Vector2(Scalar(x), Scalar(y))))
				{
					// checking the 2x2 pixels necessary for the interpolation of the gradient values
					if (transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x] == 0xFFu
							&& (x == templateWidth - 1u || transformedCurrentMaskData[y * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu)
							&& (y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x] == 0xFFu)
							&& (x == templateWidth - 1u || y == templateHeight - 1u || transformedCurrentMaskData[(y + 1u) * transformedCurrentMaskStrideElements + x + 1u] == 0xFFu))
					{
						const Vector2 transformedPoint(homography * Vector2(Scalar(x), Scalar(y)));

						CV::FrameInterpolatorBilinear::interpolatePixel<Scalar, Scalar, tChannels * 2u>(gradientCurrentData, currentWidth, currentHeight, gradientCurrentPaddingElements, transformedPoint, interpolatedGradientValues);

						if constexpr (tParameters == 8u)
						{
							Geometry::Jacobian::calculateHomographyJacobian2x8(localJacobian.template row<0>(), localJacobian.template row<1>(), Scalar(x), Scalar(y), homography);
						}
						else
						{
							Geometry::Jacobian::calculateHomographyJacobian2x9(localJacobian.template row<0>(), localJacobian.template row<1>(), Scalar(x), Scalar(y), homography);
						}

						for (unsigned int n = 0u; n < tChannels; n++)
						{
							gradient.element<0u, 0u>() = interpolatedGradientValues[n * 2u + 0u];
							gradient.element<0u, 1u>() = interpolatedGradientValues[n * 2u + 1u];

							intermediate = gradient * localJacobian;
							intermediate.multiplyWithTransposedLeftAndAdd(localHessian);

							ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

							const Scalar errorValue = (Scalar(transformedCurrentData[y * transformedCurrentFrameStrideElements + tChannels * x + n]) - Scalar(templateFrame.constpixel<uint8_t>(x, y)[n])) * inv255;

							transposedJacobianError += intermediate * errorValue;
						}
					}
				}
			}
		}
	}

	hessian.resize(tParameters, tParameters);
	ocean_assert(hessian.rows() == localHessian.rows() && hessian.columns() == localHessian.columns());
	memcpy(hessian.data(), localHessian.data(), hessian.elements() * sizeof(Scalar));

	jacobianError.resize(tParameters, 1);
	ocean_assert(jacobianError.rows() == transposedJacobianError.columns() && jacobianError.columns() == transposedJacobianError.rows());
	memcpy(jacobianError.data(), transposedJacobianError.data(), jacobianError.elements() * sizeof(Scalar));

	return true;
}

template <unsigned int tChannels, bool tDetermineTemplateMeans>
void HomographyImageAlignmentDense::determineMeans8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, Scalar* templateMeans, Scalar* currentMeans, unsigned int* templateMeansDenominator, unsigned int* currentMeansDenominator, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused)
{
	ocean_assert(templateFrame != nullptr && templateFrame->isValid() && templateFrame->dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && templateFrame->channels() == tChannels);

	ocean_assert(templateSubRegion != nullptr && transformedCurrentSubFrame != nullptr && currentMeansDenominator);
	ocean_assert(currentMeans != nullptr && (!tDetermineTemplateMeans || (templateMeans != nullptr && templateMeansDenominator != nullptr)));

	ocean_assert(threadIndex < threads);
	ocean_assert_and_suppress_unused(unused == 1u, unused);

	const Box2 boundingBox = templateSubRegion->boundingBox();

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!boundingBox.box2integer(templateFrame->width(), templateFrame->height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		ocean_assert(false && "This must never happen!");
	}

	ocean_assert(transformedCurrentSubFrame->width() == boundingBoxWidth && transformedCurrentSubFrame->height() == boundingBoxHeight);

	const uint8_t* transformedCurrentSubFrameData = transformedCurrentSubFrame->constdata<uint8_t>();

	const unsigned int transformedCurrentSubFrameStrideElements = transformedCurrentSubFrame->strideElements();

	if constexpr (tDetermineTemplateMeans)
	{
		unsigned int localMeasurements = 0u;

		uint64_t transformedCurrentFrameMeanI[tChannels] = {0u};
		uint64_t providerTemplateFrameMeanI[tChannels] = {0u};

		for (unsigned int y = boundingBoxTop + threadIndex; y < boundingBoxTop + boundingBoxHeight; y += threads)
		{
			const uint8_t* templatePixel = templateFrame->constpixel<uint8_t>(boundingBoxLeft, y);

			for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
			{
				if (templateSubRegion->isInside(Vector2(Scalar(x), Scalar(y))))
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						providerTemplateFrameMeanI[n] += templatePixel[n];
					}

					ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

					const uint8_t* transformedCurrentSubFramePixel = transformedCurrentSubFrameData + (y - boundingBoxTop) * transformedCurrentSubFrameStrideElements + (x - boundingBoxLeft);

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						transformedCurrentFrameMeanI[n] += transformedCurrentSubFramePixel[n];
					}

					localMeasurements++;
				}

				templatePixel += tChannels;
			}
		}

		const OptionalScopedLock scopedLock(lock);

		*templateMeansDenominator += localMeasurements;
		*currentMeansDenominator += localMeasurements;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			currentMeans[n] += Scalar(transformedCurrentFrameMeanI[n]);
			templateMeans[n] += Scalar(providerTemplateFrameMeanI[n]);
		}
	}
	else
	{
		unsigned int localMeasurements = 0u;

		uint64_t transformedCurrentFrameMeanI[tChannels] = {0u};

		for (unsigned int y = boundingBoxTop + threadIndex; y < boundingBoxTop + boundingBoxHeight; y += threads)
		{
			for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
			{
				if (templateSubRegion->isInside(Vector2(Scalar(x), Scalar(y))))
				{
					ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

					const uint8_t* transformedCurrentSubFramePixel = transformedCurrentSubFrameData + (y - boundingBoxTop) * transformedCurrentSubFrameStrideElements + (x - boundingBoxLeft);

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						transformedCurrentFrameMeanI[n] += transformedCurrentSubFramePixel[n];
					}

					localMeasurements++;
				}
			}
		}

		const OptionalScopedLock scopedLock(lock);

		*currentMeansDenominator += localMeasurements;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			currentMeans[n] += Scalar(transformedCurrentFrameMeanI[n]);
		}
	}
}

template <unsigned int tChannels, bool tDetermineTemplateMeans>
void HomographyImageAlignmentDense::determineMeansMask8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const Frame* transformedCurrentSubFrameMask, Scalar* templateMeans, Scalar* currentMeans, unsigned int* templateMeansDenominator, unsigned int* currentMeansDenominator, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused)
{
	ocean_assert(templateFrame != nullptr && templateFrame->isValid() && templateFrame->dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && templateFrame->channels() == tChannels);

	ocean_assert(templateSubRegion != nullptr && transformedCurrentSubFrame != nullptr && transformedCurrentSubFrameMask != nullptr && currentMeansDenominator != nullptr);
	ocean_assert(currentMeans != nullptr && (!tDetermineTemplateMeans || (templateMeans != nullptr && templateMeansDenominator != nullptr)));

	ocean_assert(threadIndex < threads);
	ocean_assert_and_suppress_unused(unused == 1u, unused);

	const Box2 boundingBox = templateSubRegion->boundingBox();

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!boundingBox.box2integer(templateFrame->width(), templateFrame->height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		ocean_assert(false && "This must never happen!");
	}

	ocean_assert(transformedCurrentSubFrame->width() == boundingBoxWidth && transformedCurrentSubFrame->height() == boundingBoxHeight);

	const uint8_t* transformedCurrentSubFrameData = transformedCurrentSubFrame->constdata<uint8_t>();
	const uint8_t* transformedCurrentSubFrameMaskData = transformedCurrentSubFrameMask->constdata<uint8_t>();

	const unsigned int transformedCurrentSubFrameStrideElements = transformedCurrentSubFrame->strideElements();
	const unsigned int transformedCurrentSubFrameMaskStrideElements = transformedCurrentSubFrameMask->strideElements();

	if constexpr (tDetermineTemplateMeans)
	{
		unsigned int localTemplateMeasurements = 0u;
		unsigned int localCurrentMeasurements = 0u;

		uint64_t transformedCurrentFrameMeanI[tChannels] = {0u};
		uint64_t providerTemplateFrameMeanI[tChannels] = {0u};

		for (unsigned int y = boundingBoxTop + threadIndex; y < boundingBoxTop + boundingBoxHeight; y += threads)
		{
			const uint8_t* templatePixel = templateFrame->constpixel<uint8_t>(boundingBoxLeft, y);

			for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
			{
				if (templateSubRegion->isInside(Vector2(Scalar(x), Scalar(y))))
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						providerTemplateFrameMeanI[n] += templatePixel[n];
					}

					localTemplateMeasurements++;

					const uint8_t* transformedCurrentSubFrameMaskTopLeft = transformedCurrentSubFrameMaskData + (y - boundingBoxTop) * transformedCurrentSubFrameMaskStrideElements + (x - boundingBoxLeft);

					// checking the 2x2 pixels necessary for the interpolation of the gradient values
					if (transformedCurrentSubFrameMaskTopLeft[0]
							&& (x == boundingBoxLeft + boundingBoxWidth - 1u || transformedCurrentSubFrameMaskTopLeft[1])
							&& (y == boundingBoxTop + boundingBoxHeight - 1u || transformedCurrentSubFrameMaskTopLeft[transformedCurrentSubFrameMaskStrideElements])
							&& (x == boundingBoxLeft + boundingBoxWidth - 1u || y == boundingBoxTop + boundingBoxHeight - 1u || transformedCurrentSubFrameMaskTopLeft[transformedCurrentSubFrameMaskStrideElements + 1u]))
					{
						ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

						const uint8_t* transformedCurrentSubFrameTopLeft = transformedCurrentSubFrameData + (y - boundingBoxTop) * transformedCurrentSubFrameStrideElements + (x - boundingBoxLeft) * tChannels;

						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							transformedCurrentFrameMeanI[n] += transformedCurrentSubFrameTopLeft[n];
						}

						localCurrentMeasurements++;
					}
				}

				templatePixel += tChannels;
			}
		}

		const OptionalScopedLock scopedLock(lock);

		*templateMeansDenominator += localTemplateMeasurements;
		*currentMeansDenominator += localCurrentMeasurements;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			currentMeans[n] += Scalar(transformedCurrentFrameMeanI[n]);
			templateMeans[n] += Scalar(providerTemplateFrameMeanI[n]);
		}
	}
	else
	{
		unsigned int localMeasurements = 0u;

		uint64_t transformedCurrentFrameMeanI[tChannels] = {0u};

		for (unsigned int y = boundingBoxTop + threadIndex; y < boundingBoxTop + boundingBoxHeight; y += threads)
		{
			for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
			{
				if (templateSubRegion->isInside(Vector2(Scalar(x), Scalar(y))))
				{
					const uint8_t* transformedCurrentSubFrameMaskTopLeft = transformedCurrentSubFrameMaskData + (y - boundingBoxTop) * transformedCurrentSubFrameMaskStrideElements + (x - boundingBoxLeft);

					// checking the 2x2 pixels necessary for the interpolation of the gradient values
					if (transformedCurrentSubFrameMaskTopLeft[0]
							&& (x == boundingBoxLeft + boundingBoxWidth - 1u || transformedCurrentSubFrameMaskTopLeft[1])
							&& (y == boundingBoxTop + boundingBoxHeight - 1u || transformedCurrentSubFrameMaskTopLeft[transformedCurrentSubFrameMaskStrideElements])
							&& (x == boundingBoxLeft + boundingBoxWidth - 1u || y == boundingBoxTop + boundingBoxHeight - 1u || transformedCurrentSubFrameMaskTopLeft[transformedCurrentSubFrameMaskStrideElements + 1u]))
					{
						ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

						const uint8_t* transformedCurrentSubFrameTopLeft = transformedCurrentSubFrameData + (y - boundingBoxTop) * transformedCurrentSubFrameStrideElements + (x - boundingBoxLeft) * tChannels;

						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							transformedCurrentFrameMeanI[n] += transformedCurrentSubFrameTopLeft[n];
						}

						localMeasurements++;
					}
				}
			}
		}

		const OptionalScopedLock scopedLock(lock);

		*currentMeansDenominator += localMeasurements;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			currentMeans[n] += Scalar(transformedCurrentFrameMeanI[n]);
		}
	}
}

template <unsigned int tChannels, bool tUseMeans>
void HomographyImageAlignmentDense::determineError8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const Scalar* templateMeans, const Scalar* currentMeans, Scalar* squaredError, unsigned int* errorDenominator, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused)
{
	ocean_assert(templateFrame != nullptr && templateFrame->isValid() && templateFrame->dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && templateFrame->channels() == tChannels);

	ocean_assert(templateSubRegion != nullptr && transformedCurrentSubFrame != nullptr && squaredError != nullptr && errorDenominator != nullptr);
	ocean_assert(!tUseMeans || (templateMeans != nullptr && currentMeans != nullptr));

	ocean_assert(threadIndex < threads);
	ocean_assert_and_suppress_unused(unused == 1u, unused);

	const Box2 boundingBox = templateSubRegion->boundingBox();

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!boundingBox.box2integer(templateFrame->width(), templateFrame->height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		ocean_assert(false && "This must never happen!");
	}

	ocean_assert(transformedCurrentSubFrame->width() == boundingBoxWidth && transformedCurrentSubFrame->height() == boundingBoxHeight);

	const uint8_t* transformedCurrentSubFrameData = transformedCurrentSubFrame->constdata<uint8_t>();

	const unsigned int transformedCurrentSubFrameStrideElements = transformedCurrentSubFrame->strideElements();

	unsigned int localMeasurements = 0u;

	Scalar localSqrErrorSum = Scalar(0);
	uint64_t localSqrErrorSumI = 0u;

	for (unsigned int y = boundingBoxTop + threadIndex; y < boundingBoxTop + boundingBoxHeight; y += threads)
	{
		const uint8_t* templatePixel = templateFrame->constpixel<uint8_t>(boundingBoxLeft, y);

		for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
		{
			if (templateSubRegion->isInside(Vector2(Scalar(x), Scalar(y))))
			{
				ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

				const uint8_t* transformedCurrentSubFramePixel = transformedCurrentSubFrameData + (y - boundingBoxTop) * transformedCurrentSubFrameStrideElements + (x - boundingBoxLeft) * tChannels;

				if constexpr (tUseMeans)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						const Scalar channelError = (Scalar(transformedCurrentSubFramePixel[n]) - currentMeans[n]) - (Scalar(templatePixel[n]) - templateMeans[n]);

						localSqrErrorSum += channelError * channelError;
					}
				}
				else
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						const int channelError = int(transformedCurrentSubFramePixel[n]) - int(templatePixel[n]);

						localSqrErrorSumI += channelError * channelError;
					}
				}

				++localMeasurements;
			}

			templatePixel += tChannels;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	if constexpr (tUseMeans)
	{
		*squaredError += localSqrErrorSum / Scalar(255 * 255);
	}
	else
	{
		*squaredError += Scalar(localSqrErrorSumI) / Scalar(255 * 255);
	}

	*errorDenominator += localMeasurements;
}

template <unsigned int tChannels, bool tUseMeans>
void HomographyImageAlignmentDense::determineErrorMask8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const Frame* transformedCurrentSubFrameMask, const Scalar* templateMeans, const Scalar* currentMeans, Scalar* squaredError, unsigned int* errorDenominator, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused)
{
	ocean_assert(templateFrame != nullptr && templateFrame->isValid() && templateFrame->dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && templateFrame->channels() == tChannels);

	ocean_assert(templateSubRegion != nullptr && transformedCurrentSubFrame != nullptr && transformedCurrentSubFrameMask != nullptr && squaredError != nullptr && errorDenominator != nullptr);
	ocean_assert(!tUseMeans || (templateMeans != nullptr && currentMeans != nullptr));

	ocean_assert(threadIndex < threads);
	ocean_assert_and_suppress_unused(unused == 1u, unused);

	const Box2 boundingBox = templateSubRegion->boundingBox();

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!boundingBox.box2integer(templateFrame->width(), templateFrame->height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		ocean_assert(false && "This must never happen!");
	}

	ocean_assert(transformedCurrentSubFrame->width() == boundingBoxWidth && transformedCurrentSubFrame->height() == boundingBoxHeight);

	const uint8_t* transformedCurrentSubFrameData = transformedCurrentSubFrame->constdata<uint8_t>();
	const uint8_t* transformedCurrentSubFrameMaskData = transformedCurrentSubFrameMask->constdata<uint8_t>();

	const unsigned int transformedCurrentSubFrameStrideElements = transformedCurrentSubFrame->strideElements();
	const unsigned int transformedCurrentSubFrameMaskStrideElements = transformedCurrentSubFrameMask->strideElements();

	unsigned int localMeasurements = 0u;

	Scalar localSqrErrorSum = Scalar(0);
	uint64_t localSqrErrorSumI = 0u;

	for (unsigned int y = boundingBoxTop + threadIndex; y < boundingBoxTop + boundingBoxHeight; y += threads)
	{
		const uint8_t* templatePixel = templateFrame->constpixel<uint8_t>(boundingBoxLeft, y);

		for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
		{
			if (templateSubRegion->isInside(Vector2(Scalar(x), Scalar(y))))
			{
				ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

				const uint8_t* transformedCurrentSubFrameMaskTopLeft = transformedCurrentSubFrameMaskData + (y - boundingBoxTop) * transformedCurrentSubFrameMaskStrideElements + (x - boundingBoxLeft);

				// checking the 2x2 pixels necessary for the interpolation of the gradient values
				if (transformedCurrentSubFrameMaskTopLeft[0]
						&& (x == boundingBoxLeft + boundingBoxWidth - 1u || transformedCurrentSubFrameMaskTopLeft[1])
						&& (y == boundingBoxTop + boundingBoxHeight - 1u || transformedCurrentSubFrameMaskTopLeft[transformedCurrentSubFrameMaskStrideElements])
						&& (x == boundingBoxLeft + boundingBoxWidth - 1u || y == boundingBoxTop + boundingBoxHeight - 1u || transformedCurrentSubFrameMaskTopLeft[transformedCurrentSubFrameMaskStrideElements + 1u]))
				{
					const uint8_t* transformedCurrentSubFrameTopLeft = transformedCurrentSubFrameData + (y - boundingBoxTop) * transformedCurrentSubFrameStrideElements + (x - boundingBoxLeft) * tChannels;

					if constexpr (tUseMeans)
					{
						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							const Scalar channelError = (Scalar(transformedCurrentSubFrameTopLeft[n]) - currentMeans[n]) - (Scalar(templatePixel[n]) - templateMeans[n]);

							localSqrErrorSum += channelError * channelError;
						}
					}
					else
					{
						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							const int channelError = int(transformedCurrentSubFrameTopLeft[n]) - int(templatePixel[n]);

							localSqrErrorSumI += channelError * channelError;
						}
					}

					localMeasurements++;
				}
			}

			templatePixel += tChannels;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	if constexpr (tUseMeans)
	{
		*squaredError += localSqrErrorSum / Scalar(255 * 255);
	}
	else
	{
		*squaredError += Scalar(localSqrErrorSumI) / Scalar(255 * 255);
	}

	*errorDenominator += localMeasurements;
}

template <unsigned int tParameters, unsigned int tChannels, bool tUseMeans>
void HomographyImageAlignmentDense::determineHessianAndErrorJacobian8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const unsigned int transformedBoundingBoxLeft, const unsigned int transformedBoundingBoxTop, const unsigned int transformedBoundingBoxWidth, const unsigned int transformedBoundingBoxHeight, const Frame* gradientCurrentFrame, const SquareMatrix3* homography, const Scalar* templateMeans, const Scalar* currentMeans, Matrix* hessian, Matrix* jacobianError, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused)
{
	static_assert(tParameters == 8u || tParameters == 9u, "Invalid parameter number!");

	ocean_assert(templateFrame != nullptr && templateFrame->isValid() && templateFrame->dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && templateFrame->channels() == tChannels);

	ocean_assert(templateSubRegion != nullptr && transformedCurrentSubFrame != nullptr && gradientCurrentFrame != nullptr && homography != nullptr && hessian != nullptr && jacobianError != nullptr);
	ocean_assert(!tUseMeans || (templateMeans != nullptr && currentMeans != nullptr));

	ocean_assert(Numeric::isEqual((*homography)[8], 1));

	ocean_assert(threadIndex < threads);
	ocean_assert_and_suppress_unused(unused == 1u, unused);

	const Box2 boundingBox = templateSubRegion->boundingBox();

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!boundingBox.box2integer(templateFrame->width(), templateFrame->height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		ocean_assert(false && "This must never happen!");
	}

	ocean_assert(transformedCurrentSubFrame->width() == boundingBoxWidth && transformedCurrentSubFrame->height() == boundingBoxHeight);

	const uint8_t* transformedCurrentSubFrameData = transformedCurrentSubFrame->constdata<uint8_t>();
	const unsigned int transformedCurrentSubFrameStrideElements = transformedCurrentSubFrame->strideElements();

	const int16_t* gradientCurrentData = gradientCurrentFrame->constdata<int16_t>();
	const unsigned int gradientCurrentPaddingElements = gradientCurrentFrame->paddingElements();

	StaticMatrix<Scalar, 1, 2> gradient;
	StaticMatrix<Scalar, 2, tParameters> localJacobian;
	StaticMatrix<Scalar, 1, tParameters> intermediate;

	StaticMatrix<Scalar, tParameters, tParameters> localHessian(false);
	StaticMatrix<Scalar, 1, tParameters> transposedJacobianError(false);

	Scalar interpolatedGradientData[tChannels * 2u];

	for (unsigned int y = boundingBoxTop + threadIndex; y < boundingBoxTop + boundingBoxHeight; y += threads)
	{
		const uint8_t* templatePixel = templateFrame->constpixel<uint8_t>(boundingBoxLeft, y);

		for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
		{
			if (templateSubRegion->isInside(Vector2(Scalar(x), Scalar(y))))
			{
				const Vector2 transformedPoint((*homography) * Vector2(Scalar(x), Scalar(y)));

				CV::FrameInterpolatorBilinear::interpolatePixel<int16_t, Scalar, tChannels * 2u>(gradientCurrentData, transformedBoundingBoxWidth, transformedBoundingBoxHeight, gradientCurrentPaddingElements, Vector2(transformedPoint.x() - Scalar(transformedBoundingBoxLeft), transformedPoint.y() - Scalar(transformedBoundingBoxTop)), interpolatedGradientData);

				if constexpr (tParameters == 8u)
				{
					Geometry::Jacobian::calculateHomographyJacobian2x8(localJacobian.template row<0>(), localJacobian.template row<1>(), Scalar(x), Scalar(y), *homography);
				}
				else
				{
					Geometry::Jacobian::calculateHomographyJacobian2x9(localJacobian.template row<0>(), localJacobian.template row<1>(), Scalar(x), Scalar(y), *homography);
				}

				const uint8_t* transformedCurrentSubFrameTopLeft = transformedCurrentSubFrameData + (y - boundingBoxTop) * transformedCurrentSubFrameStrideElements + (x - boundingBoxLeft) * tChannels;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					// normally, we would have to normalize the gradients by (0.5 / 255), however we normalize the hessian and jacobian vector at the end as this is simply a scalar factor
					gradient.element<0, 0>() = Scalar(interpolatedGradientData[2u * n + 0u]);
					gradient.element<0, 1>() = Scalar(interpolatedGradientData[2u * n + 1u]);

					intermediate = gradient * localJacobian;
					intermediate.multiplyWithTransposedLeftAndAdd(localHessian);

					ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

					if constexpr (tUseMeans)
					{
						// we also normalize (by 1/255) the error at the end as it is a simple scalar factor
						const Scalar channelError = (Scalar(transformedCurrentSubFrameTopLeft[n]) - currentMeans[n]) - (Scalar(templatePixel[n]) - templateMeans[n]);

						transposedJacobianError += intermediate * channelError;
					}
					else
					{
						// we also normalize (by 1/255) the error at the end as it is a simple scalar factor
						const Scalar channelError = Scalar(int(transformedCurrentSubFrameTopLeft[n]) - int(templatePixel[n]));

						transposedJacobianError += intermediate * channelError;
					}
				}
			}

			templatePixel += tChannels;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	ocean_assert(hessian->rows() == localHessian.rows() && hessian->columns() == localHessian.columns());
	ocean_assert(jacobianError->rows() == transposedJacobianError.columns() && jacobianError->columns() == transposedJacobianError.rows());

	const Scalar hessianNormalization = Scalar(0.5 * 0.5) / Scalar(255 * 255); // normalization due to the un-normalized integer gradient

	for (unsigned int n = 0u; n < tParameters * tParameters; ++n)
	{
		hessian->data()[n] += localHessian.data()[n] * hessianNormalization;
	}

	const Scalar jacobianNormalization = Scalar(0.5) / Scalar(255 * 255); // normalization due to the integer gradient (0.5/255) and normalization due to the error (1/255)

	for (unsigned int n = 0u; n < tParameters; ++n)
	{
		jacobianError->data()[n] += transposedJacobianError.data()[n] * jacobianNormalization;
	}
}

template <unsigned int tParameters, unsigned int tChannels, bool tUseMeans>
void HomographyImageAlignmentDense::determineHessianAndErrorJacobianMask8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const Frame* transformedCurrentSubFrameMask, const unsigned int transformedBoundingBoxLeft, const unsigned int transformedBoundingBoxTop, const unsigned int transformedBoundingBoxWidth, const unsigned int transformedBoundingBoxHeight, const Frame* gradientCurrentFrame, const SquareMatrix3* current_H_template, const Scalar* templateMeans, const Scalar* currentMeans, Matrix* hessian, Matrix* jacobianError, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused)
{
	static_assert(tParameters == 8u || tParameters == 9u, "Invalid parameter number!");

	ocean_assert(templateFrame != nullptr && templateFrame->isValid() && templateFrame->dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && templateFrame->channels() == tChannels);

	ocean_assert(templateSubRegion != nullptr && transformedCurrentSubFrame != nullptr && gradientCurrentFrame != nullptr && current_H_template != nullptr && hessian != nullptr && jacobianError != nullptr);
	ocean_assert(!tUseMeans || (templateMeans != nullptr && currentMeans != nullptr));

	ocean_assert(Numeric::isEqual((*current_H_template)[8], 1));

	ocean_assert(threadIndex < threads);
	ocean_assert_and_suppress_unused(unused == 1u, unused);

	const Box2 boundingBox = templateSubRegion->boundingBox();

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!boundingBox.box2integer(templateFrame->width(), templateFrame->height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		ocean_assert(false && "This must never happen!");
	}

	ocean_assert(transformedCurrentSubFrame->width() == boundingBoxWidth && transformedCurrentSubFrame->height() == boundingBoxHeight);

	const uint8_t* transformedCurrentSubFrameD = transformedCurrentSubFrame->constdata<uint8_t>();
	const uint8_t* transformedCurrentSubFrameMaskD = transformedCurrentSubFrameMask->constdata<uint8_t>();

	const unsigned int transformedCurrentSubFrameStrideElements = transformedCurrentSubFrame->strideElements();
	const unsigned int transformedCurrentSubFrameMaskStrideElements = transformedCurrentSubFrameMask->strideElements();

	const int16_t* gradientCurrentData = gradientCurrentFrame->constdata<int16_t>();
	const unsigned int gradientCurrentPaddingElements = gradientCurrentFrame->paddingElements();

	StaticMatrix<Scalar, 1, 2> gradient;
	StaticMatrix<Scalar, 2, tParameters> localJacobian;
	StaticMatrix<Scalar, 1, tParameters> intermediate;

	StaticMatrix<Scalar, tParameters, tParameters> localHessian(false);
	StaticMatrix<Scalar, 1, tParameters> transposedJacobianError(false);

	Scalar interpolatedGradientData[tChannels * 2u];

	for (unsigned int y = boundingBoxTop + threadIndex; y < boundingBoxTop + boundingBoxHeight; y += threads)
	{
		const uint8_t* templatePixel = templateFrame->constpixel<uint8_t>(boundingBoxLeft, y);

		for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
		{
			if (templateSubRegion->isInside(Vector2(Scalar(x), Scalar(y))))
			{
				const uint8_t* transformedCurrentSubFrameMaskTopLeft = transformedCurrentSubFrameMaskD + (y - boundingBoxTop) * transformedCurrentSubFrameMaskStrideElements + (x - boundingBoxLeft);

				// checking the 2x2 pixels necessary for the interpolation of the gradient values
				if (transformedCurrentSubFrameMaskTopLeft[0]
							&& (x == boundingBoxLeft + boundingBoxWidth - 1u || transformedCurrentSubFrameMaskTopLeft[1])
							&& (y == boundingBoxTop + boundingBoxHeight - 1u || transformedCurrentSubFrameMaskTopLeft[transformedCurrentSubFrameMaskStrideElements])
							&& (x == boundingBoxLeft + boundingBoxWidth - 1u || y == boundingBoxTop + boundingBoxHeight - 1u || transformedCurrentSubFrameMaskTopLeft[transformedCurrentSubFrameMaskStrideElements + 1u]))
				{
					const Vector2 transformedPoint((*current_H_template) * Vector2(Scalar(x), Scalar(y)));

					if (transformedPoint.x() >= Scalar(0) && transformedPoint.x() <= Scalar(transformedBoundingBoxWidth - 1u) && transformedPoint.y() >= Scalar(0) && transformedPoint.y() <= Scalar(transformedBoundingBoxHeight - 1u))
					{
						CV::FrameInterpolatorBilinear::interpolatePixel<int16_t, Scalar, tChannels * 2u, CV::PC_TOP_LEFT>(gradientCurrentData, transformedBoundingBoxWidth, transformedBoundingBoxHeight, gradientCurrentPaddingElements, Vector2(transformedPoint.x() - Scalar(transformedBoundingBoxLeft), transformedPoint.y() - Scalar(transformedBoundingBoxTop)), interpolatedGradientData);

						if constexpr (tParameters == 8u)
						{
							Geometry::Jacobian::calculateHomographyJacobian2x8(localJacobian.template row<0>(), localJacobian.template row<1>(), Scalar(x), Scalar(y), *current_H_template);
						}
						else
						{
							Geometry::Jacobian::calculateHomographyJacobian2x9(localJacobian.template row<0>(), localJacobian.template row<1>(), Scalar(x), Scalar(y), *current_H_template);
						}

						const uint8_t* transformedCurrentSubFrameTopLeft = transformedCurrentSubFrameD + (y - boundingBoxTop) * transformedCurrentSubFrameStrideElements + (x - boundingBoxLeft) * tChannels;

						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							// normally, we would have to normalize the gradients by (0.5 / 255), however we normalize the Hessian and jacobian vector at the end as this is simply a scalar factor
							gradient.element<0, 0>() = Scalar(interpolatedGradientData[2u * n + 0u]);
							gradient.element<0, 1>() = Scalar(interpolatedGradientData[2u * n + 1u]);

							intermediate = gradient * localJacobian;
							intermediate.multiplyWithTransposedLeftAndAdd(localHessian);

							ocean_assert(y >= boundingBoxTop && x >= boundingBoxLeft);

							if constexpr (tUseMeans)
							{
								// we also normalize (by 1/255) the error at the end as it is a simple scalar factor
								const Scalar channelError = (Scalar(transformedCurrentSubFrameTopLeft[n]) - currentMeans[n]) - (Scalar(templatePixel[n]) - templateMeans[n]);

								transposedJacobianError += intermediate * channelError;
							}
							else
							{
								// we also normalize (by 1/255) the error at the end as it is a simple scalar factor
								const Scalar channelError = Scalar(int(transformedCurrentSubFrameTopLeft[n]) - int(templatePixel[n]));

								transposedJacobianError += intermediate * channelError;
							}
						}
					}
				}
			}

			templatePixel += tChannels;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	ocean_assert(hessian->rows() == localHessian.rows() && hessian->columns() == localHessian.columns());
	ocean_assert(jacobianError->rows() == transposedJacobianError.columns() && jacobianError->columns() == transposedJacobianError.rows());

	const Scalar hessianNormalization = Scalar(0.5 * 0.5) / Scalar(255 * 255); // normalization due to the un-normalized integer gradient

	for (unsigned int n = 0u; n < tParameters * tParameters; ++n)
	{
		hessian->data()[n] += localHessian.data()[n] * hessianNormalization;
	}

	const Scalar jacobianNormalization = Scalar(0.5) / Scalar(255 * 255); // normalization due to the integer gradient (0.5/255) and normalization due to the error (1/255)

	for (unsigned int n = 0u; n < tParameters; ++n)
	{
		jacobianError->data()[n] += transposedJacobianError.data()[n] * jacobianNormalization;
	}
}

template <unsigned int tParameters, unsigned int tChannels, bool tUseMeans>
void HomographyImageAlignmentDense::determineErrorJacobianInverseCompositional8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const Scalar* templateMeans, const Scalar* currentMeans, const Scalar* jacobianRows, Matrix* jacobianError, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused)
{
	static_assert(tParameters == 8u || tParameters == 9u, "Invalid parameter number!");

	ocean_assert(templateFrame != nullptr && templateFrame->isValid() && templateFrame->dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && templateFrame->channels() == tChannels);

	ocean_assert(templateSubRegion != nullptr && transformedCurrentSubFrame != nullptr && jacobianRows != nullptr && jacobianError != nullptr);
	ocean_assert(!tUseMeans || (templateMeans != nullptr && currentMeans != nullptr));

	ocean_assert(threadIndex < threads);
	ocean_assert_and_suppress_unused(unused == 1u, unused);

	const Box2 boundingBox = templateSubRegion->boundingBox();

	unsigned int boundingBoxLeft = 0u, boundingBoxTop = 0u, boundingBoxWidth = 0u, boundingBoxHeight = 0u;
	if (!boundingBox.box2integer(templateFrame->width(), templateFrame->height(), boundingBoxLeft, boundingBoxTop, boundingBoxWidth, boundingBoxHeight))
	{
		ocean_assert(false && "This must never happen!");
	}

	ocean_assert(transformedCurrentSubFrame->width() == boundingBoxWidth && transformedCurrentSubFrame->height() == boundingBoxHeight);

	const uint8_t* transformedCurrentSubFrameData = transformedCurrentSubFrame->constdata<uint8_t>();
	const unsigned int transformedCurrentSubFrameStrideElements = transformedCurrentSubFrame->strideElements();

	StaticMatrix<Scalar, tParameters, 1u> localJacobianError(false);

	for (unsigned int y = boundingBoxTop + threadIndex; y < boundingBoxTop + boundingBoxHeight; y += threads)
	{
		const uint8_t* templatePixel = templateFrame->constpixel<uint8_t>(boundingBoxLeft, y);

		unsigned int jacobianIndex = tChannels * ((y - boundingBoxTop) * boundingBoxWidth - boundingBoxLeft);

		for (unsigned int x = boundingBoxLeft; x < boundingBoxLeft + boundingBoxWidth; ++x)
		{
			if (templateSubRegion->isInside(Vector2(Scalar(x), Scalar(y))))
			{
				const uint8_t* transformedCurrentSubFramePixel = transformedCurrentSubFrameData + (y - boundingBoxTop) * transformedCurrentSubFrameStrideElements + (x - boundingBoxLeft) * tChannels;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					const Scalar* jacobianRow = jacobianRows + (jacobianIndex + n) * tParameters;

					if constexpr (tUseMeans)
					{
						const Scalar channelError = (Scalar(transformedCurrentSubFramePixel[n]) - currentMeans[n]) - (Scalar(templatePixel[n]) - templateMeans[n]);

						for (unsigned int i = 0u; i < tParameters; ++i)
						{
							localJacobianError[i] += jacobianRow[i] * channelError;
						}
					}
					else
					{
						const Scalar channelError = Scalar(int(transformedCurrentSubFramePixel[n]) - int(templatePixel[n]));

						for (unsigned int i = 0u; i < tParameters; ++i)
						{
							localJacobianError[i] += jacobianRow[i] * channelError;
						}
					}
				}
			}

			templatePixel += tChannels;
			jacobianIndex += tChannels;
		}
	}

	constexpr Scalar jacobianNormalization = Scalar(0.5) / Scalar(255 * 255); // 0.5 / 255 for the gradient normalization, 1 / 255 for the error normalization

	const OptionalScopedLock scopedLock(lock);

	for (unsigned int n = 0u; n < tParameters; ++n)
	{
		jacobianError->data()[n] += localJacobianError.data()[n] * jacobianNormalization;
	}
}

}

}
