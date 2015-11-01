#include "CParticleStatus.h"

extern D3DXVECTOR4             g_DebugInfo[ 4 ];

//----------------------------------
// ������SSTATUS
// ���ܣ����캯��
//----------------------------------
CPARTICLESTATUS::CPARTICLESTATUS( )
  {
    m_iCount = 0;
    m_v4ReturnColor = D3DXVECTOR4( 1.0 , 1.0 , 1.0 , 1.0 );
  }
//----------------------------------
// ������~SSTATUS
// ���ܣ���������
//----------------------------------
CPARTICLESTATUS::~CPARTICLESTATUS( )
  {
  }

//----------------------------------
// ������RandVector3
// ���ܣ��������һ��������
// ���룺����λ�ã��ڴ�����λ�õ������Ĵ�С
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
// ������RandFloat
// ���ܣ�������ø�������
// ���룺����λ�ã��������İ뾶
//----------------------------------
float RandFloat( float fCenter , float fRadius )
  {
    #define RAND  (( float )rand( ) / RAND_MAX ) 
    return fCenter + RAND * fRadius;
  }

//----------------------------------
// ������ResetPositionColor
// ���ܣ���λλ�ú���ɫ
// ע�⣺m_fTime ����ʼ��������������Ҳ����λ��ֻ��������λ�ú��ٶȺ���ɫ
//----------------------------------
void CPARTICLESTATUS::ResetPositionColor( )
  {
    //m_fTime = RandFloat( 3.0 , DURATIONTIME / 2.0 );                    // ʱ�䷶Χ[ 1 - 5 ]
    m_fLifeTimeWeak = RandFloat( 3.0 , 0.0 );                       // ˥����ʼ[ 3 ]
    //m_fLifeTimeEnd  = RandFloat( 4.0 , 0.0 );                       // ˥����ʼ[ 4 ]
    m_v4ReturnColor = D3DXVECTOR4( 1.0 , 1.0 , 1.0 , 1.0 );         // ��ɫΪ��ɫ
    // ��ʼ��λ��
    D3DXVECTOR3 v3Position( 0.0 , 4.0 , 0.0 );                      // 
    D3DXVECTOR3 v3Radius( 0.0 , 2.0 , 0.0 );                        // ��Χ[ 0 - 2 ]
    m_MSCurrent.v3Position = RandVector3( v3Position , v3Radius );
    // �ٶ�
    D3DXVECTOR3 v3Velocity( 0.0 , 7.20 , 0.0 );   
    D3DXVECTOR3 v3V( 5.10 , 5.10 , 5.10 );     
    m_MSCurrent.v3Velocity = RandVector3( v3Velocity , v3V );     // �����ٶ�
    m_MSNext = m_MSCurrent;
    //
  }
//----------------------------------
// ������InitPositionColor
// ���ܣ���ʼ��λ�ú���ɫ
//----------------------------------
void CPARTICLESTATUS::InitPositionColor( ) // �ڴ˳�ʼ�����ӵ�����ʱ����
  {
    m_fTime = RandFloat( 4.0 , DURATIONTIME  );                     // ʱ�䷶Χ[ 0 - 8 ]
    m_fLifeTimeWeak = RandFloat( 3.0 , 0.0 );                       // ˥����ʼ[ 3 ]
    m_fLifeTimeEnd  = RandFloat( 3.0 , 1.0 );                       // ˥����ʼ[ 4 ]
    m_v4ReturnColor = D3DXVECTOR4( 1.0 , 1.0 , 1.0 , 1.0 );         // ��ɫΪ��ɫ
    // ��ʼ��λ��
    D3DXVECTOR3 v3Position( 0.0 , 4.0 , 0.0 );                      // 
    D3DXVECTOR3 v3Radius( 0.0 , 2.0 , 0.0 );                        // ��Χ[ 0 - 2 ]
    m_MSCurrent.v3Position = RandVector3( v3Position , v3Radius );
    // �ٶ�
    D3DXVECTOR3 v3Velocity( 0.0 , 7.20 , 0.0 );   
    D3DXVECTOR3 v3V( 1.10 , 1.10 , 1.10 );     
    m_MSCurrent.v3Velocity = RandVector3( v3Velocity , v3V );     // �����ٶ�
    // ����
    m_MSCurrent.V3DAMP = D3DXVECTOR3( 0.3 , 0.3 , 0.3 ); 
    // ���ٶ�
    m_MSCurrent.v3Acceleration = D3DXVECTOR3( 0.0 , -9.8 , 0.0 );  // ���ü��ٶ�
    //
    m_MSNext = m_MSCurrent;
  }
//----------------------------------
// ������OnFrameMove
// ���ܣ�����������������������λ�ú���ɫ
// 
//----------------------------------
void CPARTICLESTATUS::OnFrameMove( double fInTime , float fElapsedTime , int i )
  {
    // ���Ƚ�ϵͳʱ��ת����ģ���ڵ�ʱ��
    static float sfTimeStart = fInTime + m_fTime;
    float  fTime = fInTime - sfTimeStart;
    // ���㶯������
    int iCount = int ( fTime / m_fLifeTimeEnd );
    if( iCount > m_iCount )
      {
        m_iCount = m_iCount + 1;
        ResetPositionColor( );
      }
    // ���԰�������������ʼΪԭ����������ʱ��
    m_fCurrentTimeInParticle = fTime - m_iCount * m_fLifeTimeEnd ;
    m_MSNext = RK4( &m_MSCurrent , fTime , fElapsedTime );
    if( m_MSNext.v3Position.y < 0 )
      {
        m_MSNext.v3Velocity.y = -m_MSNext.v3Velocity.y ;
      }
    m_MSCurrent = m_MSNext ;
    // ������ɫ
    float fColor ;
    if( m_fCurrentTimeInParticle > m_fLifeTimeWeak ) // �������𽥰����Է�ʽ����
      {
        fColor = ( m_fCurrentTimeInParticle - m_fLifeTimeWeak ) / ( m_fLifeTimeEnd - m_fLifeTimeWeak );
        m_v4ReturnColor.x = 1.0 - fColor;
      }
    // ����ʹ��
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
// ������GetPosition
// ���ܣ�����λ��
//----------------------------------
D3DXVECTOR4 CPARTICLESTATUS::GetPosition( )
  {
    return m_MSCurrent.v3Position ;
  }

//----------------------------------
// ������GetColor
// ���ܣ�������ɫ
//----------------------------------
D3DXVECTOR4 CPARTICLESTATUS::GetColor( )
  {
    return m_v4ReturnColor ;
  }

