#include "framework.h"
#include "Ship.h"
#include "Projectile.h"
#include "Engine.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "Windowscodecs.lib")

Engine::Engine() : m_pDirect2dFactory(NULL), m_pRenderTarget(NULL)
{
    // Constructor
    // Initialize your game elements here

    ship = new Ship();

    shipProjectile = NULL;

    // Initialize invaders
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            Point2D invPos;
            invPos.x = 50 + j * 60;
            invPos.y = 80 + i * 50;
            invaders[i * 10 + j] = new Invader(invPos);
        }
    }
    aliveInvaders = 50;

    invaderProjectile = NULL;

    // Initializes 3 ships representing lives left. 
    //These are purely for drawing the lives left on the screen, we don't actually control them or check for collisions
    lives = 3;
    for (int i = 0; i < lives; i++)
    {
        Ship* lifeShip = new Ship(i);
        lifeShips[i] = lifeShip;
    }

    leftPressed = false;
    rightPressed = false;
    firePressed = false;

    gameOver = false;
    gameWon = false;
}

Engine::~Engine()
{
    // Destructor

    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pRenderTarget);

    // Safe-release your game elements here
    delete ship;

    if (shipProjectile != NULL)
        delete shipProjectile;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            delete invaders[i * 10 + j];
        }
    }

    if (invaderProjectile != NULL)
        delete invaderProjectile;
}

HRESULT Engine::InitializeD2D(HWND m_hwnd)
{
    // Initializes Direct2D, for drawing
    D2D1_SIZE_U size = D2D1::SizeU(RESOLUTION_X, RESOLUTION_Y);
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
    m_pDirect2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
        &m_pRenderTarget
    );

    // Initialize text writing factory and format
    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(m_pDWriteFactory),
        reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
    );

    m_pDWriteFactory->CreateTextFormat(
        L"Verdana",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        60,
        L"", //locale
        &m_pTextFormat
    );

    m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &m_pWhiteBrush
    );

    // Initialize the D2D part of your game elements here

    ship->InitializeD2D(m_pRenderTarget);

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            invaders[i * 10 + j]->InitializeD2D(m_pRenderTarget);
        }
    }

    // Initialize the Direct2D for the "lives" ships
    for (int i = 0; i < lives; i++)
    {
        lifeShips[i]->InitializeD2D(m_pRenderTarget);
    }

    return S_OK;
}

void Engine::KeyUp(WPARAM wParam)
{
    // If keyup, we un-set the keys flags
    // We don't do any logic here, because we want to control the logic in the Logic method
    if (wParam == VK_LEFT)
        leftPressed = false;
    if (wParam == VK_RIGHT)
        rightPressed = false;
    if (wParam == VK_SPACE)
        firePressed = false;
}

void Engine::KeyDown(WPARAM wParam)
{
    // If keyup, we set the keys flags
    // We don't do any logic here, because we want to control the logic in the Logic method
    if (wParam == VK_LEFT)
        leftPressed = true;
    if (wParam == VK_RIGHT)
        rightPressed = true;
    if (wParam == VK_SPACE)
        firePressed = true;
}

void Engine::Logic(double elapsedTime)
{
    // This is the logic part of the engine. Add your game logic here.
    // It runs every frame and it receives the elapsed time between the last frame and this one, in seconds.
    // Use this value for a smooth and consistent movement, regardless of the CPU or graphics speed

    // Ship Logic
    ship->Advance(elapsedTime);
    if (!gameOver || gameWon)
    {
        if (leftPressed)
            ship->GoLeft(elapsedTime);
        if (rightPressed)
            ship->GoRight(elapsedTime);
        if (firePressed)
        {
            if (shipProjectile == NULL)
            {
                shipProjectile = new Projectile(ship->GetPosition(), true);
                shipProjectile->InitializeD2D(m_pRenderTarget);
            }
            firePressed = false;
        }
    }

    // Ship projectile logic
    if (shipProjectile != NULL)
    {
        shipProjectile->Advance(elapsedTime);
        if (shipProjectile->IsOut())
        {
            delete shipProjectile;
            shipProjectile = NULL;
        }
    }

    // Invaders logic
    double lowestInvader = 0;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            invaders[i * 10 + j]->Advance(elapsedTime);
            if (invaders[i * 10 + j]->IsAlive())
            {
                if (lowestInvader < invaders[i * 10 + j]->GetPosition().y)
                {
                    lowestInvader = invaders[i * 10 + j]->GetPosition().y;
                }
            }
        }
    }
    if (lowestInvader >= RESOLUTION_Y - 20)
    {
        gameOver = true;
        gameWon = false;
    }
    if (!gameOver || !gameWon)
    {
        Invader::elapsedStepDelay += elapsedTime;
        // If enough time has passed, we move the invaders
        if (Invader::elapsedStepDelay > Invader::stepDelay) {
            Invader::elapsedStepDelay = 0;

            // Switching between sprites to display
            if (Invader::imageNo == 1)
                Invader::imageNo = 2;
            else
                Invader::imageNo = 1;

            if (Invader::step > 0) // If we are going right
            {
                // We check the most right invader
                double edgePosition = 0;
                for (int i = 0; i < 5; i++)
                {
                    for (int j = 0; j < 10; j++)
                    {
                        if (invaders[i * 10 + j]->IsAlive()) {
                            double yPos = invaders[i * 10 + j]->GetPosition().x;
                            if (yPos > edgePosition)
                                edgePosition = yPos;
                        }
                    }
                }
                // If we went too far right, we move down and reverse direction
                if (edgePosition >= RESOLUTION_X - 50) {
                    for (int i = 0; i < 5; i++)
                    {
                        for (int j = 0; j < 10; j++)
                        {
                            invaders[i * 10 + j]->MoveDown();
                        }
                    }
                    Invader::step = -Invader::step;
                }
                else {
                    // Otherwise, we continue to move right
                    for (int i = 0; i < 5; i++)
                    {
                        for (int j = 0; j < 10; j++)
                        {
                            invaders[i * 10 + j]->MoveRight();
                        }
                    }
                }
            }
            else if (Invader::step < 0) // If we are going left
            {
                // We check the most left invader
                double edgePosition = RESOLUTION_X;
                for (int i = 0; i < 5; i++)
                {
                    for (int j = 0; j < 10; j++)
                    {
                        if (invaders[i * 10 + j]->IsAlive()) {
                            double yPos = invaders[i * 10 + j]->GetPosition().x;
                            if (yPos < edgePosition)
                                edgePosition = yPos;
                        }
                    }
                }
                // If we went too far left, we move down and reverse direction
                if (edgePosition <= 50) {
                    for (int i = 0; i < 5; i++)
                    {
                        for (int j = 0; j < 10; j++)
                        {
                            invaders[i * 10 + j]->MoveDown();
                        }
                    }
                    Invader::step = -Invader::step;
                }
                else {
                    // Otherwise, we continue to move left
                    for (int i = 0; i < 5; i++)
                    {
                        for (int j = 0; j < 10; j++)
                        {
                            invaders[i * 10 + j]->MoveLeft();
                        }
                    }
                }
            }
        }
    }

    // Invader - Projectile interaction
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (shipProjectile != NULL)
            {
                if (invaders[i * 10 + j]->IsAlive())
                {
                    if (invaders[i * 10 + j]->IsHit(shipProjectile->GetPosition()))
                    {
                        invaders[i * 10 + j]->Kill();
                        aliveInvaders--;
                        Invader::stepDelay *= 0.95;
                        delete shipProjectile;
                        shipProjectile = NULL;

                        if (aliveInvaders == 0)
                        {
                            gameOver = true;
                            gameWon = true;
                        }
                    }
                }
            }
        }
    }

    // Invader projectile logic
    if (!gameOver || !gameWon)
    {
        Invader::elapsedFireDelay += elapsedTime;
        if (Invader::elapsedFireDelay > Invader::fireDelay) {
            Invader::elapsedFireDelay = 0;
            if (invaderProjectile == NULL)
            {
                int randomInvader = rand() % aliveInvaders;
                for (int i = 0; i < 5; i++)
                {
                    for (int j = 0; j < 10; j++)
                    {
                        if (invaders[i * 10 + j]->IsAlive()) {
                            if (randomInvader == 0) {
                                invaderProjectile = new Projectile(invaders[i * 10 + j]->GetPosition(), false);
                                invaderProjectile->InitializeD2D(m_pRenderTarget);
                            }
                            randomInvader--;
                        }
                    }
                }
            }
        }
    }
    if (invaderProjectile != NULL)
    {
        invaderProjectile->Advance(elapsedTime);
        if (invaderProjectile->IsOut())
        {
            delete invaderProjectile;
            invaderProjectile = NULL;
        }
    }

    // Ship - Projectile interaction
    if (invaderProjectile != NULL)
    {
        if (ship->IsHit(invaderProjectile->GetPosition()))
        {
            ship->Kill();
            lives--;
            delete invaderProjectile;
            invaderProjectile = NULL;

            if (lives == 0)
            {
                gameOver = true;
                gameWon = false;
            }
        }
    }

}

HRESULT Engine::Draw()
{
    // This is the drawing method of the engine.
    // It runs every frame

    // Draws the elements in the game using Direct2D
    HRESULT hr;

    m_pRenderTarget->BeginDraw();

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());


    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    // Below you can add drawing logic for your game elements

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            invaders[i * 10 + j]->Draw(m_pRenderTarget);
        }
    }

    if (shipProjectile != NULL)
        shipProjectile->Draw(m_pRenderTarget);

    ship->Draw(m_pRenderTarget);

    if (invaderProjectile != NULL)
        invaderProjectile->Draw(m_pRenderTarget);

    // Draws the "lives" ships
    for (int i = 0; i < lives; i++)
    {
        lifeShips[i]->Draw(m_pRenderTarget);
    }

    // Game Over: we draw the "Game Over" or "You Win" texts
    if (gameOver)
    {
        D2D1_RECT_F rectangle2 = D2D1::RectF(0, 0, RESOLUTION_X, RESOLUTION_X);

        if (gameWon)
        {
            m_pRenderTarget->DrawText(
                L"You Win!",
                8,
                m_pTextFormat,
                rectangle2,
                m_pWhiteBrush
            );
        }
        else
        {
            m_pRenderTarget->DrawText(
                L"Game Over!",
                10,
                m_pTextFormat,
                rectangle2,
                m_pWhiteBrush
            );
        }

    }
    
    hr = m_pRenderTarget->EndDraw();

    return S_OK;
}