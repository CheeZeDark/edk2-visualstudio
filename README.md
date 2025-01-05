# edk2-visualstudio

Documentation on how to setup an edk2 environment, build it using MSVC and how to create a Visual Studio C++ project to develop UEFI applications.

Inspired by Alex Ionescu's [VisualUefi](https://github.com/ionescu007/VisualUefi) project, but updated 

## edk2 Setup

### 1. Setting up edk2 environment

Refer to this article: https://github.com/tianocore/tianocore.github.io/wiki/Windows-systems

Steps:
- Install `nasm` from https://www.nasm.us, installer is available, run the installer as admin.
- Set environment variable `NASM_PREFIX` to `C:\Program Files\NASM\`.
- Download ACPICA from https://www.intel.com/content/www/us/en/developer/topic-technology/open/acpica/download.html, download Windows Binary Tools.
- Unzip the contents of `iasl-win-20240927.zip` to `C:\ASL`.
- Install Python for all users and add python to environment variables.
- Set environment variable `PYTHON_HOME` to `C:\Program Files\Python313` (replace `313` with your installed version).
- Make sure you have a stable internet connection, in case cloning the repo and the submodules fail, you might have to start that command all over again.
- Clone edk2 recursively to `C:\edk2`: `git clone --recurse-submodules -j8 https://github.com/tianocore/edk2`.
- Set `C:\edk2` as the working directory.
- Run `git submodule update --init --recursive` (to verify all submodules are setup properly, result of the command should be empty).
- Run `edksetup.bat Rebuild`.
- Edit target configuration `notepad Conf\target.txt`: change `TARGET_ARCH` to `X64`, `TOOL_CHAIN_TAG` to `VS2022`.
- Run `edksetup.bat Rebuild` again.

### 2. Compiling edk2

- Set `C:\edk2` as the working directory.
- Edit target configuration using `notepad Conf\target.txt`.
- Run `edksetup`.
- Run `build -p MdeModulePkg/MdeModulePkg.dsc`.
- Run `build -p MdePkg/MdePkg.dsc`.
- Run `build -p ShellPkg/ShellPkg.dsc`.
- Run `build -p  UnitTestFrameworkPkg\UnitTestFrameworkPkg.dsc` (optional).
- Run the following PowerShell command: `Get-ChildItem -Path "C:\edk2\Build\*" -Include *.lib -Recurse | Copy-Item -Destination C:\edk2\build_lib\`.

### Visual C++ UEFI project setup
- Create an Empty C++ project inside Visual Studio.
- Set target as Release and x64.
- Set the following project options:
  - (Advanced) Target file extension: .efi
  - (VC++ Directories) Include directories: `C:\edk2\MdeModulePkg\Include;C:\edk2\MdePkg\Include;C:\edk2\MdePkg\Include\X64;C:\edk2\ShellPkg\Include;C:\edk2\CryptoPkg\Include`
  - (VC++ Directories) Library directories: `C:\edk2\build_lib`
  - (C/C++>Optimization) Omit frame pointers: Yes?
  - (C/C++>Preprocessor) Preprocessor definitions: `NDEBUG;%(PreprocessorDefinitions)`
  - (C/C++>Code Generation) Enable C++ exceptions: No, Security check: Disable security check, Control Flow Guard: No
  - (C/C++>All Options) SDL Checks: '' (set to empty)
  - (Linker>General) Enable incremental linking: No
  - (Linker>Input) Additional dependencies: `UefiHiiLib.lib;UefiHiiServicesLib.lib;UefiSortLib.lib;UefiShellLib.lib;BaseLib.lib;BaseDebugPrintErrorLevelLib.lib;BasePrintLib.lib;UefiLib.lib;UefiBootServicesTableLib.lib;UefiRuntimeServicesTableLib.lib;UefiDevicePathLibDevicePathProtocol.lib;UefiDebugLibConOut.lib;UefiMemoryLib.lib;UefiMemoryAllocationLib.lib;BaseSynchronizationLib.lib;UefiFileHandleLib.lib;UefiApplicationEntryPoint.lib;`
  - (Linker>Input) Ignore all default libraries: Yes
  - (Linker>Manifest File) Generate manifest: No
  - (Linker>System) Subsystem: EFI Application
  - (Linker>Advanced) Entry point: `EfiMain`, Randomized base address: No, DEP: No
- Copy the following files from `C:\edk2\Build\Shell\RELEASE_VS2022\X64\ShellPkg\Application\Shell\Shell\DEBUG`: `AutoGen.c`, `AutoGen.h`, `ShellStrDefs.h` and include them in the project.
- Add `#include "AutoGen.h"` at the start of `AutoGen.c`.
- Comment out `gEfiCallerBaseName` definition inside AutoGen, and define those inside your own project.
- Inside `AutoGen.c` change `ProcessLibraryConstructorList` to Snippet 1, and make `ProcessLibraryDestructorList` empty.
- Compile the project and add missing PCDs from other compiled AutoGen files (search for the missing definitions inside Build folder).
- Compilation should be successful.

(Example AutoGen files are included in this project.)

## Adding additional libraries

In case you want to use additional libraries (like UefiBootManagerLib), you need to add the relevant libraries to Additional dependencies inside Linker settings, add the relevant constructors to `ProcessLibraryConstructorList`, fix missing PCDs and Guids and add any other missing definitions or libraries.

## Snippets

### Snippet 1

```
VOID
EFIAPI
ProcessLibraryConstructorList(
	IN EFI_HANDLE        ImageHandle,
	IN EFI_SYSTEM_TABLE* SystemTable
)
{
	EFI_STATUS  Status;

	Status = DxeDebugLibConstructor(ImageHandle, SystemTable);
	ASSERT_EFI_ERROR(Status);

	Status = UefiBootServicesTableLibConstructor(ImageHandle, SystemTable);
	ASSERT_EFI_ERROR(Status);

	Status = UefiRuntimeServicesTableLibConstructor(ImageHandle, SystemTable);
	ASSERT_EFI_ERROR(Status);

	Status = UefiLibConstructor(ImageHandle, SystemTable);
	ASSERT_EFI_ERROR(Status);

	Status = DevicePathLibConstructor(ImageHandle, SystemTable);
	ASSERT_EFI_ERROR(Status);

	Status = UefiHiiServicesLibConstructor(ImageHandle, SystemTable);
	ASSERT_EFI_ERROR(Status);

}
```