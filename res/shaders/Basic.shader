#shader vertex
#version 330 core
        
layout(location = 0) in vec4 position;

void main() {
    gl_Position = position;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main() {
   color = u_Color;
};

// #shader fragment_dark
// #version 330 core

// layout(location = 0) out vec4 color;

// void main() {
//    color = vec4(0.2, 0.2, 0.2, 1.0);
// };

// #shader fragment_light
// #version 330 core

// layout(location = 0) out vec4 color;

// void main() {
//    color = vec4(0.8, 0.8, 0.8, 1.0);
// };