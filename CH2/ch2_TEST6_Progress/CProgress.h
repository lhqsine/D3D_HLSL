
#include "dxstdafx.h"

#ifndef CPROGRESS_H
#define CPROGRESS_H


struct  BOARD_D3DVERTEX
{
	float x, y, z, w; // 位置
    // 下面的颜色值，必须和顶点流设置的 D3DDECLTYPE_D3DCOLOR 对应
	DWORD color;                // 在后续初始化时，以 RGBA 初始化，并告诉 D3D，顶点流格式为 D3DDECLTYPE_D3DCOLOR
    float u ,  v;               // 纹理坐标
};


class CPROGRESS
{
private:
    private:
      LPDIRECT3DTEXTURE9            m_pColorTexture;      // 颜色纹理
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                // 顶点缓冲区
      LPDIRECT3DINDEXBUFFER9        m_pIB;                // 顶点缓冲区指针
      DWORD                         m_dwNumIndices;       // 顶点的索引个数
      DWORD                         m_dwNumVertices;      // 顶点个数

      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // 顶点声明接口
      LPD3DXEFFECT                  m_pEffect;            // 效果接口

	  HRESULT CreateSquare( IDirect3DDevice9* pd3dDevice );


public:
      CPROGRESS( );
      ~CPROGRESS( );

      void    Init( );
      HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice );
      HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice);
      void    OnFrameMove();
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice  ,float fProgress );
      void    OnLostDevice();
      void    OnDestroyDevice();
};



#endif