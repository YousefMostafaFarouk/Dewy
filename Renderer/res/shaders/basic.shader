#shader vertex
#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float texIndex;

out vec2 v_TexCoord;
out float v_TexIndex;

uniform mat4 u_MVP;

void main()
{
    gl_Position = position * u_MVP;
    v_TexCoord = texCoord;
    v_TexIndex = texIndex;
}

#shader fragment
#version 450 core
layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float v_TexIndex;

uniform vec4 u_Color;
uniform int u_NumberOfTextures;
uniform sampler2D u_Texture[32];

void main()
{
    int index = int(v_TexIndex);
    // we are subtracting one because the index is one based
    vec4 texColor = texture(u_Texture[index-1], v_TexCoord);
    color = texColor; //* 0.5 * index + 0.1;
}
        