// ----------------------------------------------------------------------------------------------
// Ч���ļ�
// TextureDebug 
// ----------------------------------------------------------------------------------------------

//float    fXScale = 1.0;
//float    fYScale = 1.0;


texture g_texScene;                         // ��������
texture g_texTextureBloom_DownSize_3x3;     // ��С�����ڴ�С 1/ 9 ����������ˮƽ����ģ��
texture g_texTextureBloom_DownSize_3x3_H ;  // 
texture g_texLastOutput;                    // ����������ʾ������
// 
// ��С�� 1 / 9 
float2 g_DownSizeOffset[ 4 ] =
  {
    float2( - 1 / 106.0 , - 1 / 68.0 ) , float2( - 1 / 106.0 , + 1 / 68.0 ) , float2( + 1 / 106.0 , - 1 / 68.0 ) , float2( 1 / 106.0 , 1 / 68.0 ) ,
  };

float g_Weight9[ 9 ] = // �����������
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
// ������Default 
// ���ܣ����ݾ�����Ⱦ����Ļ
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
// ������Bloom_Tech ʹ�ú���
// ˵�������ȱȽ�
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
// ������HBloom_Tech ʹ�ú���
// ˵����ˮƽģ��
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
// ������Bloom_Tech ʹ�ú���
// ˵������ֱģ��
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
// ����������С 1 / 3
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
    // ������Χ�ĸ��������
    for( int i = 0 ; i < 4 ; i++ )
      {
        v4Average += tex2D( TextureScene_SamplerIn , In.tex + g_DownSizeOffset[ i ] ) ;
      }
    v4Average =  v4Average * 0.25 ;
    //------------ �����õ� Shader -------------
    // ��ʹ���������ʱ��������ʹ���������ʱ����
    // v4Average = tex2D( TextureScene_SamplerIn , In.tex );
    // 
    if( dot( COLORTOGRAY , v4Average.xyz ) < 0.7 )
      {
        v4Average = float4( 0.0 , 0.0 , 0.0 , 0.0 );
      }
    return float4( v4Average.rgb , 1.0 ) ;
  }
//---------------------------------------------
// ������������
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
// ������Default
// ���ܣ���Ⱦ CFullWindow�������������ں�Ч����Ⱦ
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
// ������DownSizeBrightSet_Tech
// ���ܣ���С����������������ֵ������Ҫ Bloom ������ֵ
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
// ������HBloom_Tech
// ���ܣ�ˮƽģ��
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
// ������VBloom_Tech
// ���ܣ���ֱģ��
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
// ������LastOutPut_Tech
// ���ܣ��� Bloom ���
//-----------------------------------------
/*
technique LastOutPut_Tech
  {
    
  }
*/
