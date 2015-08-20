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
    float fScale = sin(( 1.40 * distance( In.Position ,fZero ) - g_fTime * 5 )) + 1.5  ;
    float fY = -3.0 * exp( - fScale ) +1.0 ;
    //;
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
