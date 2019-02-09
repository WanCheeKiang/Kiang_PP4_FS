#pragma once
#include<DirectXMath.h>
using namespace DirectX;
#include<d3d11.h>
#pragma comment(lib,"d3d11.lib")
#include <windows.h>
#include<d3d11.h>
#include <d3d11_1.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include<d2d1.h>
#include<WinUser.h>
#include<dwrite.h>
#include <sstream>
#include"Resource.h"
#include<fbxsdk.h>
#include<iostream>
#include<vector>
#include"DDSTextureLoader.h"

//#include<D3DX11tex.h>
using namespace std;

struct Vertex
{
	XMFLOAT4 pos;
	XMFLOAT3 normal;
	XMFLOAT2 Texture;
};

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT4 dir;
	XMFLOAT4 ambient;//周圍環境條件 
	XMFLOAT4 diffuse;//漫射 

};
struct cbPerFrame
{
	Light light;
};


struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 outputColor;
	Light directLight;
	
};


struct cbPerObject
{
	XMMATRIX WVP;
	XMMATRIX World;
	XMFLOAT4 outputColor;
};
#define s_width 500
#define s_height 500
#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
