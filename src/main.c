#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Please type correct entering.\n");
    } else if (argc == 3 && !strcmp(argv[1], "myhuffman")) {
        if (compress(argv[2], NULL)) {
            printf("File not found!\n");
            exit(-1);
        } else {
            printf("Compress succeed!\n");
        }
    } else if (argc == 4 && !strcmp(argv[1], "myhuffman")) {
        if (strcmp(argv[2], "-z") == 0) {
            if (compress(argv[3], NULL)) {
                printf("File not found!\n");
                exit(-1);
            } else {
                printf("Compress succeed!\n");
            }
        } else if (strcmp(argv[2], "-u") == 0) {
            if (strcmp((argv[3] + ((int) strlen(argv[3])) - 5), ".huff") != 0) {
                printf("It is not a huffman file!\n");
                exit(-1);
            } else {
                if (uncompress(argv[3], NULL)) {
                    printf("File not found!\n");
                    exit(-1);
                } else {
                    printf("Uncompress succeed!\n");
                }
            }
        } else {
            printf("Command not found!\n");
        }
    } else if (argc == 5 && !strcmp(argv[1], "myhuffman")) {
        if (strcmp(argv[2], "-zr") == 0) {
            if (compress(argv[3], argv[4])) {
                printf("File not found!\n");
                exit(-1);
            } else {
                printf("Compress succeed!\n");
            }
        } else {
            printf("Command not cound!\n");
        }
    } else {
        printf("Wrong typing.\n");
    }

}
