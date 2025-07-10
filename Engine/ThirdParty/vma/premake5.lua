project "vma"
    kind "None"  -- Header-only library
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")
    
    files {
        "vk_mem_alloc.h",
    }
    
    includedirs {
        ".",
        "$(VULKAN_SDK)/include/",
    }
