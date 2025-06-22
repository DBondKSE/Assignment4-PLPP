#include "algorithm.h"
#include <stdlib.h>
#include <string.h>

__attribute__((visibility("default"))) char *encrypt(char *rawText, int key) {
    char *encrypted = malloc(strlen(rawText) + 1);
    for (int i = 0; i < strlen(rawText); i++) {
        char s = *(rawText + i);
        *(encrypted + i) =
                (s >= 65 && s <= 90) || (s >= 97 && s <= 122)
                    ? s <= 90
                          ? 65 + (s - 65 + key) % 26
                          : 97 + (s - 97 + key) % 26
                    : s;
    }
    *(encrypted + strlen(rawText)) = '\0';
    return encrypted;
}

__attribute__((visibility("default"))) char *decrypt(char *encryptedText, int key) {
    char *decrypted = malloc(strlen(encryptedText) + 1);
    for (int i = 0; i < strlen(encryptedText); i++) {
        char s = *(encryptedText + i);
        *(decrypted + i) =
                (s >= 65 && s <= 90) || (s >= 97 && s <= 122)
                    ? s <= 90
                          ? 65 + (s - 39 - key) % 26
                          : 97 + (s - 71 - key) % 26
                    : s;
    }
    *(decrypted + strlen(encryptedText)) = '\0';
    return decrypted;
}
