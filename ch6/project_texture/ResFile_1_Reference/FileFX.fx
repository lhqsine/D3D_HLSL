//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matWorldProject;       // ����ͶӰ������������
float4x4 g_matViewProject;        // ����ͶӰ����Ĺ۲����
float4x4 g_matProjectProject;     // ����ͶӰ�����ͶӰ����

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    //ADDRESSU =  CLAMP;
    //ADDRESSV = CLAMP;
  };

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;  
    float2 TextureUV  : TEXCOORD0;  
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD0;  
    float4 ProjectUV  : TEXCOORD1;
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    // �����任����
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Position = mul( In.Position , matWorldViewProject );
    Out.TextureUV = In.TextureUV;
    // ͶӰ����
    float4x4 matWorldViewProjectTexture = mul( g_matWorldProject ,g_matViewProject );
    float4x4 matWorldViewProjectProjectTexture = mul( matWorldViewProjectTexture , g_matProjectProject );
    Out.ProjectUV = mul( In.Position ,matWorldViewProjectProjectTexture );
    Out.ProjectUV.y = - Out.ProjectUV.y;
    return Out;
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    float4 vColorBackGround = tex2D( MeshTextureSampler ,In.TextureUV ) ;
    // ������������
    float4x4 matOffset; // ƫ�ƾ���
    matOffset._11 = matOffset._22 = matOffset._33 = 0.5;
    matOffset._41 = matOffset._42 = matOffset._43 = 0.5;
    //
    matOffset._12 = matOffset._13 = matOffset._14 = 0.0;
    matOffset._21 = matOffset._23 = matOffset._24 = 0.0;
    matOffset._31 = matOffset._32 = matOffset._34 = 0.0;
    matOffset._44 = 1.0;
    float4 Offset = mul( In.ProjectUV , matOffset );
    float4 vColor = tex2Dproj( MeshTextureSampler ,Offset );
    // ���������ɫ��
    return vColorBackGround * 0.3 + vColor * 0.9;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        FillMode            = Solid;
        AddressU[ 0 ]       = Border;
        AddressV[ 0 ]       = Border;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
