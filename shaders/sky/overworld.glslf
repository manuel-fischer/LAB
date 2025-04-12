#include "/sky/overworld.glsl"

uniform vec4 fog_color;
uniform vec4 horizon_color;
uniform float sky_visibility;

uniform float time;

in vec3 Normal;

layout(location = 0) out vec4 fragColor;

void main()
{
    vec3 normalizedNormal = normalize(Normal);

    fragColor = vec4(
        LAB_OverworldSky(fog_color.rgb, horizon_color.rgb,
                         1-sky_visibility, normalizedNormal, time, true),
        1
    );
}