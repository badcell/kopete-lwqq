#ifndef WEBQQ_BRIDGE_CALLBACK_H
#define WEBQQ_BRIDGE_CALLBACK_H

#include <QVariant>

extern "C"
{
#include "lwqq.h"
}

typedef enum  {
  NEED_VERIFY2 = 0,/*call ac_need_verify2(LwqqClient* lc,LwqqVerifyCode* code)*/
  LOGIN_COMPLETE,  /*call ac_login_stage_1(LwqqClient* lc,LwqqErrorCode err)*/
  LOGIN_STAGE_2, /*call ac_login_stage_2(LwqqAsyncEvent* event,LwqqClient* lc)*/
  LOGIN_STAGE_3, /*call ac_login_stage_3(LwqqClient* lc)*/
  LOGIN_STAGE_f, /*call ac_login_stage_f(LwqqClient* lc)*/
  FRIEND_AVATAR, /*call ac_friend_avatar(LwqqClient* lc, LwqqBuddy *buddy)*/
  GROUP_AVATAR,/*call ac_group_avatar(LwqqClient* lc, LwqqGroup *group)*/
  QQ_MSG_CHECK,/*call ac_qq_msg_check(LwqqClient* lc)*/
  GROUP_MEMBERS,/*call ac_group_members(LwqqClient* lc, LwqqGroup *group)*/
  
}CallbackFunctionType;


typedef  struct CallbackObject {
  
  void *ptr1;
  void *ptr2;
  void *ptr3;
  LwqqErrorCode err;
  CallbackFunctionType fun_t;
}CallbackObject;


class ObjectInstance : public QObject
{
    Q_OBJECT
public:
    static ObjectInstance *instance(); //单件接口
    void callSignal(CallbackObject); //在回调里这样调用ObjectInstance::instance()->callSignal();
signals:
    void signal_from_instance(CallbackObject);
private:
    ObjectInstance();
    static ObjectInstance *m_instance;
};



#endif
