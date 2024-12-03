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
    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_btn_clicked()
{
    QString email = ui->email_Edit->text();
    //QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    QRegularExpression regex(R"((\w+)@(\w+)\.([a-zA-Z]{2,}))");
    bool match = regex.match(email).hasMatch();
    if(match){
        //发送http验证码；
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);

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
    //注册注册用户回包逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug()<< "email is " << email ;
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


void RegisterDialog::on_sure_btn_clicked()
{
    if(ui->user_lineEdit->text() == ""){
        showTip(tr("用户名不能为空"), false);
        return;
    }

    if(ui->email_Edit->text() == ""){
        showTip(tr("邮箱不能为空"), false);
        return;
    }

    if(ui->pass_Edit->text() == ""){
        showTip(tr("密码不能为空"), false);
        return;
    }

    if(ui->pass2_lineEdit->text() == ""){
        showTip(tr("确认密码不能为空"), false);
        return;
    }

    if(ui->pass2_lineEdit->text() != ui->pass_Edit->text()){
        showTip(tr("密码和确认密码不匹配"), false);
        return;
    }

    if(ui->varify_Edit->text() == ""){
        showTip(tr("验证码不能为空"), false);
        return;
    }

    //day11 发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->user_lineEdit->text();
    json_obj["email"] = ui->email_Edit->text();
    json_obj["passwd"] = ui->pass_Edit->text();
    json_obj["confirm"] = ui->pass2_lineEdit->text();
    json_obj["varifycode"] = ui->varify_Edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
}

