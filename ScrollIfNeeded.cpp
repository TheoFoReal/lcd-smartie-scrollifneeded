#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

static char resultBuffer[512];
static int scrollOffset = 0;

// --- Plugin Template ---
// Returned when the plugin is selected in LCD Smartie's plugin list.
// This is the string that gets inserted/shown for the plugin.
DLL_EXPORT char* __stdcall SmartieAbout() {
    static const char* templateText =
        "$dll(ScrollIfNeeded,1,[length]/[text],[speed]/[empty frames])";
    return (char*)templateText;
}

DLL_EXPORT char* __stdcall function1(char* param1, char* param2) {
    // --- Parse param1 into width and text ---
    int width = 20;
    char* text = param1;
    char param1Copy[1024];

    if (param1 != NULL && param1[0] != '\0') {
        strncpy(param1Copy, param1, sizeof(param1Copy) - 1);
        param1Copy[sizeof(param1Copy) - 1] = '\0';

        char* slash = strchr(param1Copy, '/');
        if (slash != NULL) {
            *slash = '\0';
            int parsedWidth = atoi(param1Copy);
            if (parsedWidth > 0) {
                width = parsedWidth;
            }
            text = slash + 1;
        } else {
            text = param1Copy;
        }
    } else {
        strcpy(resultBuffer, "");
        scrollOffset = 0;
        return resultBuffer;
    }

    // --- Parse param2 into step and emptyFrames ---
    // Format: "[step]/[emptyFrames]"
    // If only one number is given, it is used as step and emptyFrames = 1.
    int step = 1;
    int emptyFrames = 1;
    char param2Copy[64];

    if (param2 != NULL && param2[0] != '\0') {
        strncpy(param2Copy, param2, sizeof(param2Copy) - 1);
        param2Copy[sizeof(param2Copy) - 1] = '\0';

        char* slash2 = strchr(param2Copy, '/');
        if (slash2 != NULL) {
            *slash2 = '\0';
            int parsedStep = atoi(param2Copy);
            if (parsedStep > 0) {
                step = parsedStep;
            }
            int parsedEmpty = atoi(slash2 + 1);
            if (parsedEmpty > 0) {
                emptyFrames = parsedEmpty;
            }
        } else {
            int parsedStep = atoi(param2Copy);
            if (parsedStep > 0) {
                step = parsedStep;
            }
        }
    }

    if (text[0] == '\0') {
        strcpy(resultBuffer, "");
        scrollOffset = 0;
        return resultBuffer;
    }

    int textLength = (int)strlen(text);

    // If the text fits, display it statically.
    if (textLength <= width) {
        strncpy(resultBuffer, text, sizeof(resultBuffer) - 1);
        resultBuffer[sizeof(resultBuffer) - 1] = '\0';
        scrollOffset = 0;
        return resultBuffer;
    }

    // --- Build the padded string for scrolling ---
    static char paddedText[2048];
    int currentLen = 0;

    strncpy(paddedText, text, sizeof(paddedText) - 1);
    paddedText[sizeof(paddedText) - 1] = '\0';
    currentLen = (int)strlen(paddedText);

    // Number of spaces = width + (emptyFrames * step) - 1.
    int spacesToAdd = width + (emptyFrames * step) - 1;
    if (currentLen + spacesToAdd >= (int)sizeof(paddedText)) {
        spacesToAdd = (int)sizeof(paddedText) - currentLen - 1;
    }
    if (spacesToAdd > 0) {
        memset(paddedText + currentLen, ' ', spacesToAdd);
        currentLen += spacesToAdd;
        paddedText[currentLen] = '\0';
    }

    int paddedLength = (int)strlen(paddedText);
    if (scrollOffset >= paddedLength) scrollOffset = 0;

    int i;
    for (i = 0; i < width; i++) {
        int index = (scrollOffset + i) % paddedLength;
        resultBuffer[i] = paddedText[index];
    }
    resultBuffer[width] = '\0';

    scrollOffset += step;
    return resultBuffer;
}

DLL_EXPORT void __stdcall SmartieInit() { scrollOffset = 0; }
DLL_EXPORT void __stdcall SmartieFini() {}
