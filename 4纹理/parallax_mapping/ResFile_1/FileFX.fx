//--------------------------------------------------------------------------------------
// File: �ܹ�����.fx
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh
texture g_NormalTexture;
texture g_HeightTexture;

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4   g_vEyePosition;            // �۲���λ��
float4   g_vLightPosition ;         // �ƹ�λ��

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

sampler MeshNormalSampler = sampler_state
  {
    Texture = <g_NormalTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };
sampler MeshHeightSampler = sampler_state
  {
    Texture = <g_HeightTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;  
    float3 Normal     : NORMAL; 
    float3 Tanget     : TANGENT;              // ģ�Ͷ������������
    float2 TextureUV  : TEXCOORD0;  
    //
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD1;  
    float3 vLookAt    : TEXCOORD3;       // ģ����ÿ��������ָ��۲��ߵķ���
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Position = mul( In.Position , matWorldViewProject );
    Out.TextureUV = In.TextureUV;
    //----------------- �ڴ˼���һ���任���� ---------------------
    // ����������任�����߿ռ�ϵ�ı任����
    float3x3 matWorldToModel = float3x3 (
      mul( In.Tanget ,                   g_matWorld ).xyz ,
      mul( cross( In.Tanget,In.Normal ), g_matWorld ).xyz ,
      mul( In.Normal ,                   g_matWorld ).xyz );

    // 
    float4 Position = mul( In.Position ,g_matWorld );
    // ÿ�������ϵ�ָ��۲��ߵķ���
    Out.vLookAt = mul ( matWorldToModel ,normalize( Position - g_vEyePosition ));
    return Out;
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    // �����Ӳ���ͼ��ƫ��
    float3 ViewDir = normalize( In.vLookAt );
    float2 OffsetUV = In.TextureUV ;
    float3 h = tex2D( MeshHeightSampler ,In.TextureUV ).y ;
    OffsetUV += float2( -ViewDir.x , ViewDir.y ) * ( h * 0.04 - 0.02 );
    float4 texColor = tex2D( MeshTextureSampler ,OffsetUV );
    // 
    return texColor  ;
    
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
