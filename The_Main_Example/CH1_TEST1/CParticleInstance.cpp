//-----------------------------------------------------------------------------
// �ļ���CRenderObject.cpp
// ���ܣ���Ⱦ����ͷ�ļ�
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "CParticleInstance.h"
#include <ctime>

// ����Ϊ 12 �� 10
#define XLENGTH 10
#define ZLENGTH 10

CPARTICLEINSTANCE::CPARTICLEINSTANCE( )
  {
    m_pVB           = NULL;
    m_pVBInstance   = NULL;
    m_pIB           = NULL;
    m_dwNumIndices  = 0;
    m_dwNumVertices = 0;
    m_pDecl          = NULL;
    m_pDeclInstance = NULL;
    m_pMeshSysMemory    = NULL;
    m_pEffect       = NULL;
    m_pColorTexture  = NULL;
    // ����
    m_pStatus    = NULL;

  }

CPARTICLEINSTANCE::~CPARTICLEINSTANCE( )
  {
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pVBInstance );
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pDecl );
    SAFE_RELEASE( m_pDeclInstance );
    SAFE_RELEASE( m_pMeshSysMemory );
    SAFE_RELEASE( m_pColorTexture );
    // ����
    SAFE_DELETE_ARRAY( m_pStatus );
  }

//
void CPARTICLEINSTANCE::Init(  )
  {
    m_iParticleNum = XLENGTH * ZLENGTH ;
  }

HRESULT CPARTICLEINSTANCE::OnCreateDevice( IDirect3DDevice9* pd3dDevice ,TCHAR* FileName )
  {
    HRESULT hr = S_OK;
    //---------------------------------
    // ����״̬��ʼ��
    //---------------------------------
    if( 0 == m_iParticleNum ) return E_FAIL;
    if( NULL == ( m_pStatus = new CPARTICLESTATUS[ m_iParticleNum ] )) return E_FAIL;
    // ��ʼ��λ��
    for( int i = 0 ; i < m_iParticleNum ; i ++ )
      {
        m_pStatus[ i ].InitPositionColor( );
      }
    //
    if( FAILED( hr = LoadResource( FileName ))) return hr;
    TCHAR* szFile = GetResourceName( ENU_MODELX );
    LPD3DXMESH           pMeshSysMemClone = NULL;
    if( FAILED( hr = D3DXLoadMeshFromX( szFile ,D3DXMESH_SYSTEMMEM ,
      pd3dDevice, NULL, NULL, NULL, NULL, &pMeshSysMemClone ))) return hr;
    // ------------------------ ���µķ�ʽ���������� -------------------------------
    //
    D3DVERTEXELEMENT9 decl[] =
      {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0   }, 
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0   },
        D3DDECL_END()
      };
    D3DVERTEXELEMENT9 decl_Instance[] =
      {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0   }, 
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0   },
        { 1, 0,  D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1   },
        D3DDECL_END()
      };
    // --------- ��Ϊװ��� Mesh �Ķ���ṹ����������Ҫ�ģ���˰�������Ҫ�ĸ�ʽ����һ�� ----------
    //if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl , &m_pDecl ))) return hr;
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl_Instance , &m_pDeclInstance ))) return hr;
    if( FAILED( hr = pMeshSysMemClone->CloneMesh( D3DXMESH_MANAGED, decl, pd3dDevice, &m_pMeshSysMemory ))) return hr;
    if( FAILED( hr = m_pMeshSysMemory->GetIndexBuffer( &m_pIB ))) return hr;
    if( FAILED( hr = m_pMeshSysMemory->GetVertexBuffer( &m_pVB ))) return hr;
    m_dwNumIndices  = m_pMeshSysMemory->GetNumFaces();
    m_dwNumVertices = m_pMeshSysMemory->GetNumVertices();
    SAFE_RELEASE( pMeshSysMemClone );
    // 
    D3DXVECTOR3* pData;
    if( FAILED( hr = m_pMeshSysMemory->LockVertexBuffer( 0 ,( LPVOID* ) &pData ))) return hr;
    if( FAILED( hr= D3DXComputeBoundingSphere( pData, m_pMeshSysMemory->GetNumVertices(), D3DXGetFVFVertexSize( m_pMeshSysMemory->GetFVF() ), &m_vCenter, &m_fObjectRadius ))) return hr;
    if( FAILED( hr = m_pMeshSysMemory->UnlockVertexBuffer( ))) return hr;
    //SAFE_RELEASE( pMeshSysMemClone );
    //
    szFile = GetResourceName( ENU_FXFILE );
    LPD3DXBUFFER pCode = NULL;
    if( FAILED( hr = D3DXCreateEffectFromFile( pd3dDevice, szFile, NULL, NULL, 0, NULL, &m_pEffect, &pCode )))
      {
        LPVOID pBufErr = pCode->GetBufferPointer();
        char* ErrInfo = ( char* ) pBufErr;
        pCode->Release( );
        return hr;
      }
    //
    szFile = GetResourceName( ENU_TEXCOLORMAP );
    if( FAILED( hr = D3DXCreateTextureFromFileEx( pd3dDevice, szFile, D3DX_DEFAULT, D3DX_DEFAULT, 
      D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
      D3DX_DEFAULT, D3DX_DEFAULT, 0, 
      NULL, NULL, &m_pColorTexture ) )) return hr;
    //-------------------------
    // ����ʵ������������������
    //-------------------------
    if( FAILED( hr = pd3dDevice->CreateVertexBuffer( m_iParticleNum * sizeof( PARTICLE_D3DVERTEX_INSTANCE ), 0, 0, 
      D3DPOOL_MANAGED, &m_pVBInstance , 0 )))      return hr;
    PARTICLE_D3DVERTEX_INSTANCE* pInstance;
    if( SUCCEEDED( hr = m_pVBInstance->Lock( 0, NULL,  (void**) &pInstance, 0 )))
      {
        for( int x = 0 ; x < XLENGTH ; x++ )     // X = 50 ; Z = 20
          {
            for( int z = 0 ; z < ZLENGTH ; z ++ )
              {
                // ÿ�������ʵ��ID
                pInstance[ x * ZLENGTH + z ].fInstance         = x * ZLENGTH + z; 
              }
          }
        m_pVBInstance->Unlock( );
      }
    // 
    return hr;
  }

HRESULT CPARTICLEINSTANCE::OnResetDevice( )
  {
    HRESULT hr = S_OK;
    if( FAILED( hr = m_pEffect->SetTechnique( "DefaultTech" ))) return hr ;
    if( FAILED( hr = m_pEffect->SetTexture( "g_ColorTexture", m_pColorTexture ))) return hr;
    return hr;
  }
//--------------------------------------------------------------------
// ������OnFrameMove
// ���ܣ�
// pEye ����������ϵ�е���Ӱ��λ�ã�pPosition ����������ϵ�е�����λ��
//--------------------------------------------------------------------
void CPARTICLEINSTANCE::OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,
  double fTime , float fElapsedTime , D3DXVECTOR3* pCameraPosition )
  {
    HRESULT hr = S_OK;
    hr = m_pEffect->SetMatrix( "g_matWorld", pWorld ) ;
    hr = m_pEffect->SetMatrix( "g_matView", pView ) ;
    hr = m_pEffect->SetMatrix( "g_matProject", pProject );
    hr = m_pEffect->SetFloat( "g_fTime", ( float ) fTime ) ;
    // ������Ӱ��λ��
    D3DXVECTOR4 v4CameraPosition = D3DXVECTOR4( pCameraPosition->x , pCameraPosition->y , pCameraPosition->z , 1.0f );
    hr = m_pEffect->SetVector( "g_vLookAt" , &v4CameraPosition );   // g_vLookAt ������Ӱ��λ��
    // 
    // #define XLENGTH 12
    // #define ZLENGTH 8
    D3DXVECTOR4 vpPositionArray[ XLENGTH * ZLENGTH ];
    D3DXVECTOR4 vpLifeStatusArray[ XLENGTH * ZLENGTH ];
    for( int x = 0 ; x < XLENGTH ; x ++ )
      {
        for( int z = 0 ; z < ZLENGTH ; z++ )
          {
            int i = x * ZLENGTH + z ;
            m_pStatus[ i ].OnFrameMove( fTime , fElapsedTime , i );
            // ��λ�ô���
            D3DXVECTOR4 v4Temp     = m_pStatus[ i ].GetPosition( );
            vpPositionArray[ i ]   = v4Temp;
            v4Temp                 = m_pStatus[ i ].GetColor( );
            vpLifeStatusArray[ i ] = v4Temp ;
          }
      }
    hr = m_pEffect->SetVectorArray( "g_vVectorArray" , vpPositionArray , XLENGTH * ZLENGTH );
    hr = m_pEffect->SetVectorArray( "g_vLifeStatus" , vpLifeStatusArray , XLENGTH * ZLENGTH );

    // ���Է�����
  }

void CPARTICLEINSTANCE::OnFrameRenderInstance( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr = S_OK;
    //
    hr = pd3dDevice->SetVertexDeclaration( m_pDeclInstance );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( PARTICLE_D3DVERTEX )); 
    hr = pd3dDevice->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | m_iParticleNum );
    //hr = pd3dDevice->SetIndices( m_pIB ); 
    // 
    hr = pd3dDevice->SetStreamSource( 1 , m_pVBInstance , 0 , sizeof( PARTICLE_D3DVERTEX_INSTANCE ));
    hr = pd3dDevice->SetStreamSourceFreq( 1 , D3DSTREAMSOURCE_INSTANCEDATA | 1ul );
    hr = pd3dDevice->SetIndices( m_pIB );
    // ���� BillBoard ��λ�ú���Ӱ��λ��
    UINT iPass, cPasses;
    hr = m_pEffect->Begin( &cPasses , 0 );
    for( iPass = 0 ; iPass < cPasses ; iPass++ )
      {
        hr = m_pEffect->BeginPass( iPass );
        hr = m_pEffect->CommitChanges( );
        hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , 0 , 0 , m_dwNumVertices , 0 , m_dwNumIndices ) ;
        hr = m_pEffect->EndPass( );
      }
    hr = m_pEffect->End( );
    hr = pd3dDevice->SetStreamSourceFreq( 0 , 1 );
    hr = pd3dDevice->SetStreamSourceFreq( 1 , 1 );
    // 
  }
/*
void CPARTICLEINSTANCE::OnFrameRender( IDirect3DDevice9* pd3dDevice , D3DXVECTOR3* pCameraPosition )
  {
    HRESULT hr = S_OK;
    //
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( PARTICLE_D3DVERTEX )); 
    hr = pd3dDevice->SetIndices( m_pIB ); 
    // 
    // ���� BillBoard ��λ�ú���Ӱ��λ��
    UINT iPass, cPasses;
    hr = m_pEffect->Begin( &cPasses, D3DXFX_DONOTSAVESAMPLERSTATE );
    for( int i = 0 ; i < m_iParticleNum ; i ++ )
      {
        //m_pMSNext[ i ] = RK4( &m_pMSCurrent[ i ] , fTime , fElapsedTime );
        //D3DXVECTOR4 v4BillBoardPosition  = D3DXVECTOR4( m_pMSNext[ i ].v3Position.x , m_pMSNext[ i ].v3Position.y , m_pMSNext[ i ].v3Position.z , 1.0f );
        //D3DXVECTOR4 v4BillBoardPosition  = D3DXVECTOR4( 3.0 , 3.0 , 3.0 , 1.0 );
        D3DXVECTOR4 v4CameraPosition = D3DXVECTOR4( pCameraPosition->x , pCameraPosition->y , pCameraPosition->z , 1.0f );
        hr = m_pEffect->SetVector( "g_vEyePt" , &v4BillBoardPosition );       // g_vEyePt ���� BillBoard λ��
        hr = m_pEffect->SetVector( "g_vLookAt" , &v4CameraPosition );   // g_vLookAt ������Ӱ��λ��
        //UINT iPass, cPasses;
        for ( iPass = 0; iPass < cPasses; iPass++ )
          {
            hr = m_pEffect->BeginPass( iPass );
            hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , 0, 0, m_dwNumVertices, 0,m_dwNumIndices );
            hr = m_pEffect->EndPass();
          }
      }
    hr = m_pEffect->End();
    // 
  }
*/

void CPARTICLEINSTANCE::OnLostDevice()
  {
    if( m_pEffect )   m_pEffect->OnLostDevice();
  }

void CPARTICLEINSTANCE::OnDestroyDevice()
  {
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pVBInstance );
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pDecl );
    SAFE_RELEASE( m_pDeclInstance );
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pMeshSysMemory );
    SAFE_RELEASE( m_pColorTexture );
    //
    SAFE_DELETE_ARRAY( m_pStatus );
  }

//----------------------------------------------------------------------------
// LoadResource
// ���ܣ�������Ⱦ������ļ���*.shm��װ�룬������ GetResourceName ʹ��
//----------------------------------------------------------------------------
HRESULT CPARTICLEINSTANCE::LoadResource( TCHAR* FileName )
  {
    // ���ļ�
    BOOL bReturn;
    HANDLE hAndle;
    DWORD  uByte;
    // �ļ����ڷ�
    //---------------------- ������ --------------------------
    //TCHAR strPath[MAX_PATH];
    //GetCurrentDirectory( MAX_PATH , strPath );
    if( INVALID_HANDLE_VALUE == ( hAndle = CreateFile( FileName,
      GENERIC_READ,0,NULL, OPEN_EXISTING,0,NULL )))    return E_FAIL;
    // ----------------- ����ļ�ͷ --------------------
    SSHADERMODULEFILEHEAD sFileHead;
    if( NULL == ( bReturn = ReadFile( hAndle ,( LPVOID )&sFileHead ,sizeof( SSHADERMODULEFILEHEAD ),
      &uByte , NULL )))
      {
        CloseHandle( hAndle );
        return E_FAIL;
      }
    
    if( NULL != strcmp( FILEHEADTAG , sFileHead.s_szHead )) 
      {
        CloseHandle( hAndle );
        return E_FAIL;
      }
    
    // ------------------ ��ȡ��Դ���� -----------------
    m_iResNum  = sFileHead.s_iResourceFileNum;  
    if( NULL == ( m_pResourceInfo = new SRESOURCEINFO[ m_iResNum ] )) return E_FAIL;
    // ----------------- ��ȡ��Դ��Ϣ ------------------
    if( NULL == ( bReturn = ReadFile( hAndle ,( LPVOID )m_pResourceInfo ,sizeof( SRESOURCEINFO ) * m_iResNum ,
      &uByte , NULL )))
      {
        CloseHandle( hAndle );
        return E_FAIL;
      }
    // 
    CloseHandle( hAndle );
    return S_OK;
  }

//----------------------------------------------------------------------------
// GetResourceName
// ���ܣ�������Դ�����������ļ���
//----------------------------------------------------------------------------
TCHAR* CPARTICLEINSTANCE::GetResourceName( RESTYPE enuResType )
  {
    TCHAR* szReturn = NULL;
    for( int i = 0 ; i < m_iResNum ;i ++ )
      {
        if( enuResType == m_pResourceInfo[ i ].s_enuResType ) 
          {
            szReturn = m_pResourceInfo[ i ].s_szFileName;
          }
      }
    return szReturn;
  }

