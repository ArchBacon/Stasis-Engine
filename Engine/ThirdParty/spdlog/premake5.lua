project "spdlog"
    kind "None"  -- Header-only
    language "C++"
    cppdialect "C++11"
    
    files {
        "include/spdlog/**.h",
    }
    
    includedirs {
        "include",
    }

    filter "action:vs*"
        defines { "_CRT_SECURE_NO_WARNINGS" }