//-----------------------------------------------------------------------------
// 文件：CRenderObject.h 
// 功能：渲染物体头文件
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#ifndef _FILETAG_H
#define _FILETAG_H

#define FILEHEADTAG  "SHMFILE" 

enum  RESTYPE
  {
    // Model
    ENU_MODELX         = 0X00100001 , // X Model
    ENU_MODELOTHER     = 0X00100002 , // 其他类型的模型
    // Texture Type
    ENU_TEXCOLORMAP    = 0X00200001 , // Color Map
    ENU_TEXNORMALMAP   = 0X00200002 , // Normal Map
    ENU_TEXBUMPMAP     = 0X00200003 , // Bump Map
    ENU_SPECULARMAP    = 0x00200004 , // Specular Map
    // Fx File
    ENU_FXFILE         = 0X00300001 ,
    // Music        
    ENU_SGT            = 0X00400001 , // 音乐文件
    ENU_DLS            = 0X00400002 , // DLS 文件
    // Path File
    ENU_ANIMATION      = 0X00500001 , // 动画文件
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
      // 模型
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                  // 顶点缓冲区
      LPDIRECT3DINDEXBUFFER9        m_pIB;                  // 顶点缓冲区指针
      DWORD                         m_dwNumIndices;         // 顶点的索引个数
      DWORD                         m_dwNumVertices;        // 顶点个数
      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;                // 顶点声明接口
      // 纹理大小
      //int                           m_TextureSize;          // 纹理大小
      // 保存空间位置的纹理
      LPDIRECT3DTEXTURE9            m_pRTTTexturePosition;  // 纹理
      LPD3DXRENDERTOSURFACE         m_pRTTInterfacePosition;// 控制纹理的接口
      LPDIRECT3DSURFACE9            m_pRTTSurfacePosition;  // 页面
      // 保存空间法向量的纹理
      LPDIRECT3DTEXTURE9            m_pRTTTextureNormal;    // 纹理
      LPD3DXRENDERTOSURFACE         m_pRTTInterfaceNormal;  // 控制纹理的接口
      LPDIRECT3DSURFACE9            m_pRTTSurfaceNormal;    // 页面
      //
      LPD3DXMESH                    m_pMeshSysMemory ;    //
      D3DXVECTOR3                   m_vCenter;
      FLOAT                         m_fObjectRadius;
      // 纹理
      IDirect3DTexture9*            m_pColorTexture  ;
      // 效果
      LPD3DXEFFECT                  m_pEffect;            // 效果接口
      //
      int                           m_iResNum;            // 本渲染对象的资源数
      SRESOURCEINFO*                m_pResourceInfo;      // 保存资源名字
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


