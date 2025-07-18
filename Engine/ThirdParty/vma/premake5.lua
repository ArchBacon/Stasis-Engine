project "vma"
    kind "None"  -- Header-only library
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")
    
    files {
        "include/vma/vk_mem_alloc.h",
    }
    
    includedirs {
        "include",
        "$(VULKAN_SDK)/include/",
    }
