#pragma once
#include"includes.h"

class Rendertexture
{

public:
	ID3D11Texture2D* Texture2D = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11DepthStencilView* dsv = nullptr;
	Rendertexture();
	~Rendertexture();
	void CreateRenderTexture(ID3D11Device* Device, D3D11_TEXTURE2D_DESC* Texture, D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc);
	void TextureCleanUp();
};

