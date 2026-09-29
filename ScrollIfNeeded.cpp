#include <windows.h>
#include <string.h>
#include <stdio.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

// Number of characters to advance per plugin call.
#define SCROLL_STEP 2

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

    strncpy(paddedText, param1, sizeof(paddedText) - 1);
    paddedText[sizeof(paddedText) - 1] = '\0';
    currentLen = (int)strlen(paddedText);

    // Number of spaces = width + (SCROLL_STEP - 1).
    //
    // Why the extra (SCROLL_STEP - 1) spaces?
    // The display shows a "window" of `width` characters starting at
    // scrollOffset. For the display to be completely empty (all spaces),
    // scrollOffset must land inside the gap such that the whole window
    // falls within the spaces.
    //
    // With a gap of exactly `width` spaces, only ONE specific offset gives
    // a fully empty frame. If SCROLL_STEP doesn't land on that exact offset
    // (e.g. because textLength is odd and we step by 2), the display jumps
    // from "text + spaces" straight to "spaces + text" and never appears
    // fully empty.
    //
    // Adding (SCROLL_STEP - 1) extra spaces gives a gap of at least
    // SCROLL_STEP consecutive fully-empty offsets, guaranteeing we hit one
    // of them regardless of step parity.
    int spacesToAdd = width + (SCROLL_STEP - 1);
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

    // Advance by SCROLL_STEP characters per call instead of 1.
    scrollOffset += SCROLL_STEP;
    return resultBuffer;
}

DLL_EXPORT void __stdcall SmartieInit() { scrollOffset = 0; }
DLL_EXPORT void __stdcall SmartieFini() {}
