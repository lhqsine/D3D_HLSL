// ----------------------------------------------------------------------------------------------
// 效果文件
// 问题：传入的顶点颜色值必须如下形式
//
// ----------------------------------------------------------------------------------------------

float    fXScale = 0.2;
float    fYScale = 0.03;
float    g_fProgress;

texture texColorTexture;

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

//--------------------------------- ScreenShader --------------------------------------
//
VS_OUTPUT MainVS_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    // 将 Y 缩放后向下移动 1 ,在向上移动 0.03，
    // 将 X 左移 1 ，在加上传入值
    //In.pos.x = sign( In.pos.x ) * g_fProgress * 2 - ( 1 - g_fProgress * 2 );
    In.pos.x = sign( In.pos.x ) - 2 + 2 * g_fProgress ;
    In.pos.y = sign( In.pos.y ) * fYScale  - 1 + fYScale;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.9 , 1 );
    Out.tex        = In.tex;
    Out.color = In.color;
    return Out;
  }

// -------------- ps ---------------------
float4 MainPS_Screen( VS_OUTPUT In ):COLOR
  {
    //return tex2D( ColorMapSampler ,In.tex ) ;
    float4 vColor = float4( In.color.x ,In.color.y ,In.color.z ,1 );
    return tex2D( ColorMapSampler ,In.tex ) * vColor;
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
        FillMode          = WireFrame;
        VertexShader = compile vs_2_0 MainVS_Screen();
        PixelShader  = compile ps_2_0 MainPS_Screen_Line();
      }
    */
  }

