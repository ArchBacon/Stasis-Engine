project "fastgltf"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")
    
    files {
        "include/**",
        "src/*",
    }

    includedirs {
        "include",
    }

    externalincludedirs {
         "../simdjson/include",  -- for SIMD JSON parsing
    }

    defines {
        "FASTGLTF_ENABLE_DOCS=OFF",
        "FASTGLTF_COMPILE_AS_CPP20=ON",
    }

    filter "configurations:Debug"
        defines { "DEBUG", "FASTGLTF_ENABLE_ASSERTS" }
        runtime "Debug"
        symbols "On"
    
    filter "configurations:Development"
        defines { "DEVELOPMENT", "FASTGLTF_ENABLE_ASSERTS" }
        runtime "Release"
        symbols "On"
        optimize "Debug"
    
    filter "configurations:Shipping"
        defines { "SHIPPING", "NDEBUG" }
        runtime "Release"
        symbols "Off"
        optimize "Full"

