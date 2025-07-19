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
    include "Engine/ThirdParty/vma"
    include "Engine/ThirdParty/vkbootstrap"
    include "Engine/ThirdParty/SDL"
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

    removefiles {
        "%{prj.name}/Shaders/**.bat",
        "%{prj.name}/Shaders/**.spv",
    }

    includedirs {
        "%{prj.name}/Source/Public/",
        "%{prj.name}/Source/Private/",
        "$(VULKAN_SDK)/include/",
        "%{prj.name}/ThirdParty/*/include",
    }

    libdirs {
        "$(VULKAN_SDK)/Lib/",
        "%{prj.name}/ThirdParty/SDL/lib",
    }

    links {
        -- linked libraries
        "vulkan-1",
        "SDL3.lib",
        -- Dependencies
        "spdlog",
        "GLM",
        "ImGui",
        "simdjson",
        "fastgltf",
        "stb",
        "vma",
        "vkbootstrap",
    }

    postbuildcommands {
        "{COPY} %{wks.location}Engine/ThirdParty/SDL/lib/SDL3.dll %{wks.location}Binaries/\"" .. outputdir .. "\"/%{prj.name}",
        "{COPY} %{wks.location}Engine/Shaders/*.spv %{wks.location}Binaries/\"" .. outputdir .. "\"/%{prj.name}/Shaders",
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
