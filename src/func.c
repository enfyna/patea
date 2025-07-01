#include <stddef.h>
#include <string.h>

#include "func.h"

bool starts_with(const char* text, const char* prefix)
{
    if (text == NULL || prefix == NULL) {
        return false;
    }

    size_t ln_text = strlen(text);
    size_t ln_pref = strlen(prefix);

    if (ln_pref > ln_text) {
        return false;
    }

    for (size_t i = 0; i < ln_pref; i++) {
        if (text[i] != prefix[i]) {
            return false;
        }
    }

    return true;
}
