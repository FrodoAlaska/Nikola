#pragma once

#include "nikola_base.h"
#include "nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// *** Audio ***

///---------------------------------------------------------------------------------------------------------------------
/// Consts

/// The maximum amount of buffers an audio source can handle at a time.
const sizei AUDIO_QUEUE_BUFFERS_MAX = 32;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBufferFormat
enum AudioBufferFormat {
  /// Indicates an audio buffer with 8-bit unsigned integers per sample.
  AUDIO_BUFFER_FORMAT_U8  = 19 << 0,  
  
  /// Indicates an audio buffer with 16-bit signed integers per sample.
  AUDIO_BUFFER_FORMAT_I16 = 19 << 1,  
  
  /// Indicates an audio buffer with 32-bit floats per sample.
  ///
  /// @NOTE: This might not be supported on every platform.
  AUDIO_BUFFER_FORMAT_F32 = 19 << 2,  
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
  /// The format of each sample in the buffer.
  AudioBufferFormat format; 

  /// The channels of the audio buffer. 
  ///
  /// 1 = Mono. 
  /// 2 = Stereo.
  ///
  /// @NOTE: If 3D spatialization is required, the audio buffer 
  /// MUST be Mono.
  u32 channels; 

  /// The sample rate of the audio buffer.
  u32 sample_rate;

  /// The size in bytes of the `data` (i.e samples) array.
  sizei size = 0;

  /// The data or samples of the audio buffer.
  void* data = nullptr;
};
/// AudioBufferDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSourceDesc
struct AudioSourceDesc {
  /// The volume given to the audio source. 
  /// The range is from `0.0f` to `1.0f`.
  ///
  /// @NOTE: This is `1.0f` by default.
  f32 volume = 1.0f; 

  /// The pitch given to the audio source. 
  /// The range is from `0.0f` to `1.0f`.
  ///
  /// @NOTE: This is `1.0f` by default.
  f32 pitch  = 1.0f; 

  /// The position of the audio source. 
  ///
  /// @NOTE: Nikola uses a right-handed coordinate system by default. 
  /// Make sure to keep that in mind when positioning the audio source.
  ///
  /// @NOTE: This is `{0.0f, 0.0f, 0.0f}` by default.
  Vec3 position  = Vec3(0.0f);
  
  /// The velocity of the audio source. 
  ///
  /// @NOTE: This is `{0.0f, 0.0f, 0.0f}` by default.
  Vec3 velocity  = Vec3(0.0f);
  
  /// The direction of the audio source. 
  ///
  /// @NOTE: This is `{0.0f, 0.0f, 0.0f}` by default.
  Vec3 direction = Vec3(0.0f);

  /// Indicates whether the audio source should be loopable.
  ///
  /// @NOTE: This is `false` by default.
  bool is_looping = false; 

  /// The number of buffers to be processed in the `buffers` array.
  ///
  /// @NOTE: The max value should not exceed `AUDIO_QUEUE_BUFFERS_MAX`.
  sizei buffers_count = 0;

  /// An array up to `AUDIO_QUEUE_BUFFERS_MAX` of the 
  /// desired buffers to be processed.
  AudioBufferID buffers[AUDIO_QUEUE_BUFFERS_MAX];
};
/// AudioSourceDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioListenerDesc
struct AudioListenerDesc {
  /// The volume of the listener.
  ///
  /// @NOTE: This is set to `1.0f` by default.
  f32 volume = 1.0f;

  /// The position of the listener. 
  ///
  /// @NOTE: Nikola uses a right-handed coordinate system by default. 
  /// Make sure to keep that in mind when positioning the listener.
  ///
  /// @NOTE: This is `{0.0f, 0.0f, 0.0f}` by default.
  Vec3 position = Vec3(0.0f);
  
  /// The velocity of the listener.
  ///
  /// @NOTE: This is set to `1.0f` by default.
  Vec3 velocity = Vec3(0.0f);
};
/// AudioListenerDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Audio device functions

/// Initialize an audio device to enable audio operations on `device_name.
/// The function will return `true` if all goes well. Otherwise, `false` will be returned.
///
/// @NOTE: If `device_name` is set to `nullptr`, the audio device will choose the system's 
/// default sound card.
NIKOLA_API bool audio_device_init(const char* device_name);

/// Shutdown the audio system, reclaiming any allocated memory.
NIKOLA_API void audio_device_shutdown();

/// Audio device functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioBuffer functions

/// Create an audio buffer with the information given in `desc`, returning back 
/// an identifier for any future operations.
NIKOLA_API AudioBufferID audio_buffer_create(const AudioBufferDesc& desc);

/// Destroy the given `buffer`, reclaiming any allocated memory in the process.
NIKOLA_API void audio_buffer_destroy(AudioBufferID buffer);

/// Retrieve the underlying `AudioBufferDesc` of `buffer`.
NIKOLA_API AudioBufferDesc& audio_buffer_get_desc(AudioBufferID buffer);

/// Update the data and parametars of `buffer` to the information provided in `desc`.
NIKOLA_API void audio_buffer_update(AudioBufferID buffer, const AudioBufferDesc& desc);

/// AudioBuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioSource functions

/// Create an audio source with the information given in `desc`, returning back 
/// an identifier for any future operations.
NIKOLA_API AudioSourceID audio_source_create(const AudioSourceDesc& desc);

/// Destroy the given `source`, reclaiming any allocated memory in the process.
NIKOLA_API void audio_source_destroy(AudioSourceID source);

/// Retrieve the underlying `AudioSourceDesc` of `source`.
NIKOLA_API AudioSourceDesc& audio_source_get_desc(AudioSourceID source);

/// Start/play the buffer(s) of the given `source`. 
///
/// @NOTE: If `source` currently lacks any audio buffers, this function 
/// will simply do nothing.
NIKOLA_API void audio_source_start(AudioSourceID source);

/// Stop the processing of any buffer(s) of the given `source`. 
NIKOLA_API void audio_source_stop(AudioSourceID source);

/// Restart the currently processing buffer(s) of the given `source` to their initial state. 
///
/// @NOTE: This will not play the source once again after rewinding. It will only 
/// reset the state of the buffers and exit.
NIKOLA_API void audio_source_restart(AudioSourceID source);

/// Pause any processing of buffer(s) of the given `source`.
NIKOLA_API void audio_source_pause(AudioSourceID source);

/// Queue `count` amount of `buffers` to the given `source` to be played one after the other.
NIKOLA_API void audio_source_queue_buffers(AudioSourceID source, const AudioBufferID* buffers, const sizei count);

/// Return back the "playing" state of the given `source`.
NIKOLA_API bool audio_source_is_playing(AudioSourceID source);

/// Set the given `buffer` to be processed by `source`.
NIKOLA_API void audio_source_set_buffer(AudioSourceID source, const AudioBufferID buffer);

/// Set the volume of `source` to the given `volume`.
NIKOLA_API void audio_source_set_volume(AudioSourceID source, const f32 volume);

/// Set the pitch of `source` to the given `pitch`.
NIKOLA_API void audio_source_set_pitch(AudioSourceID source, const f32 pitch);

/// Set the looping flag of `source` to the given `looping`.
NIKOLA_API void audio_source_set_looping(AudioSourceID source, const bool looping);

/// Set the position of `source` to the given `position`.
NIKOLA_API void audio_source_set_position(AudioSourceID source, const Vec3& position);

/// Set the velocity of `source` to the given `velocity`.
NIKOLA_API void audio_source_set_velocity(AudioSourceID source, const Vec3& velocity);

/// Set the direction of `source` to the given `direction`.
NIKOLA_API void audio_source_set_direction(AudioSourceID source, const Vec3& direction);

/// AudioSource functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AudioListener functions

/// Initialize the global audio listener to the information in `desc`.
NIKOLA_API void audio_listener_init(const AudioListenerDesc& desc);

/// Retrieve the underlying `AudioListenerDesc` of global audio listener.
NIKOLA_API AudioListenerDesc& audio_listener_get_desc();

/// Set the volume of global audio listener to the given `volume`.
NIKOLA_API void audio_listener_set_volume(const f32 volume);

/// Set the position of global audio listener to the given `position`.
NIKOLA_API void audio_listener_set_position(const Vec3& position);

/// Set the velocity of global audio listener to the given `velocity`.
NIKOLA_API void audio_listener_set_velocity(const Vec3& velocity);

/// AudioListener functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Audio ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
