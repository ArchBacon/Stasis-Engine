project "spdlog"
    kind "None"  -- Header-only
    language "C++"
    cppdialect "C++11"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")
    
    files {
        "include/spdlog/**.h",
    }
    
    includedirs {
        "include",
    }

    filter "action:vs*"
        defines { "_CRT_SECURE_NO_WARNINGS" }