#include "framework.h"
#include "Point2D.h"
#include "Engine.h"
#include "Invader.h"

// Statics used to control all invaders at the same time
ID2D1Bitmap* Invader::ppBitmap1 = NULL;
ID2D1Bitmap* Invader::ppBitmap2 = NULL;
double Invader::elapsedStepDelay = 0;
double Invader::stepDelay = 0.25;
int Invader::step = 5;
int Invader::imageNo = 1;
double Invader::elapsedFireDelay = 0;
double Invader::fireDelay = 2.5;

Invader::Invader(Point2D initPosition) : m_pRedBrush(NULL)
{
	// Sets the invader's position
	position = initPosition;

	lifeStage = 1; // 1 = alive, 2 = exploding, 3 = dead
}

Invader::~Invader()
{
}

void Invader::InitializeD2D(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	// Creates a green brush for drawing the ship
	m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red),
		&m_pRedBrush
	);


	// We load the 2 images for invaders
	if (ppBitmap1 == NULL) {
		// I'm using a static because I don't want to load the same image multiple times
		IWICBitmapDecoder* pDecoder = NULL;
		IWICBitmapFrameDecode* pSource = NULL;
		IWICFormatConverter* pConverter = NULL;
		IWICImagingFactory* pIWICFactory = NULL;

		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&pIWICFactory));

		HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
			L"invader1.jpg",
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
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
			);
		}

		if (SUCCEEDED(hr))
		{

			// Create a Direct2D bitmap from the WIC bitmap.
			hr = m_pRenderTarget->CreateBitmapFromWicBitmap(
				pConverter,
				NULL,
				&ppBitmap1
			);
		}

		SafeRelease(&pIWICFactory);
		SafeRelease(&pDecoder);
		SafeRelease(&pSource);
		SafeRelease(&pConverter);
	}

	if (ppBitmap2 == NULL) {
		// I'm using a static because I don't want to load the same image multiple times
		IWICBitmapDecoder* pDecoder = NULL;
		IWICBitmapFrameDecode* pSource = NULL;
		IWICFormatConverter* pConverter = NULL;
		IWICImagingFactory* pIWICFactory = NULL;

		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&pIWICFactory));

		HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
			L"invader2.jpg",
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
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
			);
		}

		if (SUCCEEDED(hr))
		{

			// Create a Direct2D bitmap from the WIC bitmap.
			hr = m_pRenderTarget->CreateBitmapFromWicBitmap(
				pConverter,
				NULL,
				&ppBitmap2
			);
		}

		SafeRelease(&pIWICFactory);
		SafeRelease(&pDecoder);
		SafeRelease(&pSource);
		SafeRelease(&pConverter);
	}

}

void Invader::MoveLeft()
{
	// Moves the invader left
	position.x -= abs(Invader::step);
}

void Invader::MoveRight()
{
	// Moves the invader right
	position.x += abs(Invader::step);
}

void Invader::MoveDown()
{
	// Moves the invader down
	position.y += 2 * abs(Invader::step);
}


void Invader::Kill()
{
	lifeStage = 2; // Explosion mode
	explosionTime = 0;
}

void Invader::Advance(double elapsedTime)
{
	if (lifeStage == 2) // In explosion mode
	{
		explosionTime += elapsedTime;
		if (explosionTime > 0.5)
			lifeStage = 3; // Completely dead
	}
}

void Invader::Draw(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	
	if (lifeStage == 1)
	{
		D2D1_RECT_F rectangle1 = D2D1::RectF(
			position.x - 22.5, position.y - 20,
			position.x + 22.5, position.y + 20
		);

		// imageNo can be 1 or 2, and that decides which sprite we draw, to look like an animation
		if (imageNo == 1)
			m_pRenderTarget->DrawBitmap(Invader::ppBitmap1, rectangle1);
		else
			m_pRenderTarget->DrawBitmap(Invader::ppBitmap2, rectangle1);
	}

	if (lifeStage == 2)
	{
		// If it's in explosion mode, we draw 9 red points moving away from the center, simulating an explosion
		int angleStep = 360 / 9;
		for (int i = 0; i < 9; i++)
		{
			D2D1_ELLIPSE ellipseBall = D2D1::Ellipse(
				D2D1::Point2F(position.x + (explosionTime * 120) * sin(i * angleStep * 3.14159 / 180), position.y - (explosionTime * 120) * cos(i * angleStep * 3.14159 / 180)),
				4, 4
			);
			m_pRenderTarget->FillEllipse(&ellipseBall, m_pRedBrush);
		}
	}

}

Point2D Invader::GetPosition()
{
	return position;
}

bool Invader::IsAlive()
{
	return lifeStage == 1;
}

bool Invader::IsHit(Point2D projectile)
{
	if (projectile.x > position.x - 22.5 && projectile.x < position.x + 22.5)
	{
		if (projectile.y > position.y - 20 && projectile.y < position.y + 20)
		{
			return true;
		}
	}
	return false;
}