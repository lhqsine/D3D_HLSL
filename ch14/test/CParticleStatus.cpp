#include "CParticleStatus.h"

extern D3DXVECTOR4             g_DebugInfo[ 4 ];

//----------------------------------
// 函数：SSTATUS
// 功能：构造函数
//----------------------------------
CPARTICLESTATUS::CPARTICLESTATUS( )
  {
    m_iCount = 0;
    m_v4ReturnColor = D3DXVECTOR4( 1.0 , 1.0 , 1.0 , 1.0 );
  }
//----------------------------------
// 函数：~SSTATUS
// 功能：析构函数
//----------------------------------
CPARTICLESTATUS::~CPARTICLESTATUS( )
  {
  }

//----------------------------------
// 函数：RandVector3
// 功能：随机设置一个向量，
// 输入：中心位置，在此中心位置的向量的大小
//----------------------------------
D3DXVECTOR3 RandVector3( D3DXVECTOR3 v3Center , D3DXVECTOR3 v3Radius )
  {
    //srand( unsigned( time( 0 )));
    #define RAND  (( float )rand( ) / RAND_MAX ) 
    D3DXVECTOR3 v3Return;
    v3Return.x = ( RAND - 0.5 ) * v3Radius.x + v3Center.x ;
    v3Return.y = ( RAND - 0.5 ) * v3Radius.y + v3Center.y ;
    v3Return.z = ( RAND - 0.5 ) * v3Radius.z + v3Center.z ;
    return v3Return ;
  }
//----------------------------------
// 函数：RandFloat
// 功能：随机设置浮点量，
// 输入：中心位置，浮点数的半径
//----------------------------------
float RandFloat( float fCenter , float fRadius )
  {
    #define RAND  (( float )rand( ) / RAND_MAX ) 
    return fCenter + RAND * fRadius;
  }

//----------------------------------
// 函数：ResetPositionColor
// 功能：复位位置和颜色
// 注意：m_fTime 不初始化，其他物理量也不复位，只重新设置位置和速度和颜色
//----------------------------------
void CPARTICLESTATUS::ResetPositionColor( )
  {
    //m_fTime = RandFloat( 3.0 , DURATIONTIME / 2.0 );                    // 时间范围[ 1 - 5 ]
    m_fLifeTimeWeak = RandFloat( 3.0 , 0.0 );                       // 衰弱开始[ 3 ]
    //m_fLifeTimeEnd  = RandFloat( 4.0 , 0.0 );                       // 衰弱开始[ 4 ]
    m_v4ReturnColor = D3DXVECTOR4( 1.0 , 1.0 , 1.0 , 1.0 );         // 颜色为白色
    // 初始化位置
    D3DXVECTOR3 v3Position( 0.0 , 4.0 , 0.0 );                      // 
    D3DXVECTOR3 v3Radius( 0.0 , 2.0 , 0.0 );                        // 范围[ 0 - 2 ]
    m_MSCurrent.v3Position = RandVector3( v3Position , v3Radius );
    // 速度
    D3DXVECTOR3 v3Velocity( 0.0 , 7.20 , 0.0 );   
    D3DXVECTOR3 v3V( 5.10 , 5.10 , 5.10 );     
    m_MSCurrent.v3Velocity = RandVector3( v3Velocity , v3V );     // 设置速度
    m_MSNext = m_MSCurrent;
    //
  }
//----------------------------------
// 函数：InitPositionColor
// 功能：初始化位置和颜色
//----------------------------------
void CPARTICLESTATUS::InitPositionColor( ) // 在此初始化粒子的三个时间量
  {
    m_fTime = RandFloat( 4.0 , DURATIONTIME  );                     // 时间范围[ 0 - 8 ]
    m_fLifeTimeWeak = RandFloat( 3.0 , 0.0 );                       // 衰弱开始[ 3 ]
    m_fLifeTimeEnd  = RandFloat( 3.0 , 1.0 );                       // 衰弱开始[ 4 ]
    m_v4ReturnColor = D3DXVECTOR4( 1.0 , 1.0 , 1.0 , 1.0 );         // 颜色为白色
    // 初始化位置
    D3DXVECTOR3 v3Position( 0.0 , 4.0 , 0.0 );                      // 
    D3DXVECTOR3 v3Radius( 0.0 , 2.0 , 0.0 );                        // 范围[ 0 - 2 ]
    m_MSCurrent.v3Position = RandVector3( v3Position , v3Radius );
    // 速度
    D3DXVECTOR3 v3Velocity( 0.0 , 7.20 , 0.0 );   
    D3DXVECTOR3 v3V( 1.10 , 1.10 , 1.10 );     
    m_MSCurrent.v3Velocity = RandVector3( v3Velocity , v3V );     // 设置速度
    // 阻尼
    m_MSCurrent.V3DAMP = D3DXVECTOR3( 0.3 , 0.3 , 0.3 ); 
    // 加速度
    m_MSCurrent.v3Acceleration = D3DXVECTOR3( 0.0 , -9.8 , 0.0 );  // 设置加速度
    //
    m_MSNext = m_MSCurrent;
  }
//----------------------------------
// 函数：OnFrameMove
// 功能：计算最终输出的两个结果，位置和颜色
// 
//----------------------------------
void CPARTICLESTATUS::OnFrameMove( double fInTime , float fElapsedTime , int i )
  {
    // 首先将系统时间转换成模块内的时间
    static float sfTimeStart = fInTime + m_fTime;
    float  fTime = fInTime - sfTimeStart;
    // 计算动画次数
    int iCount = int ( fTime / m_fLifeTimeEnd );
    if( iCount > m_iCount )
      {
        m_iCount = m_iCount + 1;
        ResetPositionColor( );
      }
    // 可以按以粒子生命开始为原点的坐标计量时间
    m_fCurrentTimeInParticle = fTime - m_iCount * m_fLifeTimeEnd ;
    m_MSNext = RK4( &m_MSCurrent , fTime , fElapsedTime );
    if( m_MSNext.v3Position.y < 0 )
      {
        m_MSNext.v3Velocity.y = -m_MSNext.v3Velocity.y ;
      }
    m_MSCurrent = m_MSNext ;
    // 设置颜色
    float fColor ;
    if( m_fCurrentTimeInParticle > m_fLifeTimeWeak ) // 将亮度逐渐按线性方式降低
      {
        fColor = ( m_fCurrentTimeInParticle - m_fLifeTimeWeak ) / ( m_fLifeTimeEnd - m_fLifeTimeWeak );
        m_v4ReturnColor.x = 1.0 - fColor;
      }
    // 调试使用
    if( 0 == i )
      {
        g_DebugInfo[ 0 ].x = m_v4ReturnColor.x ;  
        g_DebugInfo[ 0 ].y = m_fLifeTimeEnd ;  
        g_DebugInfo[ 0 ].z = m_fCurrentTimeInParticle ;  
        g_DebugInfo[ 0 ].w = fColor ;  
      }
    else if( 1 == i )
      {
        g_DebugInfo[ 1 ].x = m_v4ReturnColor.x ;  
        g_DebugInfo[ 1 ].y = m_fLifeTimeEnd ;  
        g_DebugInfo[ 1 ].z = m_fCurrentTimeInParticle ;  
        g_DebugInfo[ 1 ].w = fColor ;  
      }
    else if( 2 == i )
      {
        g_DebugInfo[ 2 ].x = m_v4ReturnColor.x ;  
        g_DebugInfo[ 2 ].y = m_fLifeTimeEnd ;  
        g_DebugInfo[ 2 ].z = m_fCurrentTimeInParticle ;  
        g_DebugInfo[ 2 ].w = fColor ;  
      }
    else if( 3 == i )
      {
        g_DebugInfo[ 3 ].x = m_v4ReturnColor.x ;  
        g_DebugInfo[ 3 ].y = m_fLifeTimeEnd ;  
        g_DebugInfo[ 3 ].z = m_fCurrentTimeInParticle ;  
        g_DebugInfo[ 3 ].w = fColor ;  
      }
    //
  }

//----------------------------------
// 函数：GetPosition
// 功能：返回位置
//----------------------------------
D3DXVECTOR4 CPARTICLESTATUS::GetPosition( )
  {
    return m_MSCurrent.v3Position ;
  }

//----------------------------------
// 函数：GetColor
// 功能：返回颜色
//----------------------------------
D3DXVECTOR4 CPARTICLESTATUS::GetColor( )
  {
    return m_v4ReturnColor ;
  }

