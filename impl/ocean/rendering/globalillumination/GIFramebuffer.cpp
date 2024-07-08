/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIFramebuffer.h"
#include "ocean/rendering/globalillumination/GIEngine.h"
#include "ocean/rendering/globalillumination/GINode.h"
#include "ocean/rendering/globalillumination/GIRenderable.h"
#include "ocean/rendering/globalillumination/GIScene.h"
#include "ocean/rendering/globalillumination/GIView.h"

#include "ocean/base/Utilities.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameFilterSobelMagnitude.h"

#include "ocean/rendering/View.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIFramebuffer::GIFramebuffer(const Engine::GraphicAPI preferredGraphicAPI) :
	Framebuffer(),
	preferredGraphicAPI_(preferredGraphicAPI),
	antialiasingEnabled_(false),
	lightingModes_(Lighting::LM_SHADING_FULL)
{
	// nothing to do here
}

GIFramebuffer::~GIFramebuffer()
{
	// nothing to do here
}

GIFramebuffer::FaceMode GIFramebuffer::faceMode() const
{
	ocean_assert(false && "Missing implementation!");

	return PrimitiveAttribute::MODE_DEFAULT;
}

GIFramebuffer::CullingMode GIFramebuffer::cullingMode() const
{
	ocean_assert(false && "Missing implementation!");

	return PrimitiveAttribute::CULLING_DEFAULT;
}

GIFramebuffer::RenderTechnique GIFramebuffer::renderTechnique() const
{
	switch (lightingModes_)
	{
		case Lighting::LM_LIGHTING_FULL:
			return TECHNIQUE_FULL;

		case Lighting::LM_SHADING_FULL:
			return TECHNIQUE_SHADED;

		// **TODO** find a matching lighting state
		case Lighting::LM_SHADING_LAMBERT:
			return TECHNIQUE_TEXTURED;

		case Lighting::LM_UNLIT:
			return TECHNIQUE_UNLIT;

		default:
			break;
	}

	ocean_assert(false && "Invalid framebuffer lighting states!");
	return TECHNIQUE_UNLIT;
}

bool GIFramebuffer::isAntialiasingSupported(const unsigned int /*buffers*/) const
{
	return true;
}

bool GIFramebuffer::isAntialiasing() const
{
	return antialiasingEnabled_;
}

bool GIFramebuffer::isQuadbufferedStereoSupported() const
{
	return false;
}

void GIFramebuffer::setView(const ViewRef& newView)
{
	if (newView.isNull())
	{
		return;
	}

	Framebuffer::setView(newView);
}

void GIFramebuffer::setFaceMode(const FaceMode /*faceMode*/)
{
	// Missing implementation!
}

void GIFramebuffer::setCullingMode(const CullingMode /*cullingMode*/)
{
	// Missing implementation!
}

void GIFramebuffer::setRenderTechnique(const RenderTechnique technique)
{
	switch (technique)
	{
		case TECHNIQUE_FULL:
			lightingModes_ = Lighting::LM_LIGHTING_FULL;
			break;

		case TECHNIQUE_SHADED:
			lightingModes_ = Lighting::LM_SHADING_FULL;
			break;

		// **TODO** find a matching lighting state
		case TECHNIQUE_TEXTURED:
			lightingModes_ = Lighting::LM_SHADING_LAMBERT;
			break;

		case TECHNIQUE_UNLIT:
			lightingModes_ = Lighting::LM_UNLIT;
			break;

		default:
			ocean_assert(false && "Invalid render technique!");
			lightingModes_ = Lighting::LM_UNLIT;
	}
}

bool GIFramebuffer::setSupportAntialiasing(const unsigned int /*buffers*/)
{
	// Missing implementation
	return false;
}

bool GIFramebuffer::setAntialiasing(const bool antialiasing)
{
	antialiasingEnabled_ = antialiasing;
	return true;
}

bool GIFramebuffer::setSupportQuadbufferedStereo(const bool /*enable*/)
{
	// Missing implementation
	return false;
}

void GIFramebuffer::makeCurrent()
{
	// nothing to do here
}

void GIFramebuffer::makeNoncurrent()
{
	// nothing to do here
}

void GIFramebuffer::viewport(unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height) const
{
	left = 0u;
	top = 0u;

	width = frame_.width();
	height = frame_.height();
}

void GIFramebuffer::setViewport(const unsigned int /*left*/, const unsigned int /*top*/, const unsigned int width, const unsigned int height)
{
	if (!frame_.set(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), false, true))
	{
		ocean_assert(false && "This should never happen!");
	}
}

void GIFramebuffer::addScene(const SceneRef& scene)
{
	Framebuffer::addScene(scene);
}

void GIFramebuffer::removeScene(const SceneRef& scene)
{
	Framebuffer::removeScene(scene);
}

void GIFramebuffer::clearScenes()
{
	Framebuffer::clearScenes();
}

void GIFramebuffer::render()
{
	if (!framebufferView)
	{
		ocean_assert(false && "The framebuffer does not hold any view!");
		return;
	}

	LightSources lightSources;

	if (framebufferView->useHeadlight())
	{
		const SmartObjectRef<GIView> view(framebufferView);
		ocean_assert(view);

		lightSources.push_back(LightPair(view->headlight(), HomogenousMatrix4(view->transformation().translation())));
	}

	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	TracingGroup tracingGroup;
	for (Scenes::const_iterator i = framebufferScenes.begin(); i != framebufferScenes.end(); ++i)
	{
		SmartObjectRef<GIScene> scene(*i);
		ocean_assert(scene);

		scene->buildTracing(tracingGroup, HomogenousMatrix4(true), lightSources);
	}

	if (scopedWorker)
	{
		scopedWorker()->executeFunction(Worker::Function::create(*this, &GIFramebuffer::renderSubset, &lightSources, (const TracingGroup*)&tracingGroup, scopedWorker()->threads(), 0u, 0u), 0u, scopedWorker()->threads(), 3u, 4u, 1u);
	}
	else
	{
		GIFramebuffer::renderSubset(&lightSources, &tracingGroup, 1u, 0u, 1u);
	}

	if (antialiasingEnabled_ && frame_.width() >= 3u && frame_.height() >= 3u)
	{
		if (!sobelFrame_.set(FrameType(frame_, FrameType::FORMAT_Y8), false, true))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		CV::FrameFilterSobelMagnitude::Comfort::filterHorizontalVerticalTo1Response(frame_, sobelFrame_, scopedWorker());

		if (scopedWorker)
		{
			scopedWorker()->executeFunction(Worker::Function::create(*this, &GIFramebuffer::renderAntialiasedSubset, sobelFrame_.constdata<uint8_t>(), sobelFrame_.paddingElements(), &lightSources, (const TracingGroup*)(&tracingGroup), scopedWorker()->threads(), 0u, 0u), 0u, scopedWorker()->threads(), 5u, 6u, 1u);
		}
		else
		{
			renderAntialiasedSubset(sobelFrame_.constdata<uint8_t>(), sobelFrame_.paddingElements(), &lightSources, &tracingGroup, 1u, 0u, 1u);
		}
	}
}

void GIFramebuffer::renderSubset(LightSources* lightSources, const TracingGroup* group, const unsigned int threads, const unsigned int firstThreadIndex, const unsigned int mustBeOne)
{
	ocean_assert(lightSources != nullptr);
	ocean_assert(group != nullptr);
	ocean_assert_and_suppress_unused(mustBeOne == 1u, mustBeOne);

	const View* view = &*framebufferView;

	ocean_assert(frame_.isValid());

	for (unsigned int n = firstThreadIndex; n < frame_.pixels(); n += threads)
	{
		const unsigned int y = n / frame_.width();
		const unsigned int x = n % frame_.width();

		const Line3 ray = view->viewingRay(Scalar(x), Scalar(y), frame_.width(), frame_.height());

		RGBAColor color;

		if (renderRay(view->transformation().translation(), ray, *group, *lightSources, color))
		{
			const float redValue = minmax(0.0f, color.red(), 1.0f);
			const float greenValue = minmax(0.0f, color.green(), 1.0f);
			const float blueValue = minmax(0.0f, color.blue(), 1.0f);

			uint8_t* const pixel = frame_.pixel<uint8_t>(x, y);

			pixel[0] = uint8_t(redValue * 255.0f + 0.5f);
			pixel[1] = uint8_t(greenValue * 255.0f + 0.5f);
			pixel[2] = uint8_t(blueValue * 255.0f + 0.5f);
		}
	}
}

void GIFramebuffer::renderAntialiasedSubset(const uint8_t* sobelResponse, const unsigned int sobelResponsePaddingElements, LightSources* lightSources, const TracingGroup* group, const unsigned int threads, const unsigned int firstThreadIndex, const unsigned int mustBeOne)
{
	ocean_assert(sobelResponse != nullptr);

	ocean_assert(lightSources != nullptr);
	ocean_assert(group != nullptr);
	ocean_assert_and_suppress_unused(mustBeOne == 1u, mustBeOne);

	const unsigned int sobelResponseStrideElements = frame_.width() + sobelResponsePaddingElements;

	const View* view = &*framebufferView;

	ocean_assert(frame_.isValid());

	for (unsigned int n = firstThreadIndex; n < frame_.pixels(); n += threads)
	{
		const unsigned int y = n / frame_.width();
		const unsigned int x = n % frame_.width();

		RGBAColor color(0.0f, 0.0f, 0.0f);
		Scalar totalFactor = 0;

		Scalar samplingFactor = 1;

		const uint8_t sobelResponsePixel = sobelResponse[y * sobelResponseStrideElements + x];

		if (sobelResponsePixel >= 70)
		{
			samplingFactor = Scalar(0.1);
		}
		else if (sobelResponsePixel >= 50)
		{
			samplingFactor = Scalar(0.2);
		}
		else if (sobelResponsePixel >= 40)
		{
			samplingFactor = Scalar(0.25);
		}
		else if (sobelResponsePixel >= 25)
		{
			samplingFactor = Scalar(0.5);
		}

		if (samplingFactor != 1)
		{
			for (Scalar xx = Scalar(-0.5); xx <= Scalar(0.501); xx += samplingFactor)
			{
				for (Scalar yy = Scalar(-0.5); yy <= Scalar(0.501); yy += samplingFactor)
				{
					const Vector2 sample = Vector2(Scalar(x), Scalar(y)) + Vector2(xx, yy);

					const Line3 ray = view->viewingRay(sample.x(), sample.y(), frame_.width(), frame_.height());

					const Scalar factor = Numeric::normalizedGaussianDistribution2(xx, yy, Scalar(1), Scalar(1));

					RGBAColor localColor;
					if (!renderRay(view->transformation().translation(), ray, *group, *lightSources, localColor))
					{
						uint8_t* const pixel = frame_.pixel<uint8_t>(x, y);

						localColor = RGBAColor(float(pixel[0]) / 255.0f, float(pixel[1]) / 255.0f, float(pixel[2]) / 255.0f);
					}

					color.combine(localColor.damped(float(factor)));
					totalFactor += factor;
				}
			}

			color.damp(1.0f / float(totalFactor));

			const float redValue = minmax(0.0f, color.red(), 1.0f);
			const float greenValue = minmax(0.0f, color.green(), 1.0f);
			const float blueValue = minmax(0.0f, color.blue(), 1.0f);

			uint8_t* const pixel = frame_.pixel<uint8_t>(x, y);

			pixel[0] = uint8_t(redValue * 255.0f + 0.5f);
			pixel[1] = uint8_t(greenValue * 255.0f + 0.5f);
			pixel[2] = uint8_t(blueValue * 255.0f + 0.5f);
		}
	}
}

bool GIFramebuffer::intersection(const Line3& /*ray*/, RenderableRef& /*renderable*/, Vector3& /*position*/)
{
	ocean_assert(false && "Missing implementation!");
	return false;
}

bool GIFramebuffer::renderRay(const Vector3& viewPosition, const Line3& ray, const TracingGroup& group, const LightSources& /*lightSources*/, RGBAColor& color) const
{
	// find the nearest intersection

	RayIntersection intersection;
	group.findNearestIntersection(ray, intersection, true, Numeric::eps());

	if (!intersection)
	{
		ocean_assert(framebufferView);
		return false;
	}

	// determine the color for the most nearest intersection

	const TracingObject* tracingObject = intersection.tracingObject();
	ocean_assert(tracingObject);

	return tracingObject->determineColor(viewPosition, ray.direction(), intersection, group, 2u, nullptr, lightingModes_, color);
}

void GIFramebuffer::release()
{
	// nothing to do here
}

}

}

}
