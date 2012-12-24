#pragma once
#include "mesh.h"
#include "D3DRenderer.h"

class D3DMesh :
	public Mesh
{
public:
	D3DMesh(void);
	D3DMesh(const char* filename, D3DRenderer* render, bool reverse=false);
	~D3DMesh(void);

	virtual void draw();
	virtual void drawWireframe();
	virtual void writeBuffer();

private:
	ID3D11Buffer* vertBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11DeviceContext* context;

	ID3D11RasterizerState*   rasterSolidState;
	ID3D11RasterizerState*   rasterWireframeState;
	ID3D11RasterizerState*   rasterEdgeState;
};

