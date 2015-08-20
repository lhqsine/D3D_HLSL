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
        printf( "\n顶点缓冲区数据" );
        CBuffer.DisplayVertex( );
        printf( "\n索引缓冲区数据" );
        CBuffer.DisplayIndex( );
        printf( "\n邻接缓冲区数据" );
        CBuffer.DisplayAdjInfo( );
        printf( "\n代理顶点数据" );
        CBuffer.DisplayPointRep( );;
      }
    printf( "\n按ESC键退出" );
    _getch( );
    return 0;
  }
