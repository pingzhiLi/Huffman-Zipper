//
// Created by 李平治 on 2021/1/2.
//

#ifndef CODE_HUFFMAN_H
#define CODE_HUFFMAN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node {
    long weight;
    short parent, lchild, rchild;
} HuffNode, *HuffTree;


typedef struct huffman_code {
    unsigned char len;//长度
    unsigned char *codestr;
} HuffCode;


int compress(char *sourceFileName, char *objFileName);

long dataFrequency(FILE *in, long fre[]);

int createTree(long w[], int n, HuffNode ht[]);

int huffCode(HuffTree htp, int n, HuffCode hc[]);

unsigned char charsToBits(unsigned char chars[8]);

int writeCompressFile(FILE *in, FILE *out, HuffTree ht, HuffCode hc[], char *sourceFileName, long sourceFileSize);

int uncompress(char *sourceFileName, char *objFileName);

void getMiniTwo(FILE *in, short mini_ht[][2]);

int writeUncompressFile(FILE *in, FILE *out, short mini_ht[][2], long bitsPos, long objFileSize);


int compress(char *sourceFileName, char *objFileName) {
    FILE *inFile, *outFile;
    int i, j;
    HuffCode hc[256];
    HuffNode ht[256 * 2 - 1];
    long frequency[256], sourceFileSize = 0;

    if (fopen(sourceFileName, "rb") == NULL) {
        puts("ERROR!\n");
        return -1;
    }
    if (objFileName == NULL) {
        if ((objFileName = (char *) malloc(256 * sizeof(char))) == NULL) {
            puts("ERROR!\n");
            return -1;
        }
        strcpy(objFileName, sourceFileName);
        strcat(objFileName, ".huff");
    }
    if (strcmp(sourceFileName, objFileName) == 0) {
        puts("ERROR!\n");
        return -1;
    }
    if ((outFile = fopen(objFileName, "wb")) == NULL) {
        puts("ERROR!\n");
        return -1;
    }
    if ((inFile = fopen(sourceFileName, "rb")) == NULL) {
        puts("ERROR!\n");
        return -1;
    }
    sourceFileSize = dataFrequency(inFile, frequency);
    if (createTree(frequency, 256, ht) != 0) {
        puts("ERROR!\n");
        return -1;
    }

    if (huffCode(ht, 256, hc) != 0) {
        puts("ERROR!\n");
        return -1;
    }
    printf("Compressing file: %s\n", sourceFileName);
    if (writeCompressFile(inFile, outFile, ht, hc, sourceFileName, sourceFileSize) != 0) {
        puts("ERROR!\n");
        return -1;
    }
    fclose(inFile);
    fclose(outFile);
    for (i = 0; i < 256; i++) {
        free(hc[i].codestr);
    }
    return 0;
}


long dataFrequency(FILE *in, long frequency[]) {
    int i, read_len;
    unsigned char buf[256];
    long fileSize;

    for (i = 0; i < 256; i++) {
        frequency[i] = 0;
    }
    fseek(in, 0L, SEEK_SET);
    read_len = 256;

    while (read_len == 256) {
        read_len = fread(buf, 1, 256, in);
        for (i = 0; i < read_len; i++) {
            frequency[*(buf + i)]++;
        }
    }
    for (i = 0, fileSize = 0; i < 256; i++) {
        fileSize += frequency[i];
    }

    return fileSize;
}

int createTree(long w[], int n, HuffNode ht[]) {
    int m, i, j, s1, s2, x;
    long minValue, min;
    if (n < 1) return -1;
    m = 2 * n - 1;
    if (ht == NULL) return -1;
    for (i = 0; i < n; i++) {
        ht[i].weight = w[i];
        ht[i].parent = ht[i].lchild = ht[i].rchild = -1;
    }
    for (; i < m; i++) {
        ht[i].weight = ht[i].parent = ht[i].lchild = ht[i].rchild = -1;
    }
    for (i = n; i < m; i++) {
        minValue = 1145140;
        min = 0;
        for (x = 0; x < i; x++) {
            if (ht[x].parent == -1) break;
        }
        for (j = 0; j < i; j++) {
            if (ht[j].parent == -1 && ht[j].weight < minValue) {
                minValue = ht[j].weight;
                min = j;
            }
        }
        s1 = min;
        minValue = 1145140, min = 0;
        for (j = 0; j < i; j++) {
            if (ht[j].parent == -1 && ht[j].weight < minValue && j != s1) {
                minValue = ht[j].weight;
                min = j;
            }
        }
        s2 = min;
        ht[s1].parent = ht[s2].parent = i;
        ht[i].lchild = s1;
        ht[i].rchild = s2;
        ht[i].weight = ht[s1].weight + ht[s2].weight;
    }
    return 0;
}


int huffCode(HuffTree htp, int n, HuffCode hc[]) {
    int i, j, p, codelen;
    unsigned char *code = (unsigned char *) malloc(n * sizeof(unsigned char));

    if (code == NULL) return -1;
    for (i = 0; i < n; i++) {
        for (p = i, codelen = 0; p != 2 * n - 2; p = htp[p].parent, codelen++) {
            code[codelen] = (htp[htp[p].parent].lchild == p ? 0 : 1);
        }
        if ((hc[i].codestr = (unsigned char *) malloc((codelen) * sizeof(unsigned char))) == NULL) {
            return -1;
        }
        hc[i].len = codelen;
        for (j = 0; j < codelen; j++) {
            hc[i].codestr[j] = code[codelen - j - 1];
        }
    }
    free(code);
    return 0;
}


unsigned char charsToBits(unsigned char chars[8]) {
    int i;
    unsigned char bits = 0;
    bits |= chars[0];
    for (i = 1; i < 8; ++i) {
        bits <<= 1;
        bits |= chars[i];
    }
    return bits;
}


int writeCompressFile(FILE *in, FILE *out, HuffTree ht, HuffCode hc[], char *sourceFileName, long sourceFileSize) {
    unsigned int i, readCounter, writeCounter, zipHead = 0xFFFFFFFF;
    unsigned char writeCharCounter, code_char_counter, copyCharCounter,
            read_buf[256], write_buf[256], write_chars[8], filename_size = strlen(sourceFileName);
    HuffCode *cur_huffCode;
    fseek(in, 0L, SEEK_SET);
    fseek(out, 0L, SEEK_SET);
    fwrite(&zipHead, sizeof(unsigned int), 1, out);
    fwrite(&filename_size, sizeof(unsigned char), 1, out);
    fwrite(sourceFileName, sizeof(char), filename_size, out);
    fwrite(&sourceFileSize, sizeof(long), 1, out);
    for (i = 256; i < 256 * 2 - 1; i++) {
        fwrite(&(ht[i].lchild), sizeof(ht[i].lchild), 1, out);
        fwrite(&(ht[i].rchild), sizeof(ht[i].rchild), 1, out);
    }
    writeCounter = writeCharCounter = 0;
    readCounter = 256;
    while (readCounter == 256) {
        readCounter = fread(read_buf, 1, 256, in);
        for (i = 0; i < readCounter; i++) {
            cur_huffCode = &hc[read_buf[i]];
            code_char_counter = 0;
            while (code_char_counter != cur_huffCode->len) {
                copyCharCounter = (8 - writeCharCounter > (cur_huffCode->len - code_char_counter) ?
                                   (cur_huffCode->len - code_char_counter) : (8 - writeCharCounter));//min of two
                memcpy(write_chars + writeCharCounter, cur_huffCode->codestr + code_char_counter, copyCharCounter);
                writeCharCounter += copyCharCounter;
                code_char_counter += copyCharCounter;
                if (writeCharCounter == 8) {
                    writeCharCounter = 0;
                    write_buf[writeCounter++] = charsToBits(write_chars);
                    if (writeCounter == 256) {
                        fwrite(write_buf, 1, 256, out);
                        writeCounter = 0;
                    }
                }
            }
        }

    }
    fwrite(write_buf, 1, writeCounter, out);
    if (writeCharCounter != 0) {
        writeCharCounter = charsToBits(write_chars);
        fwrite(&writeCharCounter, 1, 1, out);
    }
    return 0;
}


void getMiniTwo(FILE *in, short mini_ht[][2]) {
    int i;
    for (i = 0; i < 256; i++) {
        mini_ht[i][0] = mini_ht[i][1] = -1;
    }
    fread(mini_ht[i], sizeof(short), 2 * (256 - 1), in);
}


int writeUncompressFile(FILE *in, FILE *out, short mini_ht[][2], long bitsPos, long objFileSize) {
    long cur_size;
    unsigned char read_buf[256], write_buf[256], convert_bit;
    unsigned int read_counter, write_counter, cur_pos;
    fseek(in, bitsPos, SEEK_SET);
    fseek(out, 0L, SEEK_SET);
    read_counter = 256 - 1;
    cur_size = write_counter = 0;
    cur_pos = 256 * 2 - 2;
    while (cur_size != objFileSize) {
        if (++read_counter == 256) {
            fread(read_buf, 1, 256, in);
            read_counter = 0;
        }
        for (convert_bit = 128; convert_bit != 0; convert_bit >>= 1) {
            cur_pos = ((read_buf[read_counter] & convert_bit) == 0 ? mini_ht[cur_pos][0] : mini_ht[cur_pos][1]);
            if (cur_pos < 256) {
                write_buf[write_counter] = (unsigned char) cur_pos;
                if (++write_counter == 256) {
                    fwrite(write_buf, 1, 256, out);
                    write_counter = 0;
                }
                cur_pos = 256 * 2 - 2;
                if (++cur_size == objFileSize)
                    break;
            }
        }

    }
    fwrite(write_buf, 1, write_counter, out);
    return 0;
}


int uncompress(char *sourceFileName, char *objFileName) {
    FILE *in, *out;
    short mini_ht[256 * 2 - 1][2];
    long objFileSize;
    unsigned int huffHead;
    unsigned char fileNameSize;
    if ((in = fopen(sourceFileName, "rb")) == NULL) {
        return -1;
    }
    fread(&huffHead, sizeof(unsigned int), 1, in);
    if (huffHead != 0xFFFFFFFF) {
        return -1;
    }
    if (objFileName == NULL) {
        if ((objFileName = (char *) malloc(256 * sizeof(char))) == NULL) {
            return -1;
        }
        fread(&fileNameSize, sizeof(unsigned char), 1, in);
        fread(objFileName, sizeof(char), fileNameSize, in);
        objFileName[fileNameSize] = '\0';
    } else {
        fread(&fileNameSize, sizeof(unsigned char), 1, in);
        fseek(in, fileNameSize, SEEK_CUR);
    }
    printf("Uncompressing file:%s\n", objFileName);
    if ((out = fopen(objFileName, "wb")) == NULL) {
        return -1;
    }
    fread(&objFileSize, sizeof(long), 1, in);
    getMiniTwo(in, mini_ht);
    if (writeUncompressFile(in, out, mini_ht, ftell(in), objFileSize) != 0) {
        puts("ERROR!\n");
        return -1;
    }
    fclose(in);
    fclose(out);
    return 0;
}


#endif //CODE_HUFFMAN_H
