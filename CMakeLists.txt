cmake_minimum_required(VERSION 3.8)
project(OperationSystemsCourse)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Term.c Print.c Find.c JITCompiler.c MultiplyByTwo.c AddFour.c hw4/Main.c hw4/StaticLib.c hw4/FirstDynamicLib.c hw4/SecondDynamicLib.c)
add_executable(OperationSystemsCourse ${SOURCE_FILES})