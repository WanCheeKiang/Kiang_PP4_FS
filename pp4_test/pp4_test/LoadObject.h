#pragma once
#include"includes.h"

class LoadObject
{

public:
	LoadObject();
	~LoadObject();
	ModelImport CreateSphere(RotationMatrixs rotationMatrixs, int LatLines, int LongLines);
	ModelImport MakeGrid(int width, int height);
	void ProcessFbxMesh(FbxNode* Node, ModelImport* Model,float scale);
	void Compactify(int numIndices, int numVertices, Vertex* vertices, int* indices, ModelImport& model);
	ModelImport ImportFbxModel(const char* FileName,float scale);
	ModelBuffer* CreateModelBuffer(ID3D11Device* device, ModelImport model, const wchar_t* TextureName = nullptr);
	ModelImport LoadObjBuffer(int numIndices, int numVertices, const OBJ_VERT* verts, const unsigned int* indices);
	void MultiTexture(const wchar_t* TextureName1, const wchar_t* TextureName2, ID3D11Device* device);
	void RenderObject(ID3D11DeviceContext* DevContext, Camera camera, cbPerFrame constBufferPF, ID3D11SamplerState* SamplerState, ModelBuffer* model, D3D_PRIMITIVE_TOPOLOGY SetPrimitiveTopology, ID3D11Buffer* buffer, ID3D11Buffer* pfbuffer, XMFLOAT4 outputColor);
	void InstanceRender(ID3D11DeviceContext* DevContext, ID3D11InputLayout* inputlayout, int vertexCount, int instancesCount, ID3D11PixelShader* ps, ID3D11VertexShader*vs, ID3D11SamplerState* SamplerState);
	void RenderTexture(ID3D11DeviceContext* DevContext, Camera camera, cbPerFrame constBufferPF, ID3D11SamplerState* SamplerState, ID3D11PixelShader* ps, ID3D11VertexShader*vs, D3D_PRIMITIVE_TOPOLOGY SetPrimitiveTopology, ID3D11Buffer* buffer, ID3D11Buffer* pfbuffer, ID3D11Buffer* vertexBuffer, ID3D11Buffer* Indexbuffer);

};

