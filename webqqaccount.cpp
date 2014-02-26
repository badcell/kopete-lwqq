/*
    webqqaccount.cpp - Kopete Webqq Protocol

    Copyright (c) 2003      by Will Stephenson		 <will@stevello.free-online.co.uk>
    Kopete    (c) 2002-2003 by the Kopete developers <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU General Public                   *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>

#include <kaction.h>
#include <kdebug.h>
#include <klocale.h>
#include <kactionmenu.h>
#include <kmenu.h>
#include <kicon.h>

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <libgen.h>
#include <pthread.h>

// KDE
#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kaction.h>
#include <kactionmenu.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <krun.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <kicon.h>
#include <knotification.h>

#include <QImage>
#include <QBuffer>
#include <QFile>

// Kopete
#include <kopetegroup.h>
//#include "kopetedeletecontacttask.h"
#include <kopetechatsession.h>
#include <kopetemessage.h>
#include <kopetepassword.h>
#include <kopeteuiglobal.h>
#include <knotification.h>
#include <kopetemetacontact.h>
#include <kopetecontactlist.h>
#include <kopetetransfermanager.h>
#include <kopeteview.h>
#include <kopeteaddedinfoevent.h>


#include "webqqcontact.h"
#include "webqqprotocol.h"

#include "webqqloginverifywidget.h"

#include "js.h"
#include "qq_types.h"
#include "translate.h"

#include "webqqaccount.h"
#include "webqqshowgetinfo.h"


#define OPEN_URL(var,url) snprintf(var,sizeof(var),"xdg-open '%s'",url);
#define GLOBAL_HASH_JS() SHARE_DATA_DIR"/hash.js"
#define LOCAL_HASH_JS(buf)  (snprintf(buf,sizeof(buf),"%s/hash.js",\
            lwdb_get_config_dir()),buf)
            

WebqqAccount::WebqqAccount( WebqqProtocol *parent, const QString& accountID )
: Kopete::PasswordedAccount ( parent, accountID, false)
{	
	m_protocol = parent;
	m_lc = NULL;
	// Init the myself contact
	setMyself( new WebqqContact( this, accountId(), accountId(), Kopete::ContactList::self()->myself() ) );
	myself()->setOnlineStatus( m_protocol->WebqqOffline ); 

	/*init lwqq account*/
	//initLwqqAccount();
    fprintf(stderr, "clen alll\n");
    this->cleanAll_contacts();
	m_targetStatus = Kopete::OnlineStatus::Offline;
	
    QString filename = "/home/zj/497717277.png";
    QFile f(filename);
    avatarData.clear();
    if (f.exists() && f.size() > 0 && f.open(QIODevice::ReadOnly))
    {
        qDebug()<<"read";
        avatarData = f.readAll();
        f.close();
    }

	int relink_retry = 0;
	
//	if((relink_retry = purple_account_get_int(account, "relink_retry", 0))>0)
//	    ac->relink_timer = purple_timeout_add_seconds(relink_retry*60, relink_keepalive, ac);
//	lwqq_log_set_level(purple_account_get_int(account,"verbose",0));

	
	qRegisterMetaType<CallbackObject>("CallbackObject");

	QObject::connect( ObjectInstance::instance(), SIGNAL(signal_from_instance(CallbackObject)),
			  this,SLOT(slotReceivedInstanceSignal(CallbackObject)) ); 
	
}

WebqqAccount::~WebqqAccount()
{
	//delete m_server;
}

void WebqqAccount::fillActionMenu( KActionMenu *actionMenu )
{
	Kopete::Account::fillActionMenu( actionMenu );

	actionMenu->addSeparator();

	KAction *action;

	action = new KAction (KIcon("webqq_showvideo"), i18n ("Show my own video..."), actionMenu );
        //, "actionShowVideo");
	QObject::connect( action, SIGNAL(triggered(bool)), this, SLOT(slotShowVideo()) );
	actionMenu->addAction(action);
	action->setEnabled( isConnected() );
}

bool WebqqAccount::createContact(const QString& contactId, Kopete::MetaContact* parentContact)
{
	WebqqContact* newContact = new WebqqContact( this, contactId, parentContact->displayName(), parentContact );
	return newContact != 0L;
}


void WebqqAccount::setAway( bool away, const QString & /* reason */ )
{
	if ( away )
		slotGoAway();
	else
		slotGoOnline();
}

void WebqqAccount::setOnlineStatus(const Kopete::OnlineStatus& status, const Kopete::StatusMessage &reason, const OnlineStatusOptions& /*options*/)
{
	if ( status.status() == Kopete::OnlineStatus::Online &&
			myself()->onlineStatus().status() == Kopete::OnlineStatus::Offline )
	{
	    m_targetStatus = Kopete::OnlineStatus::Online;
	    slotGoOnline();		
	}
	else if (status.status() == Kopete::OnlineStatus::Online &&
			(myself()->onlineStatus().status() == Kopete::OnlineStatus::Away || 
				myself()->onlineStatus().status() == Kopete::OnlineStatus::Busy||
				myself()->onlineStatus().status() == Kopete::OnlineStatus::Invisible) )
	{
	    m_targetStatus = Kopete::OnlineStatus::Online; 
	    setAway( false, reason.message() );		
	}
	else if ( status.status() == Kopete::OnlineStatus::Offline )
		slotGoOffline();
	else if ( status.status() == Kopete::OnlineStatus::Away )
	{
	      m_targetStatus = Kopete::OnlineStatus::Away;
	      slotGoAway( /* reason */ );	
	}
	else if ( status.status() == Kopete::OnlineStatus::Busy )
	{
	    m_targetStatus = Kopete::OnlineStatus::Busy;
	    slotGoBusy( /* reason */ );	
	}
	else if ( status.status() == Kopete::OnlineStatus::Invisible )
	{
	    m_targetStatus = Kopete::OnlineStatus::Invisible;
	    slotGoHidden( );
	}
}

void WebqqAccount::init_client_events(LwqqClient *lc)
{
    lwqq_add_event(lc->events->login_complete,
            _C_(2p,cb_login_stage_1,lc, &lc->args->login_ec));
    lwqq_add_event(lc->events->new_friend,
            _C_(2p,cb_friend_come,lc, &lc->args->buddy));
    lwqq_add_event(lc->events->new_group,
            _C_(2p,cb_group_come,lc, &lc->args->group));
    lwqq_add_event(lc->events->poll_msg,
            _C_(p,cb_qq_msg_check,lc));
    lwqq_add_event(lc->events->poll_lost,
            _C_(p,cb_lost_connection,lc));
    lwqq_add_event(lc->events->need_verify,
            _C_(2p,cb_need_verify2,lc, &lc->args->vf_image));
    lwqq_add_event(lc->events->delete_group,
            _C_(2p,cb_delete_group_local,lc, &lc->args->deleted_group));
    lwqq_add_event(lc->events->group_member_chg,
            _C_(2p,cb_flush_group_members,lc, &lc->args->group));
    lwqq_add_event(lc->events->upload_fail,
            _C_(4p,cb_upload_content_fail,lc, &lc->args->serv_id, &lc->args->content, &lc->args->err));
}

void WebqqAccount::initLwqqAccount()
{
	if(m_lc)
	  return;
	
    qDebug("init lwqq account\n");

		
    const char* username = s_strdup(accountId().toAscii().constData());
    const char* password = "XXXX";
	qq_account* ac = qq_account_new(username, password);

	m_lc = ac->qq;
    init_client_events(ac->qq);
	
	ac->db = lwdb_userdb_new(username,NULL,0);
	ac->qq->data = ac;
	
	lwqq_bit_set(ac->flag,QQ_USE_QQNUM,ac->db!=NULL);
	
	/*	
	lwqq_bit_set(ac->flag,IGNORE_FONT_SIZE,purple_account_get_bool(account, "disable_custom_font_size", FALSE));
	lwqq_bit_set(ac->flag, IGNORE_FONT_FACE, purple_account_get_bool(account, "disable_custom_font_face", FALSE));
	lwqq_bit_set(ac->flag, DARK_THEME_ADAPT, purple_account_get_bool(account, "dark_theme_fix", FALSE));
	lwqq_bit_set(ac->flag, DEBUG_FILE_SEND, purple_account_get_bool(account,"debug_file_send",FALSE));
	lwqq_bit_set(ac->flag, REMOVE_DUPLICATED_MSG, purple_account_get_bool(account,"remove_duplicated_msg",FALSE));
	lwqq_bit_set(ac->flag, QQ_DONT_EXPECT_100_CONTINUE,purple_account_get_bool(account,"dont_expected_100_continue",FALSE));
	lwqq_bit_set(ac->flag, NOT_DOWNLOAD_GROUP_PIC, purple_account_get_bool(account, "no_download_group_pic", FALSE));
	ac->recent_group_name = s_strdup(purple_account_get_string(account, "recent_group_name", "Recent Contacts"));
*/
}

void WebqqAccount::destoryLwqqAccount()
{

    printf("will close qq\n");
    if (m_lc == NULL)
      return;
    qq_account* ac = (qq_account *)(m_lc->data);
    LwqqErrorCode err;

    if(lwqq_client_logined(ac->qq))
        lwqq_logout(ac->qq,&err);
    lwqq_msglist_close(ac->qq->msg_list);
/*
    LwqqGroup* g;
    LIST_FOREACH(g,&ac->qq->groups,entries){ 
        qq_cgroup_free((qq_chat_group*)g->data);
    }
*/
    lwqq_client_free(ac->qq);
    lwdb_userdb_free(ac->db);
    qq_account_free(ac);
//   purple_connection_set_protocol_data(gc,NULL);
    //translate_global_free();
/*
    g_ref_count -- ;
    if(g_ref_count == 0){
        lwqq_http_global_free();
        lwqq_async_global_quit();
        lwdb_global_free();
    }
*/  
  
    m_lc = NULL;
}

int WebqqAccount::login()
{
    LwqqErrorCode err;

    lwqq_login(m_lc,LWQQ_STATUS_ONLINE, &err);

    return 0;
}

void WebqqAccount::logout()
{
    LwqqErrorCode err;

    lwqq_logout(m_lc, &err);
    if (err != LWQQ_EC_OK) {
        kWarning(14210) << "logout failed"<<endl;
    } else {
        kWarning(14210) << "loginout success"<<endl;
    }
}



void WebqqAccount::slotReceivedInstanceSignal(CallbackObject cb)
{
  /*call need_verify2 function*/
  if(cb.fun_t == NEED_VERIFY2)
  {
    LwqqClient* lc = (LwqqClient*)(cb.ptr1);
    LwqqVerifyCode* code = (LwqqVerifyCode*) (cb.ptr2);
    this->ac_need_verify2(lc, code);
  }
  else if(cb.fun_t == LOGIN_COMPLETE)
  {
    LwqqClient* lc = (LwqqClient*)(cb.ptr1);
    LwqqErrorCode *err = (LwqqErrorCode *)(cb.ptr2);
    this->ac_login_stage_1(lc, err);
  }
  else if(cb.fun_t == LOGIN_STAGE_2)
  {
    LwqqClient* lc = (LwqqClient*)(cb.ptr1);
    LwqqAsyncEvent *ev = (LwqqAsyncEvent *)(cb.ptr2);
    this->ac_login_stage_2(ev,lc);
  }
  else if(cb.fun_t == LOGIN_STAGE_3)
  {
    LwqqClient* lc = (LwqqClient*)(cb.ptr1);
    this->ac_login_stage_3(lc);
  }
  else if(cb.fun_t == LOGIN_STAGE_f)
  {
    LwqqClient* lc = (LwqqClient*)(cb.ptr1);
    this->ac_login_stage_f(lc);
  }
  else if (cb.fun_t == FRIEND_AVATAR)
  {
    LwqqClient *lc = (LwqqClient*)(cb.ptr1);
    LwqqBuddy *buddy = (LwqqBuddy*)(cb.ptr2);
    this->ac_friend_avatar(lc, buddy);
  }
  else if(cb.fun_t == GROUP_AVATAR)
  {
    LwqqClient *lc = (LwqqClient*)(cb.ptr1);
    LwqqGroup *group = (LwqqGroup*)(cb.ptr2);
    this->ac_group_avatar(lc, group);
  }
  else if(cb.fun_t == GROUP_MEMBERS)
  {
    LwqqClient *lc = (LwqqClient*)(cb.ptr1);
    LwqqGroup *group = (LwqqGroup*)(cb.ptr2);
    this->ac_group_members(lc, group);
  }
  else if (cb.fun_t == QQ_MSG_CHECK)
  {
    LwqqClient *lc = (LwqqClient*)(cb.ptr1);
    this->ac_qq_msg_check(lc);
  }else if(cb.fun_t == SHOW_CONFIRM)
  {
      LwqqClient *lc = (LwqqClient*)(cb.ptr1);
      LwqqConfirmTable *table = (LwqqConfirmTable*)(cb.ptr2);
      add_info *info = (add_info*)(cb.ptr3);
      this->ac_show_confirm_table(lc, table, info);
  }else if(cb.fun_t == SHOW_MESSAGE)
  {
      msg_type type = cb.type;
      const char *title = (const char*)(cb.ptr1);
      const char *msg = (const char *)(cb.ptr2);
      this->ac_show_messageBox(type, title, msg);
  }
  
  
  /*add function here*/
  
  /*
  else if(cb.fun_t == XXX)
  {
  }
  */
  
}

void WebqqAccount::buddy_message(LwqqClient* lc,LwqqMsgMessage* msg)
{
    qq_account* ac = lwqq_client_userdata(lc);
    //PurpleConnection* pc = ac->gc;
    char buf[BUFLEN]={0};
    //clean buffer
    strcpy(buf,"");
    LwqqBuddy* buddy = msg->buddy.from;
    qDebug()<<"buddy message";
    translate_struct_to_message(ac,msg,buf);

    /*find the contact*/
    WebqqContact * contact = dynamic_cast<WebqqContact *>(contacts().value( QString(buddy->qqnumber) ));
    if ( !contact)
	return;
    
    contact->receivedMessage(stransMsg(QString::fromUtf8(buf)));
    
    //serv_got_im(pc, local_id, buf, PURPLE_MESSAGE_RECV, msg->time);
}

QString WebqqAccount::stransMsg(const QString &message)
{
    QString reMsg;
    int pos = 0;
    int endPos = 0;
    int size = 0;
    int msgPos = 0;
    qDebug()<<"strans:"<<message;
    reMsg = "<span style=\" ";
    if((pos = message.indexOf("face=\"")) >= 0)
    {
        size = QString("face=\"").size();
        endPos = message.indexOf("\"", pos + size);
        reMsg +=" font-family:'" + message.mid(pos + size, endPos - pos - size ) + "';";
        msgPos = pos;
    }
    if(message.indexOf("<b>") >= 0)
        reMsg += "font-weight:600;";
    if(message.indexOf("<i>") >= 0)
        reMsg += " font-style:italic;";
    if(message.indexOf("<u>") >= 0)
        reMsg += " text-decoration: underline;";
    if((pos = message.indexOf("color=\"#")) >= 0)
    {
        size = QString("color=\"#").size();
        reMsg += " color:#" + message.mid(pos+size, 6) + ";" ;
        msgPos = pos;
    }

    if((pos = message.indexOf("size=\"")) >= 0)
    {
        size = QString("size=\"").size();
        endPos = message.indexOf("\"", pos + size);
        reMsg +=" font-size:" + message.mid(pos + size, endPos - pos - size ) + "pt;";
        msgPos = pos;
    }
    reMsg += "\">";
    if((pos = message.indexOf(">", msgPos)) >= 0)
    {
        endPos = message.indexOf("</font>", pos);
        size = QString(">").size();
        reMsg +=message.mid(pos + size, endPos - pos - size ) + "</span>";
    }

    qDebug()<<"reMsg:"<<reMsg;
    return reMsg;
}

void WebqqAccount::group_message(LwqqClient *lc, LwqqMsgMessage *msg)
{
    qq_account* ac = lwqq_client_userdata(lc);
    LwqqGroup* group = find_group_by_gid(lc,(msg->super.super.type==LWQQ_MS_DISCU_MSG)?msg->discu.did:msg->super.from);
    Kopete::ContactPtrList justMe;
    LwqqBuddy* buddy;
    QString sendId;
    if(group == NULL) return;

    //force open dialog
    static char buf[BUFLEN] ;
    strcpy(buf,"");

    translate_struct_to_message(ac,msg,buf);
    /*find the contact*/
    WebqqContact * chatContact = dynamic_cast<WebqqContact *>(contacts().value( QString(group->gid) ));
    if ( !chatContact)
    return;
    justMe.append(myself());
    qDebug()<<"time:"<<QDateTime::fromTime_t(msg->time)<<QString(msg->group.send);
    if((buddy = find_buddy_by_uin(lc,msg->group.send)))
        sendId = QString(buddy->qqnumber);
    else
        sendId = QString(msg->group.send);
    Kopete::Message kmsg( contact(sendId), justMe );
    //kmsg.setTimestamp( QString(msg->time) );
    qDebug()<<"group message:"<<QString::fromUtf8(buf);
    kmsg.setHtmlBody(stransMsg(QString::fromUtf8(buf)));
    kmsg.setDirection( Kopete::Message::Inbound );

    chatContact->groupSession()->appendMessage(kmsg);
}

void WebqqAccount::ac_qq_msg_check(LwqqClient *lc)
{
    //printf("[%s] start\n", __FUNCTION__);
    if(!lwqq_client_valid(lc)) 
      return;
    LwqqRecvMsgList* l = lc->msg_list;
    LwqqRecvMsg *msg,*prev;
    LwqqMsgSystem* sys_msg;

    pthread_mutex_lock(&l->mutex);
    if (TAILQ_EMPTY(&l->head)) {
        /* No message now, wait 100ms */
        pthread_mutex_unlock(&l->mutex);
        return ;
    }
    msg = TAILQ_FIRST(&l->head);
    while(msg) {
        int res = SUCCESS;
        if(msg->msg) {
            switch(lwqq_mt_bits(msg->msg->type)) {
            case LWQQ_MT_MESSAGE:
                switch(msg->msg->type){
                    case LWQQ_MS_BUDDY_MSG:
			//printf("this is a message\n");
                        buddy_message(lc,(LwqqMsgMessage*)msg->msg);
                        break;
                    case LWQQ_MS_GROUP_MSG:
                    case LWQQ_MS_DISCU_MSG:
                    case LWQQ_MS_GROUP_WEB_MSG:
                        group_message(lc,(LwqqMsgMessage*)msg->msg);
                        break;
                    case LWQQ_MS_SESS_MSG:
                        //whisper_message(lc,(LwqqMsgMessage*)msg->msg);
                        break;
                    default: 
			break;
                }
                break;
            case LWQQ_MT_STATUS_CHANGE:
                qDebug()<<"LWQQ_MT_STATUS_CHANGE";
                //status_change(lc,(LwqqMsgStatusChange*)msg->msg);
                break;
            case LWQQ_MT_KICK_MESSAGE:
                qDebug()<<"LWQQ_MT_KICK_MESSAGE";
                //kick_message(lc,(LwqqMsgKickMessage*)msg->msg);
                break;
            case LWQQ_MT_SYSTEM:
#if 1
                sys_msg = (LwqqMsgSystem*)msg->msg;
                if(sys_msg->type == LwqqMsgSystem::VERIFY_REQUIRED){
                    msg->msg = NULL;
                    LwqqBuddy* buddy = lwqq_buddy_new();
                    LwqqAsyncEvent* ev = lwqq_info_get_stranger_info(
                            lc, sys_msg->super.from, buddy);
                    lwqq_async_add_event_listener(ev,
                            _C_(3p,system_message,lc,sys_msg,buddy));
                }else{
                    system_message(lc,(LwqqMsgSystem*)msg->msg,NULL);
                }
#endif 
                break;
            case LWQQ_MT_BLIST_CHANGE:
                qDebug()<<"LWQQ_MT_BLIST_CHANGE";
                //blist_change(lc,(LwqqMsgBlistChange*)msg->msg);
                break;
            case LWQQ_MT_OFFFILE:
                qDebug()<<"LWQQ_MT_OFFFILE";
                //offline_file(lc,(LwqqMsgOffFile*)msg->msg);
                break;
            case LWQQ_MT_FILE_MSG:
                qDebug()<<"LWQQ_MT_FILE_MSG";
                //file_message(lc,(LwqqMsgFileMessage*)msg->msg);
                break;
            case LWQQ_MT_FILETRANS:
                qDebug()<<"LWQQ_MT_FILETRANS";
                //complete_file_trans(lc,(LwqqMsgFileTrans*)msg->msg->opaque);
                break;
            case LWQQ_MT_NOTIFY_OFFFILE:
                qDebug()<<"LWQQ_MT_NOTIFY_OFFFILE";
                //notify_offfile(lc,(LwqqMsgNotifyOfffile*)msg->msg);
                break;
            case LWQQ_MT_INPUT_NOTIFY:
                qDebug()<<"LWQQ_MT_INPUT_NOTIFY";
                //input_notify(lc,(LwqqMsgInputNotify*)msg->msg);
                break;
            case LWQQ_MT_SYS_G_MSG:
                qDebug()<<"LWQQ_MT_SYS_G_MSG";
                //sys_g_message(lc,(LwqqMsgSysGMsg*)msg->msg);
                break;
            case LWQQ_MT_SHAKE_MESSAGE:
                qDebug()<<"LWQQ_MT_SHAKE_MESSAGE";
                //shake_message(lc,(LwqqMsgShakeMessage*)msg->msg);
                break;
            default:
                //lwqq_puts("unknow message\n");
                break;
            }
        }

        prev = msg;
        msg = TAILQ_NEXT(msg,entries);
        if(res == SUCCESS){
            TAILQ_REMOVE(&l->head,prev, entries);
            lwqq_msg_free(prev->msg);
            s_free(prev);
        }
    }
    pthread_mutex_unlock(&l->mutex);
    return ;  
}

void WebqqAccount::ac_friend_avatar(LwqqClient* lc, LwqqBuddy *buddy)
{   
    //printf("[%s] qq: %s, stat is: %d, len:%d\n",__FUNCTION__, buddy->qqnumber, buddy->stat, buddy->avatar_len);
    QByteArray bytearry(buddy->avatar, buddy->avatar_len);
    
    /*find out the contact*/ 
    //qDebug()<<"find out the contact";
    WebqqContact * contact = dynamic_cast<WebqqContact *>(contacts().value( QString(buddy->qqnumber) ));
    if ( !contact)
	return;
    
    /*first set its stat*/
    Kopete::OnlineStatus webqqStatus = statusFromLwqqStatus(buddy->stat);
    contact->setOnlineStatus(webqqStatus);
    
    /*set the contact's icon*/
   // qDebug() <<"contact icon:" << bytearry.size();
    if (!bytearry.isEmpty())
    {
      contact->setDisplayPicture(bytearry);
    }else{

        //qDebug()<<"avatarData size:" << avatarData.size();
        //QFile::remove(filename);
        contact->setDisplayPicture(avatarData);
    }
  
}

void WebqqAccount::ac_group_avatar(LwqqClient *lc, LwqqGroup *group)
{
    printf("[%s] qq: %s, name is: %s, len:%d ,markname:%s, groupnum:%s, type:%d\n",__FUNCTION__, group->gid, group->name, group->avatar_len, group->markname, group->account, group->type);
    QByteArray bytearry(group->avatar, group->avatar_len);

    /*find out the contact*/
    qDebug()<<"find out the contact";
    WebqqContact * contact = dynamic_cast<WebqqContact *>(contacts().value( QString(group->gid) ));
    if ( !contact)
    return;

    /*first set its stat*/
    //Kopete::OnlineStatus webqqStatus = statusFromLwqqStatus(buddy->stat);
    contact->setOnlineStatus(m_protocol->WebqqOnline );

    /*set the contact's icon*/
    qDebug() <<"contact icon:" << bytearry.size();
    if (!bytearry.isEmpty())
    {
      contact->setDisplayPicture(bytearry);
    }else{

        qDebug()<<"avatarData size:" << avatarData.size();
        //QFile::remove(filename);
        contact->setDisplayPicture(avatarData);
    }
}

void WebqqAccount::ac_group_members(LwqqClient *lc, LwqqGroup *group)
{
    LwqqSimpleBuddy* member;
    LwqqBuddy* buddy;
    LIST_FOREACH(member,&group->members,entries)
    {
        QString contactName;
        if(member->card)
            contactName = QString::fromUtf8(member->card);
        else
            contactName = QString::fromUtf8(member->nick);
        qDebug()<<"groupname:"<<contactName<<"qq:"<<QString(member->uin)<<QString(group->gid);
        qDebug()<<"clientid:"<<QString(lc->clientid)<<"uin"<<QString(lc->myself->uin);
        if(strcmp(lc->myself->uin, member->uin) != 0)
        {
            if((buddy = find_buddy_by_uin(lc,member->uin))) {
                if( !contact(QString(buddy->qqnumber)))
                {
                    addContact( QString(buddy->qqnumber), contactName,  0L, Kopete::Account::Temporary);
                }
                contact(QString(group->gid))->webqq_addcontacts(contact(QString(buddy->qqnumber)));
            }else{

                if( !contact(QString(member->uin)))
                {
                    addContact( QString(member->uin), contactName,  0L, Kopete::Account::Temporary);
                }

                contact(QString(group->gid))->webqq_addcontacts(contact(QString(member->uin)));
            }
        }
    }
    contact(QString(group->gid))->set_group_members();
}

void WebqqAccount::ac_need_verify2(LwqqClient* lc, LwqqVerifyCode* code)
{
    printf("[%s] \n", __FUNCTION__);
    const char *dir = "/tmp/";
    char fname[32];
    char vcode[256] = {0};
    snprintf(fname,sizeof(fname),"%s.jpeg",lc->username);
    lwqq_util_save_img(code->data,code->size,fname,dir);
    //if(!lwqq_client_valid(code->lc)) return;
    LoginVerifyDialog *dlg = new LoginVerifyDialog();
    QString fullPath = QString(dir) + QString(fname);
    dlg->setImage(fullPath);
    dlg->exec();
    QString input = dlg->getVerifyString();
    
    code->str = s_strdup(input.toAscii().data());
    vp_do(code->cmd,NULL);
    
    delete dlg;
    
    return ;
}



void test(LwqqClient *lc)
{
  printf("has called test\n");
}

void WebqqAccount::ac_login_stage_1(LwqqClient* lc,LwqqErrorCode* p_err)
{
     LwqqErrorCode err = *p_err;
     QString message;
     switch(err){
        case LWQQ_EC_OK:
	    message = i18n( "Connect OK");
	    //KNotification::event( "Connect OK", message, myself()->onlineStatus().protocolIcon(KIconLoader::SizeMedium) );
        //myself()->setOnlineStatus( m_targetStatus );
	    password().setWrong(false);
	    
	    afterLogin();    
            break;
	case LWQQ_EC_ERROR: /*error verify code or error password or error username*/
	    printf("err msg: %s\n",m_lc->last_err);  
	    if (strncmp(m_lc->last_err, "Wrong username or password", strlen("Wrong username or password")) == 0)
	    {	
		password().setWrong();
		/*this is very ugly, since we need user to reinput verfy code*/
		message = i18n( "Password Error!");
		KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Error, message);
		//KNotification::event( "Connect failed", message, myself()->onlineStatus().protocolIcon(KIconLoader::SizeMedium) );
	    }
	    else if (strncmp(m_lc->last_err, "Wrong verify code", strlen("Wrong verify code")) == 0)
	    {
		/*this is very ugly, since we need user to reinput verfy code*/
		m_lc->vc = NULL;
		message = i18n( "Verify Code Error!");
		KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Error, message);
		//KNotification::event( "Connect failed", message, myself()->onlineStatus().protocolIcon(KIconLoader::SizeMedium) );
	    }
	    myself()->setOnlineStatus( m_protocol->WebqqOffline);   
	    disconnected( Manual );			// don't reconnect
	    return;
	case LWQQ_EC_LOGIN_ABNORMAL:
	    disconnected( Manual );			// don't reconnect
	    message = i18n( "Login Abnormal!");
	    KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Error, message);
	    myself()->setOnlineStatus( m_protocol->WebqqOffline);
            //purple_connection_error_reason(gc,PURPLE_CONNECTION_ERROR_OTHER_ERROR,_("Account Problem Occurs,Need lift the ban"));
            return ;
        case LWQQ_EC_NETWORK_ERROR:
	    disconnected( Manual );			// don't reconnect
	    myself()->setOnlineStatus( m_protocol->WebqqOffline);
	    message = i18n( "Network error!");
	    KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Error, message);
            //purple_connection_error_reason(gc,PURPLE_CONNECTION_ERROR_OTHER_ERROR,_("Network Error"));
            return;
        default:
         qDebug()<<"error:"<<err;
         fprintf(stderr,"err msg: %s\n",m_lc->last_err);
	    disconnected( Manual );			// don't reconnect
	    myself()->setOnlineStatus( m_protocol->WebqqOffline);
	    message = i18n( "Unknown error!");
	    KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Error, message);
            return;
    }

    
    //QObject::connect(timer_after_login, SIGNAL(timeout()), this, SLOT(afterLogin()));
    //timer_after_login->setSingleShot(true);
    //timer_after_login->start(500);

}

void WebqqAccount::pollMessage()
{
  printf("hahahaaa\n");
  if(!m_lc)
    return;
  
  LwqqRecvMsgList *msgList = lwqq_msglist_new(m_lc);

  lwqq_msglist_poll(msgList, POLL_AUTO_DOWN_BUDDY_PIC);
  //lwqq_msglist_free(msgList);

}

void WebqqAccount::afterLogin()
{
    char path[512];
    if(access(LOCAL_HASH_JS(path),F_OK)==0)
        get_friends_info_retry(m_lc, hash_with_local_file);
    else
        get_friends_info_retry(m_lc, hash_with_remote_file);

}

void WebqqAccount::ac_login_stage_2(LwqqAsyncEvent* event,LwqqClient* lc)
{
    if(event->result != LWQQ_EC_OK){
      printf("Get group list failed, error code is %d\n", event->result);
      /*  qq_account* ac = lc->data;
        purple_connection_error_reason(ac->gc, 
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, 
                _("Get Group List Failed"));*/
        //return;
    }
    LwqqAsyncEvset* set = lwqq_async_evset_new();
    LwqqAsyncEvent* ev;
    ev = lwqq_info_get_discu_name_list(lc);
    lwqq_async_evset_add_event(set,ev);
    ev = lwqq_info_get_online_buddies(lc,NULL);
    lwqq_async_evset_add_event(set,ev);
    
    qq_account* ac = lwqq_client_userdata(lc);
  /*  
    const char* alias = purple_account_get_alias(ac->account);
    if(alias == NULL){
        ev = lwqq_info_get_friend_detail_info(lc,lc->myself);
        lwqq_async_evset_add_event(set,ev);
    }
*/

    ev = lwqq_info_get_friend_detail_info(lc,lc->myself);
    lwqq_async_evset_add_event(set,ev);
    
    lwqq_async_add_evset_listener(set,_C_(p,cb_login_stage_3,lc));
     
}

void WebqqAccount::friend_come(LwqqClient* lc,LwqqBuddy* buddy)
{

  //printf("[%s] buddy's qq number: %s, \n", __FUNCTION__, buddy->qqnumber);

    
#if 0   
    qq_account* ac = lwqq_client_userdata(lc);
    ac->disable_send_server = 1;
    PurpleAccount* account=ac->account;
    PurpleBuddy* bu = NULL;
 #endif
    Kopete::Group group;
    LwqqFriendCategory* cate;
    int cate_index = buddy->cate_index;
    QString categoryName;

    if(cate_index == LWQQ_FRIEND_CATE_IDX_PASSERBY)
    {
	categoryName = QString("Passerby");
    }
    else
    {
        LIST_FOREACH(cate,&m_lc->categories,entries) 
	{
            if(cate->index==cate_index) 
	    {
		categoryName = QString::fromUtf8(cate->name);
                break;
            }
        }
    }
    
    QList<Kopete::Group*> groupList = Kopete::ContactList::self()->groups();
    Kopete::Group *g;
    Kopete::Group *targetGroup = NULL;
    
    foreach(g, groupList)
    {
	if(g->displayName() == categoryName)
	{
	  targetGroup = g;
	  break;
	}
    }
    /*if not found this group, add a new group*/
    if (targetGroup == NULL)
    {
      targetGroup = new Kopete::Group(categoryName);
      Kopete::ContactList::self()->addGroup( targetGroup );  
    }
    
    /*get buddy's display name*/
    QString displayName;
    if (buddy->markname != NULL)
    {
      displayName = QString::fromUtf8(buddy->markname);
    }
    else
    {
	if (QString(buddy->nick).length() == 0)
	    displayName = QString::fromUtf8(buddy->qqnumber);
	else
	    displayName = QString::fromUtf8(buddy->nick); 
    }
    
    //this is avaliable when reload avatar in
    //login_stage_f
    if(buddy->avatar_len)
    {
        ac_friend_avatar(lc, buddy);
    }
    else
    {
        LwqqAsyncEvset* set = lwqq_async_evset_new();
	LwqqAsyncEvent* ev;
	//download avatar 
	ev = lwqq_info_get_friend_avatar(lc,buddy);
	lwqq_async_evset_add_event(set,ev);
	ev = lwqq_info_get_friend_detail_info(lc, buddy);/*get detail of friend*/
	lwqq_async_evset_add_event(set,ev);
	
	lwqq_async_add_evset_listener(set,_C_(2p,cb_friend_avatar, lc, buddy));
	//lwqq_async_add_event_listener(ev,_C_(2p,cb_friend_avatar, lc, buddy));
    }

    addContact( QString(buddy->qqnumber), displayName,  targetGroup, Kopete::Account::ChangeKABC );
    
    WebqqContact * addedContact = dynamic_cast<WebqqContact *>(contacts().value( QString(buddy->qqnumber) ));
    addedContact->setContactType(WebqqContact::Contact_Chat);
//    QByteArray data;
//    data.clear();
//    addedContact->setDisplayPicture(data);
#if 0
    const char* key = try_get(buddy->qqnumber,buddy->uin);
    const char* disp = try_get(buddy->markname,buddy->nick);
    bu = purple_find_buddy(account,key);
    if(bu == NULL) {
        bu = purple_buddy_new(ac->account,key,(buddy->markname)?buddy->markname:buddy->nick);
        purple_blist_add_buddy(bu,NULL,group,NULL);
        //if there isn't a qqnumber we shouldn't save it.
        if(!buddy->qqnumber) 
	  purple_blist_node_set_flags(PURPLE_BLIST_NODE(bu),PURPLE_BLIST_NODE_FLAG_NO_SAVE);
    }
    purple_buddy_set_protocol_data(bu, buddy);
    buddy->data = bu;
    if(purple_buddy_get_group(bu)!=group&&strcmp(purple_buddy_get_group(bu)->name,ac->recent_group_name)!=0) 
        purple_blist_add_buddy(bu,NULL,group,NULL);
    if(!bu->alias || strcmp(bu->alias,disp)!=0 )
        purple_blist_alias_buddy(bu,disp);
    if(buddy->stat){
        purple_prpl_got_user_status(account, key, buddy_status(buddy), NULL);
    }
    //this is avaliable when reload avatar in
    //login_stage_f
    if(buddy->avatar_len)
        friend_avatar(ac, buddy);
    //download avatar 
    PurpleBuddyIcon* icon;
    if((icon = purple_buddy_icons_find(account,key))==0) {
        LwqqAsyncEvent* ev = lwqq_info_get_friend_avatar(lc,buddy);
        lwqq_async_add_event_listener(ev,_C_(2p,friend_avatar,ac,buddy));
    }

    qq_account_insert_index_node(ac, buddy,NULL);

    ac->disable_send_server = 0;
#endif
  
}

void WebqqAccount::initCategory()
{
    printf("Try to init Category\n");
    LwqqFriendCategory* cate;
    

    QList<Kopete::Group*> groupList = Kopete::ContactList::self()->groups();
    Kopete::Group *g;
    Kopete::Group *newGroup;
    
    foreach(g, groupList)
    {
	Kopete::ContactList::self()->removeGroup( g );
    }
     
    /*add new group*/
    LIST_FOREACH(cate,&m_lc->categories,entries) {
		QString groupName = QString::fromUtf8(cate->name);;
                newGroup = new Kopete::Group(groupName);
		Kopete::ContactList::self()->addGroup( newGroup );      
    }
   

}

WebqqContact *WebqqAccount::contact(const QString &id)
{
    return dynamic_cast<WebqqContact *>(contacts().value( id ));
}

void WebqqAccount::group_come(LwqqClient* lc,LwqqGroup* group)
{

  printf("group info:\t\t type: %d\t\t name: %s\t\t markname: %s\n", group->type, group->name, group->markname);

#if 0
    qq_account* ac = lwqq_client_userdata(lc);
    ac->disable_send_server = 1;
    PurpleAccount* account=ac->account;
    PurpleGroup* qun = purple_group_new(QQ_GROUP_DEFAULT_CATE);
    PurpleGroup* talk = purple_group_new(_("Discussion"));
    GHashTable* components;
    PurpleChat* chat;
    const char* type;
    const char* key = try_get(group->account,group->gid);

    if( (chat = purple_blist_find_chat(account,key)) == NULL) {
        components = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
        g_hash_table_insert(components,g_strdup(QQ_ROOM_KEY_GID),g_strdup(key));
        type = (group->type==LWQQ_GROUP_QUN)? QQ_ROOM_TYPE_QUN:QQ_ROOM_TYPE_DISCU;
        g_hash_table_insert(components,g_strdup(QQ_ROOM_TYPE),g_strdup(type));
        chat = purple_chat_new(account,key,components);
        purple_blist_add_chat(chat,lwqq_group_is_qun(group)?qun:talk,NULL);
        //we shouldn't save it.
        if(!group->account) purple_blist_node_set_flags(PURPLE_BLIST_NODE(chat),PURPLE_BLIST_NODE_FLAG_NO_SAVE);
    } else {
        components = chat->components;
        if(!g_hash_table_lookup(components,QQ_ROOM_TYPE)){
            type = (group->type==LWQQ_GROUP_QUN)? QQ_ROOM_TYPE_QUN:QQ_ROOM_TYPE_DISCU;
            g_hash_table_insert(components,s_strdup(QQ_ROOM_TYPE),g_strdup(type));
        }

        if(!group->account) purple_blist_node_set_flags(PURPLE_BLIST_NODE(chat),PURPLE_BLIST_NODE_FLAG_NO_SAVE);
    }

    qq_chat_group* cg = qq_cgroup_new(QQ_CG_OPT);
    group->data = cg;
    cg->group = group;
    cg->chat = chat;
    
    if(lwqq_group_is_qun(group)){
        qq_set_group_name(cg);
        if(purple_buddy_icons_node_has_custom_icon(PURPLE_BLIST_NODE(chat))==0){
            LwqqAsyncEvent* ev = lwqq_info_get_group_avatar(lc,group);
            lwqq_async_add_event_listener(ev,_C_(2p,group_avatar,ev,group));
        }
    }else{
        qq_set_group_name(cg);
    }

    qq_account_insert_index_node(ac, NULL, group);

    ac->disable_send_server = 0;
#endif

    QString categoryName;
    if(group->type == 0)
        categoryName = QString("Group");
    else
        categoryName = QString("Discussion");

    QList<Kopete::Group*> groupList = Kopete::ContactList::self()->groups();
    Kopete::Group *g;
    Kopete::Group *targetGroup = NULL;

    foreach(g, groupList)
    {
    if(g->displayName() == categoryName)
    {
      targetGroup = g;
      break;
    }
    }
    /*if not found this group, add a new group*/
    if (targetGroup == NULL)
    {
      printf("add group\n");
      targetGroup = new Kopete::Group(categoryName);
      Kopete::ContactList::self()->addGroup( targetGroup );
    }

    /*get buddy's display name*/
    QString displayName;
    if (group->markname != NULL)
    {
      displayName = QString::fromUtf8(group->markname);
    }
    else
    {
    if (QString(group->name).length() == 0)
        displayName = QString::fromUtf8(group->gid);
    else
        displayName = QString::fromUtf8(group->name);
    }

    //this is avaliable when reload avatar in
    //login_stage_f
    if(group->avatar_len)
    {
        ac_group_avatar(lc, group);
    }
    else
    {
        LwqqAsyncEvset* set = lwqq_async_evset_new();
        LwqqAsyncEvent* ev;
        //download avatar
        ev = lwqq_info_get_group_avatar(lc,group);
        lwqq_async_evset_add_event(set,ev);
        lwqq_async_add_evset_listener(set,_C_(2p,cb_group_avatar, lc, group));
        ev = lwqq_info_get_group_detail_info(lc, group, NULL);/*get detail of friend*/
        lwqq_async_evset_add_event(set,ev);
        lwqq_async_add_evset_listener(set,_C_(2p,cb_group_members, lc, group));
        //lwqq_async_add_event_listener(ev,_C_(2p,cb_friend_avatar, lc, buddy));
    }
    addContact( QString(group->gid), displayName,  targetGroup, Kopete::Account::ChangeKABC );
    WebqqContact * addedContact = dynamic_cast<WebqqContact *>(contacts().value( QString(group->gid) ));
    addedContact->setContactType(WebqqContact::Contact_Group);


//    if(group->type == 0)
//    {
//        addContact( QString(group->gid), displayName,  targetGroup, Kopete::Account::ChangeKABC );
//        WebqqContact * addedContact = dynamic_cast<WebqqContact *>(contacts().value( QString(group->gid) ));
//        addedContact->setContactType(0);
//    }else
//    {
//        addContact( QString(group->account), displayName,  targetGroup, Kopete::Account::ChangeKABC );
//        WebqqContact * addedContact = dynamic_cast<WebqqContact *>(contacts().value( QString(group->account) ));
//        addedContact->setContactType(1);
//    }

}

void WebqqAccount::ac_login_stage_3(LwqqClient* lc)
{
  printf("**************stage3\n");

  qq_account* ac = lwqq_client_userdata(lc);
/*
    purple_connection_set_state(purple_account_get_connection(ac->account),PURPLE_CONNECTED);

    if(!purple_account_get_alias(ac->account))
        purple_account_set_alias(ac->account,lc->myself->nick);
    if(purple_buddy_icons_find_account_icon(ac->account)==NULL){
        LwqqAsyncEvent* ev=lwqq_info_get_friend_avatar(lc,lc->myself);
        lwqq_async_add_event_listener(ev,_C_(2p,friend_avatar,ac,lc->myself));
    }
*/
    LwqqAsyncEvent* ev = NULL;

    lwdb_userdb_flush_buddies(ac->db, 5,5);
    lwdb_userdb_flush_groups(ac->db, 1,10);

    if(ac->flag & QQ_USE_QQNUM){  //QQ_USE_QQNUM
        lwdb_userdb_query_qqnumbers(ac->db,lc);
    }
/*
    //we must put buddy and group clean before any add operation.
    GSList* ptr = purple_blist_get_buddies();
    while(ptr){
        PurpleBuddy* buddy = ptr->data;
        if(buddy->account == ac->account){
            const char* qqnum = purple_buddy_get_name(buddy);
            //if it isn't a qqnumber,we should delete it whatever.
            if(lwqq_buddy_find_buddy_by_qqnumber(lc,qqnum)==NULL){
                purple_blist_remove_buddy(buddy);
            }
        }
        ptr = ptr->next;
    }
*/
    //clean extra duplicated node
//    all_reset(ac,RESET_GROUP_SOFT|RESET_DISCU);

    LwqqAsyncEvset* set = lwqq_async_evset_new();
    LwqqBuddy* buddy;
    LIST_FOREACH(buddy,&lc->friends,entries) {
	//printf("query friend\n");
	//printf("buddy's uin is %s,last_modify is %d, qqnumber is %s\n", buddy->uin, buddy->last_modify,buddy->qqnumber);
        lwdb_userdb_query_buddy(ac->db, buddy);
        //if((ac->flag& 1 )&& ! buddy->qqnumber){ //QQ_USE_QQNUM =1
	if(! buddy->qqnumber){
            ev = lwqq_info_get_friend_qqnumber(lc,buddy);
            lwqq_async_evset_add_event(set, ev);
        }
        if(buddy->last_modify == 0 || buddy->last_modify == -1) {
            ev = lwqq_info_get_single_long_nick(lc, buddy);
            lwqq_async_evset_add_event(set, ev);
            ev = lwqq_info_get_level(lc, buddy);
            lwqq_async_evset_add_event(set, ev);
            //if buddy is unknow we should update avatar in friend_come
            //for better speed in first load
            if(buddy->last_modify == LWQQ_LAST_MODIFY_RESET){
                ev = lwqq_info_get_friend_avatar(lc,buddy);
                lwqq_async_evset_add_event(set, ev);
            }
        }
        if(buddy->last_modify != -1 && buddy->last_modify != 0)	
            friend_come(lc,buddy);
    }
    
    LwqqGroup* group;
    LIST_FOREACH(group,&lc->groups,entries) {
        //LwqqAsyncEvset* set = NULL;
        lwdb_userdb_query_group(ac->db, group);
        if((ac->flag && 1)&& ! group->account){ //QQ_USE_QQNUM = 1
	  
            ev = lwqq_info_get_group_qqnumber(lc,group);
            lwqq_async_evset_add_event(set, ev);
        }
        if(group->last_modify == -1 || group->last_modify == 0){
            ev = lwqq_info_get_group_memo(lc, group);
            lwqq_async_evset_add_event(set, ev);
        }
        //because group avatar less changed.
        //so we dont reload it.

        if(group->last_modify != -1 && group->last_modify != 0)
            group_come(lc,group);
    }
    lwqq_async_add_evset_listener(set, _C_(p,cb_login_stage_f,lc));
    //after this we finished the qqnumber fast index.

    //we should always put discu after group deletion.
    //to avoid delete discu list.
    LwqqGroup* discu;

/*    
    LIST_FOREACH(discu,&lc->discus,entries) {
        discu_come(lc,discu);
    }
*/
    myself()->setOnlineStatus( m_targetStatus );
    ac->state = LOAD_COMPLETED;
    LwqqPollOption flags = POLL_AUTO_DOWN_DISCU_PIC|POLL_AUTO_DOWN_GROUP_PIC|POLL_AUTO_DOWN_BUDDY_PIC;
    if(ac->flag& REMOVE_DUPLICATED_MSG)
        flags |= POLL_REMOVE_DUPLICATED_MSG;
    if(ac->flag& NOT_DOWNLOAD_GROUP_PIC)
        flags &= ~POLL_AUTO_DOWN_GROUP_PIC;
    
    lwqq_msglist_poll(lc->msg_list, flags);
}


void WebqqAccount::ac_login_stage_f(LwqqClient* lc)
{
  /*   
    printf("ac_login_stage_f\n");
    qq_account* ac = lwqq_client_userdata(lc);
    LwqqBuddy *buddy;
    LIST_FOREACH(buddy,&lc->friends,entries) {
	printf("query friend\n");
	printf("buddy's uin is %s,last_modify is %d, qqnumber is %s\n", buddy->uin, buddy->last_modify,buddy->qqnumber);
	lwdb_userdb_query_buddy(ac->db, buddy);
	addContact( QString(buddy->qqnumber), QString(buddy->qqnumber),  0L, Kopete::Account::ChangeKABC ); 
    }


    LwqqBuddy* buddy;
    LwqqGroup* group;
    qq_account* ac = lc->data;
    lwdb_userdb_begin(ac->db);
    LIST_FOREACH(buddy,&lc->friends,entries) {
        if(buddy->last_modify == -1 || buddy->last_modify == 0){
            lwdb_userdb_insert_buddy_info(ac->db, buddy);
            friend_come(lc, buddy);
        }
    }
    LIST_FOREACH(group,&lc->groups,entries){
        if(group->last_modify == -1 || group->last_modify == 0){
            lwdb_userdb_insert_group_info(ac->db, group);
            group_come(lc,group);
        }
    }
    lwdb_userdb_commit(ac->db);
*/
    //initCategory();
    LwqqBuddy* buddy;
    LwqqGroup* group;
    qq_account* ac = lc->data;
    lwdb_userdb_begin(ac->db);
    LIST_FOREACH(buddy,&lc->friends,entries) {
        if(buddy->last_modify == -1 || buddy->last_modify == 0){
            lwdb_userdb_insert_buddy_info(ac->db, buddy);
            friend_come(lc, buddy);
        }
    }
    LIST_FOREACH(group,&lc->groups,entries){
        if(group->last_modify == -1 || group->last_modify == 0){
            lwdb_userdb_insert_group_info(ac->db, group);
            group_come(lc,group);
        }
    }
    lwdb_userdb_commit(ac->db);
    myself()->setOnlineStatus( m_targetStatus );
}




void WebqqAccount::setStatusMessage(const Kopete::StatusMessage& statusMessage)
{
	Q_UNUSED(statusMessage);
	/* Not used in webqq */
}

#if 0
void WebqqAccount::connect( const Kopete::OnlineStatus& /* initialStatus */ )
{
	kDebug ( 14210 ) ;
	//myself()->setOnlineStatus( WebqqProtocol::protocol()->webqqOnline );
	login();
	QObject::connect ( m_server, SIGNAL (messageReceived(QString)),
			this, SLOT (receivedMessage(QString)) );
}
#endif

void WebqqAccount::connectWithPassword( const QString &passwd )
{
	
	if ( isAway() )
	{
		slotGoOnline();
		return;
	}

	if ( isConnected() ||
	     myself()->onlineStatus() == m_protocol->WebqqConnecting )
	{
		return;

	}
	
	/*
	 * Fixed Bug: first login after create account
	 * 
	 */
	if (m_targetStatus == Kopete::OnlineStatus::Offline)
	{
	  m_targetStatus = Kopete::OnlineStatus::Online;
	  slotGoOnline();
	  return;
	}

	if ( passwd.isNull() )
	{ //cancel the connection attempt
		static_cast<WebqqContact*>( myself() )->setOnlineStatus( m_protocol->WebqqOffline );
		return;
	}

	
	static_cast<WebqqContact *>( myself() )->setOnlineStatus( m_protocol->WebqqConnecting );
	
#if 0
	/*first clean all contact list*/
	QHashIterator<QString, Kopete::Contact*>itr( contacts() );
	for ( ; itr.hasNext(); ) {
		itr.next();
		Kopete::DeleteContactTask *deleteTask = new Kopete::DeleteContactTask(itr.value());
		deleteTask->start();
	}
	
#endif   

	/*
	 * !Urgly, 
	 *change lwqqclient username and password
	*/
	if(m_lc == NULL)
	    initLwqqAccount();
	
	s_free(m_lc->username);
	s_free(m_lc->password);
	m_lc->username = s_strdup(accountId().toAscii().constData());
	m_lc->password = s_strdup(passwd.toAscii().constData());

	/*first get the connect type*/
	LwqqStatus targetLwqqStatus;
	if (m_targetStatus == Kopete::OnlineStatus::Online)
	{
	  targetLwqqStatus = LWQQ_STATUS_ONLINE;
	}
	else if(m_targetStatus == Kopete::OnlineStatus::Away)
	{
	  targetLwqqStatus = LWQQ_STATUS_AWAY;
	}
	else if(m_targetStatus == Kopete::OnlineStatus::Busy)
	  targetLwqqStatus = LWQQ_STATUS_BUSY;
	else if(m_targetStatus == Kopete::OnlineStatus::Invisible)
	  targetLwqqStatus = LWQQ_STATUS_HIDDEN;
	else
	{
	  targetLwqqStatus = LWQQ_STATUS_ONLINE;
	}
	
	/*try to connect to lwqq*/
    lwqq_get_http_handle(m_lc)->ssl = 0;
	LwqqErrorCode err;
    lwqq_login(m_lc, targetLwqqStatus, NULL);
	
}

bool WebqqAccount::isOffline()
{
    if(m_targetStatus == Kopete::OnlineStatus::Offline)
        return true;
    else
        return false;
}

void WebqqAccount::disconnect()
{
	kDebug ( 14210 ) ;
	//logout();
	
	//pollTimer->stop();
	//QObject::disconnect(pollTimer, 0, 0, 0);
	destoryLwqqAccount();/*destory this lwqq client*/
	
	myself()->setOnlineStatus( m_protocol->WebqqOffline );

}


void WebqqAccount::slotGoOnline ()
{
	kDebug ( 14210 ) ;
	if (!isConnected ())
	{
		connect ();
	}
	else
	{
		myself()->setOnlineStatus( m_protocol->WebqqOnline );
		lwqq_info_change_status(m_lc, LWQQ_STATUS_ONLINE);
	}
	//updateContactStatus();
}

void WebqqAccount::slotGoAway ()
{
	kDebug ( 14210 ) ;
	

	if (!isConnected ())		
		connect();
	else
	{
	    myself()->setOnlineStatus( m_protocol->WebqqAway );
	    lwqq_info_change_status(m_lc, LWQQ_STATUS_AWAY);
	}
	//updateContactStatus();
}

void WebqqAccount::slotGoHidden()
{
  	kDebug ( 14210 ) ;

	if (!isConnected ())
		connect();
	else
	{
	  myself()->setOnlineStatus( m_protocol->WebqqHidden );
	  lwqq_info_change_status(m_lc, LWQQ_STATUS_HIDDEN);
	}
}


void WebqqAccount::slotGoBusy ()
{
	kDebug ( 14210 ) ;

	if (!isConnected ())
		connect();
	else
	{
	  myself()->setOnlineStatus( m_protocol->WebqqBusy );
	  lwqq_info_change_status(m_lc, LWQQ_STATUS_BUSY);
	}
	//updateContactStatus();
}


void WebqqAccount::slotGoOffline ()
{
	kDebug ( 14210 ) ;

	if (isConnected ())
		disconnect ();
	else
	  updateContactStatus();
}

void WebqqAccount::slotShowVideo ()
{
	kDebug ( 14210 ) ;

	if (isConnected ())
	{
		//WebqqWebcamDialog *webqqWebcamDialog = new WebqqWebcamDialog(0, 0);
		//Q_UNUSED(webqqWebcamDialog);
	}
	//updateContactStatus();
}

void WebqqAccount::receivedMessage( const QString &message )
{
	// Look up the contact the message is from
	QString from;
	WebqqContact* messageSender;
 
	from = message.section( ':', 0, 0 );
	Kopete::Contact* contact = contacts().value(from);
	messageSender = dynamic_cast<WebqqContact *>( contact );

	kWarning( 14210 ) << " got a message from " << from << ", " << messageSender << ", is: " << message;
	// Pass it on to the contact to pocess and display via a KMM
	if ( messageSender )
		messageSender->receivedMessage( message );
	else
		kWarning(14210) << "unable to look up contact for delivery";
}

void WebqqAccount::updateContactStatus()
{
	QHashIterator<QString, Kopete::Contact*>itr( contacts() );
	for ( ; itr.hasNext(); ) {
		itr.next();
		itr.value()->setOnlineStatus( myself()->onlineStatus() );
	}
}

void WebqqAccount::find_add_contact(const QString name, Find_Type type, QString groupName)
{
    m_groupName = groupName;
    m_contactName = name;
    if(type == Buddy)
    {
        qDebug()<<"add buddy";
        qq_add_buddy(m_lc, name.toUtf8().constData(), NULL);
    }else if(type == Group)
    {

    }
}
//clean all contacts and groups
void WebqqAccount::cleanAll_contacts()
{
    QList<Kopete::Group*> groupList = Kopete::ContactList::self()->groups();
    QList<Kopete::MetaContact *> metaContactList =  Kopete::ContactList::self()->metaContacts();
    foreach ( Kopete::MetaContact* metaContact, metaContactList )
        Kopete::ContactList::self()->removeMetaContact( metaContact );

    foreach ( Kopete::Group* group, groupList )
        Kopete::ContactList::self()->removeGroup( group );

}

void WebqqAccount::ac_show_confirm_table(LwqqClient *lc, LwqqConfirmTable *table, add_info *info)
{
    qDebug()<<"ac_show_confirm_table";
    m_addInfo = info;
    if(table->body){
        ShowGetInfoDialog *dlg = new ShowGetInfoDialog();
        dlg->setAddInfo(QString::fromUtf8(table->body));
        dlg->exec();
        if(dlg->okOrCancle() == "OK")
            confirm_table_yes(table, NULL, 0);
        else
            confirm_table_no(table, NULL);
        delete dlg;

    }
    if(table->input_label){
        ShowGetInfoDialog *dlg = new ShowGetInfoDialog();
        dlg->setVerifify();
        dlg->exec();
        QString input = dlg->getVerificationString();
        if(dlg->okOrCancle() == "OK")
            confirm_table_yes(table, input.toUtf8().constData(), 0);
        else
            confirm_table_no(table, input.toUtf8().constData());
        delete dlg;
    }
    if(table->exans_label||table->flags&LWQQ_CT_ENABLE_IGNORE||table->flags&LWQQ_CT_CHOICE_MODE){
        ShowGetInfoDialog *dlg = new ShowGetInfoDialog();
        dlg->setRequired(QString::fromUtf8(table->body));
        dlg->exec();
        QString input = dlg->getVerificationString();
        if(dlg->okOrCancle() == "OK")
            confirm_table_yes(table, input.toUtf8().constData(), dlg->webqqAnswer());
        else
            confirm_table_no(table, input.toUtf8().constData());
        delete dlg;
    }
}

void WebqqAccount::ac_show_messageBox(msg_type type, const char *title, const char *message)
{
    if(type == MSG_INFO)
    {
        KMessageBox::information(Kopete::UI::Global::mainWidget(), i18n(message), i18n(title));
    }else if(type == MSG_ERROR)
    {
        KMessageBox::error(Kopete::UI::Global::mainWidget(), i18n(message), i18n(title));
    }else if(type == MSG_ADD)
    {
        KMessageBox::information(Kopete::UI::Global::mainWidget(), i18n(message), i18n(title));
        qDebug()<<"add buddy!!!!";
        QList<Kopete::Group*> groupList = Kopete::ContactList::self()->groups();
        Kopete::Group *g;
        Kopete::Group *targetGroup = NULL;

        foreach(g, groupList)
        {
            if(g->displayName() == m_groupName)
            {
                targetGroup = g;
                break;
            }
        }
        qDebug()<<"start add";
        LwqqBuddy* buddy = lwqq_buddy_new();
        buddy->qqnumber = m_addInfo->qq;
        buddy->nick = m_addInfo->name;
        buddy->uin = m_addInfo->uin;
        qDebug()<<"qq:"<<QString::fromUtf8(m_addInfo->qq)<<"name:"<<QString::fromUtf8(m_addInfo->name)<<"uin"<<QString::fromUtf8(m_addInfo->uin);
        LwqqAsyncEvset* set = lwqq_async_evset_new();
        LwqqAsyncEvent* ev;
        ev = lwqq_info_get_friend_avatar(m_lc,buddy);
        lwqq_async_evset_add_event(set,ev);
        ev = lwqq_info_get_friend_detail_info(m_lc, buddy);/*get detail of friend*/
        lwqq_async_evset_add_event(set,ev);
        lwqq_async_add_evset_listener(set,_C_(2p,cb_friend_avatar, m_lc, buddy));
        qDebug()<<"lwqq_async_add_evset_listener";
        addContact(QString::fromUtf8(m_addInfo->qq), QString::fromUtf8(m_addInfo->name), targetGroup, Kopete::Account::ChangeKABC);
        qDebug()<<"addContact";
        WebqqContact * addedContact = dynamic_cast<WebqqContact *>(contacts().value( QString(buddy->qqnumber) ));
        addedContact->setContactType(WebqqContact::Contact_Chat);
        LIST_INSERT_HEAD(&m_lc->friends,buddy,entries);
        lwdb_userdb_insert_buddy_info(((qq_account*)(m_lc->data))->db, buddy);
        qDebug()<<"edn";
    }
}

static void cb_need_verify2(LwqqClient* lc,LwqqVerifyCode** code)
{
  printf("need verify2\n");
  CallbackObject cb;
  cb.fun_t = NEED_VERIFY2;
  cb.ptr1 = (void *)lc;
  cb.ptr2 = (void *)(*code);
  ObjectInstance::instance()->callSignal(cb);
}

static void cb_login_stage_1(LwqqClient* lc,LwqqErrorCode* err)
{
  printf("stage_1\n");
  CallbackObject cb;
  cb.fun_t = LOGIN_COMPLETE;
  cb.ptr1 = (void *)lc;
  cb.ptr2= (void *)err;
  ObjectInstance::instance()->callSignal(cb);
} 

static void cb_login_stage_2(LwqqAsyncEvent* event,LwqqClient* lc)
{
  CallbackObject cb;
  cb.fun_t = LOGIN_STAGE_2;
  cb.ptr1 = (void *)lc;
  cb.ptr2 = (void *)event;
  ObjectInstance::instance()->callSignal(cb);
}

static void cb_login_stage_3(LwqqClient* lc)
{
  printf("cb_login_stage_3\n");
  CallbackObject cb;
  cb.fun_t = LOGIN_STAGE_3;
  cb.ptr1 = (void *)lc;
  ObjectInstance::instance()->callSignal(cb);
}

void cb_login_stage_f(LwqqClient* lc)
{
  CallbackObject cb;
  cb.fun_t = LOGIN_STAGE_f;
  cb.ptr1 = (void *)lc;
  ObjectInstance::instance()->callSignal(cb);
}


void cb_friend_avatar(LwqqClient* lc,LwqqBuddy* buddy)
{
    CallbackObject cb;
    cb.fun_t = FRIEND_AVATAR;
    cb.ptr1 = (void *)lc;
    cb.ptr2 = (void*)buddy;
    ObjectInstance::instance()->callSignal(cb);
}

void cb_group_avatar(LwqqClient *lc, LwqqGroup *group)
{
    CallbackObject cb;
    cb.fun_t = GROUP_AVATAR;
    cb.ptr1 = (void *)lc;
    cb.ptr2 = (void*)group;
    ObjectInstance::instance()->callSignal(cb);
}

void cb_group_members(LwqqClient *lc, LwqqGroup *group)
{
    CallbackObject cb;
    cb.fun_t = GROUP_MEMBERS;
    cb.ptr1 = (void *)lc;
    cb.ptr2 = (void*)group;
    ObjectInstance::instance()->callSignal(cb);
}

void cb_qq_msg_check(LwqqClient* lc)
{
    CallbackObject cb;
    cb.fun_t = QQ_MSG_CHECK;
    cb.ptr1 = (void *)lc;
    ObjectInstance::instance()->callSignal(cb);
}

void cb_show_confirm_table(LwqqClient *lc, LwqqConfirmTable *table, add_info *info)
{
    CallbackObject cb;
    cb.fun_t = SHOW_CONFIRM;
    cb.ptr1 = (void *)lc;
    cb.ptr2 = (void*)table;
    cb.ptr3 = (void*)info;
    ObjectInstance::instance()->callSignal(cb);
}

void cb_show_messageBox(msg_type type, const char *title, const char *message)
{
    CallbackObject cb;
    cb.fun_t = SHOW_MESSAGE;
    cb.ptr1 = (void *)title;
    cb.ptr2 = (void*)message;
    cb.type = type;
    ObjectInstance::instance()->callSignal(cb);
}

static void verify_required_confirm(LwqqClient* lc,char* account,LwqqConfirmTable* ct)
{
    if(ct->answer == LWQQ_NO)
        lwqq_info_answer_request_friend(lc, account, ct->answer, ct->input);
    else if(ct->answer == LWQQ_IGNORE){
        //ignore it.
    }else
        lwqq_info_answer_request_friend(lc, account, ct->answer, NULL);
    lwqq_ct_free(ct);
    s_free(account);
}



static char* hash_with_local_file(const char* uin,const char* ptwebqq,void* js)
{
    char path[512];
    qq_jso_t* obj = qq_js_load(js,LOCAL_HASH_JS(path));
    char* res = qq_js_hash(uin, ptwebqq, js);
    qq_js_unload(js, obj);
    return res;
}

static char* hash_with_remote_file(const char* uin,const char* ptwebqq,void* js)
{
    //github.com is too slow
    qq_download("http://pidginlwqq.sinaapp.com/hash.js", 
            "hash.js", lwdb_get_config_dir());
    return hash_with_local_file(uin, ptwebqq, js);
}

static void friends_valid_hash(LwqqAsyncEvent* ev,LwqqHashFunc last_hash)
{
    printf("friends_valid_hash\n");
    LwqqClient* lc = ev->lc;
    qq_account* ac = lc->data;
    if(ev->result == LWQQ_EC_HASH_WRONG){
        if(last_hash == hash_with_local_file){
            get_friends_info_retry(lc, hash_with_remote_file);
        }else{
	  /*
            purple_connection_error_reason(ac->gc, 
                    PURPLE_CONNECTION_ERROR_OTHER_ERROR, 
                    _("Hash Function Wrong, WebQQ Protocol update"));*/
        }
        return;
    }
    if(ev->result != LWQQ_EC_OK){
      printf("Get friend list failed\n");
      /*
        purple_connection_error_reason(ac->gc, 
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, 
                _("Get Friend List Failed"));*/
        return;
    }
    LwqqAsyncEvent* event;
    event = lwqq_info_get_group_name_list(lc,NULL);
    lwqq_async_add_event_listener(event,_C_(2p,cb_login_stage_2,event,lc));
}

static void get_friends_info_retry(LwqqClient* lc,LwqqHashFunc hashtry)
{
    printf("get_friends_info_retry\n");
    LwqqAsyncEvent* ev;
    qq_account* ac = lc->data;
    ev = lwqq_info_get_friends_info(lc,hashtry,ac->js);
    lwqq_async_add_event_listener(ev, _C_(2p,friends_valid_hash,ev,hashtry));
    printf("has called get_friends_info_retry\n");
}

//find buddy and group ,add they
#if 1

static void confirm_table_yes(LwqqConfirmTable* table,const char *input, LwqqAnswer answer)
{
    if(table->exans_label){
       table->answer = answer;
    }else
        table->answer = LWQQ_YES;
    if(table->input_label){
        table->input = s_strdup(input);
    }
    vp_do(table->cmd,NULL);
}
static void confirm_table_no(LwqqConfirmTable* table,const char *input)
{
    table->answer = (table->flags&LWQQ_CT_ENABLE_IGNORE)?LWQQ_IGNORE:LWQQ_NO;
    if(table->input_label){
        table->input = s_strdup(input);
    }
    vp_do(table->cmd,NULL);
}


static void add_friend_receipt(LwqqAsyncEvent* ev)
{
    int err = ev->result;
    LwqqClient* lc = ev->lc;
    if(err == 6 ){
        cb_show_messageBox(MSG_ERROR, s_strdup("Error Message"), s_strdup("ErrCode:6\nPlease try agagin later\n"));
        //purple_notify_message(ac->gc,PURPLE_NOTIFY_MSG_INFO,_("Error Message"),_("ErrCode:6\nPlease try agagin later\n"),NULL,NULL,NULL);
    }
}

static void add_friend_ask_message(LwqqClient* lc,LwqqConfirmTable* ask,LwqqBuddy* b)
{
    add_friend(lc,ask,b,s_strdup(ask->input));
}

static void add_friend(LwqqClient* lc,LwqqConfirmTable* c,LwqqBuddy* b,char* message)
{
    if(c->answer == LWQQ_NO){
        goto done;
    }
    if(message==NULL){
        LwqqConfirmTable* ask = s_malloc0(sizeof(*ask));
        ask->input_label = s_strdup(_("Invite Message"));
        ask->cmd = _C_(3p,add_friend_ask_message,lc,ask,b);
        add_info *info = s_malloc0(sizeof(*info));
        info->qq = s_strdup(b->qqnumber);
        info->name = s_strdup(b->nick);
        info->uin = s_strdup(b->uin);
        cb_show_confirm_table(lc, ask, info);
        lwqq_ct_free(c);
        return ;
    }else{
        LwqqAsyncEvent* ev = lwqq_info_add_friend(lc, b,message);
        lwqq_async_add_event_listener(ev, _C_(p,add_friend_receipt,ev));
    }
done:
    lwqq_ct_free(c);
    lwqq_buddy_free(b);
    s_free(message);
}


static void format_body_from_buddy(char* body,size_t buf_len,LwqqBuddy* buddy)
{
    char body_[1024] = {0};
#define ADD_INFO(k,v)  if(v) format_append(body_,"%s:%s\n",k,v)
    ADD_INFO(_("QQ"), buddy->qqnumber);
    ADD_INFO(_("Nick"), buddy->nick);
    ADD_INFO(_("Longnick"), buddy->personal);
    ADD_INFO(_("Gender"), qq_gender_to_str(buddy->gender));
    ADD_INFO(_("Shengxiao"), qq_shengxiao_to_str(buddy->shengxiao));
    ADD_INFO(_("Constellation"), qq_constel_to_str(buddy->constel));
    ADD_INFO(_("Blood Type"), qq_blood_to_str(buddy->blood));
    ADD_INFO(_("Birthday"),lwqq_date_to_str(buddy->birthday));
    ADD_INFO(_("Country"), buddy->country);
    ADD_INFO(_("Province"), buddy->province);
    ADD_INFO(_("City"), buddy->city);
    ADD_INFO(_("Phone"), buddy->phone);
    ADD_INFO(_("Mobile"), buddy->mobile);
    ADD_INFO(_("Email"), buddy->email);
    ADD_INFO(_("Occupation"), buddy->occupation);
    ADD_INFO(_("College"), buddy->college);
    ADD_INFO(_("Homepage"), buddy->homepage);
#undef ADD_INFO
    strncat(body,body_,buf_len-strlen(body));
}

static void system_message(LwqqClient* lc,LwqqMsgSystem* system,LwqqBuddy* buddy)
{
    qq_account* ac = lwqq_client_userdata(lc);
    char buf1[256]={0};
    if(system->type == LwqqMsgSystem::VERIFY_REQUIRED) {
        char buf2[2048];
        LwqqConfirmTable* ct = s_malloc0(sizeof(*ct));
        ct->title = s_strdup(_("Friend Confirm"));
        snprintf(buf2,sizeof(buf2),
                _("%s\nRequest as your friend\nAdditional Reason:%s\n\n"),system->account,system->verify_required.msg);
        format_body_from_buddy(buf2,sizeof(buf2),buddy);
        ct->body = s_strdup(buf2);
        ct->exans_label = s_strdup(_("Agree and add back"));
        ct->input_label = s_strdup(_("Refuse reason"));
        ct->flags = LWQQ_CT_ENABLE_IGNORE;
        ct->cmd = _C_(3p,verify_required_confirm,lc,s_strdup(system->account),ct);
        add_info *info = s_malloc0(sizeof(*info));
        info->qq = s_strdup(buddy->qqnumber);
        info->name = s_strdup(buddy->nick);
        info->uin = s_strdup(buddy->uin);
        cb_show_confirm_table(lc, ct, info);
        lwqq_buddy_free(buddy);
        lwqq_msg_free((LwqqMsg*)system);

    } else if(system->type == LwqqMsgSystem::VERIFY_PASS_ADD) {
        snprintf(buf1,sizeof(buf1),_("%s accept your request,and add back you as friend too"),system->account);
        //purple_notify_message(ac->gc,PURPLE_NOTIFY_MSG_INFO,_("System Message"),_("Add Friend"),buf1,NULL,NULL);
        cb_show_messageBox(MSG_ADD, s_strdup("Add Friend"), s_strdup(buf1));
    } else if(system->type == LwqqMsgSystem::VERIFY_PASS) {
        snprintf(buf1,sizeof(buf1),_("%s accept your request"),system->account);
        cb_show_messageBox(MSG_INFO, s_strdup("Add Friend"), s_strdup(buf1));
        //purple_notify_message(ac->gc,PURPLE_NOTIFY_MSG_INFO,_("System Message"),_("Add Friend"),buf1,NULL,NULL);
    }
}

static void search_buddy_receipt(LwqqAsyncEvent* ev,LwqqBuddy* buddy,char* uni_id,char* message)
{
    int err = ev->result;
    LwqqClient* lc = ev->lc;
    LwqqConfirmTable* confirm = s_malloc0(sizeof(*confirm));
    add_info *info = s_malloc0(sizeof(*info));
    char body[1024] = {0};
    //captcha wrong
    if(err == 10000){
        LwqqAsyncEvent* event = lwqq_info_search_friend(lc,uni_id,buddy);
        lwqq_async_add_event_listener(event, _C_(4p,search_buddy_receipt,event,buddy,uni_id,message));
        return;
    }
    if(err == LWQQ_EC_NO_RESULT){
        cb_show_messageBox(MSG_ERROR, s_strdup("Error Message"), s_strdup("Account not exists or not main display account"));
        //purple_notify_message(ac->gc,PURPLE_NOTIFY_MSG_INFO,_("Error Message"),_("Account not exists or not main display account"),NULL,NULL,NULL);
        goto failed;
    }
    if(!buddy->token){
        cb_show_messageBox(MSG_ERROR, s_strdup("Error Message"), s_strdup("Get friend infomation failed"));
        //purple_notify_message(ac->gc,PURPLE_NOTIFY_MSG_INFO,_("Error Message"),_("Get friend infomation failed"),NULL,NULL,NULL);
        goto failed;
    }

    confirm->title = s_strdup(_("Friend Confirm"));

    format_body_from_buddy(body,sizeof(body),buddy);
    confirm->body = s_strdup(body);
    confirm->cmd = _C_(4p,add_friend,lc,confirm,buddy,message);
    info->qq = s_strdup(buddy->qqnumber);
    info->name = s_strdup(buddy->nick);
    info->uin = s_strdup(buddy->uin);
    cb_show_confirm_table(lc,confirm, info);
    s_free(uni_id);
    return;
failed:
    lwqq_buddy_free(buddy);
    s_free(message);
    s_free(uni_id);
}

void qq_add_buddy( LwqqClient* lc, const char *username, const char *message)
{
    const char* uni_id = username;
    LwqqGroup* g = NULL;
    LwqqBuddy* f_buddy = lwqq_buddy_new();
    LwqqFriendCategory* cate = NULL;
    if(cate == NULL){
        f_buddy->cate_index = 0;
    }else
        f_buddy->cate_index = cate->index;
        //friend->qqnumber = s_strdup(qqnum);
        LwqqAsyncEvent* ev = lwqq_info_search_friend(lc,uni_id,f_buddy);
        const char* msg = NULL;
        lwqq_async_add_event_listener(ev, _C_(4p,search_buddy_receipt,ev,f_buddy,s_strdup(uni_id),s_strdup(msg)));

}

static void add_group_receipt(LwqqAsyncEvent* ev,LwqqGroup* g)
{
    int err = ev->result;
    LwqqClient* lc = ev->lc;
    qq_account* ac = lc->data;
    if(err == 6 ){
        cb_show_messageBox(MSG_ERROR, s_strdup("Error Message"), s_strdup("ErrCode:6\nPlease try agagin later\n"));
        //purple_notify_message(ac->gc,PURPLE_NOTIFY_MSG_INFO,_("Error Message"),_("ErrCode:6\nPlease try agagin later\n"),NULL,NULL,NULL);
    }
    lwqq_group_free(g);
}
static void add_group(LwqqClient* lc,LwqqConfirmTable* c,LwqqGroup* g)
{
    LwqqAsyncEvent* ev = lwqq_info_add_group(lc, g, c->input);
    if(c->answer == LWQQ_NO){
        goto done;
    }

    lwqq_async_add_event_listener(ev, _C_(2p,add_group_receipt,ev,g));
done:
    lwqq_ct_free(c);
}
static void search_group_receipt(LwqqAsyncEvent* ev,LwqqGroup* g)
{
    int err = ev->result;
    LwqqClient* lc = ev->lc;
    qq_account* ac = lc->data;
    if(err == 10000){
        LwqqAsyncEvent* event = lwqq_info_search_group_by_qq(lc,g->qq,g);
        lwqq_async_add_event_listener(event, _C_(2p,search_group_receipt,ev,g));
        return;
    }
    if(err == LWQQ_EC_NO_RESULT){
        cb_show_messageBox(MSG_ERROR, s_strdup("Error Message"), s_strdup("Get QQ Group Infomation Failed"));
        //purple_notify_message(ac->gc,PURPLE_NOTIFY_MSG_INFO,_("Error Message"),_("Get QQ Group Infomation Failed"),NULL,NULL,NULL);
        lwqq_group_free(g);
        return;
    }
    LwqqConfirmTable* confirm = s_malloc0(sizeof(*confirm));
    confirm->title = s_strdup(_("Confirm QQ Group"));
    confirm->input_label = s_strdup(_("Additional Reason"));
    char body[1024] = {0};
#define ADD_INFO(k,v)  format_append(body,_(k":%s\n"),v)
    ADD_INFO("QQ",g->qq);
    ADD_INFO("Name",g->name);
#undef ADD_INFO
    confirm->body = s_strdup(body);
    confirm->cmd = _C_(3p,add_group,lc,confirm,g);
    add_info *info = s_malloc0(sizeof(*info));
    info->qq = s_strdup(g->qq);
    info->name = s_strdup(g->name);
    info->uin = s_strdup(g->gid);
    cb_show_confirm_table(lc,confirm, info);
}

static void search_group(LwqqClient* lc,const char* text)
{
    LwqqGroup* g = lwqq_group_new(LwqqGroup::LWQQ_GROUP_QUN);
    LwqqAsyncEvent* ev;
    ev = lwqq_info_search_group_by_qq(lc, text, g);
    lwqq_async_add_event_listener(ev, _C_(2p,search_group_receipt,ev,g));
}
static void do_no_thing(void* data,const char* text)
{
}
static void qq_add_group(LwqqClient* lc, const char *name)
{
   search_group(lc, name);
}

#endif




static void cb_friend_come(LwqqClient* lc,LwqqBuddy** buddy){printf("********%s\n", __FUNCTION__);}
static void cb_group_come(LwqqClient* lc,LwqqGroup** group){printf("********%s\n", __FUNCTION__);}

static void cb_lost_connection(LwqqClient* lc){printf("********%s\n", __FUNCTION__);}
static void cb_upload_content_fail(LwqqClient* lc,const char** serv_id,LwqqMsgContent** c,int err)
{printf("********%s\n", __FUNCTION__);}
static void cb_delete_group_local(LwqqClient* lc,const LwqqGroup** g){printf("********%s\n", __FUNCTION__);}
static void cb_flush_group_members(LwqqClient* lc,LwqqGroup** g){printf("********%s\n", __FUNCTION__);}


Kopete::OnlineStatus statusFromLwqqStatus(LwqqStatus status)
{
    switch(status) {
	case LWQQ_STATUS_LOGOUT:
	  return WebqqProtocol::protocol()->WebqqLogout;
	case LWQQ_STATUS_ONLINE:
	  return WebqqProtocol::protocol()->WebqqOnline;
	case LWQQ_STATUS_OFFLINE:
	  return WebqqProtocol::protocol()->WebqqOffline;
	case LWQQ_STATUS_AWAY:
	  return WebqqProtocol::protocol()->WebqqAway;
	case LWQQ_STATUS_HIDDEN:
 	  return WebqqProtocol::protocol()->WebqqHidden;
	case LWQQ_STATUS_BUSY:
	  return WebqqProtocol::protocol()->WebqqBusy;
	case LWQQ_STATUS_CALLME:
	  return WebqqProtocol::protocol()->WebqqCallme;
	default:
	  return WebqqProtocol::protocol()->WebqqOffline;
    }
    
}


#include "webqqaccount.moc"
