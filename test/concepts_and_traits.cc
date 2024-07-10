#include <functional>
#include <string>
#include <typeinfo>
#include <sys/time.h>
#include <filesystem>

#include "gtest/gtest.h"
#include "toolpex/concepts_and_traits.h"

using namespace toolpex;
using namespace ::std::placeholders;

namespace 
{
    void dummy1(int, double, int){}
    void dummy2(double, int) noexcept {}
    void dummy3(){}

    ::std::function<void(int, int, int)> dummy4;

    struct dummy5_t
    {
        constexpr void operator()(int, int) const noexcept {}
    };

    ::std::function<double(const int&, double) noexcept> dummy7;
    double dummy8() { return 1.0; }
    double dummy9() noexcept { return 1.0; }
    double dummy10(const int&) noexcept { return 1.0; }
}

TEST(concepts, number_of_parameters)
{
    ASSERT_EQ(int(toolpex::number_of_parameters_v<decltype(dummy1)>), 3);
    ASSERT_EQ(int(toolpex::number_of_parameters_v<decltype(dummy2)>), 2);
    ASSERT_EQ(int(toolpex::number_of_parameters_v<decltype(dummy3)>), 0);
    ASSERT_EQ(toolpex::number_of_parameters_v<decltype(dummy4)>, 3);

    ASSERT_EQ(toolpex::number_of_parameters_v<decltype(&dummy5_t::operator())>, 2);
    //ASSERT_EQ(toolpex::number_of_parameters_v<decltype(::std::function<double(const int&, double) noexcept>)>, 2);
    ASSERT_EQ(toolpex::number_of_parameters_v<decltype(dummy10)>, 1);
}

TEST(concepts, get_return_type)
{
    ::std::string name1 = typeid(get_return_type_t<decltype(dummy7)>).name();
    ::std::string name2 = typeid(get_return_type_t<decltype(dummy8)>).name();
    ::std::string name3 = typeid(get_return_type_t<decltype(dummy9)>).name();
    ASSERT_EQ(name1, name2);
    ASSERT_EQ(name1, name3);
}

TEST(concepts, timespec_like)
{
    ASSERT_TRUE(timespec_like_concept<::timespec>);
    ASSERT_TRUE(timeval_like_concept<::timeval>);
}

TEST(concepts, span_like)
{
    ::std::span<int> dummyspan{};
    ASSERT_TRUE(span_like<::std::span<int>>);
    ASSERT_TRUE(span_like<decltype(dummyspan)>);
}

TEST(concepts, std_specification_predict)
{
    ASSERT_TRUE(is_std_chrono_duration<::std::chrono::milliseconds>);
    ASSERT_FALSE(is_std_chrono_duration<::std::chrono::system_clock::time_point>);
    ASSERT_TRUE(is_std_chrono_time_point<::std::chrono::system_clock::time_point>);
    ASSERT_FALSE(is_std_chrono_time_point<::std::chrono::milliseconds>);
    ASSERT_TRUE(is_std_chrono_duration_or_time_point<::std::chrono::milliseconds>);
    ASSERT_TRUE(is_std_chrono_duration_or_time_point<::std::chrono::system_clock::time_point>);
}

TEST(concepts, size_as_byte)
{
    ASSERT_TRUE(size_as_byte<::std::byte>);
    ASSERT_TRUE(size_as_byte<char>);
    ASSERT_TRUE(size_as_byte<unsigned char>);
}

TEST(concepts, boolean_testable)
{
    ASSERT_TRUE(boolean_testable<bool>);
    ASSERT_TRUE(boolean_testable<void*>);
    ASSERT_TRUE(boolean_testable<char[]>);
    ASSERT_TRUE(boolean_testable<bool&>);
    ASSERT_TRUE(boolean_testable<void*&>);

    class foo
    {
    public:
        operator bool() const noexcept { return true; }
    };

    ASSERT_TRUE(boolean_testable<foo>);
    ASSERT_TRUE(boolean_testable<foo&>);
    ASSERT_TRUE(boolean_testable<const foo&>);
}

TEST(concepts, bit_observable)
{
    class foo
    {
    public:
        foo operator|(const foo&) const;
        foo operator^(const foo&) const;
        foo operator&(const foo&) const;
    };

    ASSERT_TRUE(bit_observable<int>);
    ASSERT_TRUE(bit_observable<int&>);
    ASSERT_TRUE(bit_observable<foo>);
    ASSERT_TRUE(bit_observable<const foo&>);
    ASSERT_TRUE(bit_observable<::std::filesystem::perms>);
    ASSERT_TRUE(bit_observable<::std::filesystem::perms&>);
}
