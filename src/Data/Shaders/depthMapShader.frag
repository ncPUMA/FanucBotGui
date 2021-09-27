
varying float distToCamera;

vec3 toColor(float distance)
{
    float dist = distance * 100.0;

    vec3 clr;
    clr.b = floor(dist / 256.0 / 256.0);
    clr.g = floor((dist - clr.b * 256.0 * 256.0) / 256.0);
    clr.r = floor(dist - clr.b * 256.0 * 256.0 - clr.g * 256.0);
    clr = clr / 255.;
    return clr;
}

void main()
{
    float dist = distToCamera;
    if (dist < 0.0)
        dist = 0.0;

    occSetFragColor(vec4(toColor(dist), 1.));
}

