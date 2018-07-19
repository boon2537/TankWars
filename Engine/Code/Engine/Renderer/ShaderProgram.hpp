#pragma once

#include "Engine/ThirdParty/gl/glcorearb.h"
#include "Engine/ThirdParty/gl/glext.h"
#include "Engine/ThirdParty/gl/wglext.h"
#include <string>

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	bool LoadFromFiles(const std::string& root, const char* defines = nullptr); // load a shader from file
	bool LoadFromFiles(const std::string& vs_path, const std::string& fs_path, const char* defines = nullptr);
	bool LoadFromRaw(char* vertexShader, char* fragmentShader, const char* defines = nullptr);
	void UpdateDefines(const char* defines);
	
	static GLuint LoadShader(const char* filename, GLenum type, const char* defines = nullptr);
	static GLuint LoadShaderFromRaw(char* text, GLenum type, const char* defines = nullptr);
	static char* InjectPreprocessorDefines(char* text, const char* defines = nullptr);
	static void LogShaderError(GLuint shader_id, const char* path, __int64 errorLogOffset = 0);
	static GLuint CreateAndLinkProgram(GLint vs, GLint fs);
	static void LogProgramError(GLuint program_id);

public:
	std::string m_id = "";
	GLuint m_programHandle; // OpenGL handle for this program, default 0
	char* m_defines = nullptr;
};
