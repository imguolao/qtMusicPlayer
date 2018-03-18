#include "music.h"
#include <QPixmap>//加载图片
#include <QFile>
#include <QPainter>
#include <QFileDialog>
#include <QUrl>
#include <QDebug>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QFileInfo>

int Music::z = 2;
Music::Music(QWidget *parent) : QWidget(parent)
{
    QPixmap background;
    background.load(":/image/music_bg.bmp");//加载背景图片
    this -> resize(background.width(),background.height());//设置窗口和图片大小一致background.size()
    this -> setWindowFlags(Qt::FramelessWindowHint);//产生一个无窗口边框的窗口，用户无法改变它的大小也无法移动它
    //this -> setAttribute(Qt::WA_TintedBackground);//添加一个有色背景
    add = false;
    moved = 0;
    timer = new QTimer(this);
    timer2 = new QTimer(this);

    player = new QMediaPlayer(this);//QMediaplayer用于解析音频文件和视频文件
    playList = new QMediaPlaylist;
    /*
     * QMediaPlaylist作为一个媒体播放列表，保存媒体的路径.
     * 此类有一个函数，void    setPlaybackMode(PlaybackMode mode)，
     * 而enum    PlaybackMode { CurrentItemOnce, CurrentItemInLoop, Sequential, Loop, Random }
     * 分别为播放一次，单循环，顺序，列表循环，随机播放。
    */

    init_controls();//创建按钮
    init_skin();//外部加载qss文件，绘制界面样式

    connect(player, SIGNAL(metaDataAvailableChanged(bool)), this, SLOT(showMessage(bool)));
    connect(seekSlider,SIGNAL(sliderMoved(int)), this,SLOT(seekChange(int)));
}
//绘制背景
void Music::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    QPixmap backgound;
    backgound.load(":/image/music_bg.bmp");
    paint.drawPixmap(0, 0, backgound.width(), backgound.height(),backgound);
    event ->accept();

}

//创建按钮
void Music::init_controls()
{   
    BtnClose = new QPushButton(this);
    BtnClose -> setObjectName("BtnClose");//如果要对这个对象单独设stylesheet的话一定要设置它的objectName
    BtnClose -> setGeometry(365,2,30,30);//窗口左上角为原点（X365，Y2），（宽30，高30）
    BtnClose -> setToolTip(tr("退出"));
    BtnClose -> setCursor(QCursor(Qt::PointingHandCursor));//鼠标指针形状为手

    BtnMin=new QPushButton(this);
    BtnMin->setObjectName(tr("BtnMin"));
    BtnMin->setGeometry(330,5,25,30);
    BtnMin->setToolTip(tr("最小化"));
    BtnMin->setCursor(QCursor(Qt::PointingHandCursor));

    playPattern=new QPushButton(this);
    /*QIcon icon1(":/image/Seq.png");
    playPattern->setIcon(icon1);*/
    playPattern->setObjectName(tr("playPattern"));
    playPattern->setGeometry(20,255,50,50);
    playPattern->setToolTip(tr("列表循环"));
    playPattern->setCursor(QCursor(Qt::PointingHandCursor));


    BtnPlay=new QPushButton(this);
    BtnPlay->setObjectName(tr("BtnPlay"));
    BtnPlay->setGeometry(160,250,56,56);
    BtnPlay->setToolTip(tr("播放"));
    BtnPlay->setCursor(QCursor(Qt::PointingHandCursor));

    BtnPrev=new QPushButton(this);
    BtnPrev->setObjectName(tr("BtnPrev"));
    BtnPrev->setGeometry(120,255,45,45);
    BtnPrev->setToolTip(tr("上一首"));
    BtnPrev->setCursor(QCursor(Qt::PointingHandCursor));

    BtnNext=new QPushButton(this);
    BtnNext->setObjectName(tr("BtnNext"));
    BtnNext->setGeometry(210,255,45,45);
    BtnNext->setToolTip(tr("下一首"));
    BtnNext->setCursor(QCursor(Qt::PointingHandCursor));

    muteButton=new QPushButton(this);
    muteButton->setObjectName(tr("muteButton"));
    muteButton->setGeometry(370,320,25,25);
    muteButton->setToolTip(tr("关闭声音"));
    muteButton->setCursor(QCursor(Qt::PointingHandCursor));


    volumeControl=new QSlider(Qt::Vertical,this);//QSlider(Qt::Horizontal,this)创建一个水平方向的滑动条QSlider控件
    volumeControl->setObjectName(tr("volumeControl"));
    volumeControl->setGeometry(375,240,15,80);
    volumeControl->setCursor(QCursor(Qt::PointingHandCursor));
    volumeControl->setRange(0,100);//设置滑动条控件的最小值和最大值
    volumeControl ->setValue(50);//设置初值为50；

    seekSlider = new QSlider(Qt::Horizontal,this);
    seekSlider -> setGeometry(100,345,200,15);
    seekSlider->setObjectName(tr("seekSlider"));
    seekSlider -> setCursor(QCursor(Qt::PointingHandCursor));

    addMore = new QPushButton(this);
    addMore -> setGeometry(295,2,30,30);
    addMore -> setObjectName(tr("addMore"));
    addMore->setToolTip(tr("添加歌曲"));
    addMore -> setCursor(QCursor(Qt::PointingHandCursor));

    showMge = new QLabel(this);
    showMge -> setGeometry(34,320,400,20);
    showMge -> setFont(QFont("Times",10,QFont::Bold));//字体使用Times,10号字体，加粗
    QPalette pac;//创建调色板
    pac.setColor(QPalette::WindowText,QColor(70,80,70));
    showMge -> setPalette(pac);

    list = new QListWidget(this);
    list ->setGeometry(20,50,360,180);
    list -> setFont(QFont("Times",10,QFont::Bold));
    list -> setPalette(pac);
    list ->setStyleSheet("background: rgba(0,0,0,0.1);");


    message = new QLabel(this);
    message -> setGeometry(20,30,140,20);
    message -> setFont(QFont("Times",10,QFont::Bold));
    message -> setPalette(pac);

    showPro = new QLabel(this);
    showPro -> setGeometry(35,340,50,20);
    showPro -> setFont(QFont("Times",10,QFont::Bold));
    showPro -> setPalette(pac);
    QTime mov(0,0,0);
    showPro ->setText(mov.toString("mm:ss"));

    showTime = new QLabel(this);
    showTime -> setGeometry(325,340,50,20);
    showTime -> setFont(QFont("Times",10,QFont::Bold));
    showTime -> setPalette(pac);
    QTime mo(0,0,0);//QTime 提供时间函数给用户使用
    showTime ->setText(mo.toString("mm:ss"));//显示分：秒

    title = new QLabel(this);//设置标题
    title -> setGeometry(5,0,200,30);
    title ->setFont(QFont("Times",15,QFont::Bold));
    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0,0,0));
    title -> setPalette(pa);
    title -> setText("MusicPlayer");



    //信号与槽
    connect(BtnClose, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(BtnMin, SIGNAL(clicked(bool)), this, SLOT(showMinimized()));
    connect(addMore, SIGNAL(clicked(bool)), this, SLOT(addMoremusic()));
    connect(BtnPlay, SIGNAL(clicked(bool)), this, SLOT(playMusic()));
    connect (BtnPrev,SIGNAL(clicked(bool)), this, SLOT(preMusic()));
    connect(BtnNext, SIGNAL(clicked(bool)), this, SLOT(nextMusic()));
    connect(muteButton, SIGNAL(clicked(bool)), this, SLOT(meteOpen()));
    connect(volumeControl, SIGNAL(sliderMoved(int)), this, SLOT(volumChange(int)));
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT( positionChange(qint64)));
    connect(playPattern,SIGNAL(clicked(bool)),this,SLOT(musicPlayPattern()));


}

//外部加载qss文件，绘制界面样式
void Music::init_skin()
{
    QFile file(":/qss/skin.qss");
    file.open(QFile::ReadOnly);
    this -> setStyleSheet(QObject::tr(file.readAll()));
    file.close();

}
//播放模式
void Music::musicPlayPattern()
{

        //z=++z%3;
        if(z==0)
        {
            //顺序播放
            playPattern->setStyleSheet("QPushButton:!hover{border-image: url(:/image/Seq.png);}"
                                       "QPushButton:hover{border-image: url(:/image/Seq.png);}"
                                       "QPushButton:pressed{border-image: url(:/image/Seq.png);}"
                                       );
            playPattern->setToolTip(tr("列表循环"));
            playList->setPlaybackMode(QMediaPlaylist::Sequential);
            z = 2;

        }
        else if(z==1)
        {
            //随机播放
            playPattern->setStyleSheet("QPushButton:!hover{border-image: url(:/image/Ran.png);}"
                                       "QPushButton:hover{border-image: url(:/image/Ran.png);}"
                                       "QPushButton:pressed{border-image: url(:/image/Ran.png);}"
                                       );
            playPattern->setToolTip(tr("随机播放"));
            playList->setPlaybackMode(QMediaPlaylist::Random);
            z--;

        }
        else
        {
            //单曲循环
            playPattern->setStyleSheet("QPushButton:!hover{border-image: url(:/image/Single.png);}"
                                       "QPushButton:hover{border-image: url(:/image/Single.png);}"
                                       "QPushButton:pressed{border-image: url(:/image/Single.png);}"
                                       );
            playPattern->setToolTip(tr("单曲循环"));
            playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            z--;
        }
}


void Music::addMoremusic()
{
    QString songFileName,songName;
    QFileInfo info;
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("打开文件"),"",tr("music(*.mp3)"));
    if(!fileNames.isEmpty())
    {
        add = true;
    }
    for(int i = 0; i < fileNames.size(); i ++)
    {
        playList -> addMedia(QUrl::fromLocalFile(fileNames.at(i)));//将音乐文件逐一加入到播放列表
        songFileName = fileNames.at(i);
        info = QFileInfo(songFileName);
        songName = info.fileName();
        list ->addItem(songName);
        message -> setText(tr("添加成功"));
    }

    playList->setCurrentIndex(0);
    if(z == 0)
    {
        playList->setPlaybackMode(QMediaPlaylist::Random);
    }
    else if(z == 1)
    {
        playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else
    {
        playList->setPlaybackMode(QMediaPlaylist::Sequential);
    }
    player -> setPlaylist(playList);
    BtnPlay->setStyleSheet("QPushButton:!hover{border-image: url(:/image/play_hover.png);}"
                           "QPushButton:hover{border-image: url(:/image/play_hover.png);}"
                           "QPushButton:pressed{border-image: url(:/image/play_press.png);}"
                           );


    connect(timer2, SIGNAL(timeout()), this, SLOT(clearMessage()));
    timer2 ->start(2000);
}

void Music::playMusic()
{

    if(!add)
    {
        QMessageBox::information(this,"添加音乐","请先添加音乐","确定");
    }
    else
    {

        if(player -> state() == QMediaPlayer::PlayingState)//判断是否为播放模式
        {

            //设置播放模式为暂停
            player -> pause();
            timer ->stop();
            //改变播放按钮的背景图片
            BtnPlay->setStyleSheet("QPushButton:!hover{border-image: url(:/image/play_hover.png);}"
                                   "QPushButton:hover{border-image: url(:/image/play_hover.png);}"
                                   "QPushButton:pressed{border-image: url(:/image/play_press.png);}"
                                   );

        }
        else
        {
            //否则设置播放模式
            player -> play();
            connect(timer, SIGNAL(timeout()), this, SLOT(posChange()));
            timer->start(1000);

            BtnPlay->setStyleSheet("QPushButton:!hover{border-image: url(:/image/pause_hover.png);}"
                                   "QPushButton:hover{border-image: url(:/image/pause_hover.png);}"
                                   "QPushButton:pressed{border-image: url(:/image/pause_press.png);}"
                                   );

        }
    }




}
void Music::preMusic()
{
    //上一首，并更新播放时间为0
    moved = 0;
    playList ->previous();

}

void Music::nextMusic()
{
    //下一首，更新播放时间为0
    moved = 0;
    playList->next();
}

void Music::meteOpen()
{
    static bool flag=true;//判断是否已点击
    if(flag){

        //如果点下小喇叭，则改变小喇叭按钮的背景图像

        muteButton->setStyleSheet("QPushButton:!hover{border-image: url(:/image/sound_close.png)}"
                                  "QPushButton:hover{border-image: url(:/image/sound_close.png)}"
                                  "QPushButton:pressed{border-image: url(:/image/sound_close.png)}"
                                  "QPushButton:focus{padding:-1;}");
        muteButton->setToolTip(tr("打开声音"));
        player -> setMuted(true);//关闭声音

    }else{
        muteButton->setStyleSheet("QPushButton:!hover{border-image: url(:/image/sound.png)}"
                                  "QPushButton:hover{border-image: url(:/image/sound.png)}"
                                  "QPushButton:pressed{border-image: url(:/image/sound.png)}"
                                  "QPushButton:focus{padding:-1;}");

        muteButton->setToolTip(tr("关闭声音"));
        player -> setMuted(false);//打开声音
    }
    flag=!flag;
}

void Music::volumChange(int vol)
{
    //关联滑块的移动，将值设置播放的音量
    player -> setVolume(vol);
}

void Music::positionChange(qint64 position)
{
    seekSlider->setMaximum(player->duration() / 1000);//设置滑块的长度范围为音乐长.00度
    seekSlider->setValue(position / 1000);//如果音乐进度改变，则改变滑块显示位置
    moved = position;
    QTime moveTime(0,(moved/60000) % 60,(moved / 1000) % 60);
    showPro ->setText(moveTime.toString("mm:ss"));
}

void Music::showMessage(bool ok)
{
    if(ok)
    {
        QString name= player->metaData(QMediaMetaData::Title).toString();
        QString author= player->metaData(QMediaMetaData::Author).toString();
        showMge -> setText("正在播放："+name + " -" + author);

        QTime  displayTime(0,(player -> duration() / 60000) % 60,(player ->duration() / 1000) % 60);
        showTime -> setText(displayTime.toString("mm:ss"));


    }
}

void Music::seekChange(int position)
{
    player -> setPosition(position * 1000);//如果滑块改变位置，则改变音乐进度
    moved = position * 1000;
    QTime moveTime(0,(moved/60000) % 60,(moved / 1000) % 60);
    showPro ->setText(moveTime.toString("mm:ss"));
}

void Music::posChange()
{
    /*moved = moved + 1000;
    QTime moveTime(0,(moved/60000) % 60,(moved / 1000) % 60);
    showPro ->setText(moveTime.toString("mm:ss"));*/

    if(moved >= player -> duration())
    {

        moved = 0;
    }
}

void Music::clearMessage()
{
    message -> setText("");

}
//令窗口可以被拖动
void Music::mousePressEvent(QMouseEvent *event){
    if(event->buttons()==Qt::LeftButton)
    {
        dragPosition=event->globalPos()-frameGeometry().topLeft();
        event->accept();
    }
}

void Music::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)//event->buttons()==Qt::LeftButton && event ->y() < 50 && event ->x() < 330
    {
        this ->move(event->globalPos() - dragPosition);
        event->accept();
    }
}
