//-----------------------------------------------------------------------------
// 文件：CRENDERTOCUBEMAP.cpp 
// 功能：渲染物体头文件
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "CRENDERTOCUBEMAP.h"

//----------------------------------------------------------------------------
// CRENDERTOCUBEMAP
// 
//----------------------------------------------------------------------------
CRENDERTOCUBEMAP::CRENDERTOCUBEMAP( )
  {
    m_pVB           = NULL;
    m_pIB           = NULL;
    m_dwNumIndices  = 0;
    m_dwNumVertices = 0;
    m_pDecl          = NULL;
    m_pMeshSysMemory    = NULL;
    m_pEffect       = NULL;
    m_pColorTexture  = NULL;
    m_pRTTSurfaceBox       = NULL;
    //
    m_TextureSize  = 256;
    m_pRTTTextureBox = NULL;
    //m_pRTTInterfaceBox = NULL;
    m_pRTTSurfaceBox = NULL;
    m_pRTTDepthStencil = NULL;
    //
    m_vBaryCentric = D3DXVECTOR3( 0.0 , 0.0 , 0.0 );
    D3DXMatrixIdentity( &m_matWorldRTT );
    //
    m_iCount          = 0;
  }

//----------------------------------------------------------------------------
// ~CRENDERTOCUBEMAP
// 
//----------------------------------------------------------------------------
CRENDERTOCUBEMAP::~CRENDERTOCUBEMAP( )
  {
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pDecl );
    SAFE_RELEASE( m_pMeshSysMemory );
    SAFE_RELEASE( m_pColorTexture );
    SAFE_RELEASE( m_pRTTSurfaceBox );
    SAFE_RELEASE( m_pRTTDepthStencil );
    //
    SAFE_RELEASE( m_pRTTTextureBox );
    //SAFE_RELEASE( m_pRTTInterfaceBox );
  }

//----------------------------------------------------------------------------
// Init
// 
//----------------------------------------------------------------------------
void CRENDERTOCUBEMAP::Init( )
  {
  }

//----------------------------------------------------------------------------
// OnCreateDevice
// 
//----------------------------------------------------------------------------
HRESULT CRENDERTOCUBEMAP::OnCreateDevice( IDirect3DDevice9* pd3dDevice ,TCHAR* FileName )
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
        { 0, 0,  D3DDECLTYPE_FLOAT3 , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0   }, 
        { 0, 12, D3DDECLTYPE_FLOAT3 , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,    0   },
        { 0, 24, D3DDECLTYPE_FLOAT2 , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0   },
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
    DWORD dwFlag = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION ;
    if( FAILED( hr = D3DXCreateEffectFromFile( pd3dDevice , szFile , NULL , NULL , dwFlag , NULL , &m_pEffect , &pCode )))
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
    // 
    return hr;
  }

//------------------------------------------------------------
// 函数：OnResetDevice
// 功能：根据格式创建纹理，再从纹理页面创建渲染接口
// -----------------------------------------------------------
HRESULT CRENDERTOCUBEMAP::OnResetDevice( IDirect3DDevice9* pd3dDevice ,float fAspectRatio )
  {
    HRESULT hr = S_OK;
    if( FAILED( hr = m_pEffect->SetTechnique( "DefaultCubeTech" ))) return hr ;
    if( FAILED( hr = m_pEffect->SetTexture( "g_ColorTexture", m_pColorTexture ))) return hr;
    //------------------------------------------------------------
    // 核心代码：根据纹理大小创建一个 D3DFMT_A8R8G8B8 格式的纹理
    // -----------------------------------------------------------
    if( FAILED( hr = D3DXCreateCubeTexture( pd3dDevice , m_TextureSize , 1 , 
      D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pRTTTextureBox )))
      {
        if( FAILED( hr = D3DXCreateCubeTexture( pd3dDevice, m_TextureSize , 1 , 
          0 ,D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pRTTTextureBox )))
          {
            return hr;
          }
      }
    // 
    /*
    D3DSURFACE_DESC desc;
    hr = m_pRTTTextureBox->GetSurfaceLevel( 0 , &m_pRTTInterfaceBox );
    hr = m_pRTTSurfaceBox->GetDesc( &desc );
    
    if( FAILED( hr = D3DXCreateRenderToEnvMap( pd3dDevice , desc.Width , desc.Height ,
      desc.Format , TRUE,D3DFMT_D16 , &m_pRTTInterface )))
      {
        return hr;
      }
    */
    // 创建深度模板缓冲区
    DXUTDeviceSettings d3dSettings = DXUTGetDeviceSettings();
    hr = pd3dDevice->CreateDepthStencilSurface( m_TextureSize , m_TextureSize , d3dSettings.pp.AutoDepthStencilFormat ,
      D3DMULTISAMPLE_NONE , 0 , TRUE , &m_pRTTDepthStencil , NULL );
    //-------------------------- 投影矩阵 ------------------------------------------
    D3DXMatrixPerspectiveFovLH( &m_matProjectRTT , D3DX_PI/4 , fAspectRatio , 1.0f , 100.0f );
    //
    return hr;
  }

//----------------------------------------------------------------------------
// OnFrameMove
// 
//----------------------------------------------------------------------------
void CRENDERTOCUBEMAP::OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,D3DXVECTOR3* pv3EyePt, double fTime )
  {
    HRESULT hr = S_OK;
    hr = m_pEffect->SetMatrix( "g_matWorld", pWorld ) ;
    hr = m_pEffect->SetMatrix( "g_matView", pView ) ;
    hr = m_pEffect->SetMatrix( "g_matProject", pProject );
    hr = m_pEffect->SetFloat( "g_fTime", ( float ) fTime ) ;
    //
    D3DXVECTOR4 v4EyePt = D3DXVECTOR4( pv3EyePt->x , pv3EyePt->y , pv3EyePt->z , 0.0 );
    hr = m_pEffect->SetVector( "g_ViewPosition" , &v4EyePt );
    //
    float Determinant;
    D3DXMATRIX matOut;
    D3DXMatrixInverse( &matOut , &Determinant ,pWorld );
    D3DXMatrixTranspose( &matOut , &matOut );
    hr = m_pEffect->SetMatrix( "g_matWorldNormalInverseTranspose", &matOut );
    // 
  }

//----------------------------------------------------------------------------
// 函数：OnFrameRender
// 功能：将立方体纹理贴到到小球上
//----------------------------------------------------------------------------
void CRENDERTOCUBEMAP::OnFrameRender( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr = S_OK;
    //
    hr = m_pEffect->SetTechnique( "DefaultCubeTech" );
    hr = m_pEffect->SetTexture( "g_CubeTexture" , m_pRTTTextureBox );
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( CRENDERTOCUBEMAP_D3DVERTEX )); 
    hr = pd3dDevice->SetIndices( m_pIB ); 
    //
    UINT iPass, cPasses;
    hr = m_pEffect->Begin( &cPasses, 0 );
    for ( iPass = 0; iPass < cPasses; iPass++ )
      {
        hr = m_pEffect->BeginPass( iPass );
        hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , 0, 0, m_dwNumVertices, 0,m_dwNumIndices );
        hr = m_pEffect->EndPass();
      }
    hr = m_pEffect->End();
  }

//----------------------------------------------------------------------------
// 函数：RenderToCubeMap
// 功能：渲染整个场景到立方体纹理，其中有几个注意点:
//       世界矩阵：需要按照小球所处的位置设置
//       观察矩阵：即从小球的世界空间坐标来观察世界
//       投影矩阵：可以按程序设置的方式来决定
//----------------------------------------------------------------------------
HRESULT CRENDERTOCUBEMAP::RenderToCubeMap( LPDIRECT3DDEVICE9 pd3dDevice , D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime )
  {
    HRESULT hr;
    D3DXMATRIXA16 matView = *pView;
    matView._41 = matView._43 = matView._42 = 0;
    D3DXMATRIXA16 matProject;
    D3DXMatrixPerspectiveFovLH( &matProject , D3DX_PI * 0.5f , 1.0f , 1.00f , 100.0f );
    // 开始渲染
    LPDIRECT3DSURFACE9 pRTOld = NULL;
    V( hr = pd3dDevice->GetRenderTarget( 0, &pRTOld ) );
    LPDIRECT3DSURFACE9 pDSOld = NULL;
    if( SUCCEEDED( hr = pd3dDevice->GetDepthStencilSurface( &pDSOld )))
      {
        V( hr = pd3dDevice->SetDepthStencilSurface( m_pRTTDepthStencil ));
      }
    for( int iFace = 0 ; iFace < 6 ; iFace ++ )
      {
        LPDIRECT3DSURFACE9 pSurf = NULL;
        hr = m_pRTTTextureBox->GetCubeMapSurface(( D3DCUBEMAP_FACES )iFace , 0 , &pSurf );
        hr = pd3dDevice->SetRenderTarget( 0 , pSurf );
        SAFE_RELEASE( pSurf );
        //
        D3DXMATRIXA16 mView = DXUTGetCubeMapViewMatrix( iFace );
        //D3DXMatrixMultiply( &mView , &matView , &mView );
        //
        hr = pd3dDevice->Clear( 0 , NULL , D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , 0x000000ff , 1.0 , 0 );
        if( SUCCEEDED( hr = pd3dDevice->BeginScene( )))
          {
            for( int i = 0 ; i < m_iCount ; i ++ )
              {
                switch( m_pCRenderObject[ i ].eType )
                  {
                    case ENU_RENDEROBJECT:
                      (( CRENDEROBJECT* )m_pCRenderObject[ i ].pType )->OnFrameMove( pWorld , &mView , &matProject , fTime );
                      (( CRENDEROBJECT* )m_pCRenderObject[ i ].pType )->OnFrameRender( pd3dDevice );
                      break;
                    case ENU_CUBE:
                      (( CRENDEROBJECTCUBE* )m_pCRenderObject[ i ].pType )->OnFrameMove( pWorld , &mView , &matProject , fTime );
                      (( CRENDEROBJECTCUBE* )m_pCRenderObject[ i ].pType )->OnFrameRender( pd3dDevice );
                      break;
                  }
              }
            pd3dDevice->EndScene( );
          }
      }
    if( pDSOld )
      {
        hr = pd3dDevice->SetDepthStencilSurface( pDSOld );
        SAFE_RELEASE( pDSOld );
        pDSOld = NULL;
      }
    hr = pd3dDevice->SetRenderTarget( 0 , pRTOld );
    SAFE_RELEASE( pRTOld );
    pRTOld = NULL;
    return hr;
  }
/*
HRESULT CRENDERTOCUBEMAP::SetTexture( LPDIRECT3DTEXTURE9 pTexture ) 
  {
    HRESULT hr;
    hr = m_pEffect->SetTexture( "g_RenderToTexture",pTexture );
    return hr;
  }
*/
void CRENDERTOCUBEMAP::OnLostDevice()
  {
    if( m_pEffect )   m_pEffect->OnLostDevice();
    // 
    SAFE_RELEASE( m_pRTTSurfaceBox );
    //SAFE_RELEASE( m_pRTTInterfaceBox );
    SAFE_RELEASE( m_pRTTTextureBox );
    SAFE_RELEASE( m_pRTTDepthStencil );
  }

void CRENDERTOCUBEMAP::OnDestroyDevice()
  {
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pDecl );
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pMeshSysMemory );
    SAFE_RELEASE( m_pColorTexture );
    //
    SAFE_RELEASE( m_pRTTSurfaceBox );
    //SAFE_RELEASE( m_pRTTInterfaceBox );
    SAFE_RELEASE( m_pRTTTextureBox );
    SAFE_RELEASE( m_pRTTDepthStencil );
  }

//----------------------------------------------------------------------------
// LoadResource
// 功能：将本渲染对象的文件（*.shm）装入，分析供 GetResourceName 使用
//----------------------------------------------------------------------------
HRESULT CRENDERTOCUBEMAP::LoadResource( TCHAR* FileName )
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
TCHAR* CRENDERTOCUBEMAP::GetResourceName( RESTYPE enuResType )
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

void CRENDERTOCUBEMAP::AddToRenderList( void* pRenderObject , ERENDERTYPE eType )
  {
    m_pCRenderObject[ m_iCount ].pType  = pRenderObject;
    m_pCRenderObject[ m_iCount ].eType  = eType ;
    m_iCount = m_iCount + 1;
  }

