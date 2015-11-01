//-----------------------------------------------------------------------------
// �ļ���CRenderObject.h
// ���ܣ���Ⱦ����ͷ�ļ� 
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "CRenderObject.h"
#include "CRenderObjectCube.h"

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

#ifndef _CRENDERTOCUBEMAP_H
#define _CRENDERTOCUBEMAP_H

#define MAXRENDEROBJECT  16

struct CRENDERTOCUBEMAP_D3DVERTEX
  {
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Normal;
    float u , v;
  };

enum ERENDERTYPE
  {
    ENU_RENDEROBJECT = 0X00000001 , // �� CRENDEROBJECT ��
    ENU_CUBE         = 0X00000002 , // �� CRENDEROBJECTCUBE ��
  };

struct SRENDEROBJECT
  {
    ERENDERTYPE eType;
    void*       pType;
  };

class CRENDERTOCUBEMAP
  {
    private:
      // ģ��
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                // ���㻺����
      LPDIRECT3DINDEXBUFFER9        m_pIB;                // ���㻺����ָ��
      DWORD                         m_dwNumIndices;       // �������������
      DWORD                         m_dwNumVertices;      // �������
      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // ���������ӿ�
      //
      int                           m_iCount;
      SRENDEROBJECT                 m_pCRenderObject[ MAXRENDEROBJECT ];
      //
      LPD3DXMESH                    m_pMeshSysMemory ;    // 
      D3DXVECTOR3                   m_vCenter;            // 
      FLOAT                         m_fObjectRadius;      // 
      // ����
      IDirect3DTexture9*            m_pColorTexture  ;    // 
      //IDirect3DSurface9*            m_pSurface;
      // ------------- ��Ⱦ������������ʹ�õı��� -----------------
      int                           m_TextureSize;        // �����С
      // ��Ⱦ������ı����ͽӿ�
      IDirect3DCubeTexture9*        m_pRTTTextureBox;     // װ�뷴������
      //LPD3DXRENDERTOSURFACE         m_pRTTInterfaceBox;   // ���Ʒ�������Ľӿ�
      LPDIRECT3DSURFACE9            m_pRTTSurfaceBox;     // ҳ��
      LPDIRECT3DSURFACE9            m_pRTTDepthStencil;   // ���ģ�建����
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
      CRENDERTOCUBEMAP( );
      ~CRENDERTOCUBEMAP( );
      //
      float GetRadius() { return m_fObjectRadius ;}

      //HRESULT SetTexture( LPDIRECT3DTEXTURE9 ) ;
      IDirect3DSurface9* GetTextureSurface( ) { return m_pRTTSurfaceBox ; }
      IDirect3DCubeTexture9* GetTexture( ) { return m_pRTTTextureBox ; }
      LPDIRECT3DSURFACE9 GetDepthStencilSurface( ) { return m_pRTTDepthStencil ; }
      //
      void    Init( );
      HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice ,TCHAR* FileName );
      HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice , float );
      void    OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject , D3DXVECTOR3* , double fTime );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice );
      void    OnLostDevice();
      void    OnDestroyDevice();
      //
      void    AddToRenderList( void* pRenderObject , ERENDERTYPE eType );
      HRESULT RenderToCubeMap( LPDIRECT3DDEVICE9 pd3dDevice , D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime );
  };


#endif


