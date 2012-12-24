#pragma once
#include "renderer.h"
#include "app.h"

#include "FW1FontWrapper.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX10math.h>
#include <exception>

#define __d3d10_h__
#include "nvapi.h"
#undef __d3d10_h__
class DirectXInitException : public std::exception {
};


class D3DRenderTarget {
public:
	D3DRenderTarget() : renderTargetTexture(NULL), renderTargetView(NULL), shaderResourceView(NULL) {;}
	~D3DRenderTarget() {
		if (renderTargetTexture) renderTargetTexture->Release();
		if (renderTargetView) renderTargetView->Release();
		if (shaderResourceView) shaderResourceView->Release();
	}
	ID3D11Texture2D* renderTargetTexture;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11ShaderResourceView* shaderResourceView;
};

class D3DWindowTarget {
public:
	D3DWindowTarget() {
		vertexBuffer = NULL;
		indexBuffer = NULL;
		rt = NULL;
	}
	~D3DWindowTarget() {
		if (vertexBuffer) vertexBuffer->Release();
		if (indexBuffer) indexBuffer->Release();
		if (rt) delete rt;
	}
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	D3DRenderTarget* rt;
	int x, y, w, h;
};

class D3DRenderer :
	public Renderer
{
public:
	D3DRenderer(void) {;}
	D3DRenderer(App* app);
	~D3DRenderer(void);

	virtual void drawPoints(int n, float* coords, float* colors, float pointSize);
	virtual void drawText(const WCHAR* text, float x, float y, int color, float size);
	virtual void predraw(bool lights=true);
	virtual void clear();
	virtual void display();
	virtual void setProjection(float fov, float aspect, float near, float far);
	virtual void lookAt(float eye[3], float towards[3], float up[3]);
	virtual void ortho();
	virtual void ortho(float w, float h);

	virtual void applyRotation(float a, float b, float c);
	virtual void applyTranslation(float a, float b, float c);
	virtual void applyScale(float a, float b, float c);
	virtual void resetTransforms();
	virtual void enableStereo();
	virtual void disableStereo();

	struct D3DPoint {
		D3DXVECTOR4 pos;
		D3DXVECTOR4 col;
		D3DXVECTOR3 norm;
		D3DXVECTOR2 tex;
	};

	D3DRenderTarget* InitializeRenderTarget(int w, int h);
	void setRenderTarget(D3DRenderTarget* target=NULL);
	void clearRenderTarget(D3DRenderTarget* target);
	D3DWindowTarget* InitializeWindowTarget(int w, int h);
	void DrawWindowTarget(D3DWindowTarget* wt, int x, int y);

	friend class D3DMesh;
protected:
	bool initDeviceAndSwapChain();
	bool initCompleteDeviceAndSwapChain();
	bool initRenderTargets();
	bool initShaders();
	void initCamera();
	bool initVertexBuffer();
	bool initFont();

	struct MatrixBuffer {
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	struct LightBuffer {
		D3DXVECTOR4 lightpos;
		D3DXVECTOR4 diffuse;
		D3DXVECTOR4 ambient;
	};
	
	ID3D11Device*           device; 
	ID3D11DeviceContext*    context;
	IDXGISwapChain*         swapChain;
	D3D_FEATURE_LEVEL       featureLevel; 
	ID3D11RenderTargetView* rendertarget;
	ID3D11Texture2D*        depthstencil;
	ID3D11DepthStencilView* depthview;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilState* noDepthState;
	ID3D11RasterizerState*   rasterSolidState;
	ID3D11RasterizerState*   rasterWireframeState;
	ID3D11RasterizerState*   rasterEdgeState;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexTexShader;
	ID3D11PixelShader* pixelTexShader;
	ID3D11SamplerState* samplestate;
	ID3D11VertexShader* vertexLightShader;
	ID3D11PixelShader* pixelLightShader;
	ID3D11InputLayout* vertexLayout;
	ID3D11InputLayout* vertexTexLayout;
	MatrixBuffer matrices;
	LightBuffer light;
	
	static const int MAX_POINTS = 640*480;
	ID3D11Buffer* testBuffer;
	ID3D11Buffer* pointBuffer;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	D3DPoint pts[MAX_POINTS];

	IFW1Factory* fontfactory;
	IFW1FontWrapper* fontwrapper;

	StereoHandle stereohandle;

	bool fullscreen;
	int w, h;
};