//----------------------------------------------------------------------------
// 文件：CBoneAnimation.h
// 功能：声明程序所需变量和结构及类
// 4个结构
// 各模块的之间的头文件有相互关联的关系，无法拆分成独立头文件
// 曾经拆分过，在程序充分理解后在实施拆分
//----------------------------------------------------------------------------
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
    char    s_szHead[16];
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

//----------------------------------------------------------------------------
// D3DXFRAME_DERIVED 结构
//----------------------------------------------------------------------------
#ifndef D3DXFRAME_DERIVED_H
#define D3DXFRAME_DERIVED_H

struct D3DXFRAME_DERIVED: public D3DXFRAME
  {
    D3DXMATRIXA16        CombinedTransformationMatrix;
  };

#endif 
//-----------------------------------------------------------------------------
// D3DXMESHCONTAINER_DERIVED 结构
//-----------------------------------------------------------------------------
#ifndef D3DXMESHCONTAINER_DERIVED_H
#define D3DXMESHCONTAINER_DERIVED_H

struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
  {
    LPDIRECT3DTEXTURE9*  ppTextures;       // array of textures, entries are NULL if no texture specified    
                                
    // SkinMesh info             
    LPD3DXMESH           pOrigMesh;
    LPD3DXATTRIBUTERANGE pAttributeTable;
    DWORD                NumAttributeGroups; 
    DWORD                NumInfl;
    LPD3DXBUFFER         pBoneCombinationBuf;
    D3DXMATRIX**         ppBoneMatrixPtrs;
    D3DXMATRIX*          pBoneOffsetMatrices;
    DWORD                NumPaletteEntries;
    bool                 UseSoftwareVP;
    DWORD                iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning
  };

#endif


#ifndef BONEANIMATION_H
#define BONEANIMATION_H

struct BONEOBJECT_D3DVERTEX
  {
    D3DXVECTOR3 Position;
    float u , v;
  };

class CBONEANIMATION
  {
    private:
      ID3DXAnimationController*   m_pAnimController ;       // OnCreateDevice OnFrameMove OnDestroyDevice
      D3DXVECTOR3                 m_vObjectCenter;                  // OnCreateDevice OnFrameMove
      FLOAT                       m_fObjectRadius;                  // OnCreateDevice OnResetDevice OnFrameMove
      D3DXMATRIXA16*              m_pBoneMatrices ;           // GenerateSkinnedMesh
      UINT                        m_NumBoneMatricesMax ;         // GenerateSkinnedMesh
      D3DXMATRIXA16               m_matWorld;
      D3DXMATRIXA16               m_matView;                        // OnFrameMove DrawMeshContainer
      D3DXMATRIXA16               m_matProj;                        // OnFrameRender OnResetDevice
      //
      LPD3DXEFFECT                m_pEffect;
      LPD3DXFRAME                 m_pFrameRoot;                 // 根构架
      IDirect3DTexture9*          m_pColorTexture;
      //
      int                           m_iResNum;            // 本渲染对象的资源数
      SRESOURCEINFO*                m_pResourceInfo;      // 保存资源名字
      HRESULT LoadResource( TCHAR* FileName );
      TCHAR* GetResourceName( RESTYPE enuResType );
    private:

    public:
      CBONEANIMATION( );
      ~CBONEANIMATION( );
      HRESULT GenerateSkinnedMesh( IDirect3DDevice9 *pd3dDevice, D3DXMESHCONTAINER_DERIVED *pMeshContainer );
      HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );
      void    DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );
      void    DrawFrame( IDirect3DDevice9 *pd3dDevice, LPD3DXFRAME pFrame );
      HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );
      void    UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix );
      float   ReturnRadius( )     { return m_fObjectRadius; }
      D3DXVECTOR3 ReturnCenter( ) { return m_vObjectCenter; }
      //
      void    Init( );
      HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice, TCHAR* FileName );
      HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice ,const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
      void    OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice );
      void    OnLostDevice();
      void    OnDestroyDevice();
  };

#endif

//-----------------------------------------------------------------------------
// CALLOCATEHIERARCHY_H 结构
//-----------------------------------------------------------------------------
#ifndef CALLOCATEHIERARCHY_H
#define CALLOCATEHIERARCHY_H

class CAllocateHierarchy: public ID3DXAllocateHierarchy
  {
    public:
      STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
      STDMETHOD(CreateMeshContainer)(THIS_ 
        LPCSTR Name, 
        CONST D3DXMESHDATA *pMeshData,
        CONST D3DXMATERIAL *pMaterials, 
        CONST D3DXEFFECTINSTANCE *pEffectInstances, 
        DWORD NumMaterials, 
        CONST DWORD *pAdjacency, 
        LPD3DXSKININFO pSkinInfo, 
        LPD3DXMESHCONTAINER *ppNewMeshContainer);
      STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
      STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
      //CAllocateHierarchy()   {  }
      CAllocateHierarchy( CBONEANIMATION *pApp ) :m_pApp( pApp )   {  }

    public:
      CBONEANIMATION* m_pApp;
  };

#endif

