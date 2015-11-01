// ---------------------------------------------------------------
// Ч���ļ�
 
// ---------------------------------------------------------------

texture texColorTexture;      // ��������

float4x4 g_matInvView;        // �۲���������
float    g_fTime ;            // ʱ��
float4   g_v4Viewport;        // ���ڴ�С
//
const float4 g_v3BackGroundColor  = float4( 0.0 , 0.0 , 0.0 , 1.0 );
const float3 g_v3LightPosition    = float3( 10.0 , 10.0 , -10.0 );
const float4 g_v4LightColor       = float4( 1.0 , 1.0 , 1.0 , 1.0 );
const float  ESP = 0.001;    // �����ݲ�

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

struct RAY              // ����׷�ٽṹ
  {
    float3 s_v3Orient;    // ԭ��
    float3 v3EyeDirection; // ���߷���
  };

struct SPHEREOBJECT   
  {
    float3 s_v3Center;        // ����
    float  s_fRad2;           // �洢ģ�Ͱ뾶��ƽ��
    float4 s_v4Color;         // ģ�͵���ɫ
    float  s_Kd, s_Ks, s_Kr;  // ģ���ϵļ����ؼ�
  };

// 
#define SQR( N ) ( N * N )
#define NOBJECTS 4
static SPHEREOBJECT g_SphereObject[ NOBJECTS ] =  
  {
    // ��������
      {
        0.0 , 0.0 , 0.0 ,
        SQR( 1.0 ) ,
        0.0 , 0.5 , 1.0 , 1.0,
        1.0 , 1.0 , 0.5 ,
      },
    // ת����С��
      {
        1.5 , -0.5 , 0.0 ,
        SQR( 0.5 ) ,
        0.0 , 1.0 , 0.0 , 1.0 ,
        1.0 , 1.0 , 0.5 ,
      },
    // ת����С��2
      {
        1.5 , +1.3 , 0.0,
        SQR( 0.3 ),
        1.0 , 1.0 , 0.0 , 1.0 ,
        1.0 , 1.0 , 0.5 ,
      },
    // �������
      {
        0.0 , -101.0 , 0.0 ,
        SQR( 100.0 ),
        1.0 , 1.0 , 1.0 , 1.0 ,
        1.3 , 0.2 , 0.5 ,
      }
  };

//----------------------------------------------------
// ������SphereIntersect
// ���ܣ���
// �����������Ǹ�ģ�ͣ�����⣬�����Ƿ��ཻ��
//----------------------------------------------------
float SphereIntersect( SPHEREOBJECT SphereObject , RAY Ray , out bool bHit )
  {
    float3 v3OrientToSphere = SphereObject.s_v3Center - Ray.s_v3Orient;   // �ӵ㵽���ĵ�����
    float fProjectToLight = dot( v3OrientToSphere , Ray.v3EyeDirection ); // �ӵ㵽���ĵ������ڹ��߷����ͶӰ
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
// ������SphereNormal
// ���ܣ�������ģ�ͽ���ķ�����
// ���������������ģ�ͣ�����Ŀռ�λ�ã���������㴦�ķ�����
//----------------------------------------------------
float3 SphereNormal( SPHEREOBJECT SphereObject , float3 v3HitPoint )
  {
    return normalize( v3HitPoint - SphereObject.s_v3Center );
  }
//----------------------------------------------------
// ������NearestHit
// ���ܣ�������Ľ���
// ������������������ߣ����ཻ���ģ��������������Ƿ��ཻ������
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
// ������AnyHit
// ���ܣ���ÿ������ģ����ѭ�����鿴�Ƿ��н���
// ������������ߵĽṹ
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
// ������Phong
// ���ܣ�ʹ�÷��Ϸ�ʽ�������
// ���������������������ģ�ͽ���ķ�����������⣬���򣬸߹���ˣ������䣬�߹�����
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
// ������Shade
// ���ܣ��������
// ���������������������ģ�ͽ����λ�ã�������������⣬�����Ƿ���ģ���ཻ
//----------------------------------------------------
float4 Shade( float3 v3HitPoint , float3 v3Normal , float3 v3Direction , int iHitObj )
  {
    float3 v3LightIn = normalize( g_v3LightPosition - v3HitPoint );

    // �Ƿ�����Ӱ��
    RAY RayShadow;
    RayShadow.s_v3Orient = v3HitPoint;
    RayShadow.v3EyeDirection = v3LightIn;
    bool bShadowed = AnyHit( RayShadow );

    // �ƹ�
    float4 v4Diffuse = g_SphereObject[ iHitObj ].s_v4Color * g_SphereObject[ iHitObj ].s_Kd;
    if ( iHitObj == 3 ) 
      {
        v4Diffuse *= tex2Dlod( ColorMapSampler , float4( v3HitPoint.xz / 8 , 0.0 , 0.0 ));    // ��������
      }
    float4 v4Spec = g_v4LightColor * g_SphereObject[ iHitObj ].s_Ks;
    float  fShadowFactor = 0.25 + 0.75 * !bShadowed;    
    
    return Phong( v3Normal , v3LightIn , v3Direction , 40.0 , v4Diffuse , v4Spec ) * fShadowFactor;
  }
//----------------------------------------------------
// ������animate_scene
// ���ܣ�������С���˶�����
// ���������������ʱ��
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
// ������MainPS_Screen
// ���ܣ�
// ������
//----------------------------------------------------
float4 MainPS_Screen( VS_OUTPUT In ):COLOR
  {
    animate_scene( g_fTime );    
    // �����۾������Ĺ���
    float3 v3EyeDirection;
    v3EyeDirection.xy = (( In.tex * 2.0 ) - 1.0 ) * g_v4Viewport;
    v3EyeDirection.y = -v3EyeDirection.y;    // ��ת y ����
    v3EyeDirection.z = 600.0;              // ׷�ٵ�������Ϊ500

    // �任�۲����
    RAY RayFromEye;
    RayFromEye.s_v3Orient = mul( float4( 0 , 0 , 0 , 1 ) , g_matInvView );
    RayFromEye.v3EyeDirection = mul( v3EyeDirection , ( float3x3 )g_matInvView );
    RayFromEye.v3EyeDirection = normalize( RayFromEye.v3EyeDirection );

    // �ҵ�����ĵ�
    int iHitObj;
    bool bHit;
    float3 v3HitPoint = NearestHit( RayFromEye , iHitObj , bHit );
    float4 v4Color = g_v3BackGroundColor;
    if( bHit )   
      {
        // �������
        float3 v3Normal = SphereNormal( g_SphereObject[ iHitObj ] , v3HitPoint );
        v4Color = Shade( v3HitPoint , v3Normal , RayFromEye.v3EyeDirection , iHitObj );
        
        // �������
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

//--------------------------- ����---------------------------
technique Default
  {
    pass P0
      {
        VertexShader = compile vs_3_0 MainVS_Screen();
        PixelShader  = compile ps_3_0 MainPS_Screen();
      }
  }

