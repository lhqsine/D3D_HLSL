// ----------------------------------------------------------------------------------------------
// 效果文件
// TextureDebug 
// ----------------------------------------------------------------------------------------------

float    fXScale = 0.2;
float    fYScale = 0.2;

texture texColorTexture_0;
texture texColorTexture_1;
texture texColorTexture_2;
texture texColorTexture_3;
texture texColorTexture_4;

texture texDepth;
texture texNormal;

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

sampler ColorMapSampler_0 = sampler_state
  {
    Texture   = ( texColorTexture_0 );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

sampler ColorMapSampler_1 = sampler_state
  {
    Texture   = ( texColorTexture_1 );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

sampler ColorMapSampler_2 = sampler_state
  {
    Texture   = ( texColorTexture_2 );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

sampler ColorMapSampler_3 = sampler_state
  {
    Texture   = ( texColorTexture_3 );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

sampler ColorMapSampler_4 = sampler_state
  {
    Texture   = ( texColorTexture_4 );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

sampler DepthSampler = sampler_state
  {
    Texture   = ( texDepth );
    MipFilter = POINT;
    MagFilter = POINT;
    ADDRESSU  = BORDER;
    ADDRESSV  = BORDER;
  };

sampler NormalSampler = sampler_state
  {
    Texture   = ( texNormal );
    MipFilter = POINT;
    MagFilter = POINT;
    ADDRESSU  = BORDER;
    ADDRESSV  = BORDER;
  };

//--------------------------------- Default --------------------------------------
//
VS_OUTPUT MainVS_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x ) * fXScale  - ( 1 - fXScale * 2 );
    In.pos.y = sign( In.pos.y ) * fYScale  + ( 1 - fYScale * 2 );
    Out.pos  = float4( In.pos.x , In.pos.y , 0.9 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 MainPS_Screen( VS_OUTPUT In ):COLOR
  {
    //return tex2D( ColorMapSampler ,In.tex ) ;
    float4 vColor = float4( In.color.x ,In.color.y ,In.color.z ,1 );
    return tex2D( ColorMapSampler_0 ,In.tex ) * vColor;
  }

float4 MainPS_Screen_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 1.0 ,1.0 ,0.0 ,0.0 );
  }

//--------------------------- Tech_0 ------------------------
VS_OUTPUT MainVS_Screen_0( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x ) * fXScale * 2 - ( 1 - fXScale * 2 );
    In.pos.y = sign( In.pos.y ) * fYScale * 2 + ( 1 - fYScale * 2 );
    Out.pos  = float4( In.pos.x , In.pos.y , 0.9 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }
// 将32为纹理转成浮点数
/*
float4 MainPS_Screen_0( VS_OUTPUT In ):COLOR
  {
    float4 v4Color = tex2D( ColorMapSampler_0 ,In.tex ) ;
    float4 ToFloat = float4( 1.0 , 1 / 256.0 , 1 / 65526.0 , 1 / 16777216.0 );
    float  fDepth = dot( v4Color , ToFloat ); 
    return float4( fDepth , fDepth , fDepth , 1.0 );
  }
*/

float4 MainPS_Screen_0( VS_OUTPUT In ):COLOR
  {
    float4 v4Color = tex2D( ColorMapSampler_0 ,In.tex ) ;
    return v4Color;
  }

float4 MainPS_Screen_0_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 1.0 ,1.0 ,0.0 ,1.0 );
  }

//--------------------------- Tech_1 ------------------------
VS_OUTPUT MainVS_Screen_1( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x ) * fXScale  - ( 1 - fXScale );
    In.pos.y = sign( In.pos.y ) * fYScale  + ( 1 - fYScale );
    Out.pos  = float4( In.pos.x , In.pos.y , 0.9 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 MainPS_Screen_1( VS_OUTPUT In ):COLOR
  {
    return tex2D( ColorMapSampler_1 ,In.tex ) ;
  }

float4 MainPS_Screen_1_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 1.0 ,0.0 ,0.0 ,1.0 );
  }

//--------------------------- Tech_2 ------------------------
VS_OUTPUT MainVS_Screen_2( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x ) * fXScale  - ( 1 - fXScale * 3 );
    In.pos.y = sign( In.pos.y ) * fYScale  + ( 1 - fYScale  );
    Out.pos  = float4( In.pos.x , In.pos.y , 0.9 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 MainPS_Screen_2( VS_OUTPUT In ):COLOR
  {
    return tex2D( ColorMapSampler_2 ,In.tex ) ;
  }

float4 MainPS_Screen_2_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 0.0 ,1.0 ,0.0 ,1.0 );
  }

//--------------------------- Tech_3 ------------------------
VS_OUTPUT MainVS_Screen_3( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x ) * fXScale  - ( 1 - fXScale  );
    In.pos.y = sign( In.pos.y ) * fYScale  + ( 1 - fYScale * 3 );
    Out.pos  = float4( In.pos.x , In.pos.y , 0.9 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 MainPS_Screen_3( VS_OUTPUT In ):COLOR
  {
    return tex2D( ColorMapSampler_3 ,In.tex ) ;
  }

float4 MainPS_Screen_3_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 1.0 ,1.0 ,0.0 ,1.0 );
  }

//--------------------------- Tech_4 ------------------------
VS_OUTPUT MainVS_Screen_4( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x ) * fXScale  - ( 1 - fXScale * 3 );
    In.pos.y = sign( In.pos.y ) * fYScale  + ( 1 - fYScale * 3 );
    Out.pos  = float4( In.pos.x , In.pos.y , 0.9 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 MainPS_Screen_4( VS_OUTPUT In ):COLOR
  {
    return tex2D( ColorMapSampler_4 ,In.tex ) ;
  }

float4 MainPS_Screen_4_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 0.0 ,0.0 ,1.0 ,1.0 );
  }

VS_OUTPUT MainVS_Screen_SSAO( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x ) * fXScale * 2 - ( 1 - fXScale * 2 );
    In.pos.y = sign( In.pos.y ) * fYScale * 2 + ( 1 - fYScale * 2 );
    Out.pos  = float4( In.pos.x , In.pos.y , 0.9 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 MainPS_Screen_SSAO( VS_OUTPUT In ):COLOR
  {
    return tex2D( DepthSampler , In.tex ) + tex2D( NormalSampler , In.tex );
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
    
    pass P1
      {
        FillMode          = Solid;
        VertexShader = compile vs_2_0 MainVS_Screen();
        PixelShader  = compile ps_2_0 MainPS_Screen_Line();
      }
      
  }

technique Tech_0
  {
    pass P0
      {
        AlphaBlendEnable  = true;
        SrcBlend          = SrcAlpha;
        DestBlend         = InvSrcAlpha;
        //FillMode          = WireFrame;
        CullMode            = CCW;
        VertexShader = compile vs_2_0 MainVS_Screen_0();
        PixelShader  = compile ps_2_0 MainPS_Screen_0();
      }
    pass P0
      {
        FillMode          = WireFrame;
        VertexShader = compile vs_2_0 MainVS_Screen_0();
        PixelShader  = compile ps_2_0 MainPS_Screen_0_Line();
      }
  }

technique Tech_1
  {
    pass P0
      {
        //AlphaBlendEnable  = true;
        //SrcBlend          = SrcAlpha;
        //DestBlend         = InvSrcAlpha;
        //FillMode          = WireFrame;
        CullMode            = CCW;
        VertexShader = compile vs_2_0 MainVS_Screen_1();
        PixelShader  = compile ps_2_0 MainPS_Screen_1();
      }
    pass P1
      {
        FillMode = WireFrame;
        VertexShader = compile vs_2_0 MainVS_Screen_1();
        PixelShader  = compile ps_2_0 MainPS_Screen_1_Line();
      }
  }

technique Tech_2
  {
    pass P0
      {
        //AlphaBlendEnable  = true;
        //SrcBlend          = SrcAlpha;
        //DestBlend         = InvSrcAlpha;
        //FillMode          = WireFrame;
        CullMode            = CCW;
        VertexShader = compile vs_2_0 MainVS_Screen_2();
        PixelShader  = compile ps_2_0 MainPS_Screen_2();
      }
    pass P1
      {
        FillMode = WireFrame;
        VertexShader = compile vs_2_0 MainVS_Screen_2();
        PixelShader  = compile ps_2_0 MainPS_Screen_2_Line();
      }
  }

technique Tech_3
  {
    pass P0
      {
        //AlphaBlendEnable  = true;
        //SrcBlend          = SrcAlpha;
        //DestBlend         = InvSrcAlpha;
        //FillMode          = WireFrame;
        CullMode            = CCW;
        VertexShader = compile vs_2_0 MainVS_Screen_3();
        PixelShader  = compile ps_2_0 MainPS_Screen_3();
      }
    pass P1
      {
        FillMode = WireFrame;
        VertexShader = compile vs_2_0 MainVS_Screen_3();
        PixelShader  = compile ps_2_0 MainPS_Screen_3_Line();
      }
  }

technique Tech_4
  {
    pass P0
      {
        //AlphaBlendEnable  = true;
        //SrcBlend          = SrcAlpha;
        //DestBlend         = InvSrcAlpha;
        //FillMode          = WireFrame;
        CullMode            = CCW;
        VertexShader = compile vs_2_0 MainVS_Screen_4();
        PixelShader  = compile ps_2_0 MainPS_Screen_4();
      }
    pass P1
      {
        FillMode = WireFrame;
        VertexShader = compile vs_2_0 MainVS_Screen_4();
        PixelShader  = compile ps_2_0 MainPS_Screen_4_Line();
      }
  }

technique TECH_SSAO
  {
    pass P0
      {
        VertexShader = compile vs_2_0 MainVS_Screen_SSAO();
        PixelShader  = compile ps_2_0 MainPS_Screen_SSAO();
      }
  }
