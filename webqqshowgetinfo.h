/*
    webqqshowgetinfo.h - Webqq show info

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
#ifndef WEBQQSHOWGETINFO_H
#define WEBQQSHOWGETINFO_H
#include <QDialog>
#include "type.h"
class QLabel;
class QLineEdit;
class QPushButton;
class QString;
class QTextEdit;
class QRadioButton;
class QGroupBox;
class QVBoxLayout;

class ShowGetInfoDialog : public QDialog
{
    Q_OBJECT

public:
    ShowGetInfoDialog(QWidget *parent = 0);
    virtual ~ShowGetInfoDialog(){};
    QString getVerificationString();
    void setVerifify();
    void setAddInfo(QString info);
    void setUserInfo(QString info);
    void setRequired(QString info);
    QString okOrCancle(){
        return m_okOrCancle;
    }
    LwqqAnswer webqqAnswer();
private:
    QLineEdit*      m_inputverififyEdit;
    QPushButton*    m_okButton;
    QPushButton* m_cancelButton;
    QTextEdit* m_infoEdit;
    QLabel* m_showLabel;
    QString m_okOrCancle;
    QRadioButton *m_refuseButton;
    QRadioButton *m_agreeButton;
    QRadioButton *m_agreeAddButton;
private slots:
    void    onOkButtonClicked();
    void    onCancleButtonClicked();
};
#endif // WEBQQSHOWGETINFO_H
