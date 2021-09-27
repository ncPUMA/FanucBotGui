
varying float distToCamera;

void main()
{
    gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;
    vec3 tmp = occWorldViewMatrix * occModelWorldMatrix * occVertex;
    distToCamera = -tmp.z;
}
