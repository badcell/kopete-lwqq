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
