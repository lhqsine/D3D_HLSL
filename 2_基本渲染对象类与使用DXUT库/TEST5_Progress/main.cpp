//--------------------------------------------------------------------------------------
// File: EmptyProject.cpp
// Starting point for new Direct3D applications
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "resource.h"
#include "CProgress.h"
//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // 字体对象
ID3DXSprite*            g_pSprite       = NULL; // 显示字体用
CModelViewerCamera      g_Camera;               // 用户摄影机
CDXUTDialog             g_HUD;                  // 与用户的显示接口

CPROGRESS                  g_RenderObject;         // 显示的物体

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        4

//--------------------------------------------------------------------------------------
// Forward declarations 
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
void    RenderText( double fTime);
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
    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );
    InitApp();
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTCreateWindow( L"CH2_TEST6_Progress" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 410, IsDeviceAcceptable, ModifyDeviceSettings );
    DXUTMainLoop();
    return DXUTGetExitCode();
  }
//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
  {
	  g_HUD.SetCallback( OnGUIEvent);

	  int iY = 10;
	  g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screeen", 35, iY, 125, 22);
	  g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF(F3)", 35, iY += 24, 125, 22);
	  g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );		
  }
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
  D3DFORMAT BackBufferFormat, bool bWindowed )
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
    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_THIN, 1, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
      L"宋体", &g_pFont ) );




    if( FAILED( hr = g_RenderObject.OnCreateDevice( pd3dDevice  ))) return hr;
    // 
    // 
    D3DXVECTOR3 vecEye( 10.0f, 8.0f, 0.0f );
    D3DXVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
    return S_OK;
    return S_OK;
  }
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
  const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
  {
    HRESULT hr;
    if( g_pFont )    V_RETURN( g_pFont->OnResetDevice() );
    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pSprite ) );
    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170 , 0);
	g_HUD.SetSize( 170, 170 );
	g_RenderObject.OnResetDevice(pd3dDevice );
    return S_OK;
  }
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
  {
		
  }
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
  {
    HRESULT hr;
	D3DXMATRIXA16 DXUT_WorldViewProjection;

	D3DXMATRIXA16 mWorld;
	D3DXMATRIXA16 mView;
	D3DXMATRIXA16 mProj;

	V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.25f, 0.25f, 0.55f), 1.0f, 0))
   
    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
      {
		  g_RenderObject.OnFrameRender( pd3dDevice , abs( sin( fTime )) );

		  g_HUD.OnRender( fElapsedTime );
		  RenderText(fTime );
			
		  V( pd3dDevice->EndScene() );
      }
  }
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void RenderText( double fTime  )
  {
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
    CDXUTTextHelper txtHelper( g_pFont, g_pSprite, 16 );
    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 2, 0 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats() );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

    txtHelper.DrawTextLine( L"显示需要的信息" );
    txtHelper.SetInsertionPos( 10, pd3dsdBackBuffer->Height- 15 * 2 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( L"正在装入:" );
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
    g_RenderObject.OnLostDevice( );
  }
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
  {
	  CDXUTDirectionWidget::StaticOnDestroyDevice();
	  SAFE_RELEASE(g_pFont);
	  g_RenderObject.OnDestroyDevice();
  }
