//--------------------------------------------------------------------------------------
// File: BasicHLSL.fx
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD0;  
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD0;  
    
  };

float4 Modify( float4 In )
  {
    // 在 X 方向移动
    float4 v4Out = In;
    //v4Out.x = In.x + 4.0 * sin( 2.0 * g_fTime );
    // 在 X = 2.0 处缩放，建立一个矩阵，与 x 的位置有关，即建立一个与 X 位置有关的函数
    float4x4 matOffset;
    matOffset._11 = matOffset._22 = matOffset._33 = 1.0;
    matOffset._41 = matOffset._42 = matOffset._43 = 0.0;
    //
    matOffset._12 = matOffset._13 = matOffset._14 = 0.0;
    matOffset._21 = matOffset._23 = matOffset._24 = 0.0;
    matOffset._31 = matOffset._32 = matOffset._34 = 0.0;
    matOffset._44 = 1.0;
    float x = In.x - 18.0 * sin( g_fTime / 2.0 );
    float y = In.x - 18.0 * cos( g_fTime );
    matOffset._22 = 1.0 + exp( - x * x / 15.0 ) ;
    matOffset._33 = 1.0 + exp( - y * y / 15.0 ) ;
    //matOffset._44 = 3.5;
    //matOffset._14 = 4.0 * sin( 2.0 * g_fTime );
    v4Out = mul( matOffset , In );
    return v4Out;
  }

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    //
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    float4 v4Position = Modify( In.Position );
      
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Position = mul( v4Position , matWorldViewProject );
    Out.TextureUV = In.TextureUV;
    return Out; 
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    float4 vColor = tex2D( MeshTextureSampler ,In.TextureUV );
    //vColor = tex2D( MeshTextureSampler ,In.TextureUV );
    return vColor;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        CullMode            = None;
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
