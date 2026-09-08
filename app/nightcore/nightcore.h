#ifndef NIGHTCORE_H
#define NIGHTCORE_H

#include <QAudioFormat>
#include <QByteArray>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Nightcore {
  class Processor {
  public:
    void setSpeed(double s);
    void setReverbDecay(double d);
    void setSampleRate(int fs);
    void reset();

    double speed() const { return speed_; }
    double reverbDecay() const { return decay_; }
    bool isPassthrough() const { return speed_ == 1.0 && decay_ == 0.0; }

    qint64 processChunk(const char *in_data, qint64 in_frames, const QAudioFormat &fmt, QByteArray &out);

  private:
    void ensureChannels(int ch);
    float processReverbSample(float x, int ch);

    double speed_ = 1.0;
    double decay_ = 0.0;
    int fs_ = 44100;
    int channels_ = 0;

    struct ChState {
      std::vector<std::vector<float>> combBuf;
      std::vector<int> combIdx;
      std::vector<std::vector<float>> apBuf;
      std::vector<int> apIdx;
    };
    std::vector<ChState> ch_;
    float fb_ = 0.0f;
    float wet_ = 0.0f;
  };
}

#endif
