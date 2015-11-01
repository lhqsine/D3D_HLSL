//--------------------------------------------------------------------------------------
// File: 受光物体.fx
// 
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matWorldNormalInverseTranspose; 
float4   g_vEyePosition;            // 观察者位置
float4   g_vLightPosition ;         // 灯光位置

sampler2D MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
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
    //float3 Normal     : TEXCOORD0; 
    float2 TextureUV  : TEXCOORD1;  
    float2 Color      : TEXCOORD2;
  };


VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    In.Position.xyz = In.Position.xyz + 0.2 * tex2Dlod( MeshTextureSampler , In.Position + float4( sin( g_fTime ) , cos( g_fTime ) , 1.0 , 1.0 ));
    Out.Position = mul( In.Position , matWorldViewProject );
    Out.TextureUV = In.TextureUV;
    
    // 法向量
    float3 Normal = normalize( mul( In.Normal , ( float3x3 )g_matWorldNormalInverseTranspose ));
    float4 Position = mul( In.Position ,g_matWorld );
    float3 vLight = g_vLightPosition - Position ;
    // 每个顶点上的指向观察者的方向
    float3 vLookAt = normalize( Position - g_vEyePosition);
    //
    float fAmbient = 0.1f;
    // 高光因子
    float4   fSpecularColor  = float4( 1.0 ,  1.0 ,1.0 , 1.0 );
    // 漫反射光
    Out.Color.x  = 0.4 * dot( Normal, normalize( vLight ));
    // 高光
    Out.Color.y = pow( saturate( dot( reflect( normalize( vLight ) , Normal ), vLookAt )), 4 );
    //
    return Out;
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    // 环境光
    float fAmbient = 0.1f;
    return tex2D( MeshTextureSampler ,In.TextureUV ) * ( fAmbient + In.Color.x + In.Color.y  );
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        FillMode            = Solid;
        VertexShader = compile vs_3_0 RenderSceneVS( );
        PixelShader  = compile ps_3_0 RenderScenePS( );
      }
  }
