#pragma once

#include <stdbool.h>

#define global static
#define internal static
#define local_persist static

bool starts_with(const char* text, const char* prefix);
