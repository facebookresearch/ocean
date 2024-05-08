/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_VIEW_H
#define FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_VIEW_H

#include "application/ocean/orca/win/OrcaWin.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/devices/Manager.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/MediumRef.h"
#include "ocean/media/Recorder.h"

#include "ocean/rendering/BitmapFramebuffer.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/UndistortedBackground.h"

#include "ocean/scenedescription/Scene.h"
#include "ocean/scenedescription/SDLScene.h"
#include "ocean/scenedescription/SDXScene.h"

#include <vector>

namespace Ocean
{

namespace Orca
{

namespace Win
{

/**
 * This class implements a view window.
 * @ingroup orcawin
 */
class View : public CWnd
{
	public:

		/**
		 * Definition of different view types.
		 */
		enum ViewType
		{
			/// Unknown view type.
			TYPE_UNKNOWN,
			/// Mono view.
			TYPE_MONO_VIEW,
			/// Parallel view.
			TYPE_PARALLEL_VIEW,
			/// Quad-buffered stereo view.
			TYPE_STEREO_VIEW_QUADBUFFERED,
			/// Anaglyph stereo view.
			TYPE_STEREO_VIEW_ANAGLYPH
		};

		/**
		 * Definition of different recorder types.
		 */
		enum RecorderType
		{
			/// None recorder.
			TYPE_NONE,
			/// Image recorder.
			TYPE_IMAGE,
			/// Image sequence recorder.
			TYPE_IMAGE_SEQUENCE,
			/// Movie recorder.
			TYPE_MOVIE,
			/// Memory recorder.
			TYPE_MEMORY
		};

		/**
		 * Definition of different fullscreen cursor modes.
		 */
		enum CursorMode
		{
			/// The cursor is visible always.
			CM_VISIBLE,
			/// The cursor hides after inactivity.
			CM_HIDE_INACTIVITY,
			/// The cursor is hidden always.
			CM_HIDE_ALWAYS
		};

		/**
		 * Definition of a vector holding recorder encoder names.
		 */
		typedef std::vector<std::string> EncoderNames;

		/**
		 * Definition of a set holding permanent scene description objects.
		 */
		typedef std::set<SceneDescription::SDXSceneRef> SceneSet;

	public:

		/**
		 * Creates a new view window.
		 */
		View();

		/**
		 * Destructs a view window.
		 */
		virtual ~View();

		/**
		 * Returns the rendering engine of this view.
		 * @return Rendering engine
		 */
		inline Rendering::EngineRef engine() const;

		/**
		 * Returns the type of this view.
		 * @return View type
		 */
		ViewType type() const;

		/**
		 * Returns the name of the used rendering engine.
		 * @return Engine name
		 */
		std::string renderer() const;

		/**
		 * Returns whether the framebuffer supports quad-buffered stereo.
		 * @return True, if so
		 */
		bool supportsQuadbufferedStereo() const;

		/**
		 * Returns whether the stereo images are reversed for stereo view.
		 * @return True, if so
		 */
		bool eyesReversed() const;

		/**
		 * Returns whether the framebuffer supports anti-aliasing.
		 * @param buffers Number of buffers to be checked
		 * @return True, if so
		 */
		bool supportsAntialiasing(const unsigned int buffers) const;

		/**
		 * Returns whether the framebuffer uses anti-aliasing.
		 * @return True, if so
		 */
		bool antialiasing() const;

		/**
		 * Returns the horizontal field of view in radian.
		 * @return Field of view in radian
		 */
		double horizontalFieldOfView() const;

		/**
		 * Returns the near distance of this view.
		 * @return Near distance
		 */
		double nearDistance() const;

		/**
		 * Returns the far distance of this view.
		 * @return Far distance
		 */
		double farDistance() const;

		/**
		 * Returns the focus distance of this view.
		 * @return Focus distance
		 */
		double focusDistance() const;

		/**
		 * Returns the preferred framerate of this view in Hz.
		 * @return Preferred frame rate in Hz
		 */
		float preferredFramerate() const;

		/**
		 * Returns whether the headlight is enabled.
		 * @return True, if so
		 */
		bool useHeadlight() const;

		/**
		 * Returns the background color of this view.
		 * @return Background color
		 */
		unsigned int backgroundColor() const;

		/**
		 * Returns the background medium used for this view.
		 * @return Background medium
		 */
		Media::FrameMediumRef backgroundMedium();

		/**
		 * Returns the selected recorder encoder name.
		 * @return Recorder encoder
		 */
		std::string recorderEncoder() const;

		/**
		 * Configures the recorder encoder.
		 * @return True, if succeeded
		 */
		bool recorderEncoderConfiguration();

		/**
		 * Returns the recorder framerate in Hz.
		 * @return Recorder framerate
		 */
		double recorderFramerate() const;

		/**
		 * Returns a list of available recorder encoder names.
		 * @return Encoder names
		 */
		EncoderNames recorderEncoderNames() const;

		/**
		 * Sets the type of this view.
		 * @param type Type to set
		 * @return True, if succeeded
		 */
		bool setType(const ViewType type);

		/**
		 * Sets the rendering engine used in this view window.
		 * @param engineName Name of the engine to use
		 * @param preferredGraphicAPI Graphic API which should be used for rendering
		 * @return True, if succeeded
		 */
		bool setRenderer(const std::string& engineName, const Rendering::Engine::GraphicAPI preferredGraphicAPI = Rendering::Engine::API_DEFAULT);

		/**
		 * Sets whether the rendering engine will support quad-buffered stereo views.
		 * @param support True, if stereo will be supported
		 * @return True, if succeeded
		 */
		bool setSupportQuadbufferedStereo(const bool support);

		/**
		 * Sets whether the left and right image are reversed for stereo views.
		 * @param reverse True, if so
		 * @return True, if succeeded
		 */
		bool setEyesReversed(const bool reverse);

		/**
		 * Sets whether the framebuffer will support anti-aliasing.
		 * @param buffers Number of buffers the framebuffer should support
		 * @return True, if succeeded
		 */
		bool setSupportAntialiasing(const unsigned int buffers);

		/**
		 * Sets whether the framebuffer will use anti-aliasing.
		 * @param anti-aliasing True, if anti-aliasing should be used
		 * @return True, if succeeded
		 */
		bool setAntialiasing(const bool antialiasing);

		/**
		 * Sets the face mode of the renderer used in this view window.
		 * @param mode Face mode to set
		 * @return True, if succeeded
		 */
		bool setRendererFaceMode(const Rendering::Framebuffer::FaceMode mode);

		/**
		 * Sets the culling mode of the renderer used in this view window.
		 * @param mode Culling mode to set
		 * @return True, if succeeded
		 */
		bool setRendererCullingMode(const Rendering::Framebuffer::CullingMode mode);

		/**
		 * Sets the rendering technique of the renderer used in this view window.
		 * @param technique Rendering technique to set
		 * @return True, if succeeded
		 */
		bool setRendererTechnique(const Rendering::Framebuffer::RenderTechnique technique);

		/**
		 * Sets the phantom mode of the renderer used in this view window.
		 * @param mode Phantom mode to set
		 * @return True, if succeeded
		 */
		bool setPhantomMode(const Rendering::PhantomAttribute::PhantomMode mode);

		/**
		 * Sets the horizontal field of view in radian.
		 * @param fovx Field of view in radian to set
		 * @return True, if succeeded
		 */
		bool setHorizontalFieldOfView(const double fovx);

		/**
		 * Sets the near distance of the view.
		 * @param nearDistance Near value to set
		 * @return True, if succeeded
		 */
		bool setNearDistance(const double nearDistance);

		/**
		 * Sets the far distance of the view.
		 * @param farDistance Far value to set
		 * @return True, if succeeded
		 */
		bool setFarDistance(const double farDistance);

		/**
		 * Sets the focus distance of this view.
		 * @param focusDistance Focus distance to set
		 * @return True, if succeeded
		 */
		bool setFocusDistance(const double focusDistance);

		/**
		 * Sets the preferred frame rate of this view.
		 * @param fps Framerate in Hz
		 * @return True, if succeeded
		 */
		bool setPreferredFramerate(const float fps);

		/**
		 * Sets whether the headlight is enabled.
		 * @param state True, if headlight is enabled.
		 * @return True, if succeeded
		 */
		bool setUseHeadlight(const bool state);

		/**
		 * Sets the background color of this view.
		 * @param color The color to set
		 * @return True, if succeeded
		 */
		bool setBackgroundColor(const unsigned int color);

		/**
		 * Sets the background medium of this view.
		 * @param medium Background medium to set
		 * @return True, if succeeded
		 */
		bool setBackgroundMedium(const Media::FrameMediumRef& medium);

		/**
		 * Sets the background display type of this view.
		 * @param type Display type to set
		 * @return True, if succeeded
		 */
		bool setBackgroundDisplayType(const Rendering::UndistortedBackground::DisplayType type);

		/**
		 * Sets the cursor fullscreen mode.
		 * @param mode Fullscreen cursor mode to set
		 * @return True, if succeeded
		 */
		bool setCursorMode(const CursorMode mode);

		/**
		 * Sets whether the viewing position and orientation will be stored for the next session.
		 * @param store True, if so
		 * @return True, fi succeeded
		 */
		bool setStorePosition(const bool store);

		/**
		 * Sets the recorder type.
		 * @param type Recorder type to set.
		 */
		bool setRecorderType(const RecorderType type);

		/**
		 * Sets the recorder encoder name.
		 * @param encoder The encoder to set
		 * @return True, if succeeded
		 */
		bool setRecorderEncoder(const std::string& encoder);

		/**
		 * Sets the preferred recorder frame rate in Hz.
		 * @param fps Preferred frame rate
		 * @return True, if succeeded
		 */
		bool setRecorderFrameRate(const double fps);

		/**
		 * Sets the output filename of the used (file output) recorder.
		 * @param filename The filename to set
		 * @return True, if succeeded
		 */
		bool setRecorderFilename(const std::string& filename);

		/**
		 * Sets whether the output filename will be extended with the current date and time.
		 * @param extend True, if so
		 * @return True, if succeeded
		 */
		bool setRecorderExtendedFilename(const bool extend);

		/**
		 * Fits the current scene to the current screen dimension.
		 */
		void fitSceneToScreen();

		/**
		 * Called if the application is in idle state.
		 */
		void onIdle();

		/**
		 * Loads and applies configuration parameters like e.g. position and dimension.
		 */
		void applyConfiguration();

		/**
		 * Stores configuration parameters.
		 */
		void storeConfiguration();

		/**
		 * Returns the main view.
		 * @return Main view
		 */
		static View& mainView();

	protected:

		/**
		 * Called by the GUI framework before the creation of the Windows window attached to this CWnd object.
		 * @param createStruct Structure holding informations about the behavior and appearance of the window to create
		 * @return True, if succeeded
		 */
		virtual BOOL PreCreateWindow(CREATESTRUCT& createStruct);

		/**
		 * (Re)initializes the framebuffer.
		 * @return True, if succeeded
		 */
		bool initializeFramebuffer();

		/**
		 * Renders a new frame.
		 */
		void render();

		/**
		 * Checks the cursor fullscreen mode.
		 */
		void checkCursorFullscreen();

		/**
		 * The GUI framework calls this function if the windows has to be repainted.
		 */
		afx_msg void OnPaint();

		/**
		 * The GUI framework calls this function after the window's size has changed.
		 * @param type Window size change type
		 * @param width New window width
		 * @param height New window height
		 */
		afx_msg void OnSize(UINT type, int width, int height);

		/**
		 * The GUI framework calls this function when the user double-clicks the left mouse button.
		 * @param flags Key down flags
		 * @param point Mouse cursor position
		 */
		afx_msg void OnLButtonDblClk(UINT flags, CPoint point);

		/**
		 * The GUI framework calls this function when the user presses the left mouse button.
		 * @param flags Key down flags
		 * @param point Mouse cursor position
		 */
		afx_msg void OnLButtonDown(UINT flags, CPoint point);

		/**
		 * The GUI framework calls this function when the user releases the left mouse button.
		 * @param flags Key down flags
		 * @param point Mouse cursor position
		 */
		afx_msg void OnLButtonUp(UINT flags, CPoint point);

		/**
		 * The GUI framework calls this function when the user presses the right mouse button.
		 * @param flags Key down flags
		 * @param point Mouse cursor position
		 */
		afx_msg void OnRButtonDown(UINT flags, CPoint point);

		/**
		 * The GUI framework calls this function when the user releases the right mouse button.
		 * @param flags Key down flags
		 * @param point Mouse cursor position
		 */
		afx_msg void OnRButtonUp(UINT flags, CPoint point);

		/**
		 * The GUI framework calls this function when the mouse cursor moves.
		 * @param flags Key down flags
		 * @param point Mouse cursor position
		 */
		afx_msg void OnMouseMove(UINT flags, CPoint point);

		/**
		 * The GUI framework calls this function as a user rotates the mouse wheel and encounters the wheel's next notch.
		 * @param flags Key down flags
		 * @param delta Mouse wheel delta
		 * @param point Mouse cursor position
		 * @return True, if mouse wheeling is activated
		 */
		afx_msg BOOL OnMouseWheel(UINT flags, short delta, CPoint point);

		/**
		 * The GUI framework calls this function when a nonsystem key is pressed.
		 * @param nChar Virtual key which has been pressed
		 * @param nRepCnt Repeat count
		 * @param nFlags Scan code
		 */
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		/**
		 * The GUI framework calls this function when a nonsystem key is released.
		 * @param nChar Virtual key which has been pressed
		 * @param nRepCnt Repeat count
		 * @param nFlags Scan code
		 */
		afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

		/**
		 * The GUI framework calls this function to start the recorder.
		 */
		void onStartRecorder();

		/**
		 * The GUI framework calls this function to stop the recorder.
		 */
		void onStopRecorder();

		/**
		 * The GUI framework calls this function to enable the recorder.
		 */
		void onEnableRecorder();

		/**
		 * The GUI framework calls this function to disable the recorder.
		 */
		void onDisableRecorder();

		/**
		 * Determines the picked object hit by a given mouse position.
		 * @param mouseX Horizontal mouse position defining the picking ray
		 * @param mouseY Vertical mouse position defining the picking ray
		 * @param ray Resulting viewing ray
		 * @param objectId Id of the picked rendering object
		 * @param objectPosition Intersection position in 3D space bettween intersection ray and rendering object.
		 * @return True, if a rendering object has been hit
		 */
		bool pickObject(const unsigned int mouseX, const unsigned int mouseY, Line3& ray, Rendering::ObjectId& objectId, Vector3& objectPosition);

		/**
		 * Runtime event message function declaration.
		 */
		DECLARE_MESSAGE_MAP();

	protected:

		/// Invalid cursor coordinate.
		static constexpr int invalidMouse_ = 2147483647;

		/// Rendering engine used for this view window.
		Rendering::EngineRef renderingEngine_;

		/// Rendering framebuffer used for this view window.
		Rendering::WindowFramebufferRef renderingFramebuffer_;

		/// Rendering background object used for the view.
		Rendering::BackgroundRef renderingBackground_;

		/// Rendering background display type.
		Rendering::UndistortedBackground::DisplayType renderingBackgroundType_ = Rendering::UndistortedBackground::DT_FASTEST;

		/// Recorder used for this view window.
		Media::RecorderRef recorder_;

		/// Flag determining whether the rendering framebuffer supports stereo views.
		bool framebufferSupportsQuadbufferedStereo_ = false;

		/// Number of buffers the rendering framebuffer supports for anti-aliasing.
		unsigned int framebufferAntialiasingBuffers_ = 0u;

		/// Last horizontal mouse cursor position connected with the left button.
		int navigationLastLeftMouseX_ = invalidMouse_;

		/// Last vertical mouse cursor position connected with the left button.
		int navigationLastLeftMouseY_ = invalidMouse_;

		/// Last horizontal mouse cursor position connected with the right button.
		int navigationLastRightMouseX_ = invalidMouse_;

		/// Last vertical mouse cursor position connected with the right button.
		int navigationLastRightMouseY_ = invalidMouse_;

		/// Navigation keyboard control button multiplication factor.
		float navigationControlFactor_ = 10.0f;

		/// Navigation keyboard shift button multiplication factor.
		float navigationAlternativeFactor_ = 0.1f;

		/// Timestamp of the last mouse interaction.
		Timestamp recentNavigationTimestamp_;

		/// Timestamp of the last window resize instruction.
		Timestamp sizeTimestamp_;

		/// Cursor fullscreen mode.
		CursorMode cursorMode_ = CM_VISIBLE;

		/// Cursor visible state.
		bool cursorVisible_ = true;

		/// State determining whether the viewing position should be stored for the next restart.
		bool storeViewingPosition_ = false;

		/// Render frame counter.
		unsigned int renderFrameCounter_ = 0u;

		/// Timestamp for the start of the next update task.
		double nextUpdateTimestamp_ = 0.0;

		/// Timestamp associated with the render frame counter.
		double renderFrameCounterTimestamp_ = 0.0;

		/// Preferred frame rate of this view in Hz.
		double preferredFramerate_ = 60.0;

		/// High performance timer.
		HighPerformanceTimer highPerformanceTimer_;

		/// State determining that the view's field of view has to be adjusted to the background's field of view.
		bool adjustFovXToBackground_ = false;

		/// Flag determines whether the configuration has been applied.
		bool configurationApplied_ = false;

		/// View lock.
		mutable Lock lock_;
};

inline Rendering::EngineRef View::engine() const
{
	return renderingEngine_;
}

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_VIEW_H
