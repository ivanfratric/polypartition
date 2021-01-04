#!python
import os, subprocess

opts = Variables([], ARGUMENTS)

# Gets the standard flags CC, CCX, etc.
env = DefaultEnvironment()

# Define our options. Use future-proofed names for platforms.
platform_array = ["", "windows", "macos", "linux"]
opts.Add(EnumVariable("target", "Compilation target", "debug", ["d", "debug", "r", "release"]))
opts.Add(EnumVariable("platform", "Compilation platform", "", platform_array))
opts.Add(EnumVariable("p", "Alias for 'platform'", "", platform_array))
opts.Add(BoolVariable("use_llvm", "Use the LLVM / Clang compiler", "no"))

# Only support 64-bit systems.
bits = 64

# Updates the environment with the option variables.
opts.Update(env)

# Process platform arguments.
if env["p"] != "":
    env["platform"] = env["p"]

if env["platform"] == "":
    print("No valid target platform selected.")
    quit()

# Process other arguments.
if env["use_llvm"]:
    env["CXX"] = "clang++"

env.Append(CCFLAGS=["-Wno-unused-result"])

# Check our platform specifics
if env["platform"] == "macos":
    if env["target"] in ("debug", "d"):
        env.Append(CCFLAGS=["-g", "-O2", "-arch", "x86_64"])
        env.Append(LINKFLAGS=["-arch", "x86_64"])
    else:
        env.Append(CCFLAGS=["-g", "-O3", "-arch", "x86_64"])
        env.Append(LINKFLAGS=["-arch", "x86_64"])

elif env["platform"] == "linux":
    if env["target"] in ("debug", "d"):
        env.Append(CCFLAGS=["-fPIC", "-g3", "-Og"])
    else:
        env.Append(CCFLAGS=["-fPIC", "-g", "-O3"])

elif env["platform"] == "windows":
    # This makes sure to keep the session environment variables
    # on Windows, so that you can run scons in a VS 2017 prompt
    # and it will find all the required tools.
    env.Append(ENV=os.environ)

    env.Append(CCFLAGS=["-DWIN32", "-D_WIN32", "-D_WINDOWS", "-W3", "-GR", "-D_CRT_SECURE_NO_WARNINGS"])
    if env["target"] in ("debug", "d"):
        env.Append(CCFLAGS=["-EHsc", "-D_DEBUG", "-MDd"])
    else:
        env.Append(CCFLAGS=["-O2", "-EHsc", "-DNDEBUG", "-MD"])

# Tweak this if you want to use different folders,
# or more folders, to store your source code in.
env.Append(CPPPATH=["./", "../src/"])
sources = Glob("*.cpp") + Glob("../src/*.cpp")

program = env.Program(target="./polypartition_test", source=sources)

Default(program)

# Generates help for the -h scons option.
Help(opts.GenerateHelpText(env))
