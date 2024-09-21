#include "Shader.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        std::cout << "Failed to open shader file: " << filepath << std::endl;
        return { "", "" };  // Return empty if the file can't be opened
    }

    enum class Shadertype {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    Shadertype type = Shadertype::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = Shadertype::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = Shadertype::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
    : m_IsCompiled(false) {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    m_ShaderID = glCreateProgram();
    glAttachShader(m_ShaderID, vertexShader);
    glAttachShader(m_ShaderID, fragmentShader);
    glLinkProgram(m_ShaderID);

    GLint isLinked = 0;
    glGetProgramiv(m_ShaderID, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(m_ShaderID, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(m_ShaderID, maxLength, &maxLength, &infoLog[0]);

        std::cerr << "Shader linking failed: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;

        glDeleteProgram(m_ShaderID);
        return;
    }

    m_IsCompiled = true;

    glDetachShader(m_ShaderID, vertexShader);
    glDetachShader(m_ShaderID, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(m_ShaderID);
}

void Shader::Bind() const {
    glUseProgram(m_ShaderID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value) {
    GLint location = GetUniformLocation(name);
    glUniform1i(location, value);
}

GLuint Shader::CompileShader(GLenum type, const std::string& source) {
    GLuint id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);

        std::cerr << "Shader compilation failed: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

GLint Shader::GetUniformLocation(const std::string& name) {
    GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found!" << std::endl;
    }
    return location;
}
