#ifndef LINKBUTTON_H
#define LINKBUTTON_H

#include<QPushButton>

class LinkButton: public QPushButton
{
    Q_OBJECT

public:
    LinkButton(int i,QString filePath);
    virtual ~LinkButton();

signals:
    void getPointer(int, QString);

public slots:
    void sendPointer();

private:
    int i1;
    QString filePath1;
};


#endif // LINKBUTTON_H
