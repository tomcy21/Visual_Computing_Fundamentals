#version 430 core

out vec4 color;
in vec4 vertColorOut;

void main()
{
    color = vertColorOut;
}
