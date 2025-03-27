#include <string.h>

#include "func.h"

bool starts_with(const char* text, const char* prefix)
{
    if (text == NULL || prefix == NULL) {
        return false;
    }

    unsigned long ln_text = strlen(text);
    unsigned long ln_pref = strlen(prefix);

    if (ln_pref > ln_text) {
        return false;
    }

    for (unsigned long i = 0; i < ln_pref; i++) {
        if (text[i] != prefix[i]) {
            return false;
        }
    }

    return true;
}
