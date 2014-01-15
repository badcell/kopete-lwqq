
#include "webqqaccount.h"

#include <QtGui/QtGui>  

#include "webqqloginverifywidget.h"

LoginVerifyDialog::LoginVerifyDialog(QWidget *parent)  
    : QDialog(parent)  
{  
  
    m_inputCodeEdit = new QLineEdit(this);  
    m_okButton = new QPushButton(tr("OK"));
    
    m_picLabel = new QLabel("");
    m_infoLabel = new QLabel(tr("please input verify code"));
    
    QGridLayout* pGridLayout = new QGridLayout();  
    pGridLayout->addWidget(m_picLabel,1,1,2,2);  
    pGridLayout->addWidget(m_infoLabel,3,1,1,2);  
    pGridLayout->addWidget(m_inputCodeEdit,4,0,1,2);  
    pGridLayout->addWidget(m_okButton,4,3,1,1);   
    
    setLayout(pGridLayout);  
   
    setWindowTitle(tr("Verify code"));  
    resize(400,300);  
    connect(m_okButton, SIGNAL(clicked()), this, SLOT(onButtonClicked()));    
}  

void LoginVerifyDialog::onButtonClicked()
{
  this->hide();
}
void LoginVerifyDialog::setImage(QString fullPath)
{
    m_picLabel->setPixmap(QPixmap(fullPath)); 
}

QString LoginVerifyDialog::getVerifyString()
{
  return m_inputCodeEdit->text();
}