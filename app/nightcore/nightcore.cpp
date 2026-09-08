#include "nightcore/nightcore.h"

#include <algorithm>
#include <cmath>

namespace Nightcore {
  namespace {
    constexpr int kCombs[4] = {1116, 1188, 1277, 1356};
    constexpr int kAps[2] = {556, 441};
  }

  void Processor::setSpeed(double s) {
    speed_ = std::clamp(s, 0.5, 2.0);
  }

  void Processor::setReverbDecay(double d) {
    decay_ = std::clamp(d, 0.0, 1.0);
    fb_ = float(0.2 + decay_ * 0.65);
    wet_ = float(decay_ * 0.8);
    if (decay_ == 0.0) reset();
  }

  void Processor::setSampleRate(int fs) {
    if (fs <= 0) return;
    if (fs == fs_) return;
    fs_ = fs;
    ch_.clear();
    channels_ = 0;
  }

  void Processor::reset() {
    for (auto &c : ch_) {
      for (auto &b : c.combBuf) std::fill(b.begin(), b.end(), 0.0f);
      for (auto &b : c.apBuf) std::fill(b.begin(), b.end(), 0.0f);
      std::fill(c.combIdx.begin(), c.combIdx.end(), 0);
      std::fill(c.apIdx.begin(), c.apIdx.end(), 0);
    }
  }

  void Processor::ensureChannels(int ch) {
    if (ch == channels_ && !ch_.empty()) return;
    channels_ = ch;
    ch_.resize(ch);
    const double k = fs_ / 44100.0;
    for (int c = 0; c < ch; c++) {
      auto &s = ch_[c];
      s.combBuf.resize(4);
      s.combIdx.assign(4, 0);
      for (int i = 0; i < 4; i++) {
        int len = std::max(16, int(kCombs[i] * k));
        s.combBuf[i].assign(len, 0.0f);
      }
      s.apBuf.resize(2);
      s.apIdx.assign(2, 0);
      for (int i = 0; i < 2; i++) {
        int len = std::max(16, int(kAps[i] * k));
        s.apBuf[i].assign(len, 0.0f);
      }
    }
  }

  float Processor::processReverbSample(float x, int ch) {
    auto &s = ch_[ch];
    float verb = 0.0f;
    for (int i = 0; i < 4; i++) {
      auto &buf = s.combBuf[i];
      int &idx = s.combIdx[i];
      idx = (idx + 1) % int(buf.size());
      float y = buf[idx];
      buf[idx] = x + y * fb_;
      verb += y;
    }
    verb *= 0.25f;
    for (int i = 0; i < 2; i++) {
      auto &buf = s.apBuf[i];
      int &idx = s.apIdx[i];
      idx = (idx + 1) % int(buf.size());
      float bufout = buf[idx];
      float y = -0.5f * verb + bufout;
      buf[idx] = verb + 0.5f * y;
      verb = y;
    }
    return x + verb * wet_;
  }

  qint64 Processor::processChunk(const char *in_data, qint64 in_frames, const QAudioFormat &fmt, QByteArray &out) {
    const int ch = fmt.channelCount();
    if (in_frames <= 0 || ch <= 0) {
      out.clear();
      return 0;
    }
    ensureChannels(ch);

    qint64 out_frames = speed_ == 1.0 ? in_frames : qint64(double(in_frames) / speed_);
    if (out_frames < 1) out_frames = 1;
    if (out_frames > in_frames * 2 + 1) out_frames = in_frames * 2 + 1;

    std::vector<float> fin(size_t(in_frames * ch));
    switch (fmt.sampleFormat()) {
      case QAudioFormat::Float:
        for (qint64 i = 0; i < in_frames * ch; i++) fin[size_t(i)] = reinterpret_cast<const float *>(in_data)[i];
        break;
      case QAudioFormat::Int16: {
        auto *p = reinterpret_cast<const int16_t *>(in_data);
        for (qint64 i = 0; i < in_frames * ch; i++) fin[size_t(i)] = p[i] / 32768.0f;
        break;
      }
      case QAudioFormat::Int32: {
        auto *p = reinterpret_cast<const int32_t *>(in_data);
        for (qint64 i = 0; i < in_frames * ch; i++) fin[size_t(i)] = p[i] / 2147483648.0f;
        break;
      }
      default:
        out.clear();
        return 0;
    }

    std::vector<float> fout(size_t(out_frames * ch));
    if (speed_ == 1.0) {
      fout = fin;
      if (qint64(fout.size() / ch) != out_frames) {
        out_frames = in_frames;
        fout.resize(size_t(out_frames * ch));
      }
    } else {
      for (qint64 j = 0; j < out_frames; j++) {
        double pos = double(j) * speed_;
        qint64 i0 = qint64(pos);
        double frac = pos - i0;
        if (i0 >= in_frames - 1) {
          i0 = in_frames - 1;
          frac = 0;
        }
        for (int c = 0; c < ch; c++) {
          float a = fin[size_t(i0 * ch + c)];
          float b = (i0 + 1 < in_frames) ? fin[size_t((i0 + 1) * ch + c)] : a;
          fout[size_t(j * ch + c)] = float(a + (b - a) * frac);
        }
      }
    }

    if (decay_ > 0.0) {
      for (qint64 j = 0; j < out_frames; j++)
        for (int c = 0; c < ch; c++)
          fout[size_t(j * ch + c)] = processReverbSample(fout[size_t(j * ch + c)], c);
    }

    const int bpf = fmt.bytesPerFrame();
    out.resize(int(out_frames * bpf));
    char *dst = out.data();
    switch (fmt.sampleFormat()) {
      case QAudioFormat::Float:
        for (qint64 i = 0; i < out_frames * ch; i++) reinterpret_cast<float *>(dst)[i] = std::clamp(fout[size_t(i)], -1.0f, 1.0f);
        break;
      case QAudioFormat::Int16: {
        auto *p = reinterpret_cast<int16_t *>(dst);
        for (qint64 i = 0; i < out_frames * ch; i++) p[i] = int16_t(std::clamp(fout[size_t(i)], -1.0f, 1.0f) * 32767.0f);
        break;
      }
      case QAudioFormat::Int32: {
        auto *p = reinterpret_cast<int32_t *>(dst);
        for (qint64 i = 0; i < out_frames * ch; i++) p[i] = int32_t(std::clamp(fout[size_t(i)], -1.0f, 1.0f) * 2147483647.0f);
        break;
      }
      default:
        break;
    }
    return out_frames;
  }
}
