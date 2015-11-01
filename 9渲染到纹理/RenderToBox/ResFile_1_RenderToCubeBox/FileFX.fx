//--------------------------------------------------------------------------------------
// 文件：FileFX.fx 
// 坐标轴物体
//--------------------------------------------------------------------------------------

texture     g_ColorTexture;             // 颜色纹理
textureCUBE g_CubeTexture;          // 渲染到缓冲区的纹理

float    g_fTime;                   // 传入时间
float4x4 g_matWorld;                // 世界矩阵
float4x4 g_matView;                 // 观察矩阵
float4x4 g_matProject;              // 投影矩阵
float4x4 g_matWorldNormalInverseTranspose ;// 世界矩阵的逆阵的转置
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
    // 按正常方式变换顶点
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    // 在世界坐标系中变换法向量
    float3 v3Normal = normalize( mul( In.Normal , ( float3x3 )g_matWorldNormalInverseTranspose ));
    // 将顶点变换顶点到世界空间
    float4 PositionInWorld = mul( In.Position , g_matWorld );
    // 计算每个顶点上的观察向量，即由观察者指向顶点
    float3 v3View = normalize( PositionInWorld.xyz - g_ViewPosition.xyz );
    // 计算每个顶点上的反射向量
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

