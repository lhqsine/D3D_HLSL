#include "CBuffer.h"

//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
CBUFFER::CBUFFER( )
  {
    m_pD3D           = NULL;
    m_pd3dDevice     = NULL;
    m_pMesh          = NULL;
    m_pVB            = NULL;
    m_pIB            = NULL;
    m_pVertex        = NULL;
    // 
    m_pdwAdj         = NULL;
    m_pdwPtRep       = NULL;
    // 
    m_pIndex16       = NULL;
    m_pIndex32       = NULL;
    // 
    m_pBuffer        = NULL;
    m_pAdjIndex16    = NULL;
    m_pAdjIndex32    = NULL;
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
CBUFFER::~CBUFFER( )
  {
    SAFE_RELEASE( m_pD3D );
    SAFE_RELEASE( m_pd3dDevice );
    SAFE_RELEASE( m_pMesh );
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_DELETE_ARRAY( m_pVertex );
    // 
    SAFE_DELETE_ARRAY( m_pdwAdj );
    SAFE_DELETE_ARRAY( m_pdwPtRep );
    // 
    SAFE_DELETE_ARRAY( m_pIndex16 );
    SAFE_DELETE_ARRAY( m_pIndex32 );
    //
    SAFE_RELEASE( m_pBuffer );
    SAFE_DELETE_ARRAY( m_pAdjIndex16 );
    SAFE_DELETE_ARRAY( m_pAdjIndex32 );

  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
HRESULT CBUFFER::LoadModel( LPCWSTR pFileName )
  {
    HRESULT hr;
    // ��ȡ���ھ��
    HWND hWnd = FindWindow( L"ConsoleWindowClass" , NULL );
    if( NULL == hWnd ) return E_FAIL;
    // ��ʼ�� D3D �ӿ�
    // ���� 3D ����.
    if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )       return E_FAIL;
    // ����ϵͳ������Ҫ�����Ĳ������������ D3DPRESENT_PARAMETERS �Ĳ���
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof( d3dpp ));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    // �����豸
    if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
      D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pd3dDevice ) ) )  return E_FAIL;
    // װ��ģ��
    if( FAILED( hr = D3DXLoadMeshFromX( pFileName , D3DXMESH_SYSTEMMEM, m_pd3dDevice , &m_pBuffer , NULL , NULL , NULL ,  &m_pMesh ))) 
      {
        Cleanup( );
        return E_FAIL;
      }
    DWORD dwBufSize = m_pBuffer->GetBufferSize( );
    //
    if( FAILED( hr = m_pMesh->GetVertexBuffer( &m_pVB ))) 
      {
        Cleanup( );
        return E_FAIL;
      }
    if( FAILED( hr = m_pMesh->GetIndexBuffer( &m_pIB ))) 
      {
        Cleanup( );
        return E_FAIL;
      }
    m_dwFaceNum   = m_pMesh->GetNumFaces( );
    m_dwVertexNum = m_pMesh->GetNumVertices( );
    //----------------------------
    // ���涥�㻺�����е�����
    if( NULL == ( m_pVertex = new D3DXVECTOR3[ m_dwVertexNum ] )) 
      {
        Cleanup( );
        return E_FAIL;
      }
    D3DXVECTOR3*  pVerticesLock;
    if( SUCCEEDED( hr = m_pVB->Lock( 0 , 0 , ( void** )&pVerticesLock , 0 )))
      {       
        memcpy( m_pVertex , pVerticesLock , sizeof( D3DXVECTOR3 ) * m_dwVertexNum );
        hr = m_pVB->Unlock( );
      }
    // 
    D3DINDEXBUFFER_DESC Desc;
    m_pIB->GetDesc( &Desc );
    switch( Desc.Format )
      {
        case D3DFMT_INDEX16:
          {
            //--------------------------------
            // ȡ����������Ϣ
            //--------------------------------
            if( NULL == ( m_pIndex16 = new INDEX16[ m_dwFaceNum ] ))
              {
                Cleanup( );
                return E_FAIL;
              }
            INDEX16*    pIndiceLock16 ;
            if( FAILED( hr = m_pIB->Lock( 0 , 0 , ( void** )&pIndiceLock16 , 0 )))
              {
                Cleanup( );
                return E_FAIL;
              }
            memcpy( m_pIndex16 , pIndiceLock16 , sizeof( INDEX16 ) * m_dwFaceNum );
            hr = m_pIB->Unlock( );
            //-----------------------------------
            // ȡ�ڽ���Ϣ
            //-----------------------------------
            DWORD dwSize = m_pBuffer->GetBufferSize( ) / 2 / 3;
            if( NULL == ( m_pAdjIndex16 = new INDEX16[ dwSize ]))
              {
                Cleanup( );
                return E_FAIL;
              }
            memcpy( m_pAdjIndex16 , m_pBuffer->GetBufferPointer( ) , m_pBuffer->GetBufferSize( ));
            //-----------------------------------
            // �������
            //-----------------------------------
            if( NULL == ( m_pdwAdj = new DWORD[ 3 * m_dwFaceNum ])) 
              {
                Cleanup( );
                return E_FAIL;
              }
            if( NULL == ( m_pdwPtRep = new DWORD[ m_dwVertexNum ]))
              {
                Cleanup( );
                return E_FAIL;
              }
            if( FAILED( hr = m_pMesh->GenerateAdjacency( ADJACENCY_EPSILON , m_pdwAdj )))
              {
                Cleanup( );
                return E_FAIL;
              }
            if( FAILED( hr = m_pMesh->ConvertAdjacencyToPointReps( m_pdwAdj , m_pdwPtRep )))
              {
                Cleanup( );
                return E_FAIL;
              }
            //-----------------------------------
            // ʹ����������
            //-----------------------------------
          }
          break;
        case D3DFMT_INDEX32:
          {
            if( NULL == ( m_pIndex32 = new INDEX32 [m_dwFaceNum ] ))
              {
                Cleanup( );
                return E_FAIL;
              }
            INDEX16*    pIndiceLock32 ;
            if( SUCCEEDED( hr = m_pIB->Lock( 0 , 0 , ( void** )&pIndiceLock32 , 0 )))
              {
                Cleanup( );
                return E_FAIL;
              }
            memcpy( m_pIndex32 , pIndiceLock32 , sizeof( INDEX32 ) * m_dwFaceNum );
            hr = m_pIB->Unlock( );
          }
          break;
        default:
          {
            Cleanup( );
            return E_FAIL;
          }
          break;
      }
    return hr;
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
DWORD CBUFFER::GetVertexNum( )
  {
    return m_dwVertexNum;
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
DWORD CBUFFER::GetFaceNum( )
  {
    return m_dwFaceNum;
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
D3DXVECTOR3* CBUFFER::GetVertex( )
  {
    return m_pVertex;
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
INDEX16* CBUFFER::GetIndex16( )
  {
    return m_pIndex16;
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
INDEX32* CBUFFER::GetIndex32( )
  {
    return m_pIndex32;
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
void CBUFFER::DisplayVertex( )
  {
    for( DWORD i = 0 ; i < m_dwVertexNum ; i ++ )
      {
        printf( "\n[%d:%d] , %f , %f , %f " , i , m_dwVertexNum , m_pVertex[ i ].x , m_pVertex[ i ].y , m_pVertex[ i ].z  );
      }
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
void CBUFFER::DisplayIndex( )
  {
    if( NULL != m_pIndex16 )
      {
        for( DWORD i = 0 ; i < m_dwFaceNum ; i ++ )
          {
            printf( "\n[%d : %d ] , %d , %d , %d " , i , m_dwFaceNum , m_pIndex16[ i ].V0  , m_pIndex16[ i ].V1 , m_pIndex16[ i ].V2  );
          }
      }
    else if( NULL != m_pIndex32 )
      {
      }
    else
      printf( "\n����������δ֪����" );
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
void CBUFFER::DisplayAdjInfo( )
  {
    if( NULL != m_pIndex16 )
      {
        for( DWORD i = 0 ; i < m_dwFaceNum ; i ++ )
          {
            printf( "\n[%d : %d ] , %d , %d , %d " , i , m_dwFaceNum , m_pAdjIndex16[ i ].V0  , m_pAdjIndex16[ i ].V1 , m_pAdjIndex16[ i ].V2  );
          }
      }
    else if( NULL != m_pIndex32 )
      {
      }
    else
      printf( "\n����������δ֪����" );
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
void CBUFFER::DisplayPointRep( )
  {
    if( NULL != m_pIndex16 )
      {
        for( DWORD i = 0 ; i < m_dwVertexNum ; i ++ )
          {
            printf( "\n[%d]:[%d]" , i , m_pdwPtRep[ i ] );
          }
      }
  }
//-----------------------------------------------------------------------------
// ������
// ���ܣ�
//-----------------------------------------------------------------------------
void CBUFFER::Cleanup( )
  {
    SAFE_RELEASE( m_pD3D );
    SAFE_RELEASE( m_pd3dDevice );
    SAFE_RELEASE( m_pMesh );
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_DELETE_ARRAY( m_pVertex );
    SAFE_DELETE_ARRAY( m_pIndex16 );
    SAFE_DELETE_ARRAY( m_pIndex32 );
    //
    SAFE_RELEASE( m_pBuffer );
    SAFE_DELETE_ARRAY( m_pAdjIndex16 );
    SAFE_DELETE_ARRAY( m_pAdjIndex32 );
  }



