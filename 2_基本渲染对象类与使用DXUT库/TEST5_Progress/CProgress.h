
#include "dxstdafx.h"

#ifndef CPROGRESS_H
#define CPROGRESS_H


struct  BOARD_D3DVERTEX
{
	float x, y, z, w; // λ��
    // �������ɫֵ������Ͷ��������õ� D3DDECLTYPE_D3DCOLOR ��Ӧ
	DWORD color;                // �ں�����ʼ��ʱ���� RGBA ��ʼ���������� D3D����������ʽΪ D3DDECLTYPE_D3DCOLOR
    float u ,  v;               // ��������
};


class CPROGRESS
{
private:
    private:
      LPDIRECT3DTEXTURE9            m_pColorTexture;      // ��ɫ����
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                // ���㻺����
      LPDIRECT3DINDEXBUFFER9        m_pIB;                // ���㻺����ָ��
      DWORD                         m_dwNumIndices;       // �������������
      DWORD                         m_dwNumVertices;      // �������

      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // ���������ӿ�
      LPD3DXEFFECT                  m_pEffect;            // Ч���ӿ�

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