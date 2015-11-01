//--------------------------------------------------------------------------------------
// 文件：FileFX.fx
// 遮挡物体
//--------------------------------------------------------------------------------------

texture g_ColorTexture;             // 颜色纹理
texture g_RenderToTexture;          // 渲染到缓冲区的纹理

float    g_fTime;                   // 传入时间
float4x4 g_matWorld;                // 世界矩阵
float4x4 g_matView;                 // 观察矩阵
float4x4 g_matProject;              // 投影矩阵
//
float4x4 g_matWorldRTT;             // RTT 的世界矩阵
float4x4 g_matViewRTT;              // RTT 的观察矩阵
float4x4 g_matProjectRTT;           // RTT 的投影矩阵

float  g_fNear = 10.0f;
float  g_fFar  = 30.0f;

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

sampler MeshRTTSampler = sampler_state
  {
    Texture = <g_RenderToTexture>;
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
    float2 TextureUV  : TEXCOORD0;  
  };


struct VS_OUTPUT
  {
    float4 Position         : POSITION;   // 保存用户观察空间中的点的位置
    float2 TextureUV        : TEXCOORD0;  // 模型纹理坐标
    float4 PositionInLight  : TEXCOORD1;  // 保存处于灯光空间中的点的位置
    float2 Depth            : TEXCOORD2;  // 
    
  };

//-----------------------------------------------------------------------------
// 函数：TextureToFloat
// 功能：将某个位置的深度取出来
//-----------------------------------------------------------------------------
float TextureToFloat( float2 tex )
  {
    float4 fTexture  = tex2D( MeshRTTSampler ,tex ) ;
    float4 fFactor = float4( 16777216.0 / 16777216.0 ,65536.0 / 16777216.0 ,256.0 / 16777216.0 , 1 / 16777216.0 );
    float  fReturn = dot( fTexture , fFactor );
    return fReturn ;
  }

//---------------------------------------
// Default 技术使用
//---------------------------------------
VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    // 正常的变换流程
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    // 将顶点变换到光源为观察点的坐标空间
    float4x4 matLightWorldView = mul( g_matWorldRTT ,g_matViewRTT );
    float4x4 matLightWorldViewProject = mul( matLightWorldView ,g_matProjectRTT );
    Output.PositionInLight = mul( In.Position ,matLightWorldViewProject );
    Output.Depth = 0;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    float fShadow = 0.0;
    // 取深度位置
    float fDistance = In.PositionInLight.z / g_fFar ;
    // 投影空间坐标转换成纹理空间坐标。也就是找到投影空间中的点和纹理空间中的点的对应关系。
    // 从当前屏幕的像素点取纹理坐标，再从纹理上取距离。按照各点计算出在纹理上各像素点对应的距离，从 -1 到 +1 转换成 0 到 1
    float2 f2TextureUV = 0.5 * In.PositionInLight.xy / In.PositionInLight.w + float2( 0.5, 0.5 );
    f2TextureUV.y = 1.0f - f2TextureUV.y;
    // 取出两个值，
    float2 v2Moment = tex2D( MeshRTTSampler , f2TextureUV ).xy ;
    // 
    float fE_x2 = v2Moment.y ;
    float fEx_2 = v2Moment.x * v2Moment.x ;
    float fVariance = fE_x2 - fEx_2 ;
    float fMD = v2Moment.x - fDistance ;
    float fMD_2 = fMD * fMD;
    float fP = fVariance /( fVariance + fMD_2 );
    fShadow = max( fP , fDistance <= v2Moment.x );

    float4 vColor = tex2D( MeshTextureSampler ,In.TextureUV ) * ( fShadow + 0.2 );
    return vColor;
    
  }

//-----------------------------------------------------------------------------
// 渲染深度到纹理的 VS 和 PS
// 使用 R8G8B8 纹理来保存线性深度缓冲区
//-----------------------------------------------------------------------------
// 使用24位的深度缓冲区
/*
float4 FloatToTexture( float fIn ) 
  { 
    float fColor =  fIn ;
    // 11111111_11111111_11111111_11111111
    float fR ,fG ,fB ,fA;
    fColor = modf( fColor * 256 ,fR );
    fColor = modf( fColor * 256 ,fG );
    fColor = modf( fColor * 256 ,fB );
    fColor = modf( fColor * 256 , fA );
    return float4( fR / 256 , fG  / 256 , fB  / 256 , fA / 256 );
  }
*/
//---------------------------------------
// RenderDepthToTextureShader 技术使用
//---------------------------------------
VS_OUTPUT RenderDepthToTextureVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorldRTT ,g_matViewRTT );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProjectRTT );
    Output.Position = mul( In.Position ,matWorldViewProject );
    // 不需要纹理坐标
    Output.TextureUV = In.TextureUV;
    Output.Depth = Output.Position.zw;
    // 
    Output.PositionInLight = 0;
    return Output;    
  }

float4 RenderDepthToTexturePS( VS_OUTPUT In ) :COLOR0
  { 
    //float fDistance = ( In.Depth.x - g_fNear ) / ( g_fFar - g_fNear );
    float fDistance = In.Depth.x / g_fFar ;
    // 返回深度值和深度平方值
    float4 vColor = float4( fDistance , fDistance * fDistance , 0 , 0 ); //FloatToTexture( fDistance );
    return vColor;
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

technique RenderDepthToTextureShader
  {
    pass P0
      {
        FillMode = Solid;
        VertexShader = compile vs_2_0 RenderDepthToTextureVS( );
        PixelShader  = compile ps_2_0 RenderDepthToTexturePS( );
      }
  }

