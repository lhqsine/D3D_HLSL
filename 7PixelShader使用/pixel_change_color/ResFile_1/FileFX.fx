//--------------------------------------------------------------------------------------
// 文件：FileFX.fx
// 坐标轴物体
//--------------------------------------------------------------------------------------

texture g_ColorTexture;             // 颜色纹理

float    g_fTime;                   // 传入时间
float4x4 g_matWorld;                // 世界矩阵
float4x4 g_matView;                 // 观察矩阵
float4x4 g_matProject;              // 投影矩阵
//
sampler2D MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = MIRROR;
    AddressV  = MIRROR;
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

// --------------------- Default Render -----------------------
VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
//    float4 vColor = tex2D( MeshTextureSampler ,In.TextureUV * 2.0 ) ;
//    float OutColor = vColor.x * 0.3 + vColor.y * 0.5 + vColor.z * 0.2 ;
//    return float4( OutColor , OutColor , OutColor , 1.0 );
      float4 vColor = tex2D( MeshTextureSampler ,In.TextureUV ) ;
	  float4 OutColor = float4( 1 - vColor.x , 1 - vColor.y , 1 - vColor.z, 1.0) ;
	  return OutColor;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }

