//--------------------------------------------------------------------------------------
// 文件：FileFX.fx
// 物体：地面和外框 
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh
texture g_BlackTexture;

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

sampler BlackTextureSampler = sampler_state
  {
    Texture = <g_BlackTexture>;
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

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    //
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    //In.Position.y = ReturnY_2( In );
      
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Position = mul( In.Position , matWorldViewProject );
    Out.TextureUV = In.TextureUV;
    return Out; 
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    float4 vColor = tex2D( MeshTextureSampler ,In.TextureUV );
    return vColor;
  }

float4 RenderScenePSBG( VS_OUTPUT In ) :COLOR0
  { 
    float4 vColor = tex2D( BlackTextureSampler ,In.TextureUV );
    return vColor;
  }

float4 RenderScenePS_Line( VS_OUTPUT In ) :COLOR0
  { 
    float4 vColor = float4( 0.0 ,1.0 ,1.0 ,1.0 );
    return vColor;
  }

//------------------------------------------------------------------------------
// 地面
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        //AlphaBlendEnable = true;
        AlphaTestEnable    = true;
        AlphaRef           = 0x1;
        AlphaFunc          = Greater;      
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
    /*
    pass P1
      {
        FillMode            = WireFrame;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS_Line( );
      }
      */
  }

technique DefaultTechBackGround
  {
    pass P0
      {
        CullMode           = None;
        AlphaTestEnable    = true;
        AlphaRef           = 0x40;
        AlphaFunc          = Greater;      
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePSBG( );
      }
    /*
    pass P1
      {
        FillMode            = WireFrame;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS_Line( );
      }
      */
  }
