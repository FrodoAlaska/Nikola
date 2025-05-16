#pragma once

#include "nikola_base.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// *** Audio ***

///---------------------------------------------------------------------------------------------------------------------
/// Consts

const sizei AUDIO_QUEUE_BUFFERS_MAX = 32;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBufferFormat
enum AudioBufferFormat {
  AUDIO_BUFFER_FORMAT_U8  = 20 << 0,  
  AUDIO_BUFFER_FORMAT_I16 = 20 << 1,  
  AUDIO_BUFFER_FORMAT_F32 = 20 << 2,  
};
/// AudioBufferFormat
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer
struct AudioBuffer;
/// AudioBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSourceID
typedef u32 AudioSourceID;
/// AudioBufferID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBufferDesc
struct AudioBufferDesc {
  AudioBufferFormat format; 

  u32 channels; 
  u32 sample_rate;

  sizei size = 0;
  void* data = nullptr;
};
/// AudioBufferDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Audio device functions

NIKOLA_API bool audio_device_init(const char* device_name);

NIKOLA_API void audio_device_shutdown();

/// Audio device functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer functions

NIKOLA_API AudioBuffer* audio_buffer_create(const AudioBufferDesc& desc);

NIKOLA_API void audio_buffer_destroy(AudioBuffer* buffer);

NIKOLA_API AudioBufferDesc& audio_buffer_get_desc(AudioBuffer* buffer);

NIKOLA_API void audio_buffer_update(AudioBuffer* buffer, const AudioBufferDesc& desc);

/// AudioBuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSource functions

NIKOLA_API AudioSourceID audio_source_create(const AudioBuffer* buffer);

NIKOLA_API void audio_source_destroy(AudioSourceID source);

NIKOLA_API void audio_source_start(AudioSourceID source);

NIKOLA_API void audio_source_stop(AudioSourceID source);

NIKOLA_API void audio_source_restart(AudioSourceID source);

NIKOLA_API void audio_source_pause(AudioSourceID source);

NIKOLA_API void audio_source_queue_buffers(AudioSourceID source, const AudioBuffer** buffers, const sizei count);

NIKOLA_API bool audio_source_is_playing(AudioSourceID source);

NIKOLA_API void audio_source_set_buffer(AudioSourceID source, AudioBuffer* buffer);

NIKOLA_API void audio_source_set_volume(AudioSourceID source, const f32 volume);

NIKOLA_API void audio_source_set_pitch(AudioSourceID source, const f32 pitch);

NIKOLA_API void audio_source_set_looping(AudioSourceID source, const bool looping);

NIKOLA_API void audio_source_set_position(AudioSourceID source, const f32 x, const f32 y, const f32 z);

NIKOLA_API void audio_source_set_velocity(AudioSourceID source, const f32 x, const f32 y, const f32 z);

NIKOLA_API void audio_source_set_direction(AudioSourceID source, const f32 x, const f32 y, const f32 z);

/// AudioSource functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Audio ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
