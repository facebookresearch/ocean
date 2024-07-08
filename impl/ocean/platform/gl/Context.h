/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_CONTEXT_H
#define META_OCEAN_PLATFORM_GL_CONTEXT_H

#include "ocean/platform/gl/GL.h"

#include "ocean/base/Thread.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class encapsulates an OpenGL context.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT Context
{
	public:

		/**
		 * Definition of a set holding strings.
		 */
		typedef std::set<std::string> StringSet;

		/**
		 * Definition of the glGetStringi function pointer type.
		 */
		typedef const GLubyte*(*glGetStringiFunction)(GLenum, GLuint);

		/**
		 * Definition of the glGenFramebuffers function pointer type.
		 */
		typedef void (*glGenFramebuffersFunction)(GLsizei, GLuint*);

		/**
		 * Definition of the glDeleteFramebuffers function pointer type.
		 */
		typedef void (*glDeleteFramebuffersFunction)(GLsizei, const GLuint*);

		/**
		 * Definition of the glBindFramebuffer function pointer type.
		 */
		typedef void (*glBindFramebufferFunction)(GLenum, GLuint);

		/**
		 * Definition of the glGenRenderbuffers function pointer type.
		 */
		typedef void (*glGenRenderbuffersFunction)(GLsizei, GLuint*);

		/**
		 * Definition of the glDeleteRenderbuffers function pointer type.
		 */
		typedef void (*glDeleteRenderbuffersFunction)(GLsizei, const GLuint*);

		/**
		 * Definition of the glBindRenderbuffer function pointer type.
		 */
		typedef void (*glBindRenderbufferFunction)(GLenum, GLuint);

		/**
		 * Definition of the glRenderbufferStorage function pointer type.
		 */
		typedef void (*glRenderbufferStorageFunction)(GLenum, GLenum, GLsizei, GLsizei);

		/**
		 * Definition of the glRenderbufferStorageMultisample function pointer type.
		 */
		typedef void (*glRenderbufferStorageMultisampleFunction)(GLenum, GLsizei, GLenum, GLsizei, GLsizei);

		/**
		 * Definition of the glFramebufferRenderbuffer function pointer type.
		 */
		typedef void (*glFramebufferRenderbufferFunction)(GLenum, GLenum, GLenum, GLuint);

		/**
		 * Definition of the glBlitFramebuffer function pointer type.
		 */
		typedef void(*glBlitFramebufferFunction)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);

		/**
		 * Definition of the glCheckFramebufferStatus function pointer type.
		 */
		typedef GLenum (*glCheckFramebufferStatusFunction)(GLenum);

		/**
		 * Definition of the glGenBuffers function pointer type.
		 */
		typedef void (*glGenBuffersFunction)(GLsizei, GLuint*);

		/**
		 * Definition of the glDeleteBuffers function pointer type.
		 */
		typedef void (*glDeleteBuffersFunction)(GLsizei, const GLuint*);

		/**
		 * Definition of the glBindBuffer function pointer type.
		 */
		typedef void (*glBindBufferFunction)(GLenum, GLuint);

		/**
		 * Definition of the glBufferData function pointer type.
		 */
		typedef void (*glBufferDataFunction)(GLenum, GLsizeiptr, const GLvoid*, GLenum);

		/**
		 * Definition of the glCreateProgram function pointer type.
		 */
		typedef GLuint (*glCreateProgramFunction)();

		/**
		 * Definition of the glDeleteProgram function pointer type.
		 */
		typedef void (*glDeleteProgramFunction)(GLuint);

		/**
		 * Definition of the glIsProgram function pointer type.
		 */
		typedef GLboolean (*glIsProgramFunction)(GLuint);

		/**
		 * Definition of the glAttachShader function pointer type.
		 */
		typedef void (*glAttachShaderFunction)(GLuint, GLuint);

		/**
		 * Definition of the glDetachShader function pointer type.
		 */
		typedef void (*glDetachShaderFunction)(GLuint, GLuint);

		/**
		 * Definition of the glLinkProgram function pointer type.
		 */
		typedef void (*glLinkProgramFunction)(GLuint);

		/**
		 * Definition of the glUseProgram function pointer type.
		 */
		typedef void (*glUseProgramFunction)(GLuint);

		/**
		 * Definition of the glCreateShader function pointer type.
		 */
		typedef GLuint (*glCreateShaderFunction)(GLenum);

		/**
		 * Definition of the glDeleteShader function pointer type.
		 */
		typedef void (*glDeleteShaderFunction)(GLuint);

		/**
		 * Definition of the glCompileShader function pointer type.
		 */
		typedef void (*glCompileShaderFunction)(GLuint);

		/**
		 * Definition of the glShaderSource function pointer type.
		 */
		typedef void (*glShaderSourceFunction)(GLuint, GLsizei, const GLchar**, const GLint*);

		/**
		 * Definition of the glGetProgramiv function pointer type.
		 */
		typedef void (*glGetProgramivFunction)(GLuint, GLenum, GLint*);

		/**
		 * Definition of the glGetShaderiv function pointer type.
		 */
		typedef void (*glGetShaderivFunction)(GLuint, GLenum, GLint*);

		/**
		 * Definition of the glGetProgramInfoLog function pointer type.
		 */
		typedef void (*glGetProgramInfoLogFunction)(GLuint, GLsizei, GLsizei*, GLchar*);

		/**
		 * Definition of the glGetShaderInfoLog function pointer type.
		 */
		typedef void (*glGetShaderInfoLogFunction)(GLuint, GLsizei, GLsizei*, GLchar*);

		/**
		 * Definition of the glGetAttribLocation function pointer type.
		 */
		typedef GLint (*glGetAttribLocationFunction)(GLuint, const GLchar*);

		/**
		 * Definition of the glGetUniformLocation function pointer type.
		 */
		typedef GLint (*glGetUniformLocationFunction)(GLuint, const GLchar*);

		/**
		 * Definition of the glUniform1i function pointer type.
		 */
		typedef void (*glUniform1iFunction)(GLint, GLint);

		/**
		 * Definition of the glUniform1fv function pointer type.
		 */
		typedef void (*glUniform1fvFunction)(GLint, GLsizei, const GLfloat*);

		/**
		 * Definition of the glUniform2fv function pointer type.
		 */
		typedef void (*glUniform2fvFunction)(GLint, GLsizei, const GLfloat*);

		/**
		 * Definition of the glUniform3fv function pointer type.
		 */
		typedef void (*glUniform3fvFunction)(GLint, GLsizei, const GLfloat*);

		/**
		 * Definition of the glUniform4fv function pointer type.
		 */
		typedef void (*glUniform4fvFunction)(GLint, GLsizei, const GLfloat*);

		/**
		 * Definition of the glUniformMatrix3fv function pointer type.
		 */
		typedef void(*glUniformMatrix3fvFunction)(GLint, GLsizei, GLboolean, const GLfloat*);

		/**
		 * Definition of the glUniformMatrix4fv function pointer type.
		 */
		typedef void (*glUniformMatrix4fvFunction)(GLint, GLsizei, GLboolean, const GLfloat*);

		/**
		 * Definition of the glEnableVertexAttribArray function pointer type.
		 */
		typedef void (*glEnableVertexAttribArrayFunction)(GLuint);

		/**
		 * Definition of the glDisableVertexAttribArray function pointer type.
		 */
		typedef void (*glDisableVertexAttribArrayFunction)(GLuint);

		/**
		 * Definition of the glVertexAttribPointer function pointer type.
		 */
		typedef void (*glVertexAttribPointerFunction)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);

		/**
		 * Definition of the glActiveTexture function pointer type.
		 */
		typedef void (*glActiveTextureFunction)(GLenum);

		/**
		 * Definition of the glGenerateMipmap function pointer type.
		 */
		typedef void (*glGenerateMipmapFunction)(GLenum);

		/**
		 * Definition of the glBlendFuncSeparate function pointer type.
		 */
		typedef void (*glBlendFuncSeparateFunction)(GLenum, GLenum, GLenum, GLenum);

		/**
		 * Definition of the glTexImage2DMultisample function pointer type.
		 */
		typedef void (*glTexImage2DMultisampleFunction)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);

		/**
		 * Definition of the glFramebufferTexture2D function pointer type.
		 */
		typedef void (*glFramebufferTexture2DFunction)(GLenum, GLenum, GLenum, GLuint, GLint);

		/**
		 * Definition of the glBindFragDataLocation function pointer type.
		 */
		typedef void (*glBindFragDataLocationFunction)(GLuint, GLuint, const char*);

		/**
		 * Definition of the glGenVertexArrays function pointer type.
		 */
		typedef void (*glGenVertexArraysFunction)(GLsizei, GLuint*);

		/**
		 * Definition of the glDeleteVertexArrays function pointer type.
		 */
		typedef void (*glDeleteVertexArraysFunction)(GLsizei, const GLuint*);

		/**
		 * Definition of the glBindVertexArray function pointer type.
		 */
		typedef void(*glBindVertexArrayFunction)(GLuint);

#ifdef _WINDOWS

		typedef HGLRC (*wglCreateContextAttribsARBFunction)(HDC, HGLRC, const int*);

#endif

	public:

		/**
		 * Releases the context.
		 */
		virtual ~Context();

		/**
		 * Makes the OpenGL rendering context of this framebuffer the calling thread's current rendering context or makes the calling thread's current rendering context not longer current.
		 * @param state True, to make the context current; False, to make the current context not current any longer
		 */
		virtual void makeCurrent(const bool state = true) = 0;

		/**
		 * Swaps the foreground and background buffer if this context has two buffers.
		 * @return True, if this context has two buffers
		 */
		virtual bool swapBuffers();

		/**
		 * Returns the major OpenGL version of this context.
		 * The determination of the version may fail if the version is below 3.0.<br>
		 * @return The context's major OpenGL version, 0 if the major version could not be determined
		 */
		inline unsigned int majorVersion() const;

		/**
		 * Returns the minor OpenGL version of this context.
		 * The determination of the version may fail if the version is below 3.0.<br>
		 * @return The context's minor OpenGL version, 0 if the major version could not be determined
		 */
		inline unsigned int minorVersion() const;

		/**
		 * Returns whether this context provides the compatibility profile or the core profile.
		 * In general, the compatibility profile allows to use also deprecated functions.<br>
		 * However, an existing core profile does not guarantee a compatibility profile with same features (OpenGL version).
		 * @return True, if this context provides the compatibility profile
		 */
		inline bool compatibilityProfile() const;

		/**
		 * Returns the set of supported extensions.
		 * @return The supported extensions
		 */
		inline const StringSet& supportedExtensions() const;

		/**
		 * Returns whether a specific extension is supported.
		 * @param extension The extension to check
		 * @return True, if so
		 */
		inline bool isExtensionSupported(const std::string& extension) const;

		/**
		 * Returns the number of multi samples that this context supports.
		 * @return The context's number of multi samples
		 */
		inline unsigned int multisamples() const;

		/**
		 * Releases the context explicitly.
		 */
		virtual void release();

		/**
		 * Returns whether this context is valid.
		 * @return True, if so
		 */
		virtual bool isValid() const;

	protected:

		/**
		 * Creates a new context.
		 */
		Context() = default;

		/**
		 * Creates the OpenGL context for this object.
		 * @param initializeOpenGL33 True, to initialize the capabilities of OpenGL 3.3 and higher
		 * @param multisamples The number of multisamples requested for the context
		 * @return True, if succeeded
		 */
		virtual bool createOpenGLContext(const bool initializeOpenGL33 = true, const unsigned int multisamples = 1u);

		/**
		 * Releases the OpenGL context of this framebuffer.
		 * @return True, if succeeded
		 */
		virtual bool releaseOpenGLContext();

#ifdef _WINDOWS
		/**
		 * Creates a the OpennGL context handle on Windows platforms.
		 * @param dc The device context for which the OpenGL context will be created, must be valid
		 * @param initializeOpenGL33 True, to initialize the capabilities of OpenGL 3.3 and higher
		 * @param multisamples The number of multisamples requested for the context and also the resulting number of multisamples if the context could be created successfully, with range [1, infinity)
		 * @return Resulting OpenGL context handle, if the context could be created
		 */
		static HGLRC windowsCreateOpenGLContextHandle(HDC dc, const bool initializeOpenGL33, unsigned int& multisamples);
#endif

	public:

		/// The glGetStringi function.
		glGetStringiFunction glGetStringi = nullptr;

		/// The glGenFramebuffers function.
		glGenFramebuffersFunction glGenFramebuffers = nullptr;

		/// The glDeleteFramebuffer function.
		glDeleteFramebuffersFunction glDeleteFramebuffers = nullptr;

		/// The glBindFramebuffer function.
		glBindFramebufferFunction glBindFramebuffer = nullptr;

		/// The glGenRenderbuffers functions.
		glGenRenderbuffersFunction glGenRenderbuffers = nullptr;

		/// The glDeleteRenderbuffers function.
		glDeleteRenderbuffersFunction glDeleteRenderbuffers = nullptr;

		/// The glBindRenderbuffer function.
		glBindRenderbufferFunction glBindRenderbuffer = nullptr;

		/// The glRenderbufferStorage function.
		glRenderbufferStorageFunction glRenderbufferStorage = nullptr;

		/// The glRenderbufferStorageMultisample function.
		glRenderbufferStorageMultisampleFunction glRenderbufferStorageMultisample = nullptr;

		/// The glFramebufferRenderbuffer function.
		glFramebufferRenderbufferFunction glFramebufferRenderbuffer = nullptr;

		/// The glBlitFramebuffer function;
		glBlitFramebufferFunction glBlitFramebuffer = nullptr;

		/// The glCheckFramebufferStatus function.
		glCheckFramebufferStatusFunction glCheckFramebufferStatus = nullptr;

		/// The glGenBuffers function.
		glGenBuffersFunction glGenBuffers = nullptr;

		/// The glDeleteBuffers function.
		glDeleteBuffersFunction glDeleteBuffers = nullptr;

		/// The glBindBuffer function.
		glBindBufferFunction glBindBuffer = nullptr;

		/// The glBufferData function.
		glBufferDataFunction glBufferData = nullptr;

		/// The glCreateProgram function.
		glCreateProgramFunction glCreateProgram = nullptr;

		/// The glDeleteProgram function.
		glDeleteProgramFunction glDeleteProgram = nullptr;

		/// The glIsProgram function.
		glIsProgramFunction glIsProgram = nullptr;

		/// The glAttachShader function.
		glAttachShaderFunction glAttachShader = nullptr;

		/// The glDetachShader function.
		glDetachShaderFunction glDetachShader = nullptr;

		/// The glLinkProgram function.
		glLinkProgramFunction glLinkProgram = nullptr;

		/// The glUseProgram function.
		glUseProgramFunction glUseProgram = nullptr;

		/// The glCreateShader function.
		glCreateShaderFunction glCreateShader = nullptr;

		/// The glDeleteShader function.
		glDeleteShaderFunction glDeleteShader = nullptr;

		/// The glCompileShader function.
		glCompileShaderFunction glCompileShader = nullptr;

		/// The glShaderSource function.
		glShaderSourceFunction glShaderSource = nullptr;

		/// The glGetProgramiv function.
		glGetProgramivFunction glGetProgramiv = nullptr;

		/// The glGetShaderiv function.
		glGetShaderivFunction glGetShaderiv = nullptr;

		/// The glGetProgramInfoLog function.
		glGetProgramInfoLogFunction glGetProgramInfoLog = nullptr;

		/// The glGetShaderInfoLog function.
		glGetShaderInfoLogFunction glGetShaderInfoLog = nullptr;

		/// The glGetAttribLocation function.
		glGetAttribLocationFunction glGetAttribLocation = nullptr;

		/// The glGetUniformLocation function.
		glGetUniformLocationFunction glGetUniformLocation = nullptr;

		/// The glUniform1i function.
		glUniform1iFunction glUniform1i = nullptr;

		/// The glUniform1fv function.
		glUniform1fvFunction glUniform1fv = nullptr;

		/// The glUniform2fv function.
		glUniform2fvFunction glUniform2fv = nullptr;

		/// The glUniform3fv function.
		glUniform3fvFunction glUniform3fv = nullptr;

		/// The glUniform4fv function.
		glUniform4fvFunction glUniform4fv = nullptr;

		/// The glUniformMatrix3fv function.
		glUniformMatrix3fvFunction glUniformMatrix3fv = nullptr;

		/// The glUniformMatrix4fv function.
		glUniformMatrix4fvFunction glUniformMatrix4fv = nullptr;

		/// The glEnableVertexAttribArray function.
		glEnableVertexAttribArrayFunction glEnableVertexAttribArray = nullptr;

		/// The glEnableVertexAttribArray function.
		glDisableVertexAttribArrayFunction glDisableVertexAttribArray = nullptr;

		/// The glVertexAttribPointer function.
		glVertexAttribPointerFunction glVertexAttribPointer = nullptr;

		/// The glActiveTexture function.
		glActiveTextureFunction glActiveTexture = nullptr;

		/// The glGenerateMipmap function.
		glGenerateMipmapFunction glGenerateMipmap = nullptr;

		/// The glBlendFuncSeparate function.
		glBlendFuncSeparateFunction glBlendFuncSeparate = nullptr;

		/// The glTexImage2DMultisample function.
		glTexImage2DMultisampleFunction glTexImage2DMultisample = nullptr;

		/// The glFramebufferTexture2D function.
		glFramebufferTexture2DFunction glFramebufferTexture2D = nullptr;

		/// The glGenVertexArrays function.
		glGenVertexArraysFunction glGenVertexArrays = nullptr;

		/// The glDeleteVertexArrays function.
		glDeleteVertexArraysFunction glDeleteVertexArrays = nullptr;

		/// glBindVertexArray function.
		glBindVertexArrayFunction glBindVertexArray = nullptr;

	protected:

		/// The OpenGL context handle.
		void* handle_ = nullptr;

		/// The major version of this OpenGL context.
		unsigned int majorVersion_ = 0u;

		/// The minor version of this OpenGL context.
		unsigned int minorVersion_ = 0u;

		/// True, if this context provides the compatibility profile.
		bool compatibilityProfile_ = false;

		/// The set of supported extensions.
		StringSet extensions_;

		/// The number of multi samples of this context.
		unsigned int multisamples_ = 0u;

#ifdef OCEAN_DEBUG
		// The id of the thread which is responsible for the context.
		Thread::ThreadId threadId_;
#endif
};

inline unsigned int Context::majorVersion() const
{
	return majorVersion_;
}

inline unsigned int Context::minorVersion() const
{
	return minorVersion_;
}

inline bool Context::compatibilityProfile() const
{
	return compatibilityProfile_;
}

inline const Context::StringSet& Context::supportedExtensions() const
{
	return extensions_;
}

inline bool Context::isExtensionSupported(const std::string& extension) const
{
	return extensions_.find(extension) != extensions_.end();
}

inline unsigned int Context::multisamples() const
{
	return multisamples_;
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_CONTEXT_H
