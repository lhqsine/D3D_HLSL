
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "resource.h"
#include "CRenderToCubeMap.h"
#include "CRenderObject.h"
#include "CRenderObjectCube.h"
#include "CBoard.h"
//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont         = NULL; // 字体
ID3DXSprite*            g_pSprite       = NULL; // 显示字体用
CDXUTDialog             g_HUD;                  // 与用户的显示接口

D3DXMATRIXA16 g_matWorld;
D3DXMATRIXA16 g_matView;
D3DXMATRIXA16 g_matProject;

CRENDERTOCUBEMAP        g_RenderToCubeMap;            // 显示立方体纹理的物体
CRENDEROBJECT           g_RenderObject_Ground;        // 地面物体
CRENDEROBJECT           g_RenderObject_MoveObject;    // 移动物体，
CRENDEROBJECTCUBE       g_RenderObject_SkyBox;        // 天空盒

CBOARD                  g_TextureDebug;

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
    DXUTCreateWindow( L"EmptyProject_DXUT_2_RenderToCubeMap" );
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
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
      pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
      {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
      }
    else
      {
        pDeviceSettings->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
      }
    if ((pCaps->DevCaps & D3DDEVCAPS_PUREDEVICE) != 0 && 
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
    lstrcpy( szRcPath ,szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_1_RenderToCubeBox" );
    SetCurrentDirectory( szRcPath );
    TCHAR* szFile_1 = L"ShmFile.shm";
    if( FAILED( hr = g_RenderToCubeMap.OnCreateDevice( pd3dDevice ,szFile_1 ))) return hr;
    // 
    lstrcpy( szRcPath ,szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_2_Ground" );
    SetCurrentDirectory( szRcPath );
    TCHAR* szFile_2 = L"ShmFile.shm";
    if( FAILED( hr = g_RenderObject_Ground.OnCreateDevice( pd3dDevice ,szFile_2 ))) return hr;
    // 
    lstrcpy( szRcPath ,szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_4_SkyBox" );
    SetCurrentDirectory( szRcPath );
    TCHAR* szFile_4 = L"ShmFile.shm";
    if( FAILED( hr = g_RenderObject_SkyBox.OnCreateDevice( pd3dDevice ,szFile_4 ))) return hr;
    // 
    lstrcpy( szRcPath ,szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_3_MoveObject" );
    SetCurrentDirectory( szRcPath );
    TCHAR* szFile_3 = L"ShmFile.shm";
    if( FAILED( hr = g_RenderObject_MoveObject.OnCreateDevice( pd3dDevice ,szFile_3 ))) return hr;
    // 
    lstrcpy( szRcPath ,szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_Debug" );
    SetCurrentDirectory( szRcPath );
    if( FAILED( hr = g_TextureDebug.OnCreateDevice( pd3dDevice ))) return hr;
    //
    g_RenderToCubeMap.AddToRenderList( &g_RenderObject_Ground , ENU_RENDEROBJECT);
    g_RenderToCubeMap.AddToRenderList( &g_RenderObject_MoveObject , ENU_RENDEROBJECT );
    g_RenderToCubeMap.AddToRenderList( &g_RenderObject_SkyBox , ENU_CUBE );
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
    D3DXMatrixPerspectiveFovLH( &g_matProject ,D3DX_PI/4 ,fAspectRatio ,1.0 ,100.0 );
    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    //
    g_RenderToCubeMap.OnResetDevice( pd3dDevice , fAspectRatio );
    //
    g_RenderObject_Ground.OnResetDevice( pd3dDevice ,fAspectRatio );
    g_RenderObject_MoveObject.OnResetDevice( pd3dDevice ,fAspectRatio );
    g_RenderObject_SkyBox.OnResetDevice( );
    g_TextureDebug.OnResetDevice( pd3dDevice );
    //
    return S_OK;
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
  {
    //
    //D3DXMatrixRotationY( &g_matWorld ,fTime );
    D3DXMatrixIdentity( &g_matWorld );
    // 
    D3DXVECTOR3 vEyePt( 30 * sin( fTime / 2.0 )  , 10 , 30 * cos( fTime / 2.0 ));
    D3DXVECTOR3 vLookatPt( 0.0 , 0 , 0.0 );
    D3DXVECTOR3 vUpVec( 0 , 1 , 0 );
    D3DXMatrixLookAtLH( &g_matView , &vEyePt , &vLookatPt , &vUpVec );
    // 
    //g_RenderObject_Ground.OnFrameMove( &g_matWorld , &g_matView , &g_matProject , fTime  );
    //g_RenderObject_MoveObject.OnFrameMove( &g_matWorld , &g_matView , &g_matProject , fTime  );
    g_RenderToCubeMap.OnFrameMove( &g_matWorld , &g_matView , &g_matProject , &vEyePt , fTime  );
    // 将物体渲染到纹理//
  }

//--------------------------------------------------------------------------------------
// 函数：OnFrameRender
// 功能：渲染过程中，重定向渲染页面与渲染到缓冲区一样
// 大概过程：首先保存真正的渲染页面，然后再设置渲染目标，再渲染，最后显示两个页面
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
  {
    HRESULT hr;
    // 在此可以渲染场景了
    D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"RenderToCubeMap" );
      {
        hr = g_RenderToCubeMap.RenderToCubeMap( pd3dDevice , &g_matWorld , &g_matView , &g_matProject , fTime );
      }
    D3DPERF_EndEvent( );
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR( 0.0f,0.25f,0.25f,0.55f ), 1.0f, 0 ));
    // 渲染场景
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
      {
        // 渲染小球
        D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"UseCubeMap" );
          {
            g_RenderToCubeMap.OnFrameRender( pd3dDevice );
          }
        D3DPERF_EndEvent( );
        // 移动地面和小立方体
        g_RenderObject_Ground.OnFrameMove( &g_matWorld , &g_matView , &g_matProject , fTime  );
        g_RenderObject_MoveObject.OnFrameMove( &g_matWorld , &g_matView , &g_matProject , fTime  );
        g_RenderObject_SkyBox.OnFrameMove( &g_matWorld , &g_matView , &g_matProject , fTime  );
        // 渲染地面
        D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"RenderToGround" );
          {
            g_RenderObject_Ground.OnFrameRender( pd3dDevice );      
          }
        D3DPERF_EndEvent( );
        // 渲染小立方体
        D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"RenderMoveObject" );
          {
            g_RenderObject_MoveObject.OnFrameRender( pd3dDevice );
          }
        D3DPERF_EndEvent( );
        // 渲染立方体
        D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"RenderSkyBox" );
          {
            g_RenderObject_SkyBox.OnFrameRender( pd3dDevice );
          }
        D3DPERF_EndEvent( );
        // 显示渲染的纹理
        D3DPERF_BeginEvent( D3DCOLOR_XRGB( 0 , 255 , 0 ) , L"DebugTexture" );
          {
            //g_TextureDebug.OnFrameRender( pd3dDevice , g_RenderToCubeMap.GetTexture( ) , 0 );
          }
        D3DPERF_EndEvent( );
        // 渲染结束
        g_HUD.OnRender( fElapsedTime ); 
        RenderText( fTime );
        
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
    if( g_pFont )        g_pFont->OnLostDevice();
    SAFE_RELEASE(g_pSprite);
    g_RenderToCubeMap.OnLostDevice( );
    g_RenderObject_Ground.OnLostDevice( );
    g_RenderObject_MoveObject.OnLostDevice( );
    g_RenderObject_SkyBox.OnLostDevice( );
    g_TextureDebug.OnLostDevice( );
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
  {
    SAFE_RELEASE(g_pFont);
    g_RenderToCubeMap.OnDestroyDevice( );
    g_RenderObject_Ground.OnDestroyDevice( );
    g_RenderObject_MoveObject.OnDestroyDevice( );
    g_RenderObject_SkyBox.OnDestroyDevice( );
    g_TextureDebug.OnDestroyDevice( );
  }

