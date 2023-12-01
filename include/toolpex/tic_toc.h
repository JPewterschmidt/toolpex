#ifndef TOOLPEX_TIC_TOC_H
#define TOOLPEX_TIC_TOC_H

#include <chrono>
#include <iostream>
#include <sstream>

#include "toolpex/macros.h"

TOOLPEX_NAMESAPCE_BEG

auto tic()
{
    return ::std::chrono::high_resolution_clock::now();
}

[[nodiscard]] auto toc(const auto& tp)
{
    ::std::stringstream ss{};   
    auto now = ::std::chrono::high_resolution_clock::now();
    ss << ::std::chrono::duration_cast<::std::chrono::nanoseconds>(now - tp).count() << "ns" << ::std::endl;
    return ss.str();
}

TOOLPEX_NAMESAPCE_END

#endif
