uniform samplerCube cubeMap;

varying vec3 normal;
varying vec3 eyePosition;

uniform mat4 inverseViewMatrix;

void main() {
    
    vec3 N = normalize(normal);
    vec3 V = normalize(-eyePosition);

    vec3 R = normalize(reflect(V, N));
    
    vec3 R_inv = (inverseViewMatrix * vec4(R, 0.0)).xyz;
    	
    gl_FragColor = vec4(textureCube(cubeMap, R_inv).xyz, 1.0);
}
