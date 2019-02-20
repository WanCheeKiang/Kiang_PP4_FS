#pragma once
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

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
#include<dinput.h>

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#include <iostream>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
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
		return XMMatrixScalingFromVector(scale)*XMMatrixRotationQuaternion(rotation)*XMMatrixTranslationFromVector(pos);
	}
	Transform()
	{
		pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		rotation = XMQuaternionIdentity();
		scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

	}

	XMVECTOR Forward()
	{
			XMVECTOR dir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			XMVECTOR direction = XMVector3Rotate(dir, rotation);

			return direction;
	}

	XMVECTOR Right()
	{
		XMVECTOR dir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		XMVECTOR direction = XMVector3Rotate(dir, rotation);

		return direction;
	}

	XMVECTOR Up()
	{
		XMVECTOR dir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMVECTOR direction = XMVector3Rotate(dir, rotation);

		return direction;
	}
};
struct DirectionalLightGPU
{
	DirectionalLightGPU()
	{
		ZeroMemory(this, sizeof(DirectionalLightGPU));
	}
	XMFLOAT4 ambient;//周圍環境條件 
	XMFLOAT4 diffuse;//漫射 
	XMFLOAT3 pos;
	float space;
	XMFLOAT3 dir;
	float align;
};
struct PointLightGPU
{
	PointLightGPU()
	{
		ZeroMemory(this, sizeof(PointLightGPU));
	}
	XMFLOAT4 diffuse;//漫射 
	XMFLOAT3 dir;
	float range;
	XMFLOAT3 pos;
	float align;
};

struct SpotLightGPU
{
	SpotLightGPU()
	{
		ZeroMemory(this, sizeof(SpotLightGPU));
	}
	XMFLOAT4 diffuse;//漫射 
	XMFLOAT3 dir;
	float range;
	XMFLOAT3 pos;
	float InConeRatio;
	float OutConeRatio;
	XMFLOAT3 align;
};

struct cbPerFrame
{
	DirectionalLightGPU directLight;
	PointLightGPU ptLight;
	SpotLightGPU stLight;
	float time;
	XMFLOAT3 align;
	XMFLOAT4 CameraPos;
};


struct CBufferPerObject
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 outputColor;
	
};

struct InstanceObject
{
	XMMATRIX mWorld[4];
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMMATRIX space;
};


struct ModelBuffer
{
	ID3D11Buffer* VertBuffer;
	ID3D11Buffer* IndexBuffer;
	ID3D11VertexShader* vs;
	ID3D11PixelShader* ps;
	ID3D11ShaderResourceView* srv;
	int vertCount;
	int indexCount;
	Transform transform;

	ModelBuffer()
	{
		VertBuffer = nullptr;
		IndexBuffer = nullptr;
		vs = nullptr;
		ps = nullptr;
		srv = nullptr;
	}
};

struct ModelImport
{
	vector<Vertex> vertices;
	vector<int>indices;
};


struct Materrial
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT4 reflect;
};

struct RotationMatrixs
{
	XMMATRIX Rotationx;
	XMMATRIX Rotationy;
	XMMATRIX Rotationz;
};

struct HeightMap
{
	int width;
	int height;
	XMFLOAT3* heightMap;
};

struct Camera
{
	XMMATRIX projection;
	Transform transform;

	XMMATRIX View()
	{
		return XMMatrixInverse(nullptr, transform.createMatrix());
	}
};

struct Instance
{
	XMVECTOR pos;
};

struct InstanceObj
{
	int VertCount;
	int InstanceCount;

};
struct DirectionalLight
{
	Transform transform;
	DirectionalLightGPU DirLight;
};

struct PointLight
{
	Transform transform;
	PointLightGPU PtLight;
};
struct SpotLight
{
	Transform transform;
	SpotLightGPU StLight;
};

#define s_width 500
#define s_height 500
#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
