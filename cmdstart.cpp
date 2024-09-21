#include "cmdstart.h"
#include "ui_cmdstart.h"
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QTextCodec>


QString FilePath;
ST_STRUCT ST;
int I;
cmdstart::cmdstart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cmdstart),
    m_Process{nullptr}
{
    ui->setupUi(this);
}

cmdstart::~cmdstart()
{
    delete ui;
}
void cmdstart::closeEvent(QCloseEvent *)
{
    emit ExitWin();
}

//传值
void cmdstart::getData(QString filePath,int i){

    FilePath=filePath;
    I=i;

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
    QJsonArray ListArray = ListValue.toArray();
    QJsonValue ListValueChild = ListArray.at(i);
    QJsonObject ListObj = ListValueChild.toObject();
    ST_STRUCT st;
    st.name = ListObj.value("Name").toString();
    st.path = ListObj.value("Path").toString();
    st.type = ListObj.value("Type").toString();
    st.config = ListObj.value("Config").toString();


    ui->intextEdit->setText(st.config);

    if(nullptr != m_Process)
    {
        m_Process->close();
        delete m_Process;
        m_Process = nullptr;
//        QMessageBox::warning(this,tr("提示"),tr("杀死已存在的进程"));
    }

    m_Process = new QProcess(this);
    QString fileName = st.path;
    QString command = "cmd";
    QStringList args;
    args.append("/c");
    args.append(st.config.split("\t"));

   // qDebug() << "Arguments:" << args;

    m_Process->start(command,args);

    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString result = gbkCodec->toUnicode(m_Process->readAllStandardOutput());

    ui->outtextEdit->setText(result);
    if(m_Process->waitForStarted(3000))
    {
//        QMessageBox::warning(this,tr("提示"),tr("进程拉起成功！"));
    }
    else
    {
        QMessageBox::warning(this,tr("提示"),tr("错误: 进程拉起失败！！"));
    }
}

//启动按钮
void cmdstart::on_start_clicked(){

    QString inconfig=ui->intextEdit->toPlainText();
    ui->outtextEdit->setText("");

    if(nullptr != m_Process)
    {
        m_Process->close();
        delete m_Process;
        m_Process = nullptr;
    }

    m_Process = new QProcess(this);
    QString command = "cmd";
    QStringList args;
    args.append("/c");
    args.append(inconfig.split("\t"));
    m_Process->start(command,args);
    if(m_Process->waitForStarted(3000))
    {
        QMessageBox::warning(this,tr("提示"),tr("进程拉起成功！"));
    }
    else
    {
        QMessageBox::warning(this,tr("提示"),tr("错误: 进程拉起失败！！"));
    }
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString result = gbkCodec->toUnicode(m_Process->readAllStandardOutput());
    ui->outtextEdit->setText(result);

}
//保存按钮
void cmdstart::on_confirm_clicked(){

    //打开文件
    QFile file(FilePath);
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

    //修改对象
    QJsonObject rootObj=jsonDoc.object();
    QJsonValue ListValue = rootObj.value("List");
    if (ListValue.type() == QJsonValue::Array)
    {
        // 转换成数组类型
        QJsonArray ListArray = ListValue.toArray();
        //修改第i个obj的Config
        QJsonObject obj;
        obj=ListArray.at(I).toObject();
        obj.insert("Config",ui->intextEdit->toPlainText());
        //移除第i个对象
        ListArray.removeAt(I);
        //插入到第i个对象
        ListArray.insert(I,obj);
        rootObj["List"] = ListArray;
    }

    // 将object设置为本文档的主要对象
    jsonDoc.setObject(rootObj);

    // 重写打开文件，覆盖原有文件，达到删除文件全部内容的效果
    QFile writeFile(FilePath);
    if (!writeFile.open(QFile::WriteOnly | QFile::Truncate)) {
        qDebug() << "can't open error!";
        return;
    }

    // 将修改后的内容写入文件
    QTextStream wirteStream(&writeFile);
    wirteStream.setCodec("UTF-8");		// 设置读取编码是UTF8
    wirteStream << jsonDoc.toJson();	// 写入文件
    writeFile.close();
}
