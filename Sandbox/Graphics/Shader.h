#pragma once

#include <GL/glew.h>
#include <string>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
private:
    GLuint m_ShaderID;
    bool m_IsCompiled;

    GLuint CompileShader(GLenum type, const std::string& source);

    GLint GetUniformLocation(const std::string& name);

public:
   
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);

    ~Shader();

    void Bind() const;

    void Unbind() const;
   
    bool IsCompiled() const { return m_IsCompiled; }

    void SetUniform1i(const std::string& name, int value);

    static ShaderProgramSource ParseShader(const std::string& filepath);

};


