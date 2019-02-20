#include "LoadObject.h"
#include"DDSTextureLoader.h"


LoadObject::LoadObject()
{
}


LoadObject::~LoadObject()
{
}

ModelImport LoadObject::CreateSphere(RotationMatrixs rotationMatrixs,int LatLines, int LongLines)
{
	ModelImport sphere;

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;
	int vertCount = ((LatLines - 2) * LongLines) + 2;
	int indicesCount = (((LatLines - 3)*(LongLines) * 2) + (LongLines * 2));
	sphere.vertices.resize(vertCount);


	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMStoreFloat4(&sphere.vertices[0].pos, currVertPos);


	for (DWORD i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14 / (LatLines - 1));
		rotationMatrixs.Rotationx = XMMatrixRotationX(spherePitch);
		for (DWORD j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * (6.28 / (LongLines));
			rotationMatrixs.Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (rotationMatrixs.Rotationx * rotationMatrixs.Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);
			sphere.vertices[i*LongLines + j + 1].pos.x = XMVectorGetX(currVertPos);
			sphere.vertices[i*LongLines + j + 1].pos.y = XMVectorGetY(currVertPos);
			sphere.vertices[i*LongLines + j + 1].pos.z = XMVectorGetZ(currVertPos);
		}
	}

	sphere.vertices[vertCount - 1].pos.x = 0.0f;
	sphere.vertices[vertCount - 1].pos.y = 0.0f;
	sphere.vertices[vertCount - 1].pos.z = -1.0f;



	sphere.indices.resize(indicesCount * 3);

	int k = 0;
	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		sphere.indices[k] = 0;
		sphere.indices[k + 1] = l + 1;
		sphere.indices[k + 2] = l + 2;
		k += 3;
	}

	sphere.indices[k] = 0;
	sphere.indices[k + 1] = LongLines;
	sphere.indices[k + 2] = 1;
	k += 3;

	for (DWORD i = 0; i < LatLines - 3; ++i)
	{
		for (DWORD j = 0; j < LongLines - 1; ++j)
		{
			sphere.indices[k] = i * LongLines + j + 1;
			sphere.indices[k + 1] = i * LongLines + j + 2;
			sphere.indices[k + 2] = (i + 1)*LongLines + j + 1;

			sphere.indices[k + 3] = (i + 1)*LongLines + j + 1;
			sphere.indices[k + 4] = i * LongLines + j + 2;
			sphere.indices[k + 5] = (i + 1)*LongLines + j + 2;

			k += 6; // next quad
		}

		sphere.indices[k] = (i*LongLines) + LongLines;
		sphere.indices[k + 1] = (i*LongLines) + 1;
		sphere.indices[k + 2] = ((i + 1)*LongLines) + LongLines;

		sphere.indices[k + 3] = ((i + 1)*LongLines) + LongLines;
		sphere.indices[k + 4] = (i*LongLines) + 1;
		sphere.indices[k + 5] = ((i + 1)*LongLines) + 1;

		k += 6;
	}

	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		sphere.indices[k] = sphere.vertices.size() - 1;
		sphere.indices[k + 1] = (vertCount - 1) - (l + 1);
		sphere.indices[k + 2] = (vertCount - 1) - (l + 2);
		k += 3;
	}

	sphere.indices[k] = sphere.vertices.size() - 1;
	sphere.indices[k + 1] = (vertCount - 1) - LongLines;
	sphere.indices[k + 2] = vertCount - 2;


	return sphere;
}

ModelImport LoadObject::MakeGrid(int width, int height)
{
	ModelImport model;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			Vertex v;
			v.normal = XMFLOAT3(0.f, 1.f, 0.f);
			v.Texture.x = i / (width - 1);
			v.Texture.y = j / (height - 1);
			v.pos.x = -0.5f + ((float)i / (width - 1));
			v.pos.y = 0.0f;
			v.pos.z = -0.5f + ((float)j / (height - 1));

			model.vertices.push_back(v);
		}
	}

	for (int i = 0; i < height; i++)
	{
		int start = i;
		int end = (width - 1)*height + i;
		model.indices.push_back(start);
		model.indices.push_back(end);
	}

	for (int i = 0; i < width; i++)
	{
		int start = i * height;
		int end = height - 1 + i * height;
		model.indices.push_back(start);
		model.indices.push_back(end);
	}

	return model;
}
void LoadObject::ProcessFbxMesh(FbxNode* Node, ModelImport* Model,float scale)
{
	// set up output console
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	//FBX Mesh stuff
	int childrenCount = Node->GetChildCount();

	cout << "\nName:" << Node->GetName();

	// Get the Normals array from the mesh
	FbxArray<FbxVector4> normalsVec;

	for (int i = 0; i < childrenCount; i++)
	{
		FbxNode *childNode = Node->GetChild(i);
		FbxMesh *mesh = childNode->GetMesh();

		int materialCount = childNode->GetSrcObjectCount<FbxSurfaceMaterial>();
		if (mesh != NULL)
		{
			cout << "\nMesh:" << childNode->GetName();

			// Get index count from mesh
			int numIndices = mesh->GetPolygonVertexCount();
			cout << "\nIndice Count:" << numIndices;

			// No need to allocate int array, FBX does for us
			int* indices = mesh->GetPolygonVertices();

			// Get vertex count from mesh
			int numVertices = mesh->GetControlPointsCount();
			cout << "\nVertex Count:" << numVertices;

			mesh->GetPolygonVertexNormals(normalsVec);
			cout << "\nNormalVec Count:" << normalsVec.Size();

			// Create SimpleVertex array to size of this mesh
			Vertex* vertices = new Vertex[numVertices];

			for (int index = 0; index < materialCount; index++)
			{
				FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)childNode->GetSrcObject<FbxSurfaceMaterial>(index);

				if (material != nullptr)
				{
					cout << "\nmaterial: " << material->GetName() << std::endl;
					FbxProperty fbxProp = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

					int layeredTExtureCount = fbxProp.GetSrcObjectCount<FbxLayeredTexture>();

					if (layeredTExtureCount > 0)
					{
						for (int j = 0; j < layeredTExtureCount; j++)
						{
							FbxLayeredTexture* layered_texture =
								FbxCast<FbxLayeredTexture>(fbxProp.GetSrcObject<FbxLayeredTexture>(j));
							int layercount = layered_texture->GetSrcObjectCount<FbxTexture>();

							for (int k = 0; k < layercount; k++)
							{
								FbxFileTexture* texture =
									FbxCast<FbxFileTexture>(layered_texture->GetSrcObject<FbxTexture>(k));
								const char* textureName = texture->GetFileName();
								cout << textureName;
							}
						}
					}
					else
					{
						int textureCount = fbxProp.GetSrcObjectCount<FbxTexture>();
						for (int j = 0; j < textureCount; j++)
						{
							FbxFileTexture* texture = FbxCast<FbxFileTexture>(fbxProp.GetSrcObject<FbxTexture>(j));
							const char* textureName = texture->GetFileName();
							cout << textureName;
							FbxProperty p = texture->RootProperty.Find("Filename");
							cout << p.Get<FbxString>() << std::endl;
						}
					}
				}
			}

			FbxStringList UvNameList;
			mesh->GetUVSetNames(UvNameList);
			FbxVector2 UVvalue2;


			//================= Process Vertices ===================
			for (int j = 0; j < numVertices; j++)
			{
				FbxVector4 vert = mesh->GetControlPointAt(j);
				vertices[j].pos.x = (float)vert.mData[0] / scale;
				vertices[j].pos.y = (float)vert.mData[1] / scale;
				vertices[j].pos.z = (float)vert.mData[2] / scale;
			}
			// Declare a new array for the second vertex array
			// Note the size is numIndices not numVertices
			Vertex *vertices2 = new Vertex[numIndices];
			// align (expand) vertex array and set the normals
			for (int j = 0; j < numIndices; j++)
			{
				vertices2[j].pos = vertices[indices[j]].pos;
				vertices2[j].normal.x = (float)normalsVec[j][0];
				vertices2[j].normal.y = (float)normalsVec[j][1];
				vertices2[j].normal.z = (float)normalsVec[j][2];
			}

			for (int UVindex = 0; UVindex < UvNameList.GetCount(); UVindex++)
			{
				const char* UvSetName = UvNameList.GetStringAt(UVindex);
				FbxGeometryElementUV* UVelement = mesh->GetElementUV(UvSetName);
				if (!UVelement)
					continue;
				if (UVelement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex&&
					UVelement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
					return;
				const bool useIndex = UVelement->GetReferenceMode() != FbxGeometryElement::eDirect;
				const int IndexCount = (useIndex) ? UVelement->GetIndexArray().GetCount() : 0;

				const int PolyCount = mesh->GetPolygonCount();

				if (UVelement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					for (int PolyIndex = 0; PolyIndex < PolyCount; PolyIndex++)
					{
						const int PolySize = mesh->GetPolygonSize(PolyIndex);
						for (int vertIndex = 0; vertIndex < PolySize; vertIndex++)
						{
							FbxVector2 UVvalue;
							int PolyVertIndex = mesh->GetPolygonVertex(PolyIndex, vertIndex);
							int UVindex = useIndex ? UVelement->GetIndexArray().GetAt(PolyVertIndex) : PolyVertIndex;
							UVvalue = UVelement->GetDirectArray().GetAt(UVindex);
							vertices2[PolyIndex].Texture.x = (float)UVvalue.mData[0];
							vertices2[PolyIndex].Texture.y = 1.0f - (float)UVvalue.mData[1];
						}
					}
				}
				else if (UVelement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					int PolyIndexCounter = 0;
					for (int PolyIndex = 0; PolyIndex < PolyCount; ++PolyIndex)
					{
						// build the max index array that we need to pass into MakePoly
						const int PolySize = mesh->GetPolygonSize(PolyIndex);
						for (int VertIndex = 0; VertIndex < PolySize; ++VertIndex)
						{
							if (PolyIndexCounter < IndexCount)
							{

								//the UV index depends on the reference mode
								int lUVIndex = useIndex ? UVelement->GetIndexArray().GetAt(PolyIndexCounter) : PolyIndexCounter;

								UVvalue2 = UVelement->GetDirectArray().GetAt(lUVIndex);
								vertices2[PolyIndexCounter].Texture.x = (float)UVvalue2.mData[0];
								vertices2[PolyIndexCounter].Texture.y = 1.0f - (float)UVvalue2.mData[1];

								PolyIndexCounter++;
							}
						}
					}
				}

			}

			// vertices is an "out" var so make sure it points to the new array
			// and clean up first array
			delete vertices;
			vertices = vertices2;

			// make new indices to match the new vertex(2) array
			delete indices;
			indices = new int[numIndices];
			for (int j = 0; j < numIndices; j++)
			{
				indices[j] = j;
			}

			if (true)
			{

				Compactify(numIndices, numVertices, vertices, indices, *Model);
			}

			else
			{
				// numVertices is an "out" var so set to new size
				// this is used in the DrawIndexed functions to set the 
				// the right number of triangles
				numVertices = numIndices;
			}

			ProcessFbxMesh(childNode, Model,scale);

			delete[] indices;
		}
	}
}
void LoadObject::Compactify(int numIndices, int numVertices, Vertex* vertices, int* indices, ModelImport& model)
{
	float epsilon = 0.1001f;

	cout << "\nindex count BEFORE/AFTER compaction " << numIndices;
	cout << "\nvertex count ORIGINAL (FBX source): " << numVertices;
	//check if thers indices and vertices are repeated
	for (int i = 0; i < numIndices; i++)
	{
		bool found = false;
		unsigned int j;
		for (j = 0; j < model.vertices.size(); j++)
		{
			if (abs(vertices[indices[i]].pos.x - model.vertices[j].pos.x) < epsilon	&&
				abs(vertices[indices[i]].pos.y - model.vertices[j].pos.y) < epsilon	&&
				abs(vertices[indices[i]].pos.z - model.vertices[j].pos.z) < epsilon	&&
				vertices[indices[i]].normal.x == model.vertices[j].normal.x	&&
				vertices[indices[i]].normal.y == model.vertices[j].normal.y	&&
				vertices[indices[i]].normal.z == model.vertices[j].normal.z &&
				vertices[indices[i]].Texture.x == model.vertices[j].Texture.x    &&
				vertices[indices[i]].Texture.y == model.vertices[j].Texture.y)
			{
				found = true;
				break;
			}

		}
		if (found == true)
		{
			indices[i] = j;
		}
		else
		{
			model.vertices.push_back(vertices[indices[i]]);
			indices[i] = model.vertices.size() - 1;
		}
	}

	delete vertices;
	model.indices.resize(numIndices);
	for (int i = 0; i < numIndices; i++)
	{
		model.indices[i] = indices[i];
	}
	numVertices = model.vertices.size();
	// print out some stats

	cout << "\nvertex count AFTER expansion: " << numIndices;
	cout << "\nvertex count AFTER compaction: " << model.vertices.size();
	cout << "\nSize reduction: " << ((numIndices - model.vertices.size()) / (float)numIndices)*100.00f << "%";
	cout << "\nor " << (model.vertices.size() / (float)numIndices) << " of the expanded size";

}

ModelImport LoadObject::ImportFbxModel(const char* FileName, float scale)
{

	ModelImport model;
	const char* lFilename = FileName;// "Axe Asset\\Axe_1.fbx";

	FbxManager* lSdkManager = FbxManager::Create();
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
	if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
	lImporter->Import(lScene);
	lImporter->Destroy();
	ProcessFbxMesh(lScene->GetRootNode(), &model,scale);

	return model;
}
ModelBuffer* LoadObject::CreateModelBuffer(ID3D11Device* device,ModelImport model, const wchar_t* TextureName)
{
	ModelBuffer* buffer = new ModelBuffer;
	buffer->indexCount = model.indices.size();
	buffer->vertCount = model.vertices.size();

	D3D11_BUFFER_DESC bd = {};
	//set vertex buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex)*model.vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = model.vertices.data();
	device->CreateBuffer(&bd, &initData, &buffer->VertBuffer);


	//set index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int)*model.indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	initData.pSysMem = model.indices.data();
	device->CreateBuffer(&bd, &initData, &buffer->IndexBuffer);
	if (TextureName != nullptr)
		CreateDDSTextureFromFile(device, TextureName, nullptr, &buffer->srv);

	return buffer;
}
ModelImport LoadObject::LoadObjBuffer(int numIndices, int numVertices, const OBJ_VERT* verts, const unsigned int* indices)
{

	ModelImport model;
	model.indices.resize(numIndices);
	model.vertices.resize(numVertices);


	float objScale = 0.5f;

	for (int i = 0; i < numVertices; i++)
	{

		model.vertices[i].pos.x = verts[i].pos[0] * objScale;
		model.vertices[i].pos.y = verts[i].pos[1] * objScale;
		model.vertices[i].pos.z = verts[i].pos[2] * objScale;
		model.vertices[i].pos.w = 1.0f;
		model.vertices[i].normal.x = verts[i].nrm[0];
		model.vertices[i].normal.y = verts[i].nrm[1];
		model.vertices[i].normal.z = verts[i].nrm[2];

		model.vertices[i].Texture.x = verts[i].uvw[0];
		model.vertices[i].Texture.y = verts[i].uvw[1];

	}

	for (int i = 0; i < numIndices; i++)
	{
		model.indices[i] = indices[i];
	}

	return model;

}

void LoadObject::RenderObject(ID3D11DeviceContext* DevContext, Camera camera, cbPerFrame constBufferPF, ID3D11SamplerState* SamplerState,ModelBuffer* model, D3D_PRIMITIVE_TOPOLOGY SetPrimitiveTopology, ID3D11Buffer* buffer, ID3D11Buffer* pfbuffer, XMFLOAT4 outputColor)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	CBufferPerObject cb;
	DevContext->IASetIndexBuffer(model->IndexBuffer, DXGI_FORMAT_R32_UINT, offset);
	DevContext->IASetVertexBuffers(0, 1, &model->VertBuffer, &stride, &offset);

	DevContext->IASetPrimitiveTopology(SetPrimitiveTopology);
	cb.mWorld = XMMatrixTranspose(model->transform.createMatrix());
	cb.mView = XMMatrixTranspose(camera.View());
	cb.mProjection = XMMatrixTranspose(camera.projection);
	cb.outputColor = outputColor;

	XMStoreFloat4(&constBufferPF.CameraPos, camera.transform.pos);
	DevContext->UpdateSubresource(buffer, 0, nullptr, &cb, 0, 0);
	DevContext->UpdateSubresource(pfbuffer, 0, nullptr, &constBufferPF, 0, 0);
	// Render  the axe
	DevContext->VSSetShader(model->vs, nullptr, 0);
	DevContext->PSSetShader(model->ps, nullptr, 0);
	DevContext->VSSetConstantBuffers(0, 1, &buffer);
	DevContext->PSSetConstantBuffers(0, 1, &buffer);
	DevContext->VSSetConstantBuffers(1, 1, &pfbuffer);
	DevContext->PSSetConstantBuffers(1, 1, &pfbuffer);

	DevContext->PSSetSamplers(0, 1, &SamplerState);
	DevContext->PSSetShaderResources(0, 1, &model->srv);
	DevContext->DrawIndexed(model->indexCount, 0, 0);

}
void LoadObject::InstanceRender(ID3D11DeviceContext* DevContext,ID3D11InputLayout* inputlayout , int vertexCount, int instancesCount, ID3D11PixelShader* ps, ID3D11VertexShader*vs, ID3D11SamplerState* SamplerState)
{
	DevContext->IASetInputLayout(inputlayout);

	DevContext->VSSetShader(vs, nullptr, 0);
	DevContext->PSSetShader(ps, nullptr, 0);
	DevContext->PSSetSamplers(0, 1, &SamplerState);

	DevContext->DrawInstanced(vertexCount, instancesCount, 0, 0);
}

void LoadObject::MultiTexture(const wchar_t* TextureName1, const wchar_t* TextureName2, ID3D11Device* device)
{
	ID3D11ShaderResourceView* textures_srv[2];
	if (TextureName1 != nullptr)
		CreateDDSTextureFromFile(device, TextureName1, nullptr, &textures_srv[0]);
	if (TextureName2 != nullptr)
		CreateDDSTextureFromFile(device, TextureName2, nullptr, &textures_srv[1]);
}

