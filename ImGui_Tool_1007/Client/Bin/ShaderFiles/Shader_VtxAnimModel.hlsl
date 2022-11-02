#include "Client_Shader_Defines.hpp"
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float		g_fAlpha;

float4			g_vCamPosition;
float			g_fFogRange = 10.f;

struct tagBoneMatrices
{
	matrix		BoneMatrix[256];
};

tagBoneMatrices		g_BoneMatrices;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;

sampler DefaultSampler = sampler_state {

	filter = min_mag_mip_linear;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	float4x4	BoneMatrix = g_BoneMatrices.BoneMatrix[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrix[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrix[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrix[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	Out.vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix).xyz;
	Out.vTangent = mul(vector(In.vTangent, 0.f), BoneMatrix).xyz;
	

	Out.vPosition = mul(vPosition, matWVP);

	Out.vNormal = normalize(mul(float4(Out.vNormal, 0.f), g_WorldMatrix)).xyz;
	Out.vTangent = normalize(mul(float4(Out.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);
	if (0.f >= Out.vDiffuse.a)
		discard;

	return Out;
}

PS_OUT PS_MAIN2(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse.a *= (g_fAlpha);

	if (0.f >= Out.vDiffuse.a)
		discard;

	return Out;
}
PS_OUT PS_MAIN3(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (0.f >= Out.vDiffuse.a)
		discard;

	return Out;
}
PS_OUT PS_MAIN_TRAIL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//Out.vColor = (vector)1.f;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse *= vector(0.5f, 0.2f, 0.8f, g_fAlpha);

	if (0 >= Out.vDiffuse.a)
		discard;

	return Out;
}


technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_CullNone);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass DefaultPass2
	{
		SetRasterizerState(RS_CullNone);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN2();
	}

	pass MotionTrail
	{
		SetRasterizerState(RS_CullNone);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TRAIL();
	}

	pass DefaultPass3
	{
		SetRasterizerState(RS_CullNone);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN3();
	}
}