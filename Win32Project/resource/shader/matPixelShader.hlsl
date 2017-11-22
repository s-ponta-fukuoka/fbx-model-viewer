struct pixcelIn
{
	float4 pos : SV_POSITION;
	float4 nrm : NORMAL0;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
	float4 col2: COL2;
	float2 tex2 : TEX2;
};

float3 g_lightVec = { 0.71,-0.71,0.0 };
Texture2D txDiffuse2 : register(t1);
SamplerState samLinear : register(s0);

float4 main(pixcelIn IN) : SV_Target
{
	pixcelIn OUT;
	
	float bright = max(dot(IN.nrm, g_lightVec), 0);
	
	float4 shadow = lerp(float4(0.7f, 0.7f, 0.8f, 1.0f), float4(1.0f, 1.0f, 1.0f, 1.0f), bright);
	
	float4 diffuse = txDiffuse2.Sample(samLinear, IN.tex2) * saturate(IN.col) * shadow;
	
	OUT.col = diffuse;

	//float bright = max(dot(IN.nrm, g_lightVec), 0);
	//
	//float4 shadow = lerp(float4(0.7f, 0.7f, 0.8f, 1.0f), float4(1.0f, 1.0f, 1.0f, 1.0f), bright);
	//
	//
	//OUT.col = shadow * IN.col;
	//
	//OUT.col =  * saturate(IN.col2) * txDiffuse2.Sample(samLinear, IN.tex2);

	return OUT.col;
}