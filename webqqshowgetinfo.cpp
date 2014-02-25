#include "webqqshowgetinfo.h"
#include <QtGui/QtGui>

ShowGetInfoDialog::ShowGetInfoDialog(QWidget *parent)
    : QDialog(parent)
{
    m_okButton = new QPushButton();
    m_cancelButton = new QPushButton();
    m_showLabel = new QLabel();
    m_inputverififyEdit = new QLineEdit();
    m_infoEdit = new QTextEdit();
    connect(m_cancleButton, SIGNAL(clicked()), this, SLOT(onCancleButtonClicked()));
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
    m_okButton->setText(tr("close"));
    m_showLabel->setText(tr("Find Results"));
    m_infoEdit->setText(info);
    setWindowTitle(tr("find friend"));
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

