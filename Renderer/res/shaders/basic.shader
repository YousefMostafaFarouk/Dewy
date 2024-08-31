#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float texIndex;

out vec2 v_TexCoord;
out float v_TexIndex;

uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * position;
    v_TexCoord = texCoord;
    v_TexIndex = texIndex;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float v_TexIndex;

uniform vec4 u_Color;
uniform int u_NumberOfTextures;
uniform sampler2D u_Texture[16];

void main()
{
    int index = int(v_TexIndex);
    vec4 texColor;
    // we are subtracting one because the index is one based
    switch(int(v_TexIndex))
    {
        case 1:
            texColor = texture(u_Texture[0], v_TexCoord);
            break;
        case 2:
            texColor = texture(u_Texture[1], v_TexCoord);
            break;
        case 3:
            texColor = texture(u_Texture[2], v_TexCoord);
            break;
        case 4:
            texColor = texture(u_Texture[3], v_TexCoord);
            break;
        case 5:
            texColor = texture(u_Texture[4], v_TexCoord);
            break;
        case 6:
            texColor = texture(u_Texture[5], v_TexCoord);
            break;
        case 7:
            texColor = texture(u_Texture[6], v_TexCoord);
            break;
        case 8:
            texColor = texture(u_Texture[7], v_TexCoord);
            break;
        case 9:
            texColor = texture(u_Texture[8], v_TexCoord);
            break;
        case 10:
            texColor = texture(u_Texture[9], v_TexCoord);
            break;
        case 11:
            texColor = texture(u_Texture[10], v_TexCoord);
            break;
        case 12:
            texColor = texture(u_Texture[11], v_TexCoord);
            break;
        case 13:
            texColor = texture(u_Texture[12], v_TexCoord);
            break;
        case 14:
            texColor = texture(u_Texture[13], v_TexCoord);
            break;
        case 15:
            texColor = texture(u_Texture[14], v_TexCoord);
            break;
        case 16:
            texColor = texture(u_Texture[15], v_TexCoord);
            break;
        default:
            texColor = vec4(1.0); // Default color if index is out of range
            break;
    }
    color = texColor; //* 0.5 * index + 0.1;
}
        