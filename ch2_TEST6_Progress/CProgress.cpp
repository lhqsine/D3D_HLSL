#include "dxstdafx.h"
#include "Cprogress.h"

CPROGRESS::CPROGRESS()
{
    m_pVB            = NULL;
    m_pIB            = NULL;
    m_dwNumIndices   = 0;
    m_dwNumVertices  = 0;
    m_pDecl          = NULL;
    m_pEffect        = NULL;
}

CPROGRESS::~CPROGRESS( )
  {
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    m_dwNumIndices   = 0;
    m_dwNumVertices  = 0;
    SAFE_RELEASE( m_pDecl );
    SAFE_RELEASE( m_pEffect );
  }

//
void CPROGRESS::Init( )
  {
  }
//

HRESULT CPROGRESS::OnCreateDevice( IDirect3DDevice9* pd3dDevice )
{
	HRESULT hr = S_OK;

	TCHAR szAppPath[MAX_PATH];
	TCHAR szRcPath[MAX_PATH];
    GetCurrentDirectory( MAX_PATH, szAppPath );

	 lstrcpy( szRcPath , szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_Progress" );
    SetCurrentDirectory( szRcPath );
    GetCurrentDirectory( MAX_PATH, szAppPath );

	 // װ������
    if( FAILED( hr = D3DXCreateTextureFromFileEx( pd3dDevice,  L"ColorMap.dds", D3DX_DEFAULT, D3DX_DEFAULT, 
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

HRESULT CPROGRESS::OnResetDevice( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr = S_OK;
    if( FAILED( hr = CreateSquare( pd3dDevice ))) return hr;
    hr = m_pEffect->SetTexture( "texColorTexture" ,m_pColorTexture );
    return hr;
  }

void CPROGRESS::OnFrameMove( )
  {
  }



void CPROGRESS::OnFrameRender( IDirect3DDevice9* pd3dDevice ,float fProgress)
  {
    HRESULT hr;
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( BOARD_D3DVERTEX )); 
    hr = pd3dDevice->SetIndices( m_pIB ); 
	hr = m_pEffect->SetFloat( "g_fProgress" ,fProgress );
    UINT iPass, cPasses;
	// ------------------------- ScreenShader ---------------------------
    hr = m_pEffect->SetTechnique( "Default" );
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

void CPROGRESS::OnLostDevice()
  {
    if( m_pEffect )     m_pEffect->OnLostDevice();
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pVB );
  }

void CPROGRESS::OnDestroyDevice()
  {
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pDecl );
    //
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pColorTexture );
  }

HRESULT CPROGRESS::CreateSquare( LPDIRECT3DDEVICE9 pd3dDevice )
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
		  if( FAILED ( hr = pd3dDevice->CreateVertexBuffer( m_dwNumVertices * sizeof(BOARD_D3DVERTEX),
			  0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL))) return hr;

		  if( FAILED( hr = m_pVB->Lock( 0, 0, ( void** )&pVertices, 0 )))       return hr;

		  // �� X Zƽ��
		  // ��
		  pVertices[ 0 ].x = -10;
		  pVertices[ 0 ].y = 10;
		  pVertices[ 0 ].z = 0;
		  pVertices[ 0 ].w = 1;
		  pVertices[ 0 ].color = D3DCOLOR_ARGB( 0x70, 0xFF, 0, 0); //͸������
		  pVertices[ 0 ].u = 0;
		  pVertices[ 0 ].v = 0;

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

