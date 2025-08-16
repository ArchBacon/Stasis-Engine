project "spdlog"
    kind "None"  -- Header-only library
    language "C++"
    cppdialect "C++11"
    staticruntime "on"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")
    
    files
    {
        "include/spdlog/**.h",
    }
    
    includedirs
    {
        "include",
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
    
    filter "configurations:Development"
        runtime "Release"
        symbols "On"
        optimize "Debug"
    
    filter "configurations:Shipping"
        defines { "NDEBUG" }
        runtime "Release"
        symbols "Off"
        optimize "Full"

    filter "system:windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }