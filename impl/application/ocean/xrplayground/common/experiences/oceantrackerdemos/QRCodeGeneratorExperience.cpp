// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/QRCodeGeneratorExperience.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"
#include "ocean/rendering/Textures.h"

namespace Ocean
{

namespace XRPlayground
{

QRCodeGeneratorExperience::~QRCodeGeneratorExperience()
{
	// nothing to do here
}

bool QRCodeGeneratorExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	assert(engine);

	const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
	absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
	absoluteTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -1)));

	experienceScene()->addChild(absoluteTransform);

	Rendering::AttributeSetRef attributeSet;
	renderingBoxTransform_ = Rendering::Utilities::createBox(engine, Vector3(1, 1, 0), nullptr, &attributeSet);
	absoluteTransform->addChild(renderingBoxTransform_);

	Rendering::TexturesRef textures = engine->factory().createTextures();
	renderingFrameTexture_ = engine->factory().createFrameTexture2D();
	renderingFrameTexture_->setUseMipmaps(true);
	textures->addTexture(renderingFrameTexture_);

	attributeSet->addAttribute(textures);

	// let's cover the video background with a fully opaque, white sphere
	Rendering::AttributeSetRef sphereAttributeSet;
	Rendering::MaterialRef sphereMaterial;
	experienceScene()->addChild(Rendering::Utilities::createSphere(engine, Scalar(100), RGBAColor(1.0f, 1.0f, 1.0f), nullptr, &sphereAttributeSet, &sphereMaterial));
	sphereMaterial->setEmissiveColor(RGBAColor(1.0f, 1.0f, 1.0f));
	Rendering::PrimitiveAttributeRef spherePrimitiveAttributeSet = engine->factory().createPrimitiveAttribute();
	spherePrimitiveAttributeSet->setCullingMode(Rendering::PrimitiveAttribute::CULLING_NONE);
	spherePrimitiveAttributeSet->setLightingMode(Rendering::PrimitiveAttribute::LM_TWO_SIDED_LIGHTING);
	sphereAttributeSet->addAttribute(spherePrimitiveAttributeSet);

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (userInterface.isIOS())
	{
		showUserInterinterfaceIOS(userInterface);
	}
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	return true;
}

bool QRCodeGeneratorExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (userInterface.isIOS())
	{
		unloadUserInterinterfaceIOS(userInterface);
	}
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	renderingFrameTexture_.release();

	return true;
}

Timestamp QRCodeGeneratorExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedCodeFrameLock(codeFrameLock_);

		Frame codeFrame = std::move(codeFrame_);

	scopedCodeFrameLock.release();

	if (codeFrame.isValid())
	{
		const Vector3 scale(Scalar(0.65), Scalar(0.65), 1);
		renderingBoxTransform_->setTransformation(HomogenousMatrix4(Vector3(0, 0, 0), scale));

		renderingFrameTexture_->setTexture(std::move(codeFrame));
	}

	return timestamp;
}

bool QRCodeGeneratorExperience::generateQRCodeFrame(const std::string& codePayload, unsigned int& codeVersion)
{
	ocean_assert(!codePayload.empty());

	CV::Detector::QRCodes::QRCode code;

	if (!CV::Detector::QRCodes::QRCodeEncoder::encodeText(codePayload, CV::Detector::QRCodes::QRCode::ECC_07, code))
	{
		Log::warning() << "QR code generation failed.";
		return false;
	}

	const Frame yFrame = CV::Detector::QRCodes::Utilities::draw(code, /* frameSize */ 1000u, /* allowTrueMultiple */ true, /* border */ 2u, WorkerPool::get().scopedWorker()());
	ocean_assert(yFrame.isValid());

	Frame rgbFrame;
	CV::FrameConverter::Comfort::convert(yFrame, FrameType(yFrame, FrameType::FORMAT_RGB24), rgbFrame, WorkerPool::get().scopedWorker()());
	ocean_assert(rgbFrame.isValid());

	TemporaryScopedLock scopedCodeFrameLock(codeFrameLock_);

	codeFrame_ = std::move(rgbFrame);

	codeVersion = code.version();

	return true;
}

std::unique_ptr<XRPlaygroundExperience> QRCodeGeneratorExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new QRCodeGeneratorExperience());
}

}

}
