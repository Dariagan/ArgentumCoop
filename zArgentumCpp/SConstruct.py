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


#SETUP PARA LINUX PA TENER c++23 features:
#sudo apt install g++-13
#sudo apt install gcc-13
#ctrl+shift+p -> select intellisense configuration -> gcc-13
#sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 1
#sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 1


#compilar para windows desde linux:



#1. instalar el compilador:
#sudo apt-get install gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 wine64
#2 poner esto en la Selection=1:
#sudo update-alternatives --config x86_64-w64-mingw32-g++ # Set the default mingw32 g++ compiler option to posix (1).

#FIXME HACER LO SIGUIENTE HACE Q DEJE DE FUNCIONAR EL COMPILE FOR WINDOWS:
#sudo update-alternatives --install /usr/bin/x86_64-w64-mingw32-g++ x86_64-w64-mingw32-g++ /usr/bin/g++-13 100
#sudo update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/g++-13

env.Append(CXXFLAGS=["", "-fexceptions", "-std=c++20", "-pthread", "-fPIC"])
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