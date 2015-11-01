//-----------------------------------------------------------------------------
// 文件：CBOARD.Cpp 
// 功能：Board 的类实现
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "CBoard.h"

CBOARD::CBOARD( )
  {
    m_pVB            = NULL;
    m_pIB            = NULL;
    m_dwNumIndices   = 0;
    m_dwNumVertices  = 0;
    m_pDecl          = NULL;
    m_pEffect        = NULL;
  }

CBOARD::~CBOARD( )
  {
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    m_dwNumIndices   = 0;
    m_dwNumVertices  = 0;
    SAFE_RELEASE( m_pDecl );
    SAFE_RELEASE( m_pEffect );
    
  }
//
void CBOARD::Init( )
  {
  }

HRESULT CBOARD::OnCreateDevice( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr = S_OK;
    TCHAR szAppPath[MAX_PATH];
    GetCurrentDirectory( MAX_PATH, szAppPath );
    // 装入纹理
    if( FAILED( hr = D3DXCreateTextureFromFileEx( pd3dDevice,  L"ColorMap.bmp", D3DX_DEFAULT, D3DX_DEFAULT, 
      D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 
      NULL, NULL, &m_pColorTexture ) )) return hr;  
    // ------------------------ 以新的方式创建顶点流 -------------------------------
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
    if( FAILED( hr = D3DXCreateEffectFromFile( pd3dDevice, L"FileFX.fx", NULL, NULL, 
      0, NULL, &m_pEffect, &pCode )))    
      {
        LPVOID pBufErr = pCode->GetBufferPointer();
        char* ErrInfo = ( char* ) pBufErr;
        pCode->Release( );
        return hr;
      }
    return hr;
  }

HRESULT CBOARD::OnResetDevice( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr = S_OK;
    if( FAILED( hr = CreateSquare( pd3dDevice ))) return hr;
    hr = m_pEffect->SetTexture( "g_texColorTexture" ,m_pColorTexture );
    return hr;
  }

void CBOARD::OnFrameMove( )
  {
  }

void CBOARD::OnFrameRender( IDirect3DDevice9* pd3dDevice ,D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime )
  {
    HRESULT hr;
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( BOARD_D3DVERTEX )); 
    hr = pd3dDevice->SetIndices( m_pIB ); 
    UINT iPass, cPasses;
    // ------------------------- ScreenShader ---------------------------
    hr = m_pEffect->SetTechnique( "Default" );
    //
    //hr = m_pEffect->Begin( &uPasses, D3DXFX_DONOTSAVESTATE );
    hr = m_pEffect->Begin( &cPasses, 0 );
    for( iPass = 0; iPass < cPasses; iPass++ )
      {
        hr = m_pEffect->BeginPass( iPass );
        hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP , 0, 0, m_dwNumVertices, 0, 2 );
        hr = m_pEffect->EndPass( );
      }
    hr = m_pEffect->End();
  }

void CBOARD::OnFrameRender( IDirect3DDevice9* pd3dDevice ,LPDIRECT3DTEXTURE9 pTexture , int i , float fTime )
  {
    HRESULT hr;
    switch( i )
      {
        case 0:
          hr = m_pEffect->SetTechnique( "Tech_0" );
          hr = m_pEffect->SetTexture( "texColorTexture_0" ,pTexture );
          break;
        case 1:
          hr = m_pEffect->SetTechnique( "Tech_1" );
          hr = m_pEffect->SetTexture( "texColorTexture_1" ,pTexture );
          break;
        case 2:
          hr = m_pEffect->SetTechnique( "Tech_2" );
          hr = m_pEffect->SetTexture( "texColorTexture_2" ,pTexture );
          break;
        case 3:
          hr = m_pEffect->SetTechnique( "Tech_3" );
          hr = m_pEffect->SetTexture( "texColorTexture_3" ,pTexture );
          break;
        case 4:
          hr = m_pEffect->SetTechnique( "Tech_4" );
          hr = m_pEffect->SetTexture( "texColorTexture_4" ,pTexture );
          break;
        case 5:
          hr = m_pEffect->SetTechnique( "Tech_5" );
          hr = m_pEffect->SetTexture( "texColorTexture_0" ,pTexture );
          break;
      }
    hr = m_pEffect->SetFloat( "g_fTime" ,fTime );
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( BOARD_D3DVERTEX )); 
    hr = pd3dDevice->SetIndices( m_pIB ); 
    UINT iPass, cPasses;
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

void CBOARD::OnLostDevice()
  {
    if( m_pEffect )     m_pEffect->OnLostDevice();
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pVB );
  }

void CBOARD::OnDestroyDevice()
  {
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pDecl );
    //
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pColorTexture );
  }


//----------------------------------------------------------------------------
// 函数：CreateSquare
// 功能：创建模型
//----------------------------------------------------------------------------
HRESULT CBOARD::CreateSquare( LPDIRECT3DDEVICE9 pd3dDevice )
  {
    HRESULT hr      = S_OK;
    m_dwNumIndices  = 4;        // 两个三角形共4个点
    m_dwNumVertices = 4;                            // 4  
    // ---------------------------- 创建索引缓冲区 ---------------------------
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
        if( FAILED( hr = m_pIB->Unlock( )))  return hr;  // 写完肯定要解锁
      }
    // ---------------------------- 创建顶点缓冲区 ---------------------------
      {
        BOARD_D3DVERTEX *pVertices;
        // 
        if( FAILED( hr = pd3dDevice->CreateVertexBuffer( m_dwNumVertices * sizeof( BOARD_D3DVERTEX ),
          0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL )))          return hr;
        if( FAILED( hr = m_pVB->Lock( 0, 0, ( void** )&pVertices, 0 )))       return hr;
        // 在 X Z 平面
        // 红
        pVertices[ 0 ].x = -10;
        pVertices[ 0 ].y =  10;
        pVertices[ 0 ].z =  0;
        pVertices[ 0 ].w =  1;
        pVertices[ 0 ].color =  D3DCOLOR_ARGB( 0x70 ,0xFF ,0 ,0 );  // 透明顶点
        pVertices[ 0 ].u =  0;
        pVertices[ 0 ].v =  0;
        // 绿
        pVertices[ 1 ].x =  10;
        pVertices[ 1 ].y =  10;
        pVertices[ 1 ].z =  0;
        pVertices[ 1 ].w =  1;
        pVertices[ 1 ].color =  D3DCOLOR_ARGB( 0x70 ,0x0 ,0xFF ,0 );  // 透明顶点
        pVertices[ 1 ].u =  1;
        pVertices[ 1 ].v =  0;
        // 蓝
        pVertices[ 2 ].x = -10;
        pVertices[ 2 ].y = -10;
        pVertices[ 2 ].z = 0;
        pVertices[ 2 ].w =  1;
        pVertices[ 2 ].color =  D3DCOLOR_ARGB( 0x70 ,0x0 ,0 ,0xFF );
        pVertices[ 2 ].u =  0;
        pVertices[ 2 ].v =  1;
        // 白
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

