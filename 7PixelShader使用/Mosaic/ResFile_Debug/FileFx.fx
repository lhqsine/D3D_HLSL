// ----------------------------------------------------------------------------------------------
// 效果文件
// TextureDebug 
// ----------------------------------------------------------------------------------------------

texture texColorTexture;
texture texColorTextureIn;

float4 g_v4ScreenSize;
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

sampler ColorMapSampler = sampler_state
  {
    Texture   = ( texColorTexture );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

sampler ColorMapSamplerIn = sampler_state
  {
    Texture   = ( texColorTextureIn );
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
    // 纹理大小
    float2 fTexSize = float2( g_v4ScreenSize.x , g_v4ScreenSize.y );
    // 马赛克大小
    float2 fMosicSize = float2( 7 , 7 );
    // 将纹理坐标转换成以 8 为步长的坐标
    float2 fintXY = float2( In.tex.x * fTexSize.x ,In.tex.y * fTexSize.y );
    // 
    float2 fXYMosic = float2( int( fintXY.x / fMosicSize.x ) * fMosicSize.x ,
      int( fintXY.y / fMosicSize.y ) * fMosicSize.y ) + 0.5 * fMosicSize;
    float2 fDelXY = fXYMosic - fintXY;
    //
    float2 fUVMosic = float2( fXYMosic.x / fTexSize.x , fXYMosic.y / fTexSize.y );
    if( length( fDelXY ) < 0.5 * fMosicSize.x ) 
      return tex2D( ColorMapSamplerIn , fUVMosic );
    else  return float4( 0.0 ,0.0 , 0.0 ,0.0 );
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
        //AlphaBlendEnable  = true;
        //SrcBlend          = SrcAlpha;
        //DestBlend         = InvSrcAlpha;
        //FillMode          = WireFrame;
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

