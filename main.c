#include <psp2/display.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "graphics.h"

#define printf psvDebugScreenPrintf

int WriteFile(char *file, void *buf, int size) {
	SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}

int main(int argc, char *argv[]) {
	int i;
	void *buf = malloc(0x100);

	psvDebugScreenInit();

	for (i = 0; i < 15; i++) {
		printf("Unmounting partition with id 0x%X\n", (i * 0x100));
		vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		printf("Mounting partition 0x%X with RW permissions...\n", (i * 0x100));
		_vshIoMount(i * 0x100, 0, 1, buf); // id, unk, permission, work_buffer
	}

	printf("\n\nAuto-exiting in 5 seconds...");

	sceKernelDelayThread(5 * 1000 * 1000);
	sceKernelExitProcess(0);

	return 0;
}
