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


