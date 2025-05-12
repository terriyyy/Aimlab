#include "aimlab.h"
#include "./ui_aimlab.h"
#include "gameinterface.h"
#include<QPainter>
#include <QButtonGroup>
#include <QOverload>
#include <QSoundEffect>
#include<QPropertyAnimation>
Aimlab::Aimlab(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Aimlab)
{
    ui->setupUi(this);
    //设置固定大小
    setFixedSize(1200,600);
    //设置图标
    setWindowIcon(QIcon(":/picture/aimlab.jpg"));
    //设置标题
    setWindowTitle("AIMLAB");
    //退出按钮的实现
    connect(ui->exitbutton,&QPushButton::clicked,[=](){
        this->close();
    });
    //开始游戏按钮的实现
    connect(ui->startbutton,&QPushButton::clicked,this,&Aimlab::onStartGameClicked);




    QString styleSheet = R"(
        /* 主窗口背景 */
        QMainWindow {
            background: qradialgradient(cx:0.5, cy:0.5, radius: 1.5,
                          fx:0.5, fy:0.5,
                          stop:0 #1a1a2e, stop:1 #16213e);
        }

        /* 通用按钮样式 */
        QPushButton {
            min-width: 130px;
            min-height: 25px;
            padding: 12px 24px;
            border: none;
            border-radius: 8px;
            font-family: 'Microsoft YaHei';
            font-size: 18px;
            font-weight: bold;
            color: white;
            background-color: rgba(255, 255, 255, 0.15);

        }

        /* 开始游戏按钮 */
        QPushButton#startbutton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                      stop:0 #00b894, stop:1 #00cec9);
            font-size: 22px;
            min-height: 60px;
        }

        /* 退出按钮 */
        QPushButton#exitbutton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                      stop:0 #ff7675, stop:1 #d63031);
        }

        /* 难度选择按钮 */
        QPushButton#levelButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                      stop:0 #6c5ce7, stop:1 #a363d9);
        }

        /* 难度选项按钮 */
        QPushButton.difficultyBtn {
            background: rgba(108, 92, 231, 0.9);
            font-size: 16px;
            min-width: 100px;
            min-height: 40px;
            border: 2px solid rgba(255, 255, 255, 0.3);
        }

        /* 悬停效果 */
        QPushButton:hover {

            background-color: rgba(255, 255, 255, 0.25);
        }

        /* 按下效果 */
        QPushButton:pressed {
        padding-top: 13px;  /* 模拟按下效果 */
        padding-bottom: 11px;

        }

        /* 难度按钮悬停特效 */
        QPushButton.difficultyBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                      stop:0 #786fa6, stop:1 #574b90);
        }
    )";

    this->setStyleSheet(styleSheet);

    // ========== 设置难度按钮特殊样式 ==========
    QStringList difficultyColors = {
        "#00b894",
        "#fdcb6e",
        "#e17055",
        "#d63031",
        "#6c5ce7"
    };

    for(int i = 0; i < diffButtons.size(); ++i) {
        QPushButton* btn = diffButtons[i];
        btn->setStyleSheet(QString(R"(
            QPushButton {
                background: %1;
                border-radius: 6px;
                color: white;
                border: 2px solid %2;
            }
            QPushButton:hover {
                background: %3;
                border-color: rgba(255,255,255,0.5);
            }
        )").arg(difficultyColors[i])
                               .arg(QColor(difficultyColors[i]).lighter(120).name())
                               .arg(QColor(difficultyColors[i]).darker(120).name()));
    }




    // 添加动画
    QPushButton* startBtn = ui->startbutton;
    QPropertyAnimation *anim = new QPropertyAnimation(startBtn, "geometry", this);
    anim->setDuration(1000);
    anim->setLoopCount(-1); // 无限循环
    anim->setKeyValueAt(0, QRect(startBtn->x(), startBtn->y(), startBtn->width(), startBtn->height()));
    anim->setKeyValueAt(0.5, QRect(startBtn->x(), startBtn->y()-5, startBtn->width(), startBtn->height()));
    anim->setKeyValueAt(1, QRect(startBtn->x(), startBtn->y(), startBtn->width(), startBtn->height()));
    anim->start();
    ui->exitbutton->move(0,90);
    QPushButton* exitBtn = ui->exitbutton;
    QPropertyAnimation *bnim = new QPropertyAnimation(exitBtn, "geometry", this);
    bnim->setDuration(1000);
    bnim->setLoopCount(-1); // 无限循环
    bnim->setKeyValueAt(0, QRect(exitBtn->x(), exitBtn->y(), exitBtn->width(), exitBtn->height()));
    bnim->setKeyValueAt(0.5, QRect(exitBtn->x(), exitBtn->y()-5, exitBtn->width(), exitBtn->height()));
    bnim->setKeyValueAt(1, QRect(exitBtn->x(), exitBtn->y(), exitBtn->width(), exitBtn->height()));
    bnim->start();
    QPushButton* levelBtn =ui->levelButton;
    ui->levelButton->move(0,150);
    QPropertyAnimation *cnim = new QPropertyAnimation(levelBtn, "geometry", this);
    cnim->setDuration(1000);
    cnim->setLoopCount(-1); // 无限循环
    cnim->setKeyValueAt(0, QRect(levelBtn->x(), levelBtn->y(), levelBtn->width(), levelBtn->height()));
    cnim->setKeyValueAt(0.5, QRect(levelBtn->x(), levelBtn->y()-5, levelBtn->width(), levelBtn->height()));
    cnim->setKeyValueAt(1, QRect(levelBtn->x(), levelBtn->y(), levelBtn->width(), levelBtn->height()));
    cnim->start();
    //难度选择按钮
    levelButton = ui->levelButton;
    connect(levelButton,&QPushButton::clicked,this,&Aimlab::showDifficultyButtons);
    QStringList diffNames = {"I", "II", "III", "IV", "V"};
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
    //
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
    pix.load(":/picture/aimlab.jpg");
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
