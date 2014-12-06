//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: tooneffects.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Deomstrates toon rendering using a vertex shader in an effect file.  
//       You will need to switch to the REF device to run this sample if your
//       card does not support shaders.  Or you
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

ID3DXEffect* ToonEffect    = 0;

ID3DXMesh*  Meshes[4] = {0, 0, 0, 0};
D3DXMATRIX  WorldMatrices[4];
D3DXVECTOR4 MeshColors[4];

D3DXMATRIX ProjMatrix;

IDirect3DTexture9* ShadeTex  = 0;

D3DXHANDLE WorldViewHandle     = 0;
D3DXHANDLE WorldViewProjHandle = 0;
D3DXHANDLE ColorHandle         = 0;
D3DXHANDLE LightDirHandle      = 0;
D3DXHANDLE ShadeTexHandle      = 0;
D3DXHANDLE ToonTechHandle      = 0;

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

	D3DXCreateTeapot(Device, &Meshes[0], 0);
	D3DXCreateSphere(Device, 1.0f, 20, 20, &Meshes[1], 0);
	D3DXCreateTorus(Device, 0.5f, 1.0f, 20, 20, &Meshes[2], 0);
	D3DXCreateCylinder(Device, 0.5f, 0.5f, 2.0f, 20, 20, &Meshes[3], 0);

	D3DXMatrixTranslation(&WorldMatrices[0],  0.0f,  2.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[1],  0.0f, -2.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[2], -3.0f,  0.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[3],  3.0f,  0.0f, 0.0f);

	MeshColors[0] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	MeshColors[1] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	MeshColors[2] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	MeshColors[3] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);

	//
	// Create effects.
	//

	ID3DXBuffer* errorBuffer = 0;
	hr = D3DXCreateEffectFromFile(
		Device,           // associated device
		"tooneffect.txt", // effect filename
		0,                // no preprocessor definitions
		0,                // no ID3DXInclude interface
		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, // compile flags
		0,                // don't share parameters
		&ToonEffect,      // return effect
		&errorBuffer);    // return error messages

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

	// 
	// Save Frequently Accessed Parameter Handles
	//

	ToonTechHandle = ToonEffect->GetTechniqueByName("Toon");
	ShadeTexHandle = ToonEffect->GetParameterByName(0, "ShadeTex");
	WorldViewHandle = ToonEffect->GetParameterByName(0, "WorldViewMatrix");
	WorldViewProjHandle = ToonEffect->GetParameterByName(0, "WorldViewProjMatrix");
	ColorHandle    = ToonEffect->GetParameterByName(0, "Color");
	LightDirHandle = ToonEffect->GetParameterByName(0, "LightDirection");
	
	//
	// Set Effect Parameters
	//

	// Projection:
	D3DXMatrixPerspectiveFovLH(
		&ProjMatrix, D3DX_PI * 0.25f, // 45 - degree
		(float)Width / (float)Height,
		1.0f, 1000.0f);

	// Light direction:
	D3DXVECTOR4 lightDirection(-0.57f, 0.57f, -0.57f, 0.0f);
	ToonEffect->SetVector(LightDirHandle, &lightDirection);

	//
	// Set texture:
	IDirect3DTexture9* tex = 0;
	D3DXCreateTextureFromFile(Device, "toonshade.bmp", &tex);
	ToonEffect->SetTexture(ShadeTexHandle, tex);
	d3d::Release<IDirect3DTexture9*>(tex);

	return true;
}

void Cleanup()
{
	for(int i = 0; i < 4; i++)
	{
		d3d::Release<ID3DXMesh*>(Meshes[i]);
	}
	d3d::Release<IDirect3DTexture9*>(ShadeTex);

	d3d::Release<ID3DXEffect*>(ToonEffect);
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
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &position, &target, &up);

		//
		// Activate the Technique and Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		ToonEffect->SetTechnique( ToonTechHandle );

		UINT numPasses = 0;
    	ToonEffect->Begin(&numPasses, 0);

		D3DXMATRIX WorldView;
		D3DXMATRIX WorldViewProj;
		for(int i = 0; i < numPasses; i++)
		{
			ToonEffect->BeginPass(i);

			for(int j = 0; j < 4; j++)
			{
				WorldView     = WorldMatrices[j] * view;
				WorldViewProj = WorldMatrices[j] * view * ProjMatrix;

				ToonEffect->SetMatrix(WorldViewHandle, &WorldView);
				ToonEffect->SetMatrix(WorldViewProjHandle, &WorldViewProj);
				ToonEffect->SetVector(ColorHandle, &MeshColors[j]);
				Meshes[j]->DrawSubset(0);
			}
			ToonEffect->EndPass();
		}
		ToonEffect->End();

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


