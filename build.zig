const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "main",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }),
    });
    const root = exe.root_module;
    const os_tag = target.result.os.tag;

    const common_c_flags = &[_][]const u8{ "-std=c99", "-Wall", "-Wextra" };

    root.addCSourceFile(.{
        .file = .{ .cwd_relative = "main.c" },
        .flags = common_c_flags,
    });
    const raylib_common_flags = &[_][]const u8{
        "-std=c99",
        "-DPLATFORM_DESKTOP",
        "-DGRAPHICS_API_OPENGL_33",
        "-D_GNU_SOURCE",
        "-D_DEFAULT_SOURCE",
        "-Wno-missing-braces",
        "-Werror=pointer-arith",
        "-fno-strict-aliasing",
    };

    const raylib_sources = [_][]const u8{
        "third_party/raylib/src/rcore.c",
        "third_party/raylib/src/rmodels.c",
        "third_party/raylib/src/rshapes.c",
        "third_party/raylib/src/rtextures.c",
        "third_party/raylib/src/rtext.c",
        "third_party/raylib/src/raudio.c",
        "third_party/raylib/src/utils.c",
    };

    for (raylib_sources) |src| {
        root.addCSourceFile(.{
            .file = .{ .cwd_relative = src },
            .flags = raylib_common_flags,
        });
    }

    root.addCSourceFile(.{
        .file = .{ .cwd_relative = "third_party/raylib/src/rglfw.c" },
        .flags = raylib_common_flags,
        .language = if (os_tag == .macos) .objective_c else .c,
    });

    root.addIncludePath(.{ .cwd_relative = "renderers" });
    root.addIncludePath(.{ .cwd_relative = "third_party" });
    root.addIncludePath(.{ .cwd_relative = "third_party/raylib/src" });
    root.addIncludePath(.{ .cwd_relative = "third_party/raylib/src/external/glfw/include" });

    exe.linkLibC();

    if (os_tag == .macos) {
        const frameworks = [_][]const u8{
            "Cocoa",
            "IOKit",
            "CoreVideo",
            "OpenGL",
            "AudioToolbox",
            "CoreAudio",
        };
        for (frameworks) |fw| {
            exe.linkFramework(fw);
        }
    } else if (os_tag == .linux) {
        const libs = [_][]const u8{ "GL", "pthread", "dl", "rt", "m", "X11" };
        for (libs) |lib| {
            exe.linkSystemLibrary(lib);
        }
    } else if (os_tag == .windows) {
        const libs = [_][]const u8{
            "winmm",
            "gdi32",
            "opengl32",
            "user32",
            "kernel32",
        };
        for (libs) |lib| {
            exe.linkSystemLibrary(lib);
        }
    }

    b.installArtifact(exe);
}
