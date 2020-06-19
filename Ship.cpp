#include "framework.h"
#include "Point2D.h"
#include "Engine.h"
#include "Ship.h"

Ship::Ship() : m_pGreenBrush(NULL), m_pRedBrush(NULL)
{
	// Resets the ship in the center of the screen
	Reset();
}

Ship::Ship(int lifeNo) : m_pGreenBrush(NULL), m_pRedBrush(NULL)
{
	// This is used for the "lives" ships, just to display the number of lives left in the corner of the screen

	Reset();

	// Sets position in the corner of the screen
	position.x = 20 + lifeNo * 45;
	position.y = 30;
}

Ship::~Ship()
{
	SafeRelease(&m_pGreenBrush);
	SafeRelease(&m_pRedBrush);
}

void Ship::Reset()
{
	// Position in the center of the screen
	position.x = RESOLUTION_X / 2;
	position.y = RESOLUTION_Y - 10;

	lifeStage = 1; // 1 = alive, 2 = exploding, 3 = dead
}

void Ship::InitializeD2D(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	// Creates a green brush for drawing the ship
	m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Green),
		&m_pGreenBrush
	);
	// Creates a red brush for drawing the explosion
	m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red),
		&m_pRedBrush
	);
}

void Ship::GoLeft(double elapsedTime)
{
	// Moves the ship left
	if (lifeStage == 1) {
		position.x -= elapsedTime * 300;
		if (position.x < 10)
		{
			position.x = 10;
		}
	}
}

void Ship::GoRight(double elapsedTime)
{
	// Moves the ship right
	if (lifeStage == 1) {
		position.x += elapsedTime * 300;
		if (position.x > RESOLUTION_X - 10)
		{
			position.x = RESOLUTION_X - 10;
		}
	}
}

void Ship::Advance(double elapsedTime)
{
	if (lifeStage == 2) // If we are in explosion mode
	{
		explosionTime += elapsedTime;
		if (explosionTime > 0.5)
		{
			Reset();
		}
	}
}

void Ship::Kill()
{
	lifeStage = 2; // Turn into explosion mode
	explosionTime = 0;
}

bool Ship::IsHit(Point2D projectile)
{
	if (projectile.x > position.x - 20 && projectile.x < position.x + 20)
	{
		if (projectile.y > position.y - 20 && projectile.y < position.y + 10)
		{
			return true;
		}
	}
	return false;
}

void Ship::Draw(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	if (lifeStage == 1)
	{
		// Calculate the head position and the 2 sides based on position and rotation
		D2D1_POINT_2F headPoint = D2D1::Point2F(position.x, position.y - 20);
		D2D1_POINT_2F leftPoint = D2D1::Point2F(position.x - 20, position.y + 10);
		D2D1_POINT_2F rightPoint = D2D1::Point2F(position.x + 20, position.y + 10);

		ID2D1PathGeometry* clPath;
		ID2D1Factory* factory;
		m_pRenderTarget->GetFactory(&factory);
		factory->CreatePathGeometry(&clPath);

		ID2D1GeometrySink* pclSink;
		clPath->Open(&pclSink);
		pclSink->SetFillMode(D2D1_FILL_MODE_WINDING);
		pclSink->BeginFigure(headPoint, D2D1_FIGURE_BEGIN_FILLED);
		pclSink->AddLine(leftPoint);
		pclSink->AddLine(rightPoint);
		pclSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		pclSink->Close();
		m_pRenderTarget->FillGeometry(clPath, m_pGreenBrush);

		SafeRelease(&pclSink);
		SafeRelease(&clPath);
	}

	if (lifeStage == 2)
	{
		// If it's in explosion mode, we draw 9 red points moving away from the center, simulating an explosion
		int angleStep = 360 / 18;
		for (int i = 0; i < 18; i++)
		{
			D2D1_ELLIPSE ellipseBall = D2D1::Ellipse(
				D2D1::Point2F(position.x + (explosionTime * 120) * sin(i * angleStep * 3.14159 / 180), position.y - (explosionTime * 120) * cos(i * angleStep * 3.14159 / 180)),
				4, 4
			);
			m_pRenderTarget->FillEllipse(&ellipseBall, m_pRedBrush);
		}
	}

}

Point2D Ship::GetPosition()
{
	return position;
}