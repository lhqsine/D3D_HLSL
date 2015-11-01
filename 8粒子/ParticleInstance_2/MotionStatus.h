#include "dxstdafx.h"

#ifndef MOTION_STATUS_H
#define MOTION_STATUS_H

#define V3ACCERLATION  D3DXVECTOR3( 0.0 , -9.8 ,0.0 ) 
#define RADIUS         2.0
//-----------------------------------------------------------------------------
// ���������˶�״̬�Ľṹ������ʱ���λ�ã��ٶȣ����ٶ�
// �������л�����ʹ�ü��ٶ�
//-----------------------------------------------------------------------------
struct MOTION_STATUS  
  {
    double Time;            // ʱ��
    D3DXVECTOR3  V3DAMP;
    D3DXVECTOR3  v3Position;        // λ��
    D3DXVECTOR3  v3Velocity;        // �ٶ�
    D3DXVECTOR3  v3Acceleration;    // ���ٶ�
    //bool         bCollusion;        // �Ƿ���ײ
  };

#endif

MOTION_STATUS RK4( MOTION_STATUS* Status0 , double Time ,float h );
// ����������֮�����ײ
bool Collision( MOTION_STATUS* pObject_A_Status ,MOTION_STATUS* pObject_B_Status );



