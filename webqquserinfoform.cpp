/*
    webqquserinfoform.cpp - Webqq show user info

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
#include <QtGui>

#include "webqqcontact.h"

#include "webqquserinfoform.h"

//! [0]
WebqqUserInfoForm::WebqqUserInfoForm(WebqqContact *contact, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.label_name->setText("xxxxx");
    QString photoPath = contact->property(Kopete::Global::Properties::self()->photo()).value().toString();
    qDebug()<<"Photo path is:"<<photoPath<<endl;
    ui.label_photo->setPixmap(QPixmap(photoPath)); 
    //
    
}


