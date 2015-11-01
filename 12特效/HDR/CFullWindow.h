//-----------------------------------------------------------------------------
// 文件：CBOARD.h 
// 功能：渲染物体头文件
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "CRenderObject.h"
#include "CRenderObjectCube.h"

#ifndef CBOARD_H
#define CBOARD_H

#define MAXRENDEROBJECT  16

enum  RENDEROBJECTTYPE
  {
    ENU_RENDEROBJECT         = 0X00100001 , // 不变换的模型
    ENU_CUBEOBJECT           = 0X00100002 , // 变形动画物体
    //ENU_BONEANIMATIONOBJECT  = 0X00200001 , // 骨骼动画物体
    //ENU_MORPHANIMATIONOBJECT = 0X00200002 , // 变形动画物体
  };

struct SRENDEROBJECT
  {
    void* lpVoid ;
    RENDEROBJECTTYPE eRenderObjectType;
  };

struct BOARD_D3DVERTEX
  {
    float x ,  y  , z  ,w;      // 位置
    // 下面的颜色值，必须和顶点流设置的 D3DDECLTYPE_D3DCOLOR 对应
    DWORD color;                // 在后续初始化时，以 RGBA 初始化，并告诉 D3D，顶点流格式为 D3DDECLTYPE_D3DCOLOR
    float u ,  v;               // 纹理坐标
  };

class CFULLWINDOW
  {
    private:
      LPDIRECT3DTEXTURE9            m_pColorTexture;      // 颜色纹理
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                // 顶点缓冲区
      LPDIRECT3DINDEXBUFFER9        m_pIB;                // 顶点缓冲区指针
      DWORD                         m_dwNumIndices;       // 顶点的索引个数
      DWORD                         m_dwNumVertices;      // 顶点个数
      //---------------------------------------------------------
      // 纹理[1]，是场景纹理，大小是窗口边长大小的1/3，窗口大小为 640 * 410 ，则此纹理大小为 213 * 137
      //---------------------------------------------------------
      LPDIRECT3DTEXTURE9            m_pRTTTexture_Scene;        // 装入场景纹理，此纹理是窗口的 1 / 3
      LPD3DXRENDERTOSURFACE         m_pRTTInterface_Scene;      // 控制场景纹理的接口
      LPDIRECT3DSURFACE9            m_pRTTSurface_Scene;        // 页面
      //---------------------------------------------------------
      // 纹理[2]，是Bloom 纹理，是场景纹理[1]的 1 / 3，，即纹理大小为 71 * 46
      //---------------------------------------------------------
      LPDIRECT3DTEXTURE9            m_pRTTTextureBloom_DownSize_3x3;   // m_pRTTTextureScene 纹理的 1 / 3 ，是窗口大小的 1 / 9
      LPD3DXRENDERTOSURFACE         m_pRTTInterfaceBloom_DownSize_3x3; // 控制反射纹理的接口
      LPDIRECT3DSURFACE9            m_pRTTSurfaceBloom_DownSize_3x3;   // 页面
      //---------------------------------------------------------
      // 纹理[3]，是Bloom 纹理，即纹理大小为 71 * 46，在水平方向模糊，
      //---------------------------------------------------------
      LPDIRECT3DTEXTURE9            m_pRTTTextureBloom_DownSize_3x3_Horizon;   // 对 m_pRTTTextureBloom_DownSize_3x3 纹理做水平高斯扩展，即将屏幕的 1 / 9 做扩展
      LPD3DXRENDERTOSURFACE         m_pRTTInterfaceBloom_DownSize_3x3_Horizon; // 
      LPDIRECT3DSURFACE9            m_pRTTSurfaceBloom_DownSize_3x3_Horizon;   // 
      //---------------------------------------------------------
      // 纹理[4]，是Bloom 纹理，即纹理大小为 71 * 46，在垂直方向模糊，
      //---------------------------------------------------------
      LPDIRECT3DTEXTURE9            m_pRTTTextureBloom_DownSize_3x3_Vertical;   // 对 m_pRTTTextureBloom_DownSize_3x3 纹理做垂直高斯扩展，即将屏幕的 1 / 9 做扩展
      LPD3DXRENDERTOSURFACE         m_pRTTInterfaceBloom_DownSize_3x3_Vertical; // 
      LPDIRECT3DSURFACE9            m_pRTTSurfaceBloom_DownSize_3x3_Vertical;   // 
      // 
      int                           m_iCount;
      SRENDEROBJECT                 m_pCRenderObject[ MAXRENDEROBJECT ];
      // 渲染到纹理需要使用的三个矩阵
      D3DXMATRIX                    m_matWorldRTT;    // 投影纹理的矩阵
      D3DXMATRIX                    m_matViewRTT;     // 投影纹理的矩阵
      D3DXMATRIX                    m_matProjectRTT;  // 投影纹理的矩阵
      // 
      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // 顶点声明接口
      LPD3DXEFFECT                  m_pEffect;            // 效果接口
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
