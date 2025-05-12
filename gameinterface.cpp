#include "gameinterface.h"
#include "aimlab.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QGraphicsEllipseItem>
#include <QMessageBox>
#include<QSettings>
#include <QtCore/QPropertyAnimation>
#include <QtWidgets/QGraphicsBlurEffect>
#include<QDateTime>
#include <QGraphicsDropShadowEffect>
GameInterface::GameInterface(Aimlab* mainWindow,QWidget *parent) : QWidget(parent), mainWindow(mainWindow)
{
    setFixedSize(1200, 600);
    setWindowTitle("Aimlab");
    //创建返回主页面的按钮
    backButton = new QPushButton("返回主界面", this);
    backButton->setGeometry(10, 10, 150, 30);
    connect(backButton,&QPushButton::clicked,this,&GameInterface::onBackButtonClicked);
    gameon=new QPushButton("Start Game(Press T)",this);
    gameon->setFixedSize(200, 50);// 设置按钮大小
    gameon->setStyleSheet("font-size: 20px;");// 设置字体
    QVBoxLayout *layout = new QVBoxLayout(this); // 将按钮居中
    layout->addWidget(gameon, 0, Qt::AlignCenter);
    connect(gameon,&QPushButton::clicked,this,&GameInterface::onstartgame);// 连接按钮点击信号
    setFocusPolicy(Qt::StrongFocus);// 设置窗口焦点策略，确保能接收键盘事件
    //初始化场景和选图
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1200, 600);//场景大小
    view = new QGraphicsView(scene, this);
    view->setFixedSize(1200, 600);
    view->setStyleSheet("background: transparent; border: none;");
    view->setFrameStyle(QFrame::NoFrame);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //初始化标签
    timeLabel=new QLabel("剩余时间：30",this);
    timeLabel->setStyleSheet(R"(
    QLabel {
        color: #00FF88;
        font-size: 20px;
        font-weight: bold;
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
            stop:0 rgba(0, 64, 128, 180),
            stop:1 rgba(0, 128, 255, 150));
        border-radius: 10px;
        padding: 8px 15px;
        border: 2px solid rgba(255,255,255,0.3);
    }
)");
    timeLabel->setGraphicsEffect(createShadowEffect());
    timeLabel->setFixedSize(200, 50);  // 固定尺寸保证布局稳定
    timeLabel->move(950, 20);  // 调整位置


    scoreLabel=new QLabel("得分:0",this);
    scoreLabel->setStyleSheet(R"(
    QLabel {
        color: #FFD700;
        font-size: 20px;
        font-weight: bold;
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
            stop:0 rgba(128, 0, 128, 180),
            stop:1 rgba(255, 105, 180, 150));
        border-radius: 10px;
        padding: 8px 15px;
        border: 2px solid rgba(255,255,255,0.3);
    }
)");
    scoreLabel->setGraphicsEffect(createShadowEffect());
    scoreLabel->setFixedSize(200, 50);
    scoreLabel->move(950, 80);
    scoreLabelOriginalStyle = scoreLabel->styleSheet();

    // 初始化历史最高分标签
    highScoreLabel = new QLabel("历史最高: 0", this);
    highScoreLabel->setStyleSheet(R"(
    QLabel {
        color: #FF7F50;
        font-size: 20px;
        font-weight: bold;
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
            stop:0 rgba(255, 69, 0, 180),
            stop:1 rgba(255, 140, 0, 150));
        border-radius: 10px;
        padding: 8px 15px;
        border: 2px solid rgba(255,255,255,0.3);
    }
)");
    highScoreLabel->setGraphicsEffect(createShadowEffect());
    highScoreLabel->setFixedSize(200, 50);
    highScoreLabel->move(950, 140);


    // 布局

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // 顶部栏布局（返回按钮和时间/得分标签）
    QHBoxLayout *topBar = new QHBoxLayout;
    topBar->addWidget(backButton);
    topBar->addStretch();
    topBar->addWidget(timeLabel);
    topBar->addWidget(scoreLabel);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addLayout(topBar);
    mainLayout->addWidget(view);
    mainLayout->addWidget(gameon, 0, Qt::AlignCenter);

    //初始化定时器
    spawnTimer=new QTimer(this);
    gameTimer=new QTimer(this);
    setFocusPolicy(Qt::StrongFocus);


    // 从配置文件加载历史最高分
    QSettings settings("MyStudio", "AimLab");
    highScore = settings.value("HighScore", 0).toInt();
    maxCombo = settings.value("MaxCombo", 0).toInt();
    qDebug() << "Current High Score:" << highScore;
    highScoreLabel->setText("历史最高: " + QString::number(highScore));
    initializeSoundEffects();
    //连击
    comboLabel = new QLabel(this);
    comboLabel->setStyleSheet(R"(
    QLabel {
        color: #FFD700;
        font-size: 40px;
        font-weight: bold;
        qproperty-alignment: AlignCenter;

    }
)");
    //连击相关元素初始化
    comboLabel->setAlignment(Qt::AlignCenter);
    comboLabel->setFixedSize(300, 60);  // 设置固定大小
    comboLabel->move((width()-300)/2, 150);  // 水平居中
    comboLabel->hide();

    comboProgress = new QProgressBar(this);
    comboProgress->setRange(0, 5);
    comboProgress->setTextVisible(false);
    comboProgress->setFixedSize(300, 15);
    comboProgress->setStyleSheet(R"(
    QProgressBar {
        border: 2px solid #FFD700;
        border-radius: 7px;
        background: rgba(30,30,30,200);
        height: 15px;
    }
    QProgressBar::chunk {
        background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
            stop:0 #FF416C, stop:1 #FF4B2B);
        border-radius: 5px;
        border: 1px solid #FFD700;
    }
)");
    comboProgress->move((width()-300)/2, 220);  // 水平居中
    comboProgress->hide();
    QGraphicsDropShadowEffect *textShadow = new QGraphicsDropShadowEffect;
    textShadow->setBlurRadius(4);
    textShadow->setOffset(2, 2);
    textShadow->setColor(Qt::black);
    comboLabel->setGraphicsEffect(textShadow);

    comboTimeoutTimer = new QTimer(this);
    comboTimeoutTimer->setSingleShot(true);
    connect(comboTimeoutTimer, &QTimer::timeout, [this]{
        combo = 0;
        comboLabel->hide();
        comboProgress->hide();
    });
    view->lower(); // 确保视图在底层
    comboProgress->raise(); // 确保进度条在顶层
    comboProgress->setValue(1); // 设置初始值避免空白
    comboLabel->setAttribute(Qt::WA_TranslucentBackground);
    comboProgress->setAttribute(Qt::WA_TranslucentBackground);

    // 统计界面元素初始化
    accuracyLabel = new QLabel(this);
    accuracyLabel->setStyleSheet("color: white; font-size: 24px;");
    accuracyLabel->move(400, 200);
    accuracyLabel->hide();
    accuracyLabel->setMinimumWidth(200);
    maxComboLabel = new QLabel(this);
    maxComboLabel->setStyleSheet("color: #FFD700; font-size: 24px;");
    maxComboLabel->move(400, 250);
    maxComboLabel->hide();
    maxComboLabel->setMinimumWidth(200);

    retryButton = new QPushButton("再试一次 (R)", this);
    retryButton->setFixedSize(200, 50);
    retryButton->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #6c5ce7, stop:1 #a363d9);"
        "  color: white;"
        "  font-size: 20px;"
        "  border-radius: 10px;"
        "}"
        "QPushButton:hover { background-color: #786fa6; }"
        );
    retryButton->move(300, 350);
    retryButton->hide();
    connect(retryButton, &QPushButton::clicked, this, &GameInterface::onstartgame);

    menuButton = new QPushButton("返回主菜单 (M)", this);
    menuButton->setFixedSize(200, 50);
    menuButton->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #e17055, stop:1 #d63031);"
        "  color: white;"
        "  font-size: 20px;"
        "  border-radius: 10px;"
        "}"
        "QPushButton:hover { background-color: #ff7675; }"
        );
    menuButton->move(700, 350);
    menuButton->hide();
    connect(menuButton, &QPushButton::clicked, this, &GameInterface::onBackButtonClicked);



}
void GameInterface::keyPressEvent(QKeyEvent*event){
    if (event->key() == Qt::Key_T && !isGameStarted) {
        onstartgame();
    }
    else if (remainingTime <= 0) { // 游戏结束时响应按键
        if (event->key() == Qt::Key_R) {
            onstartgame();
        }
        else if (event->key() == Qt::Key_M) {
            onBackButtonClicked();
        }
    }
    QWidget::keyPressEvent(event);
}
void GameInterface::onstartgame(){
    resetGame();
    isGameStarted = true;
    gameon->hide();
    resetHitCounter();
    //重置游戏状态
    scene->clear();
    score=0;
    remainingTime=30;
    scoreLabel->setText("得分：0");
    timeLabel->setText("剩余时间：30");
    //启动定时器
    spawnTimer->start(spawnInterval);//按当前间隔生成小球
    gameTimer->start(1000);//每秒更新一次时间
    connect(spawnTimer,&QTimer::timeout,this,&GameInterface::spawnTarget);
    connect(gameTimer,&QTimer::timeout,this,&GameInterface::updatetimer);

}

void GameInterface::onBackButtonClicked(){
    this->hide();
    mainWindow->show();
}
//生成随机小球
void GameInterface::spawnTarget(){
    int size = QRandomGenerator::global()->bounded(30, 60); // 小球大小30~60像素
    int x = QRandomGenerator::global()->bounded(50, 1150);  // x坐标范围50~1150
    int y = QRandomGenerator::global()->bounded(50, 550);   // y坐标范围50~550
    QGraphicsEllipseItem* target = new QGraphicsEllipseItem(0, 0, size, size);
    target->setPos(x, y);
    target->setBrush(Qt::red);   // 红色小球
    target->setPen(Qt::NoPen);   // 无边框
    // 添加时间戳存储（使用 QGraphicsItem 的 setData 方法）
    target->setData(0, QDateTime::currentMSecsSinceEpoch()); // 第0位存储生成时间
    scene->addItem(target);
}
//鼠标点击事件
void GameInterface::mousePressEvent(QMouseEvent*event){
    if (!isGameStarted) return;
    QPointF scenePos = view->mapToScene(event->pos());
    QGraphicsItem* item = scene->itemAt(scenePos, QTransform());
    if (item && item->type() == QGraphicsEllipseItem::Type) {
        qint64 spawnTime = item->data(0).toLongLong();
        //点击到小球，加分
        scene->removeItem(item);
        delete item;
        score += 100; // 每次点击加100分
        scoreLabel->setText("得分: " + QString::number(score));
        playHitSound();
        hitCounter++;
        //连击
        combo++;
        // 计算反应时间

        qint64 hitTime = QDateTime::currentMSecsSinceEpoch();
        totalReactionTime += (hitTime - spawnTime);
        validHits++;
        // 更新本次最高连击
        if(combo > currentMaxCombo) {
            currentMaxCombo = combo;
        }
        // 连击反馈
        comboLabel->setText(QString("%1 COMBO!").arg(combo));
        comboLabel->show();
        comboProgress->show();
        // 更新连击显示
        updateComboDisplay();
        // 进度条动画
        int segment = combo % 5;
        comboProgress->setValue(segment == 0 ? 5 : segment);
        comboTimeoutTimer->start(2000);
        totalHits++; // 记录命中

        if(combo %5 == 0&& combo != 0){
            score += 200; // 每5连击额外奖励
            QPropertyAnimation* anim = new QPropertyAnimation(comboLabel, "geometry");
            anim->setDuration(300);
            anim->setKeyValueAt(0, QRect((width()-300)/2, 150, 300, 60));
            anim->setKeyValueAt(0.5, QRect((width()-300)/2, 130, 300, 60));
            anim->setKeyValueAt(1, QRect((width()-300)/2, 150, 300, 60));
            anim->start();

            // 添加进度条填充动画
            QPropertyAnimation* progressAnim = new QPropertyAnimation(comboProgress, "value");
            progressAnim->setDuration(200);
            progressAnim->setStartValue(5);
            progressAnim->setEndValue(5);
            progressAnim->start();

        }
    }else {
        // 点击到空白区域：扣分（例如扣50分）
        //连击重置
        totalMisses++; // 记录失误
        resetCombo();
        score -= 50;
        //错误提示动画
        scoreLabel->setStyleSheet("color: red; font-size: 20px;");
        QTimer::singleShot(200, [this]() {
            scoreLabel->setStyleSheet(scoreLabelOriginalStyle); // 恢复原始样式
        });
        // 确保分数不低于0
        if (score < 0) score = 0;
    }
    // 更新得分显示
    scoreLabel->setText("得分: " + QString::number(score));
    if (score > highScore) {
        highScore = score;
        highScoreLabel->setText("历史最高: " + QString::number(highScore));
    }
    QWidget::mousePressEvent(event);
}
//更新倒计时
void GameInterface::updatetimer(){
    remainingTime--;
    timeLabel->setText("剩余时间: " + QString("%1").arg(remainingTime, 2, 10, QLatin1Char('0')));
    if (remainingTime <= 0) {
        spawnTimer->stop();
        gameTimer->stop();
        QSettings settings("MyStudio", "AimLab");
        if (score > settings.value("HighScore", 0).toInt()) {
            settings.setValue("HighScore", score);
        }
        QMessageBox::information(this, "游戏结束", "最终得分: " + QString::number(score));
        showGameStats();
        settings.setValue("MaxCombo", maxCombo);
        if(currentMaxCombo > maxCombo) {
            maxCombo = currentMaxCombo;
            settings.setValue("MaxCombo", maxCombo);
        }

    }
}
void GameInterface::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    // 可以在这里绘制游戏界面的背景等

    QPixmap background(":/picture/background.jpg");
    painter.drawPixmap(0, 0, width(), height(), background);
    backButton->raise();
    gameon->raise();
    timeLabel->raise();
    scoreLabel->raise();
    highScoreLabel->raise();
}
void GameInterface::setSpawnInterval(int interval) {
    spawnInterval = interval;
    if (spawnTimer->isActive()) {
        spawnTimer->setInterval(spawnInterval); // 动态调整生成间隔
    }
}
void GameInterface::initializeSoundEffects()
{
    // 初始化音效组
    QStringList soundPaths = {
        ":/sounds/group0/hundun1.wav",
        ":/sounds/group0/hundun2.wav",
        ":/sounds/group0/hundun3.wav",
        ":/sounds/group0/hundun4.wav",
        ":/sounds/group0/hundun5.wav"
    };

    foreach (const QString &path, soundPaths) {
        QSoundEffect* effect = new QSoundEffect(this);
        effect->setSource(QUrl(path));

        effect->setVolume(0.8f);  // 设置音量（0.0-1.0）
        currentSoundSet.append(effect);

    }
}
void GameInterface::playHitSound()
{
    if (!currentSoundSet.isEmpty()) {
        // 确定音效索引（0-4）
        int soundIndex = qMin(hitCounter, 4);  // 超过4次都使用最后一个音效

        if (QSoundEffect* effect = currentSoundSet.value(soundIndex)) {
            effect->play();
        }
    }
}

void GameInterface::resetHitCounter()
{
    hitCounter = 0;  // 重置连续命中计数器
}
void GameInterface::updateComboDisplay()
{
    // 更新连击文字
    comboLabel->setText(QString("%1 COMBO!").arg(combo));

    // 更新进度条（0-4显示1-5，5显示5）
    int segment = combo % 5;
    comboProgress->setValue(segment == 0 ? 5 : segment);
}

void GameInterface::resetCombo()
{
    combo = 0;
    comboLabel->hide();
    comboProgress->hide();
    resetHitCounter();
}
void GameInterface::showGameStats()
{
    // 计算平均反应时间（毫秒转秒）
    float avgReaction = validHits > 0 ?
                            totalReactionTime / (validHits * 1000.0f) : 0;
    // 计算命中率（避免除以零）
    float accuracy = 0.0f;
    if (totalHits + totalMisses > 0) {
        accuracy = (static_cast<float>(totalHits) / (totalHits + totalMisses)) * 100;
    }

    // 更新标签内容
    accuracyLabel->setText(QString("命中率: %1%").arg(accuracy, 0, 'f', 1));
    maxComboLabel->setText(QString("本次最高连击: %1").arg(currentMaxCombo));
    //反应时间
    QLabel* reactionLabel = new QLabel(this);
    reactionLabel->setFixedWidth(300);
    reactionLabel->setStyleSheet("color: #00FFFF; font-size: 24px;");
    reactionLabel->setText(QString("平均反应: %1 秒").arg(avgReaction, 0, 'f', 2));
    reactionLabel->move((width()-reactionLabel->width())/2, 300);
    reactionLabel->show();

    // 显示统计元素
    accuracyLabel->show();
    maxComboLabel->show();
    retryButton->show();
    menuButton->show();
    highScoreLabel->raise();

    // 添加模糊特效
    QGraphicsBlurEffect* blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(8);
    view->setGraphicsEffect(blur);
    // 确保统计元素在最上层
    accuracyLabel->raise();
    maxComboLabel->raise();
    retryButton->raise();
    menuButton->raise();
    highScoreLabel->raise();

    accuracyLabel->move((width()-accuracyLabel->width())/2, 200);
    maxComboLabel->move((width()-maxComboLabel->width())/2, 250);
    retryButton->move((width()-500)/2, 350);
    menuButton->move((width()+100)/2, 350);

    // 添加背景半透明遮罩
    QWidget* overlay = new QWidget(this);
    overlay->setStyleSheet("background: rgba(0,0,0,0.7);");
    overlay->resize(size());
    overlay->show();
    overlay->lower();



    // 居中显示统计信息
    accuracyLabel->move((width()-accuracyLabel->width())/2, 200);
    maxComboLabel->move((width()-maxComboLabel->width())/2, 250);
    retryButton->move((width()-500)/2, 350);
    menuButton->move((width()+100)/2, 350);
}

void GameInterface::resetGame()
{
    currentMaxCombo = 0;
    totalReactionTime = 0;
    validHits = 0;
    combo=0;
    totalHits = 0;
    totalMisses = 0;

    accuracyLabel->hide();
    maxComboLabel->hide();
    retryButton->hide();
    menuButton->hide();
    view->setGraphicsEffect(nullptr);

    // 移除遮罩层
    QList<QWidget*> overlays = findChildren<QWidget*>("overlay");
    foreach(QWidget* w, overlays) w->deleteLater();
}
QGraphicsEffect* GameInterface::createShadowEffect() {
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(12);
    shadow->setOffset(3, 3);
    shadow->setColor(QColor(0, 0, 0, 150));
    return shadow;
}
GameInterface::~GameInterface()
{
    delete scene;
    delete view;
    delete spawnTimer;
    delete gameTimer;
    qDeleteAll(currentSoundSet);
    // 析构函数
}

