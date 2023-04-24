in vec4 Color;
in vec2 TexCoord;

uniform vec4 color;
uniform sampler2D textureImage;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture(textureImage, TexCoord) * color;
}
