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
