vec4 LAB_ColorSaturation(vec4 c, float f)
{
    float avg = (c.r+c.g+c.b) / 3.f;

    c.rgb = (c.rgb-avg)*f+avg;

    return c;
}


vec4 LAB_MapTone(vec4 c)
{
    return c;
}
