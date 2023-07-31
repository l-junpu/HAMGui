#pragma once
#ifndef HAM_CLOCK_H
#define HAM_CLOCK_H

#include <chrono>
#include <thread>

namespace hammer
{
    class Clock
    {
    public:

        using HRClock   = std::chrono::high_resolution_clock;
        using FDuration = std::chrono::duration<float>;

        Clock( bool LimitFrames = false
             , int TargetFPS    = 120 );

        void StartFrame( void ) noexcept;
        void EndFrame  ( void ) noexcept;

        float GetDt    ( void ) const noexcept;
        float GetFPS   ( void ) noexcept;

    private:

        bool                                           m_LimitFrames      = false;
        int                                            m_TargetFPS        = 120;
        float                                          m_FPS              = 0;
        std::chrono::duration<float>                   m_DeltaTime        = {};
        std::chrono::duration<float>                   m_TargetFrametime  = {};
        std::chrono::high_resolution_clock::time_point m_PreviousFrameTime;
    };
}

#endif