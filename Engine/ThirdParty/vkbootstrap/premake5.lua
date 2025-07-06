project "vkbootstrap"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("%{wks.location}Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}Intermediate/" .. outputdir .. "/Engine")
    
    files {
        "**",
    }
    
    includedirs {
        ".",
        "$(VULKAN_SDK)/include/",
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