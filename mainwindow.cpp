#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&_backend, &Backend::done, this, &MainWindow::onDone);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_runbutton_clicked()
{
    ui->runbutton->setDisabled(true);
    _backend.launchPlayer();
}

void MainWindow::onDone()
{
    ui->runbutton->setDisabled(false);
}
