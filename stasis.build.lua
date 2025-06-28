workspace "Stasis"
    architecture "x86_64"
    configurations { "Debug", "Development", "Shipping" }
    startproject "Engine"

    -- Output directories
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Engine"
    location "Engine"
    kind "ConsoleApp"
    language "C++"
    staticruntime "on"
    cppdialect "C++20"

    warnings "High"
    targetdir ("Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("Intermediate/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/Source/**",
        "%{prj.name}/Content/**",
    }

    includedirs {
        "%{prj.name}/Source/Public/",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Development"
        defines { "DEVELOPMENT" }
        runtime "Release"
        symbols "On"
        optimize "Debug"

    filter "configurations:Shipping"
        defines { "SHIPPING", "NDEBUG" }
        runtime "Release"
        symbols "Off"
        optimize "Full"
