#pragma once

#include <memory>

extern "C" {
#include "common_audio/ring_buffer.h"
}
#include "modules/audio_processing/aec/aec_core.h"
namespace webrtc {

class ApmDataDumper;

typedef struct Aec {
  Aec();
  ~Aec();

  std::unique_ptr<ApmDataDumper> data_dumper;

  int delayCtr;
  int sampFreq;
  int splitSampFreq;
  int scSampFreq;
  float sampFactor;  // scSampRate / sampFreq
  short skewMode;
  int bufSizeStart;
  int knownDelay;
  int rate_factor;

  short initFlag;  // indicates if AEC has been initialized

  // Variables used for averaging far end buffer size
  short counter;
  int sum;
  short firstVal;
  short checkBufSizeCtr;

  // Variables used for delay shifts
  short msInSndCardBuf;
  short filtDelay;  // Filtered delay estimate.
  int timeForDelayChange;
  int startup_phase;
  int checkBuffSize;
  short lastDelayDiff;

  // Structures
  void* resampler;

  int skewFrCtr;
  int resample;  // if the skew is small enough we don't resample
  int highSkewCtr;
  float skew;

  RingBuffer* far_pre_buf;  // Time domain far-end pre-buffer.

  int farend_started;

  // Aec instance counter.
  static int instance_count;
  AecCore* aec;
} Aec;


// Returns a pointer to the low level AEC handle.
//
// Input:
//  - handle                    : Pointer to the AEC instance.
//
// Return value:
//  - AecCore pointer           : NULL for error.
//

struct AecCore;

struct AecCore* WebRtcAec_aec_core(void* handle);

}  // namespace webrtc