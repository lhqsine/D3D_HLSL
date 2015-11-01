// ----------------------------------------------------------------------------------------------
// 效果文件
// FullWindow
// ----------------------------------------------------------------------------------------------

texture g_texPositionTexture;
texture g_texNormalTexture;
//texture g_texColorTexture;
float3  g_vLightPosition ; //

// ------------------------------- 传入顶点的颜色和 alpha 结构 -----------------------------------

struct VS_INPUT
  {
    float4 pos    : POSITION;
    float4 color  : COLOR0;
    float2 tex    : TEXCOORD0;
  };
// 
struct VS_OUTPUT
  {
    float4 pos     : POSITION;
    float4 color   : COLOR0;
    float2 tex     : TEXCOORD0;
  };

sampler PositionMapSampler = sampler_state
  {
    Texture   = ( g_texPositionTexture );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

sampler NormalMapSampler = sampler_state
  {
    Texture   = ( g_texNormalTexture );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

//--------------------------------- Default --------------------------------------
//
VS_OUTPUT MainVS_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x ) ;
    In.pos.y = sign( In.pos.y ) ;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.9 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 MainPS_Screen( VS_OUTPUT In ):COLOR
  {
    
    float4 v4OutColor = float4( 0.0 ,0.0 ,0.0 ,0.0 );
    float4 v4Position = tex2D( PositionMapSampler , In.tex );
    if( v4Position.w < 0.01 ) 
      {
        v4OutColor = float4( 0.0 , 0.0 ,0.0 , 0.0 );
      }
    else
      {
        float3 v3Normal = normalize( tex2D( NormalMapSampler , In.tex ).xyz );
        float3 v3LightDirection = normalize( g_vLightPosition - v4Position.xyz );
        v4OutColor = dot( v3Normal , v3LightDirection );
      }
    return v4OutColor;
  }

float4 MainPS_Screen_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 1.0 ,1.0 ,0.0 ,0.0 );
  }


//--------------------------- 技术---------------------------

technique Default
  {
    pass P0
      {
        AlphaBlendEnable  = true;
        SrcBlend          = SrcAlpha;
        DestBlend         = InvSrcAlpha;
        //FillMode          = WireFrame;
        CullMode = CCW;
        VertexShader = compile vs_2_0 MainVS_Screen();
        PixelShader  = compile ps_2_0 MainPS_Screen();
      }
    /*
    pass P1
      {
        FillMode          = Solid;
        VertexShader = compile vs_2_0 MainVS_Screen();
        PixelShader  = compile ps_2_0 MainPS_Screen_Line();
      }
    */
  }
