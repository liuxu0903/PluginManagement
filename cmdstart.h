#ifndef CMDSTART_H
#define CMDSTART_H
#include <QWidget>
#include "Struct.h"
#include <QProcess>

namespace Ui {
class cmdstart;
}

class cmdstart : public QWidget
{
    Q_OBJECT

public:
    explicit cmdstart(QWidget *parent = nullptr);
    ~cmdstart();
    void closeEvent(QCloseEvent *);

private:
    Ui::cmdstart *ui;
    QProcess* m_Process;

private slots:
    void on_start_clicked();

    void on_confirm_clicked();

    void getData(QString filePath,int i);


signals:
    void ExitWin();
};


#endif // CMDSTART_H
