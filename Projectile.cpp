#include "framework.h"
#include "Engine.h"
#include "Projectile.h"

Projectile::Projectile(Point2D startPosition, boolean up) : m_pRedBrush(NULL)
{
	// Initilize a projectile from received parameter
	position = startPosition;

	// Initialize the speed
	speed.x = 0;
	if (up)
		speed.y = - PROJECTILE_SPEED;
	else
		speed.y = 0.5 * PROJECTILE_SPEED;

}

Projectile::~Projectile()
{
	SafeRelease(&m_pRedBrush);
}

void Projectile::InitializeD2D(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	// Creates a red brush for drawing
	m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red),
		&m_pRedBrush
	);
}

void Projectile::Advance(double elapsedTime)
{
	// Projectile moves
	position.x += elapsedTime * speed.x;
	position.y += elapsedTime * speed.y;
}

bool Projectile::IsOut()
{
	// Returns true if the projectile is out of the screen area so we can remove it
	if (position.y < -10 || position.y > RESOLUTION_Y + 10)
	{
		return true;
	}
	return false;
}

void Projectile::Draw(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	// Draws the ball using Direct2D
	D2D1_ELLIPSE ellipseBall = D2D1::Ellipse(
		D2D1::Point2F(position.x, position.y),
		5, 5
	);
	m_pRenderTarget->FillEllipse(&ellipseBall, m_pRedBrush);
}

Point2D Projectile::GetPosition()
{
	return position;
}