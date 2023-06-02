float LAB_Random1(float n)
{
    return fract(sin(n) * 54637.2819);
}

float LAB_Random2(vec2 n)
{
    return LAB_Random1(dot(n, vec2(8.765, 12.345)));
}


float LAB_Noise2D(vec2 n) {
    vec2 cell = floor(n);
    vec2 f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
    return mix(mix(LAB_Random2(cell            ), LAB_Random2(cell + vec2(1,0)), f.x),
               mix(LAB_Random2(cell + vec2(0,1)), LAB_Random2(cell + vec2(1,1)), f.x), f.y);
}

float LAB_Noise2D_Chord2(vec2 n, vec2 variation)
{
    return 0.66*(LAB_Noise2D(n) + 0.33*LAB_Noise2D(n*2+vec2(321,567)+variation));
}

float LAB_Noise2D_Chord3(vec2 n, vec2 variation)
{
    return (1/1.5)*(LAB_Noise2D(n) + 0.5*LAB_Noise2D(n*2+vec2(321,567)+variation)) + 0.25*LAB_Noise2D(n*4+vec2(987,456)+2*variation);
}
