#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/glFunctions.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/File/File.hpp"

#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

ShaderProgram::ShaderProgram()
{

}

ShaderProgram::~ShaderProgram()
{
	free(m_defines);
}

bool ShaderProgram::LoadFromFiles(const std::string& root, const char* defines)
{
	std::string vs_file = root;
	vs_file += ".vs"; 

	std::string fs_file = root; 
	fs_file += ".fs"; 

	return LoadFromFiles(vs_file, fs_file, defines);
}

bool ShaderProgram::LoadFromFiles(const std::string& vs_path, const std::string& fs_path, const char * defines)
{
	UpdateDefines(defines);

	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vert_shader = LoadShader(vs_path.c_str(), GL_VERTEX_SHADER, defines); 
	GLuint frag_shader = LoadShader(fs_path.c_str(), GL_FRAGMENT_SHADER, defines); 

	// Link the program
	// program_handle is a member GLuint. 
	m_programHandle = CreateAndLinkProgram(vert_shader, frag_shader); 
	glDeleteShader(vert_shader); 
	glDeleteShader(frag_shader); 

	return (m_programHandle != NULL && vert_shader != NULL && frag_shader != NULL); 
}

bool ShaderProgram::LoadFromRaw(char* vertexShader, char* fragmentShader, const char* defines)
{
	UpdateDefines(defines);

	GLuint vert_shader = LoadShaderFromRaw(vertexShader, GL_VERTEX_SHADER, defines); 
	GLuint frag_shader = LoadShaderFromRaw(fragmentShader, GL_FRAGMENT_SHADER, defines); 

	m_programHandle = CreateAndLinkProgram(vert_shader, frag_shader); 
	glDeleteShader(vert_shader); 
	glDeleteShader(frag_shader); 

	return (m_programHandle != NULL); 
}

void ShaderProgram::UpdateDefines(const char* defines)
{
	if (defines != nullptr)
	{
		m_defines = new char[strlen(defines) + 1];
		memcpy(m_defines, defines, strlen(defines) + 1);
	}
}

GLuint ShaderProgram::LoadShader(const char* filename, GLenum type, const char* defines)
{
	char* buffer = (char*) FileReadToNewBuffer(filename);
	ASSERT_RECOVERABLE(buffer != nullptr, "reading file failed");

	// Create a shader
	GLuint shader_id = glCreateShader(type);
	ASSERT_RECOVERABLE(shader_id != NULL, "cannot find shader_id");

	char* injectedBuffer = InjectPreprocessorDefines(buffer, defines);
	if (injectedBuffer != buffer)
		free(buffer);

	// Bind source to it, and compile
	// You can add multiple strings to a shader, they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(injectedBuffer);
	glShaderSource(shader_id, 1, &injectedBuffer, &shader_length);
	glCompileShader(shader_id);

	//for accurate line in error reporting
	__int64 errorLogOffset = 0;
	if (defines != nullptr)
	{
		std::string temp = std::string(defines);
		errorLogOffset = std::count(temp.begin(), temp.end(), ';') + 1;
	}

	// Check status
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) 
	{
		LogShaderError(shader_id, filename, errorLogOffset); // function we write
		glDeleteShader(shader_id);
		shader_id = NULL;
	}

	free(injectedBuffer);

	return shader_id;
}

GLuint ShaderProgram::LoadShaderFromRaw(char* text, GLenum type, const char* defines)
{
	// Create a shader
	GLuint shader_id = glCreateShader(type);
	ASSERT_RECOVERABLE(shader_id != NULL, "cannot find shader_id");

	char* injectedBuffer = InjectPreprocessorDefines(text, defines);

	// Bind source to it, and compile
	// You can add multiple strings to a shader, they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(injectedBuffer);
	glShaderSource(shader_id, 1, &injectedBuffer, &shader_length);
	glCompileShader(shader_id);

	//for accurate line in error reporting
	__int64 errorLogOffset = 0;
	if (defines != nullptr)
	{
		std::string temp = std::string(defines);
		errorLogOffset = std::count(temp.begin(), temp.end(), ';') + 1;
	}

	// Check status
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) 
	{
		LogShaderError(shader_id, nullptr, errorLogOffset); // function we write
		glDeleteShader(shader_id);
		shader_id = NULL;
	}

	if (defines != nullptr)
		free(injectedBuffer);

	return shader_id;
}

char* ShaderProgram::InjectPreprocessorDefines(char* text, const char* defines)
{
	if (defines == nullptr)
		return text;

	Strings tokens = Split(std::string(defines), ';');
	for (unsigned int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] != "")
		{
			tokens[i].insert(0, "#define ");
			tokens[i] = ReplaceAllStrings(tokens[i], "=", " ");

			if (i == 0)
				tokens[i].insert(0, "\n");
			if (i < tokens.size() - 1)
				tokens[i].append("\n");
		}
	}

	std::string newText = std::string(text);
	//assume we always have #version followed by \n.
	size_t spot = newText.find("#version");
	spot = newText.find('\n', spot);
	newText.insert(spot, MergeStrings(tokens));

	char* output = new char[newText.length() + 1];
	memcpy(output, newText.c_str(), newText.length() + 1);
	return output;
}

void ShaderProgram::LogShaderError(GLuint shader_id, const char* path, __int64 errorLogOffset)
{
	// figure out how large the buffer needs to be
	GLint length;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

	// make a buffer, and copy the log to it. 
	char *buffer = new char[length + 1];
	glGetShaderInfoLog(shader_id, length, &length, buffer);

	// Print it out (may want to do some additional formatting)
	buffer[length] = NULL;

	if (path == nullptr)
	{
		DebuggerPrintf("Shader error log: %s", buffer);
	}
	else
	{
		//better error reporting
		//found the trick from https://stackoverflow.com/questions/25150708/how-to-get-the-projectdir-path-in-a-visual-studio-native-c-unit-testing-usi
		std::string dir = EXPAND(SOLUTION_DIR);
		dir.erase(0, 1); // erase the first quote
		dir.erase(dir.end() - 2); // erase the last quote and the dot
		dir.erase(dir.end() - 1); // erase the last quote and the dot

		std::string dirToFile = dir + std::string("Run_Win32\\");
		dirToFile = ReplaceAllStrings(dirToFile, std::string("\\"), std::string("/"));
		dirToFile = dirToFile + std::string(path); //assume we use forward slash

		Strings shaderErrors = Split(std::string(buffer).erase(length - 1, 1), '\n');
		int lineNumber = 0;
		std::string errorLog = "";

		for (unsigned int i = 0; i < shaderErrors.size(); i++)
		{
			if (shaderErrors[i].length() == 0) continue;

			// running on Nvidia's graphic card?
			if (shaderErrors[i].find('(') ==  1)
			{
				size_t startLook = shaderErrors[i].find('(');
				size_t endLook = shaderErrors[i].find(')');
				std::string numberText = std::string(shaderErrors[i], startLook + 1, endLook - startLook - 1);
				lineNumber = atoi(numberText.c_str());

				size_t startComment = shaderErrors[i].find(':');
				errorLog = std::string(shaderErrors[i], startComment + 2);
			}
			// running on Intel's graphic card?
			else
			{
				Strings tokens = Split(shaderErrors[i], ' ');
				size_t startLook = tokens[1].find(':');
				size_t endLook = tokens[1].find(':', 2);
				std::string numberText = std::string(tokens[1], startLook + 1, endLook - startLook - 1);
				lineNumber = atoi(numberText.c_str());
				
				std::string temp = "";
				for (unsigned int j = 2; j < tokens.size(); j++)
				{
					temp += tokens[j] + ' ';
				}
				errorLog = std::string(temp);
			}

			lineNumber = MaxInt(lineNumber - (int) errorLogOffset, 0);

			DebuggerPrintf( "%s(%d): %s\n", dirToFile.c_str(), lineNumber, errorLog.c_str());
		}
	}

	ERROR_RECOVERABLE("Shader error encountered, check the log!");

	// free up the memory we used. 
	delete buffer;
}

GLuint ShaderProgram::CreateAndLinkProgram(GLint vs, GLint fs)
{
	// create the program handle - how you will reference
	// this program within OpenGL, like a texture handle
	GLuint program_id = glCreateProgram();
	ASSERT_RECOVERABLE(program_id != 0, "create progeam failed");

	// Attach the shaders you want to use
	glAttachShader( program_id, vs );
	glAttachShader( program_id, fs );

	// Link the program (create the GPU program)
	glLinkProgram( program_id );

	// Check for link errors - usually a result
	// of incompatibility between stages.
	GLint link_status;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

	if (link_status == GL_FALSE) {
		LogProgramError(program_id);
		glDeleteProgram(program_id);
		program_id = 0;
	} 

	// no longer need the shaders, you can detach them if you want
	// (not necessary)
	glDetachShader( program_id, vs );
	glDetachShader( program_id, fs );

	return program_id;
}

void ShaderProgram::LogProgramError(GLuint program_id)
{
	// get the buffer length
	GLint length;
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);

	// copy the log into a new buffer
	char *buffer = new char[length + 1];
	glGetProgramInfoLog(program_id, length, &length, buffer);

	// print it to the output pane
	buffer[length] = NULL;
	DebuggerPrintf("Shader program error log: %s", buffer);
	ERROR_RECOVERABLE("Failed to build shader program!");

	// cleanup
	delete buffer;
}
