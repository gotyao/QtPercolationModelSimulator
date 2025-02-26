#include "widget.h"
#include "ui_widget.h"

using namespace std;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    width = height = 0;
    a = nullptr;

    opensite.clear();
    ui->label->installEventFilter(this);
    connect(this, finishcal, [=](int i, int opensite){//结束时重绘
        ui->textBrowser->append("第" + QString::number(i+1) + "次打通格子个数：" + QString::number(opensite));
        repaint();
    });
}

Widget::~Widget()
{
    delete a;
    delete ui;
}

/*****************绘图事件**************************/
void Widget::draw(QLabel *label)
{
    QPainter painter(label);

    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,600,600);

    if(opensite.empty()) return ;
    else{
        int w, h;
        w = 600/width;
        h = 600/height;
        painter.setBrush(Qt::cyan);
        for(auto e: opensite){
            if(a[e] == 1){
                QPainter painter(label);
                painter.setBrush(Qt::white);
                painter.drawRect((e%width)*w, (e/width)*h, w, h);
            }
            else{
                QPainter painter(label);
                painter.setBrush(Qt::cyan);
                painter.drawRect((e%width)*w, (e/width)*h, w, h);
            }
        }
    }
}

/***************生成网格********************/
void Widget::on_pushButton_clicked()
{
    width = (ui->lineEdit->text()).toInt();
    height = (ui->lineEdit_2->text()).toInt();
    ui->textBrowser->clear();
    a = new int[(width*height)];
    memset(a,0,(width*height*sizeof(int)));
    QMessageBox::information(this,"提示","已生成网格");
    draw(ui->label);
}

/***************开始模拟********************/
void Widget::on_pushButton_2_clicked()
{
    if(height == width && height == 0) {
        QMessageBox::warning(this,"警告","未生成网格!");
        return ;
    }
    ui->textBrowser->clear();
    int times = (ui->lineEdit_3->text()).toInt();
    double possibility[times];

    for(int i = 0; i < times; i++){//进行第i次模拟
        memset(a,0,(width*height*sizeof(int)));
        srand((unsigned)time(NULL));
        UnionFind u(width*height);
        int size = width*height;
        opensite.clear();

        bool fullopen = false;

        while(!fullopen){
            //open a new site named pos;
            int pos;
            bool find;

            do{
                find = false;
                pos = solverand(size);
                if(opensite.empty()) break;
                for(auto e:opensite){
                    if(e == pos){
                        find = true;
                        break;
                    }
                }

            }while(find);//排除已开放的格子
            a[pos] = 1;
            opensite.emplace_back(pos);

            qDebug() << pos;
            //link
            if(pos < width){
                if(pos && a[pos-1]){//left
                    u.link(pos, pos-1);
                }
                if(pos != width-1 && a[pos+1]){//right
                    u.link(pos, pos+1);
                }
                if(a[pos+width]){//down
                    u.link(pos, pos+width);
                }
            }
            else if(pos >= width*(height-1)){
                if(a[pos-width]){//up
                    u.link(pos, pos-width);
                }
                if((pos%width) && a[pos-1]){//left
                    u.link(pos, pos-1);
                }
                if(((pos+1)%width) && a[pos+1]){//right
                    u.link(pos, pos+1);
                }
            }
            else{
                if(a[pos-width]){//up
                    u.link(pos, pos-width);
                }
                if((pos%width) && a[pos-1]){//left
                    u.link(pos, pos-1);
                }
                if(((pos+1)%width) && a[pos+1]){//right
                    u.link(pos, pos+1);
                }
                if(a[pos+width]){//down
                    u.link(pos, pos+width);
                }
            }

            bool check = false;
            for(int j = width*(height-1); j < width*height; j++){//判断是否有必要更新网格
                if(a[j] == 1 && u.find(j) == u.find(pos)){
                    check = true;
                    break;
                }
            }
            if(check){
                sort(opensite.begin(), opensite.end());
                for(auto e:opensite){
                    if(e < width) a[u.find(e)] = 2;
                    if(a[u.find(e)] == 2) a[e] = 2;
                }

                for(int j = width*(height-1); j < width*height; j++){
                    if(a[j] == 2){
                        fullopen = true;
                        break;
                    }
                }
            }
        }

        possibility[i] = opensite.size()/(double)(width*height);
        qDebug()  << possibility[i];
        emit finishcal(i, opensite.size());
        Sleep(1000);//已经产生卡顿时才不产生暂停
    }

    //average
    double average = 0;
    for(int i = 0; i < times; i++){
        average += possibility[i];
    }
    average /= times;
    //ui->textBrowser->clear();
    ui->textBrowser->append("概率阈值" +QString::number(average));

    //方便下次运算
    opensite.clear();
    memset(a,0,(width*height*sizeof(int)));
}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->label){
        draw(ui->label);
        return true;
    }
    else return QWidget::eventFilter(obj, event);
}

/*******************蒙特卡洛优化****************************/
int solverand(int max)//0 ~ max-1
{
    if(max <= RAND_MAX){
        int m = (RAND_MAX / max)*max;//截断得到能整除max的均匀区间
        int re = rand();
        while(re >= m){//抛弃区间外的数
            re = rand();
        }
        return re%max;
    }
    else{
        int a, b;

        do{//5可以替换成RAND_MAX
            a = solverand(max/5);
            b = solverand(5);
        }while(a*5+b >= max);

        return a*5+b;
    }

}
