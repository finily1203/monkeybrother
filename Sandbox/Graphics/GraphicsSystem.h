#pragma once

#include <GL/glew.h>
#include <memory>
#include "Shader.h"

class Shader;

class GraphicsSystem {
public:
    
    GraphicsSystem();

   
    ~GraphicsSystem();

    GraphicsSystem(const GraphicsSystem& other);            
    GraphicsSystem& operator=(const GraphicsSystem& other); 

    // Public methods
    void Initialize();  
    void Update();      
    void Render();      
    void Cleanup();     

private:
    GLuint m_VAO;     
    GLuint m_VBO;      
    GLuint m_Texture;  
    std::unique_ptr<Shader> m_Shader;  

    
    void ReleaseResources();
};
