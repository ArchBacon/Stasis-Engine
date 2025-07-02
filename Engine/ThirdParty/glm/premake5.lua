project "GLM"
    kind "None"  -- Header-only library
    language "C++"
    cppdialect "C++11"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")    

    files {
        "include/glm/**.hpp",
        "include/glm/**.inl",
        "include/glm/**.h"
    }
    
    includedirs {
        "include"
    }