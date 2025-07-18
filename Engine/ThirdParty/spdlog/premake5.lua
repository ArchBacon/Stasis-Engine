project "spdlog"
    kind "None"  -- Header-only library
    language "C++"
    cppdialect "C++11"
    staticruntime "on"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")
    
    files {
        "include/spdlog/**.h",
    }
    
    includedirs {
        "include",
    }

    filter "action:vs*"
        defines { "_CRT_SECURE_NO_WARNINGS" }