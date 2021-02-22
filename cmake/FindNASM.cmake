include(FindPackageHandleStandardArgs)

find_program(NASM_EXECUTABLE NAMES nasm yasm DOC "Path to NASM/YASM executable")

if (NOT NASM_EXECUTABLE OR NASM_EXECUTABLE STREQUAL NASM_EXECUTABLE-NOTFOUND)
    if(WIN32)
        # Search for NASM in a few places:
        # - %LOCALAPPDATA% (if set)
        # - %ProgramFiles(x86)%/NASM (if set)
        # - %ProgramFiles%/NASM

        if(DEFINED ENV{LOCALAPPDATA})
            # Use LOCALAPPDATA.
            # TODO: Determine LOCALAPPDATA on XP?
            # NASM only uses it on Win7 (and probably Vista), though...
            set(LocalAppData "$ENV{LOCALAPPDATA}")
        endif()

        # NOTE: Due to CMP0053, we can't directly reference
        # the "ProgramFiles(x86)" variable.
        if(DEFINED ENV{ProgramW6432})
            # 64-bit Windows.
            # Use %ProgramFiles(x86)% for 32-bit,
            # and %ProgramW6432% for 64-bit.
            set(MYENV "ProgramFiles(x86)")
            set(ProgramFilesX86 "$ENV{${MYENV}}")
            set(ProgramFiles "$ENV{ProgramW6432}")
            unset(MYENV)
        else()
            # 32-bit Windows. Use %ProgramFiles% directly.
            set(ProgramFiles "$ENV{ProgramFiles}")
        endif()

        # Replace backslashes with slashes.
        STRING(REPLACE "\\" "/" LocalAppData    "${LocalAppData}")
        STRING(REPLACE "\\" "/" ProgramFiles    "${ProgramFiles}")
        STRING(REPLACE "\\" "/" ProgramFilesX86 "${ProgramFilesX86}")

        # Find NASM.
        find_program(NASM_EXECUTABLE NAMES nasm yasm
                DOC	"Path to NASM/YASM executable"
                PATHS	"${LocalAppData}/NASM_EXECUTABLE"
                "${ProgramFiles}/NASM_EXECUTABLE"
                "${ProgramFilesX86}/NASM_EXECUTABLE"
                )
    endif()
endif()

find_package_handle_standard_args(NASM
        REQUIRED_VARS NASM_EXECUTABLE
        FAIL_MESSAGE "NASM/YASM is required"
)
