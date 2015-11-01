// ----------------------------------------------------------------------------------------------
// 效果文件
// TextureDebug
// ----------------------------------------------------------------------------------------------

texture texColorTexture;
texture texColorTextureIn;

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
    // 测试 UV 坐标的原点
    float fPI = 3.1415926;
    float2 f2uv = float2( sin( In.tex.x * fPI * 16.0f ) ,cos( In.tex.y * fPI * 16.0f ));
    float4 v4Color = tex2D( ColorMapSamplerIn , In.tex );
    if(( f2uv.x > -0.40f ) && ( f2uv.y > -0.40f ))        // 左上角：红色增加
      {
        v4Color = v4Color * 1.1;
      }
    else
      {
        v4Color = float4( 0.0 , 0.00 , 0.30 , 0.5 );    // 中间分割线：白色
      }
    return v4Color;
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

