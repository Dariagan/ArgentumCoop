#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

#TODO cuando se exporte el game, usar la template de building de producción, no la debug

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/"])

#compilar para windows:
#1. instalar el compilador:
#sudo apt-get install gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 wine64
#2 poner esto en la Selection=1:
#sudo update-alternatives --config x86_64-w64-mingw32-g++ # Set the default mingw32 g++ compiler option to posix (1).

env.Append(CXXFLAGS=["-O3", "-fexceptions", "-std=c++17", "-pthread"])
sources = Glob("src/*.cpp")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "../bin/ArgentumCpp.{}.{}.framework/ArgentumCpp.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "../bin/ArgentumCpp{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)

#IMPORTANTE: PARA PASAR EL .DLL A ULISES, SACARLE EL "lib" para que se lo detecte