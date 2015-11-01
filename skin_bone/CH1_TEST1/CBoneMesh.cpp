#include "CBoneMesh.h"
//-----------------------------------------------------------------------------
// 名字：CAllocateHierarchy 类实现
// 功能：实现 CAllocateHierarchy 类的 4 个成员函数
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 名字：AllocateName()
// 功能：将 Name 的名字拷贝到 pNewName
//-----------------------------------------------------------------------------
HRESULT AllocateName( LPCSTR Name, LPSTR *pNewName )
  {
    UINT cbLength;
    if( Name != NULL )
      {
        cbLength = (UINT)strlen(Name) + 1;
        *pNewName = new CHAR[cbLength];
        if (*pNewName == NULL)
            return E_OUTOFMEMORY;
        memcpy( *pNewName, Name, cbLength*sizeof(CHAR) );
      }
    else
      {
        *pNewName = NULL;
      }
    return S_OK;
  }
//-----------------------------------------------------------------------------
// 名字：CreateFrame()
// 功能：通过传入参数 Name 来创建一个骨骼结构 D3DXFRAME_DERIVED 结构，并设置新创建
//       结构的相关变量。
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame( LPCSTR Name, LPD3DXFRAME *ppNewFrame )
  {
    HRESULT hr = S_OK;
    D3DXFRAME_DERIVED *pFrame;

    *ppNewFrame = NULL;

    pFrame = new D3DXFRAME_DERIVED;
    if (pFrame == NULL)
      {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
      }
    hr = AllocateName(Name, &pFrame->Name);
    if (FAILED(hr))
        goto e_Exit;

    // initialize other data members of the frame
    D3DXMatrixIdentity(&pFrame->TransformationMatrix);
    D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

    pFrame->pMeshContainer = NULL;
    pFrame->pFrameSibling = NULL;
    pFrame->pFrameFirstChild = NULL;

    *ppNewFrame = pFrame;
    pFrame = NULL;

    e_Exit:
    delete pFrame;
    return hr;
  }

//-----------------------------------------------------------------------------
// 名字：CreateMeshContainer()
// 功能：通过传入很多参数，最终创建一个按传入参数重新创建一个 ppNewMeshContainer，
//       其中调用了 GenerateSkinnedMesh
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer( LPCSTR Name, 
  CONST D3DXMESHDATA *pMeshData,  CONST D3DXMATERIAL *pMaterials, 
  CONST D3DXEFFECTINSTANCE *pEffectInstances, 
  DWORD NumMaterials,    CONST DWORD *pAdjacency, 
  LPD3DXSKININFO pSkinInfo,  LPD3DXMESHCONTAINER *ppNewMeshContainer) 
  {
    HRESULT hr;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
    UINT NumFaces;
    UINT iMaterial;
    UINT iBone, cBones;
    LPDIRECT3DDEVICE9 pd3dDevice = NULL;

    LPD3DXMESH pMesh = NULL;

    *ppNewMeshContainer = NULL;

    // this sample does not handle patch meshes, so fail when one is found
    if (pMeshData->Type != D3DXMESHTYPE_MESH)
      {
        hr = E_FAIL;
        goto e_Exit;
      }

    // get the pMesh interface pointer out of the mesh data structure
    pMesh = pMeshData->pMesh;

    // this sample does not FVF compatible meshes, so fail when one is found
    if (pMesh->GetFVF() == 0)
      {
        hr = E_FAIL;
        goto e_Exit;
      }

    // allocate the overloaded structure to return as a D3DXMESHCONTAINER
    pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
    if (pMeshContainer == NULL)
      {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
      }
    memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

    // make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
    hr = AllocateName(Name, &pMeshContainer->Name);
    if (FAILED(hr))
        goto e_Exit;        

    pMesh->GetDevice(&pd3dDevice);
    NumFaces = pMesh->GetNumFaces();

    // if no normals are in the mesh, add them
    if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
      {
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        // clone the mesh to make room for the normals
        hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), 
                                    pMesh->GetFVF() | D3DFVF_NORMAL, 
                                    pd3dDevice, &pMeshContainer->MeshData.pMesh );
        if (FAILED(hr))
            goto e_Exit;

        // get the new pMesh pointer back out of the mesh container to use
        // NOTE: we do not release pMesh because we do not have a reference to it yet
        pMesh = pMeshContainer->MeshData.pMesh;

        // now generate the normals for the pmesh
        D3DXComputeNormals( pMesh, NULL );
      }
    else  // if no normals, just add a reference to the mesh for the mesh container
      {
        pMeshContainer->MeshData.pMesh = pMesh;
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        pMesh->AddRef();
      }
        
    // allocate memory to contain the material information.  This sample uses
    //   the D3D9 materials and texture names instead of the EffectInstance style materials
    pMeshContainer->NumMaterials = max(1, NumMaterials);
    pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
    pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
    pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
    if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL))
      {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
      }

    memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);
    memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);

    // if materials provided, copy them
    
    if (NumMaterials > 0)            
      {
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);

        for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++)
          {
            if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
              {
                WCHAR strTexturePath[MAX_PATH];
                WCHAR wszBuf[MAX_PATH];
                MultiByteToWideChar( CP_ACP, 0, pMeshContainer->pMaterials[iMaterial].pTextureFilename, -1, wszBuf, MAX_PATH );
                wszBuf[MAX_PATH - 1] = L'\0';
                DXUTFindDXSDKMediaFileCch( strTexturePath, MAX_PATH, wszBuf );
                if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, strTexturePath,
                                                        &pMeshContainer->ppTextures[iMaterial] ) ) )
                    pMeshContainer->ppTextures[iMaterial] = NULL;

                // don't remember a pointer into the dynamic memory, just forget the name after loading
                pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
              }
          }
      }
    else // if no materials provided, use a default one
      {
        pMeshContainer->pMaterials[0].pTextureFilename = NULL;
        memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
      }
    
    // if there is skinning information, save off the required data and then setup for HW skinning
    if (pSkinInfo != NULL)
      {
        // first save off the SkinInfo and original mesh data
        pMeshContainer->pSkinInfo = pSkinInfo;
        pSkinInfo->AddRef();

        pMeshContainer->pOrigMesh = pMesh;
        pMesh->AddRef();

        // Will need an array of offset matrices to move the vertices from the figure space to the bone's space
        cBones = pSkinInfo->GetNumBones();
        pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
        if (pMeshContainer->pBoneOffsetMatrices == NULL)
          {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
          }

        // get each of the bone offset matrices so that we don't need to get them later
        for (iBone = 0; iBone < cBones; iBone++)
          {
            pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
          }

        // GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
        hr = m_pApp->GenerateSkinnedMesh( pd3dDevice, pMeshContainer );
        if (FAILED(hr))
            goto e_Exit;
      }

    *ppNewMeshContainer = pMeshContainer;
    pMeshContainer = NULL;

    e_Exit:
    SAFE_RELEASE(pd3dDevice);

    // call Destroy function to properly clean up the memory allocated 
    if (pMeshContainer != NULL)
      {
        DestroyMeshContainer(pMeshContainer);
      }

    return hr;
  }


//-----------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyFrame()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
  {
    SAFE_DELETE_ARRAY( pFrameToFree->Name );
    SAFE_DELETE( pFrameToFree );
    return S_OK; 
  }
//-----------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyMeshContainer()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
  {
    UINT iMaterial;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

    SAFE_DELETE_ARRAY( pMeshContainer->Name );
    SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
    SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
    SAFE_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );

    // release all the allocated textures
    if (pMeshContainer->ppTextures != NULL)
      {
        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
          {
            SAFE_RELEASE( pMeshContainer->ppTextures[iMaterial] );
          }
      }

    SAFE_DELETE_ARRAY( pMeshContainer->ppTextures );
    SAFE_DELETE_ARRAY( pMeshContainer->ppBoneMatrixPtrs );
    SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );
    SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
    SAFE_RELEASE( pMeshContainer->pSkinInfo );
    SAFE_RELEASE( pMeshContainer->pOrigMesh );
    SAFE_DELETE( pMeshContainer );
    return S_OK;
  }



// ----------------- The End of CAllocateHierarchy --------------------
//-----------------------------------------------------------------------------
// 名字：CBONEANIMATION 类实现
// 功能：实现 CBONEANIMATION 类的 16 个成员函数
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 名字：SetupBoneMatrixPointersOnMesh
// 功能：将 Mesh 与骨骼（Frame）对应
//-----------------------------------------------------------------------------
HRESULT CBONEANIMATION::SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase )
  {
    UINT iBone, cBones;
    D3DXFRAME_DERIVED *pFrame;

    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

    // if there is a skinmesh, then setup the bone matrices
    if (pMeshContainer->pSkinInfo != NULL)
      {
        cBones = pMeshContainer->pSkinInfo->GetNumBones();

        pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];
        if (pMeshContainer->ppBoneMatrixPtrs == NULL)
            return E_OUTOFMEMORY;

        for (iBone = 0; iBone < cBones; iBone++)
          {
            pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind( m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone) );
            if (pFrame == NULL)
                return E_FAIL;

            pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
          }
      }

    return S_OK;
  }
//-----------------------------------------------------------------------------
// 名字：DrawMeshContainer
// 功能：最终体现在渲染 Mesh 上
//-----------------------------------------------------------------------------
void CBONEANIMATION::DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
  {
    HRESULT hr;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
    UINT iAttrib;
    LPD3DXBONECOMBINATION pBoneComb;

    UINT iMatrixIndex;
    UINT iPaletteEntry;
    D3DXMATRIXA16 matTemp;
    D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );
    if (pMeshContainer->pSkinInfo != NULL)
      {
        pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
        for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
          { 
            // first calculate all the world matrices
            for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
              {
                iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
                if (iMatrixIndex != UINT_MAX)
                  {
                    D3DXMatrixMultiply( &m_pBoneMatrices[iPaletteEntry] , &pMeshContainer->pBoneOffsetMatrices[ iMatrixIndex ], pMeshContainer->ppBoneMatrixPtrs[ iMatrixIndex ]);
                    //D3DXMatrixMultiply( &m_pBoneMatrices[iPaletteEntry], &matTemp, &m_matView );
                  }
              }
            V( m_pEffect->SetMatrixArray( "g_mWorldMatrixArray", m_pBoneMatrices, pMeshContainer->NumPaletteEntries) );
            // setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
            V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );

            // Set CurNumBones to select the correct vertex shader for the number of bones
            V( m_pEffect->SetInt( "CurNumBones", pMeshContainer->NumInfl -1) );

            // Start the effect now all parameters have been updated
            UINT numPasses;
            V( m_pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE ) );
            for( UINT iPass = 0; iPass < numPasses; iPass++ )
              {
                V( m_pEffect->BeginPass( iPass ) );
                V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );
                V( m_pEffect->EndPass() );
              }
            V( m_pEffect->End() );
          }
      }
  }

//-----------------------------------------------------------------------------
// 名字：DrawFrame
// 功能：渲染继承
//-----------------------------------------------------------------------------
void CBONEANIMATION::DrawFrame( IDirect3DDevice9 *pd3dDevice, LPD3DXFRAME pFrame )
  {
    LPD3DXMESHCONTAINER pMeshContainer;
    pMeshContainer = pFrame->pMeshContainer;
    while (pMeshContainer != NULL)
      {
        DrawMeshContainer( pd3dDevice, pMeshContainer, pFrame );
        pMeshContainer = pMeshContainer->pNextMeshContainer;
      }
    if (pFrame->pFrameSibling != NULL)
      {
        DrawFrame( pd3dDevice, pFrame->pFrameSibling);
      }
    if (pFrame->pFrameFirstChild != NULL)
      {
        DrawFrame( pd3dDevice, pFrame->pFrameFirstChild );
      }
  }
//-----------------------------------------------------------------------------
// 名字：SetupBoneMatrixPointers
// 功能：更新骨骼继承
//-----------------------------------------------------------------------------
HRESULT CBONEANIMATION::SetupBoneMatrixPointers( LPD3DXFRAME pFrame )
  {
    HRESULT hr;

    if (pFrame->pMeshContainer != NULL)
      {
        hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);
        if (FAILED(hr))
            return hr;
      }

    if (pFrame->pFrameSibling != NULL)
      {
        hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);
        if (FAILED(hr))
            return hr;
      }

    if (pFrame->pFrameFirstChild != NULL)
      {
        hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);
        if (FAILED(hr))
            return hr;
      }

    return S_OK;
  }

//-----------------------------------------------------------------------------
// 名字：UpdateFrameMatrices
// 功能：更新骨骼继承
//-----------------------------------------------------------------------------
void CBONEANIMATION::UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix )
  {
    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

    if (pParentMatrix != NULL)
        D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);
    else
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

    if (pFrame->pFrameSibling != NULL)
      {
        UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
      }

    if (pFrame->pFrameFirstChild != NULL)
      {
        UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
      }
  }
//-----------------------------------------------------------------------------
// 名字：CBONEANIMATION
// 功能：构造函数
//-----------------------------------------------------------------------------
CBONEANIMATION::CBONEANIMATION( )
  {
    m_pAnimController  = NULL;
    m_pBoneMatrices    = NULL;
    m_pEffect          = NULL;
    m_pFrameRoot       = NULL;
    //
    m_pColorTexture    = NULL;
  }
//-----------------------------------------------------------------------------
// 名字：~CBONEANIMATION
// 功能：析构函数
//-----------------------------------------------------------------------------
CBONEANIMATION::~CBONEANIMATION( )
  {
    SAFE_RELEASE( m_pAnimController );
    SAFE_RELEASE( m_pEffect );
    //
    SAFE_RELEASE( m_pColorTexture );
  }

//-----------------------------------------------------------------------------
// 名字：FrameMove
// 功能：改变每帧动画
//-----------------------------------------------------------------------------
void CBONEANIMATION::OnFrameMove( D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime )
  {
    HRESULT hr;
    hr = m_pEffect->SetMatrix( "g_matWorld" ,pWorld );
    hr = m_pEffect->SetMatrix( "g_matView" ,pView );
    hr = m_pEffect->SetMatrix( "g_matProject" ,pProject );
    if( m_pAnimController != NULL )
      m_pAnimController->AdvanceTime( fTime, NULL );
    m_matView = *pView;
    UpdateFrameMatrices( m_pFrameRoot, pWorld );
  }
//-----------------------------------------------------------------------------
// 名字：Render
// 功能：渲染
//-----------------------------------------------------------------------------
void CBONEANIMATION::OnFrameRender( IDirect3DDevice9* pd3dDevice )
  {
    DrawFrame( pd3dDevice, m_pFrameRoot );
  }
//-----------------------------------------------------------------------------
// 名字：InitDeviceObjects
// 功能：初始化
//-----------------------------------------------------------------------------
HRESULT CBONEANIMATION::OnCreateDevice( IDirect3DDevice9* pd3dDevice, TCHAR* FileName )
  {
    HRESULT    hr = S_OK;
    if( FAILED( hr = LoadResource( FileName ))) return hr;
    //------------------- Model 
    TCHAR* szFile = GetResourceName( ENU_MODELX );
    CAllocateHierarchy Alloc( this );
    if( FAILED( hr = D3DXLoadMeshHierarchyFromX( szFile, D3DXMESH_MANAGED, pd3dDevice, &Alloc, NULL, &m_pFrameRoot, &m_pAnimController )))
      return hr;
    if( FAILED( hr = SetupBoneMatrixPointers( m_pFrameRoot ))) return hr;
    if( FAILED( hr = D3DXFrameCalculateBoundingSphere( m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius )))
      return hr;
    //------------------ FX
    szFile = GetResourceName( ENU_FXFILE );
    LPD3DXBUFFER pCode = NULL;
    if( FAILED( hr = D3DXCreateEffectFromFile( pd3dDevice, szFile, NULL, NULL, 0,  NULL, &m_pEffect, &pCode )))
      {
        LPVOID pBufErr = NULL;
        if( NULL == pCode ) return E_FAIL;
        pBufErr = pCode->GetBufferPointer();
        char* ErrInfo = ( char* ) pBufErr;
        pCode->Release( );
        return hr;
      }
    //--------------------- Map ------------
    szFile = GetResourceName( ENU_TEXCOLORMAP );
    if( FAILED( hr = D3DXCreateTextureFromFileEx( pd3dDevice, szFile, D3DX_DEFAULT, D3DX_DEFAULT, 
      D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
      D3DX_DEFAULT, D3DX_DEFAULT, 0, 
      NULL, NULL, &m_pColorTexture ) )) return hr;
    hr = m_pEffect->SetTexture( "g_ColorTexture" ,m_pColorTexture );
    return hr;
  }
//-----------------------------------------------------------------------------
// 名字：RestoreDeviceObjects
// 功能：恢复
//-----------------------------------------------------------------------------
HRESULT CBONEANIMATION::OnResetDevice( LPDIRECT3DDEVICE9 pd3dDevice ,const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
  {
    float fAspect = (float)pBackBufferSurfaceDesc->Width / (float)pBackBufferSurfaceDesc->Height;
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, fAspect, m_fObjectRadius/64.0f, m_fObjectRadius*200.0f );
    return S_OK;
    
  }
//-----------------------------------------------------------------------------
// 名字：InvalidateDeviceObjects
// 功能：与主函数同名函数功能相同
//-----------------------------------------------------------------------------
void CBONEANIMATION::OnLostDevice()
  {
    if( m_pEffect )
      m_pEffect->OnLostDevice();
  }
//-----------------------------------------------------------------------------
// 名字：DeleteDeviceObjects
// 功能：与主函数同名函数功能相同
//-----------------------------------------------------------------------------
void CBONEANIMATION::OnDestroyDevice()
  {
    SAFE_RELEASE( m_pEffect );
    CAllocateHierarchy Alloc( this );
    D3DXFrameDestroy( m_pFrameRoot, &Alloc );
    SAFE_RELEASE( m_pAnimController );
    SAFE_RELEASE( m_pColorTexture );
  }

//--------------------------------------------------------------------------------------
// Called either by CreateMeshContainer when loading a skin mesh, or when 
// changing methods.  This function uses the pSkinInfo of the mesh 
// container to generate the desired drawable mesh and bone combination 
// table.
//--------------------------------------------------------------------------------------
HRESULT CBONEANIMATION::GenerateSkinnedMesh( IDirect3DDevice9 *pd3dDevice, D3DXMESHCONTAINER_DERIVED *pMeshContainer )
  {
    HRESULT hr = S_OK;
    D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

    if( pMeshContainer->pSkinInfo == NULL )       return hr;

    SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
    SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );
    // Get palette size
    // First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
    // (96 - 9) /3 i.e. Maximum constant count - used constants 
    UINT MaxMatrices = 26; 
    pMeshContainer->NumPaletteEntries = min(MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());

    DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
    if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
      {
        pMeshContainer->UseSoftwareVP = false;
        Flags |= D3DXMESH_MANAGED;
      }
    else
      {
        pMeshContainer->UseSoftwareVP = true;
        //g_bUseSoftwareVP = true;
        Flags |= D3DXMESH_SYSTEMMEM;
      }

    SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

    hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
                                            (
                                            pMeshContainer->pOrigMesh,
                                            Flags, 
                                            pMeshContainer->NumPaletteEntries, 
                                            pMeshContainer->pAdjacency, 
                                            NULL, NULL, NULL,             
                                            &pMeshContainer->NumInfl,
                                            &pMeshContainer->NumAttributeGroups, 
                                            &pMeshContainer->pBoneCombinationBuf, 
                                            &pMeshContainer->MeshData.pMesh);
    if (FAILED(hr))
        goto e_Exit;


    // FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
    DWORD NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
    if (NewFVF != pMeshContainer->MeshData.pMesh->GetFVF())
      {
        LPD3DXMESH pMesh;
        hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
        if (!FAILED(hr))
          {
            pMeshContainer->MeshData.pMesh->Release();
            pMeshContainer->MeshData.pMesh = pMesh;
            pMesh = NULL;
          }
      }

    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
    LPD3DVERTEXELEMENT9 pDeclCur;
    hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl);
    if (FAILED(hr))
        goto e_Exit;

    // the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
    //   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
    //          this is more of a "cast" operation
    pDeclCur = pDecl;
    while (pDeclCur->Stream != 0xff)
      {
        if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
            pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
        pDeclCur++;
      }

    hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl);
    if (FAILED(hr))
        goto e_Exit;

    // allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
    if( m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones() )
      {
        m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

        // Allocate space for blend matrices
        delete[] m_pBoneMatrices; 
        m_pBoneMatrices  = new D3DXMATRIXA16[m_NumBoneMatricesMax];
        if( m_pBoneMatrices == NULL )
          {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
          }
      }
    e_Exit:
    return hr;
  }


//-----------------------------------------------------------------------------
// 名字：GetAnimationNum
// 功能：取动画组的个数
//-----------------------------------------------------------------------------

//------------------- the end of CBoneAnimation function ----------------------

//--------------------the end of CBoneAnimation.cpp file ----------------------

//----------------------------------------------------------------------------
// LoadResource
// 功能：将本渲染对象的文件（*.shm）装入，分析供 GetResourceName 使用
//----------------------------------------------------------------------------
  HRESULT CBONEANIMATION::LoadResource( TCHAR* FileName )
  {
    // 打开文件
    BOOL bReturn;
    HANDLE hAndle;
    DWORD  uByte;
    // 文件存在否
    //---------------------- 调试用 --------------------------
    //TCHAR strPath[MAX_PATH];
    //GetCurrentDirectory( MAX_PATH , strPath );
    if( INVALID_HANDLE_VALUE == ( hAndle = CreateFile( FileName,
      GENERIC_READ,0,NULL, OPEN_EXISTING,0,NULL )))    return E_FAIL;
    // ----------------- 检查文件头 --------------------
    SSHADERMODULEFILEHEAD sFileHead;
    if( NULL == ( bReturn = ReadFile( hAndle ,( LPVOID )&sFileHead ,sizeof( SSHADERMODULEFILEHEAD ),
      &uByte , NULL )))
      {
        CloseHandle( hAndle );
        return E_FAIL;
      }
    
    if( NULL != strcmp( FILEHEADTAG , sFileHead.s_szHead )) 
      {
        CloseHandle( hAndle );
        return E_FAIL;
      }
    
    // ------------------ 获取资源个数 -----------------
    m_iResNum  = sFileHead.s_iResourceFileNum;  
    if( NULL == ( m_pResourceInfo = new SRESOURCEINFO[ m_iResNum ] )) return E_FAIL;
    // ----------------- 读取资源信息 ------------------
    if( NULL == ( bReturn = ReadFile( hAndle ,( LPVOID )m_pResourceInfo ,sizeof( SRESOURCEINFO ) * m_iResNum ,
      &uByte , NULL )))
      {
        CloseHandle( hAndle );
        return E_FAIL;
      }
    // 
    CloseHandle( hAndle );
    return S_OK;
  }

//----------------------------------------------------------------------------
// GetResourceName
// 功能：利用资源类型索引出文件名
//----------------------------------------------------------------------------
TCHAR* CBONEANIMATION::GetResourceName( RESTYPE enuResType )
  {
    TCHAR* szReturn = NULL;
    for( int i = 0 ; i < m_iResNum ;i ++ )
      {
        if( enuResType == m_pResourceInfo[ i ].s_enuResType ) 
          {
            szReturn = m_pResourceInfo[ i ].s_szFileName;
          }
      }
    return szReturn;
  }


