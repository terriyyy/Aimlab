#include "gameinterface.h"
#include "aimlab.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QGraphicsEllipseItem>
#include <QMessageBox>
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
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //初始化标签
    timeLabel=new QLabel("剩余时间：30",this);
    timeLabel->setFixedWidth(200);
    timeLabel->setStyleSheet("color: white; font-size: 20px;");
    timeLabel->move(1000,20);

    scoreLabel=new QLabel("得分:0",this);
    scoreLabel->setStyleSheet("color: white; font-size: 20px;");
    scoreLabel->move(1000,60);

    // 布局

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // 顶部栏布局（返回按钮和时间/得分标签）
    QHBoxLayout *topBar = new QHBoxLayout;
    topBar->addWidget(backButton);
    topBar->addStretch();
    topBar->addWidget(timeLabel);
    topBar->addWidget(scoreLabel);

    mainLayout->addLayout(topBar);
    mainLayout->addWidget(view);
    mainLayout->addWidget(gameon, 0, Qt::AlignCenter);




    //初始化定时器
    spawnTimer=new QTimer(this);
    gameTimer=new QTimer(this);
    setFocusPolicy(Qt::StrongFocus);


}
void GameInterface::keyPressEvent(QKeyEvent*event){
    if (event->key() == Qt::Key_T && !isGameStarted) {
        onstartgame();
    }
    QWidget::keyPressEvent(event);
}
void GameInterface::onstartgame(){
    isGameStarted = true;
    gameon->hide();
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
GameInterface::~GameInterface()
{
    // 析构函数
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
    scene->addItem(target);
}
//鼠标点击事件
void GameInterface::mousePressEvent(QMouseEvent*event){
    if (!isGameStarted) return;
    QPointF scenePos = view->mapToScene(event->pos());
    QGraphicsItem* item = scene->itemAt(scenePos, QTransform());
    if (item && item->type() == QGraphicsEllipseItem::Type) {
        //点击到小球，加分
        scene->removeItem(item);
        delete item;
        score += 100; // 每次点击加100分
        scoreLabel->setText("得分: " + QString::number(score));
    }else {
        // 点击到空白区域：扣分（例如扣50分）
        score -= 50;
        scoreLabel->setStyleSheet("color: red; font-size: 20px;");
        QTimer::singleShot(200, [this]() {
            scoreLabel->setStyleSheet("color: white; font-size: 20px;");
        });
        // 确保分数不低于0
        if (score < 0) score = 0;
    }
    // 更新得分显示
    scoreLabel->setText("得分: " + QString::number(score));
    QWidget::mousePressEvent(event);
}
//更新倒计时
void GameInterface::updatetimer(){
    remainingTime--;
    timeLabel->setText("剩余时间: " + QString("%1").arg(remainingTime, 2, 10, QLatin1Char('0')));
    if (remainingTime <= 0) {
        spawnTimer->stop();
        gameTimer->stop();
        QMessageBox::information(this, "游戏结束", "最终得分: " + QString::number(score));
        this->hide();
        mainWindow->show();
    }
}
void GameInterface::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    // 可以在这里绘制游戏界面的背景等
    QPixmap pix2;
    pix2.load(":/picture/beijing1.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix2);
    backButton->raise();
    gameon->raise();
    timeLabel->raise();
    scoreLabel->raise();
}
void GameInterface::setSpawnInterval(int interval) {
    spawnInterval = interval;
    if (spawnTimer->isActive()) {
        spawnTimer->setInterval(spawnInterval); // 动态调整生成间隔
    }
}
