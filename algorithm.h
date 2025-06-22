#pragma once
#ifndef ALGORITHM_H
#define ALGORITHM_H

#ifdef __cplusplus
extern "C" {
#endif

    __attribute__((visibility("default"))) char* encrypt(char* rawText, int key);
    __attribute__((visibility("default"))) char* decrypt(char* encryptedText, int key);

#ifdef __cplusplus
}
#endif

#endif
