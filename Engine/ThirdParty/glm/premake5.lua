project "GLM"
    kind "None"  -- Header-only library
    language "C++"
    cppdialect "C++11"
    staticruntime "on"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")    

    files
    {
        "include/glm/**.hpp",
        "include/glm/**.inl",
        "include/glm/**.h"
    }
    
    includedirs
    {
        "include"
    }

    defines
    {
        "GLM_ENABLE_EXPERIMENTAL",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE",
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
