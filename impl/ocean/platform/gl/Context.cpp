/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/Context.h"

#if defined(_WINDOWS)
	#include <gl/wglext.h>
#endif

namespace Ocean
{

namespace Platform
{

namespace GL
{

Context::~Context()
{
	release();
}

bool Context::swapBuffers()
{
	return false;
}

void Context::release()
{
	if (handle_ != 0)
	{
		releaseOpenGLContext();
		ocean_assert(handle_ == nullptr);
	}
}

bool Context::isValid() const
{
	return handle_ != nullptr;
}

bool Context::createOpenGLContext(const bool initializeOpenGL33, const unsigned int /*multisamples*/)
{
	ocean_assert(handle_ != nullptr);
	ocean_assert(glGetError() == GL_NO_ERROR);

#ifdef OCEAN_DEBUG
	const char* versionString = (const char*)(glGetString(GL_VERSION));
	ocean_assert_and_suppress_unused(versionString != nullptr, versionString);
	ocean_assert(glGetError() == GL_NO_ERROR);

	const char* rendererString = (const char*)(glGetString(GL_RENDERER));
	ocean_assert_and_suppress_unused(rendererString != nullptr, rendererString);
	ocean_assert(glGetError() == GL_NO_ERROR);

	const char* vendorString = (const char*)(glGetString(GL_VENDOR));
	ocean_assert_and_suppress_unused(vendorString != nullptr, vendorString);
	ocean_assert(glGetError() == GL_NO_ERROR);

	const char* shadingLanguageVersionString = (const char*)(glGetString(GL_SHADING_LANGUAGE_VERSION));
	ocean_assert_and_suppress_unused(shadingLanguageVersionString != nullptr, shadingLanguageVersionString);
	ocean_assert(glGetError() == GL_NO_ERROR);
#endif

#ifdef _WINDOWS
	ocean_assert(glGetStringi == nullptr);
	glGetStringi = glGetStringiFunction(wglGetProcAddress("glGetStringi"));
#else
	glGetStringi = ::glGetStringi;
#endif

	// glGetString(GL_EXTENSIONS) is deprecated in OpenGL 3.0, so we use glGetIntegerv(GL_NUM_EXTENSIONS) instead if it fails

	ocean_assert(extensions_.empty());
	const char* extensionsPointer = (const char*)glGetString(GL_EXTENSIONS);

	if (extensionsPointer)
	{
		ocean_assert(glGetError() == GL_NO_ERROR);

		const std::string extensions(extensionsPointer);

		std::string::size_type begin = 0;

		while (begin < extensions.size())
		{
			// remove prefix ' '

			while (begin < extensions.size() && extensions[begin] == ' ')
				begin++;

			if (begin == extensions.size())
				break;

			// find (excluding) end point

			std::string::size_type end = begin + 1;

			while (end < extensions.size() && extensions[end] != ' ')
				end++;

			ocean_assert(begin < end && end <= extensions.size());

			const std::string value(extensions.substr(begin, end - begin));
			ocean_assert(value == String::trim(value));

			extensions_.insert(value);

			begin = end;
		}

		ocean_assert(begin <= extensions.size());
	}
	else
	{
		ocean_assert(glGetError() == GL_INVALID_ENUM);

		ocean_assert(glGetStringi);
		if (glGetStringi)
		{
			GLint numberExtensions = 0;
			glGetIntegerv(GL_NUM_EXTENSIONS, &numberExtensions);
			ocean_assert(glGetError() == GL_NO_ERROR);

			for (unsigned int n = 0u; n < (unsigned int)numberExtensions; ++n)
			{
				const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, GLuint(n));
				ocean_assert(extension != nullptr);

				ocean_assert(glGetError() == GL_NO_ERROR);

				if (extension)
					extensions_.insert(std::string(extension));
			}
		}
	}

	ocean_assert(glGetError() == GL_NO_ERROR);

	// the following determination of the version may fail if the OpenGL version is below 3.0

	GLint versionMajor = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
	GLint versionMinor = 0;
	glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
	const GLenum lastError = glGetError();

	ocean_assert_and_suppress_unused(lastError != GL_NO_ERROR || (versionMajor > 0 && versionMinor >= 0), lastError);

	if (versionMajor > 0)
	{
		majorVersion_ = (unsigned int)(versionMajor);
	}
	if (versionMinor > 0)
	{
		minorVersion_ = (unsigned int)(versionMinor);
	}

	compatibilityProfile_ = extensions_.find("GL_ARB_compatibility") != extensions_.end();

#ifdef _WINDOWS

	ocean_assert(glGenerateMipmap == nullptr);
	glGenerateMipmap = glGenerateMipmapFunction(wglGetProcAddress("glGenerateMipmap"));
	ocean_assert(glGenerateMipmap != nullptr);
	if (glGenerateMipmap == nullptr)
		return false;

	if (!initializeOpenGL33)
	{
		return true;
	}

	ocean_assert(glGenFramebuffers == nullptr);
	glGenFramebuffers = glGenFramebuffersFunction(wglGetProcAddress("glGenFramebuffers"));
	ocean_assert(glGenFramebuffers != nullptr);
	if (glGenFramebuffers == nullptr)
		return false;

	ocean_assert(glDeleteFramebuffers == nullptr);
	glDeleteFramebuffers = glDeleteFramebuffersFunction(wglGetProcAddress("glDeleteFramebuffers"));
	ocean_assert(glDeleteFramebuffers != nullptr);
	if (glDeleteFramebuffers == nullptr)
		return false;

	ocean_assert(glBindFramebuffer == nullptr);
	glBindFramebuffer = glBindFramebufferFunction(wglGetProcAddress("glBindFramebuffer"));
	ocean_assert(glBindFramebuffer != nullptr);
	if (glBindFramebuffer == nullptr)
		return false;

	ocean_assert(glGenRenderbuffers == nullptr);
	glGenRenderbuffers = glGenRenderbuffersFunction(wglGetProcAddress("glGenRenderbuffers"));
	ocean_assert(glGenRenderbuffers != nullptr);
	if (glGenRenderbuffers == nullptr)
		return false;

	ocean_assert(glDeleteRenderbuffers == nullptr);
	glDeleteRenderbuffers = glDeleteRenderbuffersFunction(wglGetProcAddress("glDeleteRenderbuffers"));
	ocean_assert(glDeleteRenderbuffers != nullptr);
	if (glDeleteRenderbuffers == nullptr)
		return false;

	ocean_assert(glBindRenderbuffer == nullptr);
	glBindRenderbuffer = glBindRenderbufferFunction(wglGetProcAddress("glBindRenderbuffer"));
	ocean_assert(glBindRenderbuffer != nullptr);
	if (glBindRenderbuffer == nullptr)
		return false;

	ocean_assert(glRenderbufferStorage == nullptr);
	glRenderbufferStorage = glRenderbufferStorageFunction(wglGetProcAddress("glRenderbufferStorage"));
	ocean_assert(glRenderbufferStorage != nullptr);
	if (glRenderbufferStorage == nullptr)
		return false;

	ocean_assert(glRenderbufferStorageMultisample == nullptr);
	glRenderbufferStorageMultisample = glRenderbufferStorageMultisampleFunction(wglGetProcAddress("glRenderbufferStorageMultisample"));
	ocean_assert(glRenderbufferStorageMultisample != nullptr);
	if (glRenderbufferStorageMultisample == nullptr)
		return false;

	ocean_assert(glFramebufferRenderbuffer == nullptr);
	glFramebufferRenderbuffer = glFramebufferRenderbufferFunction(wglGetProcAddress("glFramebufferRenderbuffer"));
	ocean_assert(glFramebufferRenderbuffer != nullptr);
	if (glFramebufferRenderbuffer == nullptr)
		return false;

	ocean_assert(glBlitFramebuffer == nullptr);
	glBlitFramebuffer = glBlitFramebufferFunction(wglGetProcAddress("glBlitFramebuffer"));
	ocean_assert(glBlitFramebuffer != nullptr);
	if (glBlitFramebuffer == nullptr)
		return false;

	ocean_assert(glCheckFramebufferStatus == nullptr);
	glCheckFramebufferStatus = glCheckFramebufferStatusFunction(wglGetProcAddress("glCheckFramebufferStatus"));
	ocean_assert(glCheckFramebufferStatus != nullptr);
	if (glCheckFramebufferStatus == nullptr)
		return false;

	ocean_assert(glGenBuffers == nullptr);
	glGenBuffers = glGenBuffersFunction(wglGetProcAddress("glGenBuffers"));
	ocean_assert(glGenBuffers != nullptr);
	if (glGenBuffers == nullptr)
		return false;

	ocean_assert(glDeleteBuffers == nullptr);
	glDeleteBuffers = glDeleteBuffersFunction(wglGetProcAddress("glDeleteBuffers"));
	ocean_assert(glDeleteBuffers != nullptr);
	if (glDeleteBuffers == nullptr)
		return false;

	ocean_assert(glBindBuffer == nullptr);
	glBindBuffer = glBindBufferFunction(wglGetProcAddress("glBindBuffer"));
	ocean_assert(glBindBuffer != nullptr);
	if (glBindBuffer == nullptr)
		return false;

	ocean_assert(glBufferData == nullptr);
	glBufferData = glBufferDataFunction(wglGetProcAddress("glBufferData"));
	ocean_assert(glBufferData != nullptr);
	if (glBufferData == nullptr)
		return false;

	ocean_assert(glCreateProgram == nullptr);
	glCreateProgram = glCreateProgramFunction(wglGetProcAddress("glCreateProgram"));
	ocean_assert(glCreateProgram != nullptr);
	if (glCreateProgram == nullptr)
		return false;

	ocean_assert(glDeleteProgram == nullptr);
	glDeleteProgram = glDeleteProgramFunction(wglGetProcAddress("glDeleteProgram"));
	ocean_assert(glDeleteProgram != nullptr);
	if (glDeleteProgram == nullptr)
		return false;

	ocean_assert(glIsProgram == nullptr);
	glIsProgram = glIsProgramFunction(wglGetProcAddress("glIsProgram"));
	ocean_assert(glIsProgram != nullptr);
	if (glIsProgram == nullptr)
		return false;

	ocean_assert(glAttachShader == nullptr);
	glAttachShader = glAttachShaderFunction(wglGetProcAddress("glAttachShader"));
	ocean_assert(glAttachShader != nullptr);
	if (glAttachShader == nullptr)
		return false;

	ocean_assert(glDetachShader == nullptr);
	glDetachShader = glDetachShaderFunction(wglGetProcAddress("glDetachShader"));
	ocean_assert(glDetachShader != nullptr);
	if (glDetachShader == nullptr)
		return false;

	ocean_assert(glLinkProgram == nullptr);
	glLinkProgram = glLinkProgramFunction(wglGetProcAddress("glLinkProgram"));
	ocean_assert(glLinkProgram != nullptr);
	if (glLinkProgram == nullptr)
		return false;

	ocean_assert(glUseProgram == nullptr);
	glUseProgram = glUseProgramFunction(wglGetProcAddress("glUseProgram"));
	ocean_assert(glUseProgram != nullptr);
	if (glUseProgram == nullptr)
		return false;

	ocean_assert(glCreateShader == nullptr);
	glCreateShader = glCreateShaderFunction(wglGetProcAddress("glCreateShader"));
	ocean_assert(glCreateShader != nullptr);
	if (glCreateShader == nullptr)
		return false;

	ocean_assert(glDeleteShader == nullptr);
	glDeleteShader = glDeleteShaderFunction(wglGetProcAddress("glDeleteShader"));
	ocean_assert(glDeleteShader != nullptr);
	if (glDeleteShader == nullptr)
		return false;

	ocean_assert(glCompileShader == nullptr);
	glCompileShader = glCompileShaderFunction(wglGetProcAddress("glCompileShader"));
	ocean_assert(glCompileShader != nullptr);
	if (glCompileShader == nullptr)
		return false;

	ocean_assert(glShaderSource == nullptr);
	glShaderSource = glShaderSourceFunction(wglGetProcAddress("glShaderSource"));
	ocean_assert(glShaderSource != nullptr);
	if (glShaderSource == nullptr)
		return false;

	ocean_assert(glGetProgramiv == nullptr);
	glGetProgramiv = glGetProgramivFunction(wglGetProcAddress("glGetProgramiv"));
	ocean_assert(glGetProgramiv != nullptr);
	if (glGetProgramiv == nullptr)
		return false;

	ocean_assert(glGetShaderiv == nullptr);
	glGetShaderiv = glGetShaderivFunction(wglGetProcAddress("glGetShaderiv"));
	ocean_assert(glGetShaderiv != nullptr);
	if (glGetShaderiv == nullptr)
		return false;

	ocean_assert(glGetProgramInfoLog == nullptr);
	glGetProgramInfoLog = glGetProgramInfoLogFunction(wglGetProcAddress("glGetProgramInfoLog"));
	ocean_assert(glGetProgramInfoLog != nullptr);
	if (glGetProgramInfoLog == nullptr)
		return false;

	ocean_assert(glGetShaderInfoLog == nullptr);
	glGetShaderInfoLog = glGetShaderInfoLogFunction(wglGetProcAddress("glGetShaderInfoLog"));
	ocean_assert(glGetShaderInfoLog != nullptr);
	if (glGetShaderInfoLog == nullptr)
		return false;

	ocean_assert(glGetAttribLocation == nullptr);
	glGetAttribLocation = glGetAttribLocationFunction(wglGetProcAddress("glGetAttribLocation"));
	ocean_assert(glGetAttribLocation != nullptr);
	if (glGetAttribLocation == nullptr)
		return false;

	ocean_assert(glGetUniformLocation == nullptr);
	glGetUniformLocation = glGetUniformLocationFunction(wglGetProcAddress("glGetUniformLocation"));
	ocean_assert(glGetUniformLocation != nullptr);
	if (glGetUniformLocation == nullptr)
		return false;

	ocean_assert(glUniform1i == nullptr);
	glUniform1i = glUniform1iFunction(wglGetProcAddress("glUniform1i"));
	ocean_assert(glUniform1i != nullptr);
	if (glUniform1i == nullptr)
		return false;

	ocean_assert(glUniform1fv == nullptr);
	glUniform1fv = glUniform2fvFunction(wglGetProcAddress("glUniform1fv"));
	ocean_assert(glUniform1fv != nullptr);
	if (glUniform1fv == nullptr)
		return false;

	ocean_assert(glUniform2fv == nullptr);
	glUniform2fv = glUniform2fvFunction(wglGetProcAddress("glUniform2fv"));
	ocean_assert(glUniform2fv != nullptr);
	if (glUniform2fv == nullptr)
		return false;

	ocean_assert(glUniform3fv == nullptr);
	glUniform3fv = glUniform3fvFunction(wglGetProcAddress("glUniform3fv"));
	ocean_assert(glUniform3fv != nullptr);
	if (glUniform3fv == nullptr)
		return false;

	ocean_assert(glUniform4fv == nullptr);
	glUniform4fv = glUniform4fvFunction(wglGetProcAddress("glUniform4fv"));
	ocean_assert(glUniform4fv != nullptr);
	if (glUniform4fv == nullptr)
		return false;

	ocean_assert(glUniformMatrix3fv == nullptr);
	glUniformMatrix3fv = glUniformMatrix3fvFunction(wglGetProcAddress("glUniformMatrix3fv"));
	ocean_assert(glUniformMatrix3fv != nullptr);
	if (glUniformMatrix3fv == nullptr)
		return false;

	ocean_assert(glUniformMatrix4fv == nullptr);
	glUniformMatrix4fv = glUniformMatrix4fvFunction(wglGetProcAddress("glUniformMatrix4fv"));
	ocean_assert(glUniformMatrix4fv != nullptr);
	if (glUniformMatrix4fv == nullptr)
		return false;

	ocean_assert(glEnableVertexAttribArray == nullptr);
	glEnableVertexAttribArray = glEnableVertexAttribArrayFunction(wglGetProcAddress("glEnableVertexAttribArray"));
	ocean_assert(glEnableVertexAttribArray != nullptr);
	if (glEnableVertexAttribArray == nullptr)
		return false;

	ocean_assert(glDisableVertexAttribArray == nullptr);
	glDisableVertexAttribArray = glDisableVertexAttribArrayFunction(wglGetProcAddress("glDisableVertexAttribArray"));
	ocean_assert(glDisableVertexAttribArray != nullptr);
	if (glDisableVertexAttribArray == nullptr)
		return false;

	ocean_assert(glVertexAttribPointer == nullptr);
	glVertexAttribPointer = glVertexAttribPointerFunction(wglGetProcAddress("glVertexAttribPointer"));
	ocean_assert(glVertexAttribPointer != nullptr);
	if (glVertexAttribPointer == nullptr)
		return false;

	ocean_assert(glActiveTexture == nullptr);
	glActiveTexture = glActiveTextureFunction(wglGetProcAddress("glActiveTexture"));
	ocean_assert(glActiveTexture != nullptr);
	if (glActiveTexture == nullptr)
		return false;

	ocean_assert(glBlendFuncSeparate == nullptr);
	glBlendFuncSeparate = glBlendFuncSeparateFunction(wglGetProcAddress("glBlendFuncSeparate"));
	ocean_assert(glBlendFuncSeparate != nullptr);
	if (glBlendFuncSeparate == nullptr)
		return false;

	ocean_assert(glTexImage2DMultisample == nullptr);
	glTexImage2DMultisample = glTexImage2DMultisampleFunction(wglGetProcAddress("glTexImage2DMultisample"));
	ocean_assert(glTexImage2DMultisample != nullptr);
	if (glTexImage2DMultisample == nullptr)
		return false;

	ocean_assert(glFramebufferTexture2D == nullptr);
	glFramebufferTexture2D = glFramebufferTexture2DFunction(wglGetProcAddress("glFramebufferTexture2D"));
	ocean_assert(glFramebufferTexture2D != nullptr);
	if (glFramebufferTexture2D == nullptr)
		return false;

	ocean_assert(glGenVertexArrays == nullptr);
	glGenVertexArrays = glGenVertexArraysFunction(wglGetProcAddress("glGenVertexArrays"));
	ocean_assert(glGenVertexArrays != nullptr);
	if (glGenVertexArrays == nullptr)
		return false;

	ocean_assert(glDeleteVertexArrays == nullptr);
	glDeleteVertexArrays = glDeleteVertexArraysFunction(wglGetProcAddress("glDeleteVertexArrays"));
	ocean_assert(glDeleteVertexArrays != nullptr);
	if (glDeleteVertexArrays == nullptr)
		return false;

	ocean_assert(glBindVertexArray == nullptr);
	glBindVertexArray = glBindVertexArrayFunction(wglGetProcAddress("glBindVertexArray"));
	ocean_assert(glBindVertexArray != nullptr);
	if (glBindVertexArray == nullptr)
		return false;

#else

	ocean_assert(glGenerateMipmap == nullptr);
	glGenerateMipmap = ::glGenerateMipmap;

	if (!initializeOpenGL33)
		return true;

	ocean_assert(glGenFramebuffers == nullptr);
	glGenFramebuffers = ::glGenFramebuffers;

	ocean_assert(glDeleteFramebuffers == nullptr);
	glDeleteFramebuffers = ::glDeleteFramebuffers;

	ocean_assert(glBindFramebuffer == nullptr);
	glBindFramebuffer = ::glBindFramebuffer;

	ocean_assert(glGenRenderbuffers == nullptr);
	glGenRenderbuffers = ::glGenRenderbuffers;

	ocean_assert(glDeleteRenderbuffers == nullptr);
	glDeleteRenderbuffers = ::glDeleteRenderbuffers;

	ocean_assert(glBindRenderbuffer == nullptr);
	glBindRenderbuffer = ::glBindRenderbuffer;

	ocean_assert(glRenderbufferStorage == nullptr);
	glRenderbufferStorage = ::glRenderbufferStorage;

	ocean_assert(glRenderbufferStorageMultisample == nullptr);
	glRenderbufferStorageMultisample = ::glRenderbufferStorageMultisample;

	ocean_assert(glFramebufferRenderbuffer == nullptr);
	glFramebufferRenderbuffer = ::glFramebufferRenderbuffer;

	ocean_assert(glBlitFramebuffer == nullptr);
	glBlitFramebuffer = ::glBlitFramebuffer;

	ocean_assert(glCheckFramebufferStatus == nullptr);
	glCheckFramebufferStatus = ::glCheckFramebufferStatus;

	ocean_assert(glGenBuffers == nullptr);
	glGenBuffers = ::glGenBuffers;

	ocean_assert(glDeleteBuffers == nullptr);
	glDeleteBuffers = ::glDeleteBuffers;

	ocean_assert(glBindBuffer == nullptr);
	glBindBuffer = ::glBindBuffer;

	ocean_assert(glBufferData == nullptr);
	glBufferData = ::glBufferData;

	ocean_assert(glCreateProgram == nullptr);
	glCreateProgram = ::glCreateProgram;

	ocean_assert(glDeleteProgram == nullptr);
	glDeleteProgram = ::glDeleteProgram;

	ocean_assert(glIsProgram == nullptr);
	glIsProgram = ::glIsProgram;

	ocean_assert(glAttachShader == nullptr);
	glAttachShader = ::glAttachShader;

	ocean_assert(glDetachShader == nullptr);
	glDetachShader = ::glDetachShader;

	ocean_assert(glLinkProgram == nullptr);
	glLinkProgram = ::glLinkProgram;

	ocean_assert(glUseProgram == nullptr);
	glUseProgram = ::glUseProgram;

	ocean_assert(glCreateShader == nullptr);
	glCreateShader = ::glCreateShader;

	ocean_assert(glDeleteShader == nullptr);
	glDeleteShader = ::glDeleteShader;

	ocean_assert(glCompileShader == nullptr);
	glCompileShader = ::glCompileShader;

	ocean_assert(glShaderSource == nullptr);
	glShaderSource = (glShaderSourceFunction)::glShaderSource;

	ocean_assert(glGetProgramiv == nullptr);
	glGetProgramiv = ::glGetProgramiv;

	ocean_assert(glGetShaderiv == nullptr);
	glGetShaderiv = ::glGetShaderiv;

	ocean_assert(glGetProgramInfoLog == nullptr);
	glGetProgramInfoLog = ::glGetProgramInfoLog;

	ocean_assert(glGetShaderInfoLog == nullptr);
	glGetShaderInfoLog = ::glGetShaderInfoLog;

	ocean_assert(glGetAttribLocation == nullptr);
	glGetAttribLocation = ::glGetAttribLocation;

	ocean_assert(glGetUniformLocation == nullptr);
	glGetUniformLocation = ::glGetUniformLocation;

	ocean_assert(glUniform1i == nullptr);
	glUniform1i = ::glUniform1i;

	ocean_assert(glUniform1fv == nullptr);
	glUniform1fv = ::glUniform1fv;

	ocean_assert(glUniform2fv == nullptr);
	glUniform2fv = ::glUniform2fv;

	ocean_assert(glUniform3fv == nullptr);
	glUniform3fv = ::glUniform3fv;

	ocean_assert(glUniform4fv == nullptr);
	glUniform4fv = ::glUniform4fv;

	ocean_assert(glUniformMatrix3fv == nullptr);
	glUniformMatrix3fv = ::glUniformMatrix3fv;

	ocean_assert(glUniformMatrix4fv == nullptr);
	glUniformMatrix4fv = ::glUniformMatrix4fv;

	ocean_assert(glEnableVertexAttribArray == nullptr);
	glEnableVertexAttribArray = ::glEnableVertexAttribArray;

	ocean_assert(glDisableVertexAttribArray == nullptr);
	glDisableVertexAttribArray = ::glDisableVertexAttribArray;

	ocean_assert(glVertexAttribPointer == nullptr);
	glVertexAttribPointer = ::glVertexAttribPointer;

	ocean_assert(glActiveTexture == nullptr);
	glActiveTexture = ::glActiveTexture;

	ocean_assert(glBlendFuncSeparate == nullptr);
	glBlendFuncSeparate = ::glBlendFuncSeparate;

	ocean_assert(glTexImage2DMultisample == nullptr);
	glTexImage2DMultisample = ::glTexImage2DMultisample;

	ocean_assert(glFramebufferTexture2D == nullptr);
	glFramebufferTexture2D = ::glFramebufferTexture2D;

	ocean_assert(glGenVertexArrays == nullptr);
	glGenVertexArrays = ::glGenVertexArrays;

	ocean_assert(glDeleteVertexArrays == nullptr);
	glDeleteVertexArrays = ::glDeleteVertexArrays;

	ocean_assert(glBindVertexArray == nullptr);
	glBindVertexArray = ::glBindVertexArray;

#endif

	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

bool Context::releaseOpenGLContext()
{
	if (handle_ == nullptr)
	{
		return true;
	}

	ocean_assert(Thread::currentThreadId() == threadId_);

	ocean_assert(glGetError() == GL_NO_ERROR);

#if defined(_WINDOWS)

	const bool result0 = wglMakeCurrent(nullptr, nullptr) == TRUE;
	ocean_assert(result0);

	const bool result1 = wglDeleteContext((HGLRC)handle_) == TRUE;
	ocean_assert(result1);

	const bool result = result0 && result1;

#elif defined(__APPLE__)

	const CGLError result0 = CGLSetCurrentContext(nullptr);
	ocean_assert(result0 == kCGLNoError);

	const CGLError result1 = CGLDestroyContext((CGLContextObj)handle_);
	ocean_assert(result1 == kCGLNoError);

	const bool result = result0 == kCGLNoError && result1 == kCGLNoError;

#else
	#warning Missing implementation.
#endif

	handle_ = nullptr;

#ifdef OCEAN_DEBUG
	threadId_ = Thread::ThreadId();
#endif

	return result;
}

#ifdef _WINDOWS

HGLRC Context::windowsCreateOpenGLContextHandle(HDC dc, const bool initializeOpenGL33, unsigned int& multisamples)
{
	ocean_assert(dc);
	ocean_assert(multisamples >= 1u);

	const PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,					// Color Bits Ignored
		8,									// No Alpha Buffer
		0,									// Shift Bit Ignored
		0,									// No Accumulation Buffer
		0, 0, 0, 0,							// Accumulation Bits Ignored
		24,									// 24Bit Z-Buffer (Depth Buffer)
		8,									// No Stencil Buffer
		0,									// No Auxiliary Buffer
		PFD_MAIN_PLANE,						// Main Drawing Layer
		0,									// Reserved
		0, 0, 0								// Layer Masks Ignored
	};

	GLuint pixelFormat = 0u;

	if (initializeOpenGL33 && multisamples >= 2u)
	{
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

		if (wglChoosePixelFormatARB)
		{
			unsigned int samples = multisamples;

			while (samples != 1u)
			{
				ocean_assert(samples != 0u);

				const int iAttributes[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_COLOR_BITS_ARB, 24,
					WGL_ALPHA_BITS_ARB, 8,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 0,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_SAMPLES_ARB, int(samples),
					0, 0
				};

				const float fAttributes[] =
				{
					0.0f, 0.0f
				};

				int pixelFormatARB = 0;
				UINT validFormats = 0u;
				if (wglChoosePixelFormatARB(dc, iAttributes, fAttributes, 1, &pixelFormatARB, &validFormats) == TRUE)
				{
					pixelFormat = GLuint(pixelFormatARB);
					multisamples = samples;
					break;
				}

				samples /= 2u;
			}
		}
	}

	if (pixelFormat == 0u)
	{
		pixelFormat = ChoosePixelFormat(dc, &pixelFormatDescriptor);
		if (pixelFormat == 0)
			return nullptr;

		multisamples = 1u;
	}

	ocean_assert(pixelFormat != 0u);
	if (!SetPixelFormat(dc, pixelFormat, &pixelFormatDescriptor))
		return nullptr;

	HGLRC contextHandle = nullptr;

	if (initializeOpenGL33)
	{
		HGLRC dummyContext = wglCreateContext(dc);
		ocean_assert(dummyContext);

		const bool result0 = wglMakeCurrent(dc, dummyContext) == TRUE;
		ocean_assert_and_suppress_unused(result0, result0);
		ocean_assert(glGetError() == GL_NO_ERROR);

		wglCreateContextAttribsARBFunction wglCreateContextAttribsARB = wglCreateContextAttribsARBFunction(wglGetProcAddress("wglCreateContextAttribsARB"));

		const bool result1 = wglMakeCurrent(nullptr, nullptr) == TRUE;
		ocean_assert_and_suppress_unused(result1, result1);

		const bool result2 = wglDeleteContext((HGLRC)dummyContext) == TRUE;
		ocean_assert_and_suppress_unused(result2, result2);

		dummyContext = nullptr;

		if (wglCreateContextAttribsARB)
		{
			const int contextAttributes[] =
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 3,
				WGL_CONTEXT_FLAGS_ARB, 0,
				0
			};

			contextHandle = wglCreateContextAttribsARB(dc, nullptr, contextAttributes);
		}
	}

	// if we could did not intended a 3.3 OpenGL profile or if we could not create a context with such a profile we have to do it the old way
	if (contextHandle == nullptr)
		contextHandle = wglCreateContext(dc);

	return contextHandle;
}

#endif

}

}

}
