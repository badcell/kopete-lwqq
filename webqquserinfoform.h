/*
    webqquserinfoform.h - Webqq show user info

    Copyright (c) 2014 by Jun Zhang        <jun.zhang@i-soft.com.cn>

    Kopete    (c) 2002-2006 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/
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
