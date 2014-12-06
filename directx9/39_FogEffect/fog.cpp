//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: fog.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Deomstrates fog using an effect file.  Use the arrow keys, 
//       and M, N, W, S, keys to move.
//        
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "terrain.h"
#include "camera.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

Terrain* TheTerrain      = 0;
Camera   TheCamera(Camera::AIRCRAFT);
ID3DXEffect* FogEffect   = 0;
D3DXHANDLE FogTechHandle = 0;

//
// Framework functions
//
bool Setup()
{
	HRESULT hr = 0;

	//
	// Init Scene. 
	//

	D3DXVECTOR3 lightDirection(0.0f, 1.0f, 0.0f);
	TheTerrain = new Terrain(Device, "coastMountain64.raw", 64, 64, 6, 0.5f);
	TheTerrain->genTexture(&lightDirection);

	//
	// Set texture filters.
	//

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//
	// Set Camera Position.
	//

	D3DXVECTOR3 pos(100.0f, 100.0f, -250.0f);
	TheCamera.setPosition(&pos);

	//
	// Create effect.
	//

	ID3DXBuffer* errorBuffer = 0;
	hr = D3DXCreateEffectFromFile(
		Device,
		"fog.txt",
		0,                // no preprocessor definitions
		0,                // no ID3DXInclude interface
		D3DXSHADER_DEBUG, // compile flags
		0,                // don't share parameters
		&FogEffect,
		&errorBuffer);

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

	FogTechHandle = FogEffect->GetTechniqueByName("Fog");
	
	//
	// Set Projection.
	//
	D3DXMATRIX P;

	D3DXMatrixPerspectiveFovLH(
		&P,	D3DX_PI * 0.25f, // 45 - degree
		(float)Width / (float)Height,
		1.0f, 1000.0f);

	Device->SetTransform(D3DTS_PROJECTION, &P);

	return true;
}

void Cleanup()
{
	d3d::Delete<Terrain*>(TheTerrain);
	d3d::Release<ID3DXEffect*>(FogEffect);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		// 
		// Update the scene: Allow user to rotate around scene.
		//
		
		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			TheCamera.walk(100.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			TheCamera.walk(-100.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			TheCamera.yaw(-1.0f * timeDelta);
		
		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			TheCamera.yaw(1.0f * timeDelta);

		if( ::GetAsyncKeyState('N') & 0x8000f )
			TheCamera.strafe(-100.0f * timeDelta);

		if( ::GetAsyncKeyState('M') & 0x8000f )
			TheCamera.strafe(100.0f * timeDelta);

		if( ::GetAsyncKeyState('W') & 0x8000f )
			TheCamera.pitch(1.0f * timeDelta);

		if( ::GetAsyncKeyState('S') & 0x8000f )
			TheCamera.pitch(-1.0f * timeDelta);

		D3DXMATRIX V;
		TheCamera.getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Activate the Technique and Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00CCCCCC, 1.0f, 0);
		Device->BeginScene();

		// set the technique to use
		FogEffect->SetTechnique( FogTechHandle );

		UINT numPasses = 0;
    	FogEffect->Begin(&numPasses, 0);

		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		for(int i = 0; i < numPasses; i++)
		{
			FogEffect->BeginPass(i);

			if( TheTerrain )
				TheTerrain->draw(&I, false);
			FogEffect->CommitChanges();
			FogEffect->EndPass();
		}
		FogEffect->End();

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


