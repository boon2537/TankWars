#pragma once

#include "Engine/ThirdParty/gl/glcorearb.h"
#include "Engine/ThirdParty/gl/glext.h"
#include "Engine/ThirdParty/gl/wglext.h"
#include "Engine/Renderer/Renderer.hpp"

#define GL_CHECK_ERROR() GLCheckError(__FILE__, __LINE__)

#define GL_BIND_FUNCTION(f) wglGetTypedProcAddress(&f, #f)

// Use this to deduce type of the pointer so we can cast; 
template <typename T>
bool wglGetTypedProcAddress( T *out, char const *name ) 
{
	// Grab the function from the currently bound render context
	// most opengl 2.0+ features will be found here
	*out = (T) wglGetProcAddress(name); 

	if ((*out) == nullptr) {
		// if it is not part of wgl (the device), then attempt to get it from the GLL library
		// (most OpenGL functions come from here)
		*out = (T) GetProcAddress(Renderer::gGLLibrary, name); 
	}

	return (*out != nullptr); 
}

void BindGLFunctions();
void BindNewWGLFunctions();

extern PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

extern PFNGLCLEARPROC glClear;
extern PFNGLCLEARCOLORPROC glClearColor;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLDETACHSHADERPROC glDetachShader;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLGETSHADERINFOLOGPROC glGetProgramInfoLog;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLDRAWARRAYSPROC glDrawArrays;
extern PFNGLLINEWIDTHPROC glLineWidth;
extern PFNGLENABLEPROC glEnable;
extern PFNGLBLENDFUNCPROC glBlendFunc;
extern PFNGLDISABLEPROC glDisable;
extern PFNGLBINDTEXTUREPROC glBindTexture;
extern PFNGLPIXELSTOREIPROC glPixelStorei;
extern PFNGLGENTEXTURESPROC glGenTextures;
extern PFNGLTEXPARAMETERIPROC glTexParameteri;
extern PFNGLTEXIMAGE2DPROC glTexImage2D;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLGENSAMPLERSPROC glGenSamplers;
extern PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
extern PFNGLDELETESAMPLERSPROC glDeleteSamplers;
extern PFNGLBINDSAMPLERPROC glBindSampler;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLGETERRORPROC glGetError;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC  glFramebufferTexture;
extern PFNGLDRAWBUFFERSPROC glDrawBuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
extern PFNGLDEPTHFUNCPROC glDepthFunc;
extern PFNGLDEPTHMASKPROC glDepthMask;
extern PFNGLCLEARDEPTHFPROC glClearDepthf;
extern PFNGLGETTEXIMAGEPROC glGetTexImage;
extern PFNGLGETINTEGERVPROC glGetIntegerv;
extern PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLDRAWELEMENTSPROC glDrawElements;
extern PFNGLPOLYGONMODEPROC glPolygonMode;
extern PFNGLUNIFORM1FVPROC glUniform1fv;
extern PFNGLUNIFORM2FVPROC glUniform2fv;
extern PFNGLUNIFORM3FVPROC glUniform3fv;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLCULLFACEPROC glCullFace;
extern PFNGLFRONTFACEPROC glFrontFace; 
extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
extern PFNGLBINDBUFFERBASEPROC glBindBufferBase;
extern PFNGLDELETETEXTURESPROC glDeleteTextures;
extern PFNGLTEXTURESUBIMAGE2DPROC glTexSubImage2D;
extern PFNGLTEXTURESTORAGE2DPROC glTexStorage2D;
extern PFNGLVIEWPORTPROC glViewport;
extern PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
extern PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;

bool GLCheckError(char const *file, int line);
