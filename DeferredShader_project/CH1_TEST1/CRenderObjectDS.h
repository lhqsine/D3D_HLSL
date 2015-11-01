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

#ifndef _CRENDEROBJECT_DS_H
#define _CRENDEROBJECT_DS_H

//#define RTTTEXTURESIZE 512

struct RENDEROBJECT_D3DVERTEXDS
  {
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Normal;
    float u , v;
  };

class CRENDEROBJECTDS
  {
    private:
      // ģ��
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                  // ���㻺����
      LPDIRECT3DINDEXBUFFER9        m_pIB;                  // ���㻺����ָ��
      DWORD                         m_dwNumIndices;         // �������������
      DWORD                         m_dwNumVertices;        // �������
      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;                // ���������ӿ�
      // �����С
      //int                           m_TextureSize;          // �����С
      // ����ռ�λ�õ�����
      LPDIRECT3DTEXTURE9            m_pRTTTexturePosition;  // ����
      LPD3DXRENDERTOSURFACE         m_pRTTInterfacePosition;// ��������Ľӿ�
      LPDIRECT3DSURFACE9            m_pRTTSurfacePosition;  // ҳ��
      // ����ռ䷨����������
      LPDIRECT3DTEXTURE9            m_pRTTTextureNormal;    // ����
      LPD3DXRENDERTOSURFACE         m_pRTTInterfaceNormal;  // ��������Ľӿ�
      LPDIRECT3DSURFACE9            m_pRTTSurfaceNormal;    // ҳ��
      //
      LPD3DXMESH                    m_pMeshSysMemory ;    //
      D3DXVECTOR3                   m_vCenter;
      FLOAT                         m_fObjectRadius;
      // ����
      IDirect3DTexture9*            m_pColorTexture  ;
      // Ч��
      LPD3DXEFFECT                  m_pEffect;            // Ч���ӿ�
      //
      int                           m_iResNum;            // ����Ⱦ�������Դ��
      SRESOURCEINFO*                m_pResourceInfo;      // ������Դ����
      HRESULT LoadResource( TCHAR* FileName );
      TCHAR* GetResourceName( RESTYPE enuResType );
    public:
      CRENDEROBJECTDS( );
      ~CRENDEROBJECTDS( );
      //
      float GetRadius() { return m_fObjectRadius ;}
      //void  SetLightParament( D3DXVECTOR4* vPosition );
      void  RenderToTexture( IDirect3DDevice9* pd3dDevice , D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime );
      LPDIRECT3DTEXTURE9 ReturnPositionTexture( ) { return m_pRTTTexturePosition; }
      LPDIRECT3DTEXTURE9 ReturnNormalTexture( ) { return m_pRTTTextureNormal; }
      //
      void    Init( );
      HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice ,TCHAR* FileName );
      HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice ,const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
      void    OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice );
      void    OnLostDevice();
      void    OnDestroyDevice();
  };


#endif


