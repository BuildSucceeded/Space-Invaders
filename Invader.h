#pragma once

#include "resource.h"
#include "Point2D.h"

class Invader
{
public:
	Invader(Point2D initPosition);
	~Invader();

	void InitializeD2D(ID2D1HwndRenderTarget* m_pRenderTarget);
	void MoveLeft();
	void MoveRight();
	void MoveDown();
	void Kill();
	void Advance(double elapsedTime);

	void Draw(ID2D1HwndRenderTarget* m_pRenderTarget);

	Point2D GetPosition();
	bool IsAlive();
	bool IsHit(Point2D projectile);

	static double elapsedStepDelay;
	static double stepDelay;
	static int step;
	static int imageNo;
	static double elapsedFireDelay;
	static double fireDelay;

private:
	int lifeStage;
	double explosionTime;

	Point2D position;


	ID2D1SolidColorBrush* m_pRedBrush;
	static ID2D1Bitmap* ppBitmap1;
	static ID2D1Bitmap* ppBitmap2;

};