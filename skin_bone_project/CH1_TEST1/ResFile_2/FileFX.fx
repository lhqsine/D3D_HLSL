//-----------------------------------------------------------------------------
// Skinned Mesh Effect file 
// ע��� fx �ļ�����Ҫ����任����͹۲����
// ����任�����Ѿ������ļ��н�����ı任���ǣ��۲�����Ѿ����������п���
//-----------------------------------------------------------------------------

texture g_ColorTexture;
// Matrix Pallette
static const int MAX_MATRICES = 26;
float4x3    g_mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;   // DrawMeshContainer ����
float4x4    g_matWorld;                                             // OnFrameMove
float4x4    g_matView;                                              // OnFrameMove
float4x4    g_matProject;                                           // OnFrameMove

sampler ColorTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

//----------------------------------------------------
struct VS_INPUT
  {
    float4  Pos             : POSITION;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;
    float3  Tex0            : TEXCOORD0;
  };

struct VS_OUTPUT
  {
    float4  Pos     : POSITION;
    float2  Tex0    : TEXCOORD0;
  };

VS_OUTPUT VShade( VS_INPUT Input, uniform int NumBones )
  {
    VS_OUTPUT   Output;
    float3      Pos = 0.0f;
    float       LastWeight = 0.0f;
    // ��� GF ��ʾ
    int4 IndexVector = D3DCOLORtoUBYTE4( Input.BlendIndices );
    // cast the vectors to arrays for use in the for loop below
    float BlendWeightsArray[ 4 ] = ( float[ 4 ])Input.BlendWeights;
    int   IndexArray[ 4 ]        = ( int[ 4 ])IndexVector;
    // ������
    for ( int iBone = 0; iBone < NumBones-1; iBone++ )
      {
        LastWeight = LastWeight + BlendWeightsArray[ iBone ];
        Pos += mul( Input.Pos, g_mWorldMatrixArray[ IndexArray[ iBone ]]) * BlendWeightsArray[ iBone ];
      }
    LastWeight = 1.0f - LastWeight; 
    // Now that we have the calculated weight, add in the final influence
    Pos += ( mul( Input.Pos, g_mWorldMatrixArray[ IndexArray[ NumBones - 1 ]]) * LastWeight );
    // �۲�ͶӰ����
    float4x4 matViewProject = mul( g_matView ,g_matProject );
    Output.Pos = mul( float4( Pos.xyz , 1.0f ) ,matViewProject );
    Output.Tex0  = Input.Tex0.xy;

    return Output;
  }

int CurNumBones = 2;
VertexShader vsArray[4] =   
  { 
    compile vs_1_1 VShade( 1 ), 
    compile vs_1_1 VShade( 2 ),
    compile vs_1_1 VShade( 3 ),
    compile vs_1_1 VShade( 4 )
  };

VS_OUTPUT MainVS(  VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Pos = mul( In.Pos ,matWorldViewProject );
    Out.Tex0 = In.Tex0.xy;
    return Out;
  }

float4 MainPS(  VS_OUTPUT In ):COLOR0
  {
    return tex2D( ColorTextureSampler ,In.Tex0 );
  }

//---------------------------------------
// Techniques specs follow
//---------------------------------------
technique t0
  {
    pass p0
      {
        
        //FillMode            = WireFrame;
        VertexShader = ( vsArray[ CurNumBones ]);
        //VertexShader = compile vs_2_0 MainVS( );
        PixelShader  = compile ps_2_0 MainPS( );
      }
  }

