
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector		g_vCamPosition;
vector		g_vPlayerPoition;
float		g_fFogRange = 5.f;

vector		g_vLightDir;
vector		g_vLightPos;
float		g_fLightRange;

vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = (vector)1.f;
vector		g_vMtrlSpecular = (vector)1.f;

matrix		g_ProjMatrixInv;
matrix		g_ViewMatrixInv;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_ShadeTexture;
texture2D	g_DepthTexture;
texture2D	g_SpecularTexture;
texture2D	g_DistortionTexture;
texture2D	g_DistortionTexture_Bump; 

texture2D	g_AlphaDepthTexture;
texture2D	g_AlphaDiffuseTexture;

float		g_fTick;

sampler DefaultSampler = sampler_state {

	filter = min_mag_mip_linear;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;
	float4		vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector			vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	float			fViewZ = vDepthDesc.y;

	float4			vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * /*saturate*/(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));

	Out.vShade.a = 1.f;

	vector			vProjPos;
	vProjPos.x = In.vTexUV.x * 2.f - 1.f;
	vProjPos.y = In.vTexUV.y * -2.f + 1.f;
	vProjPos.z = vDepthDesc.x;
	vProjPos.w = 1.f;

	vProjPos = vProjPos * fViewZ;

	vector			vViewPos = mul(vProjPos, g_ProjMatrixInv);
	vector			vWorldPos = mul(vViewPos, g_ViewMatrixInv);

	vector			vReflect = reflect(normalize(g_vLightDir), vNormal);
	vector			vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(/*saturate*/(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f)), 100.f);



	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector			vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	float			fViewZ = vDepthDesc.y;

	float4			vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector			vProjPos;
	vProjPos.x = In.vTexUV.x * 2.f - 1.f;
	vProjPos.y = In.vTexUV.y * -2.f + 1.f;
	vProjPos.z = vDepthDesc.x;
	vProjPos.w = 1.f;

	vProjPos = vProjPos * fViewZ;

	vector			vViewPos = mul(vProjPos, g_ProjMatrixInv);
	vector			vWorldPos = mul(vViewPos, g_ViewMatrixInv);

	vector			vLightDir = vWorldPos - g_vLightPos;

	float			fDistance = length(vLightDir);

	float			fAtt = max(g_fLightRange - fDistance, 0.f) / g_fLightRange;

	Out.vShade = (g_vLightDiffuse * /*saturate*/(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient))) * fAtt;

	Out.vShade.a = 1.f;


	vector			vReflect = reflect(normalize(vLightDir), vNormal);
	vector			vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(/*saturate*/(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f)), 30.f) * fAtt;

	Out.vSpecular.a = 0.f;

	return Out;

}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vDiffuse * vShade + vSpecular;

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BackBuffer(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_Distortion(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	
	float4		Distortion = g_DistortionTexture.Sample(DefaultSampler, In.vTexUV);
	Distortion.w *= 300.f;
	
	if (Distortion.z == 0)
		discard;

	/*if (Distortion.z != 0.f)
	{*/
		//Distortion *= Noise.x;
		//----------------------------------------------------------------------------------------------

		float2		UV;
		//UV.x = (Distortion.x/ Distortion.w + 1.f) / 2.f;
		//UV.y = (Distortion.y/ Distortion.w - 1.f) / -2.f;
		/*vProjPos.z = vDepthDesc.x;
		vProjPos.w = 1.f;*/
		//float2		Trans = In.vTexUV + g_fTick *0.2f;
		float4		Noise = g_DistortionTexture_Bump.Sample(DefaultSampler, In.vTexUV);

		UV.x = In.vTexUV.x + cos(Noise.x * g_fTick * 0.5f) * 0.02f;
		UV.y = In.vTexUV.y + sin(Noise.x * g_fTick * 0.5f) * 0.02f;
		Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, UV);
		//----------------------------------------------------------------------------------------------

		//Out.vBack = Out.vColor;
		//Out.vColor *= Distortion;
	//}
	//else
	//{
	//	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//	Out.vBack = Out.vColor;
	//}


	//float2 Trans = In.vTexUV + g_fTick *0.2f;
	//float4		Noise = g_DistortionTexture_Bump.Sample(DefaultSampler, Trans);

	//float2 UV = In.vTexUV + Noise.xy * 0.02f;

	//Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, UV);


	

	if (Out.vColor.a == 0)
		discard;

	return		Out;
}

PS_OUT PS_MAIN_FOG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector			vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float			fViewZ = vDepthDesc.y;
	vector			vProjPos;
	vProjPos.x = In.vTexUV.x * 2.f - 1.f;
	vProjPos.y = In.vTexUV.y * -2.f + 1.f;
	vProjPos.z = vDepthDesc.x;
	vProjPos.w = 1.f;

	vProjPos = vProjPos * fViewZ;

	vector			vViewPos = mul(vProjPos, g_ProjMatrixInv);
	vector			vWorldPos = mul(vViewPos, g_ViewMatrixInv);

	float		fDistance = length(g_vCamPosition - vWorldPos);

	float4		vFogColor = vector(0.f, 0.f, 0.f, 1.f);

	float		fFogPower = max((fDistance - g_fFogRange) / 15.0f, 1.f);

	Out.vColor = vDiffuse /*- vFogColor*/ / fFogPower;
	//Out.vBack = Out.vColor;
	return Out;
}

struct PS_OUT_ALPHA
{
	float4		vColor : SV_TARGET0;
	float4		vTotalDepth : SV_TARGET1;
};
PS_OUT_ALPHA PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT_ALPHA			Out = (PS_OUT_ALPHA)0;

	//vector			vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vAlphaDiffuse = g_AlphaDiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	vector			vAlphaDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	//==�Ϲݵ���
	float			fViewZ = vDepthDesc.y;


	//==���ĵ���
	float			fAlphaViewZ = vAlphaDepthDesc.y;

	if (fViewZ > fAlphaViewZ)
	{
		Out.vColor = vAlphaDiffuse;
		Out.vTotalDepth = vAlphaDepthDesc;
	}
	else
	{
		//Out.vColor = vDiffuse;
		Out.vTotalDepth = vDepthDesc;
	}

	return Out;
}

RasterizerState RS_Default
{
	FillMode = solid;
};
DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_Skip_ZTest_ZWrite
{
	DepthEnable = false;
	DepthWriteMask = zero;
};
BlendState BS_Default
{
	BlendEnable[0] = false;
};
BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

BlendState BS_Add
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = Add;
};

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Skip_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Skip_ZTest_ZWrite, 0);
		SetBlendState(BS_Add, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Skip_ZTest_ZWrite, 0);
		SetBlendState(BS_Add, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}


	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Skip_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	pass BackBuffer
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Skip_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BackBuffer();
	}

	pass Distortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_Distortion();
	}

	pass Fog
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Skip_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FOG();
	}

	pass Alpha
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Skip_ZTest_ZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

}