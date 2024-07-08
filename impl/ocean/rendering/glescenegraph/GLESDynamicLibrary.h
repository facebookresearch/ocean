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

		typedef void (__stdcall *glActiveTextureFunction)(GLenum texture);
		typedef void (__stdcall *glAttachShaderFunction)(GLuint, GLuint);
		typedef void (__stdcall *glBindBufferFunction)(GLenum, GLuint);
		typedef void (__stdcall *glBindFramebufferFunction)(GLenum target, GLuint framebuffer);
		typedef void (__stdcall *glBindVertexArrayFunction)(GLuint  array);
		typedef void (__stdcall *glBufferDataFunction)(GLenum, GLsizeiptr, const void*, GLenum);
		typedef GLenum (__stdcall *glCheckFramebufferStatusFunction)(GLenum target);
		typedef void (__stdcall *glClearDepthfFunction)(GLclampf);
		typedef void (__stdcall *glCompileShaderFunction)(GLuint);
		typedef void (__stdcall *glCompressedTexImage2DFunction)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void*);
		typedef GLuint (__stdcall *glCreateProgramFunction)();
		typedef GLuint (__stdcall *glCreateShaderFunction)(GLenum type);
		typedef void (__stdcall *glDeleteBuffersFunction)(GLsizei, const GLuint*);
		typedef void (__stdcall *glDeleteFramebuffersFunction)(GLsizei n, const GLuint * framebuffers);
		typedef void (__stdcall *glDeleteProgramFunction)(GLuint);
		typedef void (__stdcall *glDeleteShaderFunction)(GLuint);
		typedef void (__stdcall *glDeleteTexturesFunction)(GLsizei, const GLuint*);
		typedef void (__stdcall *glDeleteVertexArraysFunction)(GLsizei, const GLuint*);
		typedef void (__stdcall *glDetachShaderFunction)(GLuint, GLuint);
		typedef void (__stdcall *glDrawArraysFunction)(GLenum, GLint, GLsizei);
		typedef void (__stdcall *glDrawElementsFunction)(GLenum, GLsizei, GLenum, const void*);
		typedef void (__stdcall *glEnableVertexAttribArrayFunction)(GLuint index);
		typedef void (__stdcall *glFramebufferTexture2DFunction)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
		typedef void (__stdcall *glGenBuffersFunction)(GLsizei, GLuint*);
		typedef void (__stdcall *glGenerateMipmapFunction)(GLenum target);
		typedef void (__stdcall *glGenFramebuffersFunction)(GLsizei n, GLuint *ids);
		typedef void (__stdcall *glGenTexturesFunction)(GLsizei, GLuint*);
		typedef void (__stdcall *glGenVertexArraysFunction)(GLsizei n, GLuint *arrays);
		typedef int (__stdcall *glGetAttribLocationFunction)(GLuint, const char*);
		typedef void (__stdcall *glGetProgramInfoLogFunction)(GLuint, GLsizei, GLsizei*, char*);
		typedef void (__stdcall *glGetProgramivFunction)(GLuint, GLenum, GLint*);
		typedef void (__stdcall *glGetShaderInfoLogFunction)(GLuint, GLsizei, GLsizei*, char*);
		typedef void (__stdcall *glGetShaderivFunction)(GLuint, GLenum, GLint*);
		typedef GLubyte* (__stdcall *glGetStringiFunction)(GLenum, GLuint);
		typedef int (__stdcall *glGetUniformLocationFunction)(GLuint, const char*);
		typedef GLboolean (__stdcall *glIsProgramFunction)(GLuint program);
		typedef void (__stdcall *glLinkProgramFunction)(GLuint);
		typedef void (__stdcall *glReleaseShaderCompilerFunction)();
		typedef void (__stdcall *glShaderSourceFunction)(GLuint, GLsizei, const char**, const GLint*);
		typedef void (__stdcall *glTexImage2DMultisampleFunction)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
		typedef void (__stdcall *glUniform1fFunction)(GLint location, GLfloat x);
		typedef void (__stdcall *glUniform1fvFunction)(GLint, GLsizei, const GLfloat*);
		typedef void (__stdcall *glUniform1iFunction)(GLint, GLint);
		typedef void (__stdcall *glUniform1uiFunction)(GLint, GLuint);
		typedef void (__stdcall *glUniform2fFunction)(GLint, GLfloat, GLfloat);
		typedef void (__stdcall *glUniform2fvFunction)(GLint location, GLsizei count, const GLfloat* v);
		typedef void (__stdcall *glUniform3fFunction)(GLint, GLfloat, GLfloat, GLfloat);
		typedef void (__stdcall *glUniform3fvFunction)(GLint location, GLsizei count, const GLfloat* v);
		typedef void (__stdcall *glUniform4fFunction)(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		typedef void (__stdcall *glUniform4fvFunction)(GLint location, GLsizei count, const GLfloat* v);
		typedef void (__stdcall *glUniformMatrix3fvFunction)(GLint, GLsizei, GLboolean, const GLfloat*);
		typedef void (__stdcall *glUniformMatrix4fvFunction)(GLint, GLsizei, GLboolean, const GLfloat*);
		typedef void (__stdcall *glUseProgramFunction)(GLuint);
		typedef void (__stdcall *glVertexAttribPointerFunction)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
		typedef void (__stdcall *glVertexAttribIPointerFunction)(GLuint, GLint, GLenum, GLsizei, const void*);

	public:

		/**
		 * Initializes the library interface.
		 * The windows mobile dynamic egl dll will be loaded and all function pointers will be extracted.<br>
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
