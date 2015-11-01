// ----------------------------------------------------------------------------------------------
// 效果文件
// TextureDebug 
// ----------------------------------------------------------------------------------------------

float    fXScale = 1.0;
float    fYScale = 1.0;

texture texColorTexture;
texture texColorBlack;

float    g_fTime;
float4  g_v4Position;
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

sampler ColorMapSamplerIn = sampler_state
  {
    Texture   = ( texColorTexture );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

sampler BlackMapSamplerIn = sampler_state
  {
    Texture   = ( texColorBlack );
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
    In.pos.x = sign( In.pos.x )  ;
    In.pos.y = sign( In.pos.y )  ;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.198 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }
/*
float4 MainPS_Screen( VS_OUTPUT In ):COLOR
  {
    float4 color = tex2D( ColorMapSamplerIn , In.tex );
    return color;
  }
*/

float4 MainPS_Screen( VS_OUTPUT In ):COLOR
  {
    // 定义一些变量
    #define NUM_SAMPLES 25                // 循环次数 
    // 
    float Density             = 1.0f;     // 强度，强度越大，则光的长度越短
    float Exposure            = 0.30f;    // 总体强度
    float Weight              = 0.60f;    // 每次采样的强度
    float Decay               = 0.94f;    // 远离光源时的衰减因子
    float illuminationDecay   = 1.0f;     // 指数衰减值
    //---------------------------------------
    // 首先计算纹理的每步骤偏移向量
    //---------------------------------------
    // 光源在屏幕空间的位置
    float2 ScreenLightPos = float2( g_v4Position.x , g_v4Position.y );
    // 计算屏幕空间中从像素点到光源的向量，向量指向此像素中心
    float2 deltaTexCoord = In.tex - ScreenLightPos;
    // 通过采样的数目划分以及通过控制因子缩放，将此向量分段
    deltaTexCoord = deltaTexCoord / ( NUM_SAMPLES * Density );
    // 保存最初的采样
    float4 color = tex2D( ColorMapSamplerIn , In.tex );
    // 从等式3迭代计算汇总值
    for( int i = 0 ; i < NUM_SAMPLES ; i ++ )
      {
        //-----------------------------------
        // 变更每次的纹理坐标，获取像素值，将此像素保存
        // 其中illuminationDecay为亮度衰减值，每此循环衰减
        //-----------------------------------
        // 延光线遍历采样位置
        In.tex = In.tex - deltaTexCoord;
        // 在新的位置恢复采样
        float4 sample = tex2D( ColorMapSamplerIn ,In.tex );
        // 应用采样衰减 scale/decay 因子
        sample = illuminationDecay * Weight * sample;
        // 累加混合颜色
        color = sample + color;
        // 更新指数的衰减因子
        illuminationDecay = illuminationDecay * Decay;
      }    
    // 使用缩放控制因子输出最终颜色
    return float4(( color * Exposure ).xyz , 0 );
  }

float4 MainPS_Screen_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 1.0 ,0.0 ,0.0 ,0.0 );
  }


//--------------------------- 技术---------------------------
// CBoard 的显示
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
    /*
    pass P1
      {
        FillMode          = WireFrame;//Solid;
        VertexShader = compile vs_2_0 MainVS_Screen();
        PixelShader  = compile ps_2_0 MainPS_Screen_Line();
      }
    */
  }

