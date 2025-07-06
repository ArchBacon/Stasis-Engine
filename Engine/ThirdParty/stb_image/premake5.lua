project "STB_Image"
    kind "Utility"  -- Header-only library
    language "C++"

    targetdir ("%{wks.location}Binaries/" .. outputdir .. "/Engine")
    objdir ("%{wks.location}Intermediate/" .. outputdir .. "/Engine")

    files {
        "stb_image.h",
    }

    includedirs {
        ".",
    }

    filter "system:windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "architecture:x86_64"
        defines { "STBI_SSE2" } -- Enable SSE2 optimizations