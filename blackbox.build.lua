workspace "Blackbox"
    architecture "x86_64"
    configurations { "Development", "Debug", "Shipping" }
    startproject "Engine"
    buildoptions { "/utf-8" }

    -- Output directories
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "Engine/ThirdParty/spdlog"
    include "Engine/ThirdParty/GLM"
    include "Engine/ThirdParty/ImGui"
    include "Engine/ThirdParty/simdjson"
    include "Engine/ThirdParty/fastgltf"
    include "Engine/ThirdParty/stb"
    include "Engine/ThirdParty/SDL"
    include "Engine/ThirdParty/GLAD"
group ""

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
        "%{prj.name}/Content/**",
        "%{prj.name}/Shaders/**",
        "%{prj.name}/Source/**",
    }

    includedirs {
        "%{prj.name}/Source/Public/",
        "%{prj.name}/Source/Private/",
        "%{prj.name}/ThirdParty/*/include",
    }

    libdirs {
        "%{prj.name}/ThirdParty/SDL/lib",
    }

    links {
        -- Libraries
        "SDL3.lib",
        "opengl32.lib",
        -- Dependencies
        "spdlog",
        "GLM",
        "ImGui",
        "simdjson",
        "fastgltf",
        "stb",
        "GLAD",
    }

    postbuildcommands {
        "{COPY} %{wks.location}Engine/ThirdParty/SDL/lib/SDL3.dll %{wks.location}Binaries/\"" .. outputdir .. "\"/%{prj.name}",
        "{COPY} %{wks.location}Engine/Content/** %{wks.location}Binaries/\"" .. outputdir .. "\"/%{prj.name}/Content",
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
