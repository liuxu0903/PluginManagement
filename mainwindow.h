#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "Struct.h"
#include <QMainWindow>
#include <QProcess>
#include <QTableWidgetItem>
#include <QPushButton>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void writeJson(const QString filePath,ST_STRUCT st);


signals:
    void sendFilePath(QString filePath);

    void sendData(QString filePath,ST_STRUCT st,int i);

    void sendData(QString filePath,int i);


private slots:

    void delete_clicked(int i,QString filePath);

    void edit_clicked(int i,QString filePath);

    void start_clicked(int i,QString filePath);

    void on_Add_clicked();

    void on_Select_clicked();
    void on_cplus_clicked();
    void on_python_clicked();
    void on_exe_clicked();
    void on_mlb_clicked();
    void on_table_clicked();

private:
    Ui::MainWindow *ui;
    QProcess* m_Process;


    void SetBtnStyle(QPushButton *pBtn, QString clr, QString btnTxt);
    void SetBtnStyle_2(QPushButton *pBtn, QString clr, QString btnTxt);
    void SetBtnStyle_3(QPushButton *pBtn, QString clr, QString btnTxt);

};
#endif // MAINWINDOW_H
