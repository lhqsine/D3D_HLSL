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
    MipFilter = POINT;
    MinFilter = POINT;
    MagFilter = POINT;
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
    float2 Depth            : TEXCOORD2;  // 当前点在灯光矩阵中的深度值
    
  };

//-----------------------------------------------------------------------------
// 渲染模型 VS 和 PS
// 
//-----------------------------------------------------------------------------
float TextureToFloat( float2 tex )
  {
    float4 fTexture  = tex2D( MeshRTTSampler ,tex ) ;
    float3 fFactor = float3( 65536.0 / 65536.0 , 256.0 / 65536.0 ,1.0 / 65536.0 );
    float  fReturn = dot( fTexture.rgb , fFactor );
    //if( fReturn < 0.001 ) fReturn = 0.999;
    return fReturn ;
  }

// --------------------- Default Render -----------------------
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
    // 调试用
    // return tex2D( MeshTextureSampler , In.TextureUV );
    
    float fShadow = 1.0;
    float fDistance = In.PositionInLight.z / g_fFar  ;
    // 从纹理上取距离。按照各点计算出在纹理上各像素点对应的距离，从 -1 到 +1 转换成 0 到 1
    float2 f2TextureUV = 0.5 * In.PositionInLight.xy / In.PositionInLight.w + float2( 0.5, 0.5 );
    f2TextureUV.y = 1.0f - f2TextureUV.y;
    
    float fDistanceMap = TextureToFloat( f2TextureUV );
    // 
    // 
    // 在灯光位置观察坐标系下的各点距离
    if( fDistance - 0.009> fDistanceMap  ) // 表示在第一照射面后
      {
        fShadow = 0.4f;
      }
    float4 vColor = tex2D( MeshTextureSampler ,In.TextureUV ) * fShadow ;
    return vColor;
    
  }

//-----------------------------------------------------------------------------
// 渲染深度到纹理的 VS 和 PS
// 使用 R8G8B8 纹理来保存线性深度缓冲区
//-----------------------------------------------------------------------------
// 使用24位的深度缓冲区
float3 FloatToTexture( float fIn ) 
  { 
    float fColor =  fIn ;
    // 11111111_11111111_11111111_11111111
    float fR ,fG ,fB ;
    fColor = modf( fColor * 256 ,fR );
    fColor = modf( fColor * 256 ,fG );
    fColor = modf( fColor * 256 ,fB );
    //fColor = modf( fColor * 256 , fA );
    return float3( fR / 256 , fG  / 256 , fB  / 256 );
  }
//-------------------------------------------------
// 渲染到纹理使用的技术 RenderDepthToTextureShader
//-------------------------------------------------
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
    float fDistance = In.Depth.x / g_fFar ;
    float4 v4Color = tex2D( MeshTextureSampler , In.TextureUV );
    float4 v4Distance = float4( FloatToTexture( fDistance ) , 1.0 ) ;
    if( v4Color.a < 0.5 )
      {
        v4Distance.a = 0.0f;
      }
    else
      {
        v4Distance.a = 1.0f;
      }
    return v4Distance;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        CullMode         = None;
        AlphaBlendEnable = false;//true;
        AlphaTestEnable  = true;
        
        AlphaTestEnable    = true;
        AlphaRef           = 0x145;
        AlphaFunc          = Greater; 
        /*
        AlphaBlendEnable = true;
        BlendOp          = Add;
        //--------------------------------
        SrcBlend         = SrcColor;
        DestBlend        = InvSrcAlpha;
        */
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }

technique RenderDepthToTextureShader
  {
    pass P0
      {
        AlphaTestEnable  = true;
        AlphaRef           = 0x215;
        AlphaFunc          = Greater;  
        //BlendOp          = Add;
        //--------------------------------
        //SrcBlend         = SrcColor;
        //DestBlend        = InvSrcAlpha;
        //FillMode            = Solid;
        
        FillMode = Solid;
        VertexShader = compile vs_2_0 RenderDepthToTextureVS( );
        PixelShader  = compile ps_2_0 RenderDepthToTexturePS( );
      }
  }

