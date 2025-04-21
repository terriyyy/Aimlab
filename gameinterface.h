#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H
class Aimlab;
#include<QPushButton>
#include <QWidget>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
class GameInterface : public QWidget
{
    Q_OBJECT

public:
    explicit GameInterface(Aimlab* mainWindow,QWidget *parent = nullptr);
    ~GameInterface();
    void setSpawnInterval(int interval);
protected:
    void paintEvent(QPaintEvent *event) ;
    void keyPressEvent(QKeyEvent*event) ;
    void mousePressEvent(QMouseEvent*event);
private slots:
    void onBackButtonClicked();
    void onstartgame();
    void spawnTarget();
    void updatetimer();
private:
    Aimlab* mainWindow;
    QPushButton* backButton;
    QPushButton* gameon;
    bool isGameStarted =false;
    QGraphicsScene* scene;
    QGraphicsView*view;
    QTimer*spawnTimer;//生成小球的定时器
    QTimer*gameTimer;//倒计时的定时器
    QLabel*timeLabel;
    QLabel*scoreLabel;
    int remainingTime = 30;//时间
    int score = 0;//初始得分
    int spawnInterval = 500;//间隔时间
};

#endif // GAMEINTERFACE_H
