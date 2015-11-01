//--------------------------------------------------------------------------------------
// File: 灯光物体.fx
// 
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;
float4x4 g_matWorldNormalInverseTranspose;  // 未使用

float4   g_vLightPosition;                    // 未使用
float4   g_vLightLookAt;

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


VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    //
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    // 计算此物体的朝向矩阵，首先计算几个向量
    float3 v3Look  = normalize( + g_vLightPosition.xyz - g_vLightLookAt.xyz );
    float3 v3Right = normalize( cross( float3( 0.0f , 1.0f , 0.0f ) , v3Look ));
    float3 v3Up    = cross( v3Look , v3Right );
    // 通过几个向量建立矩阵
    float4x4 matLookAt ;
    matLookAt[ 0 ] = float4( v3Right.xyz , 0.0 );
    matLookAt[ 1 ] = float4( v3Up.xyz , 0.0 );
    matLookAt[ 2 ] = float4( v3Look.xyz , 0.0 );
    matLookAt[ 3 ] = float4( g_vLightPosition.xyz , 1.0 );
    //
    float4x4 matLocal = mul( matLookAt , g_matWorld );
    float4x4 matWorldView = mul( matLocal ,g_matView );
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

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        FillMode            = WireFrame;//Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
