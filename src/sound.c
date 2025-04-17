#include <assert.h>

#include "sound.h"

ma_engine engine;

typedef struct {
    ma_decoder decoder;
    GBytes* bytes;
} SoundData;

void sound_init(void)
{
    ma_engine_config engineConfig = ma_engine_config_init();
    ma_result result = ma_engine_init(&engineConfig, &engine);
    if (result != MA_SUCCESS) {
        g_error("[SOUND] Can't init sound.\n");
    }
}

void sound_uninit(void)
{
    ma_engine_uninit(&engine);
}

static void cb_miniaudio(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}

void sound_play(GBytes* sound_data)
{
    assert(sound_data != NULL);

    gsize size;
    const guint8* data = g_bytes_get_data(sound_data, &size);

    SoundData* sdata = g_malloc0(sizeof(SoundData));
    sdata->bytes = sound_data;

    if (ma_decoder_init_memory(data, size, NULL, &sdata->decoder) != MA_SUCCESS) {
        g_printerr("[SOUND] Failed to init decoder from memory.\n");
        g_free(sdata);
        return;
    }

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = sdata->decoder.outputFormat;
    deviceConfig.playback.channels = sdata->decoder.outputChannels;
    deviceConfig.sampleRate = sdata->decoder.outputSampleRate;
    deviceConfig.dataCallback = cb_miniaudio;
    deviceConfig.pUserData = sdata;

    ma_device* device = g_malloc0(sizeof(ma_device));

    if (ma_device_init(NULL, &deviceConfig, device) != MA_SUCCESS) {
        g_printerr("Failed to init playback device.\n");
        ma_decoder_uninit(&sdata->decoder);
        g_free(sdata);
        g_free(device);
        return;
    }

    if (ma_device_start(device) != MA_SUCCESS) {
        g_printerr("Failed to start playback device.\n");
        ma_device_uninit(device);
        ma_decoder_uninit(&sdata->decoder);
        g_free(sdata);
        g_free(device);
        return;
    }
}
