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
      MOTION_STATUS                   m_MSCurrent;              // 当前状态
      MOTION_STATUS                   m_MSNext;                 // 下一个状态
      float                           m_fTime;                  // 从此个粒子开创建时的时间
      float                           m_fLifeTimeWeak;          // 生命期1，强度开始减弱，时间小于 s_fLifeTimeEnd
      float                           m_fLifeTimeEnd;           // 生命期2，消失，位置复位，时间最长
      // 
      float                           m_fCurrentTimeInParticle; // 在粒子生命期中的时间变量
      int                             m_iCount;                 // 粒子轮回次数
      D3DXVECTOR4                     m_v4ReturnColor;          // 此粒子类返回的颜色，其中 w 保存粒子的 ID
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
