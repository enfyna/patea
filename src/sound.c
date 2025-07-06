#include <assert.h>
#include <glib.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "sound.h"
#include "utils.h"

#define SOUND_BUF_SIZE 8

typedef struct {
    ma_device* device;
    ma_decoder decoder;
    gboolean in_use;
    GBytes* bytes;
} SoundData;

global ma_engine engine;
global SoundData* sounds[SOUND_BUF_SIZE] = { 0 };

void sound_init(void)
{
    ma_engine_config engineConfig = ma_engine_config_init();
    if (ma_engine_init(&engineConfig, &engine) != MA_SUCCESS) {
        g_error("[SOUND] Can't init sound.\n");
    }
    for (size_t i = 0; i < SOUND_BUF_SIZE; i++) {
        sounds[i] = g_malloc0(sizeof(SoundData));
        sounds[i]->device = g_malloc0(sizeof(ma_device));
        sounds[i]->in_use = FALSE;
    }
}

void sound_uninit(void)
{
    for (size_t i = 0; i < SOUND_BUF_SIZE; i++) {
        if (sounds[i]->in_use) {
            ma_device_uninit(sounds[i]->device);
            ma_decoder_uninit(&sounds[i]->decoder);
        }
        g_free(sounds[i]->device);
        g_free(sounds[i]);
    }
    ma_engine_uninit(&engine);
}

internal void
cb_miniaudio(ma_device* device, void* output, const void* input, ma_uint32 frameCount)
{
    (void)input;

    SoundData* sdata = (SoundData*)device->pUserData;
    if (sdata == NULL)
        return;

    ma_decoder_read_pcm_frames(&sdata->decoder, output, frameCount, NULL);
}

void sound_play(GBytes* sound_data)
{
    assert(sound_data != NULL);

    local_persist size_t sound_current = 0;
    sound_current = (sound_current + 1) % SOUND_BUF_SIZE;

    SoundData* sdata = sounds[sound_current];

    if (sdata->in_use) {
        ma_device_uninit(sdata->device);
        ma_decoder_uninit(&sdata->decoder);
    }

    sdata->bytes = sound_data;
    sdata->in_use = TRUE;

    const guint8* data = g_bytes_get_data(sound_data, NULL);
    gsize size = g_bytes_get_size(sound_data);

    if (ma_decoder_init_memory(data, size, NULL, &sdata->decoder) != MA_SUCCESS) {
        g_printerr("[SOUND] Failed to init decoder.\n");
        return;
    }

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = sdata->decoder.outputFormat;
    deviceConfig.playback.channels = sdata->decoder.outputChannels;
    deviceConfig.sampleRate = sdata->decoder.outputSampleRate;
    deviceConfig.dataCallback = cb_miniaudio;
    deviceConfig.pUserData = sdata;

    if (ma_device_init(NULL, &deviceConfig, sdata->device) != MA_SUCCESS) {
        g_printerr("[SOUND] Failed to init device.\n");
        ma_decoder_uninit(&sdata->decoder);
        return;
    }

    if (ma_device_start(sdata->device) != MA_SUCCESS) {
        g_printerr("[SOUND] Failed to start device.\n");
        ma_device_uninit(sdata->device);
        ma_decoder_uninit(&sdata->decoder);
        return;
    }

    g_print("[SOUND] Playing: %p.\n", (void*)sound_data);
}
