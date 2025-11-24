includes("../utils")

target("graph")
    set_kind("static")
    add_files("*.cpp")
    add_includedirs(".", {public = true})


    add_packages("boost","spdlog")
    
    add_deps("utils")

    add_syslinks("tbb")