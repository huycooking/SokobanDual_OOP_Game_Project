@echo off
set SFML_INC=sfml/include
set SFML_LIB=sfml/lib
set BASE_DIR=../
set SRC_DIR=Source
set EXE_NAME=Sokoban.exe

g++.exe -std=c++17 -s -Os -static -static-libstdc++ -static-libgcc -DSFML_STATIC %BASE_DIR%/%SRC_DIR%/*.cpp -o %BASE_DIR%/%EXE_NAME% -I%BASE_DIR%/include -I %BASE_DIR%/%SFML_INC% -L %BASE_DIR%/%SFML_LIB% -lsfml-audio-s -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lfreetype -lwinmm -lws2_32 -luser32 -lgdi32 -lopengl32 -lglu32 -lvorbisfile -lvorbis -logg -lflac

if %errorlevel% neq 0 (
    echo.
    echo Compilation failed.
) else (
    echo.
    echo Compilation successful.
)