

#include "dxstdafx.h"
#include "resource.h"

#include "CRenderObject.h"
//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont         = NULL; // 字体
ID3DXSprite*            g_pSprite       = NULL; // 显示字体用
CDXUTDialog             g_HUD;                  // 与用户的显示接口

D3DXMATRIXA16 g_matWorld;
D3DXMATRIXA16 g_matView;
D3DXMATRIXA16 g_matProject;

CRENDEROBJECT          	g_RenderObjectOccluder; 
CRENDEROBJECT           g_RenderGround;         
CRENDEROBJECT           g_RenderLight;         
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
    DXUTCreateWindow( L"PlaneShadow_1" );
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
    lstrcpy( szRcPath , szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_1_Object" );
    SetCurrentDirectory( szRcPath );
    TCHAR* szFile_1 = L"ShmFile.shm";
    if( FAILED( hr = g_RenderObjectOccluder.OnCreateDevice( pd3dDevice ,szFile_1 ))) return hr;
    //
	lstrcpy( szRcPath , szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_2_Light" );
    SetCurrentDirectory( szRcPath );
    TCHAR* szFile_2 = L"ShmFile.shm";
    if( FAILED( hr = g_RenderLight.OnCreateDevice( pd3dDevice ,szFile_2 ))) return hr;
    //
	lstrcpy( szRcPath , szAppPath );
    lstrcat( szRcPath ,L"\\ResFile_3_Ground" );
    SetCurrentDirectory( szRcPath );
    TCHAR* szFile_3 = L"ShmFile.shm";
    if( FAILED( hr = g_RenderGround.OnCreateDevice( pd3dDevice ,szFile_3 ))) return hr;
	
	// 
    //lstrcpy( szRcPath , szAppPath );
    //lstrcat( szRcPath ,L"\\ResFile_1_Ordinate" );
    //SetCurrentDirectory( szRcPath );
    //TCHAR* szFile_1 = L"ShmFile.shm";
    //if( FAILED( hr = g_RenderObject_Ordinate.OnCreateDevice( pd3dDevice ,szFile_1 ))) return hr;
    ////
    //lstrcpy( szRcPath ,szAppPath );
    //lstrcat( szRcPath ,L"\\ResFile_2_Wave" );
    //SetCurrentDirectory( szRcPath );
    //TCHAR* szFile_2 = L"ShmFile.shm";
    //if( FAILED( hr = g_RenderObject_Wave.OnCreateDevice( pd3dDevice ,szFile_2 ))) return hr;
    ////     
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
    g_RenderObjectOccluder.OnResetDevice( );
    g_RenderLight.OnResetDevice( );
    g_RenderGround.OnResetDevice( );
    return S_OK;
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
  {
    //
    //D3DXMatrixRotationY( &g_matWorld ,fTime );
    // 
	D3DXMatrixIdentity( &g_matWorld );
    D3DXVECTOR3 vEyePt( 30 * sin( fTime ) , 7 , 30 * cos( fTime ));

    D3DXVECTOR3 vLookatPt( 0, 2, 0 );
    D3DXVECTOR3 vUpVec( 0, 1, 0 );
    D3DXMatrixLookAtLH( &g_matView, &vEyePt, &vLookatPt, &vUpVec );
	
	D3DXVECTOR4 vViewPosition( vEyePt.x ,vEyePt.y ,vEyePt.z ,0.0f );
    D3DXVECTOR4 vLightPosition( 4.0f * sinf( -fTime * 0.7 ) , 10.0f  , 4.0f * cosf( -fTime * 0.7 ), 1.0f );
    D3DXVECTOR4 vOccluderPosition( 2.0f * sinf( fTime ) , 4.0f  , 2.0f * cosf( fTime ), 1.0f );
	
	g_RenderGround.OnFrameMove( &g_matWorld ,&g_matView ,&g_matProject ,fTime ,&vViewPosition );
    g_RenderGround.SetLightPosition( &vLightPosition );
	
    //     先旋转在平移
    D3DXMATRIX matRotateX ;
    D3DXMatrixRotationX( &matRotateX , fTime );
    D3DXMatrixTranslation( &g_matWorld , vOccluderPosition.x , vOccluderPosition.y , vOccluderPosition.z );
    D3DXMatrixMultiply( &g_matWorld , &matRotateX , &g_matWorld );
    g_RenderObjectOccluder.OnFrameMove( &g_matWorld , &g_matView ,&g_matProject ,fTime ,&vLightPosition );
    g_RenderObjectOccluder.SetLightPosition( &vLightPosition );
	
	//压扁到平面
    D3DXPLANE Plane( 0.0 , 1.0 , 0.0 , -0.01 );
    g_RenderObjectOccluder.ShadowToPlane( &vLightPosition , &Plane );
 
 
 
    D3DXMatrixTranslation( &g_matWorld , vLightPosition.x , vLightPosition.y , vLightPosition.z );
    g_RenderLight.OnFrameMove( &g_matWorld ,&g_matView ,&g_matProject ,fTime ,&vLightPosition );
    g_RenderLight.SetLightPosition( &vLightPosition );
    // 
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
  {
    HRESULT hr;
    // 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f,0.25f,0.25f,0.55f), 1.0f, 0) );
    // 渲染场景
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
      {
        g_RenderObjectOccluder.OnFrameRender( pd3dDevice );
        g_RenderLight.OnFrameRender( pd3dDevice );
        g_RenderGround.OnFrameRender( pd3dDevice );
        g_RenderObjectOccluder.RenderShadowToPlane( pd3dDevice );
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
    CDXUTDirectionWidget::StaticOnLostDevice();
    if( g_pFont )        g_pFont->OnLostDevice();
    SAFE_RELEASE(g_pSprite);
    g_RenderGround.OnLostDevice( );
    g_RenderObjectOccluder.OnLostDevice( );
    g_RenderLight.OnLostDevice( );
  }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
  {
    CDXUTDirectionWidget::StaticOnDestroyDevice();
    SAFE_RELEASE(g_pFont);
    g_RenderGround.OnDestroyDevice( );
    g_RenderObjectOccluder.OnDestroyDevice( );
    g_RenderLight.OnDestroyDevice( );
  }

