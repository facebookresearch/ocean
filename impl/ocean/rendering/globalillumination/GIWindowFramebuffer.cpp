/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIWindowFramebuffer.h"

#include "ocean/rendering/StereoView.h"

#ifdef _WINDOWS
	#include "ocean/platform/win/Bitmap.h"
#endif


namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIWindowFramebuffer::GIWindowFramebuffer(const Engine::GraphicAPI preferredGraphicAPI) :
	GIFramebuffer(preferredGraphicAPI),
	WindowFramebuffer()
{

#if defined(_WINDOWS)
	windowHandle = nullptr;
	windowDC = nullptr;
#elif defined(__APPLE__)
	windowView = nullptr;
#endif

}

GIWindowFramebuffer::~GIWindowFramebuffer()
{
	release();
}

size_t GIWindowFramebuffer::id() const
{
#ifdef _WINDOWS
	return (size_t)(windowHandle);
#else
	ocean_assert(false && "Missing implementation!");
	return 0;
#endif
}

bool GIWindowFramebuffer::initializeById(const size_t id, const FramebufferRef& /*shareFramebuffer*/)
{
	if (id == 0)
	{
		return false;
	}

	// release a previously create framebuffer
	release();

#ifdef _WINDOWS

	windowHandle = HWND(id);
	windowDC = GetDC(windowHandle);

#elif defined(__APPLE__)

	windowView = (void*)(id);

#endif

	return true;
}

GIWindowFramebuffer::LightingMode GIWindowFramebuffer::lightingMode() const
{
	// **TODO** needs a correct implementation

	return PrimitiveAttribute::LM_DEFAULT;
}

GIWindowFramebuffer::ShadowTechnique GIWindowFramebuffer::shadowTechnique() const
{
	// **TODO** needs a correct implementation

	return SHADOW_TECHNIQUE_NONE;
}

void GIWindowFramebuffer::setLightingMode(const LightingMode /*lightingMode*/)
{
	// **TODO** needs a correct implementation
}

void GIWindowFramebuffer::setShadowTechnique(const ShadowTechnique /*technique*/)
{
	// **TODO** needs a correct implementation
}

void GIWindowFramebuffer::render()
{
	const ScopedLock scopedLock(objectLock);

#if defined(_WINDOWS)

	if (windowDC == nullptr)
	{
		return;
	}

#elif defined(__APPLE__)

	if (windowView == nullptr)
	{
		return;
	}

#endif

	frame_.setValue(0x00u);
	GIFramebuffer::render();

#if defined(_WINDOWS)

	Platform::Win::Bitmap bitmap(frame_);
	BitBlt(windowDC, 0, 0, bitmap.width(), bitmap.height(), bitmap.dc(), 0, 0, SRCCOPY);

#elif defined(__APPLE__)

	// **TOOD** what is the intended solution on Apple platforms?

	/*NSView* view = (__bridge NSView*)(windowView);
	ocean_assert(view);

	Platform::Apple::MacOS::Image image(frame_);

	[image.nsImage() drawAtPoint:NSZeroPoint fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1];*/

#endif

}

void GIWindowFramebuffer::release()
{
	const ScopedLock scopedLock(objectLock);

	GIFramebuffer::release();

#ifdef _WINDOWS
	ReleaseDC(windowHandle, windowDC);
	windowHandle = nullptr;
	windowDC = nullptr;
#endif
}

}

}

}
