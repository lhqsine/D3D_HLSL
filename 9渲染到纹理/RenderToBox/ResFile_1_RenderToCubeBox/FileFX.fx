//--------------------------------------------------------------------------------------
// �ļ���FileFX.fx 
// ����������
//--------------------------------------------------------------------------------------

texture     g_ColorTexture;             // ��ɫ����
textureCUBE g_CubeTexture;          // ��Ⱦ��������������

float    g_fTime;                   // ����ʱ��
float4x4 g_matWorld;                // �������
float4x4 g_matView;                 // �۲����
float4x4 g_matProject;              // ͶӰ����
float4x4 g_matWorldNormalInverseTranspose ;// �������������ת��
//
float4   g_ViewPosition;

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

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
    float4 Position   : POSITION;  
    float3 Normal     : NORMAL;
    float2 TextureUV  : TEXCOORD0;  
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD0;  
    float3 TexCube    : TEXCOORD1;
    //float3 Normal     : TEXCOORD2;
  };

//---------------------- Default Render -----------------------
VS_OUTPUT RenderCubeSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    // ��������ʽ�任����
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    // ����������ϵ�б任������
    float3 v3Normal = normalize( mul( In.Normal , ( float3x3 )g_matWorldNormalInverseTranspose ));
    // ������任���㵽����ռ�
    float4 PositionInWorld = mul( In.Position , g_matWorld );
    // ����ÿ�������ϵĹ۲����������ɹ۲���ָ�򶥵�
    float3 v3View = normalize( PositionInWorld.xyz - g_ViewPosition.xyz );
    // ����ÿ�������ϵķ�������
    Output.TexCube = reflect( v3View , v3Normal );
    //Output.TexCube = 2 * dot(  v3View , v3Normal ) * v3Normal - v3View;
    // 
    return Output;    
  }

float4 RenderCubeScenePS( VS_OUTPUT In ) :COLOR0
  { 
    float3 v3CubeTex = float3( In.TexCube.x , In.TexCube.y , In.TexCube.z );
    float4 v4ColorOut = texCUBE( MeshCubeSampler , v3CubeTex );
    return v4ColorOut;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultCubeTech
  {
    pass P0
      {
        CullMode            = None;
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderCubeSceneVS( );
        PixelShader  = compile ps_2_0 RenderCubeScenePS( );
      }
  }

