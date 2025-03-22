#include "/noise.glsl"



const vec3 LAB_BASE_CLOUD_COLOR = vec3(0.8, 0.7, 0.8);
const vec3 LAB_HIGHLIGHT_CLOUD_COLOR = vec3(1.0, 0.9, 1.0);

vec3 LAB_Gray(vec3 color) { return vec3(0.33333*(color.r + color.g + color.b)); }
vec3 LAB_Saturation(vec3 color, float factor) { return mix(LAB_Gray(color), color, factor); }

float LAB_OverworldSky_SunMask(vec3 normal, vec3 sun_pos, float sun_radius)
{
    float step_radius = 1.0 - sun_radius * sun_radius;
    return step(step_radius, dot(normal, sun_pos));
}


vec3 LAB_OverworldSky_Horizon(vec3 base_sky_color, vec3 horizon_color, float depth, vec3 normal)
{
    vec3 sky_color = LAB_Saturation(base_sky_color, 1.5);
    vec3 sky_color2 = 0.5*LAB_Saturation(base_sky_color, 2.0);
    vec3 ground_color = base_sky_color;
    vec3 ground_color2 = base_sky_color*0.75;


    float b = 0.5 + 0.5*sign(normal.y);
    vec3 y_col = mix(ground_color, sky_color, b);
    vec3 y_col2 = mix(ground_color2, sky_color2, b);

    float a = abs(normal.y);
    a *= a;

    y_col = mix(y_col, y_col2, abs(normal.y));

    a = 1.0-1.0/(100.0*a+1);

    return mix(horizon_color, y_col, a);
}


vec2 LAB_CloudOffset(float time)
{
    return vec2(time*0.05, 0.0);
}

float LAB_OverworldCloudMask(vec3 normal, float time, float density)
{
    vec2 rel_pos = normal.xz/normal.y*0.5;
    vec2 pos = rel_pos + LAB_CloudOffset(time);
    float noise = LAB_Noise2D_Chord3(pos, vec2(0))-0.1;
    float dist_sq = dot(rel_pos, rel_pos);
    float blur = min(2.0, 200.0/dist_sq) * clamp(dist_sq, 0.5, 1.0);
    float near_fade_out = 1.0;
    float far_fade_out = smoothstep(0.01,0.08, normal.y);
    noise = mix(0.56, noise, 1.0/max(1,0.1*dist_sq));
    float raw_mask = smoothstep(0.5, 0.5+blur*0.1, noise * density*0.9);
    return far_fade_out * near_fade_out * raw_mask;
}

float smin( float a, float b, float k )
{
	float h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return mix( b, a, h ) - k*h*(1.0-h);
}
float smax(float a, float b, float k)
{
    return smin(a, b, -k);
}


void LAB_MixCloud(inout float depth_accum, inout vec3 cloud, vec3 color, vec3 normal, float time, float depth)
{
    float mask = LAB_OverworldCloudMask(normal, time, depth);
    cloud = mix(cloud, color, mask);
    depth_accum = 1 - (1-depth_accum) * (1-mask);
    //depth_accum = smax(depth_accum, mask, 0.01);
}

// normal needs to be normalized already
vec3 LAB_OverworldSky(vec3 base_sky_color, vec3 horizon_color, float depth, vec3 normal, float time, bool enable_sun)
{
    vec3 horizon = LAB_OverworldSky_Horizon(base_sky_color, horizon_color, depth, normal);
    float sun_mask = LAB_OverworldSky_SunMask(normal, normalize(vec3(2.0, 1.0, 0.0)), 0.05);

    vec3 horizon_sun = enable_sun ? mix(horizon, vec3(3.0), sun_mask) : horizon;
    if (normal.y > 0.0)
    {
        vec3 cloud = horizon_sun;
        const float delta = 0.03;
        const vec3 mixed_cloud_color = (LAB_BASE_CLOUD_COLOR+LAB_HIGHLIGHT_CLOUD_COLOR)/2;
        float depth_accum = 0;
        LAB_MixCloud(depth_accum, cloud, LAB_HIGHLIGHT_CLOUD_COLOR, normal*vec3(1.0, 1.0-5.0*delta, 1.0), time, 0.9);
        LAB_MixCloud(depth_accum, cloud, LAB_HIGHLIGHT_CLOUD_COLOR, normal*vec3(1.0, 1.0-4.0*delta, 1.0), time, 1.0);
        LAB_MixCloud(depth_accum, cloud, LAB_HIGHLIGHT_CLOUD_COLOR, normal*vec3(1.0, 1.0-3.0*delta, 1.0), time, 1.1);
        LAB_MixCloud(depth_accum, cloud, LAB_HIGHLIGHT_CLOUD_COLOR, normal*vec3(1.0, 1.0-2.0*delta, 1.0), time, 1.1);
        LAB_MixCloud(depth_accum, cloud, mixed_cloud_color,         normal*vec3(1.0, 1.0-1.0*delta, 1.0), time, 1.0);
        LAB_MixCloud(depth_accum, cloud, LAB_BASE_CLOUD_COLOR,      normal,                               time, 0.8);
        cloud = mix(horizon_sun, cloud, smoothstep(0.1, 0.6, depth_accum)+-0.3*(1-depth_accum));
        horizon_sun = cloud;
    }

    return mix(horizon_sun, base_sky_color, depth);
}