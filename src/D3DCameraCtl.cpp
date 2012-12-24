#include "D3DCameraCtl.h"
#include <D3DX10math.h>

typedef D3DXMATRIX Matrix;
typedef D3DXVECTOR3 vec3;
void D3DCameraCtl::setCamera() {
	Matrix world;
	D3DXMatrixIdentity(&world);

	vec3 eyev = vec3(eye);
	vec3 atv = vec3(towards);
	vec3 upv = vec3(up);
	Matrix view;
	D3DXMatrixLookAtLH(&view, &eyev, &atv, &upv);

	Matrix proj;
	D3DXMatrixPerspectiveFovLH(&proj, p, w / (float) h, 0.1, 1000);
}
