// Fixes missing CRT and edk2 functions for compiling the project

// If we include the edk2 library that defines `UnitTestDebugAssert`,
// we introduce a bunch of missing definitions.
// Since the lack of function doesn't affect functionality,
// we just define it to be empty.

#include "AutoGen.h"
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>

extern "C" {
    VOID EFIAPI UnitTestDebugAssert(IN CONST CHAR8* FileName,IN UINTN LineNumber,IN CONST CHAR8* Description) {}

    VOID* __security_cookie = (VOID*)(UINTN)STACK_COOKIE_VALUE;
    VOID EFIAPI __security_check_cookie(IN UINTN CheckValue) {}

    void __GSHandlerCheck() {}
}