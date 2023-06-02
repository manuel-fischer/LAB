#include "/tone_mapping.glsl"
#include "/fog.glsl"
#include "/render_pass.glsl"

uniform sampler2D textureImage;

in vec4 Color;
in vec2 TexCoord;
in float FogFactor;
in vec3 FogColor;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture(textureImage, TexCoord) * Color;

#if COMPILED_RENDER_PASS == LAB_RENDER_PASS_ALPHA

    if(fragColor.a < 1./255.) discard;

#elif COMPILED_RENDER_PASS == LAB_RENDER_PASS_MASKED || \
      COMPILED_RENDER_PASS == LAB_RENDER_PASS_BLIT

    if(fragColor.a < 32./255.) discard;

#endif

    //fragColor = LAB_ApplyFog(fragColor, length(m_Po), fogDistance, fogColor);
    fragColor = LAB_MapTone(fragColor);


    fragColor.rgb = LAB_ApplyFog(fragColor.rgb, FogColor.rgb, FogFactor);
}
