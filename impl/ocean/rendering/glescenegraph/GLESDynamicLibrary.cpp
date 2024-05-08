/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESDynamicLibrary.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

#ifdef OCEAN_RENDERING_GLES_USE_DYNAMIC_LIBRARY

GLESDynamicLibrary::glActiveTextureFunction GLESDynamicLibrary::glActiveTexture_ = nullptr;
GLESDynamicLibrary::glAttachShaderFunction GLESDynamicLibrary::glAttachShader_ = nullptr;
GLESDynamicLibrary::glBindBufferFunction GLESDynamicLibrary::glBindBuffer_ = nullptr;
GLESDynamicLibrary::glBindFramebufferFunction GLESDynamicLibrary::glBindFramebuffer_ = nullptr;
GLESDynamicLibrary::glBindVertexArrayFunction GLESDynamicLibrary::glBindVertexArray_ = nullptr;
GLESDynamicLibrary::glBufferDataFunction GLESDynamicLibrary::glBufferData_ = nullptr;
GLESDynamicLibrary::glCheckFramebufferStatusFunction GLESDynamicLibrary::glCheckFramebufferStatus_ = nullptr;
GLESDynamicLibrary::glClearDepthfFunction GLESDynamicLibrary::glClearDepthf_ = nullptr;
GLESDynamicLibrary::glCompileShaderFunction GLESDynamicLibrary::glCompileShader_ = nullptr;
GLESDynamicLibrary::glCompressedTexImage2DFunction GLESDynamicLibrary::glCompressedTexImage2D_ = nullptr;
GLESDynamicLibrary::glCreateProgramFunction GLESDynamicLibrary::glCreateProgram_ = nullptr;
GLESDynamicLibrary::glCreateShaderFunction GLESDynamicLibrary::glCreateShader_ = nullptr;
GLESDynamicLibrary::glDeleteBuffersFunction GLESDynamicLibrary::glDeleteBuffers_ = nullptr;
GLESDynamicLibrary::glDeleteFramebuffersFunction GLESDynamicLibrary::glDeleteFramebuffers_ = nullptr;
GLESDynamicLibrary::glDeleteProgramFunction GLESDynamicLibrary::glDeleteProgram_ = nullptr;
GLESDynamicLibrary::glDeleteShaderFunction GLESDynamicLibrary::glDeleteShader_ = nullptr;
GLESDynamicLibrary::glDeleteTexturesFunction GLESDynamicLibrary::glDeleteTextures_ = nullptr;
GLESDynamicLibrary::glDeleteVertexArraysFunction GLESDynamicLibrary::glDeleteVertexArrays_ = nullptr;
GLESDynamicLibrary::glDetachShaderFunction GLESDynamicLibrary::glDetachShader_ = nullptr;
GLESDynamicLibrary::glDrawArraysFunction GLESDynamicLibrary::glDrawArrays_ = nullptr;
GLESDynamicLibrary::glDrawElementsFunction GLESDynamicLibrary::glDrawElements_ = nullptr;
GLESDynamicLibrary::glEnableVertexAttribArrayFunction GLESDynamicLibrary::glEnableVertexAttribArray_ = nullptr;
GLESDynamicLibrary::glFramebufferTexture2DFunction GLESDynamicLibrary::glFramebufferTexture2D_ = nullptr;
GLESDynamicLibrary::glGenBuffersFunction GLESDynamicLibrary::glGenBuffers_ = nullptr;
GLESDynamicLibrary::glGenerateMipmapFunction GLESDynamicLibrary::glGenerateMipmap_ = nullptr;
GLESDynamicLibrary::glGenFramebuffersFunction GLESDynamicLibrary::glGenFramebuffers_ = nullptr;
GLESDynamicLibrary::glGenTexturesFunction GLESDynamicLibrary::glGenTextures_ = nullptr;
GLESDynamicLibrary::glGenVertexArraysFunction GLESDynamicLibrary::glGenVertexArrays_ = nullptr;
GLESDynamicLibrary::glGetAttribLocationFunction GLESDynamicLibrary::glGetAttribLocation_ = nullptr;
GLESDynamicLibrary::glGetProgramInfoLogFunction GLESDynamicLibrary::glGetProgramInfoLog_ = nullptr;
GLESDynamicLibrary::glGetProgramivFunction GLESDynamicLibrary::glGetProgramiv_ = nullptr;
GLESDynamicLibrary::glGetShaderInfoLogFunction GLESDynamicLibrary::glGetShaderInfoLog_ = nullptr;
GLESDynamicLibrary::glGetShaderivFunction GLESDynamicLibrary::glGetShaderiv_ = nullptr;
GLESDynamicLibrary::glGetStringiFunction GLESDynamicLibrary::glGetStringi_ = nullptr;
GLESDynamicLibrary::glGetUniformLocationFunction GLESDynamicLibrary::glGetUniformLocation_ = nullptr;
GLESDynamicLibrary::glIsProgramFunction GLESDynamicLibrary::glIsProgram_ = nullptr;
GLESDynamicLibrary::glLinkProgramFunction GLESDynamicLibrary::glLinkProgram_ = nullptr;
GLESDynamicLibrary::glReleaseShaderCompilerFunction GLESDynamicLibrary::glReleaseShaderCompiler_ = nullptr;
GLESDynamicLibrary::glShaderSourceFunction GLESDynamicLibrary::glShaderSource_ = nullptr;
GLESDynamicLibrary::glTexImage2DMultisampleFunction GLESDynamicLibrary::glTexImage2DMultisample_ = nullptr;
GLESDynamicLibrary::glUniform1fFunction GLESDynamicLibrary::glUniform1f_ = nullptr;
GLESDynamicLibrary::glUniform1fvFunction GLESDynamicLibrary::glUniform1fv_ = nullptr;
GLESDynamicLibrary::glUniform1iFunction GLESDynamicLibrary::glUniform1i_ = nullptr;
GLESDynamicLibrary::glUniform1uiFunction GLESDynamicLibrary::glUniform1ui_ = nullptr;
GLESDynamicLibrary::glUniform2fFunction GLESDynamicLibrary::glUniform2f_ = nullptr;
GLESDynamicLibrary::glUniform2fvFunction GLESDynamicLibrary::glUniform2fv_ = nullptr;
GLESDynamicLibrary::glUniform3fFunction GLESDynamicLibrary::glUniform3f_ = nullptr;
GLESDynamicLibrary::glUniform3fvFunction GLESDynamicLibrary::glUniform3fv_ = nullptr;
GLESDynamicLibrary::glUniform4fFunction GLESDynamicLibrary::glUniform4f_ = nullptr;
GLESDynamicLibrary::glUniform4fvFunction GLESDynamicLibrary::glUniform4fv_ = nullptr;
GLESDynamicLibrary::glUniformMatrix3fvFunction GLESDynamicLibrary::glUniformMatrix3fv_ = nullptr;
GLESDynamicLibrary::glUniformMatrix4fvFunction GLESDynamicLibrary::glUniformMatrix4fv_ = nullptr;
GLESDynamicLibrary::glUseProgramFunction GLESDynamicLibrary::glUseProgram_ = nullptr;
GLESDynamicLibrary::glVertexAttribPointerFunction GLESDynamicLibrary::glVertexAttribPointer_ = nullptr;
GLESDynamicLibrary::glVertexAttribIPointerFunction GLESDynamicLibrary::glVertexAttribIPointer_ = nullptr;

bool GLESDynamicLibrary::initialize()
{
	if (glActiveTexture_ != nullptr)
	{
		return true;
	}

	initializeFunction(glActiveTexture_, "glActiveTexture");
	initializeFunction(glAttachShader_, "glAttachShader");
	initializeFunction(glBindBuffer_, "glBindBuffer");
	initializeFunction(glBindFramebuffer_, "glBindFramebuffer");
	initializeFunction(glBindVertexArray_, "glBindVertexArray");
	initializeFunction(glBufferData_, "glBufferData");
	initializeFunction(glCheckFramebufferStatus_, "glCheckFramebufferStatus");
	initializeFunction(glClearDepthf_, "glClearDepthf");
	initializeFunction(glCompileShader_, "glCompileShader");
	initializeFunction(glCompressedTexImage2D_, "glCompressedTexImage2D");
	initializeFunction(glCreateProgram_, "glCreateProgram");
	initializeFunction(glCreateShader_, "glCreateShader");
	initializeFunction(glDeleteBuffers_, "glDeleteBuffers");
	initializeFunction(glDeleteFramebuffers_, "glDeleteFramebuffers");
	initializeFunction(glDeleteProgram_, "glDeleteProgram");
	initializeFunction(glDeleteShader_, "glDeleteShader");
	initializeFunction(glDeleteTextures_, "glDeleteTextures");
	initializeFunction(glDeleteVertexArrays_, "glDeleteVertexArrays");
	initializeFunction(glDetachShader_, "glDetachShader");
	initializeFunction(glDrawArrays_, "glDrawArrays");
	initializeFunction(glDrawElements_, "glDrawElements");
	initializeFunction(glEnableVertexAttribArray_, "glEnableVertexAttribArray");
	initializeFunction(glFramebufferTexture2D_, "glFramebufferTexture2D");
	initializeFunction(glGenBuffers_, "glGenBuffers");
	initializeFunction(glGenerateMipmap_, "glGenerateMipmap");
	initializeFunction(glGenFramebuffers_, "glGenFramebuffers");
	initializeFunction(glGenTextures_, "glGenTextures");
	initializeFunction(glGenVertexArrays_, "glGenVertexArrays");
	initializeFunction(glGetAttribLocation_, "glGetAttribLocation");
	initializeFunction(glGetProgramInfoLog_, "glGetProgramInfoLog");
	initializeFunction(glGetProgramiv_, "glGetProgramiv");
	initializeFunction(glGetShaderInfoLog_, "glGetShaderInfoLog");
	initializeFunction(glGetShaderiv_, "glGetShaderiv");
	initializeFunction(glGetStringi_, "glGetStringi");
	initializeFunction(glGetUniformLocation_, "glGetUniformLocation");
	initializeFunction(glIsProgram_, "glIsProgram");
	initializeFunction(glLinkProgram_, "glLinkProgram");
	initializeFunction(glReleaseShaderCompiler_, "glReleaseShaderCompiler");
	initializeFunction(glShaderSource_, "glShaderSource");
	initializeFunction(glTexImage2DMultisample_ , "glTexImage2DMultisample");
	initializeFunction(glUniform1f_, "glUniform1f");
	initializeFunction(glUniform1fv_, "glUniform1fv");
	initializeFunction(glUniform1i_, "glUniform1i");
	initializeFunction(glUniform1ui_, "glUniform1ui");
	initializeFunction(glUniform2f_, "glUniform2f");
	initializeFunction(glUniform2fv_, "glUniform2fv");
	initializeFunction(glUniform3f_, "glUniform3f");
	initializeFunction(glUniform3fv_, "glUniform3fv");
	initializeFunction(glUniform4f_, "glUniform4f");
	initializeFunction(glUniform4fv_, "glUniform4fv");
	initializeFunction(glUniformMatrix3fv_, "glUniformMatrix3fv");
	initializeFunction(glUniformMatrix4fv_, "glUniformMatrix4fv");
	initializeFunction(glUseProgram_, "glUseProgram");
	initializeFunction(glVertexAttribPointer_, "glVertexAttribPointer");
	initializeFunction(glVertexAttribIPointer_, "glVertexAttribIPointer");

	return glActiveTexture_ != nullptr;
}

template <typename T>
void GLESDynamicLibrary::initializeFunction(T& function, const char* functionName)
{
	ocean_assert(functionName != nullptr);

	ocean_assert(function == nullptr);

#ifdef _WINDOWS
	function = T(wglGetProcAddress(functionName));
#endif

	ocean_assert(function != nullptr);
}

void GLESDynamicLibrary::release()
{
	glActiveTexture_ = nullptr;
	glAttachShader_ = nullptr;
	glBindBuffer_ = nullptr;
	glBindFramebuffer_ = nullptr;
	glBindVertexArray_ = nullptr;
	glBufferData_ = nullptr;
	glCheckFramebufferStatus_ = nullptr;
	glClearDepthf_ = nullptr;
	glCompileShader_ = nullptr;
	glCompressedTexImage2D_ = nullptr;
	glCreateProgram_ = nullptr;
	glCreateShader_ = nullptr;
	glDeleteBuffers_ = nullptr;
	glDeleteFramebuffers_ = nullptr;
	glDeleteProgram_ = nullptr;
	glDeleteShader_ = nullptr;
	glDeleteTextures_ = nullptr;
	glDeleteVertexArrays_ = nullptr;
	glDetachShader_ = nullptr;
	glDrawArrays_ = nullptr;
	glDrawElements_ = nullptr;
	glEnableVertexAttribArray_ = nullptr;
	glFramebufferTexture2D_ = nullptr;
	glGenBuffers_ = nullptr;
	glGenerateMipmap_ = nullptr;
	glGenFramebuffers_ = nullptr;
	glGenTextures_ = nullptr;
	glGenVertexArrays_ = nullptr;
	glGetAttribLocation_ = nullptr;
	glGetProgramInfoLog_ = nullptr;
	glGetProgramiv_ = nullptr;
	glGetShaderInfoLog_ = nullptr;
	glGetShaderiv_ = nullptr;
	glGetStringi_ = nullptr;
	glGetUniformLocation_ = nullptr;
	glIsProgram_ = nullptr;
	glLinkProgram_ = nullptr;
	glReleaseShaderCompiler_ = nullptr;
	glShaderSource_ = nullptr;
	glTexImage2DMultisample_ = nullptr;
	glUniform1f_ = nullptr;
	glUniform1fv_ = nullptr;
	glUniform1i_ = nullptr;
	glUniform1ui_ = nullptr;
	glUniform2f_ = nullptr;
	glUniform2fv_ = nullptr;
	glUniform3f_ = nullptr;
	glUniform3fv_ = nullptr;
	glUniform4f_ = nullptr;
	glUniform4fv_ = nullptr;
	glUniformMatrix3fv_ = nullptr;
	glUniformMatrix4fv_ = nullptr;
	glUseProgram_ = nullptr;
	glVertexAttribPointer_ = nullptr;
	glVertexAttribIPointer_ = nullptr;
}

#endif // OCEAN_RENDERING_GLES_USE_DYNAMIC_LIBRARY

}

}

}
