vec3 LAB_ApplyFog(vec3 fragColor, float fragDistance, vec3 fogColor, float fogStart, float fogEnd)
{
    float fogGradient = (fragDistance-fogStart)/(fogEnd-fogStart);
    fogGradient = smoothstep(0.f, 1.f, fogGradient);
    return mix(fragColor, fogColor, fogGradient);
}
