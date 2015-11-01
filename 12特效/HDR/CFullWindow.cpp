//-----------------------------------------------------------------------------
// �ļ���CFULLWINDOW.Cpp
// ���ܣ�Board ����ʵ�� 
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "CFullWindow.h"

CFULLWINDOW::CFULLWINDOW( )
  {
    m_pVB           = NULL;
    m_pIB           = NULL;
    m_dwNumIndices  = 0;
    m_dwNumVertices = 0;
    m_pDecl         = NULL;
    m_pEffect       = NULL;
    //
    m_pRTTTexture_Scene   = NULL;
    m_pRTTInterface_Scene = NULL;
    m_pRTTSurface_Scene   = NULL;
    // 
    m_pRTTTextureBloom_DownSize_3x3 = NULL;
    m_pRTTInterfaceBloom_DownSize_3x3 = NULL;
    m_pRTTSurfaceBloom_DownSize_3x3  = NULL;
    // 
    m_pRTTTextureBloom_DownSize_3x3_Horizon = NULL;
    m_pRTTInterfaceBloom_DownSize_3x3_Horizon = NULL;
    m_pRTTSurfaceBloom_DownSize_3x3_Horizon = NULL;
    // 
    m_pRTTTextureBloom_DownSize_3x3_Vertical = NULL;
    m_pRTTInterfaceBloom_DownSize_3x3_Vertical = NULL;
    m_pRTTSurfaceBloom_DownSize_3x3_Vertical = NULL;
    // 
    m_iCount        = 0;
    for( int i = 0 ; i < MAXRENDEROBJECT ; i ++ )
      {
        m_pCRenderObject[ i ].lpVoid = NULL;
      }
  }

CFULLWINDOW::~CFULLWINDOW( )
  {
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    m_dwNumIndices   = 0;
    m_dwNumVertices  = 0;
    SAFE_RELEASE( m_pDecl );
    SAFE_RELEASE( m_pEffect );
    //
    SAFE_RELEASE( m_pRTTTexture_Scene );
    SAFE_RELEASE( m_pRTTInterface_Scene );
    SAFE_RELEASE( m_pRTTSurface_Scene );
    //
    SAFE_RELEASE( m_pRTTTextureBloom_DownSize_3x3 );
    SAFE_RELEASE( m_pRTTInterfaceBloom_DownSize_3x3 );
    SAFE_RELEASE( m_pRTTSurfaceBloom_DownSize_3x3 );
    // 
    SAFE_RELEASE( m_pRTTTextureBloom_DownSize_3x3_Horizon );
    SAFE_RELEASE( m_pRTTInterfaceBloom_DownSize_3x3_Horizon );
    SAFE_RELEASE( m_pRTTSurfaceBloom_DownSize_3x3_Horizon );
    // 
    SAFE_RELEASE( m_pRTTTextureBloom_DownSize_3x3_Vertical );
    SAFE_RELEASE( m_pRTTInterfaceBloom_DownSize_3x3_Vertical );
    SAFE_RELEASE( m_pRTTSurfaceBloom_DownSize_3x3_Vertical );
    // 
    for( int i = 0 ; i < MAXRENDEROBJECT ; i ++ )
      {
        m_pCRenderObject[ i ].lpVoid = NULL;
      }
  }
//
void CFULLWINDOW::Init( )
  {
    for( int i = 0 ; i < MAXRENDEROBJECT ; i ++ )
      {
        m_pCRenderObject[ i ].lpVoid = NULL;
      }
  }

HRESULT CFULLWINDOW::OnCreateDevice( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr = S_OK;
    TCHAR szAppPath[MAX_PATH];
    GetCurrentDirectory( MAX_PATH, szAppPath );
    // װ������
    if( FAILED( hr = D3DXCreateTextureFromFileEx( pd3dDevice,  L"ColorMap.bmp", D3DX_DEFAULT, D3DX_DEFAULT, 
      D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 
      NULL, NULL, &m_pColorTexture ) )) return hr;  
    // ------------------------ ���µķ�ʽ���������� -------------------------------
    LPD3DXMESH           pMeshSysMemClone = NULL;
    D3DVERTEXELEMENT9 decl[] =
      {
        { 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0   }, 
        { 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0   },
        { 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0   },
        D3DDECL_END()
      };
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &m_pDecl ))) return hr;
    // ---------------------------------------------------------------------------
    LPD3DXBUFFER pCode = NULL;
    DWORD dwFlag = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION ;
    if( FAILED( hr = D3DXCreateEffectFromFile( pd3dDevice, L"FileFX.fx", NULL, NULL, 
      dwFlag, NULL, &m_pEffect, &pCode )))    
      {
        LPVOID pBufErr = pCode->GetBufferPointer();
        char* ErrInfo = ( char* ) pBufErr;
        pCode->Release( );
        return hr;
      }
    return hr;
  }

HRESULT CFULLWINDOW::OnResetDevice( IDirect3DDevice9* pd3dDevice , UINT uWidth , UINT uHeight )
  {
    HRESULT hr = S_OK;
    if( FAILED( hr = CreateSquare( pd3dDevice ))) return hr;
    //hr = m_pEffect->SetTexture( "texColorTexture" ,m_pColorTexture );
    // ------------------------------------------------
    // ����[1]���ǳ���������С�Ǵ��ڱ߳���С��1/3�����ڴ�СΪ 640 * 410 ����������СΪ 213 * 137
    //-------------------------------------------------
    #define DOWNSIZE_SCENE  3
    if( FAILED( hr = D3DXCreateTexture( pd3dDevice , uWidth / DOWNSIZE_SCENE , uHeight / DOWNSIZE_SCENE , 1 , 
      D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F , D3DPOOL_DEFAULT, &m_pRTTTexture_Scene )))
      {
        if( FAILED( hr = D3DXCreateTexture( pd3dDevice , uWidth , uHeight , 1 , 
          0 , D3DFMT_A8R8G8B8 , D3DPOOL_DEFAULT , &m_pRTTTexture_Scene )))
          {
            return hr;
          }
      }
    D3DSURFACE_DESC desc;
    hr = m_pRTTTexture_Scene->GetSurfaceLevel( 0, &m_pRTTSurface_Scene );
    hr = m_pRTTSurface_Scene->GetDesc( &desc );
    if( FAILED( hr = D3DXCreateRenderToSurface( pd3dDevice , desc.Width , desc.Height ,
      desc.Format , TRUE , D3DFMT_D16 , &m_pRTTInterface_Scene )))
      {
        return hr;
      }
    // ------------------------------------------------
    // ����[2]������Ϊ��m_pRTTTextureBloom_DownSize_3x3����Сֻ�д��ڵ� 1 / 9����ʽʹ�� D3DFMT_A32B32G32R32
    //-------------------------------------------------
    #define DOWNSIZE_BLOOM 9
    if( FAILED( hr = D3DXCreateTexture( pd3dDevice , uWidth / DOWNSIZE_BLOOM , uHeight / DOWNSIZE_BLOOM , 1 ,
      D3DUSAGE_RENDERTARGET , D3DFMT_A32B32G32R32F , D3DPOOL_DEFAULT , &m_pRTTTextureBloom_DownSize_3x3 )))
      {
        return hr;
      }
    hr = m_pRTTTextureBloom_DownSize_3x3->GetSurfaceLevel( 0 , &m_pRTTSurfaceBloom_DownSize_3x3 );
    hr = m_pRTTSurfaceBloom_DownSize_3x3->GetDesc( &desc );
    if( FAILED( hr = D3DXCreateRenderToSurface( pd3dDevice , uWidth / DOWNSIZE_BLOOM , uHeight / DOWNSIZE_BLOOM , 
      desc.Format , TRUE , D3DFMT_D16 , &m_pRTTInterfaceBloom_DownSize_3x3 )))
      {
        return hr;
      }
    // ------------------------------------------------
    // ����[3]������Ϊ��m_pRTTTextureBloom_DownSize_3x3_Horizon����Сֻ�д��ڵ� 1 / 9����ʽʹ�� D3DFMT_A32B32G32R32
    //-------------------------------------------------
    if( FAILED( hr = D3DXCreateTexture( pd3dDevice , uWidth / DOWNSIZE_BLOOM , uHeight / DOWNSIZE_BLOOM , 1 ,
      D3DUSAGE_RENDERTARGET , D3DFMT_A32B32G32R32F , D3DPOOL_DEFAULT , &m_pRTTTextureBloom_DownSize_3x3_Horizon )))
      {
        return hr;
      }
    hr = m_pRTTTextureBloom_DownSize_3x3_Horizon->GetSurfaceLevel( 0 , &m_pRTTSurfaceBloom_DownSize_3x3_Horizon );
    hr = m_pRTTSurfaceBloom_DownSize_3x3_Horizon->GetDesc( &desc );
    if( FAILED( hr = D3DXCreateRenderToSurface( pd3dDevice , uWidth / DOWNSIZE_BLOOM , uHeight / DOWNSIZE_BLOOM , 
      desc.Format , TRUE , D3DFMT_D16 , &m_pRTTInterfaceBloom_DownSize_3x3_Horizon )))
      {
        return hr;
      }
    // ------------------------------------------------
    // ����[4]������Ϊ��m_pRTTTextureBloom_DownSize_3x3_Vertical����Сֻ�д��ڵ� 1 / 9����ʽʹ�� D3DFMT_A32B32G32R32
    //-------------------------------------------------
    if( FAILED( hr = D3DXCreateTexture( pd3dDevice , uWidth / DOWNSIZE_BLOOM , uHeight / DOWNSIZE_BLOOM , 1 ,
      D3DUSAGE_RENDERTARGET , D3DFMT_A32B32G32R32F , D3DPOOL_DEFAULT , &m_pRTTTextureBloom_DownSize_3x3_Vertical )))
      {
        return hr;
      }
    hr = m_pRTTTextureBloom_DownSize_3x3_Vertical->GetSurfaceLevel( 0 , &m_pRTTSurfaceBloom_DownSize_3x3_Vertical );
    hr = m_pRTTSurfaceBloom_DownSize_3x3_Vertical->GetDesc( &desc );
    if( FAILED( hr = D3DXCreateRenderToSurface( pd3dDevice , uWidth / DOWNSIZE_BLOOM , uHeight / DOWNSIZE_BLOOM , 
      desc.Format , TRUE , D3DFMT_D16 , &m_pRTTInterfaceBloom_DownSize_3x3_Vertical )))
      {
        return hr;
      }
    return hr;
  }

void CFULLWINDOW::OnFrameMove( float fTime , D3DXVECTOR4 v4Position )
  {
    HRESULT hr;
    hr = m_pEffect->SetFloat( "g_fTime", ( float ) fTime ) ;
    //hr = m_pEffect->SetVector( "g_v4Position" , &v4Position );
  }

void CFULLWINDOW::OnFrameRender( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr;
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( BOARD_D3DVERTEX )); 
    hr = pd3dDevice->SetIndices( m_pIB ); 
    UINT iPass, cPasses;
    // ------------------------- ScreenShader ---------------------------
    // ���ڹ�ѡ����� m_pRTTTexture_Scene��m_pRTTTextureBloom_DownSize_3x3��m_pRTTTextureBloom_DownSize_3x3_Horizon��m_pRTTTextureBloom_DownSize_3x3_Vertical
    hr = m_pEffect->SetTexture( "g_texLastOutput" , m_pRTTTextureBloom_DownSize_3x3_Vertical );
    //
    //hr = m_pEffect->SetTechnique( "LastOutPut_Tech" );
    
    //
    hr = m_pEffect->Begin( &cPasses, 0 );
    for( iPass = 0; iPass < cPasses; iPass++ )
      {
        hr = m_pEffect->BeginPass( iPass );
        hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP , 0, 0, m_dwNumVertices, 0, 2 );
        hr = m_pEffect->EndPass( );
      }
    hr = m_pEffect->End();
  }

void CFULLWINDOW::OnLostDevice()
  {
    if( m_pEffect )     m_pEffect->OnLostDevice();
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pVB );
    //
    SAFE_RELEASE( m_pRTTTexture_Scene );
    SAFE_RELEASE( m_pRTTInterface_Scene );
    SAFE_RELEASE( m_pRTTSurface_Scene );
    //
    SAFE_RELEASE( m_pRTTTextureBloom_DownSize_3x3 );
    SAFE_RELEASE( m_pRTTInterfaceBloom_DownSize_3x3 );
    SAFE_RELEASE( m_pRTTSurfaceBloom_DownSize_3x3 );
    //
    SAFE_RELEASE( m_pRTTTextureBloom_DownSize_3x3_Horizon );
    SAFE_RELEASE( m_pRTTInterfaceBloom_DownSize_3x3_Horizon );
    SAFE_RELEASE( m_pRTTSurfaceBloom_DownSize_3x3_Horizon );
    // 
    SAFE_RELEASE( m_pRTTTextureBloom_DownSize_3x3_Vertical );
    SAFE_RELEASE( m_pRTTInterfaceBloom_DownSize_3x3_Vertical );
    SAFE_RELEASE( m_pRTTSurfaceBloom_DownSize_3x3_Vertical );
    // 
  }

void CFULLWINDOW::OnDestroyDevice()
  {
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pDecl );
    //
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pColorTexture );
    //
    SAFE_RELEASE( m_pRTTTexture_Scene );
    SAFE_RELEASE( m_pRTTInterface_Scene );
    SAFE_RELEASE( m_pRTTSurface_Scene );
    //
    SAFE_RELEASE( m_pRTTTextureBloom_DownSize_3x3 );
    SAFE_RELEASE( m_pRTTInterfaceBloom_DownSize_3x3 );
    SAFE_RELEASE( m_pRTTSurfaceBloom_DownSize_3x3 );
    // 
    SAFE_RELEASE( m_pRTTTextureBloom_DownSize_3x3_Horizon );
    SAFE_RELEASE( m_pRTTInterfaceBloom_DownSize_3x3_Horizon );
    SAFE_RELEASE( m_pRTTSurfaceBloom_DownSize_3x3_Horizon );
    // 
    SAFE_RELEASE( m_pRTTTextureBloom_DownSize_3x3_Vertical );
    SAFE_RELEASE( m_pRTTInterfaceBloom_DownSize_3x3_Vertical );
    SAFE_RELEASE( m_pRTTSurfaceBloom_DownSize_3x3_Vertical );
    //
  }


//----------------------------------------------------------------------------
// ������CreateSquare
// ���ܣ�����ģ��
//----------------------------------------------------------------------------
HRESULT CFULLWINDOW::CreateSquare( LPDIRECT3DDEVICE9 pd3dDevice )
  {
    HRESULT hr      = S_OK;
    m_dwNumIndices  = 4;        // ���������ι�4����
    m_dwNumVertices = 4;                            // 4  
    // ---------------------------- �������������� ---------------------------
      {
        WORD* pIndices;
        if( FAILED( hr = pd3dDevice->CreateIndexBuffer( m_dwNumIndices * sizeof( WORD ),
          0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL )))     return hr;
        if( FAILED( hr = m_pIB->Lock( 0, 0, ( void** )&pIndices, 0 )))     return hr;
        // 
        pIndices[ 0 ] = 0;
        pIndices[ 1 ] = 1;
        pIndices[ 2 ] = 2;
        pIndices[ 3 ] = 3;
        if( FAILED( hr = m_pIB->Unlock( )))  return hr;  // д��϶�Ҫ����
      }
    // ---------------------------- �������㻺���� ---------------------------
      {
        BOARD_D3DVERTEX *pVertices;
        // 
        if( FAILED( hr = pd3dDevice->CreateVertexBuffer( m_dwNumVertices * sizeof( BOARD_D3DVERTEX ),
          0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL )))          return hr;
        if( FAILED( hr = m_pVB->Lock( 0, 0, ( void** )&pVertices, 0 )))       return hr;
        // �� X Z ƽ��
        // ��
        pVertices[ 0 ].x = -10;
        pVertices[ 0 ].y =  10;
        pVertices[ 0 ].z =  0;
        pVertices[ 0 ].w =  1;
        pVertices[ 0 ].color =  D3DCOLOR_ARGB( 0x70 ,0xFF ,0 ,0 );  // ͸������
        pVertices[ 0 ].u =  0;
        pVertices[ 0 ].v =  0;
        // ��
        pVertices[ 1 ].x =  10;
        pVertices[ 1 ].y =  10;
        pVertices[ 1 ].z =  0;
        pVertices[ 1 ].w =  1;
        pVertices[ 1 ].color =  D3DCOLOR_ARGB( 0x70 ,0x0 ,0xFF ,0 );  // ͸������
        pVertices[ 1 ].u =  1;
        pVertices[ 1 ].v =  0;
        // ��
        pVertices[ 2 ].x = -10;
        pVertices[ 2 ].y = -10;
        pVertices[ 2 ].z = 0;
        pVertices[ 2 ].w =  1;
        pVertices[ 2 ].color =  D3DCOLOR_ARGB( 0x70 ,0x0 ,0 ,0xFF );
        pVertices[ 2 ].u =  0;
        pVertices[ 2 ].v =  1;
        // ��
        pVertices[ 3 ].x =  10;
        pVertices[ 3 ].y = -10;
        pVertices[ 3 ].z =  0;
        pVertices[ 3 ].w =  1;
        pVertices[ 3 ].color =  D3DCOLOR_ARGB( 0x70 ,0x0FF ,0xFF ,0 );
        pVertices[ 3 ].u =  1;
        pVertices[ 3 ].v =  1;
        //
        if( FAILED( hr = m_pVB->Unlock( )))      return hr;
      }
    return hr;
  }

//----------------------------------------------------------------------------
// ������AddToRenderList
// ���ܣ�����Ⱦģ�ͼ��뵽�б���
//----------------------------------------------------------------------------
void CFULLWINDOW::AddToRenderList( void* pCRenderObject , RENDEROBJECTTYPE eRenderType )
  {
    m_pCRenderObject[ m_iCount ].lpVoid = pCRenderObject;
    m_pCRenderObject[ m_iCount ].eRenderObjectType = eRenderType;
    m_iCount = m_iCount + 1;
  }

//----------------------------------------------------------------------------
// ������OnFrameRenderToTexture
// ���ܣ���Ⱦģ�͵�����
//----------------------------------------------------------------------------
void CFULLWINDOW::OnFrameRenderToTexture( IDirect3DDevice9* pd3dDevice ,D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime )
  {
    HRESULT hr;
    hr = m_pEffect->SetTechnique( "Default" );
    if( SUCCEEDED( hr = m_pRTTInterface_Scene->BeginScene( m_pRTTSurface_Scene , NULL )))
      {
        hr  = pd3dDevice->Clear( 0, NULL , D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
        for( int i = 0 ; i < m_iCount ; i ++ )
          {
            switch( m_pCRenderObject[ i ].eRenderObjectType )
              {
                case ENU_RENDEROBJECT:
                  (( CRENDEROBJECT* )( m_pCRenderObject[ i ].lpVoid ))->OnFrameMove( pWorld , pView , pProject , fTime );
                  (( CRENDEROBJECT* )( m_pCRenderObject[ i ].lpVoid ))->OnFrameRender( pd3dDevice );
                  break;
                case ENU_CUBEOBJECT:
                  (( CRENDEROBJECTCUBE* )( m_pCRenderObject[ i ].lpVoid ))->OnFrameMove( pWorld , pView , pProject , fTime );
                  (( CRENDEROBJECTCUBE* )( m_pCRenderObject[ i ].lpVoid ))->OnFrameRender( pd3dDevice );
                  break;
              }
          }
        hr = m_pRTTInterface_Scene->EndScene( 0 );
      }
  }

//----------------------------------------------------------------------------
// ������RenderBloom
// ���ܣ���С������ȡ����������ֵ�����ص�
// ˵����
//----------------------------------------------------------------------------
void CFULLWINDOW::RenderBloom( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr;
    //-------------------------------------------------------
    // ��С��������������ǳ������� m_pRTTTexture_Scene�����������С������ m_pRTTTextureBloom_DownSize_3x3
    //-------------------------------------------------------
    hr = m_pEffect->SetTexture( "g_texScene" , m_pRTTTexture_Scene );
    hr = m_pEffect->SetTechnique( "DownSizeBrightSet_Tech" );
    //
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( BOARD_D3DVERTEX )); 
    if( SUCCEEDED( hr = m_pRTTInterfaceBloom_DownSize_3x3->BeginScene( m_pRTTSurfaceBloom_DownSize_3x3 , NULL )))
      {
        hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
        hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( BOARD_D3DVERTEX )); 
        hr = pd3dDevice->SetIndices( m_pIB ); 
        UINT iPass, cPasses;
        hr = pd3dDevice->Clear( 0, NULL , D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
          {
            hr = m_pEffect->Begin( &cPasses, 0 );
            for( iPass = 0; iPass < cPasses; iPass++ )
              {
                hr = m_pEffect->BeginPass( iPass );
                hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP , 0, 0, m_dwNumVertices, 0, 2 );
                hr = m_pEffect->EndPass( );
              }
            hr = m_pEffect->End();
          }
      }
    hr = m_pRTTInterfaceBloom_DownSize_3x3->EndScene( 0 );
    //-------------------------------------------------------
    // ˮƽģ�������������С�ĳ������� m_pRTTTextureBloom_DownSize_3x3 ���������ˮƽ����ģ�������� m_pRTTTextureBloom_DownSize_3x3_Horizon
    //-------------------------------------------------------
    hr = m_pEffect->SetTexture( "g_texTextureBloom_DownSize_3x3" , m_pRTTTextureBloom_DownSize_3x3 );
    hr = m_pEffect->SetTechnique( "HBloom_Tech" );
    //
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( BOARD_D3DVERTEX )); 
    if( SUCCEEDED( hr = m_pRTTInterfaceBloom_DownSize_3x3_Horizon->BeginScene( m_pRTTSurfaceBloom_DownSize_3x3_Horizon , NULL )))
      {
        hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
        hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( BOARD_D3DVERTEX )); 
        hr = pd3dDevice->SetIndices( m_pIB ); 
        UINT iPass, cPasses;
        hr = pd3dDevice->Clear( 0, NULL , D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
          {
            hr = m_pEffect->Begin( &cPasses, 0 );
            for( iPass = 0; iPass < cPasses; iPass++ )
              {
                hr = m_pEffect->BeginPass( iPass );
                hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP , 0, 0, m_dwNumVertices, 0, 2 );
                hr = m_pEffect->EndPass( );
              }
            hr = m_pEffect->End();
          }
      }
    hr = m_pRTTInterfaceBloom_DownSize_3x3_Horizon->EndScene( 0 );
    //-------------------------------------------------------
    // ��ֱģ�������������С��ˮƽģ������ m_pRTTTextureBloom_DownSize_3x3_Horizon ��������Ǵ�ֱ����ģ�������� m_pRTTTextureBloom_DownSize_3x3_Vertical
    //-------------------------------------------------------
    hr = m_pEffect->SetTexture( "g_texTextureBloom_DownSize_3x3_H" , m_pRTTTextureBloom_DownSize_3x3_Horizon );
    hr = m_pEffect->SetTechnique( "VBloom_Tech" );
    // 
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( BOARD_D3DVERTEX )); 
    if( SUCCEEDED( hr = m_pRTTInterfaceBloom_DownSize_3x3_Vertical->BeginScene( m_pRTTSurfaceBloom_DownSize_3x3_Vertical , NULL )))
      {
        hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
        hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( BOARD_D3DVERTEX )); 
        hr = pd3dDevice->SetIndices( m_pIB ); 
        UINT iPass, cPasses;
        hr = pd3dDevice->Clear( 0, NULL , D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
          {
            hr = m_pEffect->Begin( &cPasses, 0 );
            for( iPass = 0; iPass < cPasses; iPass++ )
              {
                hr = m_pEffect->BeginPass( iPass );
                hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP , 0, 0, m_dwNumVertices, 0, 2 );
                hr = m_pEffect->EndPass( );
              }
            hr = m_pEffect->End();
          }
      }
    hr = m_pRTTInterfaceBloom_DownSize_3x3_Vertical->EndScene( 0 );
  }

