#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "modelparser.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loadFile_clicked();

    void on_runButton_clicked();

private:
    Ui::MainWindow *ui;
    QString modelPath_;          // << tu trzymamy ścieżkę
    ParsedFile parsed_;          // opcjonalnie: trzymamy sparsowany model
};
#endif // MAINWINDOW_H
