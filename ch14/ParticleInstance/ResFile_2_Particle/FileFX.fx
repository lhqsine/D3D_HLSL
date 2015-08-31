//--------------------------------------------------------------------------------------
// File: BasicHLSL.fx
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

//float4   g_vEyePt;           // BillBoard 所在位置
float4   g_vLookAt;          // 摄影机所在位置

#define  INSTANCENUM 200
float4   g_vVectorArray[ INSTANCENUM ];

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
    float  fInstance  : TEXCOORD1;
  };

struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD0;  
  };
//-----------------------------------------------
// 排列方式检查
//-----------------------------------------------
/*
VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT ) 0;
    int iID = ( int )In.fInstance;
    float4 v4Position = g_vVectorArray[ iID ].xyzw ;
    //float4 v4Position = g_vVectorArray[ In.fInstance ] ;
    In.Position = v4Position + In.Position;
    //
    //g_matWorld._11 = 2.0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Position = mul( In.Position ,matWorldViewProject );
//    Out.Position = mul( In.Position , matWorldViewProject );
    Out.TextureUV = In.TextureUV;
    return Out;
  }
*/

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    // 首先从顶点的一个变量中读取实例的 ID，通过此 ID 来选择使用哪个偏移
    int iID = ( int )In.fInstance;
    float4 v4EyePt = g_vVectorArray[ iID ].xyzw ;
    float3 v3Look = normalize( v4EyePt.xyz - g_vLookAt.xyz );
    float3 v3Right = normalize( cross( float3( 0.0f , 1.0f , 0.0f ) , v3Look ));
    float3 v3Up    = cross( v3Look , v3Right );
    // 通过几个向量建立矩阵
    float4x4 matLookAt ;
    matLookAt[ 0 ] = float4( v3Right.xyz , 0.0 );
    matLookAt[ 1 ] = float4( v3Up.xyz , 0.0 );
    matLookAt[ 2 ] = float4( v3Look.xyz , 0.0 );
    matLookAt[ 3 ] = float4( v4EyePt.xyz , 1.0 );
    //g_matWorld._44 = 0.5;
    float4x4 matLocal = mul( matLookAt , g_matWorld );
    float4x4 matWorldView = mul( matLocal ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    
    //In.Position = In.Position + v4EyePt;
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
    float4 vColor = float4( 1.0 , 0.0 , 0.0 , 1.0 );
    return vColor;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        ZEnable             = true;
        ZWriteEnable        = false;
        CullMode          = CCW;//None;//CCW;
        // 透明
        
        AlphaBlendEnable  = true;
        SrcBlend          = SrcAlpha;
        DestBlend         = InvSrcAlpha;
        
        //
        FillMode          = Solid;//WireFrame;
        VertexShader = compile vs_3_0 RenderSceneVS( );
        PixelShader  = compile ps_3_0 RenderScenePS( );
      }
    /*
    pass P1
      {
        FillMode          = WireFrame;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS_Line( );
      }
    */
  }
