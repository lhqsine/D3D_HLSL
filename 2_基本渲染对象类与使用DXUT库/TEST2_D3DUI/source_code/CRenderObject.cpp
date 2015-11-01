//-----------------------------------------------------------------------------
// 文件：CRenderObject.cpp
// 功能：渲染物体头文件
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "CRenderObject.h"

CRENDEROBJECT::CRENDEROBJECT( )
  {
    m_pMesh         = NULL;
    m_pEffect       = NULL;
    m_pColorTexture  = NULL;
  }

CRENDEROBJECT::~CRENDEROBJECT( )
  {
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pMesh );
    SAFE_RELEASE( m_pColorTexture );
  }

//
void CRENDEROBJECT::Init( )
  {
  }
//
HRESULT CRENDEROBJECT::OnCreateDevice( IDirect3DDevice9* pd3dDevice ,TCHAR* FileName )
  {
    HRESULT hr = S_OK;
    if( FAILED( hr = LoadResource( FileName ))) return hr;
    TCHAR* szFile = GetResourceName( ENU_MODELX );
    if( FAILED( hr = LoadMesh( pd3dDevice ,szFile ,&m_pMesh ))) return hr;
    D3DXVECTOR3* pData;
    if( FAILED( hr = m_pMesh->LockVertexBuffer( 0 ,( LPVOID* ) &pData ))) return hr;
    if( FAILED( hr= D3DXComputeBoundingSphere( pData, m_pMesh->GetNumVertices(), D3DXGetFVFVertexSize( m_pMesh->GetFVF() ), &m_vCenter, &m_fObjectRadius ))) return hr;
    if( FAILED( hr = m_pMesh->UnlockVertexBuffer( ))) return hr;
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
    
    return hr;
  }

HRESULT CRENDEROBJECT::OnResetDevice( )
  {
    HRESULT hr = S_OK;
    if( FAILED( hr = m_pEffect->SetTechnique( "DefaultTech" ))) return hr ;
    if( FAILED( hr = m_pEffect->SetTexture( "g_ColorTexture", m_pColorTexture ))) return hr;
    return hr;
  }

void CRENDEROBJECT::OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime )
  {
    HRESULT hr = S_OK;
    hr = m_pEffect->SetMatrix( "g_matWorld", pWorld ) ;
    hr = m_pEffect->SetMatrix( "g_matView", pView ) ;
    hr = m_pEffect->SetMatrix( "g_matProject", pProject );
    hr = m_pEffect->SetFloat( "g_fTime", ( float ) fTime ) ;
  }

void CRENDEROBJECT::OnFrameRender( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr = S_OK;
    //
    UINT iPass, cPasses;
    hr = m_pEffect->Begin( &cPasses, 0 );
    for ( iPass = 0; iPass < cPasses; iPass++ )
      {
        hr  =m_pEffect->BeginPass( iPass );
        hr = m_pMesh->DrawSubset( 0 );
        hr = m_pEffect->EndPass();
      }
    hr = m_pEffect->End();
  }

void CRENDEROBJECT::OnLostDevice()
  {
    if( m_pEffect )   m_pEffect->OnLostDevice();
  }

void CRENDEROBJECT::OnDestroyDevice()
  {
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pMesh );
    SAFE_RELEASE( m_pColorTexture );
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT CRENDEROBJECT::LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh )
  {
    ID3DXMesh* pMesh = NULL;
    WCHAR str[MAX_PATH];
    HRESULT hr;

    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strFileName ) );
    V_RETURN( D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, pd3dDevice, NULL, NULL, NULL, NULL, &pMesh) );

    DWORD *rgdwAdjacency = NULL;

    // Make sure there are normals which are required for lighting
    if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
      {
        ID3DXMesh* pTempMesh;
        V( pMesh->CloneMeshFVF( pMesh->GetOptions(), pMesh->GetFVF() | D3DFVF_NORMAL,  pd3dDevice, &pTempMesh ) );
        V( D3DXComputeNormals( pTempMesh, NULL ) );

        SAFE_RELEASE( pMesh );
        pMesh = pTempMesh;
      }
   
    rgdwAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == NULL )        return E_OUTOFMEMORY;
    V( pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency) );
    V( pMesh->OptimizeInplace( D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL ) );
    delete []rgdwAdjacency;
    *ppMesh = pMesh;
    return S_OK;
  }

//----------------------------------------------------------------------------
// LoadResource
// 功能：将本渲染对象的文件（*.shm）装入，分析供 GetResourceName 使用
//----------------------------------------------------------------------------
HRESULT CRENDEROBJECT::LoadResource( TCHAR* FileName )
  {
    // 打开文件
    BOOL bReturn;
    HANDLE hAndle;
    DWORD  uByte;
    // 文件存在否
    //---------------------- 调试用 --------------------------
    //TCHAR strPath[MAX_PATH];
    //GetCurrentDirectory( MAX_PATH , strPath );
    if( INVALID_HANDLE_VALUE == ( hAndle = CreateFile( FileName,
      GENERIC_READ,0,NULL, OPEN_EXISTING,0,NULL )))    return E_FAIL;
    // ----------------- 检查文件头 --------------------
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
    
    // ------------------ 获取资源个数 -----------------
    m_iResNum  = sFileHead.s_iResourceFileNum;  
    if( NULL == ( m_pResourceInfo = new SRESOURCEINFO[ m_iResNum ] )) return E_FAIL;
    // ----------------- 读取资源信息 ------------------
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
// 功能：利用资源类型索引出文件名
//----------------------------------------------------------------------------
TCHAR* CRENDEROBJECT::GetResourceName( RESTYPE enuResType )
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

