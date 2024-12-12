#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <iostream>

void MainWindow::initialize() {
    // Create the rendering widget
    realtime = new Realtime;
    aspectRatioWidget = new AspectRatioWidget(this);
    aspectRatioWidget->setAspectWidget(realtime, 3.f/4.f);

    // Create main layout
    QHBoxLayout *hLayout = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(aspectRatioWidget, 1);
    this->setLayout(hLayout);

    // Create "Start" button
    uploadFile = new QPushButton("Start", this);
    vLayout->addWidget(uploadFile);
    connect(uploadFile, &QPushButton::clicked, this, &MainWindow::onUploadFile);

    // Create two toggle boxes
    toggle1 = new QCheckBox("Edge", this);
    toggle2 = new QCheckBox("FXAA", this);
    toggle3 = new QCheckBox("Shadow", this);
    toggle4 = new QCheckBox("Developer Mode", this);

    // Add them to the layout
    vLayout->addWidget(toggle1);
    vLayout->addWidget(toggle2);
    vLayout->addWidget(toggle3);
    vLayout->addWidget(toggle4);
    // Connect their signals to slots
    connect(toggle1, &QCheckBox::clicked, this, &MainWindow::onToggle1);
    connect(toggle2, &QCheckBox::clicked, this, &MainWindow::onToggle2);
    connect(toggle3, &QCheckBox::clicked, this, &MainWindow::onToggle3);
    connect(toggle4, &QCheckBox::clicked, this, &MainWindow::onToggle4);

}

void MainWindow::finish() {
    realtime->finish();
    delete realtime;
}

void MainWindow::onUploadFile() {
    // Set the scene file path directly
    settings.sceneFilePath = "./asset/root.json";

    // Notify realtime that the scene has changed
    realtime->sceneChanged();
}

void MainWindow::onToggle1() {
    // Invert or set the setting based on toggle state
    settings.toggle1 = toggle1->isChecked();
    realtime->settingsChanged();
    std::cout << "Cool Edge: " << (settings.toggle1 ? "ON" : "OFF") << std::endl;
}

void MainWindow::onToggle2() {
    settings.toggle2 = toggle2->isChecked();
    realtime->settingsChanged();
    std::cout << "FXAA: " << (settings.toggle2 ? "ON" : "OFF") << std::endl;
}

void MainWindow::onToggle3() {
    settings.toggle3 = toggle3->isChecked();
    realtime->settingsChanged();
    std::cout << "Shadow: " << (settings.toggle3 ? "ON" : "OFF") << std::endl;
}

void MainWindow::onToggle4() {
    // Invert or set the setting based on toggle state
    settings.toggle4 = toggle4->isChecked();
    realtime->settingsChanged();
    std::cout << "Developer Mode on: " << (settings.toggle4 ? "ON" : "OFF") << std::endl;
}
