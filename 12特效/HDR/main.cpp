
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "resource.h"
#include "CRenderObject.h"
#include "CRenderObjectCube.h"
#include "CFullWindow.h"
//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont         = NULL; // 字体
ID3DXSprite*            g_pSprite       = NULL; // 显示字体用
CDXUTDialog             g_HUD;                  // 与用户的显示接口

D3DXMATRIXA16 g_matWorld;
D3DXMATRIXA16 g_matView;
D3DXMATRIXA16 g_matProject;

CRENDEROBJECT           g_RenderObject_Ground;         // 显示的物体
CRENDEROBJECTCUBE       g_RenderObject_Box;            // 天空立方体
CFULLWINDOW             g_RenderObject_FullWindow;     // 全屏特效
//--------------------------------------------------------------------------------------
// 与用户交互的控制 ID
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4

//--------------------------------------------------------------------------------------
// 函数声明
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed );
void    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing );
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown  );
void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl );
void    CALLBACK OnLostDevice();
void    CALLBACK OnDestroyDevice();

void    InitApp();
void    RenderText( double fTime );

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
  {
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );
    // 
    DXUTSetCursorSettings( true, true );
    InitApp();
    DXUTInit( true, true, true ); 
    DXUTCreateWindow( L"EmptyProject_HDR_1" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 410, IsDeviceAcceptable, ModifyDeviceSettings );
    DXUTMainLoop();
    return DXUTGetExitCode();
  }

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
  {
    g_HUD.SetCallback( OnGUIEvent ); 
    int iY = 10; 
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );
    //
    g_RenderObject_FullWindow.Init( );
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed )
  {
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,1) )   return false;
    // 
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
      AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
      D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
      return false;
    return true;
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps )
  {
    if(( pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
      pCaps->VertexShaderVersion < D3DVS_VERSION( 1,1) )
      {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
      }
    else
      {
        pDeviceSettings->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
      }
    if(( pCaps->DevCaps & D3DDEVCAPS_PUREDEVICE ) != 0 && 
      (pDeviceSettings->BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 )
      pDeviceSettings->BehaviorFlags |= D3DCREATE_PUREDEVICE;
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
  {
    HRESULT hr;
    // 
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_THIN, 1, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"宋体", &g_pFont ) );
    // 装入模型
    TCHAR szAppPath[ MAX_PATH ];
    TCHAR szRcPath[ MAX_PATH ];
    GetCurrentDirectory( MAX_PATH, szAppPath );
    // 
    lstrcpy( szRcPath , szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_1_Ground" );
    SetCurrentDirectory( szRcPath );
    TCHAR* szFile_1 = L"ShmFile.shm";
    if( FAILED( hr = g_RenderObject_Ground.OnCreateDevice( pd3dDevice ,szFile_1 ))) return hr;
    // 
    lstrcpy( szRcPath , szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_2_Cube" );
    SetCurrentDirectory( szRcPath );
    TCHAR* szFile_2 = L"ShmFile.shm";
    if( FAILED( hr = g_RenderObject_Box.OnCreateDevice( pd3dDevice ,szFile_2 ))) return hr;
    // 
    lstrcpy( szRcPath , szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_FullWindow" );
    SetCurrentDirectory( szRcPath );
    if( FAILED( hr = g_RenderObject_FullWindow.OnCreateDevice( pd3dDevice ))) return hr;
    g_RenderObject_FullWindow.AddToRenderList(( void* )&g_RenderObject_Ground , ENU_RENDEROBJECT );
    g_RenderObject_FullWindow.AddToRenderList(( void* )&g_RenderObject_Box , ENU_CUBEOBJECT );
    // 
    return S_OK;
  }


//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
  {
    HRESULT hr;
    if( g_pFont )        V_RETURN( g_pFont->OnResetDevice() );
    // 
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pSprite ) );
    // 
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    D3DXMatrixPerspectiveFovLH( &g_matProject ,D3DX_PI/4 ,fAspectRatio ,10.0 ,100.0 );
    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    // 
    g_RenderObject_Ground.OnResetDevice( );
    g_RenderObject_Box.OnResetDevice( );
    g_RenderObject_FullWindow.OnResetDevice( pd3dDevice , pBackBufferSurfaceDesc->Width  , pBackBufferSurfaceDesc->Height );
    // 
    return S_OK;
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
  {
    // 物体绕 Y 轴旋转
    D3DXMatrixRotationY( &g_matWorld ,fTime );
    //D3DXMatrixIdentity( &g_matWorld );
    // 观察矩阵
    D3DXVECTOR3 vEyePt( 30  , 7 , 0 );
    D3DXVECTOR3 vLookatPt( 0, 0, 0 );
    D3DXVECTOR3 vUpVec( 0, 1, 0 );
    D3DXMatrixLookAtLH( &g_matView, &vEyePt, &vLookatPt, &vUpVec );
    //  观察位置
    g_RenderObject_Ground.OnFrameMove( &g_matWorld ,&g_matView ,&g_matProject ,fTime );
    g_RenderObject_Box.OnFrameMove( &g_matWorld ,&g_matView ,&g_matProject ,fTime );
    D3DXVECTOR4 v4 = D3DXVECTOR4( 0.0 , 0.0 , 0.0 , 1.0 );
    g_RenderObject_FullWindow.OnFrameMove( fTime , v4 );
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
  {
    HRESULT hr;
    // 将场景渲染到纹理
    D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"RenderToTexture" );
      {
        g_RenderObject_FullWindow.OnFrameRenderToTexture( pd3dDevice , &g_matWorld , &g_matView , &g_matProject , fTime );
      }
    D3DPERF_EndEvent( );
    // 将场景纹理渲染到 Bloom 纹理
    D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"RenderBloom" );
      {
        g_RenderObject_FullWindow.RenderBloom( pd3dDevice );
      }
    D3DPERF_EndEvent( );
    //
    // 
    V( hr = pd3dDevice->Clear( 0 , NULL , D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DXCOLOR( 0.0f , 0.25f , 0.25f , 0.55f ) , 1.0f , 0 ));
    // 渲染场景 
    if( SUCCEEDED( hr = pd3dDevice->BeginScene() ) )
      {
        // 渲染场景
        D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"SceneObject" );
          {
            g_RenderObject_Ground.OnFrameRender( pd3dDevice );
            g_RenderObject_Box.OnFrameRender( pd3dDevice );
          }
        D3DPERF_EndEvent( );
        // Bloom效果
        D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"DisplayTwoTexture" );
          {
            g_RenderObject_FullWindow.OnFrameRender( pd3dDevice );
          }
        D3DPERF_EndEvent( );
        // 
        D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"Display UI" );
          {
            g_HUD.OnRender( fElapsedTime ); 
            RenderText( fTime );
          }
        D3DPERF_EndEvent( );
        
        V( pd3dDevice->EndScene() );
      }
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void RenderText( double fTime )
  {
    CDXUTTextHelper txtHelper( g_pFont, g_pSprite, 15 );
    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 2, 0 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats() );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );
    
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
    
    txtHelper.DrawTextLine( L"显示需要的信息" );

    txtHelper.End();
  }


//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing )
  {
    // 
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )        return 0;
    // 
    return 0;
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown )
  {
    if( bKeyDown )
      {
        switch( nChar )
          {
            case VK_F1: 
              break;
          }
      }
  }

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl )
  {
    
    switch( nControlID )
      {
        case IDC_TOGGLEFULLSCREEN: 
          DXUTToggleFullScreen(); 
          break;
        case IDC_TOGGLEREF:        
          DXUTToggleREF(); 
          break;
        case IDC_CHANGEDEVICE:     
          DXUTSetShowSettingsDialog( !DXUTGetShowSettingsDialog() ); 
          break;
      }
    
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice()
  {
    CDXUTDirectionWidget::StaticOnLostDevice();
    if( g_pFont )        g_pFont->OnLostDevice();
    SAFE_RELEASE(g_pSprite);
    g_RenderObject_Ground.OnLostDevice( );
    g_RenderObject_Box.OnLostDevice( );
    g_RenderObject_FullWindow.OnLostDevice( );
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
  {
    CDXUTDirectionWidget::StaticOnDestroyDevice();
    SAFE_RELEASE(g_pFont);
    g_RenderObject_Ground.OnDestroyDevice( );
    g_RenderObject_Box.OnDestroyDevice( );
    g_RenderObject_FullWindow.OnDestroyDevice( );
  }

