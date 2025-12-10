#include "Time.h"
#include <algorithm>


void Time::Init(float fixedStep)
{
    fixedDeltaTime = fixedStep;
    timeScale = 1.0f;
    paused = false;
    requestStep = false;
    deltaTime = 0.0f;
    realDeltaTime = 0.0f;
    time = 0.0;
    realTime = 0.0;
}

void Time::Update(float realDT)
{
    realDeltaTime = realDT;
    realTime += realDeltaTime;

    if (!paused)
    {
        deltaTime = realDeltaTime * timeScale;
        time += deltaTime;
        requestStep = false;
        return;
    }

    if (requestStep)
    {
        deltaTime = fixedDeltaTime * timeScale;
        time += deltaTime;
        requestStep = false;
    }
    else
    {
        deltaTime = 0.0f;
    }
}

// Ajusta la velocidad del tiempo del juego
void Time::SetTimeScale(float scale)
{
    if (scale < minTimeScale) scale = minTimeScale;
    if (scale > maxTimeScale) scale = maxTimeScale;
    timeScale = scale;
}

void Time::Pause()
{
    paused = true;
}

void Time::Resume()
{
    paused = false;
}

void Time::TogglePause()
{
    paused = !paused;
}

void Time::RequestStepOnce()
{
    requestStep = true;
}
