/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/View.h"

namespace Ocean
{

namespace Rendering
{

Framebuffer::Framebuffer()
{
	// nothing to do here
}

Framebuffer::~Framebuffer()
{
	release();
}

const Scenes& Framebuffer::scenes() const
{
	return framebufferScenes;
}

const ViewRef& Framebuffer::view() const
{
	return framebufferView;
}

void Framebuffer::viewport(unsigned int& /*left*/, unsigned int& /*top*/, unsigned int& /*width*/, unsigned int& /*height*/) const
{
	throw NotSupportedException("Viewport is not supported.");
}

Framebuffer::FaceMode Framebuffer::faceMode() const
{
	throw NotSupportedException("Face mode is not supported.");
}

Framebuffer::CullingMode Framebuffer::cullingMode() const
{
	throw NotSupportedException("Culling mode is not supported.");
}

Framebuffer::LightingMode Framebuffer::lightingMode() const
{
	throw NotSupportedException("Lighting mode is not supported.");
}

Framebuffer::RenderTechnique Framebuffer::renderTechnique() const
{
	throw NotSupportedException("Render technique is not supported.");
}

bool Framebuffer::isQuadbufferedStereoSupported() const
{
	throw NotSupportedException("Quad-buffered stereo is not supported.");
}

bool Framebuffer::isAntialiasingSupported(const unsigned int /*buffers*/) const
{
	throw NotSupportedException("Anti-Aliasing is not supported.");
}

bool Framebuffer::isAntialiasing() const
{
	throw NotSupportedException("Anti-Aliasing is not supported.");
}

Framebuffer::ShadowTechnique Framebuffer::shadowTechnique() const
{
	throw NotSupportedException("Shadow technique is not supported.");
}

HomogenousMatrix4 Framebuffer::device_T_display() const
{
	return HomogenousMatrix4(true);
}

bool Framebuffer::verticalSynchronization() const
{
	throw NotSupportedException("Vertical synchronization is not supported.");
}

std::string Framebuffer::extensions() const
{
	throw NotSupportedException("The framebuffer does provide information about extensions");
}

bool Framebuffer::hasExtension(const std::string& /*extension*/) const
{
	throw NotSupportedException("The framebuffer does provide information about extensions");
}

void Framebuffer::addScene(const SceneRef& scene)
{
	if (scene.isNull())
		return;

	const ScopedLock scopedLock(objectLock);

#ifdef OCEAN_DEBUG

	for (Scenes::const_iterator i = framebufferScenes.begin(); i != framebufferScenes.end(); ++i)
	{
		ocean_assert(*i != scene);
	}

#endif

	registerThisObjectAsParent(scene);
	framebufferScenes.push_back(scene);
}

void Framebuffer::removeScene(const SceneRef& scene)
{
	if (scene.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	for (Scenes::iterator i = framebufferScenes.begin(); i != framebufferScenes.end(); ++i)
	{
		if (*i == scene)
		{
			unregisterThisObjectAsParent(*i);

			framebufferScenes.erase(i);
			break;
		}
	}
}

void Framebuffer::clearScenes()
{
	const ScopedLock scopedLock(objectLock);

	for (Scenes::const_iterator i = framebufferScenes.begin(); i != framebufferScenes.end(); ++i)
	{
		unregisterThisObjectAsParent(*i);
	}

	framebufferScenes.clear();
}

void Framebuffer::setView(const ViewRef& view)
{
	unregisterThisObjectAsParent(framebufferView);
	registerThisObjectAsParent(view);

	framebufferView = view;
}

void Framebuffer::setViewport(const unsigned int /*left*/, const unsigned int /*top*/, const unsigned int /*width*/, const unsigned int /*height*/)
{
	throw NotSupportedException("Framebuffer::setViewport() is not supported.");
}

void Framebuffer::setPreferredPixelFormat(const FrameType::PixelFormat /*pixelFormat*/)
{
	throw NotSupportedException("This framebuffer does not support a preferred pixel format.");
}

void Framebuffer::setFaceMode(const FaceMode /*mode*/)
{
	throw NotSupportedException("This framebuffer does not support face mode overwriting.");
}

void Framebuffer::setCullingMode(const CullingMode /*cullingMode*/)
{
	throw NotSupportedException("This framebuffer does not support face culling mode overwriting.");
}

void Framebuffer::setLightingMode(const LightingMode /*lightingMode*/)
{
	throw NotSupportedException("This framebuffer does not support face lighting mode overwriting.");
}

void Framebuffer::setRenderTechnique(const RenderTechnique /*technique*/)
{
	throw NotSupportedException("This framebuffer does not support different render techniques.");
}

bool Framebuffer::setSupportQuadbufferedStereo(const bool /*enable*/)
{
	throw NotSupportedException("This framebuffer does not support quad-buffered stereo.");
}

bool Framebuffer::setSupportAntialiasing(const unsigned int /*buffers*/)
{
	throw NotSupportedException("This framebuffer does not support anti-aliasing.");
}

bool Framebuffer::setAntialiasing(const bool /*antialiasing*/)
{
	throw NotSupportedException("This framebuffer does not support anti-aliasing.");
}

void Framebuffer::setShadowTechnique(const ShadowTechnique /*technique*/)
{
	throw NotSupportedException("This framebuffer does not support shadow techniques.");
}

bool Framebuffer::setDevice_T_display(const HomogenousMatrix4& /*device_T_display*/)
{
	return false;
}

void Framebuffer::setVerticalSynchronization(const bool /*synchronize*/)
{
	throw NotSupportedException("This framebuffer does not support selection of synchronization type.");
}

void Framebuffer::render()
{
	throw NotSupportedException("This framebuffer does not support rendering.");
}

bool Framebuffer::render(Frame& /*frame*/, Frame* /*depthFrame*/)
{
	return false;
}

bool Framebuffer::intersection(const Line3& /*ray*/, RenderableRef& /*renderable*/, Vector3& /*position*/)
{
	throw NotImplementedException("Ray intersection is not supported in this render engine.");
}

void Framebuffer::makeCurrent()
{
	throw NotSupportedException("Make current is not supported.");
}

void Framebuffer::makeNoncurrent()
{
	throw NotSupportedException("Make non current is not supported.");
}

bool Framebuffer::linearizeDepthFrame(Frame& /*depthFrame*/, const Scalar /*nearDistance*/, const Scalar /*farDistance*/) const
{
	return false;
}

bool Framebuffer::initialize(const FramebufferRef& /*shareFramebuffer*/)
{
	return true;
}

void Framebuffer::release()
{
	clearScenes();

	setView(ViewRef());
}

Framebuffer::ObjectType Framebuffer::type() const
{
	return TYPE_FRAMEBUFFER;
}

}

}
