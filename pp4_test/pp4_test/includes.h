#pragma once
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

#include<DirectXMath.h>
using namespace DirectX;
#include<d3d11.h>
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
#include"Chest.h"
#define ChestData_vert 820
#define ChestData_Ind 1812

//#include<D3DX11tex.h>
using namespace std;

struct Vertex
{
	XMFLOAT4 pos;
	XMFLOAT3 normal;
	XMFLOAT2 Texture;
};

struct Transform
{
	XMVECTOR pos;
	XMVECTOR rotation;
	XMVECTOR scale;
	XMMATRIX createMatrix()
	{
		return XMMatrixAffineTransformation(scale, XMVectorSet(0.0f,0.0f,0.0f,1.0f), rotation, pos);
	}
	Transform()
	{
		pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		rotation = XMQuaternionIdentity();
		scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

	}

	XMVECTOR Forward()
	{
			XMVECTOR forward = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			XMVECTOR direction = XMVector3Rotate(forward, rotation);

			return direction;
	}
};
struct DirectionalLight
{
	DirectionalLight()
	{
		ZeroMemory(this, sizeof(DirectionalLight));
	}
	XMFLOAT4 ambient;//周圍環境條件 
	XMFLOAT4 diffuse;//漫射 
	XMFLOAT3 pos;
	float space;
	XMFLOAT3 dir;
	float align;
	

};
struct PointLight
{
	PointLight()
	{
		ZeroMemory(this, sizeof(PointLight));
	}
	XMFLOAT4 diffuse;//漫射 
	XMFLOAT3 dir;
	float range;
	XMFLOAT3 pos;
	float align;
};

struct SpotLight
{
	SpotLight()
	{
		ZeroMemory(this, sizeof(SpotLight));
	}
	XMFLOAT4 diffuse;//漫射 
	XMFLOAT3 dir;
	float range;
	XMFLOAT3 pos;
	float cone;
};

struct cbPerFrame
{
	DirectionalLight directLight;
	PointLight ptLight;
	SpotLight stLight;
	float time;
	XMFLOAT3 align;

};


struct CBufferPerObject
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 outputColor;

};



struct ModelBuffer
{
	ID3D11Buffer* VertBuffer;
	ID3D11Buffer* IndexBuffer;
	ID3D11ShaderResourceView* srv;
	int vertCount;
	int indexCount;
	Transform transform;

};

struct ModelImport
{
	vector<Vertex> vertices;
	vector<int>indices;
};

#define s_width 500
#define s_height 500
#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
