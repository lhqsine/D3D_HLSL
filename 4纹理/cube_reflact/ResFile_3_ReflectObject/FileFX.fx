//--------------------------------------------------------------------------------------
// Cube
// �������� 
//--------------------------------------------------------------------------------------

textureCUBE g_CubeTexture;

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;
float4x4 g_matWorldNormalInverse;

float4   g_ViewPosition;              // �۲�λ��

samplerCUBE MeshCubeSampler = sampler_state
  {
    Texture = <g_CubeTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float3 Position   : POSITION;   
    float3 Normal     : NORMAL;
    float2 TextureUV  : TEXCOORD0;  
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD0;  
    float3 ReflectInVertex : TEXCOORD1;    
    float3 Normal       : TEXCOORD2;
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output = ( VS_OUTPUT )0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( float4( In.Position , 1.0 ) , matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    // 
    Output.Normal =  normalize( mul( In.Normal ,( float3x3 )g_matWorldNormalInverse ));
    float4 PositionInWorld = mul( float4( In.Position , 1.0 ) ,g_matWorld );
    // ÿ�������ϵ������������Ǵӹ۲�λ��ָ�򶥵㣬�����ɶ���ָ��۲�λ��
    Output.ReflectInVertex = PositionInWorld.xyz - g_ViewPosition.xyz ;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    // ��ÿ��������ָ��۲�λ�õķ���������������ָ����������ͼ��ʵ������Ϊ��������
    float3 vReflect = reflect( In.ReflectInVertex , In.Normal );
    float4 vColor = texCUBE( MeshCubeSampler , vReflect.xyz );
    return float4( vColor.xyz , 1.0 );
  }

float4 RenderScenePS_Line( VS_OUTPUT In ) :COLOR0
  { 
    // 
    float4 vColor = float4( 1.0 , 0.0 , 0.0 , 1.0 );
    return vColor;
  }
//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        CullMode           = CCW;
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
    /*
    pass P1
      {
        CullMode           = CCW;
        FillMode            = WireFrame;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS_Line( );
      }
      */
  }
