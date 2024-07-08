/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/opengles/win/OpenGLESMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

#include "ocean/math/Numeric.h"

OpenGLESMainWindow::OpenGLESMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	ApplicationWindow(instance, name),
	eglDisplay_(EGL_NO_DISPLAY),
	eglConfig_(nullptr),
	eglSurface_(EGL_NO_SURFACE),
	eglContext_(EGL_NO_CONTEXT),
	glesShaderProgram_(0),
	glesVertexShader_(0),
	glesFragmentShader_(0),
	glesVertexBufferObject_(0),
	glesVertexBufferIndicesObject_(0),
	glesVertexAttributeLocation_(0),
	glesTextureId_(0)
{
	// nothing to do here
}

OpenGLESMainWindow::~OpenGLESMainWindow()
{
	releaseContext();
}

void OpenGLESMainWindow::render()
{
	if (eglDisplay_ == EGL_NO_DISPLAY || eglSurface_ == EGL_NO_SURFACE)
	{
		return;
	}

	ocean_assert(eglDisplay_ && eglSurface_);

	performance_.start();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(EGL_SUCCESS == eglGetError());



	glActiveTexture(GL_TEXTURE0);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, glesTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	GLint textureLocation = glGetUniformLocation(glesShaderProgram_, "texture");
	ocean_assert(GL_NO_ERROR == glGetError());

	glUniform1i(textureLocation, 0);
	ocean_assert(GL_NO_ERROR == glGetError());



	glEnableVertexAttribArray(glesVertexAttributeLocation_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glVertexAttribPointer(glesVertexAttributeLocation_, 3, GL_FLOAT, GL_FALSE, 0, 0);
	ocean_assert(GL_NO_ERROR == glGetError());

	const uint16_t indices[3] = {0, 1, 2};
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, indices);

	eglSwapBuffers(eglDisplay_, eglSurface_);
	ocean_assert(EGL_SUCCESS == eglGetError());

	performance_.stop();

	if (NumericD::isNotEqualEps(performance_.total()))
	{
		const double fps = double(performance_.measurements()) / performance_.total();
		SetWindowTextA(handle(), (std::string("OpenGLES (") + Build::buildString() + std::string(") ") + String::toAString(fps, 1u) + std::string("fps")).c_str());
	}

	if (performance_.measurements() > 60u)
	{
		performance_.reset();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void OpenGLESMainWindow::onInitialized()
{
	if (initializeContext() == false)
	{
		return;
	}

	initializeGL();
	createShaderProgram();
	createVertexMesh();
	createTexture();
}

void OpenGLESMainWindow::onDestroy()
{
	ApplicationWindow::onDestroy();

	releaseTexture();
	releaseVertexMesh();
	releaseShaderProgram();
	releaseContext();
}

void OpenGLESMainWindow::onPaint()
{
	render();
}

void OpenGLESMainWindow::onIdle()
{
	render();
}

void OpenGLESMainWindow::onResize(const unsigned int clientWidth, const unsigned int clientHeight)
{
	if (clientHeight == 0u)
	{
		return;
	}

	glViewport(0, 0, (GLsizei)(clientWidth), (GLsizei)(clientHeight));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void OpenGLESMainWindow::modifyWindowClass(WNDCLASS& windowClass)
{
	// suppress window background redrawing
	windowClass.hbrBackground = nullptr;
}

bool OpenGLESMainWindow::initializeContext()
{
	eglDisplay_ = eglGetDisplay(dc());
	if (eglDisplay_ == EGL_NO_DISPLAY)
	{
		Log::error() << "Failed to determine the EGL display, reason: " << eglGetError();
		return false;
	}

	ocean_assert(EGL_SUCCESS == eglGetError());

	EGLint majorVersion = -1;
	EGLint minorVersion = -1;
	if (!eglInitialize(eglDisplay_, &majorVersion, &minorVersion))
	{
		Log::error() << "EGL initialization failed.";
		return false;
	}

	const char* vendor = eglQueryString(eglDisplay_, EGL_VENDOR);
	ocean_assert(EGL_SUCCESS == eglGetError());
	Log::info() << "OpenGL ES vendor: " << vendor;

	const char* version = eglQueryString(eglDisplay_, EGL_VERSION);
	ocean_assert(EGL_SUCCESS == eglGetError());
	Log::info() << "OpenGL ES version: " << version;

	if (!eglBindAPI(EGL_OPENGL_ES_API))
	{
		Log::error() << "Failed to bind the EGL API, with reason: " << eglGetError();
		return false;
	}

	ocean_assert(EGL_SUCCESS == eglGetError());

	EGLint numberConfigurations = 0;
	if (!eglGetConfigs(eglDisplay_, nullptr, 0, &numberConfigurations))
	{
		Log::error() << "Not able to determine the number of configurations, with reason: " << eglGetError();
		return false;
	}

	ocean_assert(EGL_SUCCESS == eglGetError());

	std::vector<EGLConfig> eglConfigurations(numberConfigurations);
	numberConfigurations = 0;

	if (!eglGetConfigs(eglDisplay_, eglConfigurations.data(), EGLint(eglConfigurations.size()), &numberConfigurations))
	{
		Log::error() << "Not able to determine the configurations, with reason: " << eglGetError();
		return false;
	}

	ocean_assert(EGL_SUCCESS == eglGetError());

	Log::info() << "The following " << eglConfigurations.size() << " configurations are available:";

	for (size_t n = 0; n < eglConfigurations.size(); ++n)
	{
		const EGLConfig eglConfiguration = eglConfigurations[n];

		EGLint renderTypeValue = 0;
		bool result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_RENDERABLE_TYPE, &renderTypeValue) != 0;
		ocean_assert(result && EGL_SUCCESS == eglGetError());

		EGLint surfaceTypeValue = 0;
		result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_SURFACE_TYPE, &surfaceTypeValue) != 0;
		ocean_assert(result && EGL_SUCCESS == eglGetError());

		EGLint redSizeValue = 0;
		result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_RED_SIZE, &redSizeValue) != 0;
		ocean_assert(result && EGL_SUCCESS == eglGetError());

		EGLint greenSizeValue = 0;
		result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_GREEN_SIZE, &greenSizeValue) != 0;
		ocean_assert(result && EGL_SUCCESS == eglGetError());

		EGLint blueSizeValue = 0;
		result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_BLUE_SIZE, &blueSizeValue) != 0;
		ocean_assert(result && EGL_SUCCESS == eglGetError());

		EGLint alphaSizeValue = 0;
		result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_ALPHA_SIZE, &alphaSizeValue) != 0;
		ocean_assert(result && EGL_SUCCESS == eglGetError());

		EGLint depthSizeValue = 0;
		result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_DEPTH_SIZE, &depthSizeValue) != 0;
		ocean_assert(result && EGL_SUCCESS == eglGetError());

		Log::info() << String::toAString((unsigned int)n, 3u) << ": Render Type " << renderTypeValue << ", Surface Type " << surfaceTypeValue << ", RGBA sizes " << redSizeValue << "-" << greenSizeValue << "-" << blueSizeValue << "-" << alphaSizeValue << ", Depth " << depthSizeValue;
	}

	const EGLint configAttributes[] =
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_NONE
	};

	EGLint numberConfigs = 0;
	if (!eglChooseConfig(eglDisplay_, configAttributes, &eglConfig_, 1, &numberConfigs) || numberConfigs == 0)
	{
		Log::warning() << "No valid EGL configuration found, with reason: " << eglGetError() << ", we select a configuration on our own";

		for (size_t n = 0; n < eglConfigurations.size(); ++n)
		{
			const EGLConfig eglConfiguration = eglConfigurations[n];

			EGLint renderTypeValue = 0;
			bool result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_RENDERABLE_TYPE, &renderTypeValue) != 0;
			ocean_assert(result && EGL_SUCCESS == eglGetError());

			if ((renderTypeValue & EGL_OPENGL_ES3_BIT) != EGL_OPENGL_ES3_BIT)
			{
				continue;
			}

			EGLint surfaceTypeValue = 0;
			result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_SURFACE_TYPE, &surfaceTypeValue) != 0;
			ocean_assert(result && EGL_SUCCESS == eglGetError());

			if ((surfaceTypeValue & EGL_WINDOW_BIT) != EGL_WINDOW_BIT)
			{
				continue;
			}

			EGLint redSizeValue = 0;
			result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_RED_SIZE, &redSizeValue) != 0;
			ocean_assert(result && EGL_SUCCESS == eglGetError());

			EGLint greenSizeValue = 0;
			result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_GREEN_SIZE, &greenSizeValue) != 0;
			ocean_assert(result && EGL_SUCCESS == eglGetError());

			EGLint blueSizeValue = 0;
			result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_BLUE_SIZE, &blueSizeValue) != 0;
			ocean_assert(result && EGL_SUCCESS == eglGetError());

			EGLint alphaSizeValue = 0;
			result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_ALPHA_SIZE, &alphaSizeValue) != 0;
			ocean_assert(result && EGL_SUCCESS == eglGetError());

			if (redSizeValue != 8 || greenSizeValue != 8 || blueSizeValue != 8 || alphaSizeValue != 8)
			{
				continue;
			}

			EGLint depthSizeValue = 0;
			result = eglGetConfigAttrib(eglDisplay_, eglConfiguration, EGL_DEPTH_SIZE, &depthSizeValue) != 0;
			ocean_assert(result && EGL_SUCCESS == eglGetError());

			if (depthSizeValue < 24)
			{
				continue;
			}

			eglConfig_ = eglConfigurations[n];
			break;
		}
	}

	if (eglConfig_ == nullptr)
	{
		Log::error() << "No valid EGL configuration found, with reason";
		return false;
	}

	eglSurface_ = eglCreateWindowSurface(eglDisplay_, eglConfig_, handle(), nullptr);
	if (eglSurface_ == EGL_NO_SURFACE)
	{
		Log::error() << "No valid EGL surface.";
		return false;
	}

	const EGLint contextAttribs[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};

	eglContext_ = eglCreateContext(eglDisplay_, eglConfig_, nullptr, contextAttribs);
	if (eglContext_ == EGL_NO_CONTEXT)
	{
		ocean_assert(EGL_SUCCESS == eglGetError());

		Log::error() << "No valid EGL context.";
		return false;
	}

	eglMakeCurrent(eglDisplay_, eglSurface_, eglSurface_, eglContext_);
	ocean_assert(EGL_SUCCESS == eglGetError());

	return true;
}

void OpenGLESMainWindow::releaseContext()
{
	if (eglDisplay_ != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	}

	if (eglSurface_ != EGL_NO_SURFACE)
	{
		eglDestroySurface(eglDisplay_, eglSurface_);
		eglSurface_ = EGL_NO_SURFACE;
		ocean_assert(EGL_SUCCESS == eglGetError());
	}

	if (eglContext_ != EGL_NO_CONTEXT)
	{
		eglDestroyContext(eglDisplay_, eglContext_);
		eglContext_ = EGL_NO_CONTEXT;
		ocean_assert(EGL_SUCCESS == eglGetError());
	}

	if (eglDisplay_ != EGL_NO_DISPLAY)
	{
		eglTerminate(eglDisplay_);
		eglDisplay_ = EGL_NO_DISPLAY;
		ocean_assert(EGL_SUCCESS == eglGetError());
	}
}

void OpenGLESMainWindow::releaseShaderProgram()
{
	if (glesFragmentShader_ != 0)
	{
		ocean_assert(glesShaderProgram_ != 0);
		glDetachShader(glesShaderProgram_, glesFragmentShader_);
		ocean_assert(EGL_SUCCESS == eglGetError());
	}

	if (glesFragmentShader_ != 0)
	{
		glDeleteShader(glesFragmentShader_);
		glesFragmentShader_ = 0;
		ocean_assert(EGL_SUCCESS == eglGetError());
	}

	if (glesVertexShader_ != 0)
	{
		ocean_assert(glesShaderProgram_ != 0);
		glDetachShader(glesShaderProgram_, glesVertexShader_);
		ocean_assert(EGL_SUCCESS == eglGetError());
	}

	if (glesVertexShader_ != 0)
	{
		glDeleteShader(glesVertexShader_);
		glesVertexShader_ = 0;
		ocean_assert(EGL_SUCCESS == eglGetError());
	}

	if (glesShaderProgram_ != 0)
	{
		glDeleteProgram(glesShaderProgram_);
		glesShaderProgram_ = 0;
		ocean_assert(EGL_SUCCESS == eglGetError());
	}
}

void OpenGLESMainWindow::releaseVertexMesh()
{
	if (glesVertexBufferObject_ != 0)
	{
		glDeleteBuffers(1, &glesVertexBufferObject_);
		glesVertexBufferObject_ = 0;
		ocean_assert(EGL_SUCCESS == eglGetError());
	}

	if (glesVertexBufferIndicesObject_ != 0)
	{
		glDeleteBuffers(1, &glesVertexBufferIndicesObject_);
		glesVertexBufferIndicesObject_ = 0;
		ocean_assert(EGL_SUCCESS == eglGetError());
	}
}

void OpenGLESMainWindow::initializeGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClearDepthf(1.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

	glEnable(GL_DEPTH_TEST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDepthFunc(GL_LEQUAL);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void OpenGLESMainWindow::createShaderProgram()
{
	ocean_assert(glesShaderProgram_ == 0);

	glesShaderProgram_ = glCreateProgram();
	ocean_assert(glesShaderProgram_ != 0 && GL_NO_ERROR == glGetError());

	static const char* vertexShaderCode =
		"attribute vec4 meshVertex;\n\
		void main(void)\n\
		{\n\
			gl_Position = meshVertex;\n\
		}\n";

	glesVertexShader_ = glCreateShader(GL_VERTEX_SHADER);
	ocean_assert(glesVertexShader_ != 0 && GL_NO_ERROR == glGetError());

	glShaderSource(glesVertexShader_, 1, &vertexShaderCode, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());

	glCompileShader(glesVertexShader_);
	ocean_assert(GL_NO_ERROR == glGetError());

	GLint shaderCompiled = 0;
	glGetShaderiv(glesVertexShader_, GL_COMPILE_STATUS, &shaderCompiled);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(shaderCompiled);

	glAttachShader(glesShaderProgram_, glesVertexShader_);
	ocean_assert(GL_NO_ERROR == glGetError());



	static const char* fragmentShaderCode =
		"uniform sampler2D texture;\n\
		void main()\n\
		{\n\
			gl_FragColor = 0.9 * texture2D(texture, vec2(1.0, 1.0)).rgba + 0.1 * vec4(1.0, 1.0, 0.0, 1.0);\n\
		}\n";

	glesFragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
	ocean_assert(glesFragmentShader_ != 0 && GL_NO_ERROR == glGetError());

	glShaderSource(glesFragmentShader_, 1, &fragmentShaderCode, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());

	glCompileShader(glesFragmentShader_);
	ocean_assert(GL_NO_ERROR == glGetError());

	shaderCompiled = 0;
	glGetShaderiv(glesFragmentShader_, GL_COMPILE_STATUS, &shaderCompiled);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(shaderCompiled);

	glAttachShader(glesShaderProgram_, glesFragmentShader_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glLinkProgram(glesShaderProgram_);
	ocean_assert(GL_NO_ERROR == glGetError());

	// Check if linking succeeded in the same way we checked for compilation success
	GLint programLinked = 0;
	glGetProgramiv(glesShaderProgram_, GL_LINK_STATUS, &programLinked);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(programLinked);

	glUseProgram(glesShaderProgram_);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void OpenGLESMainWindow::createVertexMesh()
{
	ocean_assert(glesVertexBufferObject_ == 0);

	glGenBuffers(1, &glesVertexBufferObject_);
	ocean_assert(glesVertexBufferObject_ != 0);
	ocean_assert(GL_NO_ERROR == glGetError());

	GLfloat vertices[] =
	{
		-0.4f, -0.4f, 0.0f,
		0.4f, -0.4f, 0.0f,
		0.0f, 0.4f, 0.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, glesVertexBufferObject_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat) * 3, vertices, GL_STATIC_DRAW);
	ocean_assert(GL_NO_ERROR == glGetError());

	const uint16_t indices[3] = {0u, 1u, 2u};

	glesVertexAttributeLocation_ = glGetAttribLocation(glesShaderProgram_, "meshVertex");
	ocean_assert(glesVertexAttributeLocation_ != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void OpenGLESMainWindow::createTexture()
{
	ocean_assert(glesTextureId_ == 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	ocean_assert(GL_NO_ERROR == glGetError());

	glGenTextures(1, &glesTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, glesTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	const uint8_t pixels[] =
	{
		0xFFu, 0x00u, 0x00u,
		0xFFu, 0x00u, 0x00u,
		0xFFu, 0x00u, 0x00u,
		0xFFu, 0xFFu, 0x00u
	};

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void OpenGLESMainWindow::releaseTexture()
{
	glDeleteTextures(1, &glesTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void OpenGLESMainWindow::renderTriangle()
{
	// create a program

	const GLuint shaderProgram = glCreateProgram();
	ocean_assert(shaderProgram != 0 && GL_NO_ERROR == glGetError());


	// create a vertex shader

	const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	ocean_assert(vertexShader != 0 && GL_NO_ERROR == glGetError());

	static const char* vertexShaderCode =
		"#version 300 es\n\
		in lowp vec4 aVertex;\n\
		void main(void)\n\
		{\n\
			gl_Position = aVertex;\n\
		}\n";

	glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());

	glCompileShader(vertexShader);
	ocean_assert(GL_NO_ERROR == glGetError());

	GLint shaderCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderCompiled);
	ocean_assert(GL_NO_ERROR == glGetError());

	if (!shaderCompiled)
	{
		GLint infoLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLength);
		ocean_assert(GL_NO_ERROR == glGetError());

		std::string message;

		if (infoLength > 1 && infoLength <= 4096)
		{
			message.resize(infoLength);
			glGetShaderInfoLog(vertexShader, infoLength, nullptr, &message[0]);
			message.resize(infoLength - 1);
		}

		Log::error() << "Failed to compile vertex shader: " << message;
	}

	glAttachShader(shaderProgram, vertexShader);
	ocean_assert(GL_NO_ERROR == glGetError());


	// create a fragment shader

	const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	ocean_assert(fragmentShader != 0 && GL_NO_ERROR == glGetError());

	static const char* fragmentShaderCode =
		"#version 300 es\n\
		out lowp vec4 fragColor;\n\
		void main()\n\
		{\n\
			fragColor = vec4(1.0, 1.0, 1.0, 1.0);\n\
		}\n";

	glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());

	glCompileShader(fragmentShader);
	ocean_assert(GL_NO_ERROR == glGetError());

	shaderCompiled = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderCompiled);
	ocean_assert(GL_NO_ERROR == glGetError());

	if (!shaderCompiled)
	{
		GLint infoLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLength);
		ocean_assert(GL_NO_ERROR == glGetError());

		std::string message;

		if (infoLength > 1 && infoLength <= 4096)
		{
			message.resize(infoLength);
			glGetShaderInfoLog(vertexShader, infoLength, nullptr, &message[0]);
			message.resize(infoLength - 1);
		}

		Log::error() << "Failed to compile fragment shader: " << message;
	}

	glAttachShader(shaderProgram, fragmentShader);
	ocean_assert(GL_NO_ERROR == glGetError());


	// link vertex and fragment shader

	glLinkProgram(shaderProgram);
	ocean_assert(GL_NO_ERROR == glGetError());

	// Check if linking succeeded in the same way we checked for compilation success
	GLint programLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programLinked);
	ocean_assert(GL_NO_ERROR == glGetError());

	if (!programLinked)
	{
		GLint infoLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLength);

		std::string message;

		if (infoLength > 1 && infoLength <= 4096)
		{
			message.resize(infoLength);
			glGetProgramInfoLog(shaderProgram, infoLength, nullptr, &message[0]);
		}

		Log::error() << "Failed to link program: " << message;
	}

	glUseProgram(shaderProgram);
	ocean_assert(GL_NO_ERROR == glGetError());


	// create the geometry to render

	GLuint vertexArray = 0u;
	glGenVertexArrays(1, &vertexArray);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(vertexArray != 0u);
	glBindVertexArray(vertexArray);
	ocean_assert(GL_NO_ERROR == glGetError());

	GLuint vertexBufferObject = 0u;
	glGenBuffers(1, &vertexBufferObject);
	ocean_assert(vertexBufferObject != 0);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLfloat vertices[] =
	{
		-0.4f, -0.4f, 0.0f,
		0.4f, -0.4f, 0.0f,
		0.0f, 0.4f, 0.0f
	};

 	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat) * 3, vertices, GL_STATIC_DRAW);
	ocean_assert(GL_NO_ERROR == glGetError());

	const int vertexAttributeLocation = glGetAttribLocation(shaderProgram, "aVertex");
	ocean_assert(vertexAttributeLocation != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	glEnableVertexAttribArray(vertexAttributeLocation);
	ocean_assert(GL_NO_ERROR == glGetError());

	glVertexAttribPointer(vertexAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDrawArrays(GL_TRIANGLES, 0, 3u);
	ocean_assert(GL_NO_ERROR == glGetError());


	// cleaning up

	glDetachShader(shaderProgram, fragmentShader);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDeleteShader(fragmentShader);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDetachShader(shaderProgram, vertexShader);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDeleteShader(vertexShader);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDeleteProgram(shaderProgram);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDeleteBuffers(1, &vertexBufferObject);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDeleteVertexArrays(1, &vertexArray);
	ocean_assert(GL_NO_ERROR == glGetError());
}
