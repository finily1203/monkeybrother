#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

out vec2 TexCoords; // Pass to fragment shader

void main() {
    gl_Position = vec4(position, 1.0);
    TexCoords = texCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D u_Texture; // The texture sampler

void main() {
    FragColor = texture(u_Texture, TexCoords); // Sample the texture
}