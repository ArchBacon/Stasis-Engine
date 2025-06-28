project "GLFW"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    
    files {
        "include/GLFW/*.h",
        "src/context.c",
        "src/egl_context.*",
        "src/init.c",
        "src/input.c",
        "src/internal.h",
        "src/mappings.h",
        "src/monitor.c",
        "src/null_*.*",
        "src/osmesa_context.*",
        "src/platform.c",
        "src/platform.h",
        "src/vulkan.c",
        "src/window.c",
    }
    
    includedirs {
        "include/",
    }

    filter "system:windows"
        systemversion "latest"
    
        files {
            "src/win32_*.*",
            "src/wgl_context.*",
        }
        
        defines {
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }
        
        links {
            "Dwmapi",
            "User32",
            "Gdi32",
            "Shell32"
        }

    filter "system:linux"
        files {
            "src/x11_*.*",
            "src/xkb_*.*",
            "src/glx_context.*",
            "src/egl_context.*",
            "src/osmesa_context.*",
            "src/linux_*.*",
            "src/posix_*.*",
        }
        
        defines { "_GLFW_X11" }

    filter "system:macosx"
        files {
            "src/cocoa_*.*",
            "src/nsgl_context.*",
            "src/posix_thread.*",
            "src/posix_module.c",
        }
        
        defines { "_GLFW_COCOA" }

filter "configurations:Debug"
    runtime "Debug"
    symbols "On"

filter "configurations:Development"
    runtime "Release"
    symbols "On"
    optimize "Debug"

filter "configurations:Shipping"
    runtime "Release"
    symbols "Off"
    optimize "Full"
