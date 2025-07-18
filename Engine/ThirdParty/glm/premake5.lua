project "GLM"
    kind "None"  -- Header-only library
    language "C++"
    cppdialect "C++11"
    staticruntime "on"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")    

    files {
        "include/glm/**.hpp",
        "include/glm/**.inl",
        "include/glm/**.h"
    }
    
    includedirs {
        "include"
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