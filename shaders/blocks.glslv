#include "/color_hdr.glsl"
#include "/fog.glsl"
#include "/tone_mapping.glsl"
#include "/sky/overworld.glsl"

uniform vec3 camPos;
uniform mat4 modelproj;
uniform vec2 textureScale;
uniform float exposure;
uniform float saturation;

uniform vec4 fogColor = vec4(0.5, 0.6, 0.7, 1.0);
uniform vec4 horizon_color;
uniform float sky_visibility = 0.0;

uniform float time;

in vec3 pos;
in vec2 tex;
in vec4 color;
in LAB_ColorHDR light;

out vec4 Color;
out vec2 TexCoord;
out vec3 FogColor;
out float FogFactor;

uniform float fogStart = 100;
uniform float fogEnd = 120;

void main()
{
    gl_Position = modelproj * vec4(pos, 1.0);

    vec3 relPos = pos-camPos;
    float camDistance = length(relPos);
    FogFactor = LAB_FogGradient(camDistance, fogStart, fogEnd);

    vec3 shade = light == 0xffffffffu ? vec3(1.0) : LAB_ColorHDR_ToVec3(light);

    shade *= exposure;
    shade = clamp(shade, 0.0, 2.25);

    Color = color * LAB_ColorSaturation(vec4(shade, 1.0), saturation);
    TexCoord = textureScale * tex;

    float fadeStart = max(min(fogEnd-2, 64), fogEnd*0.7);
    if(sky_visibility != 0 && camDistance > fadeStart)
    {
        // interpolation between fog and sky
        // fadeFogToSky = 0 --> use fog color
        // fadeFogToSky = 1 --> use sky color
        float fadeFogToSky = clamp((camDistance-fadeStart)*(1.0/(fogEnd-fadeStart)), 0, 1);
        float vertexDepth = 1 - fadeFogToSky*sky_visibility;
        FogColor = LAB_OverworldSky(fogColor.rgb, horizon_color.rgb,vertexDepth, relPos/camDistance, time, true);
    }
    else
        FogColor = fogColor.rgb;
}
