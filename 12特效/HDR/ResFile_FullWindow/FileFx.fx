// ----------------------------------------------------------------------------------------------
// 效果文件
// TextureDebug 
// ----------------------------------------------------------------------------------------------

//float    fXScale = 1.0;
//float    fYScale = 1.0;


texture g_texScene;                         // 场景纹理
texture g_texTextureBloom_DownSize_3x3;     // 缩小到窗口大小 1/ 9 的纹理，用来水平方向模糊
texture g_texTextureBloom_DownSize_3x3_H ;  // 
texture g_texLastOutput;                    // 最终用于显示的纹理
// 
// 缩小到 1 / 9 
float2 g_DownSizeOffset[ 4 ] =
  {
    float2( - 1 / 106.0 , - 1 / 68.0 ) , float2( - 1 / 106.0 , + 1 / 68.0 ) , float2( + 1 / 106.0 , - 1 / 68.0 ) , float2( 1 / 106.0 , 1 / 68.0 ) ,
  };

float g_Weight9[ 9 ] = // 用于纹理采样
  {
    0.091324545 , 0.12853695 , 0.16408049 , 0.18996529 , 0.19947115 , 0.18996529 , 0.16408049 , 0.12853695 , 0.091324545 ,
  };
// 
float g_HBloomOffset9[ 9 ] =
  {
    - 4 / 71.0 , - 3 / 71.0 , - 2 / 71.0 , - 1 / 71.0 , 0.0 , 1 / 71.0 , 2 / 71.0 , 3 / 71.0 , 4 / 71.0 ,
  };

// 213 , 127
float2 g_UV4[ 4 ] =
  {
    float2( - 1 / 213.0 , 0.0 ) , float2( 0.0 , - 1 / 127.0 ),
    float2( + 1 / 213.0 , 0.0 ) , float2( 0.0 , + 1 / 213.0 ),
  };

float   g_fTime;
//float4  g_v4Position;
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

sampler TextureScene_SamplerIn = sampler_state
  {
    Texture   = ( g_texScene );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = BORDER;
    ADDRESSV  = BORDER;
  };

sampler TextureLastOutPut_SamplerIn = sampler_state
  {
    Texture   = ( g_texLastOutput );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

sampler TextureBloom_SamplerIn = sampler_state
  {
    Texture   = ( g_texTextureBloom_DownSize_3x3 );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = BORDER;//MIRROR;
    ADDRESSV  = BORDER;//MIRROR;
  };

sampler TextureBloom_V_SamplerIn = sampler_state
  {
    Texture   = ( g_texTextureBloom_DownSize_3x3_H );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = BORDER;//MIRROR;
    ADDRESSV  = BORDER;//MIRROR;
  };

//------------------------------------------
// 技术：Default 
// 功能：将据矩形渲染到屏幕
//------------------------------------------
VS_OUTPUT MainVS_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x )  ;
    In.pos.y = sign( In.pos.y )  ;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.198 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 MainPS_Screen( VS_OUTPUT In ):COLOR
  {
    float4 v4OutColor = tex2D( TextureBloom_SamplerIn , In.tex ) + tex2D( TextureScene_SamplerIn , In.tex );;
    return v4OutColor;
  }

float4 MainPS_Screen_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 1.0 ,0.0 ,0.0 ,0.0 );
  }

//------------------------------------------
// 技术：Bloom_Tech 使用函数
// 说明：亮度比较
//------------------------------------------
VS_OUTPUT SceneVS_Bright_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x )  ;
    In.pos.y = sign( In.pos.y )  ;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.198 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 ScenePS_Bright_Screen( VS_OUTPUT In ):COLOR
  {
    float4 v4Average = float4( 0.0 , 0.0 ,0.0 , 0.0 );
    for( int i = 0 ; i < 4 ; i++ )
      {
        v4Average += tex2D( TextureScene_SamplerIn , In.tex + g_UV4[ i ] );
      }
    v4Average = v4Average / 4.0;
    if( v4Average.r < 0.80 ) 
      {
        v4Average = float4( 0.0 , 0.0 , 0.0 , 1.0 );
      }
    return v4Average * 1.4;
  }
//------------------------------------------
// 技术：HBloom_Tech 使用函数
// 说明：水平模糊
//------------------------------------------
VS_OUTPUT SceneVS_HBloom_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x )  ;
    In.pos.y = sign( In.pos.y )  ;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.198 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 ScenePS_HBloom_Screen( VS_OUTPUT In ):COLOR
  {
    float4 v4Average = { 0.0f , 0.0f , 0.0f , 0.0f };
    for( int i = 0 ; i < 9 ; i++ )
      {
        v4Average += tex2D( TextureBloom_SamplerIn , In.tex + float2( g_HBloomOffset9[ i ] , 0.0 )) * g_Weight9[ i ];
      }
    
    return float4( v4Average.rgb , 1.0 );
  }
//------------------------------------------
// 技术：Bloom_Tech 使用函数
// 说明：垂直模糊
//------------------------------------------
VS_OUTPUT SceneVS_VBloom_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x )  ;
    In.pos.y = sign( In.pos.y )  ;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.198 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 ScenePS_VBloom_Screen( VS_OUTPUT In ):COLOR
  {
    float4 v4Average = { 0.0f , 0.0f , 0.0f , 0.0f };
    for( int i = 0 ; i < 9 ; i++ )
      {
        v4Average += tex2D( TextureBloom_V_SamplerIn , In.tex + float2( 0.0 , g_HBloomOffset9[ i ] )) * g_Weight9[ i ];
      }
    
    return float4( v4Average.rgb , 1.0 );
  }
//---------------------------------------------
// 将纹理再缩小 1 / 3
//---------------------------------------------
VS_OUTPUT SceneVS_DownSize_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x )  ;
    In.pos.y = sign( In.pos.y )  ;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.198 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

float4 ScenePS_DownSize_Screen( VS_OUTPUT In ):COLOR
  {
    float3 COLORTOGRAY = float3( 0.2125f , 0.7154f , 0.0721f );
    float4 v4Average = float4( 0.0f , 0.0f , 0.0f , 0.0f );
    // 采样周围四个像素输出
    for( int i = 0 ; i < 4 ; i++ )
      {
        v4Average += tex2D( TextureScene_SamplerIn , In.tex + g_DownSizeOffset[ i ] ) ;
      }
    v4Average =  v4Average * 0.25 ;
    //------------ 调试用的 Shader -------------
    // 不使用纹理过滤时正常，则使用纹理过滤时错误
    // v4Average = tex2D( TextureScene_SamplerIn , In.tex );
    // 
    if( dot( COLORTOGRAY , v4Average.xyz ) < 0.7 )
      {
        v4Average = float4( 0.0 , 0.0 , 0.0 , 0.0 );
      }
    return float4( v4Average.rgb , 1.0 ) ;
  }
//---------------------------------------------
// 输出计算的纹理
//---------------------------------------------

VS_OUTPUT SceneVS_OutPut_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x )  ;
    In.pos.y = sign( In.pos.y )  ;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.198 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }


float4 ScenePS_OutPut_Screen( VS_OUTPUT In ):COLOR
  {
    float4 v4OutColor = tex2D( TextureLastOutPut_SamplerIn , In.tex );
    return v4OutColor;
  }

//-----------------------------------------------------------
// 技术：Default
// 功能：渲染 CFullWindow，将多个纹理的融合效果渲染
//-----------------------------------------------------------
technique Default
  {
    pass P0
      {
        AlphaBlendEnable  = true;
        SrcBlend          = SrcColor;
        DestBlend         = InvSrcColor;
        //AlphaBlendEnable  = true;
        //SrcBlend          = SrcAlpha;
        //DestBlend         = InvSrcAlpha;
        FillMode          = Solid;//WireFrame;
        VertexShader = compile vs_2_0 MainVS_Screen();
        PixelShader  = compile ps_2_0 MainPS_Screen();
      }
    
    pass P1
      {
        FillMode          = WireFrame;//Solid;
        VertexShader = compile vs_2_0 MainVS_Screen();
        PixelShader  = compile ps_2_0 MainPS_Screen_Line();
      }
  }
//-----------------------------------------
// 技术：DownSizeBrightSet_Tech
// 功能：缩小纹理，并根据亮度阈值返回需要 Bloom 的亮度值
//-----------------------------------------
technique DownSizeBrightSet_Tech
  {
    pass P0
      {
        VertexShader = compile vs_3_0 SceneVS_DownSize_Screen();
        PixelShader  = compile ps_3_0 ScenePS_DownSize_Screen();
      }
  }

//-----------------------------------------
// 技术：HBloom_Tech
// 功能：水平模糊
//-----------------------------------------
technique HBloom_Tech
  {
    pass P0
      {
        VertexShader = compile vs_3_0 SceneVS_HBloom_Screen();
        PixelShader  = compile ps_3_0 ScenePS_HBloom_Screen();
      }
  }
//-----------------------------------------
// 技术：VBloom_Tech
// 功能：垂直模糊
//-----------------------------------------
technique VBloom_Tech
  {
    pass P0
      {
        AlphaBlendEnable = true;
        BlendOp          = Add;
        //--------------------------------
        SrcBlend         = SrcAlpha;
        DestBlend        = One;
        VertexShader = compile vs_3_0 SceneVS_VBloom_Screen();
        PixelShader  = compile ps_3_0 ScenePS_VBloom_Screen();
      }
  }
//-----------------------------------------
// 技术：LastOutPut_Tech
// 功能：将 Bloom 输出
//-----------------------------------------
/*
technique LastOutPut_Tech
  {
    
  }
*/
