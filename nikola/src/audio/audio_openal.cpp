#include "nikola/nikola_audio.h"
#include "nikola/nikola_base.h"

#include <AL/al.h>
#include <AL/alc.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// *** Audio ***

///---------------------------------------------------------------------------------------------------------------------
/// AudioContext
struct AudioContext {
  ALCdevice* al_device   = nullptr;
  ALCcontext* al_context = nullptr;
};
/// AudioContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer
struct AudioBuffer {
  AudioContext* context = nullptr;
  AudioBufferDesc desc  = {}; 

  u32 id;
};
/// AudioBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSource
struct AudioSource {
  AudioContext* context = nullptr;
  u32 id;
};
/// AudioBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static const char* get_al_error_string(ALenum error) {
  switch(error) {
    case AL_INVALID_NAME:
      return "AL_INVALID_NAME";
    case AL_INVALID_ENUM:
      return "AL_INVALID_ENUM";
    case AL_INVALID_VALUE:
      return "AL_INVALID_VALUE";
    case AL_INVALID_OPERATION:
      return "AL_INVALID_OPERATION";
    case AL_OUT_OF_MEMORY:
      return "AL_OUT_OF_MEMORY";
    default:
      return "AL_NO_ERROR";
  }
}

static void check_al_error(const char* func_name) {
  i32 error = alGetError();
  
  // We're fine. No need to panic. Everything is coooool.
  if(error == AL_NO_ERROR) {
    return;
  }

  // The world is on fire!
  NIKOLA_LOG_ERROR("OpenAL function \'%s\' raised a \'%s\' error", func_name, get_al_error_string(error));
}

static ALenum get_al_format(const AudioBufferFormat format, const u32 channels) {
  // Mono channels
  if(channels == 1) {
    switch(format) {
      case AUDIO_BUFFER_FORMAT_U8:
        return AL_FORMAT_MONO8;
      case AUDIO_BUFFER_FORMAT_I16:
        return AL_FORMAT_MONO16;
    }
  }
  // Stereo channels
  else if(channels == 2) {
    switch(format) {
      case AUDIO_BUFFER_FORMAT_U8:
        return AL_FORMAT_STEREO8;
      case AUDIO_BUFFER_FORMAT_I16:
        return AL_FORMAT_STEREO16;
    }
  }
  else {
    NIKOLA_LOG_ERROR("Invalid channels given to audio buffer");
    return -1;
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioContext functions

AudioContext* audio_context_init(const char* device_name) {
  AudioContext* ctx = (AudioContext*)memory_allocate(sizeof(AudioContext));
  memory_zero(ctx, sizeof(AudioContext));

  // Reset the error stack of OpenAL 
  alGetError();

  // Init OpenAL device
  ctx->al_device = alcOpenDevice(device_name);
  NIKOLA_ASSERT(ctx->al_device, "Could not open an OpenAL audio device!");
  check_al_error("alcOpenDevice");

  // Create a context and set it as the current context
  ctx->al_context = alcCreateContext(ctx->al_device, nullptr);
  alcMakeContextCurrent(ctx->al_context);
  check_al_error("alcCreateContext");

  // Print out some useful information
  NIKOLA_LOG_INFO("An OpenAL audio context was successfully created:\n" 
                 "              VENDOR: %s\n" 
                 "              RENDERER: %s\n" 
                 "              VERSION: %s\n" 
                 "              EXTENSIONS: %s\n", 
                 alGetString(AL_VENDOR), alGetString(AL_RENDERER), alGetString(AL_VERSION), alGetString(AL_EXTENSIONS));

  return ctx;
}

void audio_context_destroy(AudioContext* ctx) {
  if(!ctx) {
    return;
  }
  
  alcCloseDevice(ctx->al_device);
  alcDestroyContext(ctx->al_context);
  memory_free(ctx);

  NIKOLA_LOG_INFO("The audio context was successfully destroyed");
}

/// AudioContext functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer functions

AudioBuffer* audio_buffer_create(AudioContext* ctx, const AudioBufferDesc& desc) {
  NIKOLA_ASSERT(ctx, "Invalid AudioContext given to audio_buffer_create");

  AudioBuffer* buffer = (AudioBuffer*)memory_allocate(sizeof(AudioBuffer));
  memory_zero(buffer, sizeof(AudioBuffer));

  // Generate the ID
  alGenBuffers(1, &buffer->id); 
  check_al_error("alGenBuffers");

  // Get the correct OpenAL format based on the given Nikola format type and the channels
  ALenum format = get_al_format(desc.format, desc.channels); 

  // Set the data
  alBufferData(buffer->id, format, desc.data, desc.size, desc.sample_rate); 
  check_al_error("alBufferData");

  return buffer;
}

void audio_buffer_destroy(AudioBuffer* buffer) {
  if(!buffer) {
    return;
  }

  alDeleteBuffers(1, &buffer->id);
  memory_free(buffer);
}

AudioBufferDesc& audio_buffer_get_desc(AudioBuffer* buffer) {
  NIKOLA_ASSERT(buffer, "Invalid AudioBuffer given to audio_buffer_get_desc");
  return buffer->desc;
}

void audio_buffer_update(AudioBuffer* buffer, const AudioBufferDesc& desc) {
  NIKOLA_ASSERT(buffer, "Invalid AudioBuffer given to audio_buffer_update");

  alBufferi(buffer->id, AL_FREQUENCY, desc.sample_rate);
  alBufferi(buffer->id, AL_CHANNELS, desc.channels);
  alBufferi(buffer->id, AL_SIZE, desc.size);
}

/// AudioBuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSource functions

AudioSource* audio_source_create(AudioContext* ctx, const AudioBuffer* buffer) {
  NIKOLA_ASSERT(ctx, "Invalid AudioContext given to audio_source_create");

  AudioSource* source = (AudioSource*)memory_allocate(sizeof(AudioSource));
  memory_zero(source, sizeof(AudioSource));

  // Generate the source's ID
  alGenSources(1, &source->id);
  check_al_error("alGenSources");

  // Attach the buffer (if valid)
  if(buffer) {
    alSourcei(source->id, AL_BUFFER, buffer->id);
    check_al_error("alSourcei(AL_BUFFER)");
  }

  return source;
}

void audio_source_destroy(AudioSource* source) {
  if(!source) {
    return;
  }

  alDeleteSources(1, &source->id);
  memory_free(source);
}

void audio_source_start(AudioSource* source) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_start");

  alSourcePlay(source->id);
  check_al_error("alPlaySource");
}

void audio_source_stop(AudioSource* source) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_stop");

  alSourceStop(source->id);
  check_al_error("alStopSource");
}

void audio_source_restart(AudioSource* source) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_restart");

  alSourceRewind(source->id);
  check_al_error("alRewindSource");
}

void audio_source_pause(AudioSource* source) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_pause");

  alSourcePause(source->id);
  check_al_error("alPauseSource");
}

void audio_source_queue_buffers(AudioSource* source, const AudioBuffer** buffers, const sizei count) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_queue_buffers");
  NIKOLA_ASSERT(buffers, "Invalid AudioBuffer array given to audio_source_queue_buffers");
  
  // @TODO (Audio): This is awful. Please no
  for(sizei i = 0; i < count; i++) {
    alSourceQueueBuffers(source->id, 1, &buffers[i]->id);
    check_al_error("alSourceQueueBuffers");
  }
}

bool audio_source_is_playing(AudioSource* source) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_is_playing");

  i32 state;
  alGetSourcei(source->id, AL_SOURCE_STATE, &state);

  return state == AL_PLAYING;
}

void audio_source_set_buffer(AudioSource* source, AudioBuffer* buffer) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_set_buffer");
  NIKOLA_ASSERT(buffer, "Invalid AudioBuffer given to audio_source_set_buffer");

  alSourcei(source->id, AL_BUFFER, buffer->id);
  check_al_error("alSourcei(AL_BUFFER)");
}

void audio_source_set_volume(AudioSource* source, const f32 volume) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_set_volume");

  alSourcef(source->id, AL_GAIN, volume);
  check_al_error("alSourcef(AL_GAIN)");
}

void audio_source_set_pitch(AudioSource* source, const f32 pitch) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_set_pitch");

  alSourcef(source->id, AL_PITCH, pitch);
  check_al_error("alSourcef(AL_PITCH)");
}

void audio_source_set_looping(AudioSource* source, const bool looping) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_set_looping");

  alSourcei(source->id, AL_LOOPING, looping);
  check_al_error("alSourcei(AL_LOOPING)");
}

void audio_source_set_position(AudioSource* source, const f32 x, const f32 y, const f32 z) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_set_position");

  alSource3f(source->id, AL_POSITION, x, y, z);
  check_al_error("alSource3f(AL_POSITION)");
}

void audio_source_set_velocity(AudioSource* source, const f32 x, const f32 y, const f32 z) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_set_velocity");

  alSource3f(source->id, AL_VELOCITY, x, y, z);
  check_al_error("alSource3f(AL_VELOCITY)");
}

void audio_source_set_direction(AudioSource* source, const f32 x, const f32 y, const f32 z) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_set_direction");

  alSource3f(source->id, AL_DIRECTION, x, y, z);
  check_al_error("alSource3f(AL_DIRECTION)");
}

/// AudioSource functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Audio ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
