#include "toolpex/convert_to_systime.h"
#include "gtest/gtest.h"
#include <sys/time.h>

using namespace toolpex;

// The idea from:
// https://stackoverflow.com/questions/39421089/convert-stdchronosystem-clocktime-point-to-struct-timeval-and-back

::timeval make_timeval(time_t s, long us)
{
    ::timeval tv;
    tv.tv_sec = s;
    tv.tv_usec = us;
    return tv;
}

::timespec make_timespec(time_t s, long ns)
{
    ::timespec tv;
    tv.tv_sec = s;
    tv.tv_nsec = ns;
    return tv;
}

using namespace ::std::chrono;

static bool operator==(::timeval x, ::timeval y)
{
    return x.tv_sec == y.tv_sec && x.tv_usec == y.tv_usec;
}

static bool operator==(::timespec x, ::timespec y)
{
    return x.tv_sec == y.tv_sec && x.tv_nsec == y.tv_nsec;
}

TEST(convert_to_systmee, timespec)
{
    ASSERT_EQ(make_timespec(0, 0), convert_to_timespec(system_clock::time_point{}));
    ASSERT_EQ(make_timespec(1, 0), convert_to_timespec(system_clock::time_point{seconds{1}}));
    ASSERT_EQ(make_timespec(1, 400000), convert_to_timespec(system_clock::time_point{seconds{1} + nanoseconds{400000}}));
    ASSERT_EQ(make_timespec(-1, 400000), convert_to_timespec(system_clock::time_point{seconds{-1} + nanoseconds{400000}}));
    ASSERT_EQ(to_time_point(make_timespec(0, 0)), system_clock::time_point{});
    ASSERT_EQ(to_time_point(make_timespec(1, 0)), system_clock::time_point{seconds{1}});
    ASSERT_EQ(to_time_point(make_timespec(1, 400000)), system_clock::time_point{seconds{1} + nanoseconds{400000}});
    ASSERT_EQ(to_time_point(make_timespec(-1, 400000)), system_clock::time_point{seconds{-1} + nanoseconds{400000}});
}

TEST(convert_to_systime, timeval)
{
    ASSERT_EQ(make_timeval(0, 0), convert_to_timeval(system_clock::time_point{}));
    ASSERT_EQ(make_timeval(1, 0), convert_to_timeval(system_clock::time_point{seconds{1}}));
    ASSERT_EQ(make_timeval(1, 400000), convert_to_timeval(system_clock::time_point{seconds{1} + microseconds{400000}}));
    ASSERT_EQ(make_timeval(-1, 400000), convert_to_timeval(system_clock::time_point{seconds{-1} + microseconds{400000}}));
    ASSERT_EQ(to_time_point(make_timeval(0, 0)), system_clock::time_point{});
    ASSERT_EQ(to_time_point(make_timeval(1, 0)), system_clock::time_point{seconds{1}});
    ASSERT_EQ(to_time_point(make_timeval(1, 400000)), system_clock::time_point{seconds{1} + microseconds{400000}});
    ASSERT_EQ(to_time_point(make_timeval(-1, 400000)), system_clock::time_point{seconds{-1} + microseconds{400000}});
}
