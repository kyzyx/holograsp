#include "D3DRenderer.h"
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <Windows.h>
#include <exception>



static const D3D11_INPUT_ELEMENT_DESC layout[] = {
	{"POSITION",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",        0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

// Adapted from http://code.msdn.microsoft.com/windowsdesktop/Direct3D-Tutorial-Win32-829979ef/sourcecode?fileId=57616&pathId=1133312959
static HRESULT CompileShaderFromFile(const char* szFileName, const char* szEntryPoint, const char* szShaderModel, ID3DBlob** ppBlobOut ) { 
    HRESULT hr = S_OK; 
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS; 
    ID3DBlob* pErrorBlob; 
    hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,  
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL); 
    if(FAILED(hr)) { 
        if(pErrorBlob != NULL) 
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() ); 
        if(pErrorBlob) pErrorBlob->Release(); 
        return hr; 
    } 
    if(pErrorBlob) pErrorBlob->Release(); 
    return S_OK; 
}

bool D3DRenderer::initDeviceAndSwapChain() {
	D3D_FEATURE_LEVEL featureLevels[] = 
    { 
        D3D_FEATURE_LEVEL_11_0, 
        D3D_FEATURE_LEVEL_10_1, 
        D3D_FEATURE_LEVEL_10_0, 
    }; 
    UINT numFeatureLevels = ARRAYSIZE(featureLevels); 
	// Initialize Swap Chain Descriptor
	DXGI_SWAP_CHAIN_DESC sd; 
    ZeroMemory(&sd, sizeof(sd)); 
    sd.BufferCount = 1;
    sd.BufferDesc.Width = w; 
    sd.BufferDesc.Height = h; 
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
    sd.BufferDesc.RefreshRate.Numerator = 60; 
    sd.BufferDesc.RefreshRate.Denominator = 1; 
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // Render to buffer
	sd.OutputWindow = GetActiveWindow(); 
    sd.SampleDesc.Count = 1;                             // Multisampling
    sd.SampleDesc.Quality = 0;                           // Multisampling
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.Windowed = true; 

	// Create Swap Chain
	HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, numFeatureLevels, 
                            D3D11_SDK_VERSION, &sd, &swapChain, &device, &featureLevel, &context); 
	if (FAILED(res)) return false;
	if (fullscreen) res = swapChain->SetFullscreenState(true, NULL);
	if (FAILED(res)) return false;
	return true;
}

// From rastertek.org
bool D3DRenderer::initCompleteDeviceAndSwapChain() {
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes;
	DXGI_MODE_DESC* displayModeList;
	int num = -1; int den = -1;
	DXGI_ADAPTER_DESC adapterDesc;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result)) return false;
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result)) return false;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result)) return false;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result)) return false;
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList) return false;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result)) return false;
	for (unsigned int i = 0; i < numModes; ++i) {
		if (displayModeList[i].Height == h && displayModeList[i].Width == w) {
			num = displayModeList[i].RefreshRate.Numerator;
			den = displayModeList[i].RefreshRate.Denominator;
		}
	}
	if (num == -1) return false;
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result)) return false;
	int vcm = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	printf("%d MB of video memory available\n", vcm);
	delete [] displayModeList;
	adapterOutput->Release();
	factory->Release();

	DXGI_SWAP_CHAIN_DESC sd; 
    ZeroMemory(&sd, sizeof(sd)); 
    sd.BufferCount = 1;
    sd.BufferDesc.Width = w; 
    sd.BufferDesc.Height = h; 
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
    sd.BufferDesc.RefreshRate.Numerator = num; 
    sd.BufferDesc.RefreshRate.Denominator = den; 
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // Render to buffer
	sd.OutputWindow = GetActiveWindow(); 
    sd.SampleDesc.Count = 1;                             // Multisampling
    sd.SampleDesc.Quality = 0;                           // Multisampling
	sd.Flags = 0;
    sd.Windowed = !fullscreen;
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
                            D3D11_SDK_VERSION, &sd, &swapChain, &device, NULL, &context);
	if (FAILED(result)) return false;
	return true;
}

bool D3DRenderer::initRenderTargets() {
	// Create Render Target
	ID3D11Texture2D* backbuffer = NULL;
	HRESULT res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backbuffer);
	if (FAILED(res)) return false;
	/*D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));*/
	res = device->CreateRenderTargetView(backbuffer, NULL, &rendertarget); 
	backbuffer->Release();
	if (FAILED(res)) return false;
	
	// Set depth buffer
	D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = w;
    descDepth.Height = h;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    res = device->CreateTexture2D(&descDepth, NULL, &depthstencil);
	if (FAILED(res)) return false;


	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	res = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(res)) return false;
	depthStencilDesc.DepthEnable = false;
	res = device->CreateDepthStencilState(&depthStencilDesc, &noDepthState);
	if (FAILED(res)) return false;
	context->OMSetDepthStencilState(depthStencilState, 1);

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    res = device->CreateDepthStencilView(depthstencil, &descDSV, &depthview);
	if (FAILED(res)) return false;
	
	context->OMSetRenderTargets(1, &rendertarget, depthview);

	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	res = device->CreateRasterizerState(&rasterDesc, &rasterSolidState);
	if(FAILED(res)) return false;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	res = device->CreateRasterizerState(&rasterDesc, &rasterEdgeState);
	if(FAILED(res)) return false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	res = device->CreateRasterizerState(&rasterDesc, &rasterWireframeState);
	if(FAILED(res)) return false;
	return true;
}

D3DRenderTarget* D3DRenderer::InitializeRenderTarget(int w, int h) {
	D3DRenderTarget* rt = new D3DRenderTarget;
	HRESULT res;
	D3D11_TEXTURE2D_DESC descTarget;
    ZeroMemory( &descTarget, sizeof(descTarget) );
    descTarget.Width = w;
    descTarget.Height = h;
    descTarget.MipLevels = 1;
    descTarget.ArraySize = 1;
	descTarget.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    descTarget.SampleDesc.Count = 1;
    descTarget.SampleDesc.Quality = 0;
    descTarget.Usage = D3D11_USAGE_DEFAULT;
    descTarget.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    descTarget.CPUAccessFlags = 0;
    descTarget.MiscFlags = 0;
    res = device->CreateTexture2D(&descTarget, NULL, &rt->renderTargetTexture);
	if (FAILED(res)) {
		delete rt;
		return NULL;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = descTarget.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	// Create the render target view.
	res = device->CreateRenderTargetView(rt->renderTargetTexture, &renderTargetViewDesc, &rt->renderTargetView);
	if(FAILED(res)) {
		delete rt;
		return NULL;
	}

	// Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = descTarget.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	res = device->CreateShaderResourceView(rt->renderTargetTexture, &shaderResourceViewDesc, &rt->shaderResourceView);
	if(FAILED(res)) {
		delete rt;
		return NULL;
	}
	return rt;
}

void D3DRenderer::setRenderTarget(D3DRenderTarget* target) {
	if (!target) {
		context->OMSetRenderTargets(1, &rendertarget, depthview);
	}
	else {
		context->OMSetRenderTargets(1, &target->renderTargetView, depthview);
	}
}
void D3DRenderer::clearRenderTarget(D3DRenderTarget* target) {
	context->ClearRenderTargetView(target->renderTargetView, clearcolor);
	context->ClearDepthStencilView(depthview, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
static const int nwindowverts = 6;
D3DWindowTarget* D3DRenderer::InitializeWindowTarget(int width, int height) {
	D3DWindowTarget* wt = new D3DWindowTarget;
	wt->x = -1; wt->y = -1; wt->w = width; wt->h = height;
	wt->rt = InitializeRenderTarget(w,h);
	D3DPoint verts[nwindowverts];
	unsigned long inds[nwindowverts];
	for (int i = 0; i < nwindowverts; ++i) inds[i] = i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	memset(verts, 0, sizeof(D3DPoint)*nwindowverts);

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(D3DPoint) * nwindowverts;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = verts;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &wt->vertexBuffer);
	if(FAILED(result)) {
		delete wt;
		return NULL;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * nwindowverts;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = inds;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &wt->indexBuffer);
	if(FAILED(result))  {
		delete wt;
		return NULL;
	}
	return wt;
}

void D3DRenderer::DrawWindowTarget(D3DWindowTarget* wt, int x, int y) {
	if (x != wt->x || y != wt->y) {
		wt->x = x;
		wt->y = y;
		float left = x;
		float right = left + (float)wt->w;
		float top = y;
		float bottom = top + (float)wt->h;
		D3DPoint vertices[nwindowverts];
		// First triangle.
		vertices[0].pos = D3DXVECTOR4(left, top, 0.0f,1.0f);  // Top left.
		vertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);

		vertices[1].pos = D3DXVECTOR4(right, bottom, 0.0f,1.0f);  // Bottom right.
		vertices[1].tex = D3DXVECTOR2(1.0f, 1.0f);

		vertices[2].pos = D3DXVECTOR4(left, bottom, 0.0f,1.0f);  // Bottom left.
		vertices[2].tex = D3DXVECTOR2(0.0f, 1.0f);

		// Second triangle.
		vertices[3].pos = D3DXVECTOR4(left, top, 0.0f,1.0f);  // Top left.
		vertices[3].tex = D3DXVECTOR2(0.0f, 0.0f);

		vertices[4].pos = D3DXVECTOR4(right, top, 0.0f,1.0f);  // Top right.
		vertices[4].tex = D3DXVECTOR2(1.0f, 0.0f);

		vertices[5].pos = D3DXVECTOR4(right, bottom, 0.0f,1.0f);  // Bottom right.
		vertices[5].tex = D3DXVECTOR2(1.0f, 1.0f);

		D3D11_MAPPED_SUBRESOURCE resource;
		HRESULT res = context->Map(wt->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		if (FAILED(res)) return;
		D3DPoint* ptr = (D3DPoint*) resource.pData;
		memcpy(ptr, vertices, sizeof(D3DPoint)*nwindowverts);
		context->Unmap(wt->vertexBuffer, 0);
	}
	// Z off
	context->RSSetState(rasterSolidState);
	context->OMSetDepthStencilState(noDepthState, 1);
	D3DXMATRIX ortho(2.0/w, 0, 0, 0, 0, -2.0/h, 0, 0, 0, 0, 1, 0, -1, 1, 0, 1);
	D3DXMATRIX id;
	D3DXMatrixIdentity(&id);

	unsigned int stride = sizeof(D3DPoint);
	unsigned int offset = 0;
	context->IASetInputLayout(vertexTexLayout);
	context->IASetVertexBuffers(0,1,&(wt->vertexBuffer), &stride, &offset);
	context->IASetIndexBuffer(wt->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set matrices
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (FAILED(context->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
	MatrixBuffer* mb = (MatrixBuffer*) mapped.pData;
	D3DXMatrixTranspose(&(mb->world), &id);
	D3DXMatrixTranspose(&(mb->view), &id);
	D3DXMatrixTranspose(&(mb->projection), &ortho);
	context->Unmap(matrixBuffer, 0);

	// Set texture
	context->VSSetConstantBuffers(0, 1, &matrixBuffer);
	context->PSSetShaderResources(0,1,&wt->rt->shaderResourceView);
	context->VSSetShader(vertexTexShader, NULL, 0);
	context->PSSetShader(pixelTexShader, NULL, 0);
	context->PSSetSamplers(0,1,&samplestate);
	context->DrawIndexed(nwindowverts, 0, 0);

	context->OMSetDepthStencilState(depthStencilState, 1);
}

bool D3DRenderer::initShaders() {
	// Compile Vertex Shader
	ID3DBlob* vertexshaderblob, *pixelshaderblob;
	HRESULT res = CompileShaderFromFile("vs.vs", "VS", "vs_5_0", &vertexshaderblob);
	if (FAILED(res)) return false;
	res = device->CreateVertexShader(vertexshaderblob->GetBufferPointer(), vertexshaderblob->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(res)) return false;
	vertexshaderblob->Release();

	res = CompileShaderFromFile("tvs.vs", "VS", "vs_5_0", &vertexshaderblob);
	if (FAILED(res)) return false;
	res = device->CreateVertexShader(vertexshaderblob->GetBufferPointer(), vertexshaderblob->GetBufferSize(), NULL, &vertexTexShader);
	if (FAILED(res)) return false;
	res = device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderblob->GetBufferPointer(), vertexshaderblob->GetBufferSize(), &vertexTexLayout);
	if (FAILED(res)) return false;
	vertexshaderblob->Release();

	res = CompileShaderFromFile("vs_1light.vs", "VS", "vs_5_0", &vertexshaderblob);
	if (FAILED(res)) return false;
	res = device->CreateVertexShader(vertexshaderblob->GetBufferPointer(), vertexshaderblob->GetBufferSize(), NULL, &vertexLightShader);
	if (FAILED(res)) return false;
	res = device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderblob->GetBufferPointer(), vertexshaderblob->GetBufferSize(), &vertexLayout);
	if (FAILED(res)) return false;
	vertexshaderblob->Release();


	// Create input layout
	context->IASetInputLayout(vertexLayout);

	// Compile pixel shader
	res = CompileShaderFromFile("ps.ps", "PS", "ps_5_0", &pixelshaderblob);
	if (FAILED(res)) return false;
	res = device->CreatePixelShader(pixelshaderblob->GetBufferPointer(), pixelshaderblob->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(res)) return false;
	pixelshaderblob->Release();

	res = CompileShaderFromFile("ps_1light.ps", "DirPS", "ps_5_0", &pixelshaderblob);
	if (FAILED(res)) return false;
	res = device->CreatePixelShader(pixelshaderblob->GetBufferPointer(), pixelshaderblob->GetBufferSize(), NULL, &pixelLightShader);
	if (FAILED(res)) return false;
	pixelshaderblob->Release();

	res = CompileShaderFromFile("tps.ps", "PS", "ps_5_0", &pixelshaderblob);
	if (FAILED(res)) return false;
	res = device->CreatePixelShader(pixelshaderblob->GetBufferPointer(), pixelshaderblob->GetBufferSize(), NULL, &pixelTexShader);
	if (FAILED(res)) return false;
	pixelshaderblob->Release();

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Create the texture sampler state.
	res = device->CreateSamplerState(&samplerDesc, &samplestate);
	if(FAILED(res)) return false;

	// Set constant matrix buffer
	D3DXMatrixIdentity(&matrices.world);
	D3DXMatrixIdentity(&matrices.view);
	D3DXMatrixIdentity(&matrices.projection);
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	res = device->CreateBuffer(&bd, NULL, &matrixBuffer);
	if (FAILED(res)) return false;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(LightBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	res = device->CreateBuffer(&bd, NULL, &lightBuffer);
	if (FAILED(res)) return false;
	return true;
}

void D3DRenderer::initCamera() {
	// Viewport
	D3D11_VIEWPORT vp; 
    vp.Width = (float) w; 
    vp.Height = (float) h; 
    vp.MinDepth = 0.0f;   // Depth near plane
    vp.MaxDepth = 1.0f; 
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
	context->RSSetViewports(1, &vp);

	float fov = (float)(3.1415926536/4);
	float aspect = (float)w/(float)h;
	setProjection(fov, aspect, 0.1f, 1000.0f);
	float eye[] = {0.f,0.f,-5.f};
	float up[] = {0.f,1.f,0.f};
	float towards[] = {0.f,0.f,1.f};
	lookAt(eye, towards, up);
	resetTransforms();

	light.diffuse = D3DXVECTOR4(0.9f,0.9f,0.9f,1.f);
	light.ambient = D3DXVECTOR4(0.1f,0.1f,0.1f,1.f);
	light.lightpos = D3DXVECTOR4(0.f,0.f,-1.f,1.f);
}

bool D3DRenderer::initVertexBuffer() {
	// Initialize Vertex Buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(D3DPoint)*MAX_POINTS;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	for (int i = 0; i < MAX_POINTS; ++i) {
		pts[i].pos = D3DXVECTOR4(0.,0.,0.,1.);
		pts[i].col = D3DXVECTOR4(0.,0.,0.,1.);
	}
	HRESULT res = device->CreateBuffer(&bd, NULL, &pointBuffer);
	if (FAILED(res)) return false;

	// Set vertex buffer
	unsigned int stride = sizeof(D3DPoint);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &pointBuffer, &stride, &offset);
	return true;
}

bool D3DRenderer::initFont() {
	HRESULT res;
	res = FW1CreateFactory(FW1_VERSION, &fontfactory);
	if (FAILED(res)) return false;
	res = fontfactory->CreateFontWrapper(device, L"Arial", &fontwrapper);
	if (FAILED(res)) return false;
	return true;
}

D3DRenderer::D3DRenderer(App* app) {
	fullscreen = app->isFullscreen();
	device = NULL;
	context = NULL;
	swapChain = NULL;
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	rendertarget = NULL;
	depthstencil = NULL;
	depthview = NULL;
	fontwrapper = NULL;
	fontfactory = NULL;
	w = app->getWidth();
	h = app->getHeight();

	if (!initCompleteDeviceAndSwapChain()) {
		throw DirectXInitException();
	}
	if (!initRenderTargets()) {
		throw DirectXInitException();
	}
	if (!initShaders()) {
		throw DirectXInitException();
	}
	if (!initVertexBuffer()) {
		throw DirectXInitException();
	}
	if (!initFont()) {
		throw DirectXInitException();
	}
	initCamera();
	
	float conv;
	NvAPI_Initialize();
	NvAPI_Stereo_CreateHandleFromIUnknown(device, &stereohandle);
	NvAPI_Stereo_GetConvergence(stereohandle, &conv);
	NvAPI_Stereo_SetConvergence(stereohandle, 4.0);
}

D3DRenderer::~D3DRenderer() {
	if (vertexShader) vertexShader->Release();
	if (pixelShader)  pixelShader->Release();
	if (vertexTexShader) vertexTexShader->Release();
	if (pixelTexShader)  pixelTexShader->Release();
	if (vertexLightShader) vertexLightShader->Release();
	if (pixelLightShader)  pixelLightShader->Release();
	if (vertexLayout) vertexLayout->Release();
	if (vertexTexLayout) vertexTexLayout->Release();
	if (pointBuffer)  pointBuffer->Release();
	if (matrixBuffer) matrixBuffer->Release();
	if (lightBuffer) lightBuffer->Release();
	if (samplestate) samplestate->Release();

	if (fullscreen && swapChain) swapChain->SetFullscreenState(false, NULL);
	if (context) context->ClearState();
	if (depthstencil) depthstencil->Release();
	if (depthview) depthview->Release();
	if (depthStencilState) depthStencilState->Release();
	if (noDepthState) noDepthState->Release();
	if (rasterSolidState) rasterSolidState->Release();
	if (rasterWireframeState) rasterWireframeState->Release();
	if (rendertarget) rendertarget->Release();
	if (swapChain) swapChain->Release();
	if (context) context->Release();
	if (device) device->Release();
	if (fontwrapper) fontwrapper->Release();
	if (fontfactory) fontfactory->Release();
}

void D3DRenderer::enableStereo() {
	NvAPI_Stereo_Activate(stereohandle);
	NvAPI_Stereo_Enable();
}
void D3DRenderer::disableStereo() {
	NvAPI_Stereo_Deactivate(stereohandle);
	NvAPI_Stereo_Disable();
}

void D3DRenderer::drawPoints(int n, float* coords, float* colors, float pointSize) {
	unsigned int stride = sizeof(D3DPoint);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &pointBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Copy data into buffer
	D3D11_MAPPED_SUBRESOURCE ptr;
	for (int i = 0; i < n; ++i) {
		pts[i].pos = D3DXVECTOR4((coords+3*i)[0], (coords+3*i)[1], (coords+3*i)[2], 1.);
		pts[i].col = D3DXVECTOR4(colors[3*i], colors[3*i+1], colors[3*i+2], 1.);
	}
	if (FAILED(context->Map(pointBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ptr))) return;
	memcpy((D3DPoint*) ptr.pData, pts, sizeof(D3DPoint)*n);
	context->Unmap(pointBuffer, 0);

	predraw(false);
	context->Draw(n, 0);
}

void D3DRenderer::drawText(const WCHAR* text, float x, float y, int color, float size) {
	fontwrapper->DrawString(context, text, size, x, y, color, FW1_RESTORESTATE | FW1_CENTER);
}

void D3DRenderer::clear() {
	context->ClearRenderTargetView(rendertarget, clearcolor);
	context->ClearDepthStencilView(depthview, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DRenderer::display() {
	swapChain->Present(0,0);
}

void D3DRenderer::predraw(bool lights) {
	// Set matrices
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (FAILED(context->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
	MatrixBuffer* mb = (MatrixBuffer*) mapped.pData;
	D3DXMatrixTranspose(&(mb->world), &matrices.world);
	D3DXMatrixTranspose(&(mb->view), &matrices.view);
	D3DXMatrixTranspose(&(mb->projection), &matrices.projection);
	context->Unmap(matrixBuffer, 0);

	// Set lights
	if (FAILED(context->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
	memcpy(mapped.pData, &light, sizeof(LightBuffer));
	context->Unmap(lightBuffer, 0);

	// Draw
	context->IASetInputLayout(vertexLayout);
	if (lights) {
		context->VSSetShader(vertexLightShader, NULL, 0);
		context->VSSetConstantBuffers(0, 1, &matrixBuffer);
		context->VSSetConstantBuffers(1, 1, &lightBuffer);
		context->PSSetShader(pixelLightShader, NULL, 0);
		context->PSSetConstantBuffers(0, 1, &lightBuffer);
	}
	else {
		context->VSSetShader(vertexShader, NULL, 0);
		context->VSSetConstantBuffers(0, 1, &matrixBuffer);
		context->PSSetShader(pixelShader, NULL, 0);
	}
}

void D3DRenderer::setProjection(float fov, float aspect, float nearclip, float farclip) {
	D3DXMatrixPerspectiveFovRH(&matrices.projection, fov, aspect, nearclip, farclip);
}

typedef D3DXVECTOR3 vec3;
void D3DRenderer::lookAt(float eye[3], float towards[3], float up[3]) {
	vec3 eyev = vec3(eye);
	vec3 atv = vec3(towards) + eyev;
	vec3 upv = vec3(up);
	D3DXMatrixLookAtRH(&matrices.view, &eyev, &atv, &upv);
}

void D3DRenderer::ortho() {
	D3DXMatrixOrthoRH(&matrices.projection, (float)w, (float)h, 0.1f, 1000.0f);
}

void D3DRenderer::ortho(float ww, float hh) {
	D3DXMatrixOrthoRH(&matrices.projection, ww, hh, -100.0f, 100.0f);
}

void D3DRenderer::applyRotation(float a, float b, float c) {
	D3DXMATRIX tmp;
	D3DXMatrixRotationYawPitchRoll(&tmp, a, b, c);
	matrices.world = tmp*matrices.world;
}
void D3DRenderer::applyTranslation(float a, float b, float c) {
	D3DXMATRIX tmp;
	D3DXMatrixTranslation(&tmp, a, b, c);
	matrices.world = tmp * matrices.world;
}
void D3DRenderer::applyScale(float a, float b, float c) {
	D3DXMATRIX tmp;
	D3DXMatrixScaling(&tmp, a, b, c);
	matrices.world = tmp * matrices.world;
}
void D3DRenderer::resetTransforms() {
	D3DXMatrixIdentity(&matrices.world);
}