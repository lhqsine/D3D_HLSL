//--------------------------------------------------------------------------------------
// File: BasicHLSL.fx
// Box
//--------------------------------------------------------------------------------------

texture3D g_VolumeTexture;              // Color texture for mesh

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

sampler MeshVolumeSampler = sampler_state
  {
    Texture = <g_VolumeTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD0;  
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float4 Texture3D  : TEXCOORD1;
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.Texture3D = Output.Position;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    float4 sky        = float4( 0.0 , 0.0 , 1.0 , 1.0 );
    float4 cloudColor = float4( 1.0 , 1.0 , 1.0 , 1.0 );
    In.Texture3D.xy +=  g_fTime * 3.7;
    In.Texture3D.z  +=  g_fTime * 4.8;
    float noisy = tex3D( MeshVolumeSampler , In.Texture3D.xyz / 115.0 ).r;
    float lrp = noisy * 4  -1.0;
    return lerp( cloudColor , sky , saturate( lrp ));
  }

//------------------------------------------------------------------------------
// technique£ºBox
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        FillMode            = Solid;//WireFrame;//Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
