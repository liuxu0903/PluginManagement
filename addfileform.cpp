#include "addfileform.h"
#include "ui_addfileform.h"
#include "mainwindow.h"
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

QString filePath1;
addfileform::addfileform(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addfileform)
{
    ui->setupUi(this);

}

addfileform::~addfileform()
{
    delete ui;
}

void addfileform::writeJson(const QString filePath, ST_STRUCT st)
{
    QJsonObject obj;
    obj.insert("Name", st.name);
    obj.insert("Type", st.type);
    obj.insert("Path", st.path);
    obj.insert("Config", st.config);

    //打开文件
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "can't open error!";
        return;
    }

    // 读取文件的全部内容
    QTextStream stream(&file);
    stream.setCodec("UTF-8");		// 设置读取编码是UTF8
    QString str = stream.readAll();
    file.close();
    // QJsonParseError类用于在JSON解析期间报告错误。
    QJsonParseError jsonError;
    // 将json解析为UTF-8编码的json文档，并从中创建一个QJsonDocument。
    // 如果解析成功，返回QJsonDocument对象，否则返回null
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    // 判断是否解析失败
    if (jsonError.error != QJsonParseError::NoError && !jsonDoc.isNull()) {
        qDebug() << "Json格式错误！" << jsonError.error;
        return;
    }

    QJsonObject rootObj=jsonDoc.object();

    QJsonValue ListValue = rootObj.value("List");
    if (ListValue.type() == QJsonValue::Array) {
        // 转换成数组类型
        QJsonArray ListArray = ListValue.toArray();
        ListArray.append(obj);
        rootObj["List"] = ListArray;
    }


    // 将object设置为本文档的主要对象
    jsonDoc.setObject(rootObj);

    // 重写打开文件，覆盖原有文件，达到删除文件全部内容的效果
    QFile writeFile(filePath);
    if (!writeFile.open(QFile::WriteOnly | QFile::Truncate)) {
        qDebug() << "can't open error!";
        return;
    }

    // 将修改后的内容写入文件
    QTextStream wirteStream(&writeFile);
    wirteStream.setCodec("UTF-8");		// 设置读取编码是UTF8
    wirteStream << jsonDoc.toJson();		// 写入文件
    writeFile.close();					// 关闭文件
}

void addfileform::closeEvent(QCloseEvent *)
{
    emit ExitWin();
}

//传值
void addfileform::getFilePath(QString filePath)
{
    filePath1=filePath;
}

//选择程序按钮
void addfileform::on_pb_choosepro_clicked()
{
    ST_STRUCT st;
    st.type = ui->cb_type->currentText().trimmed();
    if(st.type==".exe"){
        QString fileName = QFileDialog::getOpenFileName(this,"选择应用程序","./","Txt files(*.exe )");
        ui->le_address->setText(fileName);
    }
    else if(st.type==".py"){
        QString fileName = QFileDialog::getOpenFileName(this,"选择应用程序","./","Txt files(*.py )");
        ui->le_address->setText(fileName);
    }
    else if(st.type==".m"){
        QString fileName = QFileDialog::getOpenFileName(this,"选择应用程序","./","Txt files(*.m )");
        ui->le_address->setText(fileName);
    }
    else{
        QString fileName = QFileDialog::getOpenFileName(this,"选择应用程序","./","Txt files(*.exe *.py *.m)");
        ui->le_address->setText(fileName);
    }
}
//确认按钮
void addfileform::on_pb_confirm_clicked()
{
    ST_STRUCT st;
    st.name = ui->le_name->text().trimmed();
    st.type = ui->cb_type->currentText().trimmed();
    st.path = ui->le_address->text().trimmed();
    st.config=ui->textEdit->toPlainText();

    writeJson(filePath1,st);

//    QMessageBox::warning(this,tr("提示"),tr("保存文件成功"));

    close();
}

//取消按钮
void addfileform::on_pb_close_clicked()
{
    close();
}
