#pragma once

#include "nikola_base.h"
#include "nikola_math.h"

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
/// AudioBufferID
typedef u32 AudioBufferID;
/// AudioBufferID
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
/// AudioSourceDesc
struct AudioSourceDesc {
  f32 volume = 1.0f; 
  f32 pitch  = 1.0f; 

  Vec3 position  = Vec3(0.0f);
  Vec3 velocity  = Vec3(0.0f);
  Vec3 direction = Vec3(0.0f);

  bool is_looping = false; 

  AudioBufferID buffers[AUDIO_QUEUE_BUFFERS_MAX];
  sizei buffers_count = 0;
};
/// AudioSourceDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioListenerDesc
struct AudioListenerDesc {
  f32 volume = 1.0f;

  Vec3 position = Vec3(0.0f);
  Vec3 velocity = Vec3(0.0f);
};
/// AudioListenerDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Audio device functions

NIKOLA_API bool audio_device_init(const char* device_name);

NIKOLA_API void audio_device_shutdown();

/// Audio device functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer functions

NIKOLA_API AudioBufferID audio_buffer_create(const AudioBufferDesc& desc);

NIKOLA_API void audio_buffer_destroy(AudioBufferID buffer);

NIKOLA_API AudioBufferDesc& audio_buffer_get_desc(AudioBufferID buffer);

NIKOLA_API void audio_buffer_update(AudioBufferID buffer, const AudioBufferDesc& desc);

/// AudioBuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSource functions

NIKOLA_API AudioSourceID audio_source_create(const AudioSourceDesc& desc);

NIKOLA_API void audio_source_destroy(AudioSourceID source);

NIKOLA_API AudioSourceDesc& audio_source_get_desc(AudioSourceID source);

NIKOLA_API void audio_source_start(AudioSourceID source);

NIKOLA_API void audio_source_stop(AudioSourceID source);

NIKOLA_API void audio_source_restart(AudioSourceID source);

NIKOLA_API void audio_source_pause(AudioSourceID source);

NIKOLA_API void audio_source_queue_buffers(AudioSourceID source, const AudioBufferID* buffers, const sizei count);

NIKOLA_API bool audio_source_is_playing(AudioSourceID source);

NIKOLA_API void audio_source_set_buffer(AudioSourceID source, const AudioBufferID buffer);

NIKOLA_API void audio_source_set_volume(AudioSourceID source, const f32 volume);

NIKOLA_API void audio_source_set_pitch(AudioSourceID source, const f32 pitch);

NIKOLA_API void audio_source_set_looping(AudioSourceID source, const bool looping);

NIKOLA_API void audio_source_set_position(AudioSourceID source, const Vec3& position);

NIKOLA_API void audio_source_set_velocity(AudioSourceID source, const Vec3& velocity);

NIKOLA_API void audio_source_set_direction(AudioSourceID source, const Vec3& direction);

/// AudioSource functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioListener functions

NIKOLA_API void audio_listener_init(const AudioListenerDesc& desc);

NIKOLA_API AudioListenerDesc& audio_listener_get_desc();

NIKOLA_API void audio_listener_set_volume(const f32 volume);

NIKOLA_API void audio_listener_set_position(const Vec3& position);

NIKOLA_API void audio_listener_set_velocity(const Vec3& velocity);

/// AudioListener functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Audio ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
