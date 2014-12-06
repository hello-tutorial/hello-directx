//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: silhouetteEdges.h
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Generates the silhouette geometry of a mesh and renders it.  Note
//       that we assume mesh vertex formats as described in MeshVertex.
//      
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __silhouetteEdgesH__
#define __silhouetteEdgesH__

#include "d3dUtility.h"

struct EdgeVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 faceNormal1;
	D3DXVECTOR3 faceNormal2;
};

struct MeshVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	static const DWORD FVF;
};

class SilhouetteEdges
{
public:
	SilhouetteEdges(
		IDirect3DDevice9* device,
		ID3DXMesh* mesh, 
		ID3DXBuffer* adjBuffer);

	~SilhouetteEdges();

	void render();

private:
	IDirect3DDevice9*            _device;
	IDirect3DVertexBuffer9*      _vb;
	IDirect3DIndexBuffer9*       _ib;
	IDirect3DVertexDeclaration9* _decl;

	UINT _numVerts;
	UINT _numFaces;

private:

	bool createVertexDeclaration();

	void getFaceNormal(
		ID3DXMesh* mesh,
		DWORD faceIndex,
		D3DXVECTOR3* faceNormal);

	void getFaceNormals(
		ID3DXMesh* mesh,
		ID3DXBuffer* adjBuffer,
		D3DXVECTOR3* currentFaceNormal,
		D3DXVECTOR3 adjFaceNormals[3],
		DWORD       faceIndex);

	void genEdgeVertices(
		ID3DXMesh* mesh,
		ID3DXBuffer* adjBuffer);

	void genEdgeIndices(
		ID3DXMesh* mesh);	
};
#endif // __silhouetteEdgesH__