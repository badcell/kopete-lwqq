/*
    webqqloginverifywidget.h - Webqq verify widget

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
#ifndef WEBQQ_LOGIN_VERIFY_WIDGET
#define WEBQQ_LOGIN_VERIFY_WIDGET
#include <QDialog>  

class QLabel;
class QLineEdit;  
class QPushButton; 
class QString;

class LoginVerifyDialog : public QDialog  
{  
    Q_OBJECT  
  
public:  
    LoginVerifyDialog(QWidget *parent = 0);  
    virtual ~LoginVerifyDialog(){};  
    QString getVerifyString();
    void setImage(QString fullPath);
  
private:  
    QLineEdit*      m_inputCodeEdit;  
    QPushButton*    m_okButton; 
    QLabel* m_picLabel;
    QLabel* m_infoLabel;
   
private slots:  
    void    onButtonClicked();  
};  



#endif
