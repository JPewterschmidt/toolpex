#include "toolpex/callback_promise.h"
#include "gtest/gtest.h"

using namespace toolpex;

TEST(callback_promise, for_value)
{
    bool ok{};

    callback_promise<int> cp([&ok](future_frame<int> ff) mutable { 
        ok = ff.value() == 5;
    });

    cp.set_value(5);

    ASSERT_TRUE(ok);
}

TEST(callback_promise, for_nonconst_ref)
{
    bool ok{};

    int i{5};
    const int* addr = &i;

    callback_promise<const int&> cp([&ok, addr](future_frame<const int&> ff) mutable {
        const auto& int_cref = ff.value();
        ok = addr == &int_cref;
    });

    cp.set_value(i);

    ASSERT_TRUE(ok);
}

TEST(callback_promise, for_ref)
{
    bool ok{};

    callback_promise<bool&> cp([](future_frame<bool&> ff) { 
        ff.value() = true;
    });

    cp.set_value(ok);

    ASSERT_TRUE(ok);
}

TEST(callback_promise, for_void)
{
    bool ok{};

    callback_promise<void> cp([&ok](future_frame<void> ff) mutable { 
        ok = true;
    });

    cp.set_value();

    ASSERT_TRUE(ok);
}
