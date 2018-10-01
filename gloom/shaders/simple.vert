#version 430 core

layout(location=0) in vec4 vertPosition;
layout(location=1) in vec4 vertColor;
uniform layout(location = 3) mat4 camera;
out vec4 vertColorOut;



void main()
{
    gl_Position = camera*vertPosition;
    vertColorOut = vertColor;
}
