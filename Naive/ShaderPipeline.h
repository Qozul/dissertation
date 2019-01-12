#pragma once
//#include "Utility.h"
#include "../Shared/Utility.h"

namespace QZL
{
	class ShaderPipeline {
	public:
		ShaderPipeline(const std::string& compute);
		ShaderPipeline(const std::string& vertex, const std::string& fragment,
			const std::string geometry = "", const std::string tessControl = "", const std::string tessEval = "");

		GLint getUniformLocation(const std::string& name) {
			GLint location = glGetUniformLocation(id_, name.c_str());
			ENSURES(location != GL_INVALID_INDEX);
			return location;
		}

		GLuint getID() {
			return id_;
		}

		void use() {
			glUseProgram(id_);
		}

		void unuse() {
			glUseProgram(0);
		}

	private:
		void compileShader(GLuint id, std::string path);
		void linkShaders(std::vector<GLuint> shaderIds);

		GLuint id_;
	};
}
