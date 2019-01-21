#version 400

in vec4 fragmentColor; 

layout(location = 0) out vec4 colorOut; 
 
void main()
{
  colorOut = fragmentColor;
}
