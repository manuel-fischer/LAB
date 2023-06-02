uniform vec3 forward;
uniform vec3 right;
uniform vec3 up;

in vec2 screen_pos;

out vec3 Normal;

void main()
{
    gl_Position = vec4(screen_pos, 1.0, 1.0);
    Normal = forward + screen_pos.x*right + screen_pos.y*up;
}
