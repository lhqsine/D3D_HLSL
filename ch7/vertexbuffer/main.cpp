#include "dxstdafx.h"
#include <stdio.h>
#include <conio.h>
#include "CBuffer.h"

int main( )
  {
    HRESULT hr;
    CBUFFER CBuffer;
    if( SUCCEEDED( hr = CBuffer.LoadModel( L"Model.x" ))) 
      {
        printf( "\n���㻺��������" );
        CBuffer.DisplayVertex( );
        printf( "\n��������������" );
        CBuffer.DisplayIndex( );
        printf( "\n�ڽӻ���������" );
        CBuffer.DisplayAdjInfo( );
        printf( "\n����������" );
        CBuffer.DisplayPointRep( );;
      }
    printf( "\n��ESC���˳�" );
    _getch( );
    return 0;
  }
