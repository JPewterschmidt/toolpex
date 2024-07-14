// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_CONVERT_TO_SYSTIME_H
#define TOOLPEX_CONVERT_TO_SYSTIME_H

#include "toolpex/macros.h"
#include "toolpex/concepts_and_traits.h"

TOOLPEX_NAMESPACE_BEG

template<timespec_like_concept TimespecLike = ::timespec>
TimespecLike convert_to_timespec(is_std_chrono_time_point auto const& tp) 
{
    using namespace std::chrono;
    auto s = time_point_cast<seconds>(tp);
    if (s > tp)
        s = s - seconds{1};
    auto ns = duration_cast<nanoseconds>(tp - s);
    TimespecLike tv;
    tv.tv_sec = s.time_since_epoch().count();
    tv.tv_nsec = ns.count();
    return tv;
}

template<timeval_like_concept TimevalLike = ::timeval>
TimevalLike convert_to_timeval(is_std_chrono_time_point auto const& tp) 
{
    using namespace std::chrono;
    auto s = time_point_cast<seconds>(tp);
    if (s > tp)
        s = s - seconds{1};
    auto us = duration_cast<microseconds>(tp - s);
    TimevalLike tv;
    tv.tv_sec = s.time_since_epoch().count();
    tv.tv_usec = us.count();
    return tv;
}

template<is_std_chrono_time_point ResultTimePoint = std::chrono::system_clock::time_point>
ResultTimePoint to_time_point(::timeval tv)
{
    using namespace std::chrono;
    return ResultTimePoint{
          seconds{tv.tv_sec} 
        + microseconds{tv.tv_usec}
    };
}

template<is_std_chrono_time_point ResultTimePoint = std::chrono::system_clock::time_point>
ResultTimePoint to_time_point(::timespec tv)
{
    using namespace std::chrono;
    return ResultTimePoint{
          seconds{tv.tv_sec} 
        + nanoseconds{tv.tv_nsec}
    };
}

TOOLPEX_NAMESPACE_END

#endif
