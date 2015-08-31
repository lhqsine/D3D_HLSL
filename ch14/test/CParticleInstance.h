//-----------------------------------------------------------------------------
// 文件：CRenderObject.h
// 功能：渲染物体头文件
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "CParticleStatus.h"

#ifndef _RESHEAD_H
#define _RESHEAD_H

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

#ifndef CPARTICLE_H
#define CPARTICLE_H

struct PARTICLE_D3DVERTEX
  {
    D3DXVECTOR3 Position;
    float u , v;
  };
// 增加描述位置信息的变量
struct PARTICLE_D3DVERTEX_INSTANCE
  {
    //D3DXVECTOR3 PositionInWorld;
    float       fInstance;
  };

class CPARTICLEINSTANCE
  {
    private:
      // 粒子个数
      int                           m_iParticleNum;       // 粒子个数
      CPARTICLESTATUS*              m_pStatus;
      // 模型
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                // 顶点缓冲区
      LPDIRECT3DINDEXBUFFER9        m_pIB;                // 顶点缓冲区指针
      DWORD                         m_dwNumIndices;       // 顶点的索引个数
      DWORD                         m_dwNumVertices;      // 顶点个数
      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // 顶点声明接口
      //
      LPDIRECT3DVERTEXBUFFER9       m_pVBInstance;        // 记录实例的顶点缓冲区
      LPDIRECT3DVERTEXDECLARATION9  m_pDeclInstance;      // 顶点声明接口
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
      CPARTICLEINSTANCE( );
      ~CPARTICLEINSTANCE( );
      //
      float GetRadius() { return m_fObjectRadius ;}
      D3DXVECTOR3 GetCenter() { return m_vCenter ; }
      IDirect3DTexture9* GetTexture( ) { return  m_pColorTexture; }
      //
      void    Init(  );
      HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice ,TCHAR* FileName );
      HRESULT OnResetDevice( );
      void    OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime , float , D3DXVECTOR3* );
      //void    OnFrameRender( IDirect3DDevice9* pd3dDevice , D3DXVECTOR3* );
      void    OnFrameRenderInstance( IDirect3DDevice9* pd3dDevice );
      void    OnLostDevice();
      void    OnDestroyDevice();
  };


#endif


