//uniform mat4 modelproj;

in vec2 pos;
in vec4 color;

out vec4 Color;

void main()
{
    gl_Position = vec4(2 * pos - vec2(1.0, 1.0), 1.0, 1.0);
    Color = color;
}
