//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: transform.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Deomstrates transforming an object by the view and projection matrices,
//       and setting the vertex color using a vertex shader.  You will have to
//       switch to the REF device if your hardware does not support shaders.  Or you
//       can use software vertex processing: D3DCREATE_SOFTWARE_VERTEXPROCESSING.
//    
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

IDirect3DVertexShader9* TransformShader    = 0;
ID3DXConstantTable* TransformConstantTable = 0;

ID3DXMesh* Teapot = 0;

D3DXHANDLE TransformViewProjHandle = 0;

D3DXMATRIX ProjMatrix;

//
// Framework functions
//
bool Setup()
{
	HRESULT hr = 0;

	//
	// Create geometry.
	//

	D3DXCreateTeapot(Device, &Teapot, 0);

	//
	// Compile shader.
	//

	ID3DXBuffer* shader      = 0;
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCompileShaderFromFile(
		"transform.txt",
		0,
		0,
		"Main",  // entry point function name
		"vs_1_1",// shader version to compile to
		D3DXSHADER_DEBUG, 
		&shader,
		&errorBuffer,
		&TransformConstantTable);

	// output any error messages
	if( errorBuffer )
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(errorBuffer);
	}

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	hr = Device->CreateVertexShader(
		(DWORD*)shader->GetBufferPointer(),
		&TransformShader);

	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	d3d::Release<ID3DXBuffer*>(shader);

	// 
	// Get Handles.
	//

	TransformViewProjHandle = TransformConstantTable->GetConstantByName(0, "ViewProjMatrix");

	//
	// Set shader constants:
	//

	TransformConstantTable->SetDefaults(Device);
 
	//
	// Set Projection Matrix.
	//

	D3DXMatrixPerspectiveFovLH(
			&ProjMatrix, D3DX_PI * 0.25f, 
			(float)Width / (float)Height, 1.0f, 1000.0f);

	//
	// Set Misc. States.
	//

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXMesh*>(Teapot);
	d3d::Release<IDirect3DVertexShader9*>(TransformShader);
	d3d::Release<ID3DXConstantTable*>(TransformConstantTable);
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

		D3DXVECTOR3 position( cosf(angle) * 10.0f, height, sinf(angle) * 10.0f );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);

		// combine view and projection transformations
		D3DXMATRIX ViewProj = V * ProjMatrix;

		TransformConstantTable->SetMatrix(
			Device,
			TransformViewProjHandle, 
			&ViewProj);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		Device->SetVertexShader(TransformShader);

		Teapot->DrawSubset(0);
		
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


