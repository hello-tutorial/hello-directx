//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: toon.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates toon rendering with silhouette edges outlined using a
//       vertex shader.  Note that you will have to switch to the REF device 
//       to view this sample if your graphics card does not support vertex shaders.  Or you
//       can use software vertex processing: D3DCREATE_SOFTWARE_VERTEXPROCESSING.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "silhouetteEdges.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

IDirect3DVertexShader9* ToonShader = 0;
ID3DXConstantTable* ToonConstTable = 0;

ID3DXMesh*  Meshes[4] = {0, 0, 0, 0};
D3DXMATRIX  WorldMatrices[4];
D3DXVECTOR4 MeshColors[4];

D3DXMATRIX ProjMatrix;

IDirect3DTexture9* ShadeTex  = 0;

D3DXHANDLE ToonWorldViewHandle     = 0;
D3DXHANDLE ToonWorldViewProjHandle = 0;
D3DXHANDLE ToonColorHandle    = 0;
D3DXHANDLE ToonLightDirHandle = 0;

SilhouetteEdges* MeshOutlines[4] = {0, 0, 0, 0};
IDirect3DVertexShader9* OutlineShader = 0;
ID3DXConstantTable* OutlineConstTable = 0;

D3DXHANDLE OutlineWorldViewHandle = 0;
D3DXHANDLE OutlineProjHandle = 0;

//
// Framework functions
//
bool Setup()
{
	HRESULT hr = 0;

	//
	// Create geometry and compute corresponding world matrix and color
	// for each mesh.
	//

	ID3DXBuffer* adjBuffer[4] = {0, 0, 0, 0};

	D3DXCreateTeapot(Device, &Meshes[0], &adjBuffer[0]);
	D3DXCreateSphere(Device, 1.0f, 20, 20, &Meshes[1], &adjBuffer[1]);
	D3DXCreateTorus(Device, 0.5f, 1.0f, 20, 20, &Meshes[2], &adjBuffer[2]);
	D3DXCreateCylinder(Device, 0.5f, 0.5f, 2.0f, 20, 20, &Meshes[3], &adjBuffer[3]);

	D3DXMatrixTranslation(&WorldMatrices[0],  0.0f,  2.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[1],  0.0f, -2.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[2], -3.0f,  0.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[3],  3.0f,  0.0f, 0.0f);

	MeshColors[0] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	MeshColors[1] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	MeshColors[2] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	MeshColors[3] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);

	//
	// Allocate mesh outlines
	//

	MeshOutlines[0] = new SilhouetteEdges(Device, Meshes[0], adjBuffer[0]);
	MeshOutlines[1] = new SilhouetteEdges(Device, Meshes[1], adjBuffer[1]);
	MeshOutlines[2] = new SilhouetteEdges(Device, Meshes[2], adjBuffer[2]);
	MeshOutlines[3] = new SilhouetteEdges(Device, Meshes[3], adjBuffer[3]);

	d3d::Release<ID3DXBuffer*>(adjBuffer[0]);
	d3d::Release<ID3DXBuffer*>(adjBuffer[1]);
	d3d::Release<ID3DXBuffer*>(adjBuffer[2]);
	d3d::Release<ID3DXBuffer*>(adjBuffer[3]);

	//
	// Compile Toon Shader
	//

	ID3DXBuffer* toonCompiledCode = 0;
	ID3DXBuffer* toonErrorBuffer  = 0;

	hr = D3DXCompileShaderFromFile(
		"toon.txt",
		0,
		0,
		"Main", // entry point function name
		"vs_1_1",
		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, 
		&toonCompiledCode,
		&toonErrorBuffer,
		&ToonConstTable);

	// output any error messages
	if( toonErrorBuffer )
	{
		::MessageBox(0, (char*)toonErrorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(toonErrorBuffer);
	}

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}

	hr = Device->CreateVertexShader(
		(DWORD*)toonCompiledCode->GetBufferPointer(),
		&ToonShader);

	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	d3d::Release<ID3DXBuffer*>(toonCompiledCode);

	//
	// Compile Outline shader.
	//

	ID3DXBuffer* outlineCompiledCode = 0;
	ID3DXBuffer* outlineErrorBuffer  = 0;

	hr = D3DXCompileShaderFromFile(
		"outline.txt",
		0,
		0,
		"Main", // entry point function name
		"vs_1_1",
		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, 
		&outlineCompiledCode,
		&outlineErrorBuffer,
		&OutlineConstTable);

	// output any error messages
	if( outlineErrorBuffer )
	{
		::MessageBox(0, (char*)outlineErrorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(outlineErrorBuffer);
	}

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}

	hr = Device->CreateVertexShader(
		(DWORD*)outlineCompiledCode->GetBufferPointer(),
		&OutlineShader);

	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	d3d::Release<ID3DXBuffer*>(outlineCompiledCode);

	//
	// Load textures.
	//

	D3DXCreateTextureFromFile(Device, "toonshade.bmp", &ShadeTex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	// 
	// Get Handles
	//

	ToonWorldViewHandle     = ToonConstTable->GetConstantByName(0, "WorldViewMatrix");
	ToonWorldViewProjHandle = ToonConstTable->GetConstantByName(0, "WorldViewProjMatrix");
	ToonColorHandle         = ToonConstTable->GetConstantByName(0, "Color");
	ToonLightDirHandle      = ToonConstTable->GetConstantByName(0, "LightDirection");

	OutlineWorldViewHandle = OutlineConstTable->GetConstantByName(0, "WorldViewMatrix");
	OutlineProjHandle      = OutlineConstTable->GetConstantByName(0, "ProjMatrix");

	//
	// Set shader constants:
	//
	
	// Light direction:
	D3DXVECTOR4 directionToLight(-0.57f, 0.57f, -0.57f, 0.0f);

	ToonConstTable->SetVector(
		Device, 
		ToonLightDirHandle,
		&directionToLight);

	ToonConstTable->SetDefaults(Device);
	OutlineConstTable->SetDefaults(Device);

	//
	// Calculate projection matrix.
	//

	D3DXMatrixPerspectiveFovLH(
		&ProjMatrix, D3DX_PI * 0.25f, 
		(float)Width / (float)Height, 1.0f, 1000.0f);

	return true;
}

void Cleanup()
{
	for(int i = 0; i < 4; i++)
	{
		d3d::Release<ID3DXMesh*>(Meshes[i]);
		d3d::Delete<SilhouetteEdges*>(MeshOutlines[i]);
	}
	d3d::Release<IDirect3DTexture9*>(ShadeTex);
	d3d::Release<IDirect3DVertexShader9*>(ToonShader);
	d3d::Release<ID3DXConstantTable*>(ToonConstTable);
	d3d::Release<IDirect3DVertexShader9*>(OutlineShader);
	d3d::Release<ID3DXConstantTable*>(OutlineConstTable);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		// 
		// Update the scene: Allow user to rotate around scene.
		//
		
		static float angle  = (3.0f * D3DX_PI) / 2.0f;
		static float height = 5.0f;
	
		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			angle -= 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			angle += 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			height += 5.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			height -= 5.0f * timeDelta;

		D3DXVECTOR3 position( cosf(angle) * 7.0f, height, sinf(angle) * 7.0f );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &position, &target, &up);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		// Draw Cartoon
		Device->SetVertexShader(ToonShader);
		Device->SetTexture(0, ShadeTex);

		D3DXMATRIX worldView;
		D3DXMATRIX worldViewProj;
		for(int i = 0; i < 4; i++)
		{
			worldView = WorldMatrices[i] * view;
			worldViewProj = WorldMatrices[i] * view * ProjMatrix;

			ToonConstTable->SetMatrix(
				Device, 
				ToonWorldViewHandle,
				&worldView);

			ToonConstTable->SetMatrix(
				Device, 
				ToonWorldViewProjHandle,
				&worldViewProj);

			ToonConstTable->SetVector(
				Device,
				ToonColorHandle,
				&MeshColors[i]);

			Meshes[i]->DrawSubset(0);
		}

		// Draw Outlines.  
		Device->SetVertexShader(OutlineShader);
		Device->SetTexture(0, 0);

		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		for(int i = 0; i < 4; i++)
		{
			worldView = WorldMatrices[i] * view;

			OutlineConstTable->SetMatrix(
				Device, 
				OutlineWorldViewHandle,
				&worldView);

			OutlineConstTable->SetMatrix(
				Device, 
				OutlineProjHandle,
				&ProjMatrix);

			MeshOutlines[i]->render();
		}

		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}


