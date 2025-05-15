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
/// AudioBufferType
enum AudioBufferType {
  AUDIO_BUFFER_PLAYBACK = 20 << 0,
  AUDIO_BUFFER_CAPTURE  = 20 << 1, 
  AUDIO_BUFFER_DUPLEX   = 20 << 2,
};
/// AudioBufferType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBufferFormat
enum AudioBufferFormat {
  AUDIO_BUFFER_FORMAT_U8  = 21 << 0,  
  AUDIO_BUFFER_FORMAT_I16 = 21 << 1,  
};
/// AudioBufferFormat
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioContext
struct AudioContext;
/// AudioContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer
struct AudioBuffer;
/// AudioBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSource
struct AudioSource;
/// AudioBuffer
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
/// AudioContext functions

NIKOLA_API AudioContext* audio_context_init(const char* device_name);

NIKOLA_API void audio_context_destroy(AudioContext* ctx);

/// AudioContext functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer functions

NIKOLA_API AudioBuffer* audio_buffer_create(AudioContext* ctx, const AudioBufferDesc& desc);

NIKOLA_API void audio_buffer_destroy(AudioBuffer* buffer);

NIKOLA_API AudioBufferDesc& audio_buffer_get_desc(AudioBuffer* buffer);

NIKOLA_API void audio_buffer_update(AudioBuffer* buffer, const AudioBufferDesc& desc);

/// AudioBuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSource functions

NIKOLA_API AudioSource* audio_source_create(AudioContext* ctx, const AudioBuffer* buffer);

NIKOLA_API void audio_source_destroy(AudioSource* source);

NIKOLA_API void audio_source_start(AudioSource* source);

NIKOLA_API void audio_source_stop(AudioSource* source);

NIKOLA_API void audio_source_restart(AudioSource* source);

NIKOLA_API void audio_source_pause(AudioSource* source);

NIKOLA_API void audio_source_queue_buffers(AudioSource* source, const AudioBuffer** buffers, const sizei count);

NIKOLA_API bool audio_source_is_playing(AudioSource* source);

NIKOLA_API void audio_source_set_buffer(AudioSource* source, AudioBuffer* buffer);

NIKOLA_API void audio_source_set_volume(AudioSource* source, const f32 volume);

NIKOLA_API void audio_source_set_pitch(AudioSource* source, const f32 pitch);

NIKOLA_API void audio_source_set_looping(AudioSource* source, const bool looping);

NIKOLA_API void audio_source_set_position(AudioSource* source, const f32 x, const f32 y, const f32 z);

NIKOLA_API void audio_source_set_velocity(AudioSource* source, const f32 x, const f32 y, const f32 z);

NIKOLA_API void audio_source_set_direction(AudioSource* source, const f32 x, const f32 y, const f32 z);

/// AudioSource functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Audio ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
