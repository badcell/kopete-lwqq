#include "webqqshowgetinfo.h"
#include <QtGui/QtGui>
#include <klocale.h>
ShowGetInfoDialog::ShowGetInfoDialog(QWidget *parent)
    : QDialog(parent)
{
    m_okButton = new QPushButton();
    m_cancelButton = new QPushButton();
    m_showLabel = new QLabel();
    m_inputverififyEdit = new QLineEdit();
    m_infoEdit = new QTextEdit();
    m_refuseButton = new QRadioButton(i18n("Refuse"));
    m_agreeButton = new QRadioButton(i18n("Agree"));
    m_agreeAddButton = new QRadioButton(i18n("Agree and add back"));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onCancleButtonClicked()));
    connect(m_okButton, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
}

void ShowGetInfoDialog::setVerifify()
{
    m_showLabel->setText(tr("Verification info"));
    m_okButton->setText(tr("OK"));
    m_cancelButton->setText(tr("Cancel"));
    setWindowTitle(tr("add friend"));
    QGridLayout* pGridLayout = new QGridLayout();
    pGridLayout->addWidget(m_showLabel, 1, 1, 1, 2);
    pGridLayout->addWidget(m_inputverififyEdit, 1, 4, 1, 3);
    pGridLayout->addWidget(m_cancelButton, 2, 2, 1, 2);
    pGridLayout->addWidget(m_okButton, 2, 5, 1, 2);
    setLayout(pGridLayout);
    resize(400, 200);
}

QString ShowGetInfoDialog::getVerificationString()
{
    return m_inputverififyEdit->text();
}

void ShowGetInfoDialog::setAddInfo(QString info)
{
    m_okButton->setText(i18n("close"));
    m_showLabel->setText(i18n("Find Results"));
    m_infoEdit->setText(info);
    setWindowTitle(i18n("find friend"));
    QGridLayout* pGridLayout = new QGridLayout();
    pGridLayout->addWidget(m_showLabel, 1, 1, 1, 2);
    pGridLayout->addWidget(m_infoEdit, 2, 1, 4, 3);
    pGridLayout->addWidget(m_okButton, 6, 2, 1, 2);
    setLayout(pGridLayout);
    resize(300, 400);
}

void ShowGetInfoDialog::setUserInfo(QString info)
{

}

void ShowGetInfoDialog::setRequired(QString info)
{
    m_okButton->setText(i18n("OK"));
    m_cancelButton->setText(i18n("Ignore"));
    QGroupBox *groupBox = new QGroupBox(i18n("Please select"));
    m_refuseButton->setChecked(true);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(m_refuseButton);
    vbox->addWidget(m_agreeButton);
    vbox->addWidget(m_agreeAddButton);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);
    QGridLayout* pGridLayout = new QGridLayout();
    if(info.isEmpty())
    {
        setWindowTitle(i18n("Block Message"));
        m_refuseButton->setText(i18n("No Block"));
        m_agreeButton->setText(i18n("Slience Receive"));
        m_agreeAddButton->setText(i18n("Block"));
        pGridLayout->addWidget(groupBox, 1, 1, 3, 3);
        pGridLayout->addWidget(m_cancelButton, 9, 1, 1,1);
        pGridLayout->addWidget(m_okButton, 9, 2, 1, 1);
    }else
    {
        setWindowTitle(i18n("Friends confirm"));
        m_showLabel->setText(i18n("Friends confirm"));
        QLabel *refuseLabel = new QLabel(i18n("Grounds for refusal"));
        m_infoEdit->setText(info);
        pGridLayout->addWidget(m_showLabel, 1, 1, 1, 2);
        pGridLayout->addWidget(m_infoEdit, 2, 1, 4, 3);
        pGridLayout->addWidget(groupBox, 6, 1, 3, 3);
        pGridLayout->addWidget(refuseLabel, 9, 1, 1, 1);
        pGridLayout->addWidget(m_inputverififyEdit, 9, 2, 1, 2);
        pGridLayout->addWidget(m_cancelButton, 10, 1, 1, 1);
        pGridLayout->addWidget(m_okButton, 10, 2, 1, 1);
    }
    setLayout(pGridLayout);
    resize(300, 400);
}

LwqqAnswer ShowGetInfoDialog::webqqAnswer()
{
    if(m_refuseButton->isChecked())
        return LWQQ_NO;
    else if(m_agreeButton->isChecked())
        return LWQQ_YES;
    else if(m_agreeAddButton->isChecked())
        return LWQQ_EXTRA_ANSWER;
}

void ShowGetInfoDialog::onCancleButtonClicked()
{
    this->hide();
    m_okOrCancle ="Cancle";
}

void ShowGetInfoDialog::onOkButtonClicked()
{
    this->hide();
    m_okOrCancle = "OK";
}

#include "webqqshowgetinfo.moc"
