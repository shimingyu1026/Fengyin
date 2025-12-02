includes("../utils")
includes("../graph")

target("traffic")
    set_kind("static")
    add_files("*.cpp")
    add_includedirs(".", {public = true})
    add_packages("boost","spdlog")
    add_deps("utils","graph")