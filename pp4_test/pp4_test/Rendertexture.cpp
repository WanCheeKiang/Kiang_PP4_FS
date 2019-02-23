#include "Rendertexture.h"



Rendertexture::Rendertexture()
{
}


Rendertexture::~Rendertexture()
{
}

void Rendertexture::CreateRenderTexture(ID3D11Device* Device, D3D11_TEXTURE2D_DESC* TextureDesc, D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc)
{
	Device->CreateTexture2D(TextureDesc, nullptr, &Texture2D);
	Device->CreateRenderTargetView(Texture2D, nullptr, &rtv);
	Device->CreateShaderResourceView(Texture2D, nullptr, &srv);
	if(dsvDesc != nullptr)
	Device->CreateDepthStencilView(Texture2D, dsvDesc, &dsv);
}

void Rendertexture::TextureCleanUp()
{
	if (Texture2D)Texture2D->Release();
	if (rtv)rtv->Release();
	if (srv)srv->Release();
}