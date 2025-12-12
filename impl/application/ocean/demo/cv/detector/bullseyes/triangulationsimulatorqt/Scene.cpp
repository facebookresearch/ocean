/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/bullseyes/triangulationsimulatorqt/Scene.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/RandomI.h"

#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"

#include "ocean/rendering/Utilities.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/BlendAttribute.h"
#include "ocean/rendering/Cone.h"
#include "ocean/rendering/Cylinder.h"
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

	// Create parallel (orthographic) view
	parallelView_ = engine_->factory().createParallelView();
	if (parallelView_)
	{
		parallelView_->setNearDistance(Scalar(0.01));
		parallelView_->setFarDistance(Scalar(100.0));
		parallelView_->setWidth(Scalar(4.0));  // Default view width in world units
		parallelView_->setBackgroundColor(RGBAColor(0.1f, 0.1f, 0.15f));
	}

	// Set initial view to perspective
	framebuffer_->setView(perspectiveView_);

	// Set two-sided lighting and disable culling to handle geometry with incorrect normals
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
		// Get transformation from the current active view
		const Rendering::ViewRef currentView = framebuffer_->view();
		if (currentView)
		{
			const HomogenousMatrix4 world_T_view = currentView->transformation();
			const Vector3 right = world_T_view.rotation() * Vector3(1, 0, 0);
			const Vector3 up = world_T_view.rotation() * Vector3(0, 1, 0);

			orbitCenter_ -= right * delta.x() * Scalar(0.001);
			orbitCenter_ += up * delta.y() * Scalar(0.001);

			updateCameraTransform();
		}
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
	isOrbiting_ = false;
	orbitRotation_ = Quaternion(Vector3(1, 0, 0), Numeric::deg2rad(-30)) * Quaternion(Vector3(0, 1, 0), Numeric::deg2rad(-30));
	orbitDistance_ = Scalar(3.0);
	orbitCenter_ = Vector3(0, 0, -(config_.frontDepthMeters + config_.backDepthMeters) * Scalar(0.5));

	updateCameraTransform();
}

void Scene::setCameraTop()
{
	isOrbiting_ = false;
	// Look down from above (-90 deg around X)
	orbitRotation_ = Quaternion(Vector3(1, 0, 0), Numeric::deg2rad(-90));
	orbitDistance_ = Scalar(2.0);
	orbitCenter_ = Vector3(0, 0, -(config_.frontDepthMeters + config_.backDepthMeters) * Scalar(0.5));

	updateCameraTransform();
}

void Scene::setCameraBottom()
{
	isOrbiting_ = false;
	// Look up from below (+90 deg around X), with 180 deg rotation around Z for consistent orientation
	orbitRotation_ = Quaternion(Vector3(1, 0, 0), Numeric::deg2rad(90)) * Quaternion(Vector3(0, 0, 1), Numeric::pi());
	orbitDistance_ = Scalar(2.0);
	orbitCenter_ = Vector3(0, 0, -(config_.frontDepthMeters + config_.backDepthMeters) * Scalar(0.5));

	updateCameraTransform();
}

void Scene::setCameraLeft()
{
	isOrbiting_ = false;
	orbitRotation_ = Quaternion(Vector3(0, 1, 0), Numeric::deg2rad(-90));
	orbitDistance_ = Scalar(2.0);
	orbitCenter_ = Vector3(0, 0, -(config_.frontDepthMeters + config_.backDepthMeters) * Scalar(0.5));

	updateCameraTransform();
}

void Scene::setCameraRight()
{
	isOrbiting_ = false;
	orbitRotation_ = Quaternion(Vector3(0, 1, 0), Numeric::deg2rad(90));
	orbitDistance_ = Scalar(2.0);
	orbitCenter_ = Vector3(0, 0, -(config_.frontDepthMeters + config_.backDepthMeters) * Scalar(0.5));

	updateCameraTransform();
}

void Scene::setCameraFront()
{
	isOrbiting_ = false;
	orbitRotation_ = Quaternion(true);  // Identity - looking along -Z
	orbitDistance_ = Scalar(2.0);
	orbitCenter_ = Vector3(0, 0, -(config_.frontDepthMeters + config_.backDepthMeters) * Scalar(0.5));

	updateCameraTransform();
}

void Scene::setCameraBack()
{
	isOrbiting_ = false;
	orbitRotation_ = Quaternion(Vector3(0, 1, 0), Numeric::pi());
	orbitDistance_ = Scalar(2.0);
	orbitCenter_ = Vector3(0, 0, -(config_.frontDepthMeters + config_.backDepthMeters) * Scalar(0.5));

	updateCameraTransform();
}

void Scene::setOrbiting(bool orbiting)
{
	if (orbiting && !isOrbiting_)
	{
		// Start orbiting from default position
		// The default view has a -30 degree Y rotation, so start from that angle
		orbitAngle_ = Numeric::deg2rad(Scalar(-30));
		orbitDistance_ = Scalar(3.0);
		orbitCenter_ = Vector3(0, 0, -(config_.frontDepthMeters + config_.backDepthMeters) * Scalar(0.5));
	}
	isOrbiting_ = orbiting;
}

bool Scene::isOrbiting() const
{
	return isOrbiting_;
}

void Scene::setProjectionMode(ProjectionMode mode)
{
	if (projectionMode_ == mode)
	{
		return;
	}

	projectionMode_ = mode;

	if (framebuffer_.isNull())
	{
		return;
	}

	if (mode == ProjectionMode::PERSPECTIVE)
	{
		if (perspectiveView_)
		{
			framebuffer_->setView(perspectiveView_);
		}
	}
	else
	{
		// Use true orthographic projection via ParallelView
		if (parallelView_)
		{
			framebuffer_->setView(parallelView_);
		}
	}

	// Update camera transform for the new view
	updateCameraTransform();
}

ProjectionMode Scene::projectionMode() const
{
	return projectionMode_;
}

void Scene::updateOrbiting()
{
	if (!isOrbiting_)
	{
		return;
	}

	// Slowly orbit around Y axis (half speed: 0.25 degrees per frame)
	orbitAngle_ += Numeric::deg2rad(Scalar(0.25));
	if (orbitAngle_ > Numeric::pi2())
	{
		orbitAngle_ -= Numeric::pi2();
	}

	// Orbit in xz-plane with fixed y height
	// Camera position: orbit around scene center at fixed height
	const Scalar cameraHeight = orbitDistance_ * Numeric::sin(Numeric::deg2rad(Scalar(30)));  // Match default view tilt
	const Scalar horizontalRadius = orbitDistance_ * Numeric::cos(Numeric::deg2rad(Scalar(30)));

	const Scalar cameraX = orbitCenter_.x() + horizontalRadius * Numeric::sin(orbitAngle_);
	const Scalar cameraZ = orbitCenter_.z() + horizontalRadius * Numeric::cos(orbitAngle_);
	const Vector3 cameraPosition(cameraX, orbitCenter_.y() + cameraHeight, cameraZ);

	// Look at orbit center
	const Vector3 lookDirection = (orbitCenter_ - cameraPosition).normalizedOrZero();

	// Build camera orientation
	Vector3 cameraZAxis = -lookDirection;  // Camera looks along -Z
	Vector3 cameraXAxis = Vector3(0, 1, 0).cross(cameraZAxis).normalizedOrZero();
	Vector3 cameraYAxis = cameraZAxis.cross(cameraXAxis);

	const SquareMatrix3 rotation(cameraXAxis, cameraYAxis, cameraZAxis);

	// Update view transform
	const HomogenousMatrix4 transform(cameraPosition, rotation);
	if (perspectiveView_)
	{
		perspectiveView_->setTransformation(transform);
	}
	if (parallelView_)
	{
		parallelView_->setTransformation(transform);
		parallelView_->setWidth(orbitDistance_ * Scalar(1.5));
	}
}

void Scene::updateCameraTransform()
{
	// Calculate camera position from orbit parameters
	const Vector3 offset = orbitRotation_ * Vector3(0, 0, orbitDistance_);
	const Vector3 cameraPosition = orbitCenter_ + offset;

	// Calculate camera orientation (look at orbit center)
	// These are the camera's local coordinate axes in world space
	const Vector3 cameraZAxis = (cameraPosition - orbitCenter_).normalizedOrZero();  // Camera looks along -Z

	// Determine the up reference for computing camera orientation
	// For most views, use world Y as up. For top/bottom views (looking along Y axis),
	// use the rotated up reference to allow Z-axis rotation to be respected.
	Vector3 upReference;
	const Scalar dotWithWorldY = Numeric::abs(cameraZAxis.y());
	if (dotWithWorldY > Scalar(0.99))
	{
		// Looking nearly straight up or down - use rotated up reference
		upReference = orbitRotation_ * Vector3(0, 1, 0);
	}
	else
	{
		// Normal view - use world Y as up for a level horizon
		upReference = Vector3(0, 1, 0);
	}

	// Compute camera X axis perpendicular to both Z axis and up reference
	Vector3 cameraXAxis = upReference.cross(cameraZAxis).normalizedOrZero();

	// Handle degenerate case when upReference is parallel to cameraZAxis
	if (cameraXAxis.isNull())
	{
		// Fall back to world Z as reference
		cameraXAxis = Vector3(0, 0, cameraZAxis.y() > 0 ? 1 : -1).cross(cameraZAxis).normalizedOrZero();
	}

	const Vector3 cameraYAxis = cameraZAxis.cross(cameraXAxis);

	const SquareMatrix3 rotation(cameraXAxis, cameraYAxis, cameraZAxis);

	// Update view transform
	const HomogenousMatrix4 transform(cameraPosition, rotation);

	// Update both views with the same transform
	if (perspectiveView_)
	{
		perspectiveView_->setTransformation(transform);
	}

	if (parallelView_)
	{
		parallelView_->setTransformation(transform);

		// Also update the parallel view's width based on orbit distance to maintain similar framing
		// This makes the orthographic view show approximately the same content as perspective at the focus point
		parallelView_->setWidth(orbitDistance_ * Scalar(1.5));
	}
}

SharedAnyCamera Scene::createCameraFromConfig(const CameraConfig& config)
{
	// Compute focal length from HFOV: tan(hfov/2) = (width/2) / focalLength
	// => focalLength = (width/2) / tan(hfov/2)
	const Scalar hfovRadians = Numeric::deg2rad(config.hfovDegrees);
	const Scalar focalLength = Scalar(config.width) * Scalar(0.5) / Numeric::tan(hfovRadians * Scalar(0.5));

	const Scalar principalX = Scalar(config.width) * Scalar(0.5);
	const Scalar principalY = Scalar(config.height) * Scalar(0.5);

	if (config.type == AnyCameraType::FISHEYE)
	{
		// Use FisheyeCamera with equidistant projection (no distortion for now)
		const Scalar radialDistortion[6] = {Scalar(0), Scalar(0), Scalar(0), Scalar(0), Scalar(0), Scalar(0)};
		const Scalar tangentialDistortion[2] = {Scalar(0), Scalar(0)};

		return std::make_shared<AnyCameraFisheye>(FisheyeCamera(config.width, config.height, focalLength, focalLength, principalX, principalY, radialDistortion, tangentialDistortion));
	}

	// Default: Pinhole camera
	return std::make_shared<AnyCameraPinhole>(PinholeCamera(config.width, config.height, focalLength, focalLength, principalX, principalY));
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

void Scene::runSimulation()
{
	gridPoints_.clear();
	triangulatedPoints_.clear();
	pointErrors_.clear();

	// Create cameras from configuration
	const SharedAnyCamera leftCamera = createCameraFromConfig(config_.leftCamera);
	const SharedAnyCamera rightCamera = createCameraFromConfig(config_.rightCamera);

	// Camera transformations (world_T_camera)
	// Device origin is at center between cameras
	// Left camera at -baseline/2, Right camera at +baseline/2
	const Scalar halfBaseline = config_.baselineMeters * Scalar(0.5);
	const HomogenousMatrix4 world_T_leftCamera(Vector3(-halfBaseline, 0, 0));
	const HomogenousMatrix4 world_T_rightCamera(Vector3(halfBaseline, 0, 0));

	const HomogenousMatrix4 flippedLeftCamera_T_world = AnyCamera::standard2InvertedFlipped(world_T_leftCamera);
	const HomogenousMatrix4 flippedRightCamera_T_world = AnyCamera::standard2InvertedFlipped(world_T_rightCamera);

	// Generate point grid - use generous bounds centered at device origin
	// Points are at negative Z values (in front of cameras which look toward -Z)
	// Use back depth to determine grid extent (larger depth = wider FOV coverage needed)
	const Scalar maxHalfWidth = config_.backDepthMeters * Scalar(1.5);  // Generous extent

	for (Scalar z = -config_.backDepthMeters; z <= -config_.frontDepthMeters + Numeric::eps(); z += config_.spacingMeters)
	{
		for (Scalar x = -maxHalfWidth; x <= maxHalfWidth + Numeric::eps(); x += config_.spacingMeters)
		{
			for (Scalar y = -maxHalfWidth; y <= maxHalfWidth + Numeric::eps(); y += config_.spacingMeters)
			{
				const Vector3 worldPoint(x, y, z);

				// Check visibility in both cameras - only use points visible from BOTH
				if (!leftCamera->isObjectPointInFrontIF(flippedLeftCamera_T_world, worldPoint) || !rightCamera->isObjectPointInFrontIF(flippedRightCamera_T_world, worldPoint))
				{
					continue;
				}

				const Vector2 leftProjection = leftCamera->projectToImageIF(flippedLeftCamera_T_world, worldPoint);
				const Vector2 rightProjection = rightCamera->projectToImageIF(flippedRightCamera_T_world, worldPoint);

				if (!leftCamera->isInside(leftProjection) || !rightCamera->isInside(rightProjection))
				{
					continue;
				}

				// Simulate triangulation with noise/offset applied
				// Error metric: angular error (radians) - angle between ray to ground truth and ray to triangulated point
				// as seen from the perturbed camera(s). If both cameras are perturbed, use the larger angle.
				Scalar maxError = 0;
				Vector3 maxErrorTriangulatedPoint = worldPoint;

				// Camera positions for angular error computation
				const Vector3 leftCameraPosition = world_T_leftCamera.translation();
				const Vector3 rightCameraPosition = world_T_rightCamera.translation();

				// Helper lambda to compute angular error from a camera position
				auto computeAngularError = [](const Vector3& cameraPosition, const Vector3& groundTruthPoint, const Vector3& triangulatedPoint) -> Scalar
				{
					const Vector3 rayToGroundTruth = (groundTruthPoint - cameraPosition).normalizedOrZero();
					const Vector3 rayToTriangulated = (triangulatedPoint - cameraPosition).normalizedOrZero();

					// Angle between rays: acos(dot product), clamped to avoid numerical issues
					const Scalar dotProduct = std::max(Scalar(-1), std::min(Scalar(1), rayToGroundTruth * rayToTriangulated));
					return Numeric::acos(dotProduct);
				};

				if (config_.randomize)
				{
					// Random mode: run multiple trials and retain max error
					for (unsigned int trial = 0u; trial < config_.numRepetitions; ++trial)
					{
						// Generate uniform noise in range [-delta, +delta]
						Scalar noiseLeftX = Scalar(0);
						Scalar noiseLeftY = Scalar(0);
						Scalar noiseRightX = Scalar(0);
						Scalar noiseRightY = Scalar(0);

						// Apply noise based on perturbation mode
						if (config_.perturbationMode == PerturbationMode::BOTH || config_.perturbationMode == PerturbationMode::LEFT_ONLY)
						{
							noiseLeftX = Random::scalar(randomGenerator_, -config_.deltaX, config_.deltaX);
							noiseLeftY = Random::scalar(randomGenerator_, -config_.deltaY, config_.deltaY);
						}

						if (config_.perturbationMode == PerturbationMode::BOTH || config_.perturbationMode == PerturbationMode::RIGHT_ONLY)
						{
							noiseRightX = Random::scalar(randomGenerator_, -config_.deltaX, config_.deltaX);
							noiseRightY = Random::scalar(randomGenerator_, -config_.deltaY, config_.deltaY);
						}

						const Vector2 noisyLeftProjection(leftProjection.x() + noiseLeftX, leftProjection.y() + noiseLeftY);
						const Vector2 noisyRightProjection(rightProjection.x() + noiseRightX, rightProjection.y() + noiseRightY);

						// Triangulate using rays
						const Line3 leftRay = leftCamera->ray(noisyLeftProjection, world_T_leftCamera);
						const Line3 rightRay = rightCamera->ray(noisyRightProjection, world_T_rightCamera);

						Vector3 triangulatedPoint;
						if (leftRay.nearestPoint(rightRay, triangulatedPoint))
						{
							// Compute angular error from perturbed camera(s)
							Scalar angularError = 0;

							if (config_.perturbationMode == PerturbationMode::LEFT_ONLY)
							{
								angularError = computeAngularError(leftCameraPosition, worldPoint, triangulatedPoint);
							}
							else if (config_.perturbationMode == PerturbationMode::RIGHT_ONLY)
							{
								angularError = computeAngularError(rightCameraPosition, worldPoint, triangulatedPoint);
							}
							else // BOTH
							{
								const Scalar leftAngularError = computeAngularError(leftCameraPosition, worldPoint, triangulatedPoint);
								const Scalar rightAngularError = computeAngularError(rightCameraPosition, worldPoint, triangulatedPoint);
								angularError = std::max(leftAngularError, rightAngularError);
							}

							if (angularError > maxError)
							{
								maxError = angularError;
								maxErrorTriangulatedPoint = triangulatedPoint;
							}
						}
					}
				}
				else
				{
					// Fixed mode: apply exact delta offset once
					Scalar offsetLeftX = Scalar(0);
					Scalar offsetLeftY = Scalar(0);
					Scalar offsetRightX = Scalar(0);
					Scalar offsetRightY = Scalar(0);

					// Apply offset based on perturbation mode
					if (config_.perturbationMode == PerturbationMode::BOTH || config_.perturbationMode == PerturbationMode::LEFT_ONLY)
					{
						offsetLeftX = config_.deltaX;
						offsetLeftY = config_.deltaY;
					}

					if (config_.perturbationMode == PerturbationMode::BOTH || config_.perturbationMode == PerturbationMode::RIGHT_ONLY)
					{
						offsetRightX = config_.deltaX;
						offsetRightY = config_.deltaY;
					}

					const Vector2 offsetLeftProjection(leftProjection.x() + offsetLeftX, leftProjection.y() + offsetLeftY);
					const Vector2 offsetRightProjection(rightProjection.x() + offsetRightX, rightProjection.y() + offsetRightY);

					// Triangulate using rays
					const Line3 leftRay = leftCamera->ray(offsetLeftProjection, world_T_leftCamera);
					const Line3 rightRay = rightCamera->ray(offsetRightProjection, world_T_rightCamera);

					Vector3 triangulatedPoint;
					if (leftRay.nearestPoint(rightRay, triangulatedPoint))
					{
						// Compute angular error from perturbed camera(s)
						if (config_.perturbationMode == PerturbationMode::LEFT_ONLY)
						{
							maxError = computeAngularError(leftCameraPosition, worldPoint, triangulatedPoint);
						}
						else if (config_.perturbationMode == PerturbationMode::RIGHT_ONLY)
						{
							maxError = computeAngularError(rightCameraPosition, worldPoint, triangulatedPoint);
						}
						else // BOTH
						{
							const Scalar leftAngularError = computeAngularError(leftCameraPosition, worldPoint, triangulatedPoint);
							const Scalar rightAngularError = computeAngularError(rightCameraPosition, worldPoint, triangulatedPoint);
							maxError = std::max(leftAngularError, rightAngularError);
						}

						maxErrorTriangulatedPoint = triangulatedPoint;
					}
				}

				gridPoints_.push_back(worldPoint);
				triangulatedPoints_.push_back(maxErrorTriangulatedPoint);
				pointErrors_.push_back(maxError);
			}
		}
	}

	// Compute statistics
	if (!pointErrors_.empty())
	{
		std::vector<Scalar> sortedErrors = pointErrors_;
		std::sort(sortedErrors.begin(), sortedErrors.end());

		Scalar sum = 0;
		for (const Scalar& error : sortedErrors)
		{
			sum += error;
		}

		stats_.meanError = sum / Scalar(sortedErrors.size());
		stats_.p50Error = sortedErrors[sortedErrors.size() / 2];
		stats_.p90Error = sortedErrors[size_t(Scalar(sortedErrors.size()) * Scalar(0.9))];
		stats_.p95Error = sortedErrors[size_t(Scalar(sortedErrors.size()) * Scalar(0.95))];
		stats_.numValidPoints = sortedErrors.size();
	}
	else
	{
		stats_ = SimulationStats();
	}
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
	if (pointsTransform_)
	{
		scene_->removeChild(pointsTransform_);
		pointsTransform_.release();
	}
	if (errorLinesTransform_)
	{
		scene_->removeChild(errorLinesTransform_);
		errorLinesTransform_.release();
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

	// Create colorized points at the triangulated (noisy) locations
	if (!triangulatedPoints_.empty() && !pointErrors_.empty())
	{
		// Generate colors based on colorization config
		RGBAColors colors;
		colors.reserve(triangulatedPoints_.size());
		for (size_t i = 0; i < pointErrors_.size(); ++i)
		{
			colors.push_back(heatmapColor(pointErrors_[i], colorizationConfig_));
		}

		pointsTransform_ = Rendering::Utilities::createPoints(*engine_, triangulatedPoints_, RGBAColor(1.0f, 1.0f, 1.0f), Scalar(3), colors);
		if (pointsTransform_)
		{
			scene_->addChild(pointsTransform_);
		}

		// Create error lines connecting ground truth to triangulated points
		if (!triangulatedPoints_.empty())
		{
			// Build vertices and line groups for error visualization
			Vectors3 lineVertices;
			lineVertices.reserve(gridPoints_.size() * 2);

			Rendering::VertexIndexGroups lineGroups;
			lineGroups.reserve(gridPoints_.size());

			for (size_t i = 0; i < gridPoints_.size(); ++i)
			{
				const unsigned int baseIndex = (unsigned int)(lineVertices.size());
				lineVertices.push_back(gridPoints_[i]);
				lineVertices.push_back(triangulatedPoints_[i]);

				Rendering::VertexIndices line;
				line.push_back(baseIndex);
				line.push_back(baseIndex + 1);
				lineGroups.push_back(line);
			}

			// Use a light gray color for the error lines
			errorLinesTransform_ = Rendering::Utilities::createLines(*engine_, lineVertices, lineGroups, RGBAColor(0.7f, 0.7f, 0.7f), Scalar(1));
			if (errorLinesTransform_)
			{
				scene_->addChild(errorLinesTransform_);
			}
		}
	}
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

RGBAColor Scene::heatmapColor(Scalar errorRadians, const ColorizationConfig& config)
{
	// Convert thresholds from degrees to radians
	const Scalar minRadians = Numeric::deg2rad(config.minAngleDegrees);
	const Scalar maxRadians = Numeric::deg2rad(config.maxAngleDegrees);

	// Normalize error to [0, 1]
	const Scalar range = maxRadians - minRadians;
	const Scalar t = (range > Numeric::eps()) ? std::max(Scalar(0), std::min(Scalar(1), (errorRadians - minRadians) / range)) : Scalar(0);

	// Interpolate between lowColor and highColor through yellow
	float r, g, b;

	if (t < Scalar(0.5))
	{
		// Low color to Yellow
		const float localT = float(t * Scalar(2));
		r = config.lowColor.red() + localT * (1.0f - config.lowColor.red());
		g = config.lowColor.green() + localT * (1.0f - config.lowColor.green());
		b = config.lowColor.blue() * (1.0f - localT);
	}
	else
	{
		// Yellow to High color
		const float localT = float((t - Scalar(0.5)) * Scalar(2));
		r = 1.0f + localT * (config.highColor.red() - 1.0f);
		g = 1.0f - localT * (1.0f - config.highColor.green());
		b = localT * config.highColor.blue();
	}

	return RGBAColor(r, g, b);
}

}

}

}

}

}

}
