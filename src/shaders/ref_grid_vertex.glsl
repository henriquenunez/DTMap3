#version 330 core
//Vertex shader

layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 inColorInfo;
//layout (location = 1) in vec2 inTexCoor;

//out vec2 TexCoor;
//out vec3 colorInfo;

out vec3 pointCoordinate;

uniform mat4 model;
uniform mat4 viewProjection;

void main()
{
   gl_Position = viewProjection * model * vec4(aPos, 1.0);
   //colorInfo = vec3(inColorInfo.x, inColorInfo.y, inColorInfo.z); //r = x, g = y, b = z;
   //TexCoor = vec2(inTexCoor.x, inTexCoor.y);
   pointCoordinate = vec3(aPos.xyz);
}
