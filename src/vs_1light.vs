cbuffer MatrixBuffer {
  matrix world;
  matrix modelview;
  matrix projection;
};

cbuffer PointLightBuffer {
	float4 lightpos;
	float4 diffuse;
	float4 ambient;
}

struct VertexInput {
  float4 position : POSITION;
  float4 color : COLOR;
  float3 normal : NORMAL;
};

struct PixelInput {
  float4 position : SV_POSITION;
  float4 color : COLOR;
  float3 normal : NORMAL;
  float3 lightpos : TEXCOORD1;
};

PixelInput VS(VertexInput input) {
  PixelInput output;
  float4 worldpos;

  input.position.w = 1.0f;
  output.position = mul(input.position, world);
  output.position = mul(output.position, modelview);
  output.position = mul(output.position, projection);
  output.color = input.color;
  output.normal = mul(input.normal, (float3x3)world);
  output.normal = normalize(output.normal);
  worldpos = mul(input.position, world);

  output.lightpos.xyz = lightpos.xyz - worldpos.xyz;
  output.lightpos = normalize(output.lightpos);

  return output;
}
