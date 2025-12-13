#pragma once
#include <cstdint>
#include "Module.h"

class Time : public Module {
public:
    inline static float deltaTime = 0.0f;        // dt usado por el juego (Game Clock)
    inline static double time = 0.0;            // segundos acumulados del Game Clock
    inline static float timeScale = 1.0f;        // multiplicador de velocidad del Game Clock
    inline static bool paused = false;            // pause flag (afecta Game Clock)

    // Real Time Clock
    inline  static float realDeltaTime = 0.0f;    // tiempo real del último frame
    inline  static double realTime = 0.0;        // segundos reales desde arranque

    // Fixed step 
    inline  static float fixedDeltaTime = 1.0f / 60.0f;;
    inline static bool requestStep = false;

    // Limits
    inline static float minTimeScale = 0.0f;
    inline static float maxTimeScale = 5.0f;

    // Inicializar valores por defecto
    static void Init(float fixedStep = 1.0f / 60.0f);

    // Llamar cada frame con el delta real (en segundos)
    static void Update(float realDT);

    // Helpers
    static void SetTimeScale(float scale);
    static void Pause();
    static void Resume();
    static void TogglePause();
    static void RequestStepOnce(); // pedir un tick
};