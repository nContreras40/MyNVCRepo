
/*
 * Nathan Contreras
 * GAME 2332-001
 * Homework PG 064 P 02
 * Move a bitmap around the screen.  Majority of code is from the book, with the exception of the code
 * I placed in to move the bitmap around the screen.
 */

#include <windows.h>
#include <iostream>
#include <time.h>

using namespace std;

const string appTitleStr = "Game Loop";
HDC   ourDevContext;
HWND  g_hWindow;
bool  gameOver = false;


// my globals
int x = 0;
int y = 0;

// Cache some things so that they don't have to be reloaded...
HBITMAP image = NULL;
HDC hImageDC = NULL;
BITMAP g_bmInfo;

//
// Pre-gameLoop Initialization 
//    Allocation of resources, graphics objects, etc. that we'll use during the 
//    gameloop
//
bool Game_Init()
{
    //start up the random number generator
    srand((UINT)time(NULL));

    image = (HBITMAP)LoadImage(0,"c.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

    //read the bitmap's properties
    GetObject(image, sizeof(BITMAP), &g_bmInfo);

    //create a device context for the bitmap
    hImageDC = CreateCompatibleDC(ourDevContext);
    SelectObject(hImageDC, image);

    return 1;
}

//
//  The gameLoop "Run" function - called whenever the windows app is idle, thus
//  we can think of this function as being repeatedly called within loop that 
//  ends only when the user closes the windows App.
//
void Game_Run()
{
    static DWORD lastTickCount = 0;
    static DWORD msDelay = 33;     // 30 "fps" (1/30 == .033 ms per iteration)
    static int lastX = -1, lastY = -1;

    if ( (GetTickCount() - lastTickCount) < msDelay ) 
       return;
   
    if ( gameOver )
       return;

    lastTickCount = GetTickCount();

    // get the window's current RECTangle structure to determine its position/dimensions
    // so we can calculate random locations to display our bitmap
    RECT rect;
    GetClientRect(g_hWindow, &rect);
    
    // int x = rand() % (rect.right - rect.left);
    // int y = rand() % (rect.bottom - rect.top);

    if ( x < rect.right-150 && y <= rect.top )
    {
       x+=5;
    }
    else if ( x >= rect.right-150 && y <= rect.bottom-100 )
    {
       y+=5;
    }
    else if ( x >= rect.left && y >= rect.bottom-100 )
    {
       x-=5;
    }
    else if ( x <= rect.left && y >= rect.top )
    {
       y-=5;
    }
    
    // one-shot: First time through we skip this, thereafter, 
    // we call InvalidateRect() for the window region where we drew the
    // bitmap during our last iteration, forcing the last draw to be erased
    
    //if ((lastX + lastY) >= 0)
    //{
    //   RECT r;
    //   r.top = lastY;
    //   r.left = lastX;
    //   r.bottom = lastY + g_bmInfo.bmHeight;
    //   r.right = lastX + g_bmInfo.bmWidth;
    //   InvalidateRect( g_hWindow, &r, TRUE );
    //}
    
    lastX = x;
    lastY = y;
    
    //Blit the pre-loaded image to the destination DC
    BitBlt( ourDevContext,          // destination (our window's) device context
            x, y,                   // random x,y location on destination 
            g_bmInfo.bmWidth, 
            g_bmInfo.bmHeight,      // source bitmap dimensions (cached when loaded)
            hImageDC,               // source bitmap DC (created/cached when loaded)
            0, 0,                   // origin within source bitmap
            SRCCOPY );              // blit method
            // SRCCOPY, SRCERASE, SRCINVERT, SRCPAINT, SRCAND, MERGEPAINT,

    //RECT r;
    //r.top = lastY;
    //r.left = lastX;
    //r.bottom = lastY + g_bmInfo.bmHeight;
    //r.right = lastX + g_bmInfo.bmWidth;
    //InvalidateRect( g_hWindow, &r, TRUE );
}

//
// Post-gameLoop processing - cleanup resources, objects, etc.
//
void Game_End()
{
 
    // Release the cached DC & HBITMAP from our pre-loaded image  
    DeleteDC(hImageDC);
    DeleteObject((HBITMAP)image);

    //free the window device context
    ReleaseDC(g_hWindow, ourDevContext);
}

/**
 ** Window callback function
 **/
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) 
    {
        case WM_DESTROY:
            gameOver = true;
            PostQuitMessage(0); // causes WM_QUIT to be posted to the message loop
            break;
        case WM_CLOSE:
           break;
        case WM_TIMER:
           break;
        case WM_PAINT:
           break;
        default:
           break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

/**
 ** MyRegiserClass function sets program window properties
 **/
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    //create the window class structure
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX); 

    //fill the struct with info
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WinProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = appTitleStr.c_str();
    wc.hIconSm       = NULL;

    //set up the window with the class info
    return RegisterClassEx(&wc);
}

/**
 ** Helper function to create the window and refresh it
 **/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    //create a new window
    g_hWindow = CreateWindow(
       appTitleStr.c_str(),              //window class
       appTitleStr.c_str(),              //title bar
       WS_OVERLAPPEDWINDOW,   //window style
       CW_USEDEFAULT,         //x position of window
       CW_USEDEFAULT,         //y position of window
       600,                   //width of the window
       400,                   //height of the window
       NULL,                  //parent window
       NULL,                  //menu
       hInstance,             //application instance
       NULL);                 //window parameters

    //Error? Bail.
    if (g_hWindow == 0) 
       return FALSE;

    //display the window
    ShowWindow(g_hWindow, nCmdShow);
    UpdateWindow(g_hWindow);

    //cache the device context associated with our window for drawing
    ourDevContext = GetDC(g_hWindow);

    return TRUE;
}

/**
 ** Entry point function
 **/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Standard Windows startup code: Create window class...
    MyRegisterClass( hInstance );
    
    // Standard Windows startup code: Create a window... 
    if ( !InitInstance( hInstance, nCmdShow ) ) 
       return 0;

    // Something different :-) Initialize the game
    if (!Game_Init()) 
       return 0;

    //
    // Here's the action:  The main message loop is modified to use PeekMessage
    // rather than GetMessage(), allowing us to run the gameLoop whenever the Windows
    // application is "idle" -- which is just about always.  
    // Contrast with standard Windows message pump:
    //
    //   while ( GetMessage( &msg, NULL, 0, 0 ) > 0 )
    //   {
    //      TranslateMessage(&msg);
    //      DispatchMessage(&msg);
    //   }
    //

    // main message loop
    MSG msg;
    msg.message = 0;
    while (msg.message != WM_QUIT)     // vs. while ( !gameOver )
    {
        //process Windows events
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        //do one iteration of the gameloop
        Game_Run();
    }

    //Cleanup the gameloop
    Game_End();

    return msg.wParam;
}


