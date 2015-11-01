//--------------------------------------------------------------------------------------
// File: 受光物体.fx
// Defered
//--------------------------------------------------------------------------------------
float    g_fFar = 40;

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
    float3 Normal     : NORMAL; 
    float2 TextureUV  : TEXCOORD0;  
  };

struct VS_OUTPUTDS
  {
    float4 Position  : POSITION;
    float3 PositionInTex : TEXCOORD0;
    float3 NormalInTex   : TEXCOORD1;
  };

//-------- 渲染位置到纹理 ------------
VS_OUTPUTDS RenderPositionVS( VS_INPUT In )
  {
    VS_OUTPUTDS Out = ( VS_OUTPUTDS ) 0 ;
    float4x4 matWorldView = mul( g_matWorld , g_matView );
    float4x4 matWorldViewProject = mul( matWorldView , g_matProject );
    Out.Position = mul( In.Position , matWorldViewProject );
    Out.PositionInTex = mul( In.Position , g_matWorld ); ;
    Out.NormalInTex = mul( In.Normal , g_matWorld ) ;
    return Out;
  }

float4 RenderPositionPS( VS_OUTPUTDS In ): COLOR0
  {
    return float4( In.PositionInTex , 1.0 );
  }
//-------- 渲染法向量到纹理 ------------
VS_OUTPUTDS RenderNormalVS( VS_INPUT In )
  {
    VS_OUTPUTDS Out = ( VS_OUTPUTDS ) 0 ;
    float4x4 matWorldView = mul( g_matWorld , g_matView );
    float4x4 matWorldViewProject = mul( matWorldView , g_matProject );
    Out.Position = mul( In.Position , matWorldViewProject );
    Out.PositionInTex = mul( In.Position , g_matWorld ); ;
    Out.NormalInTex = mul( In.Normal , g_matWorld ) ;
    return Out;
  }

float4 RenderNormalPS( VS_OUTPUTDS In ): COLOR0
  {
    return float4( In.NormalInTex , 1.0 );
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------

technique RenderPositionTech
  {
    pass P0
      {
        FillMode  = Solid;
        VertexShader = compile vs_2_0 RenderPositionVS( );
        PixelShader  = compile ps_2_0 RenderPositionPS( );
      }
  }

technique RenderNormalTech
  {
    pass P0
      {
        FillMode  = Solid;
        VertexShader = compile vs_2_0 RenderNormalVS( );
        PixelShader  = compile ps_2_0 RenderNormalPS( );
      }
  }
