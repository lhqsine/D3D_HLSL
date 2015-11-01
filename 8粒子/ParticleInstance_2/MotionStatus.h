#include "dxstdafx.h"

#ifndef MOTION_STATUS_H
#define MOTION_STATUS_H

#define V3ACCERLATION  D3DXVECTOR3( 0.0 , -9.8 ,0.0 ) 
#define RADIUS         2.0
//-----------------------------------------------------------------------------
// 保存物体运动状态的结构，任意时间的位置，速度，加速度
// 本例子中还不会使用加速度
//-----------------------------------------------------------------------------
struct MOTION_STATUS  
  {
    double Time;            // 时间
    D3DXVECTOR3  V3DAMP;
    D3DXVECTOR3  v3Position;        // 位置
    D3DXVECTOR3  v3Velocity;        // 速度
    D3DXVECTOR3  v3Acceleration;    // 加速度
    //bool         bCollusion;        // 是否碰撞
  };

#endif

MOTION_STATUS RK4( MOTION_STATUS* Status0 , double Time ,float h );
// 处理两个球之间的碰撞
bool Collision( MOTION_STATUS* pObject_A_Status ,MOTION_STATUS* pObject_B_Status );



