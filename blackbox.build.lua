workspace "Blackbox"
    architecture "x86_64"
    configurations { "Development", "Debug", "Shipping" }
    startproject "Engine"
    buildoptions { "/utf-8" }

    -- Output directories
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "Engine/ThirdParty/entt"
    include "Engine/ThirdParty/fastgltf"
    include "Engine/ThirdParty/glad"
    include "Engine/ThirdParty/glm"
    include "Engine/ThirdParty/imgui"
    include "Engine/ThirdParty/SDL"
    include "Engine/ThirdParty/simdjson"
    include "Engine/ThirdParty/spdlog"
    include "Engine/ThirdParty/stb"
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

    files
    {
        "%{prj.name}/Content/**",
        "%{prj.name}/Source/**",
        "%{prj.name}/Source/Blackbox.hpp",
    }

    includedirs
    {
        "%{prj.name}/Source/",
        "%{prj.name}/Source/Public/",
        "%{prj.name}/Source/Private/",
        "%{prj.name}/ThirdParty/*/include",
    }

    libdirs
    {
        "%{prj.name}/ThirdParty/SDL/lib",
    }

    links
    {
        -- Libraries
        "SDL3.lib",
        "opengl32.lib",
        -- Dependencies
        "EnTT",
        "fastgltf",
        "GLAD",
        "GLM",
        "ImGui",
        "simdjson",
        "spdlog",
        "stb",
    }

    defines
    {
        "GLM_ENABLE_EXPERIMENTAL",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE",
    }

    postbuildcommands
    {
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
