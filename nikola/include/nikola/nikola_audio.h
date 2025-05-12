#pragma once

#include "nikola_base.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// *** Audio ***

///---------------------------------------------------------------------------------------------------------------------
/// Consts

const sizei AUDIO_MAX_CHANNELS = 254;

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
  AUDIO_BUFFER_FORMAT_I24 = 21 << 2,  
  AUDIO_BUFFER_FORMAT_I32 = 21 << 3,  
  AUDIO_BUFFER_FORMAT_F32 = 21 << 4,  
};
/// AudioBufferFormat
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer
struct AudioBuffer;
/// AudioBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBufferCallback

using AudioBufferCallback = void(*)(AudioBuffer* buffer, void* output, const void* input);

/// AudioBufferCallback
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBufferDesc
struct AudioBufferDesc {
  AudioBufferType type; 
  AudioBufferFormat format; 

  u32 channels    = 2; 
  u32 sample_rate = 48000;

  AudioBufferCallback data_callback = nullptr;
  void* user_data                   = nullptr;
};
/// AudioBufferDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSource2D 
struct AudioSource2D {
  AudioBuffer* buffer;

  f32 volume;
  f32 pitch; 

  bool can_loop
};
/// AudioSource2D 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer functions

AudioBuffer* audio_buffer_create(const AudioBufferDesc& desc);

void audio_buffer_destroy(AudioBuffer* buffer);

AudioBufferDesc& audio_buffer_get_desc(AudioBuffer* buffer);

void audio_buffer_update(AudioBuffer* buffer, const AudioBufferDesc& desc);

void audio_buffer_start(AudioBuffer* buffer);

void audio_buffer_stop(AudioBuffer* buffer);

void audio_buffer_seek(AudioBuffer* buffer, const sizei pcm_frames);

const sizei audio_buffer_read(AudioBuffer* buffer, const sizei pcm_frames);

/// AudioBuffer functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Audio ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
