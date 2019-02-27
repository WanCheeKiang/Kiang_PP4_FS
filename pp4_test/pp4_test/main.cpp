#include"includes.h"
#include"XTime.h"
#include"LoadObject.h"
#include"MathLib.h"
#include"Rendertexture.h"
//Global : Interface
ID3D11Device* g_Device = nullptr;
IDXGISwapChain* g_SwapChain = nullptr;
IDXGISwapChain* g_SwapChain_2nd = nullptr;
ID3D11DeviceContext* g_DevContext = nullptr;
ID3D11RenderTargetView* g_rtv = nullptr;
ID3D11DepthStencilView* g_depthStencilView;
ID3D11Texture2D* g_depthStencilBuffer;
ID3D11InputLayout* g_vertLayout;
//Shadder
ID3D11VertexShader* g_VS = nullptr;
ID3D11VertexShader* g_Wave_VS = nullptr;
ID3D11PixelShader* g_PS = nullptr;
ID3D11PixelShader*  g_PS_Solid = nullptr;
ID3D11PixelShader* g_Reflection_PS = nullptr;
ID3D11VertexShader* g_PostProcess_VS = nullptr;
ID3D11PixelShader* g_PostProcess_PS = nullptr;
ID3D11PixelShader* g_NoEffect_PS = nullptr;
ID3D11VertexShader* g_MultiTex_VS = nullptr;
ID3D11PixelShader*  g_MultiTex_PS = nullptr;
ID3D11PixelShader* g_Blend_PS = nullptr;
ID3D11PixelShader* g_Warp_PS = nullptr;
ID3D11PixelShader* g_Emissive_PS = nullptr;
ID3D11PixelShader* g_SubSerface_PS = nullptr;
//Buffer
ID3D11Buffer* g_indexBUffer = nullptr;
ID3D11Buffer* g_vertBuffer = nullptr;
ID3D11Buffer* g_cbPerObjBuffer = nullptr;
ID3D11Buffer* g_InstanceBuffer = nullptr;
//State
ID3D11BlendState* Transparency = nullptr;
ID3D11RasterizerState* CCWcullMode = nullptr;
ID3D11RasterizerState* CWcullMode = nullptr;
ID3D11SamplerState* g_SamplerState = nullptr;
ID3D11SamplerState* g_SamplerState1 = nullptr;

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
Rendertexture renTex;
Rendertexture renToTex;
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
void CreateVertexShader(LPCWSTR srcFile, LPCSTR entryPoint, LPCSTR profile, ID3D11VertexShader* vs, ID3D11InputLayout* inputLayout, D3D11_INPUT_ELEMENT_DESC layout);
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
vector<ModelBuffer*>BlendObj;
ModelBuffer* skybox;
//cube map
ID3D11VertexShader* sphere_VS = nullptr;
ID3D11PixelShader* sphere_PS = nullptr;
ID3D11DepthStencilState* DSLessEqual = nullptr;
ID3D11RasterizerState* RSCullNone = nullptr;
RotationMatrixs g_RotationMatrix;
XMVECTOR g_Up = { 0.0f,1.0f,0.0f,0.0f };
XMVECTOR g_Down = { 0.0f,-1.0f,0.0f,0.0f };
XMVECTOR camTarget;
//Draw Instance set up
ID3D11VertexShader* Instance_VS = nullptr;
//post process set up
ID3D11ShaderResourceView* g_srv = nullptr;
ID3D11Texture2D* g_DSBuffer = nullptr;
//trensparent set up
ID3D11RasterizerState* FrontCull = nullptr;
ID3D11RasterizerState* BackCull = nullptr;
ID3D11BlendState* g_BlendState_on = nullptr;
ID3D11BlendState* g_BlendState_off = nullptr;
ID3D11BlendState* g_BlendState_wf = nullptr;
ID3D11DepthStencilState* g_DSS_Blend;
ID3D11ShaderResourceView* textursrv = nullptr;
ID3D11ShaderResourceView* textursrv1 = nullptr;
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

	//create shaderresourse view

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	pBackBuffer->GetDesc(&textureDesc);
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;


	hr = g_Device->CreateRenderTargetView(pBackBuffer, nullptr, &g_rtv);

	renTex.CreateRenderTexture(g_Device, &textureDesc, nullptr);
	renToTex.CreateRenderTexture(g_Device, &textureDesc, nullptr);

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

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	ID3D11Texture2D* BackBuffer = nullptr;
	hr = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBuffer));


	if (FAILED(hr))
		return hr;


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



	pVSBlob = nullptr;

	hr = CompileShader(L"Instance_VS.hlsl", "main", "vs_4_0", &pVSBlob);
	hr = g_Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &Instance_VS);


	pVSBlob->Release();



	pVSBlob = nullptr;
	hr = CompileShader(L"WaveShader_VS.hlsl", "main", "vs_4_0", &pVSBlob);
	hr = g_Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_Wave_VS);
	pVSBlob->Release();
	pPSBlob = nullptr;
	hr = CompileShader(L"Reflection_PS.hlsl", "main", "ps_4_0", &pPSBlob);
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_Reflection_PS);
	pPSBlob->Release();
	// post process
	pVSBlob = nullptr;
	hr = CompileShader(L"PostProcess_VS.hlsl", "main", "vs_4_0", &pVSBlob);
	hr = g_Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_PostProcess_VS);
	pVSBlob->Release();
	pPSBlob = nullptr;
	hr = CompileShader(L"PostProcess_PS.hlsl", "main", "ps_4_0", &pPSBlob);
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_PostProcess_PS);
	pPSBlob->Release();
	pPSBlob = nullptr;
	hr = CompileShader(L"PostProcess_PS.hlsl", "NoEffect", "ps_4_0", &pPSBlob);
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_NoEffect_PS);
	pPSBlob->Release();

	pVSBlob = nullptr;
	hr = CompileShader(L"MultiTexture_VS.hlsl", "main", "vs_4_0", &pVSBlob);
	hr = g_Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_MultiTex_VS);
	pVSBlob->Release();

	pPSBlob = nullptr;
	hr = CompileShader(L"MultiTexture_PS.hlsl", "main", "ps_4_0", &pPSBlob);
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_MultiTex_PS);
	pPSBlob->Release();
	pPSBlob = nullptr;

	pPSBlob = nullptr;
	hr = CompileShader(L"Transparent_PS.hlsl", "main", "ps_4_0", &pPSBlob);
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_Blend_PS);
	pPSBlob->Release();
	pPSBlob = nullptr;
	
	pPSBlob = nullptr;
	hr = CompileShader(L"PostProcess_PS.hlsl", "warp", "ps_4_0", &pPSBlob);
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_Warp_PS);
	pPSBlob->Release();


	pPSBlob = nullptr;
	hr = CompileShader(L"Light_PS.hlsl", "emissive", "ps_4_0", &pPSBlob);
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_Emissive_PS);
	pPSBlob->Release();

	pPSBlob = nullptr;
	hr = CompileShader(L"SubSerface_PS.hlsl", "main", "ps_4_0", &pPSBlob);
	hr = g_Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_SubSerface_PS);
	pPSBlob->Release();

	models.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\rabbit.fbx", 3), L"Solid Object Assets\\fur.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.LoadObjBuffer(ChestData_Ind, ChestData_vert, Chest_data, Chest_indicies), L"TreasureChestTexture.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\wall.fbx", scale), L"Solid Object Assets\\stone_texture.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\sphere.fbx", scale), L"Castle.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\cube.fbx", 0.5), L"Solid Object Assets\\stone_texture.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\cube.fbx", 0.5), L"Box_Red2Dark.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\plane.fbx", 0.5), L"sun.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\cube.fbx", 0.5), L"LAVA_D.dds"));
	models.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\plane.fbx", 0.5), L"Monty1.dds"));
	/*models.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\sphere.fbx", scale), nullptr));*/
	BlendObj.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\plane.fbx", 0.5), L"Fire.dds"));
	BlendObj.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\plane.fbx", 0.5), L"Fire.dds"));
	BlendObj.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.ImportFbxModel("Solid Object Assets\\plane.fbx", 0.5), L"Fire.dds"));

	lineModels.push_back(loadObj.CreateModelBuffer(g_Device, loadObj.MakeGrid(15, 15), nullptr));

	models[0]->transform.scale = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
	models[0]->transform.pos = XMVectorSet(-2.0f, 0.0f, 0.0f, 1.0f);
	models[1]->transform.pos = XMVectorSet(3.0f, 0.0f, 0.0f, 1.0f);
	models[2]->transform.pos = XMVectorSet(0.2f, -2.0f, 0.0f, 1.0f);
	models[2]->transform.scale = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
	models[2]->transform.rotation = XMVectorSet(0.0f, 1.5f, 0.0f, 1.0f);
	models[3]->transform.pos = XMVectorSet(0.0f, 4.0f, 2.0f, 1.0f);
	models[3]->transform.scale = XMVectorSet(1.5f, 1.5f, 1.5f, 1.0f);
	models[4]->transform.pos = XMVectorSet(-8.0f, 1.0f, 0.0f, 1.0f);
	models[5]->transform.pos = XMVectorSet(-8.0f, 1.0f, -4.0f, 1.0f);
	models[5]->transform.pos = XMVectorSet(-8.0f, 1.0f, -4.0f, 1.0f);
	models[6]->transform.pos = XMVectorSet(3.0f, 1.0f, -1.5f, 1.0f);
	models[8]->transform.pos = XMVectorSet(-3.0f, 2.0f, -2.0f, 1.0f);
	//models[9]->transform.pos = XMVectorSet(-2.0f, 1.0f, -3.0f, 1.0f);

	lineModels[0]->transform.scale = XMVectorSet(10.f, 10.f, 10.f, 1.f);

	BlendObj[0]->transform.pos = XMVectorSet(-7.0f, 2.2f, 0.2f, 1.0f);
	BlendObj[1]->transform.pos = XMVectorSet(-7.0f, 2.2f, 1.0f, 1.0f);
	BlendObj[2]->transform.pos = XMVectorSet(-7.0f, 2.2f, 1.8f, 1.0f);

	lineModels[0]->vs = g_Wave_VS;
	lineModels[0]->ps = g_PS_Solid;

	for (int i = 0; i < models.size(); i++)
	{
		switch (i)
		{

		case 3:
			models[i]->vs = g_VS;
			models[i]->ps = g_Reflection_PS;
			break;
		case 0:
			models[i]->vs = Instance_VS;
			models[i]->ps = g_PS;
			break;

		case 4:
			models[i]->vs = g_VS;
			models[i]->ps = g_PS;
			models[i]->srv = renToTex.srv;
			renToTex.srv = nullptr;
			break;
		case 5:
			models[i]->vs = g_MultiTex_VS;
			models[i]->ps = g_MultiTex_PS;
			break;
		case 7:
			models[i]->vs = g_VS;
			models[i]->ps = g_Emissive_PS;
			break;
		/*case 9:
			models[i]->vs = g_VS;
			models[i]->ps = g_SubSerface_PS;
			break;*/
		default:
			models[i]->vs = g_VS;
			models[i]->ps = g_PS;
			break;

		}
	}

	for (int i = 0; i < BlendObj.size(); i++)
	{
		BlendObj[i]->vs = g_VS;
		BlendObj[i]->ps = g_Blend_PS;
	}
	skybox = loadObj.CreateModelBuffer(g_Device, loadObj.CreateSphere(g_RotationMatrix, 10, 10), L"Castle.dds");
	skybox->vs = sphere_VS;
	skybox->ps = sphere_PS;
	XMVECTOR Scale = XMVectorSet(50.0f, 50.0f, 50.0f, 1.0f);


	skybox->transform.scale = Scale;
	skybox->transform.rotation = XMQuaternionIdentity();

	//set up
	D3D11_DEPTH_STENCIL_DESC DescDSS;
	ZeroMemory(&DescDSS, sizeof(D3D11_DEPTH_STENCIL_DESC));
	DescDSS.DepthEnable = true;
	DescDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DescDSS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = g_Device->CreateDepthStencilState(&DescDSS, &g_DSS_Blend);
	if (FAILED(hr))
		return hr;

	CD3D11_BLEND_DESC BlendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = g_Device->CreateBlendState(&BlendDesc, &g_BlendState_on);

	BlendDesc.RenderTarget[0].BlendEnable = FALSE;
	hr = g_Device->CreateBlendState(&BlendDesc, &g_BlendState_off);

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


	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(InstanceObject);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	hr = g_Device->CreateBuffer(&bd, nullptr, &g_InstanceBuffer);
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

	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_Device->CreateSamplerState(&SamplerDesc, &g_SamplerState1);
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

	Dirlight.transform.rotation = XMQuaternionRotationRollPitchYaw(1.0f, 1.5f, 0.0f);
	StLight.transform.rotation = XMQuaternionRotationRollPitchYaw(1.0f, 1.5f, 0.0f);
#ifdef _DEBUG
	ID3D11Debug* DebugDevice = nullptr;
	HRESULT Result = g_Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&DebugDevice));
	//Result = DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	DebugDevice->Release();
#endif


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
void CreateVertexShader(LPCWSTR srcFile, LPCSTR entryPoint, LPCSTR profile, ID3D11VertexShader* vs, ID3D11InputLayout* inputLayout, D3D11_INPUT_ELEMENT_DESC layout)
{
	ID3DBlob* VSBlob = nullptr;
	/*UINT numElements = ARRAYSIZE(layout);
	hr = g_Device->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(), &inputLayout);
	VSBlob->Release();*/

	// Set the input layout
	g_DevContext->IASetInputLayout(inputLayout);
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
	float up_angle = XMVectorGetX(XMVector3AngleBetweenVectors(g_Up, camera.transform.Forward()));
	float down_angle = XMVectorGetX(XMVector3AngleBetweenVectors(g_Down, camera.transform.Forward()));
	up_angle = XMConvertToDegrees(up_angle);
	down_angle = XMConvertToDegrees(down_angle);

	if (up_angle < 20.0f)
		Cam_y = MathLib::Clamp(0, abs(Cam_y), Cam_y);
	if (down_angle < 20.0f)
		Cam_y = MathLib::Clamp(-abs(Cam_y), 0, Cam_y);
	XMVECTOR pitch = XMQuaternionRotationAxis(camera.transform.Right(), Cam_y);
	XMVECTOR yaw = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), Cam_x);

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
	if (g_SamplerState1)g_SamplerState1->Release();
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

	for (int i = 0; i < BlendObj.size(); i++)
	{
		if (BlendObj[i])
		{
			if (BlendObj[i]->IndexBuffer)
				BlendObj[i]->IndexBuffer->Release();
			if (BlendObj[i]->VertBuffer)
				BlendObj[i]->VertBuffer->Release();
			if (BlendObj[i]->srv)
				BlendObj[i]->srv->Release();
			delete BlendObj[i];
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

	if (g_Wave_VS)g_Wave_VS->Release();
	if (g_SwapChain_2nd)g_SwapChain_2nd->Release();
	if (Instance_VS)Instance_VS->Release();

	if (g_InstanceBuffer) g_InstanceBuffer->Release();


	if (g_srv)g_srv->Release();
	if (g_DSBuffer)g_DSBuffer->Release();
	if (g_PostProcess_VS)g_PostProcess_VS->Release();
	if (g_PostProcess_PS)g_PostProcess_PS->Release();
	renTex.TextureCleanUp();
	renToTex.TextureCleanUp();
	if (g_NoEffect_PS)g_NoEffect_PS->Release();
	if (g_MultiTex_VS)g_MultiTex_VS->Release();
	if (g_MultiTex_PS)g_MultiTex_PS->Release();
	//trensparent release
	if (FrontCull)FrontCull->Release();
	if (BackCull)BackCull->Release();
	if (g_BlendState_on)g_BlendState_on->Release();
	if (g_BlendState_off)g_BlendState_off->Release();
	if (g_BlendState_wf)g_BlendState_wf->Release();

	if(g_Warp_PS)g_Warp_PS->Release();
	if (g_Emissive_PS)g_Emissive_PS->Release();
	if (textursrv)textursrv->Release();
	if (textursrv1)textursrv1->Release();
	if (g_SubSerface_PS)g_SubSerface_PS->Release();
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

	BYTE keyinput[256];
	DIKeyboard->GetDeviceState(sizeof(keyinput), (LPVOID)&keyinput);


	timer.Signal();

	GetKey(timer.Delta());

	camTarget = models[0]->transform.pos;
	//camera imformation
	UpdateCamera();
	camera.projection = XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)s_width / s_height, 1.0f, 1000.0f);
	//camera.projection = XMMatrixLookAtLH(camera.transform.pos, camTarget, g_Up);

	// Rotate the axe around the origin
	{
		XMVECTOR rot = XMQuaternionRotationRollPitchYaw(0.0f, 1.0f*timer.Delta(), 0.0f);
		models[0]->transform.rotation = XMQuaternionMultiply(models[0]->transform.rotation, rot);
	}

	// for the post processing rendering 
	g_DevContext->ClearRenderTargetView(renTex.rtv, Colors::Green);

	g_DevContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Update matrix variables and lighting variables
	//directional light
	XMVECTOR rot = XMQuaternionRotationRollPitchYaw(0.0f, 1.0f*timer.Delta(), 0.0f);
	Dirlight.transform.rotation = XMQuaternionMultiply(Dirlight.transform.rotation, rot);

	XMStoreFloat3(&Dirlight.DirLight.dir, Dirlight.transform.Forward());
	Dirlight.DirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	Dirlight.DirLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//Point Light

	Ptlight.transform.pos = XMVectorSet(-7.0f, 1.0f + sin(timer.TotalTime()), 8.0f, 1.0f);
	XMStoreFloat3(&Ptlight.PtLight.pos, Ptlight.transform.pos);
	Ptlight.PtLight.range = 6.0f;
	Ptlight.PtLight.diffuse = XMFLOAT4(0.0f, 4.0f, 0.0f, 1.0f);



	//Spot Light 
	StLight.transform.pos = XMVectorSet(3.0f + sin(timer.TotalTime() * 3), 3.0f, 3.0f, 1.0f);
	XMStoreFloat3(&StLight.StLight.pos, StLight.transform.pos);
	//StLight.StLight.dir = XMFLOAT3(-3.0f, -3.0f, 0.0f);
	XMVECTOR StLightRot = XMQuaternionRotationRollPitchYaw(0.0f, -1.0f*timer.Delta(), 0.0f);
	StLight.transform.rotation = XMQuaternionMultiply(StLight.transform.rotation, StLightRot);

	XMStoreFloat3(&StLight.StLight.dir, StLight.transform.Forward());

	StLight.StLight.range = 20.0f;
	StLight.StLight.InConeRatio = XMConvertToRadians(10.0f);
	StLight.StLight.OutConeRatio = XMConvertToRadians(30.0f);

	StLight.StLight.diffuse = XMFLOAT4(3.0f, 0.0f, 0.0f, 1.0f);


	constBufferPF.directLight = Dirlight.DirLight;
	constBufferPF.ptLight = Ptlight.PtLight;
	constBufferPF.stLight = StLight.StLight;
	constBufferPF.time = timer.TotalTime();
	g_DevContext->UpdateSubresource(g_cbPFbuffer, 0, nullptr, &constBufferPF, 0, 0);
	XMFLOAT4 setColor;

	CBufferPerObject cb1;
	setColor = XMFLOAT4(0, 0, 0, 0);

	g_DevContext->OMSetRenderTargets(1, &renTex.rtv, g_depthStencilView);

	UINT InstanceCount = 4;
	InstanceObject InstanceObj;
	for (int i = 0; i < models.size(); i++)
	{
		switch (i)
		{
		case 0:
		{
			UINT stride = sizeof(Vertex);
			UINT offset = 0;

			g_DevContext->IASetIndexBuffer(models[i]->IndexBuffer, DXGI_FORMAT_R32_UINT, offset);
			g_DevContext->IASetVertexBuffers(0, 1, &models[i]->VertBuffer, &stride, &offset);

			g_DevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			srand(time(0));
			float randX;
			float randZ;
			for (int j = 0; j < 4; j++)
			{
				randX = rand() % 10;
				randZ = rand() % 10;
				XMVECTOR tempPos;
				tempPos = XMVectorSet(randX, 0.0f, randZ, 0.0f);

				models[i]->transform.pos = tempPos;

				InstanceObj.mWorld[j] = XMMatrixTranspose(models[i]->transform.createMatrix());
			}
			InstanceObj.mView = XMMatrixTranspose(camera.View());
			InstanceObj.mProjection = XMMatrixTranspose(camera.projection);

			XMStoreFloat4(&constBufferPF.CameraPos, camera.transform.pos);

			g_DevContext->UpdateSubresource(g_InstanceBuffer, 0, nullptr, &InstanceObj, 0, 0);
			g_DevContext->UpdateSubresource(g_cbPFbuffer, 0, nullptr, &constBufferPF, 0, 0);

			g_DevContext->VSSetShader(models[i]->vs, nullptr, 0);
			g_DevContext->PSSetShader(models[i]->ps, nullptr, 0);
			g_DevContext->VSSetConstantBuffers(0, 1, &g_InstanceBuffer);
			g_DevContext->PSSetConstantBuffers(0, 1, &g_cbPerObjBuffer);
			g_DevContext->VSSetConstantBuffers(1, 1, &g_cbPFbuffer);
			g_DevContext->PSSetConstantBuffers(1, 1, &g_cbPFbuffer);

			g_DevContext->PSSetSamplers(0, 1, &g_SamplerState);
			g_DevContext->PSSetShaderResources(0, 1, &models[i]->srv);

			g_DevContext->RSSetState(RSCullNone);
			g_DevContext->DrawIndexedInstanced(models[i]->indexCount, InstanceCount, 0, 0, 0);
			break;
		}
		case 5:
			loadObj.MultiTexture(L"cobblestone.dds", g_Device);
			g_DevContext->PSSetShaderResources(1, 1, &loadObj.textures_srv);
			loadObj.RenderObject(g_DevContext, camera, constBufferPF, g_SamplerState, models[i], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_cbPerObjBuffer, g_cbPFbuffer, setColor);
			break;
		case 7:
			loadObj.MultiTexture(L"LAVA_E.dds", g_Device);
			g_DevContext->PSSetShaderResources(1, 1, &loadObj.textures_srv);
			loadObj.RenderObject(g_DevContext, camera, constBufferPF, g_SamplerState, models[i], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_cbPerObjBuffer, g_cbPFbuffer, setColor);
			break;
		default:
			loadObj.RenderObject(g_DevContext, camera, constBufferPF, g_SamplerState, models[i], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_cbPerObjBuffer, g_cbPFbuffer, setColor);
			break;
		}

	}


	for (int i = 0; i < lineModels.size(); i++)
	{
		setColor = XMFLOAT4(0, 0, 0, 0);
		loadObj.RenderObject(g_DevContext, camera, constBufferPF, g_SamplerState, lineModels[i], D3D11_PRIMITIVE_TOPOLOGY_LINELIST, g_cbPerObjBuffer, g_cbPFbuffer, setColor);
	}

	const FLOAT BlendColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	{
		int i, j;
		for (i = 0; i < BlendObj.size() - 1; i++)
		{
			// Last i elements are already in place    
			for (j = 0; j < BlendObj.size() - i - 1; j++)
			{
				
				float d1 = XMVectorGetZ(XMVector3Length( BlendObj[j]->transform.pos - camera.transform.pos));
				float d2 = XMVectorGetZ(XMVector3Length(BlendObj[j + 1]->transform.pos - camera.transform.pos));
				if (d2 > d1)
					swap(BlendObj[j], BlendObj[j + 1]);
			}
		}
	}


	skybox->transform.pos = camera.transform.pos;
	g_DevContext->OMSetDepthStencilState(DSLessEqual, 0);
	g_DevContext->RSSetState(RSCullNone);
	loadObj.RenderObject(g_DevContext, camera, constBufferPF, g_SamplerState, skybox, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_cbPerObjBuffer, g_cbPFbuffer, setColor);

	for (int i = 0; i < BlendObj.size(); i++)
	{
		g_DevContext->OMSetDepthStencilState(g_DSS_Blend, 0);
		g_DevContext->OMSetBlendState(g_BlendState_on, BlendColor, 0xFFFFFFFF);
		loadObj.RenderObject(g_DevContext, camera, constBufferPF, g_SamplerState, BlendObj[i], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_cbPerObjBuffer, g_cbPFbuffer, setColor);
		g_DevContext->OMSetDepthStencilState(DSLessEqual, 0);

	}
	g_DevContext->OMSetBlendState(g_BlendState_off, BlendColor, 0xFFFFFFFF);


	g_DevContext->CopyResource(renToTex.Texture2D, renTex.Texture2D);

	// Clear the back buffer
	g_DevContext->ClearRenderTargetView(g_rtv, Colors::MidnightBlue);
	// draw call
	g_DevContext->OMSetRenderTargets(1, &g_rtv, nullptr);
	g_DevContext->VSSetShader(g_PostProcess_VS, 0, 0);
	if (GetAsyncKeyState('1'))
	{
		g_DevContext->PSSetShader(g_PostProcess_PS, 0, 0);
	}
	else if (GetAsyncKeyState('2'))
	{
		g_DevContext->PSSetConstantBuffers(0, 1, &g_cbPFbuffer);
		g_DevContext->CSSetSamplers(0, 1, &g_SamplerState1);
		g_DevContext->PSSetShader(g_Warp_PS, 0, 0);
	}
	else
		g_DevContext->PSSetShader(g_NoEffect_PS, 0, 0);
	g_DevContext->PSSetShaderResources(0, 1, &renTex.srv);

	g_DevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_DevContext->Draw(4, 0);
	ID3D11ShaderResourceView* null[] = { nullptr };
	g_DevContext->PSSetShaderResources(0, 1, null);
	// Present our back buffer to our front buffer
	g_SwapChain->Present(0, 0);

}