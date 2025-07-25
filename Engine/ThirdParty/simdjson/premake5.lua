project "simdjson"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")
    
    files {
        "include/**.h",
        "src/**.cpp",
    }
    
    includedirs {
        "include/simdjson",
    }

    defines {
        "SIMDJSON_DISABLE_DEPRECATED_API",
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

    filter "system:windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }
