#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QCheckBox>
#include "realtime.h"
#include "utils/aspectratiowidget/aspectratiowidget.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {}
    void initialize();
    void finish();

private:
    Realtime *realtime;
    AspectRatioWidget *aspectRatioWidget;
    QPushButton *uploadFile;
    QCheckBox *toggle1;
    QCheckBox *toggle2;
    QCheckBox *toggle3;

private slots:
    void onUploadFile();
    void onToggle1();
    void onToggle2();
    void onToggle3();
};
