#include "dxstdafx.h"
#include <stdio.h>
#include <conio.h>

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#define ADJACENCY_EPSILON 0.0001f

struct INDEX16
  {
    WORD V0;
    WORD V1;
    WORD V2;
  };

struct INDEX32
  {
    DWORD V0;
    DWORD V1;
    DWORD V2;
  };

class CBUFFER
  {
    private:
      LPDIRECT3D9                   m_pD3D           ; // D3D 接口
      LPDIRECT3DDEVICE9             m_pd3dDevice     ; // 设备接口
      LPD3DXMESH                    m_pMesh          ; // 模型接口
      LPDIRECT3DVERTEXBUFFER9       m_pVB            ; // 顶点缓冲区
      LPDIRECT3DINDEXBUFFER9        m_pIB            ; // 顶点缓冲区指针
      DWORD                         m_dwFaceNum ;
      DWORD                         m_dwVertexNum ;
      D3DXVECTOR3*                  m_pVertex;
      // 一些缓冲区数据定义
      DWORD*                        m_pdwAdj;
      DWORD*                        m_pdwPtRep;
      //
      INDEX16*                      m_pIndex16;
      INDEX32*                      m_pIndex32;
      //
      LPD3DXBUFFER                  m_pBuffer;
      INDEX16*                      m_pAdjIndex16;
      INDEX32*                      m_pAdjIndex32;
      //
      void Cleanup( );
    public:
      CBUFFER( );
      ~CBUFFER( );
      //
      HRESULT LoadModel( LPCWSTR pFileName );
      //
      DWORD GetVertexNum( );
      DWORD GetFaceNum( );
      D3DXVECTOR3* GetVertex( );
      INDEX16* GetIndex16( );
      INDEX32* GetIndex32( );
      void DisplayVertex(  );
      void DisplayIndex(  );
      void DisplayAdjInfo( );
      void DisplayPointRep( );
  };




#endif


