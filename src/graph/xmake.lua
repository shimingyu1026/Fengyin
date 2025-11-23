

target("graph")
    set_kind("static")
    add_files("*.cpp")
    add_includedirs(".", {public = true})
    add_packages("boost")

    add_syslinks("tbb")