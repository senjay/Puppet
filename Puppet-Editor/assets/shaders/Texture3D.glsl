// Basic Texture Shader
#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

layout (location = 0) out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 color2;
layout (location = 0) in vec2 v_TexCoord;
layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	color = texture(u_Textures[0], v_TexCoord*10);
	color2 =vec4(v_TexCoord.x,v_TexCoord.y,0.0f,1.0f);
}