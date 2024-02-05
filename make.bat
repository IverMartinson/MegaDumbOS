@echo off

REM Compiler and linker
set NASM=nasm
set GCC=gcc
set LD=ld
set GRUB_MKIMAGE=grub-mkimage.exe

REM Flags for compilation and linking
set NASM_FLAGS=-f win32
set GCC_FLAGS=-m32 -c
set LD_FLAGS=-m i386pe -T src/linker.ld

REM Source and build directories
set SRC_DIR=src
set BUILD_DIR=build
set GRUB_DIR=src\boot

REM Source files
set NASM_SOURCE=%SRC_DIR%\kernel.asm
set GCC_SOURCE=%SRC_DIR%\kernel.c

REM Object files
set NASM_OBJ=%BUILD_DIR%\kernasm.obj
set GCC_OBJ=%BUILD_DIR%\kernc.obj

REM Target binary
set TARGET=%GRUB_DIR%\kernel.exe

:all
    call :clean
    call :build
    call :iso
    goto :eof

:build
    call :compile_nasm
    call :compile_gcc
    call :link
    goto :eof

:compile_nasm
    %NASM% %NASM_FLAGS% %NASM_SOURCE% -o %NASM_OBJ%
    goto :eof

:compile_gcc
    %GCC% %GCC_FLAGS% %GCC_SOURCE% -o %GCC_OBJ%
    goto :eof

:link
    %LD% %LD_FLAGS% -o %TARGET% %NASM_OBJ% %GCC_OBJ%
    goto :eof

:clean
    del /Q %BUILD_DIR%\* %TARGET%
    goto :eof

:iso
    %GRUB_MKIMAGE% -O i386-pc -o %BUILD_DIR%\boot.img -c %GRUB_DIR%\grub.cfg %GRUB_DIR%
    copy /b %TARGET% + %BUILD_DIR%\boot.img %BUILD_DIR%\output.iso
    goto :eof
