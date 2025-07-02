project "GLM"
    kind "None"  -- Header-only library
    language "C++"
    cppdialect "C++11"
    
    files {
        "include/glm/**.hpp",
        "include/glm/**.inl",
        "include/glm/**.h"
    }
    
    includedirs {
        "include"
    }