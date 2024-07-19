#define STRICT
#include "rtg_graph_inter.h"
#include <windows.h>
#include <basetsd.h>
#include <initguid.h>
#include <ddraw.h>
#include "graph/rtg_refreshrate.h"

namespace rt_graph {

BOOL    WINAPI DDEnumCallbackEx( GUID*, LPSTR, LPSTR, LPVOID, HMONITOR );
HRESULT WINAPI EnumModesCallback( LPDDSURFACEDESC pddsd,  LPVOID pContext );
HRESULT WINAPI EnumAllModesCallback( LPDDSURFACEDESC2 pddsd, LPVOID pContext );

//#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

bool RtgDisplayModeEnum::EnumAll()
{
    if (!EnumDevices())
    {
        return false;
    }

    std::list<SDevice>::iterator it = m_listDevices.begin();
    for (; it!=m_listDevices.end(); it++)
    {
        if (!EnumModes(&(*it)))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Name: GetDirectDrawDevices()
// Desc: Retrieves all available DirectDraw devices and stores the information
//       in g_aDevices[]
//-----------------------------------------------------------------------------
bool RtgDisplayModeEnum::EnumDevices()
{
    return DirectDrawEnumerateEx( DDEnumCallbackEx, 
        (LPVOID)this,
        DDENUM_ATTACHEDSECONDARYDEVICES |
        DDENUM_DETACHEDSECONDARYDEVICES |
        DDENUM_NONDISPLAYDEVICES )==DD_OK;
}

//-----------------------------------------------------------------------------
// Name: UpdateModesListBox()
// Desc: Updates the "modes to test" and "all modes" list boxes
//-----------------------------------------------------------------------------
bool RtgDisplayModeEnum::EnumModes( RtgDisplayModeEnum::SDevice* pDevice )
{
    LPDIRECTDRAW7 pDD = NULL;
    HRESULT       hr;    

    // Create a DirectDraw device based using the selected device guid
    if( SUCCEEDED( hr = DirectDrawCreateEx( &(pDevice->guid), (VOID**) &pDD, IID_IDirectDraw7, NULL) ) )
    {
        // Enumerate and display all supported modes along
        // with supported bit depth, and refresh rates 
        // in the "All Modes" listbox
        hr = pDD->EnumDisplayModes( DDEDM_REFRESHRATES, NULL,
            (VOID*) pDevice, EnumAllModesCallback );

        // Release this device
        SAFE_RELEASE( pDD ); 
    }

    return hr==DD_OK;
}




//-----------------------------------------------------------------------------
// Name: DDEnumCallbackEx()
// Desc: Enumerates all available DirectDraw devices
//-----------------------------------------------------------------------------
BOOL WINAPI DDEnumCallbackEx( GUID* pGUID,    
                              LPSTR strDriverDescription, 
                              LPSTR strDriverName,        
                              LPVOID pContext,           
                              HMONITOR hm )       
{
    RtgDisplayModeEnum* pEnumer = (RtgDisplayModeEnum*)pContext;
    HRESULT hr;
    LPDIRECTDRAW pDD = NULL;
   
    // Create a DirectDraw device using the enumerated guid 
    hr = DirectDrawCreateEx( pGUID, (VOID**)&pDD, IID_IDirectDraw7, NULL );

    if( SUCCEEDED(hr) )
    {
        pEnumer->m_listDevices.push_back(RtgDisplayModeEnum::SDevice());
        RtgDisplayModeEnum::SDevice* pDevice = &(pEnumer->m_listDevices.back());

        if( pGUID )
        {
            // Add it to the global storage structure
            pDevice->guid = *pGUID;
        }
        else
        {
            // Clear the guid from the global storage structure
            ZeroMemory( &(pDevice->guid), sizeof(GUID) );
        }

        // Copy the description of the driver into the structure
        lstrcpyn( pDevice->strDescription, strDriverDescription, 256 );
        lstrcpyn( pDevice->strDriverName, strDriverName, 64 );

        // Retrive the modes this device can support
        pDevice->listScreenSize.clear();
        hr = pDD->EnumDisplayModes( 0, NULL, (LPVOID)pDevice, EnumModesCallback );
    
        // Release this device 
        SAFE_RELEASE( pDD );
    }

    // Continue looking for more devices
    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: EnumModesCallback()
// Desc: Enumerates the available modes for the device from which 
//       EnumDisplayModes() was called.  It records the unique mode sizes in 
//       the g_aDevices[g_dwDeviceCount].aModeSize array
//-----------------------------------------------------------------------------
HRESULT WINAPI EnumModesCallback( LPDDSURFACEDESC pddsd,  
                                  LPVOID pContext )
{
    DWORD dwModeSizeX;
    DWORD dwModeSizeY;
    RtgDisplayModeEnum::SDevice* pDevice = (RtgDisplayModeEnum::SDevice*)pContext;

    std::list<RtgDisplayModeEnum::SScreenSize>::iterator it = pDevice->listScreenSize.begin();
    for( ; it!=pDevice->listScreenSize.end(); it++ )
    {
        dwModeSizeX = (*it).dwWidth;
        dwModeSizeY = (*it).dwHeight;

        if ( ( dwModeSizeX == pddsd->dwWidth ) && ( dwModeSizeY == pddsd->dwHeight ) )
        {
            // If this mode has been added, then stop looking
            break;
        }
    }

    // If this mode was not in g_aDevices[g_dwDeviceCount].aModeSize[]
    // then added it. 
    if( it == pDevice->listScreenSize.end() )
    {
        pDevice->listScreenSize.push_back(RtgDisplayModeEnum::SScreenSize());
        pDevice->listScreenSize.back().dwWidth  = pddsd->dwWidth;
        pDevice->listScreenSize.back().dwHeight = pddsd->dwHeight;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
// Name: EnumAllModesCallback()
// Desc: For each mode enumerated, it adds it to the "All Modes" listbox.
//-----------------------------------------------------------------------------
HRESULT WINAPI EnumAllModesCallback( LPDDSURFACEDESC2 pddsd,  
                                     LPVOID pContext )
{
    RtgDisplayModeEnum::SDevice* pDevice = (RtgDisplayModeEnum::SDevice*)pContext;

    std::list<RtgDisplayModeEnum::SScreenSize>::iterator it = pDevice->listScreenSize.begin();
    for( ; it!=pDevice->listScreenSize.end(); it++ )
    {
        RtgDisplayModeEnum::SScreenSize* pScreenSize = &(*it);
        if ( ( pScreenSize->dwWidth == pddsd->dwWidth ) && ( pScreenSize->dwHeight == pddsd->dwHeight ) )
        {
            RtgDisplayModeEnum::SBitCount* pBitCount;
            std::list<RtgDisplayModeEnum::SBitCount>::iterator itBit = pScreenSize->listBitCount.begin();
            for( ; itBit!=pScreenSize->listBitCount.end(); itBit++ )
            {
                pBitCount = &(*itBit);
                if (pBitCount->dwBitCount==pddsd->ddpfPixelFormat.dwRGBBitCount)
                {
                    break;
                }
            }
            if (itBit==pScreenSize->listBitCount.end())
            {
                pScreenSize->listBitCount.push_back(RtgDisplayModeEnum::SBitCount());
                pScreenSize->listBitCount.back().dwBitCount = pddsd->ddpfPixelFormat.dwRGBBitCount;
                pScreenSize->listBitCount.back().listRefreshRate.push_back(pddsd->dwRefreshRate);
            }else
            {
                std::list<DWORD>::iterator itRate = pBitCount->listRefreshRate.begin();
                for( ; itRate!=pBitCount->listRefreshRate.end(); itRate++ )
                {
                    if ((*itRate)==pddsd->dwRefreshRate)
                    {
                        break;
                    }
                }
                if (itBit==pScreenSize->listBitCount.end())
                {
                    pBitCount->listRefreshRate.push_back(pddsd->dwRefreshRate);
                }
            }
        }
    }
    return TRUE;
}


/*


HRESULT ResetDeviceModes( DWORD dwDeviceIndex );
HRESULT OnRefreshRate();
HRESULT PerformDirectDrawRefreshRate( LPDIRECTDRAW7 pDD, SIZE* aTestModes, 
DWORD dwTestModes );



//-----------------------------------------------------------------------------
// Name: ResetDeviceModes()
// Desc: If the user makes a mistake, and accidently says YES when a mode 
//       cannot be seen, or NO when a mode really can be seen (thus ending up 
//       with a lower refresh rate than possible) this allows the user to reset 
//       the test results and try again.
//-----------------------------------------------------------------------------
HRESULT ResetDeviceModes( DWORD dwDeviceIndex )
{
    LPDIRECTDRAW7 pDD = NULL;
    HRESULT       hr;    

    // Create a DirectDraw device based using the selected device guid
    hr = DirectDrawCreateEx( &g_aDevices[dwDeviceIndex].guid, 
        (VOID**) &pDD, IID_IDirectDraw7, NULL);

    if( SUCCEEDED(hr) )
    {
        // Set the cooperative level to normal
        if( SUCCEEDED( hr = pDD->SetCooperativeLevel( g_hDlg, DDSCL_NORMAL ) ) )
        {
            // Clear the previous mode tests
            //
            // We ignore the return code, since we would do nothing different on error returns.
            // Note that most applications would never need to call StartRefreshRate this way.
            // The reset functionality is intended to be used when a user accidentally accepted
            // a mode that didn't actually display correctly.
            pDD->StartModeTest( NULL, 0, 0 );
        }

        // Release this device
        SAFE_RELEASE( pDD ); 
    }

    hr = UpdateModesListBox( dwDeviceIndex );

    return hr;
}



//-----------------------------------------------------------------------------
// Name: OnRefreshRate()
// Desc: User hit the "Test" button
//-----------------------------------------------------------------------------
HRESULT OnRefreshRate() 
{
    HWND hWndModesToTest = GetDlgItem( g_hDlg, IDC_TEST_MODES_LIST );
    DWORD dwSelectCount = (DWORD)SendMessage( hWndModesToTest, LB_GETSELCOUNT, 0, 0 );

    if( dwSelectCount > 0 )
    {
        LPDIRECTDRAW7 pDD = NULL;
        HRESULT       hr;    
        HWND          hWndDeviceList;
        DWORD         dwDeviceIndex;

        // Get the currently selected DirectDraw device
        hWndDeviceList = GetDlgItem( g_hDlg, IDC_DDRAW_DEVICE_LIST );
        dwDeviceIndex = (DWORD)SendMessage( hWndDeviceList, LB_GETCURSEL, 0, 0 );

        // Create a DirectDraw device based using the selected device guid
        if( FAILED( hr = DirectDrawCreateEx( &g_aDevices[dwDeviceIndex].guid, 
                                            (VOID**) &pDD, IID_IDirectDraw7, NULL) ) )
            return hr;

        // This is a good usage of DDSCL_CREATEDEVICEWINDOW: DirectDraw will create a window that covers
        // the monitor, and won't mess around with our dialog box. Any mouse clicks on the cover window
        // will therefore not be received and misinterpreted by the dialog box, since such clicks will
        // be sent to DirectDraw's internal message procedure and therein ignored.

        if( FAILED( hr = pDD->SetCooperativeLevel( g_hDlg,
                                               DDSCL_EXCLUSIVE          | 
                                               DDSCL_FULLSCREEN         |
                                               DDSCL_CREATEDEVICEWINDOW | 
                                               DDSCL_SETFOCUSWINDOW ) ) )
        {
            SAFE_RELEASE( pDD ); 
            return hr;
        }



        SIZE aTestModes[256];
        DWORD dwTestModes = 0;

        // Find out which modes are selected, then just test those
        for( DWORD i = 0; i < g_aDevices[dwDeviceIndex].dwModeCount; i++ )
        {
            if( SendMessage( hWndModesToTest, LB_GETSEL, i, 0 ) )
            {
                // Record the selected modes in aTestModes[]
                aTestModes[dwTestModes] = g_aDevices[dwDeviceIndex].aModeSize[i];
                dwTestModes++;
            }
        }

        // Perform test on each of the selected modes on the selected device
        hr = PerformDirectDrawRefreshRate( pDD, aTestModes, dwTestModes );

        // Release this device
        SAFE_RELEASE( pDD ); 

        switch (hr)
        {
        case DDERR_NOMONITORINFORMATION:
            // No EDID data is present for the current monitor.
            MessageBox(g_hDlg,
                "The current monitor cannot be identified electronically.\n"
                "High refresh rates are not allowed on such monitors, so the test will not be performed.",
                "Testing Will Not Proceed", MB_OK | MB_ICONINFORMATION);
            break;
        case DDERR_NODRIVERSUPPORT:
            // The driver cannot support refresh rate testing.
            MessageBox(g_hDlg,
                "This driver does not support the refresh rate feature of DirectDraw.  Test cannot be performed.",
                "Testing Cannot Proceed", MB_OK | MB_ICONINFORMATION);
            break;
        default:
            if( SUCCEEDED(hr) )
            {
                MessageBox( g_hDlg, TEXT("Mode test completed"), TEXT("Result"), MB_OK );
                break;
            }
            else
            {
                // A StartRefreshRate error occurred.
                MessageBox(g_hDlg,
                "StartRefreshRate returned an unexpected value when called with the DDSMT_ISTESTREQUIRED flag.",
                "StartRefreshRate Error", MB_OK | MB_ICONEXCLAMATION);
                return hr;
            }
        }

        // Update the mode list boxes based on the device selected
        if( FAILED( hr = UpdateModesListBox( dwDeviceIndex ) ) )
            return hr;
    }
    else
    {
        // There weren't any modes selected to test
        MessageBox( g_hDlg, 
                    TEXT("Select one or more modes to test from the list box"), 
                    TEXT("No modes selected"), MB_OK );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetupPrimarySurface()
// Desc: Setups a primary DirectDraw surface
//-----------------------------------------------------------------------------
HRESULT SetupPrimarySurface( LPDIRECTDRAW7 pDD, LPDIRECTDRAWSURFACE7* ppDDS )
{
    DDSURFACEDESC2 ddsd;
 
    ZeroMemory( &ddsd, sizeof(ddsd) );

    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    return pDD->CreateSurface(&ddsd, ppDDS, NULL);
}




//-----------------------------------------------------------------------------
// Name: UpdatePrimarySurface()
// Desc: Fills the primary surface with white, and diplays the timeout value
//       on screen
//-----------------------------------------------------------------------------
HRESULT UpdatePrimarySurface( LPDIRECTDRAWSURFACE7 pDDS, DWORD dwTimeout )
{
    DDBLTFX ddbltfx;
    HDC     hDC;
    char    strTimeout[128];
    RECT    rect;
    HRESULT hr;
  
    // Clear the screen:  
    ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
    ddbltfx.dwSize      = sizeof(ddbltfx);
    ddbltfx.dwFillColor = 0xFFFFFFFF;

    hr = pDDS->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );
    if( FAILED( hr ) )
        return hr;

    // Display the timeout value 
    if( FAILED( hr = pDDS->GetDC( &hDC ) ) )
        return hr;

    GetWindowRect( g_hDlg, &rect );
    wsprintf( strTimeout, TEXT("Press space to accept or escape to reject. ")
              TEXT("%2d seconds until timeout"), dwTimeout );
    DrawText( hDC, strTimeout, strlen(strTimeout), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );

    // Cleanup
    pDDS->ReleaseDC( hDC );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: PerformDirectDrawRefreshRate()
// Desc: Perform the IDirectDraw7::StartRefreshRate and IDirectDraw7::EvaluateMode
//       tests
// Returns: S_OK if no modes needed testing, or all modes tested successfully,
//          informative error code otherwise.
//-----------------------------------------------------------------------------
HRESULT PerformDirectDrawRefreshRate( LPDIRECTDRAW7 pDD, SIZE* aTestModes, 
                                   DWORD dwTestModes )
{
    LPDIRECTDRAWSURFACE7 pDDSPrimary = NULL;
    HRESULT hr;
    MSG     msg;
    DWORD   dwFlags = 0;
    DWORD   dwTimeout;
    BOOL    bMsgReady;

    // First call StartRefreshRate with the DDSMT_ISTESTREQUIRED flag to determine
    // whether the tests can be performed and need to be performed.
    hr = pDD->StartModeTest( aTestModes, dwTestModes, DDSMT_ISTESTREQUIRED);

    switch (hr)
    {
    case DDERR_NEWMODE:
        // DDERR_NEWMODE means that there are modes that need testing.
        break;
    case DDERR_TESTFINISHED:
        // DDERR_TESTFINISHED means that all the modes that we wish to test have already been tested correctly
        return S_OK;
    default:
        //Possible return codes here include DDERR_NOMONITORINFORMATION or DDERR_NODRIVERSUPPORT or
        //other fatal error codes (DDERR_INVALIDPARAMS, DDERR_NOEXCLUSIVEMODE, etc.)
        return hr;
    }

    hr = pDD->StartModeTest( aTestModes, dwTestModes, 0 );
    if( hr == DDERR_TESTFINISHED )
    {
        // The tests completed early, so return
        return S_OK;
    }

    // Create the primary DirectDraw surface
    if( FAILED( SetupPrimarySurface( pDD, &pDDSPrimary ) ) )
        return hr;

    // Loop until the mode tests are complete
    while( TRUE )
    {
        bMsgReady = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

        if( bMsgReady )
        {
            if (msg.message == WM_KEYDOWN)
            {
                switch (msg.wParam)
                {
                case VK_SPACE:
                    dwFlags = DDEM_MODEPASSED;
                    break;

                case VK_ESCAPE:
                    dwFlags = DDEM_MODEFAILED;
                    break;
                }
            }
            else
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
        else
        {
            // This method will only succeed with monitors that contain EDID data. 
            // If the monitor is not EDID compliant, then the method will return 
            // DDERR_TESTFINISHED without testing any modes. If the EDID table does not 
            // contain values higher than 60hz, no modes will tested. Refresh rates 
            // higher than 100 hz will only be tested if the EDID table contains values 
            // higher than 85hz.
            hr = pDD->EvaluateMode(dwFlags, &dwTimeout);

            if( hr == DD_OK )
            {
                if( pDDSPrimary )
                {
                    // Clear the screen, and display the timeout value
                    UpdatePrimarySurface( pDDSPrimary, dwTimeout );
                }
            }
            else if( hr == DDERR_NEWMODE )
            {
                // Cleanup the last DirectDraw surface, and create
                // a new one for the new mode
                SAFE_RELEASE( pDDSPrimary );

                if( FAILED( SetupPrimarySurface( pDD, &pDDSPrimary ) ) )
                    return hr;

                dwFlags = 0;
            }
            else if( hr == DDERR_TESTFINISHED )
            {
                // Test complete, so stop looping
                break;
            }

            Sleep( 100 );
        }
    }

    // Cleanup
    SAFE_RELEASE( pDDSPrimary );

    return S_OK;
}
*/

} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
