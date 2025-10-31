/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_DYNAMIC_LIBRARY_H
#define META_OCEAN_RENDERING_GLES_DYNAMIC_LIBRARY_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"

#ifdef OCEAN_RENDERING_GLES_USE_DYNAMIC_LIBRARY

#define GL_GLEXT_PROTOTYPES

#if defined(OCEAN_PLATFORM_BUILD_LINUX)
	#include <GL/gl.h>
	#include <GL/glext.h>
#else
	#include <gl/GL.h>
	#include <gl/glext.h>
#endif

namespace Ocean
{

#define glActiveTexture(a)                                 Rendering::GLESceneGraph::GLESDynamicLibrary::glActiveTexture_(a)
#define glAttachShader(a, b)                               Rendering::GLESceneGraph::GLESDynamicLibrary::glAttachShader_(a, b)
#define glBindBuffer(a, b)                                 Rendering::GLESceneGraph::GLESDynamicLibrary::glBindBuffer_(a, b)
#define glBindFramebuffer(a, b)                            Rendering::GLESceneGraph::GLESDynamicLibrary::glBindFramebuffer_(a, b)
#define glBindVertexArray(a)                               Rendering::GLESceneGraph::GLESDynamicLibrary::glBindVertexArray_(a)
#define glBufferData(a, b, c, d)                           Rendering::GLESceneGraph::GLESDynamicLibrary::glBufferData_(a, b, c, d)
#define glCheckFramebufferStatus(a)                        Rendering::GLESceneGraph::GLESDynamicLibrary::glCheckFramebufferStatus_(a)
#define glClearDepthf(a)                                   Rendering::GLESceneGraph::GLESDynamicLibrary::glClearDepthf_(a)
#define glCompileShader(a)                                 Rendering::GLESceneGraph::GLESDynamicLibrary::glCompileShader_(a)
#define glCompressedTexImage2D(a, b, c, d, e, f, g, h)     Rendering::GLESceneGraph::GLESDynamicLibrary::glCompressedTexImage2D_(a, b, c, d, e, f, g, h)
#define glCreateProgram()                                  Rendering::GLESceneGraph::GLESDynamicLibrary::glCreateProgram_()
#define glCreateShader(a)                                  Rendering::GLESceneGraph::GLESDynamicLibrary::glCreateShader_(a)
#define glDebugMessageCallback(a, b)                       Rendering::GLESceneGraph::GLESDynamicLibrary::glDebugMessageCallback_(a, b)
#define glDeleteBuffers(a, b)                              Rendering::GLESceneGraph::GLESDynamicLibrary::glDeleteBuffers_(a, b)
#define glDeleteFramebuffers(a, b)                         Rendering::GLESceneGraph::GLESDynamicLibrary::glDeleteFramebuffers_(a, b)
#define glDeleteProgram(a)                                 Rendering::GLESceneGraph::GLESDynamicLibrary::glDeleteProgram_(a)
#define glDeleteShader(a)                                  Rendering::GLESceneGraph::GLESDynamicLibrary::glDeleteShader_(a)
#define glDeleteTextures(a, b)                             Rendering::GLESceneGraph::GLESDynamicLibrary::glDeleteTextures_(a, b)
#define glDeleteVertexArrays(a, b)                         Rendering::GLESceneGraph::GLESDynamicLibrary::glDeleteVertexArrays_(a, b)
#define glDetachShader(a, b)                               Rendering::GLESceneGraph::GLESDynamicLibrary::glDetachShader_(a, b)
#define glDrawArrays(a, b, c)                              Rendering::GLESceneGraph::GLESDynamicLibrary::glDrawArrays_(a, b, c)
#define glDrawElements(a, b, c, d)                         Rendering::GLESceneGraph::GLESDynamicLibrary::glDrawElements_(a, b, c, d)
#define glEnableVertexAttribArray(a)                       Rendering::GLESceneGraph::GLESDynamicLibrary::glEnableVertexAttribArray_(a)
#define glFramebufferTexture2D(a, b, c, d, e)              Rendering::GLESceneGraph::GLESDynamicLibrary::glFramebufferTexture2D_(a, b, c, d, e)
#define glGenBuffers(a, b)                                 Rendering::GLESceneGraph::GLESDynamicLibrary::glGenBuffers_(a, b)
#define glGenerateMipmap(a)                                Rendering::GLESceneGraph::GLESDynamicLibrary::glGenerateMipmap_(a)
#define glGenFramebuffers(a, b)                            Rendering::GLESceneGraph::GLESDynamicLibrary::glGenFramebuffers_(a, b)
#define glGenTextures(a, b)                                Rendering::GLESceneGraph::GLESDynamicLibrary::glGenTextures_(a, b)
#define glGenVertexArrays(a, b)                            Rendering::GLESceneGraph::GLESDynamicLibrary::glGenVertexArrays_(a, b)
#define glGetAttribLocation(a, b)                          Rendering::GLESceneGraph::GLESDynamicLibrary::glGetAttribLocation_(a, b)
#define glGetProgramInfoLog(a, b, c, d)                    Rendering::GLESceneGraph::GLESDynamicLibrary::glGetProgramInfoLog_(a, b, c, d)
#define glGetProgramiv(a, b, c)                            Rendering::GLESceneGraph::GLESDynamicLibrary::glGetProgramiv_(a, b, c)
#define glGetShaderInfoLog(a, b, c, d)                     Rendering::GLESceneGraph::GLESDynamicLibrary::glGetShaderInfoLog_(a, b, c, d)
#define glGetShaderiv(a, b, c)                             Rendering::GLESceneGraph::GLESDynamicLibrary::glGetShaderiv_(a, b, c)
#define glGetStringi(a, b)                                 Rendering::GLESceneGraph::GLESDynamicLibrary::glGetStringi_(a, b)
#define glGetUniformLocation(a, b)                         Rendering::GLESceneGraph::GLESDynamicLibrary::glGetUniformLocation_(a, b)
#define glIsProgram(a)                                     Rendering::GLESceneGraph::GLESDynamicLibrary::glIsProgram_(a)
#define glLinkProgram(a)                                   Rendering::GLESceneGraph::GLESDynamicLibrary::glLinkProgram_(a)
#define glReleaseShaderCompiler()                          Rendering::GLESceneGraph::GLESDynamicLibrary::glReleaseShaderCompiler_()
#define glShaderSource(a, b, c, d)                         Rendering::GLESceneGraph::GLESDynamicLibrary::glShaderSource_(a, b, c, d)
#define glTexImage2DMultisample(a, b, c, d, e, f)          Rendering::GLESceneGraph::GLESDynamicLibrary::glTexImage2DMultisample_(a, b, c, d, e, f)
#define glUniform1f(a, b)                                  Rendering::GLESceneGraph::GLESDynamicLibrary::glUniform1f_(a, b)
#define glUniform1i(a, b)                                  Rendering::GLESceneGraph::GLESDynamicLibrary::glUniform1i_(a, b)
#define glUniform1ui(a, b)                                 Rendering::GLESceneGraph::GLESDynamicLibrary::glUniform1ui_(a, b)
#define glUniform2f(a, b, c)                               Rendering::GLESceneGraph::GLESDynamicLibrary::glUniform2f_(a, b, c)
#define glUniform2fv(a, b, c)                              Rendering::GLESceneGraph::GLESDynamicLibrary::glUniform2fv_(a, b, c)
#define glUniform3f(a, b, c, d)                            Rendering::GLESceneGraph::GLESDynamicLibrary::glUniform3f_(a, b, c, d)
#define glUniform3fv(a, b, c)                              Rendering::GLESceneGraph::GLESDynamicLibrary::glUniform3fv_(a, b, c)
#define glUniform4f(a, b, c, d, e)                         Rendering::GLESceneGraph::GLESDynamicLibrary::glUniform4f_(a, b, c, d, e)
#define glUniform4fv(a, b, c)                              Rendering::GLESceneGraph::GLESDynamicLibrary::glUniform4fv_(a, b, c)
#define glUniform1fv(a, b, c)                              Rendering::GLESceneGraph::GLESDynamicLibrary::glUniform1fv_(a, b, c)
#define glUniformMatrix3fv(a, b, c, d)                     Rendering::GLESceneGraph::GLESDynamicLibrary::glUniformMatrix3fv_(a, b, c, d)
#define glUniformMatrix4fv(a, b, c, d)                     Rendering::GLESceneGraph::GLESDynamicLibrary::glUniformMatrix4fv_(a, b, c, d)
#define glUseProgram(a)                                    Rendering::GLESceneGraph::GLESDynamicLibrary::glUseProgram_(a)
#define glVertexAttribPointer(a, b, c, d, e, f)            Rendering::GLESceneGraph::GLESDynamicLibrary::glVertexAttribPointer_(a, b, c, d, e, f)
#define glVertexAttribIPointer(a, b, c, d, e)              Rendering::GLESceneGraph::GLESDynamicLibrary::glVertexAttribIPointer_(a, b, c, d, e)

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class stores the OpenGL function pointers for OpenGL 2.0+ functions
  * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESDynamicLibrary
{
	public:

		using DebugCallbackFunction = void (*)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

		using glActiveTextureFunction = void (__stdcall *)(GLenum texture);
		using glAttachShaderFunction = void (__stdcall *)(GLuint, GLuint);
		using glBindBufferFunction = void (__stdcall *)(GLenum, GLuint);
		using glBindFramebufferFunction = void (__stdcall *)(GLenum target, GLuint framebuffer);
		using glBindVertexArrayFunction = void (__stdcall *)(GLuint  array);
		using glBufferDataFunction = void (__stdcall *)(GLenum, GLsizeiptr, const void*, GLenum);
		using glCheckFramebufferStatusFunction = GLenum (__stdcall *)(GLenum target);
		using glClearDepthfFunction = void (__stdcall *)(GLclampf);
		using glCompileShaderFunction = void (__stdcall *)(GLuint);
		using glCompressedTexImage2DFunction = void (__stdcall *)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void*);
		using glCreateProgramFunction = GLuint (__stdcall *)();
		using glCreateShaderFunction = GLuint (__stdcall *)(GLenum type);
		using glDebugMessageCallbackFunction = void (__stdcall *)(DebugCallbackFunction, const void*);
		using glDeleteBuffersFunction = void (__stdcall *)(GLsizei, const GLuint*);
		using glDeleteFramebuffersFunction = void (__stdcall *)(GLsizei n, const GLuint * framebuffers);
		using glDeleteProgramFunction = void (__stdcall *)(GLuint);
		using glDeleteShaderFunction = void (__stdcall *)(GLuint);
		using glDeleteTexturesFunction = void (__stdcall *)(GLsizei, const GLuint*);
		using glDeleteVertexArraysFunction = void (__stdcall *)(GLsizei, const GLuint*);
		using glDetachShaderFunction = void (__stdcall *)(GLuint, GLuint);
		using glDrawArraysFunction = void (__stdcall *)(GLenum, GLint, GLsizei);
		using glDrawElementsFunction = void (__stdcall *)(GLenum, GLsizei, GLenum, const void*);
		using glEnableVertexAttribArrayFunction = void (__stdcall *)(GLuint index);
		using glFramebufferTexture2DFunction = void (__stdcall *)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
		using glGenBuffersFunction = void (__stdcall *)(GLsizei, GLuint*);
		using glGenerateMipmapFunction = void (__stdcall *)(GLenum target);
		using glGenFramebuffersFunction = void (__stdcall *)(GLsizei n, GLuint *ids);
		using glGenTexturesFunction = void (__stdcall *)(GLsizei, GLuint*);
		using glGenVertexArraysFunction = void (__stdcall *)(GLsizei n, GLuint *arrays);
		using glGetAttribLocationFunction = int (__stdcall *)(GLuint, const char*);
		using glGetProgramInfoLogFunction = void (__stdcall *)(GLuint, GLsizei, GLsizei*, char*);
		using glGetProgramivFunction = void (__stdcall *)(GLuint, GLenum, GLint*);
		using glGetShaderInfoLogFunction = void (__stdcall *)(GLuint, GLsizei, GLsizei*, char*);
		using glGetShaderivFunction = void (__stdcall *)(GLuint, GLenum, GLint*);
		using glGetStringiFunction = GLubyte* (__stdcall *)(GLenum, GLuint);
		using glGetUniformLocationFunction = int (__stdcall *)(GLuint, const char*);
		using glIsProgramFunction = GLboolean (__stdcall *)(GLuint program);
		using glLinkProgramFunction = void (__stdcall *)(GLuint);
		using glReleaseShaderCompilerFunction = void (__stdcall *)();
		using glShaderSourceFunction = void (__stdcall *)(GLuint, GLsizei, const char**, const GLint*);
		using glTexImage2DMultisampleFunction = void (__stdcall *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
		using glUniform1fFunction = void (__stdcall *)(GLint location, GLfloat x);
		using glUniform1fvFunction = void (__stdcall *)(GLint, GLsizei, const GLfloat*);
		using glUniform1iFunction = void (__stdcall *)(GLint, GLint);
		using glUniform1uiFunction = void (__stdcall *)(GLint, GLuint);
		using glUniform2fFunction = void (__stdcall *)(GLint, GLfloat, GLfloat);
		using glUniform2fvFunction = void (__stdcall *)(GLint location, GLsizei count, const GLfloat* v);
		using glUniform3fFunction = void (__stdcall *)(GLint, GLfloat, GLfloat, GLfloat);
		using glUniform3fvFunction = void (__stdcall *)(GLint location, GLsizei count, const GLfloat* v);
		using glUniform4fFunction = void (__stdcall *)(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		using glUniform4fvFunction = void (__stdcall *)(GLint location, GLsizei count, const GLfloat* v);
		using glUniformMatrix3fvFunction = void (__stdcall *)(GLint, GLsizei, GLboolean, const GLfloat*);
		using glUniformMatrix4fvFunction = void (__stdcall *)(GLint, GLsizei, GLboolean, const GLfloat*);
		using glUseProgramFunction = void (__stdcall *)(GLuint);
		using glVertexAttribPointerFunction = void (__stdcall *)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
		using glVertexAttribIPointerFunction = void (__stdcall *)(GLuint, GLint, GLenum, GLsizei, const void*);

	public:

		/**
		 * Initializes the library interface.
		 * The windows mobile dynamic EGL dll will be loaded and all function pointers will be extracted.<br>
		 * Don't call any interface function before a successful initialization.
		 */
		static bool initialize();

		/**
		 * Releases the library interface and disposes all interface functions.
		 */
		static void release();

	protected:

		/**
		 * Initializes a function pointer.
		 * @param function The function to be initialized, must point to nullptr
		 * @param functionName The name of the function to initialize
		 * @tparam T The data type of the function pointer
		 */
		template <typename T>
		static void initializeFunction(T& function, const char* functionName);

	public:

		static glActiveTextureFunction glActiveTexture_;
		static glAttachShaderFunction glAttachShader_;
		static glBindBufferFunction glBindBuffer_;
		static glBindFramebufferFunction glBindFramebuffer_;
		static glBindVertexArrayFunction glBindVertexArray_;
		static glBufferDataFunction glBufferData_;
		static glCheckFramebufferStatusFunction glCheckFramebufferStatus_;
		static glClearDepthfFunction glClearDepthf_;
		static glCompileShaderFunction glCompileShader_;
		static glCompressedTexImage2DFunction glCompressedTexImage2D_;
		static glCreateProgramFunction glCreateProgram_;
		static glCreateShaderFunction glCreateShader_;
		static glDebugMessageCallbackFunction glDebugMessageCallback_;
		static glDeleteBuffersFunction glDeleteBuffers_;
		static glDeleteFramebuffersFunction glDeleteFramebuffers_;
		static glDeleteProgramFunction glDeleteProgram_;
		static glDeleteShaderFunction glDeleteShader_;
		static glDeleteTexturesFunction glDeleteTextures_;
		static glDeleteVertexArraysFunction glDeleteVertexArrays_;
		static glDetachShaderFunction glDetachShader_;
		static glDrawArraysFunction glDrawArrays_;
		static glDrawElementsFunction glDrawElements_;
		static glEnableVertexAttribArrayFunction glEnableVertexAttribArray_;
		static glFramebufferTexture2DFunction glFramebufferTexture2D_;
		static glGenBuffersFunction glGenBuffers_;
		static glGenerateMipmapFunction glGenerateMipmap_;
		static glGenFramebuffersFunction glGenFramebuffers_;
		static glGenTexturesFunction glGenTextures_;
		static glGenVertexArraysFunction glGenVertexArrays_;
		static glGetAttribLocationFunction glGetAttribLocation_;
		static glGetProgramInfoLogFunction glGetProgramInfoLog_;
		static glGetProgramivFunction glGetProgramiv_;
		static glGetShaderInfoLogFunction glGetShaderInfoLog_;
		static glGetShaderivFunction glGetShaderiv_;
		static glGetStringiFunction glGetStringi_;
		static glGetUniformLocationFunction glGetUniformLocation_;
		static glIsProgramFunction glIsProgram_;
		static glLinkProgramFunction glLinkProgram_;
		static glReleaseShaderCompilerFunction glReleaseShaderCompiler_;
		static glShaderSourceFunction glShaderSource_;
		static glTexImage2DMultisampleFunction glTexImage2DMultisample_;
		static glUniform1fFunction glUniform1f_;
		static glUniform1fvFunction glUniform1fv_;
		static glUniform1iFunction glUniform1i_;
		static glUniform1uiFunction glUniform1ui_;
		static glUniform2fFunction glUniform2f_;
		static glUniform4fvFunction glUniform2fv_;
		static glUniform3fFunction glUniform3f_;
		static glUniform4fvFunction glUniform3fv_;
		static glUniform4fFunction glUniform4f_;
		static glUniform4fvFunction glUniform4fv_;
		static glUniformMatrix3fvFunction glUniformMatrix3fv_;
		static glUniformMatrix4fvFunction glUniformMatrix4fv_;
		static glUseProgramFunction glUseProgram_;
		static glVertexAttribPointerFunction glVertexAttribPointer_;
		static glVertexAttribIPointerFunction glVertexAttribIPointer_;
};

}

}

}

#endif // OCEAN_RENDERING_GLES_USE_DYNAMIC_LIBRARY

#endif // META_OCEAN_RENDERING_GLES_DYNAMIC_LIBRARY_H
