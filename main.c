#include <psp2/display.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "graphics.h"

#define printf psvDebugScreenPrintf

int cp(const char *to, const char *from)
{
    SceUID fd_to, fd_from;
    char buf[16*1024];
    ssize_t nread;
    int saved_errno;
	//
    fd_from = sceIoOpen(from, SCE_O_RDONLY, 0777);
    if (fd_from < 0)
        return -1;

    fd_to = sceIoOpen(to, SCE_O_WRONLY|SCE_O_CREAT, 0777);
    if (fd_to < 0)
        goto out_error;

    while (nread = sceIoRead(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = sceIoWrite(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (sceIoClose(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        sceIoClose(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    sceIoClose(fd_from);
    if (fd_to >= 0)
        sceIoClose(fd_to);

    errno = saved_errno;
    return -1;
}

int WriteFile(char *file, void *buf, int size) {
	SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}


int main(int argc, char *argv[]) {
	void *buf = malloc(0x100);
	
	psvDebugScreenInit();
    printf("VSOI v0.1\n\n");
    // Mount vs0 as RW
	
	printf("Unmounting partition with id 0x%X\n", 0x300); // 0x300 is vs0
	vshIoUmount(0x300, 0, 0, 0);
	
	printf("Mounting partition 0x%X with RW permissions...\n", 0x300);
	_vshIoMount(0x300, 0, 2, buf);
	
	
    // Copy VitaShell's eboot.bin to vs0:app/NPXS10000/eboot.bin
    
    // Backup Near's eboot elsewhere
    if (sceIoRemove("ux0:/data/nearEboot.bin") < 0) 
		printf("Backup eboot not found.\n");
	else
		printf("Removed existing backup eboot.\n");
		
    if (cp("ux0:/data/nearEboot.bin", "vs0:app/NPXS10000/eboot.bin") != 0)
		printf("Error backing up the eboot.\n");
	else
		printf("Eboot backup created.\n");
	
	// Remove Near's eboot and copy VitaShell's to that directory
	SceUID fd;
	fd = sceIoOpen("app0:vsEboot.bin", SCE_O_RDONLY, 0777);		
	if (fd >= 0)
	{
		printf("Using app0:vsEboot.bin\n");
		sceIoRemove("vs0:app/NPXS10000/eboot.bin");
		if (cp("vs0:app/NPXS10000/eboot.bin", "app0:vsEboot.bin") >= 0)
			printf("Successfully copied eboot to directory!\n");
		else
			printf("Error copying eboot to directory!\n");					
	}
	else
	{
		printf("ERROR: VitaShell eboot not found!\n");
	}
	
	// Back up HENkaku config
	sceIoRemove("ux0:data/ux0_config.txt");
	sceIoRemove("ux0:data/ur0_config.txt");
	printf("Backing up HENkaku config...\n");
	fd = sceIoOpen("ux0:tai/config.txt", SCE_O_RDONLY, 0777);
	SceUID fdR = sceIoOpen("ur0:tai/config.txt", SCE_O_RDONLY, 0777);
	if (fd >= 0)
	{
		sceIoClose(fd);
		printf("Found HENkaku config in ux0. Backing up.\n");
		cp("ux0:data/ux0_config.txt", "ux0:tai/config.txt");
	}
	else
	{
		printf("HENkaku config not found at ux0. Skipping.\n");
	}
	if (fdR >= 0)
	{
		sceIoClose(fdR);
		printf("Found HENkaku config in ur0. Backing up.\n");
		cp("ux0:data/ur0_config.txt", "ur0:tai/config.txt");
	}
	else
	{
		printf("HENkaku config not found at ur0. Skipping.\n");
	}	
	// Remove app.db and reboot to force db rebuild
	
	printf("Removing app.db...\n");
	
	// Back up app.db before removing
	sceIoRemove("ux0:data/app_db_bak.db");
	cp("ux0:data/app_db_bak.db", "ur0:shell/db/app.db");
	sceIoRemove("ur0:shell/db/app.db");
	

	printf("\n\nRebooting in 10 seconds...");

	sceKernelDelayThread(10 * 1000 * 1000);
	scePowerRequestColdReset();

	return 0;
}
