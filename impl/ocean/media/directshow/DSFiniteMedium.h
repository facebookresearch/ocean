/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_FINITE_MEDIUM_H
#define META_OCEAN_MEDIA_DS_FINITE_MEDIUM_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSMedium.h"

#include "ocean/media/FiniteMedium.h"

#include "ocean/base/Scheduler.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class is the base class for all direct show finite mediums.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSFiniteMedium :
	virtual public FiniteMedium,
	virtual public DSMedium
{
	friend class DSFrameMedium;

	public:

		/**
		 * Returns the duration of the finite medium.
		 * @see FiniteMedium::duration().
		 */
		double duration() const override;

		/**
		 * Returns the duration without speed consideration.
		 * @see FiniteMedium::normalDuration().
		 */
		double normalDuration() const override;

		/**
		 * Returns the recent position of the finite medium.
		 * @see FiniteMedium::position().
		 */
		double position() const override;

		/**
		 * Sets the recent position of the finit medium.
		 * @see FiniteMedium::setPosition().
		 */
		bool setPosition(const double position) override;

		/**
		 * Returns the speed of the finite medium.
		 * @see FiniteMedium::speed().
		 */
		float speed() const override;

		/**
		 * Sets the speed of the finite medium.
		 * @see FiniteMedium::setSpeed().
		 */
		bool setSpeed(const float speed) override;

		/**
		 * Returns whether a previous task has been finished.
		 * @see FiniteMedium::taskFinished().
		 */
		bool taskFinished() const override;

	protected:

		/**
		 * Creates a new finite medium by a given url.
		 * @param url Url of the finite medium
		 */
		explicit DSFiniteMedium(const std::string& url);

		/**
		 * Destructs a DSFiniteMedium object.
		 */
		~DSFiniteMedium() override;

		/**
		 * Stars the filter graph.
		 * @see DSGraphObject::startGraph().
		 */
		bool startGraph() override;

		/**
		 * Creates several DirectShow interface.
		 * @return True, if succeeded
		 */
		bool createFiniteInterface();

		/**
		 * Release the DirectShow interface.
		 */
		void releaseFiniteInterface();

		/**
		 * Informs this finite medium object that the medium has been stopped (without knowledge of this finite medium).
		 */
		void hasStopped();

		/**
		 * Function called by the scheduler repeatedly.
		 */
		virtual void onScheduler();

	protected:

		/// DirectShow media seeking interface.
		ScopedIMediaSeeking seekingInterface_;

		/// Explicit state that the medium has stopped.
		bool hasStopped_ = false;

		/// Medium speed.
		float speed_ = 1.0f;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_FINITE_MEDIUM_H
