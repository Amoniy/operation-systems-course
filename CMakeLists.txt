cmake_minimum_required(VERSION 3.8)
project(OperationSystemsCourse)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES hw1/Term.c Print.c hw2/Find.c hw3/JITCompiler.c hw3/MultiplyByTwo.c hw3/AddFour.c hw4/Main.c hw4/StaticLib.c hw4/FirstDynamicLib.c hw4/SecondDynamicLib.c hw5/Client.c hw5/Server.c hw6/Client.c hw6/Server.c hw8/Handler.c asm/1Byte.cpp asm/8Byte.cpp asm/16Byte.cpp asm/16ByteAligned.cpp asm/WordCount.cpp)
add_executable(OperationSystemsCourse ${SOURCE_FILES})