#include <windows.h>
#include <string.h>
#include <stdio.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

static char resultBuffer[512];
static int scrollOffset = 0;

DLL_EXPORT char* __stdcall function1(char* param1, char* param2) {
    if (param1 == NULL || param1[0] == '\0') {
        strcpy(resultBuffer, "");
        scrollOffset = 0;
        return resultBuffer;
    }

    int width = 20;
    if (param2 != NULL && param2[0] != '\0') {
        int parsedWidth = atoi(param2);
        if (parsedWidth > 0) width = parsedWidth;
    }

    int textLength = (int)strlen(param1);

    if (textLength <= width) {
        strncpy(resultBuffer, param1, sizeof(resultBuffer) - 1);
        resultBuffer[sizeof(resultBuffer) - 1] = '\0';
        scrollOffset = 0;
        return resultBuffer;
    }

    static char paddedText[2048];
    int currentLen = 0;

    // Copy the text once.
    strncpy(paddedText, param1, sizeof(paddedText) - 1);
    paddedText[sizeof(paddedText) - 1] = '\0';
    currentLen = (int)strlen(paddedText);

    // Append a number of spaces equal to the designated display width.
    int spacesToAdd = width;
    if (currentLen + spacesToAdd >= (int)sizeof(paddedText)) {
        spacesToAdd = (int)sizeof(paddedText) - currentLen - 1;
    }
    if (spacesToAdd > 0) {
        memset(paddedText + currentLen, ' ', spacesToAdd);
        currentLen += spacesToAdd;
        paddedText[currentLen] = '\0';
    }

    // NOTE: We intentionally do NOT append the text again here.
    // Because the scroll wraps via modulo, appending the text a second time
    // would create the pattern TEXT SPACES TEXT TEXT SPACES TEXT ...
    // which produces two back-to-back texts with no gap every other loop.
    // Using only TEXT + SPACES gives a clean repeating pattern of
    // TEXT SPACES TEXT SPACES ...

    int paddedLength = (int)strlen(paddedText);
    if (scrollOffset >= paddedLength) scrollOffset = 0;

    int i;
    for (i = 0; i < width; i++) {
        int index = (scrollOffset + i) % paddedLength;
        resultBuffer[i] = paddedText[index];
    }
    resultBuffer[width] = '\0';

    scrollOffset++;
    return resultBuffer;
}

DLL_EXPORT void __stdcall SmartieInit() { scrollOffset = 0; }
DLL_EXPORT void __stdcall SmartieFini() {}
