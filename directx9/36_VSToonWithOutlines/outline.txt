////////////////////////////////////////////////////////////////////////////
// 
// File: diffuse.txt
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Vertex shader that draws the silhouette edges of a mesh.
//          
////////////////////////////////////////////////////////////////////////////

//
// Globals
//

extern matrix WorldViewMatrix;
extern matrix ProjMatrix;

static vector Black = {0.0f, 0.0f, 0.0f, 0.0f};

//
// Structures
//

struct VS_INPUT
{
    vector position    : POSITION;
    vector normal      : NORMAL0;
    vector faceNormal1 : NORMAL1; 
    vector faceNormal2 : NORMAL2;
};

struct VS_OUTPUT
{
    vector position : POSITION;
    vector diffuse  : COLOR;
};

//
// Main
//

VS_OUTPUT Main(VS_INPUT input)
{
    // zero out each member in output
    VS_OUTPUT output = (VS_OUTPUT)0;

    // transform position to view space
    input.position = mul(input.position, WorldViewMatrix);

    // Compute a vector in the direction of the vertex
    // from the eye.  Recall the eye is at the origin
    // in view space - eye is just camera position.
    vector eyeToVertex = input.position;

    // transform normals to view space.  Set w
    // components to zero since we're transforming vectors.
    // Assume there are no scalings in the world
    // matrix as well.
    input.normal.w      = 0.0f;
    input.faceNormal1.w = 0.0f;
    input.faceNormal2.w = 0.0f;

    input.normal      = mul(input.normal,      WorldViewMatrix);
    input.faceNormal1 = mul(input.faceNormal1, WorldViewMatrix);
    input.faceNormal2 = mul(input.faceNormal2, WorldViewMatrix);

    // compute the cosine of the angles between
    // the eyeToVertex vector and the face normals.
    float dot0 = dot(eyeToVertex, input.faceNormal1);
    float dot1 = dot(eyeToVertex, input.faceNormal2);

    // if cosines are different signs (positive/negative)
    // than we are on a silhouette edge.  Do the signs
    // differ?
    if( (dot0 * dot1) < 0.0f )
    {
         // yes, then this vertex is on a silhouette edge, 
         // offset the vertex position by some scalar in the
         // direction of the vertex normal.
         input.position += 0.1f * input.normal;
    }

    // transform to homogeneous clip space
    output.position = mul(input.position, ProjMatrix);

    // set outline color
    output.diffuse = Black;
    
    return output;
}
