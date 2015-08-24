//--------------------------------------------------------------------------------------
// 文件：FileFX.fx
// 坐标轴物体
//--------------------------------------------------------------------------------------

texture g_ColorTexture;             // 颜色纹理
texture g_RenderToTexture;          // 渲染到缓冲区的纹理

float    g_fTime;                   // 传入时间
float4x4 g_matWorld;                // 世界矩阵
float4x4 g_matView;                 // 观察矩阵
float4x4 g_matProject;              // 投影矩阵
//
float4x4 g_matWorldRTT;
float4x4 g_matViewRTT;
float4x4 g_matProjectRTT;

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

sampler MeshRTTSampler = sampler_state
  {
    Texture = <g_RenderToTexture>;
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
    float4 vColor = tex2D( MeshTextureSampler ,In.TextureUV ) ;
    return vColor;
  }

// --------------------- RenderToTexture ---------------------
VS_OUTPUT RenderToTextureVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorldRTT ,g_matViewRTT );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProjectRTT );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    return Output;    
  }

float4 RenderToTexturePS( VS_OUTPUT In ) :COLOR0
  { 
    float4 vColor = tex2D( MeshTextureSampler ,In.TextureUV ) ;
    return vColor;
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

technique RenderToTextureShader
  {
    pass P0
      {
        AlphaBlendEnable = true;
        BlendOp          = Add;
        //--------------------------------
        SrcBlend         = SrcColor;
        DestBlend        = InvSrcAlpha;
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderToTextureVS( );
        PixelShader  = compile ps_2_0 RenderToTexturePS( );
      }
  }

