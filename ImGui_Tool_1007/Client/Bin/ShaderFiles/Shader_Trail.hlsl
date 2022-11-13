#include "Client_Shader_Defines.hpp"
//struct tagData
//{
//	float3		Position;
//	float2		Texture;
//};
//
//struct tagPos
//{
//	tagData		Data[22];
//};
//
//tagPos		g_vPosition;

//struct tagTexture
//{
//	float2		Texture[22];
//};
//
//tagTexture		g_vTexture;

float4		g_Color;
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;

float		g_fTick;

sampler DefaultSampler = sampler_state {

	filter = min_mag_mip_linear;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	//uint		iIndex : BLENDINDEX;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		/*matWV, */matWVP;

	//matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(g_ViewMatrix, g_ProjMatrix);
	
	/*Out.vPosition = mul(float4(g_vPosition.Data[In.iIndex].Position, 1.f), matWVP);
	Out.vTexUV = g_vPosition.Data[In.iIndex].Texture;*/

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vProjPos = Out.vPosition;
	

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
	//float4		vDepth : SV_TARGET1;
	float4		vDistortion : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//float4 Diffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + g_fTick);
	Out.vColor.a = 0.2f;
	//Out.vDistortion.a = /*(1.f - In.vTexUV.y) * 0.5f + In.vTexUV.x * 0.5f*/0.9f;

	//Out.vColor.x = 0.609375f;
	//Out.vColor.y = 1.f;
	//Out.vColor.z = 0.936669f;
	Out.vColor.x = 0.f;
	Out.vColor.y = 0.f;
	Out.vColor.z = 0.f;
		
	//디스토션에는 투영 좌표를 넣는다 뎁스값도 필요한가? 
	In.vProjPos.w /= 300.f;
	Out.vDistortion = In.vProjPos;
	
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.0f, 0.0f);

	/*if (In.vTexUV.y > 0.3f)
	{
		Out.vColor.x = g_Color.x + In.vTexUV.y;
		Out.vColor.y = g_Color.y + In.vTexUV.y;
		Out.vColor.z = g_Color.z + In.vTexUV.y;
	}*/

	return Out;	
}


technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_CullNone);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}