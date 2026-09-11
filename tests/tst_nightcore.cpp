#include <QtTest>

#include "nightcore/nightcore.h"

#include <QAudioFormat>
#include <QByteArray>

#include <cmath>
#include <vector>

using Nightcore::Processor;

class TestNightcore : public QObject {
  Q_OBJECT
private slots:
  void clampsSpeedAndDecay();
  void passthroughIsIdentity();
  void resampleRatioFollowsSpeed();
  void dryReverbChangesNothing();
  void wetReverbStaysBounded();
  void resetClearsReverbTail();
  void int16Roundtrip();
};

static QAudioFormat floatStereo44100() {
  QAudioFormat fmt;
  fmt.setSampleRate(44100);
  fmt.setChannelCount(2);
  fmt.setSampleFormat(QAudioFormat::Float);
  return fmt;
}

static std::vector<float> sineFrames(int frames, int channels, float freq = 440.0f) {
  std::vector<float> out(size_t(frames * channels));
  for (int i = 0; i < frames; i++) {
    const float v = std::sin(2.0f * float(M_PI) * freq * i / 44100.0f);
    for (int c = 0; c < channels; c++) {
      out[size_t(i * channels + c)] = v;
    }
  }
  return out;
}

void TestNightcore::clampsSpeedAndDecay() {
  Processor p;
  p.setSpeed(9.9);
  QCOMPARE(p.speed(), 2.0);
  p.setSpeed(0.01);
  QCOMPARE(p.speed(), 0.5);
  p.setSpeed(1.25);
  QCOMPARE(p.speed(), 1.25);
  p.setReverbDecay(-1.0);
  QCOMPARE(p.reverbDecay(), 0.0);
  p.setReverbDecay(2.0);
  QCOMPARE(p.reverbDecay(), 1.0);
  QVERIFY(p.isPassthrough());
  p.setSpeed(1.5);
  QVERIFY(!p.isPassthrough());
}

void TestNightcore::passthroughIsIdentity() {
  Processor p;
  p.setSampleRate(44100);
  const auto in = sineFrames(1024, 2);
  QByteArray out;
  const qint64 n = p.processChunk(reinterpret_cast<const char *>(in.data()), 1024,
                                  floatStereo44100(), out);
  QCOMPARE(n, 1024);
  QCOMPARE(out.size(), 1024 * 2 * int(sizeof(float)));
  const auto *f = reinterpret_cast<const float *>(out.constData());
  for (int i = 0; i < 1024 * 2; i++) {
    QCOMPARE(f[i], in[size_t(i)]);
  }
}

void TestNightcore::resampleRatioFollowsSpeed() {
  Processor p;
  p.setSampleRate(44100);
  const auto in = sineFrames(1000, 2);
  QByteArray out;
  p.setSpeed(2.0);
  QCOMPARE(p.processChunk(reinterpret_cast<const char *>(in.data()), 1000,
                          floatStereo44100(), out),
           500);
  p.setSpeed(0.5);
  QCOMPARE(p.processChunk(reinterpret_cast<const char *>(in.data()), 1000,
                          floatStereo44100(), out),
           2000);
  // First output sample always equals first input sample (pos 0).
  p.setSpeed(1.25);
  p.processChunk(reinterpret_cast<const char *>(in.data()), 1000, floatStereo44100(), out);
  const auto *f = reinterpret_cast<const float *>(out.constData());
  QCOMPARE(f[0], in[0]);
  QCOMPARE(f[1], in[1]);
}

void TestNightcore::dryReverbChangesNothing() {
  Processor p;
  p.setSampleRate(44100);
  p.setReverbDecay(0.0);
  const auto in = sineFrames(512, 2);
  QByteArray out;
  p.processChunk(reinterpret_cast<const char *>(in.data()), 512, floatStereo44100(), out);
  const auto *f = reinterpret_cast<const float *>(out.constData());
  for (int i = 0; i < 512 * 2; i++) {
    QCOMPARE(f[i], in[size_t(i)]);
  }
}

void TestNightcore::wetReverbStaysBounded() {
  Processor p;
  p.setSampleRate(44100);
  p.setReverbDecay(1.0);
  const auto in = sineFrames(2048, 2);
  QByteArray out;
  const qint64 n = p.processChunk(reinterpret_cast<const char *>(in.data()), 2048,
                                  floatStereo44100(), out);
  QCOMPARE(n, 2048);
  const auto *f = reinterpret_cast<const float *>(out.constData());
  bool differs = false;
  for (int i = 0; i < 2048 * 2; i++) {
    QVERIFY(!std::isnan(f[i]) && !std::isinf(f[i]));
    QVERIFY(std::fabs(f[i]) <= 1.0f);
    if (f[i] != in[size_t(i)]) {
      differs = true;
    }
  }
  QVERIFY(differs); // wet tail must actually change the signal
}

void TestNightcore::resetClearsReverbTail() {
  Processor p;
  p.setSampleRate(44100);
  p.setReverbDecay(1.0);
  const auto in = sineFrames(1024, 2);
  QByteArray wet, dry;
  p.processChunk(reinterpret_cast<const char *>(in.data()), 1024, floatStereo44100(), wet);
  p.reset();
  p.setReverbDecay(0.0);
  p.processChunk(reinterpret_cast<const char *>(in.data()), 1024, floatStereo44100(), dry);
  QCOMPARE(wet.size(), dry.size());
  const auto *w = reinterpret_cast<const float *>(wet.constData());
  const auto *d = reinterpret_cast<const float *>(dry.constData());
  bool differs = false;
  for (int i = 0; i < 1024 * 2; i++) {
    if (w[i] != d[i]) {
      differs = true;
      break;
    }
  }
  QVERIFY(differs);
}

void TestNightcore::int16Roundtrip() {
  Processor p;
  p.setSampleRate(44100);
  QAudioFormat fmt = floatStereo44100();
  fmt.setSampleFormat(QAudioFormat::Int16);
  const int frames = 256, ch = 2;
  std::vector<int16_t> in(size_t(frames * ch), 10000);
  QByteArray out;
  const qint64 n = p.processChunk(reinterpret_cast<const char *>(in.data()), frames, fmt, out);
  QCOMPARE(n, frames);
  const auto *s = reinterpret_cast<const int16_t *>(out.constData());
  for (int i = 0; i < frames * ch; i++) {
    QVERIFY(std::abs(int(s[i]) - 10000) <= 1);
  }
}

QTEST_MAIN(TestNightcore)
#include "tst_nightcore.moc"
