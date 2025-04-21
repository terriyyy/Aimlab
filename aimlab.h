#ifndef AIMLAB_H
#define AIMLAB_H

#include <QMainWindow>
#include <QPushButton>
class GameInterface;
QT_BEGIN_NAMESPACE
namespace Ui {
class Aimlab;
}
QT_END_NAMESPACE

class Aimlab : public QMainWindow
{
    Q_OBJECT

public:
    Aimlab(QWidget *parent = nullptr);
    ~Aimlab();
    //重写paintEvent事件 画背景
    void paintEvent(QPaintEvent*);
private slots:
    void onStartGameClicked();//开始游戏按钮点击的槽函数
    void showDifficultyButtons();       // 显示难度选择按钮
    void setDifficulty(int level);      // 设置难度
private:
    Ui::Aimlab *ui;
    //游戏界面的指针
    GameInterface* gameInterface;
    QPushButton *levelButton;
    QList<QPushButton*> diffButtons;   // 存储难度按钮
    int currentDifficulty = 2; // 默认难度
    int getCurrentSpawnInterval() const;
};
#endif // AIMLAB_H
