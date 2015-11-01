//-----------------------------------------------------------------------------
// 文件：CRENDEROBJECTDS.cpp
// 功能：渲染物体头文件
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "CRenderObjectDS.h"

CRENDEROBJECTDS::CRENDEROBJECTDS( )
  {
    m_pVB           = NULL;
    m_pIB           = NULL;
    m_dwNumIndices  = 0;
    m_dwNumVertices = 0;
    m_pDecl          = NULL;
    m_pMeshSysMemory    = NULL;
    m_pEffect       = NULL;
    m_pColorTexture  = NULL;
    //
    m_pRTTTextureDepth = NULL;
    m_pRTTInterfaceDepth = NULL;
    m_pRTTSurfaceDepth = NULL;
  }

CRENDEROBJECTDS::~CRENDEROBJECTDS( )
  {
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pDecl );
    SAFE_RELEASE( m_pMeshSysMemory );
    SAFE_RELEASE( m_pColorTexture );
    //
    SAFE_RELEASE( m_pRTTTextureDepth );
    SAFE_RELEASE( m_pRTTInterfaceDepth );
    SAFE_RELEASE( m_pRTTSurfaceDepth );
  }

//
void CRENDEROBJECTDS::Init( )
  {
  }
//
HRESULT CRENDEROBJECTDS::OnCreateDevice( IDirect3DDevice9* pd3dDevice ,TCHAR* FileName )
  {
    HRESULT hr = S_OK;
    if( FAILED( hr = LoadResource( FileName ))) return hr;
    TCHAR* szFile = GetResourceName( ENU_MODELX );
    LPD3DXMESH           pMeshSysMemClone = NULL;
    if( FAILED( hr = D3DXLoadMeshFromX( szFile ,D3DXMESH_SYSTEMMEM ,
      pd3dDevice, NULL, NULL, NULL, NULL, &pMeshSysMemClone ))) return hr;
    // ------------------------ 以新的方式创建顶点流 -------------------------------
    //
    D3DVERTEXELEMENT9 decl[] =
      {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0   }, 
        { 0, 12, D3DDECLTYPE_FLOAT3 ,D3DDECLMETHOD_DEFAULT ,D3DDECLUSAGE_NORMAL  , 0   },
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0   },
        D3DDECL_END()
      };

    // --------- 因为装入的 Mesh 的顶点结构不是我们需要的，因此按我们需要的格式复制一个 ----------
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &m_pDecl ))) return hr;
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
    
    return hr;
  }

HRESULT CRENDEROBJECTDS::OnResetDevice( IDirect3DDevice9* pd3dDevice , const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
  {
    HRESULT hr = S_OK;
    //if( FAILED( hr = m_pEffect->SetTechnique( "DefaultTech" ))) return hr ;
    if( FAILED( hr = m_pEffect->SetTexture( "g_ColorTexture", m_pColorTexture ))) return hr;
    float fTextureWidth , fTextureHeight ;
    fTextureWidth  = pBackBufferSurfaceDesc->Width / 2.0  ;
    fTextureHeight = pBackBufferSurfaceDesc->Height / 2.0;
    //--------------------- 深度纹理 -------------------------
    D3DSURFACE_DESC desc;
    if( FAILED( hr = pd3dDevice->CreateTexture( fTextureWidth , fTextureHeight , 1 ,
      D3DUSAGE_RENDERTARGET, D3DFMT_R32F , D3DPOOL_DEFAULT , &m_pRTTTextureDepth , NULL ))) return hr;
    hr = m_pRTTTextureDepth->GetSurfaceLevel( 0 , &m_pRTTSurfaceDepth );
    hr = m_pRTTSurfaceDepth->GetDesc( &desc );
    if( FAILED( hr = D3DXCreateRenderToSurface( pd3dDevice , desc.Width , desc.Height ,
      desc.Format , TRUE , D3DFMT_D24X8 , &m_pRTTInterfaceDepth )))
      {
        return hr;
      }
    // 
    return hr;
  }

void CRENDEROBJECTDS::OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime ,D3DXVECTOR4* pvEye )
  {
    HRESULT hr = S_OK;
    hr = m_pEffect->SetMatrix( "g_matWorld", pWorld ) ;
    hr = m_pEffect->SetMatrix( "g_matView", pView ) ;
    hr = m_pEffect->SetMatrix( "g_matProject", pProject );
    hr = m_pEffect->SetFloat( "g_fTime", ( float ) fTime ) ;
    hr = m_pEffect->SetVector( "g_vEyePosition", pvEye );
    float Determinant;
    D3DXMATRIX matOut;
    D3DXMatrixInverse( &matOut , &Determinant ,pWorld );
    D3DXMatrixTranspose( &matOut ,&matOut );
    hr = m_pEffect->SetMatrix( "g_matWorldNormalInverseTranspose", &matOut );
  }

void CRENDEROBJECTDS::OnFrameRender( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr = S_OK;
    //
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( RENDEROBJECT_D3DVERTEXDS )); 
    hr = pd3dDevice->SetIndices( m_pIB ); 
    //
    UINT iPass, cPasses;
    hr = m_pEffect->Begin( &cPasses, 0 );
    for ( iPass = 0; iPass < cPasses; iPass++ )
      {
        hr = m_pEffect->BeginPass( iPass );
        //hr = m_pMesh->DrawSubset( 0 );
        hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , 0, 0, m_dwNumVertices, 0,m_dwNumIndices );
        hr = m_pEffect->EndPass();
      }
    hr = m_pEffect->End();
  }

void  CRENDEROBJECTDS::RenderToTexture( LPDIRECT3DDEVICE9 pd3dDevice , D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime )
  {
    HRESULT hr;
    hr = m_pEffect->SetMatrix( "g_matWorldRTT",  pWorld );
    hr = m_pEffect->SetMatrix( "g_matViewRTT",   pView );
    hr = m_pEffect->SetMatrix( "g_matProjectRTT",pProject );    
    // 渲染深度信息到纹理
    hr = m_pEffect->SetTechnique( "RenderDepthTech" );
    if( SUCCEEDED( hr = m_pRTTInterfaceDepth->BeginScene( m_pRTTSurfaceDepth , NULL )))
      {
        hr = pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
        hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
        hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( RENDEROBJECT_D3DVERTEXDS )); 
        hr = pd3dDevice->SetIndices( m_pIB ); 
        
        UINT uPasses;
        hr = m_pEffect->Begin( &uPasses, D3DXFX_DONOTSAVESTATE );
        for( UINT iPass = 0; iPass < uPasses; iPass++ )
          {
            hr = m_pEffect->BeginPass( iPass );
            hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 
              0, m_dwNumVertices, 0, m_dwNumIndices );
            hr = m_pEffect->EndPass();
          }
        hr = m_pEffect->End();
        hr = m_pRTTInterfaceDepth->EndScene( 0 );
        // 
      }
    // 完成任务，可以返回
  }

void CRENDEROBJECTDS::OnLostDevice()
  {
    if( m_pEffect )   m_pEffect->OnLostDevice();
    //
    SAFE_RELEASE( m_pRTTTextureDepth );
    SAFE_RELEASE( m_pRTTInterfaceDepth );
    SAFE_RELEASE( m_pRTTSurfaceDepth );
  }

void CRENDEROBJECTDS::OnDestroyDevice()
  {
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pDecl );
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pMeshSysMemory );
    SAFE_RELEASE( m_pColorTexture );
    //
    SAFE_RELEASE( m_pRTTTextureDepth );
    SAFE_RELEASE( m_pRTTInterfaceDepth );
    SAFE_RELEASE( m_pRTTSurfaceDepth );
  }

//----------------------------------------------------------------------------
// LoadResource
// 功能：将本渲染对象的文件（*.shm）装入，分析供 GetResourceName 使用
//----------------------------------------------------------------------------
HRESULT CRENDEROBJECTDS::LoadResource( TCHAR* FileName )
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
TCHAR* CRENDEROBJECTDS::GetResourceName( RESTYPE enuResType )
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




