// ---------------------------------------------------------------
// 效果文件
 
// ---------------------------------------------------------------

texture texColorTexture;      // 地面纹理

float4x4 g_matInvView;        // 观察矩阵的逆阵
float    g_fTime ;            // 时间
float4   g_v4Viewport;        // 窗口大小
//
const float4 g_v3BackGroundColor  = float4( 0.0 , 0.0 , 0.0 , 1.0 );
const float3 g_v3LightPosition    = float3( 10.0 , 10.0 , -10.0 );
const float4 g_v4LightColor       = float4( 1.0 , 1.0 , 1.0 , 1.0 );
const float  ESP = 0.001;    // 错误容差

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

sampler ColorMapSampler = sampler_state
  {
    Texture   = ( texColorTexture );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

//------------------------ Default --------------------------
VS_OUTPUT MainVS_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    In.pos.x = sign( In.pos.x ) ;
    In.pos.y = sign( In.pos.y ) ;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.9 , 1 );
    Out.tex       = In.tex;
    Out.color = In.color;
    return Out;
  }

struct RAY              // 光线追踪结构
  {
    float3 s_v3Orient;    // 原点
    float3 v3EyeDirection; // 光线方向
  };

struct SPHEREOBJECT   
  {
    float3 s_v3Center;        // 中心
    float  s_fRad2;           // 存储模型半径的平方
    float4 s_v4Color;         // 模型的颜色
    float  s_Kd, s_Ks, s_Kr;  // 模型上的几个关键
  };

// 
#define SQR( N ) ( N * N )
#define NOBJECTS 4
static SPHEREOBJECT g_SphereObject[ NOBJECTS ] =  
  {
    // 中心球体
      {
        0.0 , 0.0 , 0.0 ,
        SQR( 1.0 ) ,
        0.0 , 0.5 , 1.0 , 1.0,
        1.0 , 1.0 , 0.5 ,
      },
    // 转动的小球
      {
        1.5 , -0.5 , 0.0 ,
        SQR( 0.5 ) ,
        0.0 , 1.0 , 0.0 , 1.0 ,
        1.0 , 1.0 , 0.5 ,
      },
    // 转动的小球2
      {
        1.5 , +1.3 , 0.0,
        SQR( 0.3 ),
        1.0 , 1.0 , 0.0 , 1.0 ,
        1.0 , 1.0 , 0.5 ,
      },
    // 地面大球
      {
        0.0 , -101.0 , 0.0 ,
        SQR( 100.0 ),
        1.0 , 1.0 , 1.0 , 1.0 ,
        1.3 , 0.2 , 0.5 ,
      }
  };

//----------------------------------------------------
// 函数：SphereIntersect
// 功能：求交
// 参数：传入那个模型，入射光，返回是否相交，
//----------------------------------------------------
float SphereIntersect( SPHEREOBJECT SphereObject , RAY Ray , out bool bHit )
  {
    float3 v3OrientToSphere = SphereObject.s_v3Center - Ray.s_v3Orient;   // 视点到球心的向量
    float fProjectToLight = dot( v3OrientToSphere , Ray.v3EyeDirection ); // 视点到球心的向量在光线方向的投影
    // 
    float disc = ( fProjectToLight * fProjectToLight ) + SphereObject.s_fRad2 - dot( v3OrientToSphere , v3OrientToSphere ); //

    bHit = true;
    if ( disc <= 0 ) bHit = false;;

    disc = sqrt( disc );
    float t2 = fProjectToLight + disc;
    if ( t2 <= ESP ) bHit = false; 

    float t1 = fProjectToLight - disc;

    if (( t1 > ESP ) && ( t1 < t2 ))  
      return t1;
    else
      return t2;
  }
//----------------------------------------------------
// 函数：SphereNormal
// 功能：光线与模型交点的法向量
// 参数：输入参数：模型，交点的空间位置，输出：交点处的法向量
//----------------------------------------------------
float3 SphereNormal( SPHEREOBJECT SphereObject , float3 v3HitPoint )
  {
    return normalize( v3HitPoint - SphereObject.s_v3Center );
  }
//----------------------------------------------------
// 函数：NearestHit
// 功能：求最近的交点
// 参数：输入参数：光线，有相交点的模型索引；输出：是否相交，交点
//----------------------------------------------------
float3 NearestHit( RAY Ray , out int iHitObj , out bool bAnyHit )
  {
    float fMint = 1e10;
    iHitObj = -1;
    bAnyHit = false;
    for( int iOject = 0 ; iOject < NOBJECTS ; iOject++ )  
      {
        bool hit;
        float fDistance = SphereIntersect( g_SphereObject[ iOject ] , Ray , hit );
        if ( hit )   
          {
            if ( fDistance < fMint )  
              {
                iHitObj = iOject;
                fMint = fDistance;
                bAnyHit = true;
              }
          }
      }
    return Ray.s_v3Orient + Ray.v3EyeDirection * fMint;
  }
//----------------------------------------------------
// 函数：AnyHit
// 功能：在每个场景模型中循环，查看是否有交点
// 参数：入射光线的结构
//----------------------------------------------------
bool AnyHit( RAY Ray )
  {
    bool bAnyHit = false;
    for( int iObject = 0 ; iObject < NOBJECTS ; iObject++ ) 
      {
        bool bHit;
        float fTemp = SphereIntersect( g_SphereObject[ iObject ] , Ray , bHit );
        if ( bHit ) 
          {
            bAnyHit = true;
          }
      }
    return bAnyHit;
  }
//----------------------------------------------------
// 函数：Phong
// 功能：使用冯氏方式计算光照
// 参数：输入参数：光线与模型交点的法向量，入射光，方向，高光因此，漫反射，高光因子
//----------------------------------------------------
float4 Phong( float3 v3Normal , float3 v3LightIn , float3 v3Direction , float fShininess , float4 v4Diffuse , float4 v4Spec )
  {
    float  fDiffuse   = dot( v3Normal , v3LightIn );
    float  fDiff      = saturate( fDiffuse );
    float3 v3Reflect  = reflect( v3LightIn , v3Normal );
    float  fSpec      = pow( saturate( dot( v3Direction , v3Reflect )) , fShininess ) * ( fDiffuse > 0.0 );
    return fDiff * v4Diffuse + fSpec * v4Spec;
  }
//----------------------------------------------------
// 函数：Shade
// 功能：计算光照
// 参数：输入参数：光线与模型交点的位置，法向量，入射光，方向，是否与模型相交
//----------------------------------------------------
float4 Shade( float3 v3HitPoint , float3 v3Normal , float3 v3Direction , int iHitObj )
  {
    float3 v3LightIn = normalize( g_v3LightPosition - v3HitPoint );

    // 是否在阴影中
    RAY RayShadow;
    RayShadow.s_v3Orient = v3HitPoint;
    RayShadow.v3EyeDirection = v3LightIn;
    bool bShadowed = AnyHit( RayShadow );

    // 灯光
    float4 v4Diffuse = g_SphereObject[ iHitObj ].s_v4Color * g_SphereObject[ iHitObj ].s_Kd;
    if ( iHitObj == 3 ) 
      {
        v4Diffuse *= tex2Dlod( ColorMapSampler , float4( v3HitPoint.xz / 8 , 0.0 , 0.0 ));    // 地面纹理
      }
    float4 v4Spec = g_v4LightColor * g_SphereObject[ iHitObj ].s_Ks;
    float  fShadowFactor = 0.25 + 0.75 * !bShadowed;    
    
    return Phong( v3Normal , v3LightIn , v3Direction , 40.0 , v4Diffuse , v4Spec ) * fShadowFactor;
  }
//----------------------------------------------------
// 函数：animate_scene
// 功能：将两个小球运动起来
// 参数：传入参数：时间
//----------------------------------------------------
void animate_scene( float time )
  {
    g_SphereObject[ 1 ].s_v3Center.x = sin( time ) * 1.5;
    g_SphereObject[ 1 ].s_v3Center.z = cos( time ) * 1.5;
    //
    g_SphereObject[ 2 ].s_v3Center.x = sin( time + 2 * 3.14 / 4 ) * 1.5;
    g_SphereObject[ 2 ].s_v3Center.z = cos( time + 2 * 3.14 / 4 ) * 1.5;
  }
//----------------------------------------------------
// 函数：MainPS_Screen
// 功能：
// 参数：
//----------------------------------------------------
float4 MainPS_Screen( VS_OUTPUT In ):COLOR
  {
    animate_scene( g_fTime );    
    // 计算眼睛发出的光线
    float3 v3EyeDirection;
    v3EyeDirection.xy = (( In.tex * 2.0 ) - 1.0 ) * g_v4Viewport;
    v3EyeDirection.y = -v3EyeDirection.y;    // 反转 y 坐标
    v3EyeDirection.z = 600.0;              // 追踪的最大距离为500

    // 变换观察矩阵
    RAY RayFromEye;
    RayFromEye.s_v3Orient = mul( float4( 0 , 0 , 0 , 1 ) , g_matInvView );
    RayFromEye.v3EyeDirection = mul( v3EyeDirection , ( float3x3 )g_matInvView );
    RayFromEye.v3EyeDirection = normalize( RayFromEye.v3EyeDirection );

    // 找到最近的点
    int iHitObj;
    bool bHit;
    float3 v3HitPoint = NearestHit( RayFromEye , iHitObj , bHit );
    float4 v4Color = g_v3BackGroundColor;
    if( bHit )   
      {
        // 表面光照
        float3 v3Normal = SphereNormal( g_SphereObject[ iHitObj ] , v3HitPoint );
        v4Color = Shade( v3HitPoint , v3Normal , RayFromEye.v3EyeDirection , iHitObj );
        
        // 反射光线
        float3 v4Reflect = reflect( RayFromEye.v3EyeDirection , v3Normal );
        RAY RayFromObject;
        RayFromObject.s_v3Orient = v3HitPoint;
        RayFromObject.v3EyeDirection = v4Reflect;
        int hitobj2;
        v3HitPoint = NearestHit( RayFromObject , hitobj2 , bHit );
        if ( bHit )   
          {
            v3Normal = SphereNormal( g_SphereObject[ hitobj2 ] , v3HitPoint );
            v4Color += Shade( v3HitPoint , v3Normal , RayFromObject.v3EyeDirection , hitobj2 ) * g_SphereObject[ iHitObj ].s_Kr;
          } 
        else   
          {
            v4Color += g_v3BackGroundColor;
          }
        
      }
    return v4Color;
  }

float4 MainPS_Screen_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 1.0 ,1.0 ,0.0 ,0.0 );
  }

//--------------------------- 技术---------------------------
technique Default
  {
    pass P0
      {
        VertexShader = compile vs_3_0 MainVS_Screen();
        PixelShader  = compile ps_3_0 MainPS_Screen();
      }
  }

