//-----------------------------------------------------------------------------
// 文件：CRenderObject.h
// 功能：渲染物体头文件
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#ifndef _CRENDEROBJECT_H
#define _CRENDEROBJECT_H

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

class CRENDEROBJECT
  {
    private:
      // 模型
      LPD3DXMESH                    m_pMesh ;    // 
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
      CRENDEROBJECT( );
      ~CRENDEROBJECT( );
      //
      float GetRadius() { return m_fObjectRadius ;}
      //
      HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh );
      //
      void    Init( );
      HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice ,TCHAR* FileName );
      HRESULT OnResetDevice( );
      void    OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice );
      void    OnLostDevice();
      void    OnDestroyDevice();
  };


#endif


