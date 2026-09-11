#include <QtTest>

#include "pretty/prettydialog.h"
#include "track.h"

class TestPretty : public QObject {
  Q_OBJECT
private slots:
  void groupKeyIgnoresCaseAndSpaces();
  void groupKeySeparatesFields();
  void tidiedCollapsesWhitespace();
  void tidiedLeavesCleanAlone();
};

static Track makeTrack(const QString &artist, const QString &album, const QString &title) {
  return Track(QStringLiteral("/music/x.flac"), 0, artist, album, title, 1, 2020, 180000, 2, 900, 44100);
}

void TestPretty::groupKeyIgnoresCaseAndSpaces() {
  const Track a = makeTrack(QStringLiteral("Pink Floyd"), QStringLiteral("Animals"), QStringLiteral("Dogs"));
  const Track b = makeTrack(QStringLiteral("  pink floyd "), QStringLiteral("animals"), QStringLiteral("DOGS"));
  QCOMPARE(Pretty::groupKey(a), Pretty::groupKey(b));
}

void TestPretty::groupKeySeparatesFields() {
  const Track a = makeTrack(QStringLiteral("A"), QStringLiteral("B"), QStringLiteral("C"));
  const Track b = makeTrack(QStringLiteral("A"), QStringLiteral("B"), QStringLiteral("D"));
  const Track c = makeTrack(QStringLiteral("A"), QStringLiteral("X"), QStringLiteral("C"));
  QVERIFY(Pretty::groupKey(a) != Pretty::groupKey(b));
  QVERIFY(Pretty::groupKey(a) != Pretty::groupKey(c));
}

void TestPretty::tidiedCollapsesWhitespace() {
  QCOMPARE(Pretty::tidied(QStringLiteral("  hello   world  ")), QStringLiteral("hello world"));
  QCOMPARE(Pretty::tidied(QStringLiteral("AC  /  DC")), QStringLiteral("AC / DC"));
  QCOMPARE(Pretty::tidied(QStringLiteral("")), QStringLiteral(""));
}

void TestPretty::tidiedLeavesCleanAlone() {
  QCOMPARE(Pretty::tidied(QStringLiteral("AC/DC")), QStringLiteral("AC/DC"));
  QCOMPARE(Pretty::tidied(QStringLiteral("Pink Floyd")), QStringLiteral("Pink Floyd"));
}

QTEST_MAIN(TestPretty)
#include "tst_pretty.moc"
