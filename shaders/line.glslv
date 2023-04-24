uniform mat4 modelproj;

in vec3 pos;

void main()
{
    gl_Position = modelproj * vec4(pos, 1.0);
}
