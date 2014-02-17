/*
    webqqcontact.cpp - Kopete Webqq Protocol

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



#include <QList>
#include <QDebug>
#include <QFile>
#include <kaction.h>
#include <kdebug.h>
#include <klocale.h>

#include "kopeteaccount.h"
#include "kopetegroup.h"
#include "kopetechatsession.h"
#include "kopeteonlinestatus.h"
#include "kopetemetacontact.h"
#include "kopetechatsessionmanager.h"
#include "kopeteuiglobal.h"
#include "kopeteview.h"
#include "kopetetransfermanager.h"
#include "kopeteavatarmanager.h"

#include "webqqaccount.h"
#include "webqqfakeserver.h"
#include "webqqprotocol.h"
#include "webqqloginverifywidget.h"
#include "webqquserinfoform.h"

#include "translate.h"

#include "webqqcontact.h"

WebqqContact::WebqqContact( Kopete::Account* _account, const QString &uniqueName,
		const QString &displayName, Kopete::MetaContact *parent )
: Kopete::Contact( _account, uniqueName, parent )
{
	kDebug( 14210 ) << " uniqueName: " << uniqueName << ", displayName: " << displayName;
    m_displayName = displayName;
    m_userId = uniqueName;
	m_type = WebqqContact::Null;
	// FIXME: ? setDisplayName( displayName );
    m_chatManager = 0L;
    m_groupManager = 0L;
    m_discuManager = 0L;
    m_isGroupDestory = false;
	setOnlineStatus( WebqqProtocol::protocol()->WebqqOffline );
}

WebqqContact::~WebqqContact()
{
}

void WebqqContact::setType( WebqqContact::Type type )
{
	m_type = type;
}

bool WebqqContact::isReachable()
{
    return true;
}

void WebqqContact::serialize( QMap< QString, QString > &serializedData, QMap< QString, QString > & /* addressBookData */ )
{
    QString value;
	switch ( m_type )
	{
	case Null:
		value = QLatin1String("null");
	case Echo:
		value = QLatin1String("echo");
	case Group:
		value = QLatin1String("group");
	}
	serializedData[ "contactType" ] = value;
}

Kopete::ChatSession* WebqqContact::manager( CanCreateFlags canCreateFlags )
{
//	kDebug( 14210 ) ;
//	if ( m_msgManager )
//	{
//		return m_msgManager;
//	}
//	else if ( canCreateFlags == CanCreate )
//	{
//		QList<Kopete::Contact*> contacts;
//		contacts.append(this);
//		Kopete::ChatSession::Form form = ( m_type == Group ?
//				  Kopete::ChatSession::Chatroom : Kopete::ChatSession::Small );
//		m_msgManager = Kopete::ChatSessionManager::self()->create(account()->myself(), contacts, protocol(), form );
//		connect(m_msgManager, SIGNAL(messageSent(Kopete::Message&,Kopete::ChatSession*)),
//				this, SLOT(sendMessage(Kopete::Message&)) );
//		connect(m_msgManager, SIGNAL(destroyed()), this, SLOT(slotChatSessionDestroyed()));
//		return m_msgManager;
//	}
//	else
//	{
//		return 0;
//	}
    canCreateFlags = CanCreate;
    if(m_contactType == Contact_Chat)
    {
        if ( m_chatManager )
        {
            return m_chatManager;
        }
        else if ( canCreateFlags == CanCreate )
        {
            Kopete::ContactPtrList contacts;
            contacts.append(this);
            m_chatManager = new WebqqChatSession(protocol(), account()->myself(), contacts);
            connect(m_chatManager, SIGNAL(messageSent(Kopete::Message&,Kopete::ChatSession*)),
                    this, SLOT(sendMessage(Kopete::Message&)) );
            connect( m_chatManager, SIGNAL(myselfTyping(bool)), this, SLOT(slotTyping(bool)) );
            connect(m_chatManager, SIGNAL(destroyed()), this, SLOT(slotChatSessionDestroyed()));
            return m_chatManager;
        }
        else
        {
            return 0;
        }
    }else if(m_contactType == Contact_Group)
    {
        qDebug()<<"group manager";
        if ( m_groupManager )
        {
            qDebug()<<"m_groupManager";
            return m_groupManager;
        }
        else if ( canCreateFlags == CanCreate )
        {
            qDebug()<<"cancreate";
            Kopete::ContactPtrList contacts;
            contacts.append(this);
            m_groupManager = new WebqqGroupChatSession(protocol(), account()->myself(), contacts);
            m_groupManager->removeAllContacts();
            if(m_isGroupDestory)
            {
                foreach( Kopete::Contact *c, m_groupMebers )
                {
                    m_groupManager->joined(c);
                }
            }
            m_groupManager->setTopic(m_displayName);
            connect(m_groupManager, SIGNAL(messageSent(Kopete::Message&,Kopete::ChatSession*)),
                    this, SLOT(sendMessage(Kopete::Message&)) );
            connect(m_groupManager, SIGNAL(destroyed()), this, SLOT(slotChatSessionDestroyed()));
            return m_groupManager;
        }
        else
        {
            qDebug()<<"0";
            return 0;
        }
    }else if(m_contactType == Contact_Discu)
    {
        if ( m_discuManager )
        {
            return m_discuManager;
        }
        else if ( canCreateFlags == CanCreate )
        {
            Kopete::ContactPtrList contacts;
            contacts.append(this);
            m_discuManager = new WebqqDiscuChatSession(protocol(), account()->myself(), contacts);
            connect(m_discuManager, SIGNAL(messageSent(Kopete::Message&,Kopete::ChatSession*)),
                    this, SLOT(sendMessage(Kopete::Message&)) );
            connect(m_discuManager, SIGNAL(destroyed()), this, SLOT(slotChatSessionDestroyed()));
            return m_discuManager;
        }
        else
        {
            return 0;
        }
    }
}

void WebqqContact::webqq_addcontacts(Kopete::Contact *others)
{
    manager(CanCreate)->addContact(others);
}



QList<KAction *> *WebqqContact::customContextMenuActions() //OBSOLETE
{
	//FIXME!!!  this function is obsolete, we should use XMLGUI instead
	/*m_actionCollection = new KActionCollection( this, "userColl" );
	m_actionPrefs = new KAction(i18n( "&Contact Settings" ), 0, this,
			SLOT(showContactSettings()), m_actionCollection, "contactSettings" );

	return m_actionCollection;*/
	return 0L;
}

void WebqqContact::showContactSettings()
{
	//WebqqContactSettings* p = new WebqqContactSettings( this );
	//p->show();
}

void WebqqContact::setDisplayPicture(const QByteArray &data)
{
	//setProperty( WebqqProtocol::protocol()->iconCheckSum, checksum );	
	
	Kopete::AvatarManager::AvatarEntry entry;
	entry.name = contactId();
	entry.category = Kopete::AvatarManager::Contact;
	entry.contact = this;
    entry.image = QImage::fromData(data);
	entry = Kopete::AvatarManager::self()->add(entry);
    //qDebug() <<"setDisplayPicture:"<< entry.dataPath;
	if (!entry.dataPath.isNull())
	{
        this->removeProperty(Kopete::Global::Properties::self()->photo());
        //setProperty( Kopete::Global::Properties::self()->photo(), QString() );
        this->setProperty( Kopete::Global::Properties::self()->photo() , entry.dataPath );
//        this->setIcon(entry.dataPath);
//        this->setUseCustomIcon(true);
        //qDebug() << "datePath:" << entry.dataPath;
		//emit displayPictureChanged();
	}
}


void WebqqContact::slotUserInfo()
{	
	WebqqUserInfoForm *form = new WebqqUserInfoForm(this);
	form->show();
}

void WebqqContact::imageContact(const QString &file)
{
    QString imgFile = QString("<img src=\"%1\"/>").arg(file);
    qDebug()<<"img html:"<<imgFile;
    manager(CanCreate)->setLastUrl(file);
    Kopete::ContactPtrList justMe;
    justMe.append(account()->myself());
    Kopete::Message kmsg(account()->myself(), this);
    kmsg.setHtmlBody(imgFile);
    kmsg.setDirection( Kopete::Message::Inbound);
    if(m_contactType == Contact_Group || m_contactType == Contact_Discu)
        qq_send_chat(m_userId.toUtf8().constData(), imgFile.toUtf8().constData());
    else
        qq_send_im(m_userId.toUtf8().constData(), imgFile.toUtf8().constData());

    // give it back to the manager to display
    manager(CanCreate)->appendMessage( kmsg );
    // tell the manager it was sent successfully
    manager(CanCreate)->messageSucceeded();
}

void WebqqContact::buzzContact()
{

}

void WebqqContact::setContactType(ConType type)
{
    m_contactType = type;
}

static const char* local_id_to_serv(qq_account* ac,const char* local_id)
{
    if(ac->flag & QQ_USE_QQNUM){
        LwqqBuddy* buddy = find_buddy_by_qqnumber(ac->qq,local_id);
        return (buddy&&buddy->uin)?buddy->uin:local_id;
    }else return local_id;
}

void WebqqContact::slotTyping(bool isTyping_ )
{
    if(m_contactType == Contact_Chat)
    {
        if(isTyping_)
        {
            Kopete::ContactPtrList m_them = manager(Kopete::Contact::CanCreate)->members();
            Kopete::Contact *target = m_them.first();
            LwqqClient* lc = ((WebqqAccount*)account())->m_lc;
            qq_account *ac = (qq_account*)(lc->data);
            lwqq_msg_input_notify(ac->qq,local_id_to_serv(ac,(static_cast<WebqqContact*>(target)->m_userId).toUtf8().constData()));
        }
    }
}


void WebqqContact::sendMessage( Kopete::Message &message )
{
	kDebug( 14210 ) ;

	/*this is for test*/
	 qDebug() << "msg:" << message.plainBody();
     qDebug() << "html msg:" << message.getHtmlStyleAttribute();
     qDebug()<<"escapedBody"<<message.escapedBody();
     qDebug()<<"parsedBody" << message.parsedBody();
	//first prepare message
	QString targetQQNumber = message.to().first()->contactId();
    qDebug()<<"member:"<<targetQQNumber<<"userid"<<m_userId;
	QString messageStr = message.plainBody();
    if(m_contactType == Contact_Group || m_contactType == Contact_Discu)
        qq_send_chat(m_userId.toUtf8().constData(), messageStr.toUtf8().constData());
    else
        qq_send_im(targetQQNumber.toUtf8().constData(), messageStr.toUtf8().constData());
	
	// give it back to the manager to display
    manager(CanCreate)->appendMessage( message );
	// tell the manager it was sent successfully
    manager(CanCreate)->messageSucceeded();
}


static void cb_send_receipt(LwqqAsyncEvent* ev,LwqqMsg* msg,char* serv_id,char* what)
{
    printf("[%s] \n", __FUNCTION__);
    qq_account* ac = lwqq_async_event_get_owner(ev)->data; 
    LwqqMsgMessage* mmsg = (LwqqMsgMessage*)msg;
    if(lwqq_async_event_get_code(ev)==LWQQ_CALLBACK_FAILED) 
    {
       s_free(what);
      s_free(serv_id);
      lwqq_msg_free(msg);
    }
    
   

    if(ev == NULL){
        //qq_sys_msg_write(ac,msg->type,serv_id,_("Message body too long"),PURPLE_MESSAGE_ERROR,time(NULL));
    }else{
        int err = lwqq_async_event_get_result(ev);
        static char buf[1024]={0};
        //PurpleConversation* conv = find_conversation(msg->type,serv_id,ac);

        if(err > 0){
            snprintf(buf,sizeof(buf),_("Send failed, err:%d:\n%s"),err,what);
            
	    printf("FUNCTION: %s, %s\n",__FUNCTION__, buf);
	    //qq_sys_msg_write(ac, msg->type, serv_id, buf, PURPLE_MESSAGE_ERROR, time(NULL));
        }
        if(err == LWQQ_EC_LOST_CONN){
            ac->qq->action->poll_lost(ac->qq);
        }
    }
    if(mmsg->upload_retry <0)
    {
        //qq_sys_msg_write(ac, msg->type, serv_id, _("Upload content retry over limit"), PURPLE_MESSAGE_ERROR, time(NULL));
    }
    if(msg->type == LWQQ_MS_GROUP_MSG) 
      mmsg->group.group_code = NULL;
    else if(msg->type == LWQQ_MS_DISCU_MSG) 
      mmsg->discu.did = NULL;
failed:
    s_free(what);
    s_free(serv_id);
    lwqq_msg_free(msg);
}
//add find group by zj
static LwqqGroup* find_group_by_name(LwqqClient* lc,const char* name)
{
    LwqqGroup* group = NULL;
    LIST_FOREACH(group,&lc->groups,entries) {
        if(group->name&&strcmp(group->name,name)==0)
            return group;
    }
    LIST_FOREACH(group,&lc->discus,entries) {
        if(group->name&&strcmp(group->name,name)==0)
            return group;
    }
    return NULL;
}

static LwqqSimpleBuddy* find_group_member_by_nick_or_card(LwqqGroup* group,const char* who)
{
    if(!group || !who ) return NULL;
    LwqqSimpleBuddy* sb;
    LIST_FOREACH(sb,&group->members,entries) {
        if(sb->nick&&strcmp(sb->nick,who)==0)
            return sb;
        if(sb->card&&strcmp(sb->card,who)==0)
            return sb;
    }
    return NULL;
}

static int find_group_and_member_by_card(LwqqClient* lc,const char* card,LwqqGroup** p_g,LwqqSimpleBuddy** p_sb)
{
    if(!card) return 0;
    char nick[128]={0};
    char gname[128]={0};
    const char* pos;
    if((pos = strstr(card," ### "))!=NULL) {
        strcpy(gname,pos+strlen(" ### "));
        strncpy(nick,card,pos-card);
        nick[pos-card] = '\0';
        *p_g = find_group_by_name(lc,gname);
        if(LIST_EMPTY(&(*p_g)->members)){
            return -1;
        }
        *p_sb = find_group_member_by_nick_or_card(*p_g,nick);
        return 1;
    }
    return 0;
}

int WebqqContact::qq_send_im( const char *who, const char *what)
{
    LwqqClient* lc = ((WebqqAccount*)account())->m_lc;
    LwqqMsg* msg;
    LwqqMsgMessage *mmsg;
    qq_account *ac = (qq_account*)(lc->data);
#if QQ_USE_FAST_INDEX
    LwqqGroup* group = NULL;
    LwqqSimpleBuddy* sb = NULL;
    int ret = 0;
    if((ret = find_group_and_member_by_card(lc, who, &group, &sb))){
        if(ret==-1||!sb->group_sig){
            LwqqAsyncEvent* ev = NULL;
            if(ret==-1)//member list is empty
                ev = lwqq_info_get_group_detail_info(lc, group, NULL);
            else if(!sb->group_sig)
                ev = lwqq_info_get_group_sig(lc,group,sb->uin);
            char* who_ = s_strdup(who);
            char* what_ = s_strdup(what);
            lwqq_async_add_event_listener(ev, _C_(3pi,qq_send_im,gc,who_,what_,flags));
            lwqq_async_add_event_listener(ev, _C_(p,free,who_));
            lwqq_async_add_event_listener(ev, _C_(p,free,what_));
            return !send_visual;
        }

        msg = lwqq_msg_new(LWQQ_MS_SESS_MSG);
        mmsg = (LwqqMsgMessage*)msg;

        mmsg->super.to = s_strdup(sb->uin);
        mmsg->sess.group_sig = s_strdup(sb->group_sig);
        mmsg->sess.service_type = group->type;
    } else {
        msg = lwqq_msg_new(LWQQ_MS_BUDDY_MSG);
        mmsg = (LwqqMsgMessage*)msg;
        if(ac->flag&QQ_USE_QQNUM){
            LwqqBuddy* buddy = find_buddy_by_qqnumber(lc,who);
            if(buddy)
                mmsg->super.to = s_strdup(buddy->uin);
            else mmsg->super.to = s_strdup(who);
        }else{
            mmsg->super.to = s_strdup(who);
        }
    }
#else
    msg = lwqq_msg_new(LWQQ_MS_BUDDY_MSG);
    mmsg = (LwqqMsgMessage*)msg;

    LwqqBuddy* buddy = find_buddy_by_qqnumber(lc,who);
    if(buddy)
        mmsg->super.to = s_strdup(buddy->uin);
    else mmsg->super.to = s_strdup(who);
#endif
    mmsg->f_name = s_strdup(ac->font.family);
    mmsg->f_size = ac->font.size;
    mmsg->f_style = ac->font.style;
    strcpy(mmsg->f_color,"000000");

    translate_message_to_struct(NULL, NULL, what, msg, 0);

    LwqqAsyncEvent* ev = lwqq_msg_send(lc,mmsg);
    lwqq_async_add_event_listener(ev,_C_(4p, cb_send_receipt,ev,msg,strdup(who),strdup(what)));

    return 1;
}

int WebqqContact::qq_send_chat(const char *gid, const char *message)
{
    LwqqClient* lc = ((WebqqAccount*)account())->m_lc;
    qq_account *ac = (qq_account*)(lc->data);
    LwqqGroup* group = find_group_by_gid(ac->qq,gid);//??????
    LwqqMsg* msg;

    msg = lwqq_msg_new(LWQQ_MS_GROUP_MSG);
    LwqqMsgMessage *mmsg = (LwqqMsgMessage*)msg;
    mmsg->super.to = s_strdup(group->gid);
    if(group->type == 0 ){
        msg->type = LWQQ_MS_GROUP_MSG;
        mmsg->group.group_code = group->code;
    }else if(group->type == 1 ){
        msg->type = LWQQ_MS_DISCU_MSG;
        mmsg->discu.did = group->did;
    }
    mmsg->f_name = s_strdup(ac->font.family);
    mmsg->f_size = ac->font.size;
    mmsg->f_style = ac->font.style;
    strcpy(mmsg->f_color,"000000");

    translate_message_to_struct(ac->qq, group->gid, message, msg, 1);

    LwqqAsyncEvent* ev = lwqq_msg_send(ac->qq,mmsg);
    lwqq_async_add_event_listener(ev, _C_(4p,cb_send_receipt,ev,msg,s_strdup(group->gid),s_strdup(message)));


    return 1;
}

void WebqqContact::receivedMessage( const QString &message )
{
    qDebug() << "receivedMessage:" << message;
    QDateTime msgDT;
    msgDT.setTime_t(time(0L));
    Kopete::ContactPtrList contactList;
	contactList.append( account()->myself() );
	// Create a Kopete::Message
	Kopete::Message newMessage( this, contactList );
    //newMessage.setPlainBody( message );
    newMessage.setTimestamp( msgDT );
    newMessage.setHtmlBody(message);
	newMessage.setDirection( Kopete::Message::Inbound );

	// Add it to the manager
	manager(CanCreate)->appendMessage (newMessage);
}

void WebqqContact::set_group_members()
{
    m_groupMebers = m_groupManager->members();
}

void WebqqContact::slotChatSessionDestroyed()
{
	//FIXME: the chat window was closed?  Take appropriate steps.
    if(m_contactType == Contact_Group)
    {
        m_groupManager = 0L;
        m_isGroupDestory = true;
    }
    else if(m_contactType == Contact_Discu)
    {

    }
    else if(m_contactType == Contact_Chat)
        m_chatManager = 0L;
}

#include "webqqcontact.moc"

// vim: set noet ts=4 sts=4 sw=4:

