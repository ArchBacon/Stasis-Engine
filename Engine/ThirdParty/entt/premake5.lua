project "EnTT"
    kind "None"  -- Header-only library
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")
    
    files
    {
        "include/**",
        "src/*",
    }

    includedirs
    {
        "include",
    }

    filter "configurations:Debug"
        defines { "ENTT_DEBUG" }
        runtime "Debug"
        symbols "On"
    
    filter "configurations:Development"
        defines { "ENTT_DEBUG" }
        runtime "Release"
        symbols "On"
        optimize "Debug"
    
    filter "configurations:Shipping"
        defines { "NDEBUG", "ENTT_DISABLE_ASSERT" }
        runtime "Release"
        symbols "Off"
        optimize "Full"
