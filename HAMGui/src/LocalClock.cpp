#include "LocalClock.h"

namespace hammer
{
    Clock::Clock( bool LimitFrames 
                , int TargetFPS ) :
        m_LimitFrames     { LimitFrames }
    ,   m_TargetFPS       { TargetFPS }
    ,   m_TargetFrametime { 1.0f/TargetFPS }
    {

    }

    void Clock::StartFrame() noexcept
    {
        m_PreviousFrameTime = HRClock::now();
    }

    void Clock::EndFrame() noexcept
    {
        auto Frametime = HRClock::now() - m_PreviousFrameTime;

        // Sleep if necessary to achieve the target frame rate
        if ( m_LimitFrames )
        {
            FDuration SleepDuration = m_TargetFrametime - Frametime;
            if ( SleepDuration > FDuration::zero() )
            {
                std::this_thread::sleep_for( SleepDuration );
            }
            m_DeltaTime = m_TargetFrametime.count() > Frametime.count() ? m_TargetFrametime
                                                                        : Frametime;
        }
        else
        {
            m_DeltaTime = Frametime;
        }
    }

    float Clock::GetDt(void) const noexcept
    {
        return m_DeltaTime.count();
    }

    float Clock::GetFPS(void) noexcept
    {
        m_FPS = 1.0f / m_DeltaTime.count();
        return m_FPS;
    }
}