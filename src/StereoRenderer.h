#pragma once
#include "d3drenderer.h"
#include "nvstereo.h"

class StereoInitException : public std::exception {
};

class StereoRenderer :
	public D3DRenderer
{
public:
	StereoRenderer(App* app);
	~StereoRenderer(void);

	virtual void clear();

private:
	StereoRenderer();
	bool FullSetup(int screenWidth, int screenHeight, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
	nv::stereo::ParamTextureManagerD3D11* stereoTexMgr;
	ID3D11Texture2D* stereoParamTexture;
	ID3D11ShaderResourceView* stereoParamTextureRV;
};

