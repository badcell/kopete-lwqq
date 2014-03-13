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
#include <QLabel>
#include <klocale.h>
#include "webqqcontact.h"

#include "webqquserinfoform.h"

//! [0]
WebqqUserInfoForm::WebqqUserInfoForm(WebqqContact *contact, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    QString photoPath = contact->property(Kopete::Global::Properties::self()->photo()).value().toString();
    ui.label_photo->setPixmap(QPixmap(photoPath)); 
    initLabel();
    
}

void WebqqUserInfoForm::initLabel()
{


}

QString WebqqUserInfoForm::shengxiaoToStr(LwqqShengxiao shengxiao)
{
    switch(shengxiao)
    {
    case LWQQ_MOUTH:
        return i18n("Mouth");
        break;
    case LWQQ_CATTLE:
        return i18n("Cattle");
        break;
    case LWQQ_TIGER:
        return i18n("Tiger");
        break;
    case LWQQ_RABBIT:
        return i18n("Rabbit");
        break;
    case LWQQ_DRAGON:
        return i18n("Dragon");
        break;
    case LWQQ_SNACK:
        return i18n("Snack");
        break;
    case LWQQ_HORSE:
        return i18n("Horse");
        break;
    case LWQQ_SHEEP:
        return i18n("Sheep");
        break;
    case LWQQ_MONKEY:
        return i18n("Monkey");
        break;
    case LWQQ_CHOOK:
        return i18n("Chook");
        break;
    case LWQQ_DOG:
        return i18n("Dog");
        break;
    case LWQQ_PIG:
        return i18n("Pig");
        break;
    }
    return QString("");
}

QString WebqqUserInfoForm::constellationToStr(LwqqConstel constel)
{
    switch(constel)
    {
    case LWQQ_AQUARIUS:
        return i18n("Aquarius");
        break;
    case LWQQ_PISCES:
        return i18n("Pisces");
        break;
    case LWQQ_ARIES:
        return i18n("Aries");
        break;
    case LWQQ_TAURUS:
        return i18n("Taurus");
        break;
    case LWQQ_GEMINI:
        return i18n("Gemini");
        break;
    case LWQQ_CANCER:
        return i18n("Cancer");
        break;
    case LWQQ_LEO:
        return i18n("Leo");
        break;
    case LWQQ_VIRGO:
        return i18n("Virgo");
        break;
    case LWQQ_LIBRA:
        return i18n("Libra");
        break;
    case LWQQ_SCORPIO:
        return i18n("Scorpio");
        break;
    case LWQQ_SAGITTARIUS:
        return i18n("Sagittarius");
        break;
    case LWQQ_CAPRICORNUS:
        return i18n("Capricornus");
        break;
    }
    return QString("");
}

void WebqqUserInfoForm::setInfo(LwqqBuddy *buddy)
{
    ui.label_nick->setText(QString::fromUtf8(buddy->nick) + "<" + QString(buddy->qqnumber) + ">");
    ui.label_longnick->setText(QString::fromUtf8(buddy->long_nick));
    ui.label_level->setText(QString::fromUtf8(qq_level_to_str(buddy->level)));
    if(buddy->gender == LWQQ_FEMALE)
        ui.label_gender->setText(i18n("Female"));
    else
       ui.label_gender->setText(i18n("Male"));
    ui.label_shengxiao->setText(shengxiaoToStr(buddy->shengxiao));
    ui.label_constel->setText(constellationToStr(buddy->constel));
    ui.label_blood->setText(QString(qq_blood_to_str(buddy->blood)));
    ui.label_birthday->setText(QString(lwqq_date_to_str(buddy->birthday)));
    ui.label_country->setText(QString::fromUtf8(buddy->country));
    ui.label_city->setText(QString::fromUtf8(buddy->city));
    ui.label_province->setText(QString::fromUtf8(buddy->province));
    ui.label_phone->setText(QString::fromUtf8(buddy->phone));
    ui.label_mobile->setText(QString::fromUtf8(buddy->mobile));
    ui.label_email->setText(QString::fromUtf8(buddy->email));
    ui.label_job->setText(QString::fromUtf8(buddy->occupation));
    ui.label_school->setText(QString::fromUtf8(buddy->college));
    ui.label_page->setText(QString::fromUtf8(buddy->homepage));


}


