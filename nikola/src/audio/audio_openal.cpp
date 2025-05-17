#include "nikola/nikola_audio.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_containers.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// *** Audio ***

///---------------------------------------------------------------------------------------------------------------------
/// AudioState
struct AudioState {
  ALCdevice* al_device   = nullptr;
  ALCcontext* al_context = nullptr;

  HashMap<AudioBufferID, AudioBufferDesc> buffers;
  HashMap<AudioSourceID, AudioSourceDesc> sources;

  AudioListenerDesc listener;
};

static AudioState s_audio = {};
/// AudioState
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

AudioBufferID audio_buffer_create(const AudioBufferDesc& desc) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  AudioBufferID id;

  // Generate the ID
  alGenBuffers(1, &id); 
  check_al_error("alGenBuffers");

  // Get the correct OpenAL format based on the given Nikola format type and the channels
  sizei bytes; 
  ALenum format = get_al_format(desc.format, desc.channels, &bytes); 

  // Set the data
  alBufferData(id, format, desc.data, desc.size, desc.sample_rate); 
  check_al_error("alBufferData");

  s_audio.buffers[id] = desc; 
  return id;
}

void audio_buffer_destroy(AudioBufferID buffer) {
  alDeleteBuffers(1, &buffer);
}

AudioBufferDesc& audio_buffer_get_desc(AudioBufferID buffer) {
  return s_audio.buffers[buffer];
}

void audio_buffer_update(AudioBufferID buffer, const AudioBufferDesc& desc) {
  s_audio.buffers[buffer] = desc;

  alBufferi(buffer, AL_FREQUENCY, desc.sample_rate);
  alBufferi(buffer, AL_CHANNELS, desc.channels);
  alBufferi(buffer, AL_SIZE, desc.size);
}

/// AudioBuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSource functions

AudioSourceID audio_source_create(const AudioSourceDesc& desc) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  AudioSourceID id; 

  // Generate the source's ID
  alGenSources(1, &id);
  check_al_error("alGenSources");

  // Setting some defaults
  alSourcef(id, AL_GAIN, desc.volume);
  alSourcef(id, AL_PITCH, desc.pitch);
  alSourcefv(id, AL_POSITION, &desc.position[0]);
  alSourcefv(id, AL_VELOCITY, &desc.velocity[0]);
  alSourcefv(id, AL_DIRECTION, &desc.direction[0]);
  alSourcei(id, AL_LOOPING, desc.is_looping);

  // Attach the buffer (if valid)
  if(desc.buffers_count > 0) {
    alSourceQueueBuffers(id, desc.buffers_count, &desc.buffers[0]);
    check_al_error("alSourceQueueBuffers");
  }

  s_audio.sources[id] = desc;
  return id;
}

void audio_source_destroy(AudioSourceID source) {
  alDeleteSources(1, &source);
}

AudioSourceDesc& audio_source_get_desc(AudioSourceID source) {
  return s_audio.sources[source];
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

void audio_source_queue_buffers(AudioSourceID source, const AudioBufferID* buffers, const sizei count) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");
  NIKOLA_ASSERT(buffers, "Invalid AudioBuffer array given to audio_source_queue_buffers");
 
  // Queue the buffers
  alSourceQueueBuffers(source, count, &buffers[0]);
  check_al_error("alSourceQueueBuffers");

  // Update the internal queue
  s_audio.sources[source].buffers_count = count;
  for(sizei i = 0; i < count; i++) {
    s_audio.sources[source].buffers[i] = buffers[i];
  }
}

bool audio_source_is_playing(AudioSourceID source) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  i32 state;
  alGetSourcei(source, AL_SOURCE_STATE, &state);

  return state == AL_PLAYING;
}

void audio_source_set_buffer(AudioSourceID source, const AudioBufferID buffer) {
  NIKOLA_ASSERT(source, "Invalid AudioSource given to audio_source_set_buffer");

  alSourcei(source, AL_BUFFER, buffer);
  check_al_error("alSourcei(AL_BUFFER)");
}

void audio_source_set_volume(AudioSourceID source, const f32 volume) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  s_audio.sources[source].volume = volume;

  alSourcef(source, AL_GAIN, s_audio.sources[source].volume);
  check_al_error("alSourcef(AL_GAIN)");
}

void audio_source_set_pitch(AudioSourceID source, const f32 pitch) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  s_audio.sources[source].pitch = pitch;

  alSourcef(source, AL_PITCH, s_audio.sources[source].pitch);
  check_al_error("alSourcef(AL_PITCH)");
}

void audio_source_set_looping(AudioSourceID source, const bool looping) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  s_audio.sources[source].is_looping = looping;

  alSourcei(source, AL_LOOPING, looping);
  check_al_error("alSourcei(AL_LOOPING)");
}

void audio_source_set_position(AudioSourceID source, const Vec3& position) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");
  
  s_audio.sources[source].position = position;

  alSourcefv(source, AL_POSITION, &position[0]);
  check_al_error("alSource3f(AL_POSITION)");
}

void audio_source_set_velocity(AudioSourceID source, const Vec3& velocity) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  s_audio.sources[source].velocity = velocity;

  alSourcefv(source, AL_VELOCITY, &velocity[0]);
  check_al_error("alSource3f(AL_VELOCITY)");
}

void audio_source_set_direction(AudioSourceID source, const Vec3& direction) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  s_audio.sources[source].direction = direction;

  alSourcefv(source, AL_DIRECTION, &direction[0]);
  check_al_error("alSource3f(AL_DIRECTION)");
}

/// AudioSource functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioListener functions

void audio_listener_init(const AudioListenerDesc& desc) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  s_audio.listener = desc;

  alListenerf(AL_GAIN, desc.volume);
  alListenerfv(AL_POSITION, &desc.position[0]);
  alListenerfv(AL_VELOCITY, &desc.velocity[0]);
  check_al_error("alListenerfv");
}

AudioListenerDesc& audio_listener_get_desc() {
  return s_audio.listener;
}

void audio_listener_set_volume(const f32 volume) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  s_audio.listener.volume = volume;

  alListenerf(AL_GAIN, volume);
  check_al_error("alListenerfv");
}

void audio_listener_set_position(const Vec3& position) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  s_audio.listener.position = position;

  alListenerfv(AL_POSITION, &position[0]);
  check_al_error("alListenerfv");
}

void audio_listener_set_velocity(const Vec3& velocity) {
  NIKOLA_ASSERT(s_audio.al_device, "The audio device was not initialized for this operation to continue");

  s_audio.listener.velocity = velocity;

  alListenerfv(AL_VELOCITY, &velocity[0]);
  check_al_error("alListenerfv");
}

/// AudioListener functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Audio ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
