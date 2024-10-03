#shader vertex
#version 330 core
layout(location = 0) in vec3 position;

uniform mat3 uModel_to_NDC;

void main() {
    gl_Position = vec4(uModel_to_NDC * vec3(position), 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

uniform vec4 u_Color;  // The color to apply to the mesh

void main() {
    FragColor = u_Color;  // Directly output the color
}