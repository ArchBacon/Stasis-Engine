project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")
    
    files
    {
        "include/**.h",
        "src/**.cpp",
    }
    
    includedirs
    {
        "include/imgui",
        "$(VULKAN_SDK)/include/",
    }

    externalincludedirs
    {
        "../SDL/include",
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