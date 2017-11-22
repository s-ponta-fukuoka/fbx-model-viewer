//ì¸óÕóp
struct vertexIn
{
	float4 pos : POSITION0;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
};

//èoóÕóp
struct vertexOut
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR0;
};

vertexOut main(vertexIn IN)
{
	vertexOut OUT;

	OUT.pos = IN.pos;

	OUT.tex = IN.tex;

	OUT.color = IN.color;

	return OUT;
}
