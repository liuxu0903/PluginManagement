#ifndef STRUCT_H
#define STRUCT_H
#include <QMetaType>
#include <QString>
//自定义参数类型的头文件----***
typedef struct ST_STRUCT
{
    QString  name;
    QString  type;
    QString  path;
    QString  config;
} ST_STRUCT;

Q_DECLARE_METATYPE(ST_STRUCT)             //要调用Q_DECLARE_METATYPE，向QT声明这个结构体----***


#endif // STRUCT_H
