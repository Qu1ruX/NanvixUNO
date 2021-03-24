#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 

void main()
{
    /*while (1) {
        FILE *fp = fopen("/proc/15720/fd/1", "r");
        char c = fgetc(fp);
        while (c != EOF) {
            printf("%c", c);
            c = fgetc(fp);
        }
    }*/
    FILE *fp = fopen("/proc/28780/fd/0", "w+");
    fputs("root\n", fp);
    //fputs("root\n", fp);
    //fclose(fp);
    // for (int i = 0; i < 255; i++) {
    //     for (int j = 0; j < 255; j++) {
    //         for (int k = 0; k < 255; k++) {
    //             for (int l = 0; l < 255; l++) {
    //                 char str[4] = {i, j, k, l};
    //                 FILE *fp = fopen("/proc/15720/fd/0", "w");
    //                 fputs(str, fp);
    //                 fclose(fp);
    //             }
    //         }
    //     }
    // }
}