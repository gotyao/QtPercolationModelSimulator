#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <Windows.h>
#include <bits/stdc++.h>
#include <QMessageBox>
#include <QTextBrowser>
#include <QPaintDevice>
#include <QPainter>
#include <bits/random.h>
#include <bits/opt_random.h>
#include <bits/random.tcc>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    int width;
    int height;//网格大小
    int *a;//格子状态，0表示未开放，1表示开放但不连通，2表示连通
    std::vector<int> opensite;//打通格子编号
    void draw(QLabel *label);//绘图
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

signals:
    void finishcal(int i, int opensite);//循环结束信号
private:
    Ui::Widget *ui;
};

class UnionFind{
public:
    int *a;
    int n;
    int *sz;
    UnionFind(int v){
        n = v;
        a = new int[n];
        sz = new int[n];
        for(int i = 0; i < n; i++){
            a[i] = i;
            sz[i] = 0;
        }
    }
    ~UnionFind(){
        delete a;
        delete sz;
    }

    int find(int pos){
        int save = pos;
        while(pos != a[pos]) pos = a[pos];
        a[save] = pos;
        return pos;
    }

    void link(int pos1, int pos2){
        int fpos1 = find(pos1);
        int fpos2 = find(pos2);
        if(fpos1 == fpos2) return ;

        if(sz[fpos1] <= sz[fpos2]){
            a[fpos1] = fpos2;
            if(sz[fpos1] == sz[fpos2]) sz[fpos2]++;
        }
        else{
            a[fpos2] = fpos1;
        }
    }

    bool islinked(int pos1, int pos2){
        pos1 = find(pos1);
        pos2 = find(pos2);
        if(pos1 == pos2) return true;
        else return false;
    }
};

int solverand(int max);
#endif // WIDGET_H
