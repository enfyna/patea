#include "user.h"

size_t user_id = 0;

void user_set_current(size_t id)
{
    user_id = id;
}

size_t user_get_current(void)
{
    return user_id;
}
