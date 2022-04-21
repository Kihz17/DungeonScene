#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

const unsigned int MAX_LINE_LENGTH = 65536;		// 16x1024

namespace ShaderUtils
{
	static std::vector<std::string> LoadSourceFromFile(const std::string& filepath)
	{
		std::vector<std::string> source;
		std::ifstream theFile(filepath.c_str());
		if (!theFile.is_open())
		{
			return source;
		}

		char pLineTemp[MAX_LINE_LENGTH] = { 0 };
		while (theFile.getline(pLineTemp, MAX_LINE_LENGTH))
		{
			std::string tempString(pLineTemp);
			source.push_back(tempString);
		}

		theFile.close();
		return source;
	}

	static bool WasThereACompileError(const GLuint& shaderID, const std::string& filePath)
	{
		GLint isCompiled = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

			char* pLogText = new char[maxLength];
			glGetShaderInfoLog(shaderID, maxLength, &maxLength, pLogText);

			std::stringstream ss;
			ss << filePath << ": Compile Error: " << pLogText;

			std::cout << ss.str() << std::endl;

			delete[] pLogText;

			return true;
		}

		return false;
	}

	static bool CompileShaderFromSource(const GLuint& shaderID, const std::string& filePath, const std::vector<std::string>& source)
	{
		const unsigned int MAXLINESIZE = 8 * 1024;	// About 8K PER LINE, which seems excessive
		unsigned int numberOfLines = static_cast<unsigned int>(source.size());

		// This is an array of pointers to strings. aka the lines of source
		char** arraySource = new char* [numberOfLines];

		// Clear array to all zeros (i.e. '\0' or null terminator)
		memset(arraySource, 0, numberOfLines);

		for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++)
		{
			unsigned int numCharacters = (unsigned int)source[indexLine].length();
			// Create an array of chars for each line
			arraySource[indexLine] = new char[numCharacters + 2];		// For the '\n' and '\0' at end
			memset(arraySource[indexLine], 0, numCharacters + 2);

			// Copy line of source into array
			for (unsigned int indexChar = 0; indexChar != source[indexLine].length(); indexChar++)
			{
				arraySource[indexLine][indexChar] = source[indexLine][indexChar];
			}

			// At a '\0' at end (just in case)
			arraySource[indexLine][numCharacters + 0] = '\n';
			arraySource[indexLine][numCharacters + 1] = '\0';
		}

		glShaderSource(shaderID, numberOfLines, arraySource, NULL);
		glCompileShader(shaderID);

		// Get rid of the temp source "c" style array
		for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++)
		{
			delete[] arraySource[indexLine];
		}

		delete[] arraySource;

		return !WasThereACompileError(shaderID, filePath);
	}

	static bool WasThereALinkError(const GLuint& programID)
	{
		GLint wasError = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, &wasError);
		if (wasError == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

			char* pLogText = new char[maxLength];
			glGetProgramInfoLog(programID, maxLength, &maxLength, pLogText);

			std::stringstream ss;
			ss << "Shader Link Error: " << pLogText;

			std::cout << ss.str() << std::endl;

			delete[] pLogText;
			return true;
		}

		return false;
	}

	static GLuint CreateShader(const std::vector<std::string>& vertexSource, const std::vector<std::string>& fragmentSource, const std::string& vertexPath = "", const std::string& fragmentPath = "")
	{
		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		bool compiled = ShaderUtils::CompileShaderFromSource(vertexShaderID, vertexPath, vertexSource);
		if (!compiled)
		{
			std::cout << "Failed to compile Vertex Shader! Check log for details." << std::endl;
			return 0;
		}

		compiled = ShaderUtils::CompileShaderFromSource(fragmentShaderID, fragmentPath, fragmentSource);
		if (!compiled)
		{
			std::cout << "Failed to compile Fragment Shader! Check log for details." << std::endl;
			return 0;
		}

		GLuint ID = glCreateProgram();
		glAttachShader(ID, vertexShaderID);
		glAttachShader(ID, fragmentShaderID);
		glLinkProgram(ID);

		ShaderUtils::WasThereALinkError(ID);
		return ID;
	}

}

Shader::Shader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
	: name(name)
{
	std::vector<std::string> vertexSource = ShaderUtils::LoadSourceFromFile(vertexPath);
	std::vector<std::string> fragmentSource = ShaderUtils::LoadSourceFromFile(fragmentPath);

	std::cout << "Compiling shader " << name << std::endl;
	this->ID = ShaderUtils::CreateShader(vertexSource, fragmentSource, vertexPath, fragmentPath);
	std::cout << "Shader " << name << " compiled successfully!" << std::endl;
}

Shader::Shader(const std::string& name, const std::vector<std::string>& vertexSrc, const std::vector<std::string>& fragmentSrc)
	: name(name)
{
	std::cout << "Compiling shader " << name << std::endl;
	this->ID = ShaderUtils::CreateShader(vertexSrc, fragmentSrc);
	std::cout << "Shader " << name << " compiled successfully!" << std::endl;
}

Shader::~Shader()
{
	glDeleteProgram(this->ID);
}

void Shader::Bind() const
{
	glUseProgram(this->ID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
	glUniform1iv(glGetUniformLocation(this->ID, name.c_str()), count, values);
}

void Shader::SetFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
	glUniform2f(glGetUniformLocation(this->ID, name.c_str()), value.x, value.y);
}

void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
	glUniform3f(glGetUniformLocation(this->ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
	glUniform4f(glGetUniformLocation(this->ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::SetMat4x4(const std::string& name, const glm::mat4& value)
{
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}