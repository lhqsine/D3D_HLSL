//--------------------------------------------------------------------------------------
// File: EmptyProject.cpp
// Starting point for new Direct3D applications
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "dxstdafx.h"

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
  {
    DXUTSetCursorSettings( true, true );
    //DXUTInit( true, true, true );
    DXUTCreateWindow( L"EmptyProject_DXUT_1" );
    DXUTMainLoop();
    return DXUTGetExitCode();
  }

