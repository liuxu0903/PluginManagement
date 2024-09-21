#ifndef EDITFILEFORM_H
#define EDITFILEFORM_H
#include "Struct.h"
#include <QDialog>
#include <QWidget>
#include <QMainWindow>
#include <QFileDialog>
#include <QMap>


namespace Ui {
class editfileform;
}

class editfileform : public QDialog
{
    Q_OBJECT

public:
    explicit editfileform(QWidget *parent = nullptr);
    ~editfileform();
    void writeJson(const QString filePath,ST_STRUCT st ,int i);
    void closeEvent(QCloseEvent *);

private slots:
    void on_pb_choosepro_clicked();

    void on_pb_confirm_clicked();

    void getData(QString filePath,ST_STRUCT st,int i);

    void on_pb_close_clicked();


signals:
    void ExitWin();

private:
    Ui::editfileform *ui;
};

#endif // EDITFILEFORM_H
