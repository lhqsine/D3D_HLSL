
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <time.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

//-----------------------------------------------------------------------------
// 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D           = NULL; // D3D 接口
LPDIRECT3DDEVICE9       g_pd3dDevice     = NULL; // 设备接口
LPDIRECT3DVERTEXBUFFER9 g_pVB            = NULL; // 顶点缓冲区接口
LPD3DXEFFECT            g_pEffect        = NULL; // 效果接口

struct CUSTOMVERTEX
  {
    FLOAT x, y, z, w; // The transformed position for the vertex
    DWORD color;        // The vertex color
  };
#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZW | D3DFVF_DIFFUSE )

HRESULT InitD3D( HWND hWnd )
{
	    // 创建 3D 对象.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) 
		return E_FAIL;

	// 告诉系统设置需要创建的参数，就是填充 D3DPRESENT_PARAMETERS 的参数
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof( d3dpp ));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	 // 创建设备
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
      D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice ) ) )  return E_FAIL;
    return S_OK;

}

//-----------------------------------------------------------------------------
// 函数：InitVB
// 功能：初始化 g_pVB
// 参数：无
//-----------------------------------------------------------------------------
HRESULT InitVB()
  {
    HRESULT hr;
    CUSTOMVERTEX vertices[] =
      {
        {  5.0f ,  0.0f , 0.0f , 1.0f , 0xffff0000  }, // x, y, z, rhw, color 红色
        {  0.0f  , 5.0f , 0.0f , 1.0f , 0xff00ff00  }, // 绿色
        { -5.0f ,  0.0f , 0.0f , 1.0f , 0xff0000ff  }, // 蓝色
        
      };
    if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( 3 * sizeof( CUSTOMVERTEX ),
       0 , D3DFVF_CUSTOMVERTEX , D3DPOOL_DEFAULT , &g_pVB, NULL ))) return hr;
    VOID* pVertices;
    if( FAILED( hr = g_pVB->Lock( 0, sizeof( vertices ), ( void** )&pVertices, 0 )))    return hr;
    memcpy( pVertices, vertices, sizeof( vertices ));
    g_pVB->Unlock( );

    return S_OK;
  }

//-----------------------------------------------------------------------------
// 函数：InitGeometry()
// 功能：设置目录、装入模型、纹理、效果文件、传递效果参数到HLSL
// 参数：初始化了其他三个全局变量 g_pMesh g_pEffect g_pMeshTextures
//-----------------------------------------------------------------------------

HRESULT InitGeometry()
  {
    HRESULT hr;
    // 设置目录
    TCHAR szAppPath[ MAX_PATH ];
    TCHAR szRcPath[ MAX_PATH ];
    GetCurrentDirectory( MAX_PATH, szAppPath );
    // 
    lstrcpy( szRcPath , szAppPath );
    lstrcat( szRcPath ,"\\ResFile_1_Model" );
    SetCurrentDirectory( szRcPath );
        // 装入效果文件
    LPD3DXBUFFER pCode = NULL;
    if( FAILED( hr = D3DXCreateEffectFromFile( g_pd3dDevice, "FileFX.fx", 
      NULL, NULL, 0, NULL, &g_pEffect, &pCode )))
      {
        LPVOID pBufErr = pCode->GetBufferPointer();
        char* ErrInfo = ( char* ) pBufErr;
        pCode->Release( );
        return hr;
      }
    if( FAILED( hr = g_pEffect->SetTechnique( "DefaultTech" ))) return hr ;
    if( FAILED( hr = InitVB())) return hr;
    // 可以正确返回了
    return S_OK;
  }

//-----------------------------------------------------------------------------
// 函数：Cleanup()
// 功能：释放所有的接口
// 参数：无
//-----------------------------------------------------------------------------
VOID Cleanup()
  {
    if( NULL != g_pVB )             g_pVB->Release();
    if( NULL != g_pd3dDevice )      g_pd3dDevice->Release();
    if( NULL != g_pD3D )            g_pD3D->Release();
    if( NULL != g_pEffect )         g_pEffect->Release( );
  }


//-----------------------------------------------------------------------------
// 函数：SetupMatrices()
// 功能：将 4 个变量传递给效果文件，由效果文件来计算
// 参数：无，本模块涉及全局变量：效果文件接口
//-----------------------------------------------------------------------------
VOID SetupMatrices()
  {
    HRESULT hr;
    D3DXMATRIXA16 matWorld;
    float fTime = timeGetTime() / 1000.0f ;
    D3DXMatrixRotationY( &matWorld , fTime );
    //D3DXMatrixIdentity( &matWorld );
    hr = g_pEffect->SetFloat( "g_fTime" , fTime );
    // 世界矩阵
    hr = g_pEffect->SetMatrix( "g_matWorld" , &matWorld );
    // 观察矩阵
    D3DXVECTOR3 vEyePt( 15.0f , 3.0f , 0.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 3.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    hr = g_pEffect->SetMatrix( "g_matView" , &matView );
    // 投影矩阵
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj , D3DX_PI/4 , 64.0 / 41.0f , 1.0 , 100.0f );
    hr = g_pEffect->SetMatrix( "g_matProject" , &matProj );
  }


//-----------------------------------------------------------------------------
// 函数：Render()
// 功能：渲染模型
// 参数：无，使用全局变量
//-----------------------------------------------------------------------------
VOID Render()
  {
    HRESULT hr;
    // 清除背景缓冲区的深度缓冲区
    hr = g_pd3dDevice->Clear( 0 , NULL , D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , 
      D3DXCOLOR( 0.0f , 0.25f , 0.25f , 0.55f ) , 1.0f , 0 );
    hr = g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ));
    hr = g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    if( SUCCEEDED( g_pd3dDevice->BeginScene( )))
      {
        // 设置变换，实际上是设置实际矩阵绕 Y 轴旋转
        SetupMatrices( );
        // 按效果文件的设置渲染
        UINT iPass, cPasses;
        hr = g_pEffect->Begin( &cPasses, 0 );
        for ( iPass = 0; iPass < cPasses; iPass++ )
          {
            hr = g_pEffect->BeginPass( iPass );
            hr = g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , 0 , 1 );
            hr = g_pEffect->EndPass();
          }
        hr = g_pEffect->End();
        hr = g_pd3dDevice->EndScene();
      }
    // 将渲染的结果切换到前景缓冲区
    hr = g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
  }

//-----------------------------------------------------------------------------
// 函数：MsgProc()
// 功能：消息循环的处理
// 参数：无，是 Window 窗口的一部分
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
  {
    switch( msg )
      {
        case WM_DESTROY:
          Cleanup();
          PostQuitMessage( 0 );
          return 0;
      }
    return DefWindowProc( hWnd, msg, wParam, lParam );
  }

//-----------------------------------------------------------------------------
// 函数：WinMain()
// 功能：应用程序入口点
// 参数：实例句柄，命令行参数
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst , HINSTANCE , LPSTR , INT )
  {
    // 定义下窗口类和窗口标题
    #define CLASSNAME "EmptyProject"
    #define TITILNAME "EmptyProject_1"
    WNDCLASSEX wc =   { sizeof( WNDCLASSEX ) , CS_CLASSDC , MsgProc , 0L , 0L , 
      GetModuleHandle( NULL ) , NULL , LoadCursor( NULL, IDC_ARROW ) , NULL , NULL ,
      CLASSNAME , NULL };
    RegisterClassEx( &wc );
    // 创建窗口
    HWND hWnd = CreateWindow( CLASSNAME , TITILNAME , WS_OVERLAPPEDWINDOW, 100 , 100 , 640, 410,
      GetDesktopWindow() , NULL , wc.hInstance , NULL );
    // 初始化 D3D 和 3D 设备
    if( SUCCEEDED( InitD3D( hWnd ) ) )
      { 
        // 装载模型
        if( SUCCEEDED( InitGeometry() ) )
          {
            // 显示窗口
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );
            // 进入消息循环
            MSG msg; 
            ZeroMemory( &msg, sizeof(msg) );
            while( msg.message!=WM_QUIT )
              {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                  {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                  }
                else
                  Render();
              }
          }
      }
    UnregisterClass( CLASSNAME , wc.hInstance );
    return 0;
  }


