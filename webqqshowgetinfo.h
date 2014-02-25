#ifndef WEBQQSHOWGETINFO_H
#define WEBQQSHOWGETINFO_H
#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;
class QString;
class QTextEdit;

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
    QString okOrCancle(){
        return m_okOrCancle;
    }

private:
    QLineEdit*      m_inputverififyEdit;
    QPushButton*    m_okButton;
    QPushButton* m_cancelButton;
    QTextEdit* m_infoEdit;
    QLabel* m_showLabel;
    QString m_okOrCancle;

private slots:
    void    onOkButtonClicked();
    void    onCancleButtonClicked();
};
#endif // WEBQQSHOWGETINFO_H
