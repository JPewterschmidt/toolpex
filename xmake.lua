add_rules("mode.debug", "mode.release", "mode.asan", "mode.ubsan")
add_requires(
    "gflags", 
    "gtest", 
    "benchmark",
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

target("toolpex-test")
    set_kind("binary")
    set_languages("c++23", "c17")
    add_files("test/*.cc")
    set_warnings("all", "error")
    add_packages("gtest")
    add_deps("toolpex")
    add_packages("concurrentqueue")
    add_cxxflags("-fno-inline", {force = true})
    set_optimize("none", {force = true})
    after_build(function (target)
        os.exec(target:targetfile())
        print("xmake: unittest complete.")
    end)
    on_run(function (target)
        --nothing
    end)
    
target("toolpex-example")
    set_kind("binary")
    set_languages("c++23", "c17")
    add_deps("toolpex")
    add_files("example/*.cc")
    add_packages("gflags")
    add_packages("concurrentqueue")
