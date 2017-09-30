#version 330
in vec2 vVaryingTexCoords;
in vec2 vVaryingPostion;
out vec4 vFragColor;
uniform sampler2D tex_in;
uniform mat4 mTrans;


void main(void)
{
	vec4  vColor;
	vec4 vTex44;
	vec2  texCoords;

	texCoords = (vVaryingPostion+1.0)*0.5;

	vTex44.st = vVaryingTexCoords.st - 0.5f; 
	vTex44.p = 0.0f;
	vTex44.q = 1.0f;
	vTex44 = vTex44 * mTrans;
	vTex44.st += 0.5f; 
	vTex44.st = vTex44.st;

	//if(vTex44.s > 0.0001f && vTex44.s < 1.0001f && vTex44.t > 0.0001f && vTex44.t < 1.0001f)
	{
		vColor = texture(tex_in, vTex44.st);
	}
	//else
	{
		//discard;
	}

	
	vFragColor = vColor;


//	vFragColor = texture(tex_in, vVaryingTexCoords.st);
}
