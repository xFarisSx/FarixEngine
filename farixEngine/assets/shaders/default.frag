#version 460 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 Normal;
in vec3 crntPos;

uniform sampler2D tex0;
uniform vec4 objectColor;
uniform bool useTexture;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform bool enableLight;

uniform float matAmbient;   
uniform float matDiffuse;   
uniform float matSpecular;  
uniform float matShininess;  
vec4 pointLight()
{

  vec3 lightVec = lightPos - crntPos;
  float dist = length(lightVec);
  float a = 3.0;
  float b = 0.4;
  float inten = 1.0f/(a*dist*dist+b*dist+1.0f);

	float ambient = 0.20f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight;

  if(useTexture){
      return  texture(tex0, texCoord) * lightColor * (diffuse * inten + ambient + specular);
  } else {
    return objectColor *  lightColor * (diffuse * inten + ambient + specular);

  }
  
}



vec4 directLight()
{
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.0, 1.0, 1.0));

  
    float diffuse = max(dot(normal, lightDir), 0.0);

    vec3 viewDir = normalize(camPos - crntPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    

    vec4 baseColor = useTexture ? texture(tex0, texCoord) : objectColor;
    
    vec3 ambientTerm  = matAmbient * lightColor.rgb;
    vec3 diffuseTerm  = diffuse * matDiffuse * baseColor.rgb * lightColor.rgb;
    vec3 specularTerm = spec * matSpecular * lightColor.rgb;

    vec3 finalColor = ambientTerm + diffuseTerm + specularTerm;
    
    return vec4(finalColor, 1.0);
}



vec4 spotLight()
{
  float outerCone = 0.90f; 
  float innerCone = 0.95f;

	float ambient = 0.20f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos-crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight;


  float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
  float inten = clamp((angle - outerCone)/(innerCone-outerCone), 0.0f, 1.0f);
  
  if(useTexture){

    return  texture(tex0, texCoord) * lightColor * (diffuse * inten + ambient + specular*inten);
  } else{
    return objectColor * lightColor * (diffuse * inten + ambient + specular*inten);
  }
}

void main()
{

	
if(enableLight){
  FragColor = directLight();


} else{
FragColor= useTexture ? texture(tex0, texCoord) : objectColor;

}

}
