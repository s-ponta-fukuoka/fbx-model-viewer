//“ü—Í—p
struct pixcelIn
{
	float4 pos : SV_POSITION;
	float4 nrm : NORMAL0;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
	float4 col2: COL2;
	float4 Lpos : TEXCOORD1;
	float4 Spos : TEXCOORD2;
};

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

float4 main(pixcelIn IN) : SV_Target
{
	return IN.Spos.z / IN.Spos.w;
}