add_rules("mode.debug", "mode.release", "mode.asan", "mode.ubsan")
add_requires(
    "fmt", 
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
    set_languages("c++2b", "c17")
    set_warnings("all", "error")
    add_files("src/*.cc")
    add_packages("fmt")
    add_includedirs("include", {public = true})
    on_run(function (target)
        --nothing
    end)

--target("toolpex-test")
--    set_kind("binary")
--    set_languages("c++2b", "c17")
--    add_files("test/*.cc")
--    set_warnings("all", "error")
--    add_packages("gtest")
--    add_includedirs("include")
--    add_deps("toolpex")
--    add_packages("concurrentqueue")
--    add_cxxflags("-fno-inline", {force = true})
--    set_optimize("none", {force = true})
--    after_build(function (target)
--        os.exec(target:targetfile())
--        print("xmake: unittest complete.")
--    end)
--    on_run(function (target)
--        --nothing
--    end)
--    
--target("toolpex-example")
--    set_kind("binary")
--    set_languages("c++2b", "c17")
--    add_deps("toolpex")
--    add_files("example/*.cc")
--    add_packages("fmt", "gflags")
--    add_packages("concurrentqueue")
--    add_includedirs("include")
