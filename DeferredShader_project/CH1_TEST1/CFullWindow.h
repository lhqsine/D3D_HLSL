//-----------------------------------------------------------------------------
// 文件：CFULLWINDOW.h
// 功能：渲染物体头文件 
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#ifndef FULLWINDOW_H
#define FULLWINDOW_H

struct FULLWINDOW_D3DVERTEX
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
      HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice );
      void    OnFrameMove( );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice ,D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice ,LPDIRECT3DTEXTURE9 pTexture );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice ,LPDIRECT3DTEXTURE9 , LPDIRECT3DTEXTURE9 );
      void    OnLostDevice();
      void    OnDestroyDevice();
      //
      void  SetLightParament( D3DXVECTOR4* vPosition );
  };

#endif
