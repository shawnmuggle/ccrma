// This is a texture sampler.  It lets you sample textures!  The keyword
// "uniform" means constant - sort of.  The uniform variables are the same
// for all fragments in an object, but they can change in between objects.
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;

// Diffuse, ambient, and specular materials.  These are also uniform.
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;
uniform float alpha;

// These are values that OpenGL interpoates for us.  Note that some of these
// are repeated from the fragment shader.  That's because they're passed
// across.
varying vec2 texcoord;
varying vec3 normal;
varying vec3 eyePosition;
varying vec3 tangent;
varying vec3 bitangent;
varying vec4 shadowCoords;

vec4 pointLight(int lightId)
{
    vec3 mapNormal = texture2D(normalMap, texcoord).rgb;
    mapNormal = mapNormal * 2.0 - 1.0;
    mat3 tbn;
    tbn[0] = tangent;
    tbn[1] = bitangent;
    tbn[2] = normalize(normal);
    
    vec3 transformedNormal = tbn * mapNormal;
    
    // Normalize the normal, and calculate light vector and view vector
	vec3 N = normalize(transformedNormal);
	vec3 L = normalize(gl_LightSource[lightId].position.xyz - eyePosition.xyz);
	vec3 V = normalize(-eyePosition);
    
	// Calculate the diffuse color coefficient, and sample the diffuse texture
	float Rd = max(0.0, dot(L, N));
	vec3 Td = texture2D(diffuseMap, texcoord).rgb;
	vec3 diffuse = Rd * Kd * Td * gl_LightSource[lightId].diffuse.rgb;
	
	// Calculate the specular coefficient
	vec3 R = reflect(-L, N);
	float Rs = pow(max(0.0, dot(V, R)), alpha);
	vec3 Ts = texture2D(specularMap, texcoord).rgb;
	vec3 specular = Rs * Ks * Ts * gl_LightSource[lightId].specular.rgb;
    
	// Ambient is easy
	vec3 ambient = Ka * gl_LightSource[lightId].ambient.rgb;
    
    return vec4(diffuse + specular + ambient, 1);
}

vec4 directionalLight(int lightId)
{
    vec3 mapNormal = texture2D(normalMap, texcoord).rgb;
    mapNormal = mapNormal * 2.0 - 1.0;
    mat3 tbn;
    tbn[0] = tangent;
    tbn[1] = bitangent;
    tbn[2] = normalize(normal);
    
    vec3 transformedNormal = tbn * mapNormal;
    
    // Normalize the normal, and calculate light vector and view vector
	// Note: this is doing a directional light, which is a little different
	// from what you did in Assignment 2.
	vec3 N = normalize(transformedNormal);
	vec3 L = normalize(gl_LightSource[lightId].position.xyz);
	vec3 V = normalize(-eyePosition);
    
	// Calculate the diffuse color coefficient, and sample the diffuse texture
	float Rd = max(0.0, dot(L, N));
	vec3 Td = texture2D(diffuseMap, texcoord).rgb;
	vec3 diffuse = Rd * Kd * Td * gl_LightSource[lightId].diffuse.rgb;
	
	// Calculate the specular coefficient
	vec3 R = reflect(-L, N);
	float Rs = pow(max(0.0, dot(V, R)), alpha);
	vec3 Ts = texture2D(specularMap, texcoord).rgb;
	vec3 specular = Rs * Ks * Ts * gl_LightSource[lightId].specular.rgb;
    
	// Ambient is easy
	vec3 ambient = Ka * gl_LightSource[lightId].ambient.rgb;
    
    return vec4(diffuse + specular + ambient, 1);
}

void main() {
    vec4 directionalLightColor = directionalLight(0);
    vec4 pointLightColor = pointLight(1);
    
    // TODO: Make this take into account the surrounding pixels
    
    // divide the shadow coordinate by w
    vec4 tempShadow = shadowCoords / shadowCoords.w ;
    
    // offset the shadow coordinate to avoid bad stuff
    tempShadow.z += 0.005;
    
    // compute the depth
    float d = texture2D(shadowMap,tempShadow.xy).z;
    
    // shadow will not affect the output by default
    float shadow = 1.0;
    // however, if the w value is less than zero and the depth is less
    // than shadow.z, it will 
    if (shadowCoords.w > 0.0 && d < tempShadow.z)
        shadow = 0.5;
    
	// This actually writes to the frame buffer
	gl_FragColor = directionalLightColor * shadow + pointLightColor;
}
