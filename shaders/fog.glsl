float LAB_FogGradient(float distance, float fogStart, float fogEnd)
{
    float fogGradient = (distance-fogStart)/(fogEnd-fogStart);
    return smoothstep(0.f, 1.f, fogGradient);
}


vec3 LAB_ApplyFog(vec3 fragColor, vec3 fogColor, float fogFactor)
{
    return mix(fragColor, fogColor, fogFactor);
}
