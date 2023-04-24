in vec4 Color;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = Color.rgba;
}
