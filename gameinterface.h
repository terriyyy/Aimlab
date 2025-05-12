#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H
class Aimlab;

#include<QPushButton>
#include <QWidget>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include<QSoundEffect>
#include <QtWidgets/QProgressBar>
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
    QLabel*highScoreLabel;
    int highScore=0;
    int hitCounter = 0;                     // 连续命中计数器
    QList<QSoundEffect*> currentSoundSet;    // 当前音效组的音效

    void initializeSoundEffects();          // 初始化音效
    void playHitSound();                    // 播放命中音效
    void resetHitCounter();                 // 重置计数器
    void updateComboDisplay();
    void resetCombo();
    int combo = 0;                   // 当前连击数
    int maxCombo = 0;                // 历史最大连击记录
    QLabel* comboLabel;              // 连击显示
    QTimer* comboTimeoutTimer;       // 连击超时计时器
    QProgressBar* comboProgress;     // 连击进度条
    //结束统计
    int totalHits = 0;          // 总命中次数
    int totalMisses = 0;        // 总失误次数
    QLabel* accuracyLabel;      // 命中率标签
    QLabel* maxComboLabel;      // 最大连击标签
    QPushButton* retryButton;   // 重试按钮
    QPushButton* menuButton;    // 返回菜单按钮

    void showGameStats();       // 显示统计界面
    void createStatsUI();       // 创建统计界面元素
    void resetGame();
    int currentMaxCombo = 0;          // 本次游戏最高连击
    qint64 totalReactionTime = 0;     // 总反应时间（毫秒）
    int validHits = 0;                // 有效命中次数（用于反应时间计算）
    QGraphicsEffect* createShadowEffect();
    QString scoreLabelOriginalStyle;

};

#endif // GAMEINTERFACE_H
