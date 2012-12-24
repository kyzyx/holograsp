#include "D3DMesh.h"
#include <Windows.h>

D3DMesh::D3DMesh(void)
{
	vertBuffer = NULL;
	indexBuffer = NULL;
}

D3DMesh::D3DMesh(const char* filename, D3DRenderer* r, bool reverse) : Mesh(filename, r, reverse)
{
	HRESULT res;
	context = r->context;

	// Initialize Vertex Buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(D3DRenderer::D3DPoint)*nvert;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	res = r->device->CreateBuffer(&bd, NULL, &vertBuffer);

	// Initialize index buffer
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned int)*nface*3;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	res = r->device->CreateBuffer(&bd, NULL, &indexBuffer);

	// Populate buffers
	writeBuffer();
	
	rasterSolidState = r->rasterSolidState;
	rasterWireframeState = r->rasterWireframeState;
	rasterEdgeState = r->rasterEdgeState;
}

D3DMesh::~D3DMesh(void)
{
	if (vertBuffer) vertBuffer->Release();
	if (indexBuffer) indexBuffer->Release();
}

void D3DMesh::draw() {
	// Set vertex buffer
	context->RSSetState(rasterSolidState);
	unsigned int stride = sizeof(D3DRenderer::D3DPoint);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	render->applyTranslation(translation[0], translation[1], translation[2]);
	render->applyRotation(rotation[0], rotation[1], rotation[2]);
	render->applyScale(scale[0], scale[1], scale[2]);
	render->predraw();
	context->DrawIndexed(nface*3, 0, 0);
	render->resetTransforms();
}

void D3DMesh::drawWireframe() {
	context->RSSetState(rasterWireframeState);
	unsigned int stride = sizeof(D3DRenderer::D3DPoint);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	render->applyTranslation(translation[0], translation[1], translation[2]);
	render->applyRotation(rotation[0], rotation[1], rotation[2]);
	render->applyScale(scale[0], scale[1], scale[2]);
	render->predraw(false);
	context->DrawIndexed(nface*3, 0, 0);
	render->resetTransforms();
}

void D3DMesh::writeBuffer() {
	// Copy data into buffer
	D3D11_MAPPED_SUBRESOURCE ptr;
	if (FAILED(context->Map(vertBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ptr))) return;
	D3DRenderer::D3DPoint* p = (D3DRenderer::D3DPoint*) ptr.pData;
	for (int i = 0; i < nvert; ++i) {
		p[i].pos = D3DXVECTOR4((vertices+3*i)[0], (vertices+3*i)[1], (vertices+3*i)[2], 1.);
		p[i].norm = D3DXVECTOR3(normals+3*i);
		p[i].col = D3DXVECTOR4(colors+4*i);
		p[i].tex = D3DXVECTOR2(texture+2*i);
	}
	context->Unmap(vertBuffer, 0);

	if (FAILED(context->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ptr))) return;
	memcpy(ptr.pData, indices, sizeof(unsigned int)*3*nface);
	context->Unmap(indexBuffer, 0);
}