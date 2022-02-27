#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
uniform float scale;
uniform mat4 u_MVP;
uniform mat4 model;
uniform mat4 view;

out vec3 ourColor;
out vec2 texCoord; 

void main() {
    gl_Position = u_MVP * view * model * vec4(position.xy * scale, position.zw);
    ourColor = aColor;
    texCoord = aTexCoord; 
};

#shader fragment
#version 330 core

in vec3 ourColor;
in vec2 texCoord; 
layout(location = 0) out vec4 color;

uniform sampler2D ourTexture;

void main() {
    color = vec4(ourColor, 1.0);
    //color = texture(ourTexture, texCoord);
};
