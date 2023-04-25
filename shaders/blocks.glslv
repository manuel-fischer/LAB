#include "/color_hdr.glsl"
#include "/fog.glsl"
#include "/tone_mapping.glsl"

uniform vec3 camPos;
uniform mat4 modelproj;
uniform vec2 textureScale;
uniform float exposure;
uniform float saturation;

in vec3 pos;
in vec2 tex;
in vec4 color;
in LAB_ColorHDR light;

out vec4 Color;
out vec2 TexCoord;
out float FogFactor;

uniform float fogStart = 100;
uniform float fogEnd = 120;

void main()
{
    gl_Position = modelproj * vec4(pos, 1);

    float camDistance = distance(pos, camPos);
    FogFactor = LAB_FogGradient(camDistance, fogStart, fogEnd);

    vec3 shade = light == 0xffffffffu ? vec3(1) : LAB_ColorHDR_ToVec3(light);

    shade *= exposure;
    shade = clamp(shade, 0, 2.25);

    Color = color * LAB_ColorSaturation(vec4(shade, 1), saturation);
    TexCoord = textureScale * tex;
}
