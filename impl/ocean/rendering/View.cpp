/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/View.h"
#include "ocean/rendering/Engine.h"

namespace Ocean
{

namespace Rendering
{

View::View() :
	Object()
{
	// nothing to do here
}

View::~View()
{
	for (const BackgroundRef& background : backgrounds_)
	{
		unregisterThisObjectAsParent(background);
	}
}

Scalar View::aspectRatio() const
{
	throw NotSupportedException("View::aspectRatio() is not supported.");
}

Scalar View::nearDistance() const
{
	throw NotSupportedException("View::nearDistance() is not supported.");
}

Scalar View::farDistance() const
{
	throw NotSupportedException("View::farDistance() is not supported.");
}

HomogenousMatrix4 View::transformation() const
{
	throw NotSupportedException("View::transformation() is not supported.");
}

SquareMatrix4 View::projectionMatrix() const
{
	throw NotSupportedException("View::projectionMatrix() is not supported.");
}

RGBAColor View::backgroundColor() const
{
	throw NotSupportedException("View::backgroundColor() is not supported.");
}

BackgroundRef View::background() const
{
	const ScopedLock scopedLock(objectLock);

	if (backgrounds_.empty())
	{
		return BackgroundRef();
	}

	return backgrounds_.front();
}

View::Backgrounds View::backgrounds() const
{
	return backgrounds_;
}

bool View::useHeadlight() const
{
	throw NotSupportedException("View::useHeadlight() is not supported.");
}

PhantomAttribute::PhantomMode View::phantomMode() const
{
	throw NotSupportedException("View::phantomMode() is not supported.");
}

Line3 View::viewingRay(const Scalar /*x*/, const Scalar /*y*/, const unsigned int /*width*/, const unsigned int /*height*/) const
{
	throw NotSupportedException("The determination of a viewing ray is not supported in this render engine.");
}

bool View::setAspectRatio(const Scalar /*aspectRatio*/)
{
	throw NotSupportedException("View::setAspectRatio() is not supported.");
}

bool View::setNearDistance(const Scalar /*distance*/)
{
	throw NotSupportedException("View::setNearDistance() is not supported.");
}

bool View::setFarDistance(const Scalar distance)
{
	ocean_assert(distance > 0);

	const ScopedLock scopedLock(objectLock);

	try
	{
		for (const BackgroundRef& background : backgrounds_)
		{
			background->setDistance(distance * Scalar(0.85));
		}
	}
	catch(...)
	{
		// nothing to do here
	}

	return true;
}

bool View::setNearFarDistance(const Scalar nearDistance, const Scalar farDistance)
{
	ocean_assert_and_suppress_unused(nearDistance > 0 && farDistance > 0 && nearDistance < farDistance, nearDistance);

	const ScopedLock scopedLock(objectLock);

	try
	{
		for (const BackgroundRef& background : backgrounds_)
		{
			background->setDistance(farDistance * Scalar(0.85));
		}
	}
	catch(...)
	{
		// nothing to do here
	}

	return true;
}

void View::setTransformation(const HomogenousMatrix4& /*world_T_view*/)
{
	throw NotSupportedException("View::setTransformation() is not supported.");
}

bool View::setBackgroundColor(const RGBAColor& /*color*/)
{
	throw NotSupportedException("View::setBackground() is not supported.");
}

void View::addBackground(const BackgroundRef& background)
{
	if (background.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	registerThisObjectAsParent(background);
	backgrounds_.push_back(background);

	try
	{
		background->setDistance(farDistance() * Scalar(0.85));
	}
	catch(...)
	{
		// nothing to do here
	}
}

void View::removeBackground(const BackgroundRef& background)
{
	if (background.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	for (Backgrounds::iterator i = backgrounds_.begin(); i != backgrounds_.end(); ++i)
	{
		if (*i == background)
		{
			unregisterThisObjectAsParent(background);

			backgrounds_.erase(i);
			break;
		}
	}
}

void View::setUseHeadlight(const bool /*state*/)
{
	throw NotSupportedException("View::setUseHeadlight() is not supported.");
}

bool View::setPhantomMode(const PhantomAttribute::PhantomMode /*mode*/)
{
	throw NotSupportedException("View::setPhantomMode() is not supported.");
}

void View::fitCamera(const NodeRef& /*node*/)
{
	throw NotSupportedException("View::fitCamera() is not supported.");
}

View::ObjectType View::type() const
{
	return TYPE_VIEW;
}


}

}
