#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include <QRegularExpression>
#include <QDebug>
#include "httpmgr.h"
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    ui->err_tip->setProperty("state","normal");
    repolish(ui->err_tip);
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_btn_clicked()
{
    QString email = ui->emil_Edit->text();
    //QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    QRegularExpression regex(R"((\\w+)@(\\w+)\\.([a-zA-Z]{2,}))");
    bool match = regex.match(email).hasMatch();
    if(match){
        //发送http验证码；

    }else{
        showTip(tr("验证码不正确"),false);
    }

}

void RegisterDialog::showTip(QString s,bool isTrue)
{
    ui->err_tip->setText(s);
    if(isTrue){
        ui->err_tip->setProperty("state","normal");
    }else{
        ui->err_tip->setProperty("state","err");
    }
    repolish(ui->err_tip);

}

void RegisterDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE,[this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS){
            showTip("参数错误",false);
            return;
        }

        auto email = jsonObj["email"].toString();
        showTip("验证码已发送到邮箱，请注意查收",true);
        qDebug()<< "email is"<<email<<Qt::endl;
        return;

    });
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip("网络请求错误",false);
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()){
        showTip("网络请求错误",false);
        return;
    }

    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }



    _handlers[id](jsonDoc.object());
    return;

}

