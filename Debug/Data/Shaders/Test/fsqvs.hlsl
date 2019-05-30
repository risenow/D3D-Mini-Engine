void VSEntry(
             out float4 oPos : SV_POSITION
            ,uint id : SV_VertexID
            ,out float2 tc : TEXCOORD0)
{
    
    float x = -1, y = -1;
	x = (id == 2) ? 3.0 : -1.0;
	y = (id == 1) ? 3.0 : -1.0;

    tc = (float2(x, y) + 1.0)*0.5;

	oPos = float4(x, y, 1.0, 1.0);
    //oPos = iPos;//mul(projection, vPos);
}