#pragma once

#include "resource.h"
#include "Ship.h"
#include "Projectile.h"
#include "Invader.h"

#define RESOLUTION_X 800
#define RESOLUTION_Y 600

class Engine
{
public:
	Engine();
	~Engine();

	HRESULT InitializeD2D(HWND m_hwnd);
	void KeyUp(WPARAM wParam);
	void KeyDown(WPARAM wParam);
	void Logic(double elapsedTime);
	HRESULT Draw();

private:

	Ship* ship;
	Projectile* shipProjectile;
	Invader* invaders[50];
	int aliveInvaders;
	Projectile* invaderProjectile;

	int lives;
	Ship* lifeShips[3];

	bool leftPressed;
	bool rightPressed;
	bool firePressed;

	bool gameOver;
	bool gameWon;

	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	ID2D1SolidColorBrush* m_pWhiteBrush;

	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
};

