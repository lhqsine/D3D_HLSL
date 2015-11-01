#include "dxstdafx.h"
#include "MotionStatus.h"

D3DXVECTOR3 RandVector3( D3DXVECTOR3 v3Center , D3DXVECTOR3 v3Radius );
float RandFloat( float fCenter , float fRadius );

#ifndef _SSTATUS_H
#define _SSTATUS_H

#define DURATIONTIME  4.0f

class CPARTICLESTATUS            
  {
    private:
      MOTION_STATUS                   m_MSCurrent;              // ��ǰ״̬
      MOTION_STATUS                   m_MSNext;                 // ��һ��״̬
      float                           m_fTime;                  // �Ӵ˸����ӿ�����ʱ��ʱ��
      float                           m_fLifeTimeWeak;          // ������1��ǿ�ȿ�ʼ������ʱ��С�� s_fLifeTimeEnd
      float                           m_fLifeTimeEnd;           // ������2����ʧ��λ�ø�λ��ʱ���
      // 
      float                           m_fCurrentTimeInParticle; // �������������е�ʱ�����
      int                             m_iCount;                 // �����ֻش���
      D3DXVECTOR4                     m_v4ReturnColor;          // �������෵�ص���ɫ������ w �������ӵ� ID
    public:
      // 
      CPARTICLESTATUS( );
      ~CPARTICLESTATUS( );
      // 
      void ResetPositionColor( );
      void InitPositionColor( );
      void OnFrameMove( double fTime , float fElapsedTime , int i );
      D3DXVECTOR4 GetPosition( );
      D3DXVECTOR4 GetColor( );
  };

#endif
