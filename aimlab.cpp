#include "aimlab.h"
#include "./ui_aimlab.h"
#include "gameinterface.h"
#include<QPainter>
#include <QButtonGroup>
#include <QOverload>
Aimlab::Aimlab(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Aimlab)
{
    ui->setupUi(this);
    //设置固定大小
    setFixedSize(1200,600);
    //设置图标
    setWindowIcon(QIcon(":/picture/tubiao.jpg"));
    //设置标题
    setWindowTitle("VALORANT");
    //退出按钮的实现
    connect(ui->exitbutton,&QPushButton::clicked,[=](){
        this->close();
    });
    //开始游戏按钮的实现
    connect(ui->startbutton,&QPushButton::clicked,this,&Aimlab::onStartGameClicked);
    levelButton = ui->levelButton;
    connect(levelButton,&QPushButton::clicked,this,&Aimlab::showDifficultyButtons);
    QStringList diffNames = {"EZ", "普通", "有点强度", "真强度", "鼠标冒烟"};
    QButtonGroup *diffGroup = new QButtonGroup(this);
    for (int i = 0; i < 5; ++i) {
        QPushButton *btn = new QPushButton(diffNames[i], this);
        btn->setFixedSize(100, 40);
        btn->move(100 + i * 120, 500); // 横向排列
        btn->hide();
        diffButtons.append(btn);
        diffGroup->addButton(btn, i); // 绑定ID 0-4
    }
    // 连接难度按钮的点击信号
    // 使用 lambda 表达式将 QAbstractButton* 转换为 int
    connect(
        diffGroup,
        &QButtonGroup::buttonClicked,
        this,
        [this, diffGroup](QAbstractButton* button) {
            int level = diffGroup->id(button);  // 获取按钮的 ID（按钮的索引）
            setDifficulty(level);  // 调用 setDifficulty 函数
        }
        );


}



//设置背景图片
void Aimlab::paintEvent(QPaintEvent*){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/picture/beijing1.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}
void Aimlab::onStartGameClicked() {
    this->hide();
    gameInterface = new GameInterface(this);
    // 启动游戏时应用当前难度
    switch (currentDifficulty) {
    case 0: gameInterface->setSpawnInterval(2000); break;
    case 1: gameInterface->setSpawnInterval(1500); break;
    case 2: gameInterface->setSpawnInterval(1000); break;
    case 3: gameInterface->setSpawnInterval(500);  break;
    case 4: gameInterface->setSpawnInterval(300);  break;
    }
    gameInterface->show();
}
// 显示/隐藏难度按钮
void Aimlab::showDifficultyButtons() {
    bool isVisible = diffButtons.first()->isVisible();
    for (QPushButton *btn : diffButtons) {
        btn->setVisible(!isVisible);
    }
}
// 设置难度并隐藏按钮
void Aimlab::setDifficulty(int level) {
    currentDifficulty = level; // 保存当前难度
    // 不再直接操作 gameInterface
    for (QPushButton *btn : diffButtons) {
        btn->hide();
    }

    // 根据难度级别设置生成间隔
    int interval = 0;
    switch (level) {
    case 0: interval = 2000; break;
    case 1: interval = 1500; break;
    case 2: interval = 1000; break;
    case 3: interval = 500;  break;
    case 4: interval = 300;  break;
    }

    // 仅当 gameInterface 存在时设置间隔
    if (gameInterface) {
        gameInterface->setSpawnInterval(interval);
    }
}
int Aimlab::getCurrentSpawnInterval() const {
    switch (currentDifficulty) {
    case 0: return 2000;
    case 1: return 1500;
    case 2: return 1000;
    case 3: return 500;
    case 4: return 300;
    default: return 1000; // 默认值
    }
}
Aimlab::~Aimlab()
{
    delete ui;
    if (gameInterface) {
        delete gameInterface;
    }
}
