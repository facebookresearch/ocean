/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/bullseyes/triangulationsimulatorqt/Scene.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/RGBAColor.h"
#include "ocean/math/SquareMatrix3.h"

#include "ocean/rendering/Utilities.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/BlendAttribute.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Material.h"

#include <algorithm>

namespace Ocean
{

namespace Demo
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

namespace TriangulationSimulatorQt
{

Scene::Scene() :
	lastMousePosition_(Numeric::minValue(), Numeric::minValue()),
	orbitRotation_(true),
	orbitCenter_(0, 0, Scalar(-0.5))
{
	// Empty
}

Scene::~Scene()
{
	// Empty
}

bool Scene::initialize(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer)
{
	if (engine.isNull() || framebuffer.isNull())
	{
		return false;
	}

	engine_ = engine;
	framebuffer_ = framebuffer;

	// Create scene
	scene_ = engine_->factory().createScene();
	if (scene_.isNull())
	{
		return false;
	}

	// Create perspective view
	perspectiveView_ = engine_->factory().createPerspectiveView();
	if (perspectiveView_.isNull())
	{
		return false;
	}

	// Set perspective view parameters
	perspectiveView_->setNearDistance(Scalar(0.01));
	perspectiveView_->setFarDistance(Scalar(100.0));
	perspectiveView_->setFovX(Numeric::deg2rad(60));
	perspectiveView_->setBackgroundColor(RGBAColor(0.1f, 0.1f, 0.15f));

	// Set initial view to perspective
	framebuffer_->setView(perspectiveView_);

	// Set lighting mode
	framebuffer_->setLightingMode(Rendering::PrimitiveAttribute::LM_SINGLE_SIDE_LIGHTING);
	framebuffer_->setCullingMode(Rendering::PrimitiveAttribute::CULLING_DEFAULT);

	// Add scene to framebuffer
	framebuffer_->addScene(scene_);

	// Create coordinate axes at origin
	axesTransform_ = Rendering::Utilities::createCoordinateSystem(engine_, Scalar(0.1), Scalar(0.02), Scalar(0.005));
	if (axesTransform_)
	{
		scene_->addChild(axesTransform_);
	}

	// Reset camera to default view
	resetCamera();

	// Run initial simulation
	runSimulation();
	updateVisualization();

	return true;
}

void Scene::updateConfiguration(const SimulationConfig& config)
{
	config_ = config;
	runSimulation();
	updateVisualization();
}

const SimulationConfig& Scene::configuration() const
{
	return config_;
}

const SimulationStats& Scene::statistics() const
{
	return stats_;
}

void Scene::updateColorization(const ColorizationConfig& config)
{
	colorizationConfig_ = config;
	updateVisualization();
}

const ColorizationConfig& Scene::colorization() const
{
	return colorizationConfig_;
}

void Scene::handleMousePress(int /*button*/, const Vector2& position)
{
	lastMousePosition_ = position;
}

void Scene::handleMouseMove(const Vector2& position, int buttons)
{
	if (lastMousePosition_.x() == Numeric::minValue())
	{
		lastMousePosition_ = position;
		return;
	}

	const Vector2 delta = position - lastMousePosition_;

	// Left button: Orbit rotation
	if (buttons & 1)
	{
		const Scalar angleX = delta.x() * Scalar(0.005);
		const Scalar angleY = delta.y() * Scalar(0.005);

		// Rotate around Y-axis (horizontal drag)
		const Quaternion rotY(Vector3(0, 1, 0), -angleX);
		// Rotate around X-axis (vertical drag)
		const Quaternion rotX(Vector3(1, 0, 0), -angleY);

		orbitRotation_ = rotY * orbitRotation_ * rotX;
		orbitRotation_.normalize();

		updateCameraTransform();
	}
	// Right button: Pan
	else if (buttons & 2)
	{
		const HomogenousMatrix4 world_T_view = perspectiveView_->transformation();
		const Vector3 right = world_T_view.rotation() * Vector3(1, 0, 0);
		const Vector3 up = world_T_view.rotation() * Vector3(0, 1, 0);

		orbitCenter_ -= right * delta.x() * Scalar(0.001);
		orbitCenter_ += up * delta.y() * Scalar(0.001);

		updateCameraTransform();
	}

	lastMousePosition_ = position;
}

void Scene::handleMouseWheel(int delta)
{
	// Zoom by changing orbit distance
	const Scalar zoomFactor = Scalar(1.0) - Scalar(delta) * Scalar(0.001);
	orbitDistance_ *= zoomFactor;
	orbitDistance_ = std::max(Scalar(0.1), std::min(orbitDistance_, Scalar(20.0)));

	updateCameraTransform();
}

void Scene::resetCamera()
{
	orbitRotation_ = Quaternion(Vector3(1, 0, 0), Numeric::deg2rad(-30)) * Quaternion(Vector3(0, 1, 0), Numeric::deg2rad(-30));
	orbitDistance_ = Scalar(3.0);
	orbitCenter_ = Vector3(0, 0, Scalar(-0.5));

	updateCameraTransform();
}

void Scene::updateCameraTransform()
{
	// Calculate camera position from orbit parameters
	const Vector3 offset = orbitRotation_ * Vector3(0, 0, orbitDistance_);
	const Vector3 cameraPosition = orbitCenter_ + offset;

	// Calculate camera orientation (look at orbit center)
	const Vector3 cameraZAxis = (cameraPosition - orbitCenter_).normalizedOrZero();

	// Use world Y as up reference for level horizon
	Vector3 upReference = Vector3(0, 1, 0);
	const Scalar dotWithWorldY = Numeric::abs(cameraZAxis.y());
	if (dotWithWorldY > Scalar(0.99))
	{
		// Looking nearly straight up or down - use rotated up reference
		upReference = orbitRotation_ * Vector3(0, 1, 0);
	}

	Vector3 cameraXAxis = upReference.cross(cameraZAxis).normalizedOrZero();
	if (cameraXAxis.isNull())
	{
		cameraXAxis = Vector3(0, 0, cameraZAxis.y() > 0 ? 1 : -1).cross(cameraZAxis).normalizedOrZero();
	}

	const Vector3 cameraYAxis = cameraZAxis.cross(cameraXAxis);

	const SquareMatrix3 rotation(cameraXAxis, cameraYAxis, cameraZAxis);
	const HomogenousMatrix4 transform(cameraPosition, rotation);

	if (perspectiveView_)
	{
		perspectiveView_->setTransformation(transform);
	}
}

void Scene::runSimulation()
{
	// Stub - simulation logic will be added later
}

void Scene::updateVisualization()
{
	if (engine_.isNull() || scene_.isNull())
	{
		return;
	}

	// Remove old geometry
	if (leftCameraTransform_)
	{
		scene_->removeChild(leftCameraTransform_);
		leftCameraTransform_.release();
	}
	if (rightCameraTransform_)
	{
		scene_->removeChild(rightCameraTransform_);
		rightCameraTransform_.release();
	}

	// Create cameras for frustum visualization
	const PinholeCamera leftCamera = createPinholeCameraFromConfig(config_.leftCamera);
	const PinholeCamera rightCamera = createPinholeCameraFromConfig(config_.rightCamera);

	// Camera positions centered around device origin
	const Scalar halfBaseline = config_.baselineMeters * Scalar(0.5);

	// Create left camera frustum (yellow) at -baseline/2
	leftCameraTransform_ = createCameraFrustum(leftCamera, RGBAColor(1.0f, 1.0f, 0.0f), Scalar(0.05), config_.backDepthMeters);
	if (leftCameraTransform_)
	{
		leftCameraTransform_->setTransformation(HomogenousMatrix4(Vector3(-halfBaseline, 0, 0)));
		scene_->addChild(leftCameraTransform_);
	}

	// Create right camera frustum (cyan) at +baseline/2
	rightCameraTransform_ = createCameraFrustum(rightCamera, RGBAColor(0.0f, 1.0f, 1.0f), Scalar(0.05), config_.backDepthMeters);
	if (rightCameraTransform_)
	{
		rightCameraTransform_->setTransformation(HomogenousMatrix4(Vector3(halfBaseline, 0, 0)));
		scene_->addChild(rightCameraTransform_);
	}
}

PinholeCamera Scene::createPinholeCameraFromConfig(const CameraConfig& config)
{
	// Compute focal length from HFOV
	const Scalar hfovRadians = Numeric::deg2rad(config.hfovDegrees);
	const Scalar focalLength = Scalar(config.width) * Scalar(0.5) / Numeric::tan(hfovRadians * Scalar(0.5));

	return PinholeCamera(
		config.width, config.height,
		focalLength, focalLength,
		Scalar(config.width) * Scalar(0.5), Scalar(config.height) * Scalar(0.5));
}

Rendering::TransformRef Scene::createCameraFrustum(const PinholeCamera& camera, const RGBAColor& color, Scalar nearDist, Scalar farDist)
{
	if (engine_.isNull())
	{
		return Rendering::TransformRef();
	}

	// Compute tan of half-angles from camera parameters
	// tan(halfAngleX) = (width/2) / focalLength
	const Scalar tanHalfAngleX = Scalar(camera.width()) * Scalar(0.5) / camera.focalLengthX();
	const Scalar tanHalfAngleY = Scalar(camera.height()) * Scalar(0.5) / camera.focalLengthY();

	// Compute frustum corner offsets at near and far distances
	const Scalar nearHalfWidth = nearDist * tanHalfAngleX;
	const Scalar nearHalfHeight = nearDist * tanHalfAngleY;
	const Scalar farHalfWidth = farDist * tanHalfAngleX;
	const Scalar farHalfHeight = farDist * tanHalfAngleY;

	Vectors3 vertices;
	vertices.reserve(9);

	// Camera origin
	vertices.push_back(Vector3(0, 0, 0));  // 0

	// Near plane corners (camera looks along -Z)
	vertices.push_back(Vector3(-nearHalfWidth, -nearHalfHeight, -nearDist));  // 1: bottom-left
	vertices.push_back(Vector3(nearHalfWidth, -nearHalfHeight, -nearDist));   // 2: bottom-right
	vertices.push_back(Vector3(nearHalfWidth, nearHalfHeight, -nearDist));    // 3: top-right
	vertices.push_back(Vector3(-nearHalfWidth, nearHalfHeight, -nearDist));   // 4: top-left

	// Far plane corners
	vertices.push_back(Vector3(-farHalfWidth, -farHalfHeight, -farDist));  // 5: bottom-left
	vertices.push_back(Vector3(farHalfWidth, -farHalfHeight, -farDist));   // 6: bottom-right
	vertices.push_back(Vector3(farHalfWidth, farHalfHeight, -farDist));    // 7: top-right
	vertices.push_back(Vector3(-farHalfWidth, farHalfHeight, -farDist));   // 8: top-left

	// Create camera part lines (origin to near plane + near plane edges) - full opacity
	Rendering::VertexIndexGroups cameraLineGroups;

	// Lines from origin to near plane corners
	for (unsigned int i = 0; i < 4; ++i)
	{
		Rendering::VertexIndices line;
		line.push_back(0);
		line.push_back(i + 1);
		cameraLineGroups.push_back(line);
	}

	// Near plane edges
	for (unsigned int i = 0; i < 4; ++i)
	{
		Rendering::VertexIndices line;
		line.push_back(i + 1);
		line.push_back((i + 1) % 4 + 1);
		cameraLineGroups.push_back(line);
	}

	// Create extended part lines (near to far edges + far plane edges) - semi-transparent
	Rendering::VertexIndexGroups extendedLineGroups;

	// Far plane edges
	for (unsigned int i = 0; i < 4; ++i)
	{
		Rendering::VertexIndices line;
		line.push_back(i + 5);
		line.push_back((i + 1) % 4 + 5);
		extendedLineGroups.push_back(line);
	}

	// Lines from near to far plane corners
	for (unsigned int i = 0; i < 4; ++i)
	{
		Rendering::VertexIndices line;
		line.push_back(i + 1);
		line.push_back(i + 5);
		extendedLineGroups.push_back(line);
	}

	// Create wireframes with different opacities
	Rendering::TransformRef cameraWireframe = Rendering::Utilities::createLines(*engine_, vertices, cameraLineGroups, color, Scalar(1));

	// Semi-transparent color for extended frustum (0.25 opacity)
	const RGBAColor transparentColor(color.red(), color.green(), color.blue(), 0.1f);
	Rendering::TransformRef extendedWireframe = Rendering::Utilities::createLines(*engine_, vertices, extendedLineGroups, transparentColor, Scalar(1));

	// Create a parent transform to hold both wireframes
	Rendering::TransformRef result = engine_->factory().createTransform();
	if (cameraWireframe)
	{
		result->addChild(cameraWireframe);
	}
	if (extendedWireframe)
	{
		result->addChild(extendedWireframe);
	}

	return result;
}

}

}

}

}

}

}
