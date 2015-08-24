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
    float2 f2uv = In.tex;
    float4 v4Color = tex2D( ColorMapSamplerIn , f2uv );
    if(( f2uv.x < 0.5f ) && ( f2uv.y < 0.5f ))        // 左上角：红色增加
      {
        v4Color.r  = v4Color.r ;
        v4Color.gb = 0.0;
      }
    else if(( f2uv.x < 0.5f ) && ( f2uv.y > 0.5f ))   // 左下角：绿色增加
      {
        v4Color.g  = v4Color.g ;
        v4Color.rb = 0.0;
      }
    else if(( f2uv.x > 0.5f ) && ( f2uv.y < 0.5f ))   // 右上角：蓝色增加
      {
        v4Color.b  = v4Color.b ;
        v4Color.rg = 0.0;
      }
    else if(( f2uv.x > 0.5f ) && ( f2uv.y > 0.5f ))   // 右下角：黄色增加
      {
        v4Color.rg  = v4Color.rg ;
        v4Color.b = 0.0f;
      }
    else
      {
        v4Color = float4( 1.0 , 1.0 , 1.0 , 1.0 );    // 中间分割线：白色
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

