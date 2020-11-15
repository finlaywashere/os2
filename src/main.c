#include <efi.h>
#include <efilib.h>
 
EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable){
	InitializeLib(ImageHandle, SystemTable);
	Print(L"Hello, world!\n");
	EFI_STATUS Status = ST->ConIn->Reset(ST->ConIn, FALSE);
	return EFI_SUCCESS;
}
