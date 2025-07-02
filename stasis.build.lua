workspace "Stasis"
    architecture "x86_64"
    configurations { "Debug", "Development", "Shipping" }
    startproject "Engine"
    buildoptions { "/utf-8" }

    -- Output directories
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "Engine/ThirdParty/spdlog"
    include "Engine/ThirdParty/GLFW"
    include "Engine/ThirdParty/GLM"
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
        "%{prj.name}/Source/**",
        "%{prj.name}/Content/**",
        "%{prj.name}/ThirdParty/vkbootstrap/VkBootstrap.cpp",
    }

    includedirs {
        "%{prj.name}/Source/Public/",
        "%{prj.name}/Source/Private/",
        "%{prj.name}/Thirdparty/*/include",
        "%{prj.name}/Thirdparty/*",
        "$(VULKAN_SDK)/include/",
    }

    libdirs {
        "$(VULKAN_SDK)/Lib/",
        "%{prj.name}/Thirdparty/*/bin",
    }

    links {
        "vulkan-1",
        "spdlog",
        "GLFW",
        "GLM",
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
