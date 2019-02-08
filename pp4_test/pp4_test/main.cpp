#include"includes.h"
#include"GetFile.h"
#include"XTime.h"
//Global : Interface
ID3D11Device* g_Device = nullptr;
IDXGISwapChain* g_SwapChain = nullptr;
ID3D11DeviceContext* g_DevContext = nullptr;
ID3D11RenderTargetView* g_rtv = nullptr;
ID3D11DepthStencilView* g_depthStencilView;
ID3D11Texture2D* g_depthStencilBuffer;
ID3D11InputLayout* g_vertLayout;
//Shadder
ID3D11VertexShader* g_VS = nullptr;
ID3D11PixelShader* g_PS = nullptr;
ID3D11PixelShader*      g_PS_Solid = nullptr;
//Buffer
ID3D11Buffer* g_indexBUffer = nullptr;
ID3D11Buffer* g_vertBuffer = nullptr;
ID3D11Buffer* g_cbPerObjBuffer = nullptr;
//State
ID3D11BlendState* Transparency = nullptr;
ID3D11RasterizerState* CCWcullMode = nullptr;
ID3D11RasterizerState* CWcullMode = nullptr;
ID3D11ShaderResourceView* SRV_tex = nullptr;
ID3D11SamplerState* g_SamplerState = nullptr;

//d2d
ID3D11Buffer* cbPFbuffer; //constant buffer oer frame buffer

//
ID3D10Device1 *g_Device1 = nullptr;
ID3D11DeviceContext1*   g_DevContext1 = nullptr;
IDXGISwapChain1*        g_SwapChain1 = nullptr;
IDXGIKeyedMutex *keyedMutex11 = nullptr;
IDXGIKeyedMutex *keyedMutex10 = nullptr;
ID2D1RenderTarget *D2D_RT = nullptr;
ID2D1SolidColorBrush *Brush = nullptr;
ID3D11Texture2D *BackBuffer11 = nullptr;
ID3D11Texture2D *sharedTex11 = nullptr;
ID3D11Buffer *d2d_VertBuffer = nullptr;
ID3D11Buffer *d2d_IndexBuffer = nullptr;
ID3D11ShaderResourceView *d2dTex = nullptr;


D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

// GLobal : Values
LPCTSTR WndClassName = "Window";
HWND g_hwindow = nullptr;
HRESULT hr;
HINSTANCE g_hInst = nullptr;
XMMATRIX WVP;
XMMATRIX World1;
XMMATRIX View;
XMMATRIX Projection;

XMMATRIX d2d_World;

XMVECTOR Cmaera_pos;
XMVECTOR Camera_Target;
XMVECTOR Camera_up;

XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;

float rotate = 0.01f;


// count
Vertex *vertices;
int numVertices = 0;
int *indices;
int numIndices = 0;
float scale = 5.0f;
float xValue = 0.0f;
float yValue = 0.0f;
//Functions
HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void ProcessFbxMesh(FbxNode* Node);
void Compactify();
void UpdateCamera();
void Draw();
void GetKey();
void Render();
void CleanUp();


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Variables
ConstantBuffer cbPerOBj;
Light light;
cbPerFrame constBufferPF;

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

UINT numElements = ARRAYSIZE(layout);

//main
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

	if (FAILED(InitWindow(hInstance, nShowCmd)))
	{
		MessageBox(0, "Window Initialization - Failed",
			"Error", MB_OK);
		return 0;
	}

	if (FAILED(InitDevice()))    //Initialize Direct3D
	{
		MessageBox(0, "Direct3D Initialization - Failed",
			"Error", MB_OK);
		return 0;
	}
	MSG msg = { 0 };

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}

	CleanUp();

	return (int)msg.wParam;

}

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = "WindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hwindow = CreateWindow("WindowClass", "DX11",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!g_hwindow)
		return E_FAIL;

	ShowWindow(g_hwindow, nCmdShow);

	return S_OK;
}

HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}

HRESULT InitDevice()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hwindow, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &g_Device, &g_featureLevel, &g_DevContext);

		if (hr == E_INVALIDARG)
		{
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, &g_Device, &g_featureLevel, &g_DevContext);
		}

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	IDXGIFactory1* dxgiFactory = nullptr;
	{
		IDXGIDevice* dxgiDevice = nullptr;
		hr = g_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr))
		{
			IDXGIAdapter* adapter = nullptr;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
				adapter->Release();
			}
			dxgiDevice->Release();
		}
	}
	if (FAILED(hr))
		return hr;

	// Create swap chain
	IDXGIFactory2* dxgiFactory2 = nullptr;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
	if (dxgiFactory2)
	{
		// DirectX 11.1 or later
		hr = g_Device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_Device1));
		if (SUCCEEDED(hr))
		{
			(void)g_DevContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_DevContext1));
		}

		DXGI_SWAP_CHAIN_DESC1 sd = {};
		ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC1));
		sd.Width = width;
		sd.Height = height;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;

		hr = dxgiFactory2->CreateSwapChainForHwnd(g_Device, g_hwindow, &sd, nullptr, nullptr, &g_SwapChain1);
		if (SUCCEEDED(hr))
		{
			hr = g_SwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_SwapChain));
		}

		dxgiFactory2->Release();
	}
	else
	{
		// DirectX 11.0 systems
		DXGI_SWAP_CHAIN_DESC sd = {};
		ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = g_hwindow;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		hr = dxgiFactory->CreateSwapChain(g_Device, &sd, &g_SwapChain);
	}

	// Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
	dxgiFactory->MakeWindowAssociation(g_hwindow, DXGI_MWA_NO_ALT_ENTER);

	dxgiFactory->Release();

	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;

	hr = g_Device->CreateRenderTargetView(pBackBuffer, nullptr, &g_rtv);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	ZeroMemory(&descDepth, sizeof(D3D11_TEXTURE2D_DESC));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_Device->CreateTexture2D(&descDepth, nullptr, &g_depthStencilBuffer);
	if (FAILED(hr))
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_Device->CreateDepthStencilView(g_depthStencilBuffer, &descDSV, &g_depthStencilView);
	if (FAILED(hr))
		return hr;

	g_DevContext->OMSetRenderTargets(1, &g_rtv, g_depthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_DevContext->RSSetViewports(1, &vp);

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShader(L"Light_VS.hlsl", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			"The hlsl file cannot be compiled.  Please run this executable from the directory that contains the hlsl file.", "Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_VS);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD0", 0, DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA ,0}
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_vertLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_DevContext->IASetInputLayout(g_vertLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShader(L"Light_PS.hlsl", "DirectionalLightPS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			"The hlsl file cannot be compiled.  Please run this executable from the directory that contains the hlsl file.", "Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_PS);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Compile the pixel shader
	pPSBlob = nullptr;
	hr = CompileShader(L"Light_PS.hlsl", "PSsolid", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			"The hlsl file cannot be compiled.  Please run this executable from the directory that contains the hlsl file.", "Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_PS_Solid);
	pPSBlob->Release();
	//
	const char* lFilename = "Axe Asset\\Axe.fbx";

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
	ProcessFbxMesh(lScene->GetRootNode());
	if (FAILED(hr))
		return hr;


	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * numVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	hr = g_Device->CreateBuffer(&bd, &InitData, &g_vertBuffer);
	if (FAILED(hr))
		return hr;

	// Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_DevContext->IASetVertexBuffers(0, 1, &g_vertBuffer, &stride, &offset);

	// Create index buffer
	// Create vertex buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * numIndices;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = g_Device->CreateBuffer(&bd, &InitData, &g_indexBUffer);
	if (FAILED(hr))
		return hr;

	// Set index buffer
	g_DevContext->IASetIndexBuffer(g_indexBUffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology
	g_DevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_Device->CreateBuffer(&bd, nullptr, &g_cbPerObjBuffer);

	//create buffer for cbpf

	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	hr = g_Device->CreateBuffer(&bd, nullptr, &cbPFbuffer);
	if (FAILED(hr))
		return hr;
	//load texture
	hr = CreateDDSTextureFromFile(g_Device, L"Axe Asset\\axeTexture.dds", nullptr, &SRV_tex);

	//create sample state
	D3D11_SAMPLER_DESC SampDesc = {};
	SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SampDesc.MinLOD = 0;
	SampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_Device->CreateSamplerState(&SampDesc, &g_SamplerState);
	if (FAILED(hr))
		return hr;
	// Initialize the world matrices
	WVP = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	View = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);

	return S_OK;
}
void Draw()
{

}
void UpdateCamera()
{
	
}
void GetKey()
{
	if (GetAsyncKeyState('W'))
	{
		//up
		
	}
	if (GetAsyncKeyState('S'))
	{
		//down
		
	}
	if (GetAsyncKeyState('A'))
	{

		//left
		
	}
	if (GetAsyncKeyState('D'))
	{

		//right
		

	}
}



void ProcessFbxMesh(FbxNode* Node)
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
			numIndices = mesh->GetPolygonVertexCount();
			cout << "\nIndice Count:" << numIndices;

			// No need to allocate int array, FBX does for us
			indices = mesh->GetPolygonVertices();

			// Get vertex count from mesh
			numVertices = mesh->GetControlPointsCount();
			cout << "\nVertex Count:" << numVertices;

			mesh->GetPolygonVertexNormals(normalsVec);
			cout << "\nNormalVec Count:" << normalsVec.Size();

			// Create SimpleVertex array to size of this mesh
			vertices = new Vertex[numVertices];

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
				// Generate random normal
				//vertices[j].Normal = RAND_NORMAL;
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
				Compactify();
			}
			else
			{
				// numVertices is an "out" var so set to new size
				// this is used in the DrawIndexed functions to set the 
				// the right number of triangles
				numVertices = numIndices;
			}

			ProcessFbxMesh(childNode);


		}
	}
}
void Compactify()
{
	float epsilon = 0.1001f;
	vector<Vertex>vertexList;
	cout << "\nindex count BEFORE/AFTER compaction " << numIndices;
	cout << "\nvertex count ORIGINAL (FBX source): " << numVertices;
	//check if thers indices and vertices are repeated
	for (int i = 0; i < numIndices; i++)
	{
		bool found = false;
		unsigned int j;
		for (j = 0; j < vertexList.size(); j++)
		{
			if (abs(vertices[indices[i]].pos.x - vertexList[j].pos.x) < epsilon	&&
				abs(vertices[indices[i]].pos.y - vertexList[j].pos.y) < epsilon	&&
				abs(vertices[indices[i]].pos.z - vertexList[j].pos.z) < epsilon	&&
				vertices[indices[i]].normal.x == vertexList[j].normal.x	&&
				vertices[indices[i]].normal.y == vertexList[j].normal.y	&&
				vertices[indices[i]].normal.z == vertexList[j].normal.z &&
				vertices[indices[i]].Texture.x == vertexList[j].Texture.x    &&
				vertices[indices[i]].Texture.y == vertexList[j].Texture.y)
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
			vertexList.push_back(vertices[indices[i]]);
			indices[i] = vertexList.size() - 1;
		}
	}

	delete vertices;
	vertices = new Vertex[vertexList.size()];
	for (unsigned int i = 0; i < vertexList.size(); i++)
	{
		vertices[i] = vertexList[i];

	}
	numVertices = vertexList.size();
	// print out some stats

	cout << "\nvertex count AFTER expansion: " << numIndices;
	cout << "\nvertex count AFTER compaction: " << vertexList.size();
	cout << "\nSize reduction: " << ((numIndices - vertexList.size()) / (float)numIndices)*100.00f << "%";
	cout << "\nor " << (vertexList.size() / (float)numIndices) << " of the expanded size";

}
void CleanUp()
{
	//Release the COM Objects we created
	if(g_SwapChain)g_SwapChain->Release();
	if(g_Device)g_Device->Release();
	if(g_DevContext)g_DevContext->Release();
	if(g_rtv)g_rtv->Release();
	if(g_vertBuffer)g_vertBuffer->Release();
	if(g_indexBUffer)g_indexBUffer->Release();
	if(g_indexBUffer)g_indexBUffer->Release();
	if(g_VS)g_VS->Release();
	if(g_PS)g_PS->Release();
	if(g_vertLayout)g_vertLayout->Release();
	if(g_depthStencilView)g_depthStencilView->Release();
	if(g_depthStencilBuffer)g_depthStencilBuffer->Release();
	if(g_cbPerObjBuffer)g_cbPerObjBuffer->Release();
	if(Transparency)Transparency->Release();
	if(CCWcullMode)CCWcullMode->Release();
	if(CWcullMode)CWcullMode->Release();

	if(g_SwapChain1)g_SwapChain1->Release();
	if(g_Device1)g_Device1->Release();
	if(g_DevContext1)g_DevContext1->Release();
	if(keyedMutex11)keyedMutex11->Release();
	if(keyedMutex10)keyedMutex10->Release();
	if(D2D_RT)D2D_RT->Release();
	if(Brush)Brush->Release();
	if(BackBuffer11)BackBuffer11->Release();
	if(sharedTex11)sharedTex11->Release();
	if(d2dTex)d2dTex->Release();
	///////////////**************new**************////////////////////
	if(cbPFbuffer)cbPFbuffer->Release();
	//////////////**************new**************////////////////////
	if(g_PS_Solid)g_PS_Solid->Release();
	if(SRV_tex)SRV_tex->Release();
	if(g_SamplerState)g_SamplerState->Release();
	if(d2d_VertBuffer)d2d_VertBuffer->Release();
	if(d2d_IndexBuffer)d2d_IndexBuffer->Release();
	
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}

void Render()
{
	// Update our time
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		t = (timeCur - timeStart) / 1000.0f;
	}

	GetKey();

	// Rotate cube around the origin
	WVP = XMMatrixRotationY(t);

	// Setup our lighting parameters
	

	// Rotate the second light around the origin
	/*XMMATRIX mRotate = XMMatrixRotationY(-2.0f * t);
	XMVECTOR vLightDir = XMLoadFloat4(&LightDirs[1]);g_DevContext
	vLightDir = XMVector3Transform(vLightDir, mRotate);
	XMStoreFloat4(&LightDirs[1], vLightDir);
*/
	// Clear the back buffer
	g_DevContext->ClearRenderTargetView(g_rtv, Colors::MidnightBlue);

	// Clear the depth buffer to 1.0 (max depth)
	g_DevContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Update matrix variables and lighting variables
	//light.dir = XMFLOAT4(0.25f, 0.5f, -1.0f);
	light.dir = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);
	light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	constBufferPF.light = light;
	g_DevContext->UpdateSubresource(cbPFbuffer, 0, nullptr, &constBufferPF, 0, 0);


	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(WVP);
	cb1.mView = XMMatrixTranspose(View);
	cb1.mProjection = XMMatrixTranspose(Projection);
	
	g_DevContext->UpdateSubresource(g_cbPerObjBuffer, 0, nullptr, &cb1, 0, 0);
	g_DevContext->VSSetShader(g_VS, 0, 0);
	g_DevContext->PSSetShader(g_PS, 0, 0);
	// Render the cube
	//
	g_DevContext->VSSetShader(g_VS, nullptr, 0);
	g_DevContext->VSSetConstantBuffers(0, 1, &g_cbPerObjBuffer);
	g_DevContext->PSSetShader(g_PS, nullptr, 0);
	g_DevContext->PSSetConstantBuffers(0, 1, &g_cbPerObjBuffer);
	g_DevContext->DrawIndexed(numIndices, 0, 0);
	g_DevContext->PSSetSamplers(0, 1, &g_SamplerState);
	g_DevContext->PSSetShaderResources(0, 1, &SRV_tex);
	//
	// Render each light
	//marking where the lights come from
	//for (int m = 0; m < 2; m++)
	//{
	XMMATRIX mLight = XMMatrixTranslationFromVector(5.0f * XMLoadFloat4(&light.dir));
	XMMATRIX mLightScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	mLight = mLightScale * mLight;

	//	// Update the world variable to reflect the current light
	cb1.mWorld = XMMatrixTranspose(mLight);
	g_DevContext->UpdateSubresource(g_cbPerObjBuffer, 0, nullptr, &cb1, 0, 0);
	g_DevContext->PSSetShader(g_PS_Solid, nullptr, 0);
	g_DevContext->DrawIndexed(numIndices, 0, 0);
	//}

	// Present our back buffer to our front buffer
	
	g_SwapChain->Present(0, 0);
}