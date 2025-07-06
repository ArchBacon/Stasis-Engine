workspace "Stasis"
    architecture "x86_64"
    configurations { "Debug", "Development", "Shipping" }
    startproject "Engine"
    buildoptions { "/utf-8" }

    -- Output directories
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "Engine/ThirdParty/spdlog"
    include "Engine/ThirdParty/GLM"
    include "Engine/ThirdParty/ImGui"
    include "Engine/ThirdParty/SimdJSON"
    include "Engine/ThirdParty/FastGLTF"
    include "Engine/ThirdParty/STB_Image"
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
        -- Everything in Content
        "%{prj.name}/Content/**",
        
        -- Uncompiled shader files
        "%{prj.name}/Shaders/**.comp",
        "%{prj.name}/Shaders/**.vert",
        "%{prj.name}/Shaders/**.frag",
        "%{prj.name}/Shaders/**.glsl",
        
        -- Everything in Source
        "%{prj.name}/Source/**",
        
        -- Include third-party code
        "%{prj.name}/ThirdParty/vkbootstrap/VkBootstrap.cpp",
    }

    includedirs {
        "%{prj.name}/Source/Public/",
        "%{prj.name}/Source/Private/",
        "$(VULKAN_SDK)/include/",
        "%{prj.name}/ThirdParty/glm/include",
        "%{prj.name}/ThirdParty/imgui/include",
        "%{prj.name}/ThirdParty/SDL/include",
        "%{prj.name}/ThirdParty/spdlog/include",
        "%{prj.name}/ThirdParty/vkbootstrap",
        "%{prj.name}/ThirdParty/vma",
        "%{prj.name}/ThirdParty/simdjson/include",
        "%{prj.name}/ThirdParty/fastgltf/include",
        "%{prj.name}/ThirdParty/stb_image",
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
        "SimdJSON",
        "FastGLTF",
        "STB_Image",
    }

    prebuildcommands {
        -- Compile shader files before build starts
        "%{wks.location}Engine/Shaders/compile_shaders.bat"
    }

    postbuildcommands {
        -- Copy SDL3.dll to engine executable location
        "{COPY} %{wks.location}Engine/ThirdParty/SDL/lib/SDL3.dll %{wks.location}Binaries/\"" .. outputdir .. "\"/%{prj.name}"
    }

    filter "configurations:Debug"
        defines { "DEBUG", "GLM_FORCE_DEPTH_ZERO_TO_ONE" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Development"
        defines { "DEVELOPMENT", "GLM_FORCE_DEPTH_ZERO_TO_ONE" }
        runtime "Release"
        symbols "On"
        optimize "Debug"

    filter "configurations:Shipping"
        defines { "SHIPPING", "NDEBUG", "GLM_FORCE_DEPTH_ZERO_TO_ONE" }
        runtime "Release"
        symbols "Off"
        optimize "Full"
