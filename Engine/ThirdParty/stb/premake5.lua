project "stb"
    kind "Utility"  -- Header-only library
    language "C++"
    staticruntime "on"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")

    files
    {
        "include/stb/stb_image.h",
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

    filter "architecture:x86_64"
        defines { "STBI_SSE2" } -- Enable SSE2 optimizations