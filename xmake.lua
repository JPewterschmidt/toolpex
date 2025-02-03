add_rules("mode.debug", "mode.release", "mode.asan", "mode.ubsan")
add_requires(
    "gflags", 
    "gtest", 
    "concurrentqueue master", 
    "fmt",
    "cpptrace", 
    "libuuid"
)
set_policy("build.warning", true)
if is_mode("debug") then
    add_cxxflags("-fno-inline", {force = true})
    set_optimize("none", {force = true})
end

add_syslinks("stdc++exp")
add_packages("cpptrace", "fmt")

target("toolpex")
    set_kind("shared")
    set_languages("c++20", "c17")
    set_warnings("all", "error")
    add_packages("libuuid", "fmt")
    add_files("src/*.cc")
    add_includedirs("include", {public = true})
    on_run(function (target)
        --nothing
    end)
