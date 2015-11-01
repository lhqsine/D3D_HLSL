// ----------------------------------------------------------------------------------------------
// Ч���ļ�
// TextureDebug 
// ----------------------------------------------------------------------------------------------

float    fXScale = 1.0;
float    fYScale = 1.0;

texture texColorTexture;
texture texColorBlack;

float    g_fTime;
float4  g_v4Position;
// ------------------------------- ���붥�����ɫ�� alpha �ṹ -----------------------------------

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
    // ����һЩ����
    #define NUM_SAMPLES 25                // ѭ������ 
    // 
    float Density             = 1.0f;     // ǿ�ȣ�ǿ��Խ�����ĳ���Խ��
    float Exposure            = 0.30f;    // ����ǿ��
    float Weight              = 0.60f;    // ÿ�β�����ǿ��
    float Decay               = 0.94f;    // Զ���Դʱ��˥������
    float illuminationDecay   = 1.0f;     // ָ��˥��ֵ
    //---------------------------------------
    // ���ȼ��������ÿ����ƫ������
    //---------------------------------------
    // ��Դ����Ļ�ռ��λ��
    float2 ScreenLightPos = float2( g_v4Position.x , g_v4Position.y );
    // ������Ļ�ռ��д����ص㵽��Դ������������ָ�����������
    float2 deltaTexCoord = In.tex - ScreenLightPos;
    // ͨ����������Ŀ�����Լ�ͨ�������������ţ����������ֶ�
    deltaTexCoord = deltaTexCoord / ( NUM_SAMPLES * Density );
    // ��������Ĳ���
    float4 color = tex2D( ColorMapSamplerIn , In.tex );
    // �ӵ�ʽ3�����������ֵ
    for( int i = 0 ; i < NUM_SAMPLES ; i ++ )
      {
        //-----------------------------------
        // ���ÿ�ε��������꣬��ȡ����ֵ���������ر���
        // ����illuminationDecayΪ����˥��ֵ��ÿ��ѭ��˥��
        //-----------------------------------
        // �ӹ��߱�������λ��
        In.tex = In.tex - deltaTexCoord;
        // ���µ�λ�ûָ�����
        float4 sample = tex2D( ColorMapSamplerIn ,In.tex );
        // Ӧ�ò���˥�� scale/decay ����
        sample = illuminationDecay * Weight * sample;
        // �ۼӻ����ɫ
        color = sample + color;
        // ����ָ����˥������
        illuminationDecay = illuminationDecay * Decay;
      }    
    // ʹ�����ſ����������������ɫ
    return float4(( color * Exposure ).xyz , 0 );
  }

float4 MainPS_Screen_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 1.0 ,0.0 ,0.0 ,0.0 );
  }


//--------------------------- ����---------------------------
// CBoard ����ʾ
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

