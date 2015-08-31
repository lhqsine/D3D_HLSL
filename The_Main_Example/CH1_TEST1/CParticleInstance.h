//-----------------------------------------------------------------------------
// �ļ���CRenderObject.h
// ���ܣ���Ⱦ����ͷ�ļ�
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
    ENU_MODELOTHER     = 0X00100002 , // �������͵�ģ��
    // Texture Type
    ENU_TEXCOLORMAP    = 0X00200001 , // Color Map
    ENU_TEXNORMALMAP   = 0X00200002 , // Normal Map
    ENU_TEXBUMPMAP     = 0X00200003 , // Bump Map
    ENU_SPECULARMAP    = 0x00200004 , // Specular Map
    // Fx File
    ENU_FXFILE         = 0X00300001 ,
    // Music        
    ENU_SGT            = 0X00400001 , // �����ļ�
    ENU_DLS            = 0X00400002 , // DLS �ļ�
    // Path File
    ENU_ANIMATION      = 0X00500001 , // �����ļ�
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
// ��������λ����Ϣ�ı���
struct PARTICLE_D3DVERTEX_INSTANCE
  {
    //D3DXVECTOR3 PositionInWorld;
    float       fInstance;
  };

class CPARTICLEINSTANCE
  {
    private:
      // ���Ӹ���
      int                           m_iParticleNum;       // ���Ӹ���
      CPARTICLESTATUS*              m_pStatus;
      // ģ��
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                // ���㻺����
      LPDIRECT3DINDEXBUFFER9        m_pIB;                // ���㻺����ָ��
      DWORD                         m_dwNumIndices;       // �������������
      DWORD                         m_dwNumVertices;      // �������
      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // ���������ӿ�
      //
      LPDIRECT3DVERTEXBUFFER9       m_pVBInstance;        // ��¼ʵ���Ķ��㻺����
      LPDIRECT3DVERTEXDECLARATION9  m_pDeclInstance;      // ���������ӿ�
      //
      LPD3DXMESH                    m_pMeshSysMemory ;    //
      D3DXVECTOR3                   m_vCenter;
      FLOAT                         m_fObjectRadius;
      // ����
      IDirect3DTexture9*            m_pColorTexture  ;
      // Ч��
      LPD3DXEFFECT                  m_pEffect;            // Ч���ӿ�
      //
      int                           m_iResNum;            // ����Ⱦ�������Դ��
      SRESOURCEINFO*                m_pResourceInfo;      // ������Դ����
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


