#include "nikola/nikola_audio.h"
#include "nikola/nikola_base.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// *** Audio ***

///---------------------------------------------------------------------------------------------------------------------
/// AudioDevice
struct AudioDevice {
  ALCdevice* al_device   = nullptr;
  ALCcontext* al_context = nullptr;
};

static AudioDevice s_audio = {};
/// AudioDevice
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer
struct AudioBuffer {
  AudioBufferDesc desc  = {}; 
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

static ALenum get_al_format(const AudioBufferFormat format, const u32 channels, sizei* bytes) {
  // Mono channels
  if(channels == 1) {
    switch(format) {
      case AUDIO_BUFFER_FORMAT_U8:
        *bytes = sizeof(u8);
        return AL_FORMAT_MONO8;
      case AUDIO_BUFFER_FORMAT_I16:
        *bytes = sizeof(i16);
        return AL_FORMAT_MONO16;
      case AUDIO_BUFFER_FORMAT_F32:
        *bytes = sizeof(f32);
        return AL_FORMAT_MONO_FLOAT32;
    }
  }
  // Stereo channels
  else if(channels == 2) {
    switch(format) {
      case AUDIO_BUFFER_FORMAT_U8:
        *bytes = sizeof(u8);
        return AL_FORMAT_STEREO8;
      case AUDIO_BUFFER_FORMAT_I16:
        *bytes = sizeof(i16);
        return AL_FORMAT_STEREO16;
      case AUDIO_BUFFER_FORMAT_F32:
        *bytes = sizeof(f32);
        return AL_FORMAT_STEREO_FLOAT32;
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

bool audio_device_init(const char* device_name) {
  // Init OpenAL device
  s_audio.al_device = alcOpenDevice(device_name);
  NIKOLA_ASSERT(s_audio.al_device, "Could not open an OpenAL audio device!");

  // Create a context and set it as the current context
  s_audio.al_context = alcCreateContext(s_audio.al_device, nullptr);
  alcMakeContextCurrent(s_audio.al_context);

  // Reset the error stack of OpenAL 
  alGetError();

  // Print out some useful information
  NIKOLA_LOG_INFO("An OpenAL audio device was successfully initialized:\n" 
                 "              VENDOR: %s\n" 
                 "              RENDERER: %s\n" 
                 "              VERSION: %s",
                 alGetString(AL_VENDOR), alGetString(AL_RENDERER), alGetString(AL_VERSION));

  return true;
}

void audio_device_shutdown() {
  // This should be called otherwise we'll have a problem
  alcMakeContextCurrent(nullptr); 

  // Destroy all the resources 
  alcDestroyContext(s_audio.al_context);
  alcCloseDevice(s_audio.al_device);

  NIKOLA_LOG_INFO("The audio device was successfully destroyed");
}

/// AudioContext functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer functions

AudioBuffer* audio_buffer_create(const AudioBufferDesc& desc) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  AudioBuffer* buffer = (AudioBuffer*)memory_allocate(sizeof(AudioBuffer));
  memory_zero(buffer, sizeof(AudioBuffer));

  // Generate the ID
  alGenBuffers(1, &buffer->id); 
  check_al_error("alGenBuffers");

  // Get the correct OpenAL format based on the given Nikola format type and the channels
  sizei bytes; 
  ALenum format = get_al_format(desc.format, desc.channels, &bytes); 

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

AudioSourceID audio_source_create(const AudioBuffer* buffer) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  AudioSourceID id; 

  // Generate the source's ID
  alGenSources(1, &id);
  check_al_error("alGenSources");

  // Setting some defaults
  alSourcef(id, AL_GAIN, 1.0f);
  alSourcef(id, AL_PITCH, 1.0f);
  alSource3f(id, AL_POSITION, 0.0f, 0.0f, 0.0f);

  // Attach the buffer (if valid)
  if(buffer) {
    alSourcei(id, AL_BUFFER, buffer->id);
    check_al_error("alSourcei(AL_BUFFER)");
  }

  return id;
}

void audio_source_destroy(AudioSourceID source) {
  alDeleteSources(1, &source);
}

void audio_source_start(AudioSourceID source) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  alSourcePlay(source);
  check_al_error("alPlaySource");
}

void audio_source_stop(AudioSourceID source) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  alSourceStop(source);
  check_al_error("alStopSource");
}

void audio_source_restart(AudioSourceID source) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  alSourceRewind(source);
  check_al_error("alRewindSource");
}

void audio_source_pause(AudioSourceID source) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  alSourcePause(source);
  check_al_error("alPauseSource");
}

void audio_source_queue_buffers(AudioSourceID source, const AudioBuffer** buffers, const sizei count) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");
  NIKOLA_ASSERT(buffers, "Invalid AudioBuffer array given to audio_source_queue_buffers");
  
  // @TODO (Audio): This is awful. Please no
  for(sizei i = 0; i < count; i++) {
    alSourceQueueBuffers(source, 1, &buffers[i]->id);
    check_al_error("alSourceQueueBuffers");
  }
}

bool audio_source_is_playing(AudioSourceID source) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  i32 state;
  alGetSourcei(source, AL_SOURCE_STATE, &state);

  return state == AL_PLAYING;
}

void audio_source_set_buffer(AudioSourceID source, AudioBuffer* buffer) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_set_buffer");
  NIKOLA_ASSERT(buffer, "Invalid AudioBuffer given to audio_source_set_buffer");

  alSourcei(source, AL_BUFFER, buffer->id);
  check_al_error("alSourcei(AL_BUFFER)");
}

void audio_source_set_volume(AudioSourceID source, const f32 volume) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  alSourcef(source, AL_GAIN, volume);
  check_al_error("alSourcef(AL_GAIN)");
}

void audio_source_set_pitch(AudioSourceID source, const f32 pitch) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  alSourcef(source, AL_PITCH, pitch);
  check_al_error("alSourcef(AL_PITCH)");
}

void audio_source_set_looping(AudioSourceID source, const bool looping) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  alSourcei(source, AL_LOOPING, looping);
  check_al_error("alSourcei(AL_LOOPING)");
}

void audio_source_set_position(AudioSourceID source, const f32 x, const f32 y, const f32 z) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  alSource3f(source, AL_POSITION, x, y, z);
  check_al_error("alSource3f(AL_POSITION)");
}

void audio_source_set_velocity(AudioSourceID source, const f32 x, const f32 y, const f32 z) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  alSource3f(source, AL_VELOCITY, x, y, z);
  check_al_error("alSource3f(AL_VELOCITY)");
}

void audio_source_set_direction(AudioSourceID source, const f32 x, const f32 y, const f32 z) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  alSource3f(source, AL_DIRECTION, x, y, z);
  check_al_error("alSource3f(AL_DIRECTION)");
}

/// AudioSource functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Audio ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
