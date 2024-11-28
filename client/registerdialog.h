#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H
#include "global.h"
#include <QDialog>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_get_btn_clicked();
    void showTip(QString s,bool isTrue);

private:
    Ui::RegisterDialog *ui;
    QMap<ReqId,std::function<void(const QJsonObject& jsonObj)>> _handlers;
private:
    void initHttpHandlers();
private slots:
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // REGISTERDIALOG_H
