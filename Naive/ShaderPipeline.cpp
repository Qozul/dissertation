#include "ShaderPipeline.h"
#include <fstream>

using namespace QZL;

ShaderPipeline::ShaderPipeline(const std::string& compute)
{
	GLuint cId = glCreateShader(GL_COMPUTE_SHADER);
	ENSURES(cId != NULL);
	compileShader(cId, compute);

	linkShaders({ cId });
}

ShaderPipeline::ShaderPipeline(const std::string& vertex, const std::string& fragment,
	const std::string geometry, const std::string tessControl, const std::string tessEval)
{
	id_ = glCreateProgram();

	GLuint vId = glCreateShader(GL_VERTEX_SHADER);
	ENSURES(vId != NULL);
	compileShader(vId, vertex);

	GLuint fId = glCreateShader(GL_FRAGMENT_SHADER);
	ENSURES(fId != NULL);
	compileShader(fId, fragment);

	// TODO implement geometry and tesselation options

	linkShaders({ vId, fId });
}

void ShaderPipeline::compileShader(GLuint id, std::string path)
{
	std::ifstream file(path);
	ENSURESM(!file.fail(), path);

	std::string contents = "";
	std::string line;

	while (std::getline(file, line)) {
		contents += line + "\n";
	}

	file.close();

	const char* ptr = contents.c_str();
	glShaderSource(id, 1, &ptr, nullptr);

	glCompileShader(id);

	GLint isCompiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

		glDeleteShader(id);

		ENSURESM(false, "Shader " + path + " failed to compile");
	}
}

void ShaderPipeline::linkShaders(std::vector<GLuint> shaderIds)
{
	for (GLuint shaderId : shaderIds) {
		glAttachShader(id_, shaderId);
	}

	glLinkProgram(id_);

	GLint isLinked = 0;
	glGetProgramiv(id_, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> infoLog(maxLength);
		glGetProgramInfoLog(id_, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(id_);
		for (GLuint shaderId : shaderIds) {
			glDeleteShader(shaderId);
		}

		ENSURESM(false, "Shaders failed to link");
	}

	for (GLuint shaderId : shaderIds) {
		glDetachShader(id_, shaderId);
	}
	for (GLuint shaderId : shaderIds) {
		glDeleteShader(shaderId);
	}

	glValidateProgram(id_);
}
