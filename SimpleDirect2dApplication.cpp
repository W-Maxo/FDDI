#include "SimpleDirect2dApplication.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <windows.h> 
#include <stdio.h> 
#include <time.h>

DemoApp *xapp;

const unsigned int TIMER_2SEC = 1;

DWORD WINAPI ThreadFunction(VOID)
{
	bool a = true;

	while (a) {
		Sleep(25);
		//xapp.OnThread();
	}

	return 0;
}

//
// Provides the entry point to the application.
//
int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
    )
{
    // Ignore the return value because we want to continue running even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            DemoApp app;
			xapp = &app;

			int Code;
			DWORD ThreadId;
			HANDLE hThread;

			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunction, &Code, 0, &ThreadId);

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop(); 
            }
        }
        CoUninitialize();
    }

    return 0;
}


//
// Initialize members.
//
DemoApp::DemoApp() :
    m_hwnd(NULL),
    m_pD2DFactory(NULL),
    m_pWICFactory(NULL),
    m_pDWriteFactory(NULL),
    m_pRenderTarget(NULL),
    m_pTextFormat(NULL),
	m_pTextFormatx(NULL),
    m_pPathGeometry(NULL),
    m_pLinearGradientBrush(NULL),
    m_pBlackBrush(NULL),
    m_pGridPatternBitmapBrush(NULL),
    m_pBitmap(NULL),
    m_pAnotherBitmap(NULL),
	m_pwarning_blue(NULL),
	m_pwarning_red(NULL),
	m_penvelop(NULL),

	m_pYellowGreenBrush(NULL),
    m_pRadialGradientBrush(NULL),
	pos(0),
	Angle(0),

	m_pSilverBrush(NULL),

	timer1(0),
	timer2(0),

	ShMrk(true),
	SendData(false),
	External(true),

	/*Code(1),
	ThreadId(0),
	hThread(0),*/

	m_pAnimationTimer(NULL)
{
}

//
// Release resources.
//
DemoApp::~DemoApp()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pWICFactory);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pTextFormat);
	SafeRelease(&m_pTextFormatx);
    SafeRelease(&m_pPathGeometry);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pBlackBrush);
    SafeRelease(&m_pGridPatternBitmapBrush);
    SafeRelease(&m_pBitmap);
    SafeRelease(&m_pAnotherBitmap);
	SafeRelease(&m_pwarning_blue);
	SafeRelease(&m_pwarning_red);
	SafeRelease(&m_penvelop);
	
	SafeRelease(&m_pYellowGreenBrush);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pRadialGradientBrush);

	SafeRelease(&m_pSilverBrush);

	SafeRelease(&m_pAnimationTimer);

	//KillTimer (m_hWnd,TIMER_25MSEC);
}

DOUBLE DemoApp::RandomFromRange(
    DOUBLE minimum,
    DOUBLE maximum
    )
{
	double tmparray[100];

	for (int i = 0; i < 100; i++) tmparray[i] = minimum + (maximum - minimum) * rand() / RAND_MAX;

	return tmparray[(int)(100 * rand() / RAND_MAX)];
}

void DemoApp::OnThread(VOID)
{
	//pos++;
	//if (pos == 1000) pos = 0;
}

//
// Creates the application window and initializes
// device-independent resources.
//
HRESULT DemoApp::Initialize()
{
    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    HRESULT hr = CreateDeviceIndependentResources();
    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(wcex) };
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = WndProc;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = HINST_THISCOMPONENT;
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcex.lpszClassName = L"D2DFddi";

        RegisterClassEx(&wcex);

        // Create the application window.
        //
        // Because the CreateWindow function takes its size in pixels, we
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;

		#pragma warning(suppress : 4996)
        m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        // Create the application window.
        m_hwnd = CreateWindow(
            L"D2DFddi",
            L"Direct2D FDDI",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(640.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
            );
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }

		SetTimer(m_hwnd, TIMER_2SEC, 1, NULL); 
    }

	srand((unsigned)time(NULL));

	for (int i = 0; i < 8; i++) {
		ActiveC[i] = false;
		DTSend[i] = false;
		Receiver[i] = false;
		Receive[i] = false;
		PacketCount[i] = 0;
		TimeLst[i] = (int)RandomFromRange(250, 2500);
		Errpos[i] = -1;
	}

	//DTSend[6] = true;
	//Receiver[3] = true;

	//Errpos[3] = 0;

    return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
    static const WCHAR msc_fontName[] = L"Verdana";
    static const FLOAT msc_fontSize = 35;
	static const FLOAT msc_fontSizex = 16;

    // Create a Direct2D factory.
    ID2D1GeometrySink *pSink = NULL;
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
    if (SUCCEEDED(hr))
    {
        // Create WIC factory.
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pWICFactory)
            );
    }
    if (SUCCEEDED(hr))
    {
        
        // Create a DirectWrite factory.
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(m_pDWriteFactory),
            reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        hr = m_pDWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSize,
            L"", //locale
            &m_pTextFormat
            );

		hr = m_pDWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSizex,
            L"", //locale
            &m_pTextFormatx
            );
    }
    if (SUCCEEDED(hr))
    {
        // Center the text horizontally and vertically.
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		m_pTextFormatx->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_pTextFormatx->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        
    }

    if (SUCCEEDED(hr))
    {
        // Create a path geometry.
        hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry); 
    }   
    if (SUCCEEDED(hr))
    {
        // Use the geometry sink to write to the path geometry.
        hr = m_pPathGeometry->Open(&pSink);
    }
    if (SUCCEEDED(hr))
    {
        pSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);

        pSink->BeginFigure(
            D2D1::Point2F(0, 0),
            D2D1_FIGURE_BEGIN_FILLED
            );

        pSink->AddLine(D2D1::Point2F(200, 0));

        pSink->AddBezier(
            D2D1::BezierSegment(
                D2D1::Point2F(150, 50),
                D2D1::Point2F(150, 150),
                D2D1::Point2F(200, 200))
            );

        pSink->AddLine(D2D1::Point2F(0, 200));

        pSink->AddBezier(
            D2D1::BezierSegment(
                D2D1::Point2F(50, 150),
                D2D1::Point2F(50, 50),
                D2D1::Point2F(0, 0))
            );

        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

        hr = pSink->Close();
    }
    

    SafeRelease(&pSink);

    return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        // Create a Direct2D render target.
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
            );
        if (SUCCEEDED(hr))
        {
            // Create a black brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                &m_pBlackBrush
                );
        }

		if (SUCCEEDED(hr))
          {
              //// Create an array of gradient stops to put in the gradient stop
              //// collection that will be used in the gradient brush.
              //ID2D1GradientStopCollection *pGradientStops = NULL;
  
              //D2D1_GRADIENT_STOP gradientStops[2];
              //gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::White, 1);
              //gradientStops[0].position = 0.0f;
              //gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Transparent, 1);
              //gradientStops[1].position = 1.0f;
              //// Create the ID2D1GradientStopCollection from a previously
              //// declared array of D2D1_GRADIENT_STOP structs.
              //hr = m_pRenderTarget->CreateGradientStopCollection(
              //    gradientStops,
              //    2,
              //    D2D1_GAMMA_2_2,
              //    D2D1_EXTEND_MODE_CLAMP,
              //    &pGradientStops
              //    );

			  ID2D1GradientStopCollection *pGradientStops = NULL;
            // Create a linear gradient.
            static const D2D1_GRADIENT_STOP stops[] =
            {
                { 0.f,  D2D1::ColorF(D2D1::ColorF::White) },
                { 1.f,  { 1.0f, 1.0f, 1.0f, 0.0f }},
            };

            hr = m_pRenderTarget->CreateGradientStopCollection(
                stops,
                ARRAYSIZE(stops),
                &pGradientStops
                );
  
              // The line that determines the direction of the gradient starts at
              // the upper-left corner of the square and ends at the lower-right corner.
  
              if (SUCCEEDED(hr))
              {
                  hr = m_pRenderTarget->CreateLinearGradientBrush(
                      D2D1::LinearGradientBrushProperties(
                          D2D1::Point2F(100, 0),
						  D2D1::Point2F(100, 200)),
                      pGradientStops,
                      &m_pLinearGradientBrush
                      );
              }

			  if (SUCCEEDED(hr))
			 {
				hr = m_pRenderTarget->CreateSolidColorBrush(
                  D2D1::ColorF(D2D1::ColorF::Silver),
					&m_pSilverBrush
					);
             }
  
              // The center of the gradient is in the center of the box.
              // The gradient origin offset was set to zero(0, 0) or center in this case.
              if (SUCCEEDED(hr))
              {
                  hr = m_pRenderTarget->CreateRadialGradientBrush(
                      D2D1::RadialGradientBrushProperties(
                          D2D1::Point2F(0, 0),
                          D2D1::Point2F(0, 0),
                          0,
                          0),
                      pGradientStops,
                      &m_pRadialGradientBrush
                      );
              }
  
              SafeRelease(&pGradientStops);
		}

        //if (SUCCEEDED(hr))
        //{
        //    ID2D1GradientStopCollection *pGradientStops = NULL;
        //    // Create a linear gradient.
        //    static const D2D1_GRADIENT_STOP stops[] =
        //    {
        //        {   0.f,  { 0.f, 1.f, 1.f, 0.25f }  },
        //        {   1.f,  { 0.f, 0.f, 1.f, 1.f }  },
        //    };

        //    hr = m_pRenderTarget->CreateGradientStopCollection(
        //        stops,
        //        ARRAYSIZE(stops),
        //        &pGradientStops
        //        );
        //    if (SUCCEEDED(hr))
        //    {
        //        hr = m_pRenderTarget->CreateLinearGradientBrush(
        //            D2D1::LinearGradientBrushProperties(
        //                D2D1::Point2F(10, 0),
        //                D2D1::Point2F(10, 20)),
        //            D2D1::BrushProperties(),
        //            pGradientStops,
        //            &m_pLinearGradientBrush
        //            );
        //        pGradientStops->Release();
        //    }
        //}

        // Create a bitmap from an application resource.
        hr = LoadResourceBitmap(
            m_pRenderTarget,
            m_pWICFactory,
            L"SampleImage",
            L"Image",
            100,
            0,
            &m_pBitmap
            );

		hr = LoadResourceBitmap(
            m_pRenderTarget,
            m_pWICFactory,
            L"SampleImage_gr",
            L"Image",
            100,
            0,
            &m_pAnotherBitmap
            );

		hr = LoadResourceBitmap(
            m_pRenderTarget,
            m_pWICFactory,
            L"warning_blue",
            L"Image",
            100,
            0,
            &m_pwarning_blue
            );

		hr = LoadResourceBitmap(
            m_pRenderTarget,
            m_pWICFactory,
            L"warning_red",
            L"Image",
            100,
            0,
            &m_pwarning_red
            );

		hr = LoadResourceBitmap(
            m_pRenderTarget,
            m_pWICFactory,
            L"envelop",
            L"Image",
            100,
            0,
            &m_penvelop
            );

        //if (SUCCEEDED(hr))
        //{
        //    // Create a bitmap by loading it from a file.
        //    hr = LoadBitmapFromFile(
        //        m_pRenderTarget,
        //        m_pWICFactory,
        //        L".\\Computer.ico",
        //        100,
        //        0,
        //        &m_pBitmap
        //        );
        //}

        if (SUCCEEDED(hr))
        {
            hr = CreateGridPatternBrush(m_pRenderTarget, &m_pGridPatternBitmapBrush);
        }
    }

    return hr;
}

//
// Creates a pattern brush.
//
HRESULT DemoApp::CreateGridPatternBrush(
    ID2D1RenderTarget *pRenderTarget,
    ID2D1BitmapBrush **ppBitmapBrush
    )
{
    // Create a compatible render target.
    ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
    HRESULT hr = pRenderTarget->CreateCompatibleRenderTarget(
        D2D1::SizeF(10.0f, 10.0f),
        &pCompatibleRenderTarget
        );
    if (SUCCEEDED(hr))
    {
        // Draw a pattern.
        ID2D1SolidColorBrush *pGridBrush = NULL;
        hr = pCompatibleRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1.0f)),
            &pGridBrush
            );
        if (SUCCEEDED(hr))
        {
            pCompatibleRenderTarget->BeginDraw();
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 10.0f, 1.0f), pGridBrush);
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.1f, 1.0f, 10.0f), pGridBrush);
            pCompatibleRenderTarget->EndDraw();

            // Retrieve the bitmap from the render target.
            ID2D1Bitmap *pGridBitmap = NULL;
            hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);
            if (SUCCEEDED(hr))
            {
                // Choose the tiling mode for the bitmap brush.
                D2D1_BITMAP_BRUSH_PROPERTIES brushProperties =
                    D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP);

                // Create the bitmap brush.
                hr = m_pRenderTarget->CreateBitmapBrush(pGridBitmap, brushProperties, ppBitmapBrush);

                pGridBitmap->Release();
            }

            pGridBrush->Release();
        }

        pCompatibleRenderTarget->Release();
    }

    return hr;
}

//
//  Discard device-specific resources which need to be recreated
//  when a Direct3D device is lost
//
void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBitmap);
    SafeRelease(&m_pBlackBrush);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pAnotherBitmap);
    SafeRelease(&m_pGridPatternBitmapBrush);

	SafeRelease(&m_pYellowGreenBrush);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pRadialGradientBrush);
}

//
// The main window message loop.
//
void DemoApp::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

		timer2 = GetTickCount();

		if ((timer2 - timer1) >= 5) {
			timer1 = timer2;
			//DTSend[6] = true;

			/*for () {
			
			}*/

			if (RandomFromRange(0, 10000) > 9995) {
				bool mrk = true;

				for (int i = 0; i < 8; i++) {
					if (Errpos[i] == 0) mrk = false;
				}

				if (mrk) {
					Errpos[(int)RandomFromRange(0, 8)] = 0;
				}
				else
				{
					for (int i = 0; i < 8; i++) {
						Errpos[i] = -1;
					}
				}
			}

			for (int i = 0; i < 8; i++) {
				if (External) {
					if ((Errpos[i] == 0) && ((Angle == ((7 - i) * 45)))) {
						
						External = false;
					}
				}
				else
				{
					if ((Errpos[i] == 0) && ((Angle == ((7 - i + 1) * 45)))) {
						
						//Angle = (float)((7 - i + 1) * 45);
						External = true;
					}
				}
			}

			if (ShMrk) {
				for (int i = 0; i < 8; i++) {
					if ((360 - Angle == ((7 - i) * 45)) && DTSend[i]) {
						int h;

						do
						{
							h = (int)RandomFromRange(0, 8);
						} while (h == i);

						Receiver[h] = true;

						DTSend[i] = false;
						ActiveC[i] = true;
						ShMrk = false;
						SendData = true;
						PacketCount[i] = (int)RandomFromRange(1, 4);
						break;
					}
				}
			}
			else
			{
				for (int i = 0; i < 8; i++) {
					if ((360 - Angle == ((7 - i) * 45)) && Receiver[i]) {
						SendData = false;
						break;
					}
				}

				for (int i = 0; i < 8; i++) {
					if (!(((7 - i) * 45 - 15)  > (360 - Angle) || (360 - Angle) > ((7 - i) * 45 + 15)) && Receiver[i]) {
						Receive[i] = true;
						break;
					}
					else Receive[i] = false;
				}

				for (int i = 0; i < 8; i++) {
					if ((360 - Angle == ((7 - i) * 45)) && ActiveC[i]) {
						
						PacketCount[i]--;
						if (PacketCount[i] > 0) {
							SendData = true;
						}
						else
						{
							ActiveC[i] = false;
							Receive[i] = false;
							SendData = false;
							ShMrk = true;
							for (int v = 0; v < 8; v++) Receiver[v] = false;
						}

						break;
					}
				}
			}  

			for (int i = 0; i < 8; i++) {
				TimeLst[i]--;

				if (TimeLst[i] <= 0) {
					TimeLst[i] = (int)RandomFromRange(250, 3000);
					if (RandomFromRange(0, 10) > 7) {
						if (!ActiveC[i]) {
							DTSend[i] =true;
						}
					}
				}
			
			};

			if (External) {
				Angle++;
			}
			else
			{
				Angle--;
			}

			if (Angle == 361) Angle = 0;
			if (Angle == -1) Angle = 360;

			OnRender();
		}
    }
}

void DemoApp::DrRRctd(FLOAT l, FLOAT t, FLOAT r, FLOAT b)
{
	// Define a rounded rectangle.
          D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
              D2D1::RectF(l, t, r, b),
              10.f,
              10.f
              );

		  // Draw, then fill the rounded rectangle.
          m_pRenderTarget->DrawRoundedRectangle(roundedRect, m_pBlackBrush, 2.f);
          m_pRenderTarget->FillRoundedRectangle(roundedRect, m_pSilverBrush);
}

HRESULT DemoApp::OnRender()
{

    HRESULT hr = CreateDeviceResources();

    if (SUCCEEDED(hr))
    {
		static const WCHAR sc_fddi[] = L"FDDI";
   
        D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

        m_pRenderTarget->BeginDraw();
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        // Paint a grid background.
        m_pRenderTarget->FillRectangle(
            D2D1::RectF(0.0f, 0.0f, renderTargetSize.width, renderTargetSize.height),
            m_pGridPatternBitmapBrush
            );

		// Translate for the solid color brush.
          //m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(300, 300)));
  
			D2D1_RECT_F rcBrushRect = D2D1::RectF(0.0f, 0.0f, renderTargetSize.width, renderTargetSize.height);

			m_pRadialGradientBrush->SetRadiusX(renderTargetSize.height + 100);
			m_pRadialGradientBrush->SetRadiusY(renderTargetSize.width + 100);

			//m_pRadialGradientBrush->SetGradientOriginOffset(D2D1::Point2(100.f, 100.f));

			const D2D1_POINT_2F center = D2D1::Point2(renderTargetSize.width / 2, renderTargetSize.height / 2);
			m_pRadialGradientBrush->SetCenter(center);

          // Demonstrate a solid color brush.
          m_pRenderTarget->FillRectangle(&rcBrushRect, m_pRadialGradientBrush); 
          //m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1, NULL);


		const D2D1_ELLIPSE ellipse = D2D1::Ellipse(center,
												   renderTargetSize.height / 2 - 103, // radius X
                                                   renderTargetSize.height / 2 - 103); // radius Y

		const D2D1_ELLIPSE ellipsesm = D2D1::Ellipse(center,
												   renderTargetSize.height / 2 - 80, // radius X
                                                   renderTargetSize.height / 2 - 80); // radius Y

		m_pRenderTarget->DrawEllipse(&ellipsesm,
									 m_pBlackBrush,
									 3.0f); // stroke width
		m_pRenderTarget->DrawEllipse(&ellipse,
									 m_pBlackBrush,
									 3.0f); // stroke width

		D2D1_ELLIPSE   ellipsex;

		/*float xpos = pos * (float)0.0015;

		float x = center.x + (renderTargetSize.height / 2 - 80) * cos(5 * (float)M_PI * (float)xpos);
		float y = center.y + (renderTargetSize.height / 2 - 80) * sin(5 * (float)M_PI * (float)xpos);*/
  
		m_pRenderTarget->SetTransform(
			D2D1::Matrix3x2F::Rotation(
				Angle,
				D2D1::Point2F(
					renderTargetSize.width / 2,
					renderTargetSize.height / 2))
        );

		m_pRenderTarget->DrawText(
            sc_fddi,
            ARRAYSIZE(sc_fddi) - 1,
            m_pTextFormat,
            D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
            m_pBlackBrush
            );

		float rds;

		if (External) {
			rds = 80;
		}
		else
		{
			rds = 103;
		}

		m_pRenderTarget->SetTransform(
			D2D1::Matrix3x2F::Rotation(
				360 - Angle,
				D2D1::Point2F(
					renderTargetSize.width / 2,
					renderTargetSize.height / 2))
        );

		ellipsex = D2D1::Ellipse(D2D1::Point2F(center.x - 3, center.y - renderTargetSize.height / 2 + rds), 7, 7);

		if (ShMrk) {
			m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
		}
		else
		{
			if (SendData) {
				m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Lime));
			}
			else
			{
				m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Yellow));
			}
		}

		m_pRenderTarget->FillEllipse(ellipsex, m_pBlackBrush);
		m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

		m_pRenderTarget->DrawEllipse(ellipsex, m_pBlackBrush, 1, NULL);

        D2D1_SIZE_F size = m_pBitmap->GetSize();

        size = m_pAnotherBitmap->GetSize();
		
		int sk = 0;

		for (float z = 315; z >= 0; z-=45) {
			m_pRenderTarget->SetTransform(
				D2D1::Matrix3x2F::Rotation(
					z,
					D2D1::Point2F(
						renderTargetSize.width / 2,
						renderTargetSize.height / 2))
            );

			DrRRctd(center.x - 10,
				center.y - renderTargetSize.height / 2 + 72,
				center.x + 10,
				center.y - renderTargetSize.height / 2 + 72 + 42);

			m_pRenderTarget->DrawLine(D2D1::Point2(center.x, center.y - renderTargetSize.height / 2 + 65), // start
                   D2D1::Point2(center.x, center.y - renderTargetSize.height / 2 + 70), // end
                   m_pBlackBrush,
                   3.0f); // stroke width

			if (ActiveC[sk]) {
				m_pRenderTarget->DrawBitmap(
					m_pBitmap,
					D2D1::RectF(
						center.x - 32,
						center.y - renderTargetSize.height / 2,
						center.x + 32,
						center.y - renderTargetSize.height / 2 + 64)
				);

				//WCHAR *sc_fx;  //sc_fx = L"4";

				WCHAR buffer[65];


				_itow_s(PacketCount[sk], buffer, 10);

				/*m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));*/

				/*m_pRenderTarget->DrawText(
					buffer,
					wcslen(buffer),
					m_pTextFormatx,
					D2D1::RectF(center.x - 32,
								center.y - renderTargetSize.height / 2,
								center.x + 32,
								center.y - renderTargetSize.height / 2 + 64),
					m_pBlackBrush
				);*/

				m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
			}
			else
			{
				m_pRenderTarget->DrawBitmap(
					m_pAnotherBitmap,
					D2D1::RectF(
						center.x - 32,
						center.y - renderTargetSize.height / 2,
						center.x + 32,
						center.y - renderTargetSize.height / 2 + 64)
				);
			}

			if (DTSend[sk]) {
				m_pRenderTarget->DrawBitmap(
					m_pwarning_blue,
					D2D1::RectF(
						center.x - 24,
						center.y - renderTargetSize.height / 2 + 15,
						center.x + 24,
						center.y - renderTargetSize.height / 2 + 48 + 15)
				);
			}

			if (Receive[sk]) {
				m_pRenderTarget->DrawBitmap(
					m_penvelop,
					D2D1::RectF(
						center.x - 24,
						center.y - renderTargetSize.height / 2 + 15,
						center.x + 24,
						center.y - renderTargetSize.height / 2 + 48 + 15)
				);
			}

			sk++;
		}


		for (int i = 0; i < 8; i++) {
			if (Errpos[i] == 0) {
				m_pRenderTarget->SetTransform(
					D2D1::Matrix3x2F::Rotation(
						(float)(((float)i) * 45.0 + 22.5),
						D2D1::Point2F(
							renderTargetSize.width / 2,
							renderTargetSize.height / 2))
				);

				m_pRenderTarget->DrawBitmap(
						m_pwarning_red,
						D2D1::RectF(
							center.x - 24,
							center.y - renderTargetSize.height / 2 + 68,
							center.x + 24,
							center.y - renderTargetSize.height / 2 + 48 + 68)
				);
			}
		}

        /*m_pRenderTarget->SetTransform(
            D2D1::Matrix3x2F::Rotation(
                Angle,
                D2D1::Point2F(
                    renderTargetSize.width / 2,
                    renderTargetSize.height / 2))
            );*/

        

        //
        // Reset back to the identity transform
        //
       /* m_pRenderTarget->SetTransform(
            D2D1::Matrix3x2F::Translation(0, renderTargetSize.height - 200)
            );*/

		/*m_pRenderTarget->DrawText(
            sc_fddi,
            ARRAYSIZE(sc_fddi) - 1,
            m_pTextFormat,
            D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
            m_pBlackBrush
            );*/

        // Fill the hour glass geometry with a gradient.
        //m_pRenderTarget->FillGeometry(m_pPathGeometry, m_pLinearGradientBrush);

		// Translate for the radial gradient brush.
          //m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(1, 5)));
  
          // Draw a caption.
          /*m_pRenderTarget->DrawText(
              szRadialGradientText,
              ARRAYSIZE(szRadialGradientText) - 1,
              m_pTextFormat,
              &rcTextRect,
              m_pBlackBrush
              );*/



        //m_pRenderTarget->SetTransform(
            //D2D1::Matrix3x2F::Translation(renderTargetSize.width - 250, 0)
            //);

        // Fill the hour glass geometry with a gradient.
        //m_pRenderTarget->FillGeometry(m_pPathGeometry, m_pRadialGradientBrush);

        hr = m_pRenderTarget->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
        {
            hr = S_OK;
            DiscardDeviceResources();
        }
    }

    return hr;
}

//
//  If the application receives a WM_SIZE message, this method
//  resize the render target appropriately.
//
void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;

        // Note: This method can fail, but it's okay to ignore the
        // error here -- it will be repeated on the next call to
        // EndDraw.
        m_pRenderTarget->Resize(size);
    }
}

//
// The window message handler.
//
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pDemoApp)
            );

        result = 1;
    }
    else
    {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                )));

        bool wasHandled = false;

        if (pDemoApp)
        {
            switch (message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pDemoApp->OnResize(width, height);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DISPLAYCHANGE:
                {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
                {
                    pDemoApp->OnRender();

                    ValidateRect(hwnd, NULL);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }
                result = 1;
                wasHandled = true;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

//
// Creates a Direct2D bitmap from a resource in the
// application resource file.
//
HRESULT DemoApp::LoadResourceBitmap(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap **ppBitmap
    )
{
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void *pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);
    HRESULT hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        // Load the resource.
        imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageResDataHandle ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Lock it to get a system memory pointer.
        pImageFile = LockResource(imageResDataHandle);

        hr = pImageFile ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Calculate the size.
        imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageFileSize ? S_OK : E_FAIL;
        
    }
    if (SUCCEEDED(hr))
    {
          // Create a WIC stream to map onto the memory.
        hr = pIWICFactory->CreateStream(&pStream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                            pSource,
                            destinationWidth,
                            destinationHeight,
                            WICBitmapInterpolationModeCubic
                            );
                    if (SUCCEEDED(hr))
                    {
                        hr = pConverter->Initialize(
                            pScaler,
                            GUID_WICPixelFormat32bppPBGRA,
                            WICBitmapDitherTypeNone,
                            NULL,
                            0.f,
                            WICBitmapPaletteTypeMedianCut
                            );
                    }
                }
            }
        }
        else
        {
                    
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
                );
        }
    }
    if (SUCCEEDED(hr))
    {
        //create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
            );
    
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}

//
// Creates a Direct2D bitmap from the specified
// file name.
//
HRESULT DemoApp::LoadBitmapFromFile(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR uri,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap **ppBitmap
    )
{
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
        );
        
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {

        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);

    }
 
 
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                            pSource,
                            destinationWidth,
                            destinationHeight,
                            WICBitmapInterpolationModeCubic
                            );
                }
                if (SUCCEEDED(hr))
                {
                    hr = pConverter->Initialize(
                        pScaler,
                        GUID_WICPixelFormat32bppPBGRA,
                        WICBitmapDitherTypeNone,
                        NULL,
                        0.f,
                        WICBitmapPaletteTypeMedianCut
                        );
                }
            }
        }
        else // Don't scale the image.
        {
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
                );
        }
    }
    if (SUCCEEDED(hr))
    {
    
        // Create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
            );
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}
