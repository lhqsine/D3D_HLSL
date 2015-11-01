//-----------------------------------------------------------------------------
// �ļ���CBOARD.h
// ���ܣ���Ⱦ����ͷ�ļ� 
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#ifndef CBOARD_H
#define CBOARD_H

struct BOARD_D3DVERTEX
  {
    float x ,  y  , z  ,w;      // λ��
    // �������ɫֵ������Ͷ��������õ� D3DDECLTYPE_D3DCOLOR ��Ӧ
    DWORD color;                // �ں�����ʼ��ʱ���� RGBA ��ʼ���������� D3D����������ʽΪ D3DDECLTYPE_D3DCOLOR
    float u ,  v;               // ��������
  };

class CBOARD
  {
    private:
      LPDIRECT3DTEXTURE9            m_pColorTexture;      // ��ɫ����
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                // ���㻺����
      LPDIRECT3DINDEXBUFFER9        m_pIB;                // ���㻺����ָ��
      DWORD                         m_dwNumIndices;       // �������������
      DWORD                         m_dwNumVertices;      // �������
      // 
      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // ���������ӿ�
      LPD3DXEFFECT                  m_pEffect;            // Ч���ӿ�
      //
      HRESULT CreateSquare( IDirect3DDevice9* pd3dDevice );
    public:
      CBOARD( );
      ~CBOARD( );
      //
      void    Init( );
      HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice );
      HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice );
      void    OnFrameMove( );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice ,D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice ,LPDIRECT3DTEXTURE9 pTexture , int );
      void    OnLostDevice();
      void    OnDestroyDevice();
  };

#endif
