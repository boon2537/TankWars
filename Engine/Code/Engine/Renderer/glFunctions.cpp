#include "Engine/Renderer/glFunctions.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

void BindGLFunctions()
{
	GL_BIND_FUNCTION(glClear);
	GL_BIND_FUNCTION(glClearColor); 
	GL_BIND_FUNCTION(glDeleteShader);
	GL_BIND_FUNCTION(glCreateShader);
	GL_BIND_FUNCTION(glShaderSource);
	GL_BIND_FUNCTION(glCompileShader);
	GL_BIND_FUNCTION(glGetShaderiv);
	GL_BIND_FUNCTION(glGetShaderInfoLog);
	GL_BIND_FUNCTION(glDetachShader);
	GL_BIND_FUNCTION(glDeleteBuffers);
	GL_BIND_FUNCTION(glGenBuffers);
	GL_BIND_FUNCTION(glBindBuffer);
	GL_BIND_FUNCTION(glBufferData);
	GL_BIND_FUNCTION(glCreateProgram);
	GL_BIND_FUNCTION(glAttachShader);
	GL_BIND_FUNCTION(glLinkProgram);
	GL_BIND_FUNCTION(glGetProgramiv);
	GL_BIND_FUNCTION(glDeleteProgram);
	GL_BIND_FUNCTION(glGetProgramInfoLog);
	GL_BIND_FUNCTION(glGenVertexArrays);
	GL_BIND_FUNCTION(glBindVertexArray);
	GL_BIND_FUNCTION(glGetAttribLocation);
	GL_BIND_FUNCTION(glEnableVertexAttribArray);
	GL_BIND_FUNCTION(glVertexAttribPointer);
	GL_BIND_FUNCTION(glUseProgram);
	GL_BIND_FUNCTION(glDrawArrays);
	GL_BIND_FUNCTION(glLineWidth);
	GL_BIND_FUNCTION(glEnable);
	GL_BIND_FUNCTION(glBlendFunc);
	GL_BIND_FUNCTION(glDisable);
	GL_BIND_FUNCTION(glBindTexture);
	GL_BIND_FUNCTION(glPixelStorei);
	GL_BIND_FUNCTION(glGenTextures);
	GL_BIND_FUNCTION(glTexParameteri);
	GL_BIND_FUNCTION(glTexImage2D);
	GL_BIND_FUNCTION(glGetUniformLocation);
	GL_BIND_FUNCTION(glUniformMatrix4fv);
	GL_BIND_FUNCTION(glGenSamplers);
	GL_BIND_FUNCTION(glSamplerParameteri);
	GL_BIND_FUNCTION(glDeleteSamplers);
	GL_BIND_FUNCTION(glBindSampler);
	GL_BIND_FUNCTION(glActiveTexture);
	GL_BIND_FUNCTION(glGetError);
	GL_BIND_FUNCTION(glGenFramebuffers);
	GL_BIND_FUNCTION(glDeleteFramebuffers);
	GL_BIND_FUNCTION(glBindFramebuffer);
	GL_BIND_FUNCTION(glFramebufferTexture);
	GL_BIND_FUNCTION(glDrawBuffers);
	GL_BIND_FUNCTION(glCheckFramebufferStatus);
	GL_BIND_FUNCTION(glBlitFramebuffer);
	GL_BIND_FUNCTION(glDepthFunc);
	GL_BIND_FUNCTION(glDepthMask);
	GL_BIND_FUNCTION(glClearDepthf);
	GL_BIND_FUNCTION(glGetTexImage);
	GL_BIND_FUNCTION(glGetIntegerv);
	GL_BIND_FUNCTION(glCopyTexImage2D);
	GL_BIND_FUNCTION(glFramebufferTexture2D);
	GL_BIND_FUNCTION(glUniform1f);
	GL_BIND_FUNCTION(glDrawElements);
	GL_BIND_FUNCTION(glPolygonMode);
	GL_BIND_FUNCTION(glUniform1fv);
	GL_BIND_FUNCTION(glUniform2fv);
	GL_BIND_FUNCTION(glUniform3fv);
	GL_BIND_FUNCTION(glUniform4fv);
	GL_BIND_FUNCTION(glCullFace);
	GL_BIND_FUNCTION(glFrontFace);
	GL_BIND_FUNCTION(glBlendFuncSeparate);
	GL_BIND_FUNCTION(glBlendEquationSeparate);
	GL_BIND_FUNCTION(glBindBufferBase);
	GL_BIND_FUNCTION(glDeleteTextures);
	GL_BIND_FUNCTION(glTexSubImage2D);
	GL_BIND_FUNCTION(glTexStorage2D);
	GL_BIND_FUNCTION(glViewport);
	GL_BIND_FUNCTION(glSamplerParameterfv);
	GL_BIND_FUNCTION(glGenerateMipmap);
	GL_BIND_FUNCTION(glSamplerParameterf);
}

void BindNewWGLFunctions()
{
	GL_BIND_FUNCTION(wglGetExtensionsStringARB); 
	GL_BIND_FUNCTION(wglChoosePixelFormatARB); 
	GL_BIND_FUNCTION(wglCreateContextAttribsARB);
}

PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

PFNGLCLEARPROC glClear = nullptr;
PFNGLCLEARCOLORPROC glClearColor = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLDETACHSHADERPROC glDetachShader = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLDRAWARRAYSPROC glDrawArrays = nullptr;
PFNGLLINEWIDTHPROC glLineWidth = nullptr;
PFNGLENABLEPROC glEnable = nullptr;
PFNGLBLENDFUNCPROC glBlendFunc = nullptr;
PFNGLDISABLEPROC glDisable = nullptr;
PFNGLBINDTEXTUREPROC glBindTexture = nullptr;
PFNGLPIXELSTOREIPROC glPixelStorei = nullptr;
PFNGLGENTEXTURESPROC glGenTextures = nullptr;
PFNGLTEXPARAMETERIPROC glTexParameteri = nullptr;
PFNGLTEXIMAGE2DPROC glTexImage2D = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
PFNGLGENSAMPLERSPROC glGenSamplers = nullptr;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri = nullptr;
PFNGLDELETESAMPLERSPROC glDeleteSamplers = nullptr;
PFNGLBINDSAMPLERPROC glBindSampler = nullptr;
PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
PFNGLGETERRORPROC glGetError = nullptr;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = nullptr;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC  glFramebufferTexture = nullptr;
PFNGLDRAWBUFFERSPROC glDrawBuffers = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = nullptr;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = nullptr;
PFNGLDEPTHFUNCPROC glDepthFunc = nullptr;
PFNGLDEPTHMASKPROC glDepthMask = nullptr;
PFNGLCLEARDEPTHFPROC glClearDepthf = nullptr;
PFNGLGETTEXIMAGEPROC glGetTexImage = nullptr;
PFNGLGETINTEGERVPROC glGetIntegerv = nullptr;
PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = nullptr;
PFNGLUNIFORM1FPROC glUniform1f = nullptr;
PFNGLDRAWELEMENTSPROC glDrawElements = nullptr;
PFNGLPOLYGONMODEPROC glPolygonMode = nullptr;
PFNGLUNIFORM1FVPROC glUniform1fv = nullptr;
PFNGLUNIFORM2FVPROC glUniform2fv = nullptr;
PFNGLUNIFORM3FVPROC glUniform3fv = nullptr;
PFNGLUNIFORM4FVPROC glUniform4fv = nullptr;
PFNGLCULLFACEPROC glCullFace = nullptr;
PFNGLFRONTFACEPROC glFrontFace = nullptr;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = nullptr;
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = nullptr;
PFNGLBINDBUFFERBASEPROC glBindBufferBase = nullptr;
PFNGLDELETETEXTURESPROC glDeleteTextures = nullptr;
PFNGLTEXTURESUBIMAGE2DPROC glTexSubImage2D = nullptr;
PFNGLTEXTURESTORAGE2DPROC glTexStorage2D = nullptr;
PFNGLVIEWPORTPROC glViewport = nullptr;
PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv = nullptr;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap  = nullptr;
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf = nullptr;

bool GLCheckError(char const *file, int line)
{
#if defined(_DEBUG)
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		ConsoleErrorf("GL ERROR [0x%04x] at [%s(%i)]", error, file, line );
		DebuggerPrintf("GL ERROR [0x%04x] at [%s(%i)]", error, file, line );
		return true; 
	}
#endif
	return false; 
}
