add_rules("mode.debug", "mode.release", "mode.asan", "mode.ubsan")
add_requires(
    "gflags", 
    "gtest", 
    "benchmark",
    "concurrentqueue master"
)
set_policy("build.warning", true)
if is_mode("debug") then
    add_cxxflags("-fno-inline", {force = true})
    set_optimize("none", {force = true})
end

target("toolpex")
    set_kind("shared")
    set_languages("c++23", "c17")
    set_warnings("all", "error")
    add_files("src/*.cc")
    add_includedirs("include", {public = true})
    on_run(function (target)
        --nothing
    end)
