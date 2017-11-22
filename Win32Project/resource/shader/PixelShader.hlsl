//入力用
struct pixcelIn
{
	float4 pos : SV_POSITION;
	float4 nrm : NORMAL0;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
	float4 col2: COL2;
	float4 Lpos : POSITION_SM;
	float4 Spos : TEXCOORD2;
};

Texture2D txDiffuse : register(t0);
Texture2D txShadow : register(t1);
SamplerState samLinear : register(s0);
SamplerState samShadow : register(s1);

float3 g_lightVec = { 0.71,-0.71,0.0 };

float4 main(pixcelIn IN) : SV_Target
{
	pixcelIn OUT;

	float bright = max(-dot(IN.nrm, g_lightVec), 0);

	float4 diffuse = txDiffuse.Sample(samLinear, IN.tex) * lerp(float4(0.7f, 0.7f, 0.8f, 1.0f), float4(1.0f, 1.0f, 1.0f, 1.0f), bright);

	// ライト目線によるZ値の再算出
	float ZValue = IN.Lpos.z / IN.Lpos.w;
	
	//// 射影空間のXY座標をテクスチャ座標に変換
	float2 TransTexCoord;
	TransTexCoord.x = (1.0f + IN.Lpos.x / IN.Lpos.w)*0.5f;
	TransTexCoord.y = (1.0f - IN.Lpos.y / IN.Lpos.w)*0.5f;

	float shadowZ = txShadow.Sample(samLinear, IN.Lpos.xy);
	float shadow = (shadowZ + 0.001 > IN.Lpos.z) ? 1.0 : 0.5;

	return diffuse * saturate(shadow);
}

float4 ps_main2(pixcelIn IN) : SV_Target
{
	pixcelIn OUT;

	OUT.col = saturate(IN.col2) * IN.col;

	return OUT.col;
}