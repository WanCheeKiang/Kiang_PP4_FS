#include"includes.h"
#include"XTime.h"
#include"LoadObject.h"
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
ID3D11PixelShader*  g_PS_Solid = nullptr;
ID3D11PixelShader* g_Reflection_PS = nullptr;
//Buffer
ID3D11Buffer* g_indexBUffer = nullptr;
ID3D11Buffer* g_vertBuffer = nullptr;
ID3D11Buffer* g_cbPerObjBuffer = nullptr;
ID3D11Buffer* obj_VertBuffer = nullptr;
ID3D11Buffer* obj_IndexBuffer = nullptr;
//State
ID3D11BlendState* Transparency = nullptr;
ID3D11RasterizerState* CCWcullMode = nullptr;
ID3D11RasterizerState* CWcullMode = nullptr;
ID3D11SamplerState* g_SamplerState = nullptr;

//d2d
ID3D11Buffer* g_cbPFbuffer; //constant buffer oer frame buffer

ID3D10Device1 *g_Device1 = nullptr;
ID3D11DeviceContext1*   g_DevContext1 = nullptr;
IDXGISwapChain1*        g_SwapChain1 = nullptr;

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

// GLobal : Values
LPCTSTR WndClassName = "Window";
HWND g_hwindow = nullptr;
HRESULT hr;
HINSTANCE g_hInst = nullptr;
XMMATRIX WVP;
XMMATRIX World;
//camera value

Camera camera;
float rotate = 0.01f;
XTime timer;
//camera value
float scale = 1.0f;

float xValue = 0.0f; //value for move right && left
float zValue = 0.0f; //value for move back && forward

LoadObject loadObj;
//get input
DIMOUSESTATE g_MouseState;
IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;
LPDIRECTINPUT8 DirectInput;
//Functions
HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
HRESULT InitInput(HINSTANCE hInstance);
void UpdateCamera();
void GetKey(double time);
void CreateVertexShader(LPCWSTR srcFile, LPCSTR entryPoint, LPCSTR profile, ID3D11VertexShader* vs);
void CreatePixelShader(LPCWSTR srcFile, LPCSTR entryPoint, LPCSTR profile, ID3D11PixelShader* vs);
void Render();
void CleanUp();

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//Variables
CBufferPerObject cbPerOBj;
DirectionalLight Dirlight;
PointLight Ptlight;
SpotLight StLight;
cbPerFrame constBufferPF;
ID3D11ShaderResourceView* obj_srv;
vector<ModelBuffer*> models;
vector<ModelBuffer*> lineModels;
ModelBuffer* skybox;
//cube map
ID3D11VertexShader* sphere_VS = nullptr;
ID3D11PixelShader* sphere_PS = nullptr;
ID3D11DepthStencilState* DSLessEqual = nullptr;
ID3D11RasterizerState* RSCullNone = nullptr;
RotationMatrixs g_RotationMatrix;


//main
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(400);

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


	if (FAILED(InitInput(hInstance)))
	{
		MessageBox(0, "Input Initialization - Failed",
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

	//resize window by creating rezie buffer
	/*g_rtv->Release();
	hr = g_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);*/

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
	hr = CompileShader(L"Light_VS.hlsl", "main", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			"The Light_VS.hlsl file cannot be compiled.  Please run this executable from the directory that contains the hlsl file.", "Error", MB_OK);
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
	D3D11_INPUT_ELEMENT_DESC layout1[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA ,0},


	};
	UINT numElements = ARRAYSIZE(layout1);

	// Create the input layout
	hr = g_Device->CreateInputLayout(layout1, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_vertLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_DevContext->IASetInputLayout(g_vertLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShader(L"Light_PS.hlsl", "main", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			"The Light_PS.hlsl file cannot be compiled.  Please run this executable from the directory that contains the hlsl file.", "Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_PS);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;
	//set another pixel shader 
	pPSBlob = nullptr;
	hr = CompileShader(L"Light_PS.hlsl", "PS_Soild", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			"The Light_PS.hlsl file cannot be compiled.  Please run this executable from the directory that contains the hlsl file.", "Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_PS_Solid);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	pPSBlob = nullptr;
	pVSBlob = nullptr;

	hr = CompileShader(L"CubeMap_VS.hlsl", "main", "vs_4_0", &pVSBlob);
	hr = g_Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &sphere_VS);
	
	hr = CompileShader(L"CubeMap_PS.hlsl", "main", "ps_4_0", &pPSBlob);
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &sphere_PS);
	pPSBlob->Release();
	pVSBlob->Release();
	//CreateVertexShader(L"CubeMap_PS.hlsl", "main", "ps_4_0", sphere_VS);
	//CreatePixelShader(L"CubeMap_PS.hlsl", "main", "ps_4_0", sphere_PS);

	pPSBlob = nullptr;
	hr = CompileShader(L"Reflection_PS.hlsl", "main", "ps_4_0", &pPSBlob);
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_Reflection_PS);
	pPSBlob->Release();

	models.push_back(loadObj.CreateModelBuffer(g_Device,loadObj.ImportFbxModel("Axe Asset\\Axe_1.fbx",scale), L"Axe Asset\\axeTexture.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device,loadObj.LoadObjBuffer(ChestData_Ind, ChestData_vert, Chest_data, Chest_indicies), L"TreasureChestTexture.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device,loadObj.ImportFbxModel("Solid Object Assets\\wall.fbx",scale), L"Solid Object Assets\\stone_texture.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device,loadObj.ImportFbxModel("Solid Object Assets\\sphere.fbx",scale), L"SkyboxOcean.dds"));
	lineModels.push_back(loadObj.CreateModelBuffer(g_Device,loadObj.MakeGrid(15, 15), nullptr));

	models[0]->transform.scale = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
	models[0]->transform.pos = XMVectorSet(-2.0f, 0.0f, 0.0f, 1.0f);
	models[1]->transform.pos = XMVectorSet(3.0f, 0.0f, 0.0f, 1.0f);
	models[2]->transform.pos = XMVectorSet(0.2f, -2.0f, 0.0f, 1.0f);
	models[2]->transform.scale = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
	models[2]->transform.rotation = XMVectorSet(0.0f, 1.5f, 0.0f, 1.0f);
	models[3]->transform.pos = XMVectorSet(0.0f, 4.0f, 2.0f, 1.0f);
	models[3]->transform.scale = XMVectorSet(1.5f, 1.5f, 1.5f, 1.0f);
	lineModels[0]->transform.scale = XMVectorSet(10.f, 10.f, 10.f, 1.f);

	lineModels[0]->vs = g_VS;
	lineModels[0]->ps = g_PS_Solid;

	for (int i = 0; i < models.size(); i++)
	{
		switch (i)
		{
		case 3:
			models[i]->vs = g_VS;
			models[i]->ps = g_Reflection_PS;
			break;
		default:
			models[i]->vs = g_VS;
			models[i]->ps = g_PS;

		}
	}

	skybox = loadObj.CreateModelBuffer(g_Device,loadObj.CreateSphere(g_RotationMatrix, 10, 10), L"SkyboxOcean.dds");
	skybox->vs = sphere_VS;
	skybox->ps = sphere_PS;
	XMVECTOR Scale = XMVectorSet(50.0f, 50.0f, 50.0f, 1.0f);


	skybox->transform.scale = Scale;
	skybox->transform.rotation = XMQuaternionIdentity();

	D3D11_BUFFER_DESC bd = {};

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBufferPerObject);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_Device->CreateBuffer(&bd, nullptr, &g_cbPerObjBuffer);

	//create buffer for cbpf

	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBufferPerObject);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	hr = g_Device->CreateBuffer(&bd, nullptr, &g_cbPFbuffer);
	if (FAILED(hr))
		return hr;
	//cube map

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

	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = g_Device->CreateRasterizerState(&cmdesc, &RSCullNone);
	if (FAILED(hr))
		return hr;
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = g_Device->CreateDepthStencilState(&dssDesc, &DSLessEqual);
	if (FAILED(hr))
		return hr;
	// Initialize the world matrices
	World = XMMatrixIdentity();

	// Initialize the view matrix
	camera.transform.pos = XMVectorSet(0.0f, 5.0f, -8.0f, 0.0f);
	camera.transform.rotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(20.0f), 0.0f, 0.0f);

	// Initialize the projection matrix
	camera.projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);

	return S_OK;
}
HRESULT InitInput(HINSTANCE hInstance)
{
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, nullptr);
	if (FAILED(hr))
		return hr;
	hr = DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, nullptr);
	if (FAILED(hr))
		return hr;
	hr = DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, nullptr);
	if (FAILED(hr))
		return hr;
	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		return hr;
	hr = DIKeyboard->SetCooperativeLevel(g_hwindow, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		return hr;
	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
		return hr;
	hr = DIMouse->SetCooperativeLevel(g_hwindow, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_BACKGROUND);
	if (FAILED(hr))
		return hr;
	return hr;
}
void CreateVertexShader( LPCWSTR srcFile, LPCSTR entryPoint, LPCSTR profile, ID3D11VertexShader* vs)
{
	ID3DBlob* VSBlob = nullptr;
	//UINT numElements = ARRAYSIZE(layout1);
	/*hr = g_Device->CreateInputLayout(layout1, numElements, VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(), &g_vertLayout);
	VSBlob->Release();*/
	

	// Set the input layout
	g_DevContext->IASetInputLayout(g_vertLayout);
	//vertex shader
	VSBlob = nullptr;
	hr = CompileShader(srcFile, entryPoint, profile, &VSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			"The VS.hlsl file cannot be compiled.  Please run this executable from the directory that contains the hlsl file.", "Error", MB_OK);
		return;
	}

	hr = g_Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &vs);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			"The VS.hlsl file cannot be compiled.  Please run this executable from the directory that contains the hlsl file.", "Error", MB_OK);
		return;
	}
	VSBlob->Release();

}

void CreatePixelShader(LPCWSTR srcFile, LPCSTR entryPoint, LPCSTR profile, ID3D11PixelShader* ps)
{
	ID3DBlob* PSBlob = nullptr;
	//vertex shader
	hr = CompileShader(srcFile, entryPoint, profile, &PSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			"The PS.hlsl file cannot be compiled.  Please run this executable from the directory that contains the hlsl file.", "Error", MB_OK);
		return;
	}


	hr = g_Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &ps);

	PSBlob->Release();

}

float Cam_x = 0.0f;
float Cam_y = 0.0f;
void UpdateCamera()
{
	camera.transform.pos += xValue * camera.transform.Right();
	camera.transform.pos += zValue * camera.transform.Forward();

	XMVECTOR pitch = XMQuaternionRotationAxis(camera.transform.Right(), Cam_y);
	XMVECTOR yaw = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), Cam_x);;
	camera.transform.rotation = XMQuaternionMultiply(camera.transform.rotation, XMQuaternionMultiply(pitch, yaw));
	Cam_x = 0.0f;
	Cam_y = 0.0f;
}
void GetKey(double time)
{
	DIMOUSESTATE Curr_MouseState;
	BYTE keyState[256];

	hr = DIKeyboard->Acquire();
	if (FAILED(hr))
	{
		MessageBox(0, "Get keyboard Acquire- Failed",
			"Error", MB_OK);
		return;
	}
	hr = DIMouse->Acquire();
	if (FAILED(hr))
	{
		MessageBox(0, "Get mouse Acquire - Failed",
			"Error", MB_OK);
		return;
	}
	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &Curr_MouseState);
	DIKeyboard->GetDeviceState(sizeof(keyState), (LPVOID)&keyState);

	float speed = 5.0f * time;

	xValue = 0.f;
	zValue = 0.0f;

	if (keyState[DIK_A] & 0x80)
		xValue -= speed;
	if (keyState[DIK_D] & 0x80)
		xValue += speed;
	if (keyState[DIK_W] & 0x80)
		zValue += speed;
	if (keyState[DIK_S] & 0x80)
		zValue -= speed;
	if ((Curr_MouseState.lX != g_MouseState.lX) || (Curr_MouseState.lY != g_MouseState.lY))
	{
		Cam_x += Curr_MouseState.lX * 0.01f;

		Cam_y += Curr_MouseState.lY *0.01f;

		g_MouseState = Curr_MouseState;
	}
}



bool LoadHeightMap(char* filename, HeightMap &heightmap)
{
	FILE *filePtr;                            // Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;        // Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;        // Structure which stores information about image
	int imageSize, index;
	unsigned char height;

	filePtr = fopen(filename, "");
	if (filePtr == nullptr)
		return 0;
	//read file header
	fread(&bitmapFileHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	//read info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	heightmap.width = bitmapInfoHeader.biWidth;
	heightmap.height = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = heightmap.width * heightmap.height * 3;

	// Initialize the array which stores the image data
	unsigned char* bitmapImage = new unsigned char[imageSize];

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);

	// Initialize the heightMap array (stores the vertices of our terrain)
	heightmap.heightMap = new XMFLOAT3[heightmap.width * heightmap.height];

	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k = 0;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.
	float heightFactor = 10.0f;

	// Read the image data into our heightMap array
	for (int j = 0; j < heightmap.height; j++)
	{
		for (int i = 0; i < heightmap.width; i++)
		{
			height = bitmapImage[k];

			index = (heightmap.height * j) + i;

			heightmap.heightMap[index].x = (float)i;
			heightmap.heightMap[index].y = (float)height / heightFactor;
			heightmap.heightMap[index].z = (float)j;

			k += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	return true;

}


void CleanUp()
{
	//Release the COM Objects we created
	if (g_SwapChain)g_SwapChain->Release();
	if (g_Device)g_Device->Release();
	if (g_DevContext)g_DevContext->Release();
	if (g_rtv)g_rtv->Release();
	if (g_vertBuffer)g_vertBuffer->Release();
	if (g_indexBUffer)g_indexBUffer->Release();
	if (g_VS)g_VS->Release();
	if (g_PS)g_PS->Release();
	if (g_vertLayout)g_vertLayout->Release();
	if (g_depthStencilView)g_depthStencilView->Release();
	if (g_depthStencilBuffer)g_depthStencilBuffer->Release();
	if (g_cbPerObjBuffer)g_cbPerObjBuffer->Release();
	if (Transparency)Transparency->Release();
	if (CCWcullMode)CCWcullMode->Release();
	if (CWcullMode)CWcullMode->Release();
	if (g_Reflection_PS) g_Reflection_PS->Release();
	if (g_SwapChain1)g_SwapChain1->Release();
	if (g_Device1)g_Device1->Release();
	if (g_DevContext1)g_DevContext1->Release();
	///////////////**************new**************////////////////////
	if (g_cbPFbuffer)g_cbPFbuffer->Release();
	//////////////**************new**************////////////////////
	if (g_PS_Solid)g_PS_Solid->Release();
	if (g_SamplerState)g_SamplerState->Release();

	for (int i = 0; i < models.size(); i++)
	{
		if (models[i])
		{
			if (models[i]->IndexBuffer)
				models[i]->IndexBuffer->Release();
			if (models[i]->VertBuffer)
				models[i]->VertBuffer->Release();
			if (models[i]->srv)
				models[i]->srv->Release();
			delete models[i];
		}
	}

	for (int i = 0; i < lineModels.size(); i++)
	{
		if (lineModels[i])
		{
			if (lineModels[i]->IndexBuffer)
				lineModels[i]->IndexBuffer->Release();
			if (lineModels[i]->VertBuffer)
				lineModels[i]->VertBuffer->Release();
			if (lineModels[i]->srv)
				lineModels[i]->srv->Release();
			delete lineModels[i];
		}
	}
	if (DIKeyboard)DIKeyboard->Unacquire();
	if (DIMouse)DIMouse->Unacquire();
	if (DirectInput)DirectInput->Release();

	//skybox cleanup
	if (sphere_PS)sphere_PS->Release();
	if (sphere_VS)sphere_VS->Release();
	if (DSLessEqual)DSLessEqual->Release();
	if (RSCullNone)RSCullNone->Release();
	if (skybox)
	{
		if (skybox->IndexBuffer)skybox->IndexBuffer->Release();
		if (skybox->VertBuffer)skybox->VertBuffer->Release();
		if (skybox->srv)skybox->srv->Release();
		delete skybox;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// Note that this tutorial does not handle resizing (WM_SIZE) requests,
		// so we created the window without the resize border.

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

void Render()
{
	// Update our time

	timer.Signal();

	GetKey(timer.Delta());


	//camera imformation
	UpdateCamera();
	camera.projection = XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)s_width / s_height, 1.0f, 1000.0f);

	// Rotate the axe around the origin
	{
		XMVECTOR rot = XMQuaternionRotationRollPitchYaw(0.0f, 1.0f*timer.Delta(), 0.0f);
		models[0]->transform.rotation = XMQuaternionMultiply(models[0]->transform.rotation, rot);
	}
	// Clear the back buffer
	g_DevContext->ClearRenderTargetView(g_rtv, Colors::MidnightBlue);

	// Clear the depth buffer to 1.0 (max depth)
	g_DevContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Update matrix variables and lighting variables
	//directional light
	Dirlight.dir = XMFLOAT3(0.25f, -0.5f, 1.0f);
	Dirlight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	Dirlight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


	//Point Light
	Ptlight.range = 6.0f;
	Ptlight.diffuse = XMFLOAT4(0.0f, 4.0f, 0.0f, 1.0f);

	//update pointlight position
	//-9.0f, 1.0f, 5.0f, 0.0f
	XMVECTOR LightVec = XMVectorSet(-7.0f, 1.0f, 8.0f, 0.0f);
	XMStoreFloat3(&Ptlight.pos, LightVec);


	//Spot Light 
	StLight.pos = XMFLOAT3(3.0f, 3.0f, 0.0f);
	StLight.dir = XMFLOAT3(-3.0f, -3.0f, 0.0f);
	StLight.range = 20.0f;
	StLight.InConeRatio = XMConvertToRadians(10.0f);
	StLight.OutConeRatio = XMConvertToRadians(30.0f);

	StLight.diffuse = XMFLOAT4(3.0f, 0.0f, 0.0f, 1.0f);


	constBufferPF.directLight = Dirlight;
	constBufferPF.ptLight = Ptlight;
	constBufferPF.stLight = StLight;
	constBufferPF.time = timer.TotalTime();
	g_DevContext->UpdateSubresource(g_cbPFbuffer, 0, nullptr, &constBufferPF, 0, 0);
	XMFLOAT4 setColor;

	CBufferPerObject cb1;
	setColor = XMFLOAT4(0, 0, 0, 0);
	for (int i = 0; i < models.size(); i++)
	{

		loadObj.RenderObject(g_DevContext,camera,constBufferPF,g_SamplerState,models[i], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_cbPerObjBuffer, g_cbPFbuffer, setColor);
	}


	for (int i = 0; i < lineModels.size(); i++)
	{
		setColor = XMFLOAT4(0, 0, 0, 0);
		loadObj.RenderObject(g_DevContext, camera, constBufferPF, g_SamplerState, lineModels[i], D3D11_PRIMITIVE_TOPOLOGY_LINELIST, g_cbPerObjBuffer, g_cbPFbuffer, setColor);
	}

	skybox->transform.pos = camera.transform.pos;
	g_DevContext->OMSetDepthStencilState(DSLessEqual, 0);
	g_DevContext->RSSetState(RSCullNone);
	loadObj.RenderObject(g_DevContext, camera, constBufferPF, g_SamplerState, skybox, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_cbPerObjBuffer, g_cbPFbuffer, setColor);

	// Present our back buffer to our front buffer
	g_SwapChain->Present(0, 0);
}