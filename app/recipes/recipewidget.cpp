#include "recipewidget.h"

namespace Recipes {
  const QVector<Card> &cards() {
    static const QVector<Card> all = {
      {QStringLiteral("Ome-rice"),
       QStringLiteral("20 min"),
       QStringLiteral("- 2 eggs\n- 1 bowl cooked rice\n- 1/2 onion, diced\n- 2 tbsp ketchup + extra for drawing faces\n- 1 tbsp butter\n- salt, pepper"),
       QStringLiteral("1. Fry onion in butter, add rice and ketchup.\n2. Beat eggs, make a thin omelette.\n3. Wrap rice, draw a face with ketchup.\n4. Eat while listening to something bouncy.")},
      {QStringLiteral("Miso soup"),
       QStringLiteral("10 min"),
       QStringLiteral("- 2 cups dashi\n- 2 tbsp miso paste\n- tofu cubes\n- wakame seaweed\n- spring onions"),
       QStringLiteral("1. Simmer dashi, add tofu and wakame.\n2. Turn off heat, dissolve miso.\n3. Never boil the miso. Chiyo insists.\n4. Top with spring onions.")},
      {QStringLiteral("Fluffy pancakes"),
       QStringLiteral("25 min"),
       QStringLiteral("- 2 eggs, separated\n- 4 tbsp flour\n- 2 tbsp milk\n- 2 tbsp sugar\n- butter + maple syrup"),
       QStringLiteral("1. Whip egg whites with sugar to stiff peaks.\n2. Fold in yolks, flour, milk gently.\n3. Cook low and slow with a lid on.\n4. Stack high, syrup generously.")},
      {QStringLiteral("Onigiri"),
       QStringLiteral("15 min"),
       QStringLiteral("- 2 bowls cooked rice\n- salted water for hands\n- umeboshi / tuna mayo / salmon\n- nori sheets"),
       QStringLiteral("1. Wet hands with salt water.\n2. Pack rice around the filling, triangle shape.\n3. Wrap with nori.\n4. Perfect homework snack.")},
    };
    return all;
  }

  Widget::Widget(QWidget *parent) : QWidget(parent) {
    auto *lay = new QVBoxLayout(this);
    nowPlaying = new QLabel(tr("Nothing cooking yet"), this);
    nowPlaying->setWordWrap(true);
    list = new QListWidget(this);
    for (const auto &c : cards()) {
      list->addItem(QStringLiteral("%1 (%2)").arg(c.name, c.time));
    }
    detail = new QTextBrowser(this);
    lay->addWidget(nowPlaying);
    lay->addWidget(list);
    lay->addWidget(detail);
    setLayout(lay);
    connect(list, &QListWidget::currentRowChanged, this, &Widget::showCard);
    if (!cards().isEmpty()) {
      list->setCurrentRow(0);
    }
    onStopped();
  }

  void Widget::onStarted(const Track &track) {
    nowPlaying->setText(tr("Cooking to: %1").arg(track.shortText()));
  }

  void Widget::onStopped() {
    nowPlaying->setText(tr("Nothing cooking yet"));
  }

  void Widget::showCard(int row) {
    const auto &all = cards();
    if (row < 0 || row >= all.size()) {
      return;
    }
    const auto &c = all[row];
    detail->setPlainText(
        QStringLiteral("%1 (%2)\n\nIngredients:\n%3\n\nSteps:\n%4")
            .arg(c.name, c.time, c.ingredients, c.steps));
  }
}
