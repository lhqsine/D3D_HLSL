//-----------------------------------------------------------------------------
// �ļ���CRenderObject.h
// ���ܣ���Ⱦ����ͷ�ļ� 
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#ifndef _FILETAG_H
#define _FILETAG_H

#define FILEHEADTAG  "SHMFILE" 

enum  RESTYPE
  {
    // Model
    ENU_MODELX         = 0X00100001 , // X Model
    ENU_MODELOTHER     = 0X00100002 , // �������͵�ģ��
    // Texture Type
    ENU_TEXCOLORMAP    = 0X00200001 , // Color Map
    ENU_TEXNORMALMAP   = 0X00200002 , // Normal Map
    ENU_TEXBUMPMAP     = 0X00200003 , // Bump Map
    ENU_SPECULARMAP    = 0x00200004 , // Specular Map
    // Fx File
    ENU_FXFILE         = 0X00300001 ,
    // Music        
    ENU_SGT            = 0X00400001 , // �����ļ�
    ENU_DLS            = 0X00400002 , // DLS �ļ�
    // Path File
    ENU_ANIMATION      = 0X00500001 , // �����ļ�
  };

struct SSHADERMODULEFILEHEAD
  {
    char    s_szHead[ 16 ];
    int     s_iVerHigh;
    int     s_iVerLow;
    //
    int     s_iPack;
    int     s_iResourceFileNum;
    int     s_iCheck[2];
    //
  };

struct SRESOURCEINFO
  {
    TCHAR   s_szFileName[ 24 ];
    RESTYPE s_enuResType;
    int     s_iOffset;
  };

#endif

#ifndef _CRENDEROBJECT_H
#define _CRENDEROBJECT_H

struct RENDEROBJECT_D3DVERTEX
  {
    D3DXVECTOR3 Position;
    float u , v;
  };

//#define RTTSIZE   1024

class CRENDEROBJECT
  {
    private:
      // ģ��
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                // ���㻺����
      LPDIRECT3DINDEXBUFFER9        m_pIB;                // ���㻺����ָ��
      DWORD                         m_dwNumIndices;       // �������������
      DWORD                         m_dwNumVertices;      // �������
      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // ���������ӿ�
      //
      LPD3DXMESH                    m_pMeshSysMemory ;    // 
      D3DXVECTOR3                   m_vCenter;            // 
      FLOAT                         m_fObjectRadius;      // 
      // ����
      IDirect3DTexture9*            m_pColorTexture  ;    // 
      // ------------- ��Ⱦ������������ʹ�õı��� -----------------
      int                           m_TextureSize;        // �����С
      // ��Ⱦ������ı����ͽӿ�
      LPDIRECT3DTEXTURE9            m_pRTTTexture;     // װ�뷴������
      LPD3DXRENDERTOSURFACE         m_pRTTInterface;   // ���Ʒ�������Ľӿ�
      LPDIRECT3DSURFACE9            m_pRTTSurface;     // ҳ��
      // ��Ⱦ��������Ҫʹ�õ���������
      D3DXMATRIX                    m_matWorldRTT;    // ͶӰ����ľ���
      D3DXMATRIX                    m_matViewRTT;     // ͶӰ����ľ���
      D3DXMATRIX                    m_matProjectRTT;  // ͶӰ����ľ���
      // �������������
      D3DXVECTOR3                   m_vBaryCentric;       // ����������������
      // Ч��
      LPD3DXEFFECT                  m_pEffect;            // Ч���ӿ�
      //
      int                           m_iResNum;            // ����Ⱦ�������Դ��
      SRESOURCEINFO*                m_pResourceInfo;      // ������Դ����
      HRESULT LoadResource( TCHAR* FileName );
      TCHAR* GetResourceName( RESTYPE enuResType );
    public:
      CRENDEROBJECT( );
      ~CRENDEROBJECT( );
      //
      float GetRadius() { return m_fObjectRadius ;}
      HRESULT RenderToTexture( LPDIRECT3DDEVICE9 pd3dDevice ,D3DXVECTOR3 vPosition ,float fTime );
      LPDIRECT3DTEXTURE9  ReturnTexture( ) { return  m_pRTTTexture; }
      HRESULT SetTexture( LPDIRECT3DTEXTURE9 ) ;
      //
      void    Init( );
      HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice ,TCHAR* FileName );
      HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice , int , int );
      void    OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice );
      void    OnLostDevice();
      void    OnDestroyDevice();
  };


#endif


