/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/UndistortedBackground.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Rendering
{

PerspectiveView::PerspectiveView() :
	View()
{
	// nothing to do here
}

PerspectiveView::~PerspectiveView()
{
	// nothing to do here
}

Scalar PerspectiveView::fovX() const
{
	throw NotSupportedException("PerspectiveView::fovX() is not supported.");
}

bool PerspectiveView::setFovX(const Scalar /*fovx*/)
{
	throw NotSupportedException("PerspectiveView::setFovX() is not supported.");
}

Scalar PerspectiveView::idealFovX(bool* validCamera) const
{
	const ScopedLock scopedLock(objectLock);

	if (validCamera)
	{
		*validCamera = false;
	}

	if (backgrounds_.empty())
	{
		return fovX();
	}

	const UndistortedBackgroundRef background(backgrounds_.front());
	if (background.isNull())
	{
		return fovX();
	}

	const PinholeCamera backgroundCamera(background->camera());
	if (!backgroundCamera)
	{
		return fovX();
	}

	const Vectors2 corners =
	{
		Vector2(0, 0),
		Vector2(0, Scalar(backgroundCamera.height() - 1u)),
		Vector2(Scalar(backgroundCamera.width() - 1u), Scalar(backgroundCamera.height() - 1u)),
		Vector2(Scalar(backgroundCamera.width() - 1u), 0)
	};

	Scalar minAbsoluteHorizontal = Numeric::maxValue();
	Scalar minAbsoluteVertical = Numeric::maxValue();

	for (const Vector2& corner : corners)
	{
		const Vector2 undistorted = backgroundCamera.undistort<true>(corner);

		const Vector3 vector = background->orientation() * backgroundCamera.vectorToPlane(undistorted, 1);

		minAbsoluteHorizontal = std::min(minAbsoluteHorizontal, Numeric::abs(vector.x()));
		minAbsoluteVertical = std::min(minAbsoluteVertical, Numeric::abs(vector.y()));
	}

	if (Numeric::isEqualEps(minAbsoluteHorizontal) || Numeric::isEqualEps(minAbsoluteVertical))
	{
		return fovX();
	}

	const Scalar fovX = Numeric::atan2(minAbsoluteHorizontal, 1) * Scalar(2);
	const Scalar fovY = Numeric::atan2(minAbsoluteVertical, 1) * Scalar(2);

	ocean_assert(fovY > Numeric::eps());

	const Scalar viewAspectRatio = aspectRatio();
	const Scalar validCameraAreaAspectRatio = minAbsoluteHorizontal / minAbsoluteVertical;

	Scalar bestFovX = fovX;

	if (viewAspectRatio < validCameraAreaAspectRatio)
	{
		bestFovX = PinholeCamera::fovY2X(fovY, aspectRatio());
	}

	bestFovX = std::max(Numeric::deg2rad(1), bestFovX - Numeric::deg2rad(1));

	if (validCamera)
	{
		*validCamera = true;
	}

	return bestFovX;
}

void PerspectiveView::updateToIdealFovX()
{
	bool hasValidCamera = false;
	const Scalar fovX = idealFovX(&hasValidCamera);

	if (hasValidCamera)
	{
		setFovX(fovX);
	}
}

void PerspectiveView::setProjectionMatrix(const SquareMatrix4& /*clip_T_view*/)
{
	throw NotSupportedException("PerspectiveView::setProjectionMatrix() is not supported.");
}

void PerspectiveView::fitCamera(const NodeRef& node)
{
	NodeRef sceneNode(node);

	if (!sceneNode)
	{
		const ScopedLock scopedLock(objectLock);
		const ObjectRefs parents(parentObjects());

		for (const ObjectRef& parent : parents)
		{
			const FramebufferRef framebuffer(parent);
			ocean_assert(framebuffer);

			if (framebuffer)
			{
				const Scenes scenes(framebuffer->scenes());

				if (!scenes.empty())
				{
					sceneNode = scenes.front();
					break;
				}
			}
		}
	}

	if (sceneNode.isNull())
	{
		return;
	}

	const BoundingSphere boundingSphere(sceneNode->boundingSphere());

	const Scalar fovX = this->fovX();
	const Scalar fovY = PinholeCamera::fovX2Y(fovX, aspectRatio());

	const Scalar fov = min(fovX, fovY);

	ocean_assert(fov >= 0);
	if (fov <= Numeric::eps())
	{
		return;
	}

	/*
	 * determining the correct viewing distance by ensuring that the sphere is entirely visible within the camera frustum
	 *
	 * sin(fov / 2) == radius / viewingDistance
	 * (with hypotenuse distance between sphere center and camera)
	 *    ______
	 *   /      \
	 *  /      r \     sin(fov / 2) = r / d
	 * |     O----|
	 *  \ r/  \  /% <- % lines are tangent to the sphere
	 *   \/____\/ %
	 *      %  d\ %
	 *        %  \%
	 *          % C <- camera center
	 */

	const Scalar viewDistance = boundingSphere.radius() / Numeric::sin(fov * Scalar(0.5));
	const Vector3 viewDirection(transformation().rotationMatrix() * Vector3(0, 0, -1));
	ocean_assert(Numeric::isEqual(viewDirection.length(), 1));

	const Vector3 newViewPosition = boundingSphere.center() - viewDirection * viewDistance;
	setTransformation(HomogenousMatrix4(newViewPosition, transformation().rotationMatrix()));
}

PerspectiveView::ObjectType PerspectiveView::type() const
{
	return TYPE_PERSPECTIVE_VIEW;
}

}

}
