/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Liu YaoTing (yaoting.liu), Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  Shader.h
@brief  :  This file has the declaration of the Shader class, which is responsible 
           for compiling and linking the vertex and fragment shaders.

* Liu YaoTing (yaoting.liu) :
* 	    - Implemented parsing of shader files to extract the vertex and fragment shader source code.
*
* Javier Chua (javierjunliang.chua) :
*       - Implemented the Shader class, which is responsible for compiling and linking the vertex and fragment shaders.
*
*
* File Contributions: Liu YaoTing (30%), Javier Chua (70%)
*
/*_ _ _ _ ________________________________________________________________________________-\*/
#pragma once

#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>        // For glm::mat4
#include <glm/gtc/type_ptr.hpp>
struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
private:
    GLuint m_ShaderID;
    bool m_IsCompiled;
    bool m_IsInitialized;
    GLuint CompileShader(GLenum type, const std::string& source);

    

public:
   
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);

    ~Shader();

    void Bind() const;

    void Unbind() const;
	// void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    bool IsCompiled() const { return m_IsCompiled; }
    void SetUniformMatrix4f(const std::string& name, const glm::mat4& matrix);
    void SetUniform1i(const std::string& name, int value);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    static ShaderProgramSource ParseShader(const std::string& filepath);

    GLint GetUniformLocation(const std::string& name);
    bool isInitialized() const;
    inline GLuint GetProgram() const {
        return m_ShaderID; 
    }
};


