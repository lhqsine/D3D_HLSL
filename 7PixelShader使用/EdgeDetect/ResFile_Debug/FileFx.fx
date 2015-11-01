// ----------------------------------------------------------------------------------------------
// 效果文件
// TextureDebug 
// ----------------------------------------------------------------------------------------------

texture texColorTexture;
texture texColorTextureIn;

const float off = 1.0 / 1024.0;    // 纹理的大小
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
    // Sample neighbor pixels
    float s00 = tex2D( ColorMapSamplerIn , In.tex + float2( -off, -off)).r;
    float s01 = tex2D( ColorMapSamplerIn , In.tex + float2(  0,   -off)).r;
    float s02 = tex2D( ColorMapSamplerIn , In.tex + float2(  off, -off)).r;

    float s10 = tex2D( ColorMapSamplerIn , In.tex + float2( -off,  0)).r;
    float s12 = tex2D( ColorMapSamplerIn , In.tex + float2(  off,  0)).r;

    float s20 = tex2D( ColorMapSamplerIn , In.tex + float2( -off,  off)).r;
    float s21 = tex2D( ColorMapSamplerIn , In.tex + float2(  0,    off)).r;
    float s22 = tex2D( ColorMapSamplerIn , In.tex + float2(  off,  off)).r;

    // Sobel filter in X direction
    float sobelX = s00 + 2 * s10 + s20 - s02 - 2 * s12 - s22;
    // Sobel filter in Y direction
    float sobelY = s00 + 2 * s01 + s02 - s20 - 2 * s21 - s22;

    // Find edge, skip sqrt() to improve performance ...
    float edgeSqr = ( sobelX * sobelX + sobelY * sobelY );

    // ... and threshold against a squared value instead.
    float4 Color = float4( 0.0 , 0.0 , 0.0 , 0.0 );
    if( edgeSqr >  0.00999 )
      {
        Color = float4( 1.0 , 1.0 , 1.0 , 1.0 );
      }
    else
      {
        Color = Color;
      }
    return Color ;
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

