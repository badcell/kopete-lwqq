#ifndef WEBQQ_USER_INFO_FORM_H
#define WEBQQ_USER_INFO_FORM_H

#include "webqqcontact.h"

#include "ui_webqquserinfoform.h"

class WebqqUserInfoForm : public QWidget
{
    Q_OBJECT

public:
    WebqqUserInfoForm(WebqqContact *contact, QWidget *parent = 0);

private slots:


private:
    Ui::WebqqUserInfoForm ui;
};




#endif
