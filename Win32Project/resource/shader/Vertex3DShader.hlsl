//入力用
struct vertexIn
{
	float4 pos : POSITION0;
	float4 nrm : NORMAL0;
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
};

//出力用
struct vertexOut
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

//変換用行列
cbuffer ConstantBuffer : register(b0)
{
	float4x4  World;		//ワールド変換行列
	float4x4 View;		//ビュー変換行列
	float4x4 Projection;		//プロジェクション変換行列
}

vertexOut main(vertexIn IN)
{
	vertexOut OUT;

	float4 posWorld;
	posWorld = mul(IN.pos, World);

	OUT.pos = mul(mul(posWorld, View), Projection);

	OUT.tex = IN.tex;

	return OUT;
}
