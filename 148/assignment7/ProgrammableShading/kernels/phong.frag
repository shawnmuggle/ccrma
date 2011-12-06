// phong.frag

/*
  This fragment implements the Phong Reflection model.
*/

// The input image we will be filtering in this kernel.
uniform sampler2D normalTex;
uniform float specular;

varying vec3 modelPos;    // fragment position in model space
varying vec2 texPos;      // fragment position in texture space
varying vec3 lightSource; // light source position in model space
varying vec3 normal;	  // fragment normal in model space

varying float displacement;

void main()
{
    // Sample from the normal map, if we're not doing displacement mapping
    vec3 N;
	if (displacement < 0.0)
		N = 2.*texture2D(normalTex, texPos).xyz - 1.;
	else
		N = normal;

    vec3 C = vec3(0.0, 0.0, 0.0); // camera position

    vec3 ambientColor  = gl_LightSource[0].ambient.xyz;
    vec3 diffuseColor  = gl_LightSource[0].diffuse.xyz;
    vec3 specularColor = gl_LightSource[0].specular.xyz;

    vec3 materialColor = gl_FrontMaterial.ambient.xyz;
    vec3 materialSpec  = gl_FrontMaterial.specular.xyz;
    float shininess    = gl_FrontMaterial.shininess;

    vec3 Lm = normalize(lightSource - modelPos); // normalized vector from surface point to light source
    vec3 Rm = normalize(-reflect(Lm, N)); // reflection of Lm about N, the direction that light would bounce in
    vec3 V = normalize(C - modelPos); // normalized vector from surface point to viewer
    vec3 color = (materialColor * ambientColor +  // ambient part
                  max(0., dot(Lm, N)) * materialColor * diffuseColor);  // diffuse part
    if (specular > 0.)
    {
        color += pow(max(0., dot(Rm, V)), shininess) * materialSpec * specularColor;  // specular part
        // TODO: Try to replicate hostile takeover by specular part & show to TA
    }
    
    gl_FragColor = vec4(color, 1.);
}
