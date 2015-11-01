//-----------------------------------------------------------------------------
// �ļ���CBOARD.h 
// ���ܣ���Ⱦ����ͷ�ļ�
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "CRenderObject.h"
#include "CRenderObjectCube.h"

#ifndef CBOARD_H
#define CBOARD_H

#define MAXRENDEROBJECT  16

enum  RENDEROBJECTTYPE
  {
    ENU_RENDEROBJECT         = 0X00100001 , // ���任��ģ��
    ENU_CUBEOBJECT           = 0X00100002 , // ���ζ�������
    //ENU_BONEANIMATIONOBJECT  = 0X00200001 , // ������������
    //ENU_MORPHANIMATIONOBJECT = 0X00200002 , // ���ζ�������
  };

struct SRENDEROBJECT
  {
    void* lpVoid ;
    RENDEROBJECTTYPE eRenderObjectType;
  };

struct BOARD_D3DVERTEX
  {
    float x ,  y  , z  ,w;      // λ��
    // �������ɫֵ������Ͷ��������õ� D3DDECLTYPE_D3DCOLOR ��Ӧ
    DWORD color;                // �ں�����ʼ��ʱ���� RGBA ��ʼ���������� D3D����������ʽΪ D3DDECLTYPE_D3DCOLOR
    float u ,  v;               // ��������
  };

class CFULLWINDOW
  {
    private:
      LPDIRECT3DTEXTURE9            m_pColorTexture;      // ��ɫ����
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                // ���㻺����
      LPDIRECT3DINDEXBUFFER9        m_pIB;                // ���㻺����ָ��
      DWORD                         m_dwNumIndices;       // �������������
      DWORD                         m_dwNumVertices;      // �������
      //---------------------------------------------------------
      // ����[1]���ǳ���������С�Ǵ��ڱ߳���С��1/3�����ڴ�СΪ 640 * 410 ����������СΪ 213 * 137
      //---------------------------------------------------------
      LPDIRECT3DTEXTURE9            m_pRTTTexture_Scene;        // װ�볡�������������Ǵ��ڵ� 1 / 3
      LPD3DXRENDERTOSURFACE         m_pRTTInterface_Scene;      // ���Ƴ�������Ľӿ�
      LPDIRECT3DSURFACE9            m_pRTTSurface_Scene;        // ҳ��
      //---------------------------------------------------------
      // ����[2]����Bloom �����ǳ�������[1]�� 1 / 3�����������СΪ 71 * 46
      //---------------------------------------------------------
      LPDIRECT3DTEXTURE9            m_pRTTTextureBloom_DownSize_3x3;   // m_pRTTTextureScene ����� 1 / 3 ���Ǵ��ڴ�С�� 1 / 9
      LPD3DXRENDERTOSURFACE         m_pRTTInterfaceBloom_DownSize_3x3; // ���Ʒ�������Ľӿ�
      LPDIRECT3DSURFACE9            m_pRTTSurfaceBloom_DownSize_3x3;   // ҳ��
      //---------------------------------------------------------
      // ����[3]����Bloom �����������СΪ 71 * 46����ˮƽ����ģ����
      //---------------------------------------------------------
      LPDIRECT3DTEXTURE9            m_pRTTTextureBloom_DownSize_3x3_Horizon;   // �� m_pRTTTextureBloom_DownSize_3x3 ������ˮƽ��˹��չ��������Ļ�� 1 / 9 ����չ
      LPD3DXRENDERTOSURFACE         m_pRTTInterfaceBloom_DownSize_3x3_Horizon; // 
      LPDIRECT3DSURFACE9            m_pRTTSurfaceBloom_DownSize_3x3_Horizon;   // 
      //---------------------------------------------------------
      // ����[4]����Bloom �����������СΪ 71 * 46���ڴ�ֱ����ģ����
      //---------------------------------------------------------
      LPDIRECT3DTEXTURE9            m_pRTTTextureBloom_DownSize_3x3_Vertical;   // �� m_pRTTTextureBloom_DownSize_3x3 ��������ֱ��˹��չ��������Ļ�� 1 / 9 ����չ
      LPD3DXRENDERTOSURFACE         m_pRTTInterfaceBloom_DownSize_3x3_Vertical; // 
      LPDIRECT3DSURFACE9            m_pRTTSurfaceBloom_DownSize_3x3_Vertical;   // 
      // 
      int                           m_iCount;
      SRENDEROBJECT                 m_pCRenderObject[ MAXRENDEROBJECT ];
      // ��Ⱦ��������Ҫʹ�õ���������
      D3DXMATRIX                    m_matWorldRTT;    // ͶӰ����ľ���
      D3DXMATRIX                    m_matViewRTT;     // ͶӰ����ľ���
      D3DXMATRIX                    m_matProjectRTT;  // ͶӰ����ľ���
      // 
      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // ���������ӿ�
      LPD3DXEFFECT                  m_pEffect;            // Ч���ӿ�
      //
      HRESULT CreateSquare( IDirect3DDevice9* pd3dDevice );
    public:
      CFULLWINDOW( );
      ~CFULLWINDOW( );
      //
      void    Init( );
      HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice );
      HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice , UINT , UINT );
      void    OnFrameMove( float , D3DXVECTOR4 );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice );
      void    OnLostDevice();
      void    OnDestroyDevice();
      //
      void    OnFrameRenderToTexture( IDirect3DDevice9* pd3dDevice ,D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime );
      void    AddToRenderList( void* pCRenderObject , RENDEROBJECTTYPE );
      //
      void    RenderBloom( IDirect3DDevice9* );
  };

#endif
