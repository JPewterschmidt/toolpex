add_rules("mode.debug", "mode.release", "mode.asan", "mode.ubsan")
add_requires(
    "gflags", 
    "gtest", 
    "concurrentqueue master", 
    "libuuid"
)
set_policy("build.warning", true)
if is_mode("debug") then
    add_cxxflags("-fno-inline", {force = true})
    set_optimize("none", {force = true})
end

add_syslinks("stdc++exp")

target("toolpex")
    set_kind("shared")
    set_languages("c++23", "c17")
    set_warnings("all", "error")
    add_packages("libuuid")
    add_files("src/*.cc")
    add_includedirs("include", {public = true})
    on_run(function (target)
        --nothing
    end)
