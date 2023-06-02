in vec3 Normal;


layout(location = 0) out vec4 fragColor;

void main()
{
    vec3 normal = normalize(Normal);

    fragColor = vec4(0.8, 0.9, 1.0, 1.0);
    fragColor *= 0.5+0.5*normal.y;
}