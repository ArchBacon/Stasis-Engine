project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}Intermediate/" .. outputdir .. "/Engine")
    
    files {
        "include/**.h",
        "src/**.cpp",
    }
    
    includedirs {
        "include/imgui",
        "$(VULKAN_SDK)/include/",
    }

    externalincludedirs {
        "../SDL/include",
    }
