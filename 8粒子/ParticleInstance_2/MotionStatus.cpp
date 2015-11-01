//-----------------------------------------------------------------------------
// 文件：MotionStatus.cpp
// 功能：MotionStatus.h 的实现
//-----------------------------------------------------------------------------
#include "MotionStatus.h"

//-----------------------------------------------------------------------------
// 速度微分方程
// 传入当前状态，当前时间和步长
// 返回系数
// a = d2x / dt2
//-----------------------------------------------------------------------------
D3DXVECTOR3 Second_Order_Defferential( MOTION_STATUS* Status0 , double Time ,float h )
  {
    D3DXVECTOR3 K ;//= D3DXVECTOR3( 0.0 , -9.8 , 0.0 ) ;
    
    
    K.x = V3ACCERLATION.x - Status0->V3DAMP.x * Status0->v3Velocity.x ;
    K.y = V3ACCERLATION.y - Status0->V3DAMP.y * Status0->v3Velocity.y ;
    K.z = V3ACCERLATION.z - Status0->V3DAMP.z * Status0->v3Velocity.z ;
    
    return K ;
  }

//-----------------------------------------------------------------------------
// 4阶龙格库塔法
// 传入：上一个状态，当前时间，步长
// 返回：当前状态
//-----------------------------------------------------------------------------
MOTION_STATUS RK4( MOTION_STATUS* Status0 , double Time ,float h )
  {
    MOTION_STATUS Status_Return;
    MOTION_STATUS Status ;
    D3DXVECTOR3 K1 , K2 , K3 , K4 ;
    // 计算各个系数
    K1 = Second_Order_Defferential( Status0 , Time ,h );

    //
    Status.v3Position = Status0->v3Position + Status0->v3Velocity * h / 2.0f ;
    Status.v3Velocity = Status0->v3Velocity + K1 * h / 2.0f ;
    Status.V3DAMP = Status0->V3DAMP;

    K2 = Second_Order_Defferential( &Status , Time + h / 2.0f , h );
    // 
    Status.v3Position = Status0->v3Position + Status0->v3Velocity * h / 2.0f + h * h * K1 / 4.0f ;
    Status.v3Velocity = Status0->v3Velocity + K2 * h / 2.0f ;
    K3 = Second_Order_Defferential( &Status , Time + h / 2.0f , h );
    // 
    Status.v3Position = Status0->v3Position + h * Status0->v3Velocity + K2 * h * h / 2.0f ;
    Status.v3Velocity = Status0->v3Velocity + K3 * h ;
    K4 = Second_Order_Defferential( &Status , Time + h , h );
    // 计算新的状态s
    Status_Return.v3Position = Status0->v3Position + h * Status0->v3Velocity + h * h * ( K1 + K2 + K3 ) / 6;
    Status_Return.v3Velocity = Status0->v3Velocity + h * ( K1 + 2 * K2 + 2 * K3 + K4 ) / 6;
    Status_Return.v3Acceleration = K1 ;
    Status_Return.V3DAMP = Status0->V3DAMP ;
    // 
    return Status_Return;
  }
//-----------------------------------------------------------------------------
// 函数名：Collision
// 功能：碰撞检测
// 传入：两个物体的运动状态
// 返回：是否碰撞
//-----------------------------------------------------------------------------
bool Collision( MOTION_STATUS* pObject_A_Status ,MOTION_STATUS* pObject_B_Status )
  {
    D3DXVECTOR3 v3Length = pObject_A_Status->v3Position - pObject_B_Status->v3Position ;
    if( D3DXVec3Length( &v3Length ) >= RADIUS )
      {
        // 未发生碰撞，返回 false
        return false;
      }
    D3DXVECTOR3 v3Object_A_Normal ,v3Object_B_Normal;
    D3DXVECTOR3 v3VelocityNormalA ,v3VelocityNormalB;
    // 球速度大小
    float fVelocityA = D3DXVec3Length( &pObject_A_Status->v3Velocity );
    float fVelocityB = D3DXVec3Length( &pObject_B_Status->v3Velocity );
    // 球 A , B 的碰撞反射法向量归一化
    D3DXVec3Normalize( &v3Object_A_Normal , &v3Length );
    v3Object_B_Normal = - v3Object_A_Normal;
    // 两个球体运动速度归一化
    D3DXVec3Normalize( &v3VelocityNormalA ,&pObject_A_Status->v3Velocity );
    D3DXVec3Normalize( &v3VelocityNormalB ,&pObject_B_Status->v3Velocity );
    // 最终运动方向是其速度的大小 * 反射向量 v = i - 2 * dot(i, n) * n
    pObject_A_Status->v3Velocity = fVelocityA * 
      ( v3VelocityNormalA - 2 * D3DXVec3Dot( &( v3VelocityNormalA ) , &v3Object_A_Normal ) * v3Object_A_Normal );
    pObject_B_Status->v3Velocity = fVelocityB * 
      ( v3VelocityNormalB - 2 * D3DXVec3Dot( &( v3VelocityNormalB ) , &v3Object_B_Normal ) * v3Object_B_Normal );

    /*
    pObject_A_Status->v3Velocity = fVelocityA * 
      ( 2 * D3DXVec3Dot( &v3VelocityNormalA , &v3Object_A_Normal ) * v3Object_A_Normal - v3VelocityNormalA );
    pObject_B_Status->v3Velocity = fVelocityB * 
      ( 2 * D3DXVec3Dot( &v3VelocityNormalB , &v3Object_B_Normal ) * v3Object_B_Normal - v3VelocityNormalB );
      */
    /*
    pObject_A_Status->v3Velocity = fVelocityA * ( v3VelocityNormalA + 2 * v3VelocityNormalA );
    pObject_B_Status->v3Velocity = fVelocityB * ( v3VelocityNormalB + 2 * v3VelocityNormalB );
    */
    return true;
  }
