#include "nbr.h"

#include <nikola/nikola.h>

#include <dr_libs/dr_wav.h>
#include <dr_libs/dr_mp3.h>
#include <stb/stb_vorbis.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr 

/// ----------------------------------------------------------------------
/// Private functions

static bool wav_convert(nikola::NBRAudio* audio, const nikola::FilePath& path) {
  nikola::u32 channels, sample_rate;
  drwav_uint64 frames_count;
  
  audio->samples = drwav_open_file_and_read_pcm_frames_s16(path.c_str(), &channels, &sample_rate, &frames_count, nullptr);
  if (!audio->samples) {
    NIKOLA_LOG_ERROR("[NBR-ERROR]: Failed to read WAV file at \'%s\'", path.c_str());
    return false;
  }

  // Convert to NBRAudio
  audio->format      = (nikola::u8)nikola::AUDIO_BUFFER_FORMAT_I16;
  audio->sample_rate = sample_rate;
  audio->channels    = channels;
  audio->size        = frames_count * (channels * sizeof(nikola::i16)); 

  return true;
}

static bool mp3_convert(nikola::NBRAudio* audio, const nikola::FilePath& path) {
  drmp3_config config;
  drmp3_uint64 frames_count;
  
  audio->samples = drmp3_open_file_and_read_pcm_frames_s16(path.c_str(), &config, &frames_count, nullptr);
  if (!audio->samples) {
    NIKOLA_LOG_ERROR("[NBR-ERROR]: Failed to read MP3 file at \'%s\'", path.c_str());
    return false;
  }

  // Convert to NBRAudio
  audio->format      = (nikola::u8)nikola::AUDIO_BUFFER_FORMAT_I16;
  audio->sample_rate = config.sampleRate;
  audio->channels    = config.channels;
  audio->size        = frames_count * (config.channels * sizeof(nikola::i16)); 

  return true;
}

static bool ogg_convert(nikola::NBRAudio* audio, const nikola::FilePath& path) {
  int channels, sample_rate;

  // Open the OGG file and read the _whole_ file.
  int frames = stb_vorbis_decode_filename(path.c_str(), &channels, &sample_rate, &audio->samples);
  if(frames == -1) {
    NIKOLA_LOG_ERROR("[NBR-ERROR]: Failed to read OGG file at \'%s\'", path.c_str());
   
    nikola::memory_free(audio->samples);
    return false;
  } 

  // Convert to NBRAudio
  audio->format      = (nikola::u8)nikola::AUDIO_BUFFER_FORMAT_I16;
  audio->sample_rate = sample_rate;
  audio->channels    = channels;
  audio->size        = frames; 
  
  return true;
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Audio loader functions

bool audio_loader_load(nikola::NBRAudio* audio, const nikola::FilePath& path) {
  nikola::FilePath ext = nikola::filepath_extension(path);

  // Depending on the audio format, the conversion is different
  if(ext == ".wav") {
    return wav_convert(audio, path);
  }
  else if(ext == ".mp3") {
    return mp3_convert(audio, path);
  }
  else if(ext == ".ogg") {
    return ogg_convert(audio, path);
  }
  
  // None of the above are true, error and leave.
  NIKOLA_LOG_ERROR("[NBR-ERROR]: The given audio path \'%s\' is an unsupported format", path.c_str());
  return false;
}

void audio_loader_unload(nikola::NBRAudio& audio) {
  if(audio.samples) {
    nikola::memory_free(audio.samples);
  }
}

/// Audio loader functions
/// ----------------------------------------------------------------------


} // End of nbr

//////////////////////////////////////////////////////////////////////////
