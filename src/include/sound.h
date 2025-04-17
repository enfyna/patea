#pragma once

#include "glib.h"

#include "miniaudio.h"

void sound_init(void);
void sound_uninit(void);

void sound_play(GBytes* sound_data);
