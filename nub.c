#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int run(char **args) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }
    if(pid==0){
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    }
    int status;
    waitpid(pid,&status,0);
    return WEXITSTATUS(status);
}

void build(const char *name) {
    char src[256], obj[256], so[256], efi[256];

    snprintf(src, sizeof(src), "%s.c",   name);
    snprintf(obj, sizeof(obj), "%s.o",   name);
    snprintf(so,  sizeof(so),  "%s.so",  name);
    snprintf(efi, sizeof(efi), "%s.efi", name);

    char *compile[] = {
        "gcc",
        "-ffreestanding", "-fno-stack-protector", "-fpic",
        "-fshort-wchar", "-mno-red-zone",
        "-I/home/nullora/gnu-efi/inc", "-I/home/nullora/gnu-efi/inc/x86_64",
        "-DEFI_FUNCTION_WRAPPER",
        "-c", src, "-o", obj,
        NULL
    };

    char *link[] = {
        "ld",
        "-nostdlib", "-znocombreloc",
        "-T", "/usr/lib/elf_x86_64_efi.lds",
        "-shared", "-Bsymbolic",
        "/usr/lib/crt0-efi-x86_64.o", obj,
        "-o", so,
        "-L/usr/lib", "-lefi", "-lgnuefi",
        NULL
    };

    char *objcopy[] = {
        "objcopy",
        "-j", ".text", "-j", ".sdata", "-j", ".data", "-j", ".dynamic",
        "-j", ".dynsym", "-j", ".rel", "-j", ".rela", "-j", ".reloc",
        "--target=efi-app-x86_64",
        so, efi,
        NULL
    };

    char esp[256];
    snprintf(esp, sizeof(esp), "esp/EFI/BOOT/%s.EFI", name);

    printf("[1/4] compiling %s...\n", src);
    if (run(compile) != 0) { fprintf(stderr, "[-] compile failed\n"); exit(1); }

    printf("[2/4] linking...\n");
    if (run(link) != 0) { fprintf(stderr, "[-] link failed\n"); exit(1); }

    printf("[3/4] objcopy...\n");
    if (run(objcopy) != 0) { fprintf(stderr, "[-] objcopy failed\n"); exit(1); }

    printf("[4/4] cleanup...\n");
    printf("[+] done -> %s\n", esp);
}

void clean(const char *name) {
    char files[4][256];
    snprintf(files[0], 256, "%s.o",   name);
    snprintf(files[1], 256, "%s.so",  name);
    snprintf(files[2], 256, "%s.efi", name);
    snprintf(files[3], 256, "%s.elf", name);


    for (int i = 0; i < 3; i++) {
        if (remove(files[i]) == 0)
            printf("[-] removed %s\n", files[i]);
    }
    printf("[+] clean done\n");
}
void qemu(const char *name){
    char efi[256];
    snprintf(efi, sizeof(efi), "%s.efi", name);
    char *qemu[] = {
        "qemu-system-x86_64",
        "-bios",
        "/usr/share/ovmf/OVMF.fd",
        "-drive",
        "format=raw,file=fat:rw:./esp",
        NULL
    };
    char *cop[] = {
        "cp",
        efi,
        "esp/EFI/BOOT/BOOTX64.EFI",
        NULL
    };
    run(cop);
    run(qemu);
}
void flash(const char *name) {
    char efi[256];
    snprintf(efi, sizeof(efi), "%s.efi", name);

    char *mount[] = { "mount", "/dev/sda1", "/mnt", NULL };
    char *copy[]  = { "cp", efi, "/mnt/EFI/BOOT/BOOTX64.EFI", NULL };
    char *umount[] = { "umount", "/mnt", NULL };

    printf("[1/3] mounting...\n");
    if (run(mount) != 0) { fprintf(stderr, "[-] mount failed\n"); exit(1); }

    printf("[2/3] copying...\n");
    if (run(copy) != 0) { fprintf(stderr, "[-] copy failed\n"); exit(1); }

    printf("[3/3] unmounting...\n");
    run(umount);

    printf("[+] flashed -> /dev/sda1\n");
}
void kernel(const char *name) {
    char src[256], obj[256], elf[256];
    snprintf(src, sizeof(src), "%s.c",   name);
    snprintf(obj, sizeof(obj), "%s.o",   name);
    snprintf(elf, sizeof(elf), "%s.elf", name);

    char *compile[] = {
        "gcc",
        "-ffreestanding", "-fno-stack-protector", "-fno-pic",
        "-mno-red-zone", "-mno-mmx", "-mno-sse", "-mno-sse2",
        "-c", src, "-o", obj,
        NULL
    };

    char *link[] = {
        "ld",
        "-nostdlib", "-static",
        "-Ttext", "0x100000",
        obj, "-o", elf,
        NULL
    };

    printf("[1/2] compiling %s...\n", src);
    if (run(compile) != 0) { fprintf(stderr, "[-] compile failed\n"); exit(1); }

    printf("[2/2] linking...\n");
    if (run(link) != 0) { fprintf(stderr, "[-] link failed\n"); exit(1); }

    printf("[+] done -> %s\n", elf);
}
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage: nub <command> <target>\n");
        fprintf(stderr, "  nub make prog\n");
        fprintf(stderr, "  nub clean prog\n");
        fprintf(stderr, "  nub run prog\n");
        fprintf(stderr, "  nub flash prog\n");
        fprintf(stderr, "  nub kernel prog\n");
        return 1;
    }

    char *cmd    = argv[1];
    char *target = argv[2];

    if      (strcmp(cmd, "make")  == 0) build(target);
    else if (strcmp(cmd, "clean") == 0) clean(target);
    else if (strcmp(cmd, "run")==0) qemu(target);
    else if (strcmp(cmd, "flash")==0) flash(target);
    else if (strcmp(cmd, "kernel")==0) kernel(target);
    else {
        fprintf(stderr, "unknown command: %s\n", cmd);
        return 1;
    }

    return 0;
}