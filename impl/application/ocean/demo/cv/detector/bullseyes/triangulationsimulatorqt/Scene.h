/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_SCENE_H
#define META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_SCENE_H

#include "ocean/base/RandomGenerator.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/RGBAColor.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/ParallelView.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/PointLight.h"
#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/View.h"

#include <utility>
#include <vector>

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

/**
 * Perturbation mode for noise simulation.
 */
enum class PerturbationMode : uint32_t
{
	/// Apply noise to both cameras
	BOTH = 0u,
	/// Apply noise to left camera only
	LEFT_ONLY = 1u,
	/// Apply noise to right camera only
	RIGHT_ONLY = 2u
};

/**
 * Projection mode for scene rendering.
 */
enum class ProjectionMode : uint32_t
{
	/// Perspective projection (realistic)
	PERSPECTIVE = 0u,
	/// Orthogonal/parallel projection
	ORTHOGONAL = 1u
};

/**
 * Configuration for a single camera.
 */
struct CameraConfig
{
	/// Camera type (pinhole or fisheye)
	AnyCameraType type = AnyCameraType::PINHOLE;

	/// Image width in pixels
	unsigned int width = 400u;

	/// Image height in pixels
	unsigned int height = 400u;

	/// Horizontal field of view in degrees
	Scalar hfovDegrees = Scalar(60.0);
};

/**
 * This struct holds the simulation configuration parameters.
 */
struct SimulationConfig
{
	/// Front depth (closest point) in meters
	Scalar frontDepthMeters = Scalar(0.25);

	/// Back depth (farthest point) in meters
	Scalar backDepthMeters = Scalar(1.0);

	/// Spacing between grid points in meters
	Scalar spacingMeters = Scalar(0.15);

	/// Baseline between cameras in meters
	Scalar baselineMeters = Scalar(0.14);

	/// Left camera configuration (default: RGB camera specs - IMX681)
	/// 2016x1512, 100° HFOV
	CameraConfig leftCamera;

	/// Right camera configuration (default: CV camera specs - OG0TD1B)
	/// 400x400, 119° HFOV
	CameraConfig rightCamera;

	/// Whether to use random offsets (true) or fixed offsets (false)
	bool randomize = false;

	/// Perturbation mode (which cameras get noise applied)
	PerturbationMode perturbationMode = PerturbationMode::RIGHT_ONLY;

	/// Number of repetitions per point (only used when randomize is true)
	unsigned int numRepetitions = 100u;

	/// Delta X in pixels (range meaning depends on randomize flag)
	/// When randomize=true: random in [-deltaX, +deltaX]
	/// When randomize=false: exact offset
	Scalar deltaX = Scalar(1.0);

	/// Delta Y in pixels (range meaning depends on randomize flag)
	/// When randomize=true: random in [-deltaY, +deltaY]
	/// When randomize=false: exact offset
	Scalar deltaY = Scalar(0.0);

	/// Whether to filter points to a cone volume
	bool useConeFilter = false;

	/// Half-angle of the cone in degrees (from -Z axis)
	Scalar coneHalfAngleDegrees = Scalar(25.0);
};

/**
 * This struct holds the simulation result statistics.
 */
struct SimulationStats
{
	/// Mean error in radians
	Scalar meanError = Scalar(0);

	/// Median (P50) error in radians
	Scalar p50Error = Scalar(0);

	/// P90 error in radians
	Scalar p90Error = Scalar(0);

	/// P95 error in radians
	Scalar p95Error = Scalar(0);

	/// Number of valid points
	size_t numValidPoints = 0;
};

/**
 * This struct holds the colorization configuration parameters.
 */
struct ColorizationConfig
{
	/// Minimum error angle in degrees (maps to lowColor)
	Scalar minAngleDegrees = Scalar(0.0);

	/// Maximum error angle in degrees (maps to highColor)
	Scalar maxAngleDegrees = Scalar(0.5);

	/// Color for low error values
	RGBAColor lowColor = RGBAColor(0.0f, 1.0f, 0.0f);  // Green

	/// Color for high error values
	RGBAColor highColor = RGBAColor(1.0f, 0.0f, 0.0f);  // Red
};

/**
 * This class implements the 3D scene rendering and simulation logic for the triangulation simulator.
 */
class Scene
{
	public:

		/**
		 * Creates a new scene object.
		 */
		Scene();

		/**
		 * Destructs the scene object.
		 */
		~Scene();

		/**
		 * Initializes the scene with a framebuffer.
		 * @param engine The rendering engine to use
		 * @param framebuffer The framebuffer to render into
		 * @return True if initialization succeeded
		 */
		bool initialize(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer);

		/**
		 * Updates the simulation configuration and re-runs the simulation.
		 * @param config The new configuration to use
		 */
		void updateConfiguration(const SimulationConfig& config);

		/**
		 * Returns the current simulation configuration.
		 * @return The current configuration
		 */
		const SimulationConfig& configuration() const;

		/**
		 * Returns the current simulation statistics.
		 * @return The current statistics
		 */
		const SimulationStats& statistics() const;

		/**
		 * Updates the colorization configuration and refreshes the visualization.
		 * This does not re-run the simulation, only updates the point colors.
		 * @param config The new colorization configuration
		 */
		void updateColorization(const ColorizationConfig& config);

		/**
		 * Returns the current colorization configuration.
		 * @return The current colorization configuration
		 */
		const ColorizationConfig& colorization() const;

		/**
		 * Handles a mouse press event.
		 * @param button The mouse button that was pressed
		 * @param position The position of the mouse in screen coordinates
		 */
		void handleMousePress(int button, const Vector2& position);

		/**
		 * Handles a mouse move event.
		 * @param position The current position of the mouse in screen coordinates
		 * @param buttons The mouse buttons that are currently pressed (bitmask)
		 */
		void handleMouseMove(const Vector2& position, int buttons);

		/**
		 * Handles a mouse wheel event.
		 * @param delta The scroll delta
		 */
		void handleMouseWheel(int delta);

		/**
		 * Resets the camera to the default view.
		 */
		void resetCamera();

		/**
		 * Sets the camera to a top-down view.
		 */
		void setCameraTop();

		/**
		 * Sets the camera to a bottom-up view.
		 */
		void setCameraBottom();

		/**
		 * Sets the camera to a left side view.
		 */
		void setCameraLeft();

		/**
		 * Sets the camera to a right side view.
		 */
		void setCameraRight();

		/**
		 * Sets the camera to a front view.
		 */
		void setCameraFront();

		/**
		 * Sets the camera to a back view.
		 */
		void setCameraBack();

		/**
		 * Starts or stops the orbiting animation.
		 * @param orbiting True to start orbiting, false to stop
		 */
		void setOrbiting(bool orbiting);

		/**
		 * Returns whether the camera is currently orbiting.
		 * @return True if orbiting
		 */
		bool isOrbiting() const;

		/**
		 * Updates the orbiting animation (call each frame).
		 */
		void updateOrbiting();

		/**
		 * Sets the projection mode.
		 * @param mode The projection mode to set
		 */
		void setProjectionMode(ProjectionMode mode);

		/**
		 * Returns the current projection mode.
		 * @return The current projection mode
		 */
		ProjectionMode projectionMode() const;

	protected:

		/**
		 * Runs the triangulation simulation.
		 */
		void runSimulation();

		/**
		 * Updates the 3D visualization based on simulation results.
		 */
		void updateVisualization();

		/**
		 * Creates the camera frustum geometry.
		 * @param camera The camera to create the frustum for
		 * @param color The color of the frustum lines
		 * @param nearDist The near plane distance
		 * @param farDist The far plane distance
		 * @return The transform node containing the frustum geometry
		 */
		Rendering::TransformRef createCameraFrustum(const PinholeCamera& camera, const RGBAColor& color, Scalar nearDist, Scalar farDist);

		/**
		 * Updates the camera transform based on orbit parameters.
		 */
		void updateCameraTransform();

		/**
		 * Creates a wireframe cone visualization.
		 * @param halfAngleRadians The half-angle of the cone in radians
		 * @param length The length of the cone (depth)
		 * @return The transform node containing the cone geometry
		 */
		Rendering::TransformRef createConeVisualization(Scalar halfAngleRadians, Scalar length);

		/**
		 * Creates an AnyCamera from a camera configuration.
		 * @param config The camera configuration
		 * @return The created camera
		 */
		static SharedAnyCamera createCameraFromConfig(const CameraConfig& config);

		/**
		 * Creates a PinholeCamera from a camera configuration (for visualization).
		 * @param config The camera configuration
		 * @return The created pinhole camera
		 */
		static PinholeCamera createPinholeCameraFromConfig(const CameraConfig& config);

		/**
		 * Computes a heatmap color based on error value using the colorization config.
		 * @param errorRadians The error value in radians
		 * @param config The colorization configuration
		 * @return The heatmap color
		 */
		static RGBAColor heatmapColor(Scalar errorRadians, const ColorizationConfig& config);

		/**
		 * Computes the angular error between ground truth and triangulated point.
		 * @param cameraPosition The position of the camera
		 * @param groundTruthPoint The actual world point
		 * @param triangulatedPoint The triangulated point
		 * @return The angular error in radians
		 */
		static Scalar computeAngularError(const Vector3& cameraPosition, const Vector3& groundTruthPoint, const Vector3& triangulatedPoint);

		/**
		 * Checks if a world point is visible in both cameras and passes filters.
		 * @param worldPoint The world point to check
		 * @param leftCamera The left camera
		 * @param rightCamera The right camera
		 * @param flippedLeftCamera_T_world Transform from world to flipped left camera
		 * @param flippedRightCamera_T_world Transform from world to flipped right camera
		 * @param useConeFilter Whether to apply cone filter
		 * @param coneHalfAngleDegrees Half-angle of the cone in degrees
		 * @param leftProjection Output: the projection in the left camera
		 * @param rightProjection Output: the projection in the right camera
		 * @return True if the point is visible in both cameras and passes all filters
		 */
		static bool isPointVisible(const Vector3& worldPoint, const SharedAnyCamera& leftCamera, const SharedAnyCamera& rightCamera, const HomogenousMatrix4& flippedLeftCamera_T_world, const HomogenousMatrix4& flippedRightCamera_T_world, bool useConeFilter, Scalar coneHalfAngleDegrees, Vector2& leftProjection, Vector2& rightProjection);

		/**
		 * Applies perturbation to projection coordinates based on mode.
		 * @param leftProjection The left camera projection
		 * @param rightProjection The right camera projection
		 * @param perturbationMode The perturbation mode
		 * @param deltaX The X perturbation delta
		 * @param deltaY The Y perturbation delta
		 * @param useRandom Whether to use random noise or fixed offset
		 * @param randomGenerator Random generator for noise (only used if useRandom is true)
		 * @return Pair of perturbed projections (left, right)
		 */
		static std::pair<Vector2, Vector2> applyPerturbation(const Vector2& leftProjection, const Vector2& rightProjection, PerturbationMode perturbationMode, Scalar deltaX, Scalar deltaY, bool useRandom, RandomGenerator& randomGenerator);

		/**
		 * Triangulates a point and computes angular error based on perturbation mode.
		 * @param leftCamera The left camera
		 * @param rightCamera The right camera
		 * @param world_T_leftCamera Transform from left camera to world
		 * @param world_T_rightCamera Transform from right camera to world
		 * @param leftProjection The (perturbed) left projection
		 * @param rightProjection The (perturbed) right projection
		 * @param worldPoint The ground truth world point
		 * @param perturbationMode The perturbation mode for error computation
		 * @param triangulatedPoint Output: the triangulated point
		 * @return The angular error in radians, or -1 if triangulation failed
		 */
		static Scalar triangulateAndComputeError(const SharedAnyCamera& leftCamera, const SharedAnyCamera& rightCamera, const HomogenousMatrix4& world_T_leftCamera, const HomogenousMatrix4& world_T_rightCamera, const Vector2& leftProjection, const Vector2& rightProjection, const Vector3& worldPoint, PerturbationMode perturbationMode, Vector3& triangulatedPoint);

		/**
		 * Computes statistics from the point errors.
		 * @param errors The vector of error values
		 * @return The computed statistics
		 */
		static SimulationStats computeStatistics(const std::vector<Scalar>& errors);

	protected:

		/// The rendering engine
		Rendering::EngineRef engine_;

		/// The framebuffer
		Rendering::FramebufferRef framebuffer_;

		/// The scene
		Rendering::SceneRef scene_;

		/// The perspective view (used for both perspective and simulated orthogonal)
		Rendering::PerspectiveViewRef perspectiveView_;

		/// The parallel (orthographic) view for true orthogonal projection
		Rendering::ParallelViewRef parallelView_;

		/// Current projection mode
		ProjectionMode projectionMode_ = ProjectionMode::PERSPECTIVE;

		/// Current configuration
		SimulationConfig config_;

		/// Current colorization configuration
		ColorizationConfig colorizationConfig_;

		/// Current statistics
		SimulationStats stats_;

		/// Transform for the left camera frustum
		Rendering::TransformRef leftCameraTransform_;

		/// Transform for the right camera frustum
		Rendering::TransformRef rightCameraTransform_;

		/// Transform for the point grid
		Rendering::TransformRef pointsTransform_;

		/// Transform for the error lines (connecting ground truth to triangulated points)
		Rendering::TransformRef errorLinesTransform_;

		/// Transform for the coordinate axes
		Rendering::TransformRef axesTransform_;

		/// Transform for the cone filter visualization
		Rendering::TransformRef coneTransform_;

		/// 3D positions of grid points
		Vectors3 gridPoints_;

		/// 3D positions of triangulated points (with max error)
		Vectors3 triangulatedPoints_;

		/// Error values for each grid point
		std::vector<Scalar> pointErrors_;

		/// Last mouse position
		Vector2 lastMousePosition_;

		/// Orbit camera rotation
		Quaternion orbitRotation_;

		/// Orbit camera distance
		Scalar orbitDistance_ = Scalar(2.0);

		/// Orbit camera center
		Vector3 orbitCenter_;

		/// Random generator for simulation
		RandomGenerator randomGenerator_;

		/// Whether the camera is currently orbiting
		bool isOrbiting_ = false;

		/// Orbit animation angle (radians)
		Scalar orbitAngle_ = Scalar(0);
};

}

}

}

}

}

}

#endif // META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_SCENE_H
