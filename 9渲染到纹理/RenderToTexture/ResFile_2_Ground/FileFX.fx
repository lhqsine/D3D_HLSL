//--------------------------------------------------------------------------------------
// 文件：FileFX.fx
// 物体：振动面 
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh
texture g_RenderToTexture;          // 渲染到缓冲区的纹理

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

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

float ReturnY_1( VS_INPUT In )
  {
    // --------------------- shader 1 -----------------------
    // 设置比较点，
    float3 fZero = float3( 3.0 * sin( g_fTime / 2.0 ) ,0 , 3.0 * cos( g_fTime / 2.0 ));
    // 创建一个函数，随距离衰减，最大为2，
    float fScale =  3.0 / ( sin( 2.0 * ( distance( In.Position ,fZero ) - g_fTime )) + 1.5 ) ;
    float fY = -2.0 * exp( - fScale ) +1.0 ;
    return fY;
  }

float ReturnY_2( VS_INPUT In )
  {
    // --------------------- shader 1 -----------------------
    // 设置比较点，
    float3 fZero = float3( 0.0 ,0.0 , 0.0 );
    float fScale =  1.0 / ( sin(( distance( In.Position ,fZero ) - g_fTime )) + 1.5 ) ;
    float fY = -2.0 * exp( - fScale ) +1.0 ;
    //return fZ;
    return max( fY , -4.0 );
  }

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    //
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.Position.y = ReturnY_2( In );
      
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

float4 RenderScenePS_Line( VS_OUTPUT In ) :COLOR0
  { 
    float4 vColor = float4( 0.0 ,1.0 ,1.0 ,1.0 );
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
    /*
    pass P1
      {
        FillMode            = WireFrame;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS_Line( );
      }
      */
  }

technique RenderToTextureShader
  {
    pass P0
      {
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderToTextureVS( );
        PixelShader  = compile ps_2_0 RenderToTexturePS( );
      }
  }


