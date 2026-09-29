#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

static char resultBuffer[512];
static int scrollOffset = 0;

// Function 1: Displays text statically if it fits, otherwise scrolls it.
//
// param1: Formatted as "[width]/[text]"
//         Example: "7/Chance of Rain"
//         - width = number of designated character slots (7 in the example)
//         - text  = the string to display ("Chance of Rain")
//
// param2: The number of characters to scroll per update (e.g. "2").
//         If omitted or invalid, defaults to 1.
DLL_EXPORT char* __stdcall function1(char* param1, char* param2) {
    // --- Parse param1 into width and text ---
    int width = 20;                 // default width if parsing fails
    char* text = param1;
    char param1Copy[1024];

    if (param1 != NULL && param1[0] != '\0') {
        strncpy(param1Copy, param1, sizeof(param1Copy) - 1);
        param1Copy[sizeof(param1Copy) - 1] = '\0';

        char* slash = strchr(param1Copy, '/');
        if (slash != NULL) {
            *slash = '\0';                   // terminate the width part
            int parsedWidth = atoi(param1Copy);
            if (parsedWidth > 0) {
                width = parsedWidth;
            }
            text = slash + 1;                // everything after the slash
        } else {
            // No slash found: treat the whole parameter as text, keep default width
            text = param1Copy;
        }
    } else {
        // Empty or NULL param1
        strcpy(resultBuffer, "");
        scrollOffset = 0;
        return resultBuffer;
    }

    // --- Parse param2 for scroll step ---
    int step = 1;
    if (param2 != NULL && param2[0] != '\0') {
        int parsedStep = atoi(param2);
        if (parsedStep > 0) {
            step = parsedStep;
        }
    }

    // If the text is empty, just return empty.
    if (text[0] == '\0') {
        strcpy(resultBuffer, "");
        scrollOffset = 0;
        return resultBuffer;
    }

    int textLength = (int)strlen(text);

    // If the text fits within the width, display it statically.
    if (textLength <= width) {
        strncpy(resultBuffer, text, sizeof(resultBuffer) - 1);
        resultBuffer[sizeof(resultBuffer) - 1] = '\0';
        scrollOffset = 0; // reset scroll position
        return resultBuffer;
    }

    // --- Build the padded string for scrolling ---
    static char paddedText[2048];
    int currentLen = 0;

    strncpy(paddedText, text, sizeof(paddedText) - 1);
    paddedText[sizeof(paddedText) - 1] = '\0';
    currentLen = (int)strlen(paddedText);

    // Number of spaces = width + (step - 1).
    //
    // This guarantees at least `step` consecutive fully-empty frames,
    // so the display will always show a blank window before the text
    // reappears, regardless of the step size.
    int spacesToAdd = width + (step - 1);
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

    // Extract the visible window.
    int i;
    for (i = 0; i < width; i++) {
        int index = (scrollOffset + i) % paddedLength;
        resultBuffer[i] = paddedText[index];
    }
    resultBuffer[width] = '\0';

    // Advance by the requested step size.
    scrollOffset += step;
    return resultBuffer;
}

DLL_EXPORT void __stdcall SmartieInit() { scrollOffset = 0; }
DLL_EXPORT void __stdcall SmartieFini() {}
