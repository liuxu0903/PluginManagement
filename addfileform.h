#ifndef ADDFILEFORM_H
#define ADDFILEFORM_H
#include "mainwindow.h"
#include "Struct.h"
#include <QDialog>
#include <QWidget>
#include <QMainWindow>
#include <QFileDialog>
#include <QMap>


namespace Ui {
class addfileform;
}

class addfileform : public QDialog
{
    Q_OBJECT

public:
    explicit addfileform(QWidget *parent = nullptr);
    ~addfileform();
    void writeJson(const QString filePath,ST_STRUCT st);

    void closeEvent(QCloseEvent *);


private slots:
    void on_pb_choosepro_clicked();

    void on_pb_confirm_clicked();

    void getFilePath(QString filePath);

    void on_pb_close_clicked();

signals:
    void ExitWin();

private:
    Ui::addfileform *ui;
    QMap<QString, QVariant> configData;

};

#endif // ADDFILEFORM_H

