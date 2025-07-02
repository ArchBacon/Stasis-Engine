project "GLM"
    kind "None"  -- Header-only library
    language "C++"
    cppdialect "C++11"

    targetdir ("%{wks.location}Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}Intermediate/" .. outputdir .. "/Engine")    

    files {
        "include/glm/**.hpp",
        "include/glm/**.inl",
        "include/glm/**.h"
    }
    
    includedirs {
        "include"
    }