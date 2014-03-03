/*
    webqqaccount.h - Kopete Webqq Protocol

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

#ifndef TESTBEDACCOUNT_H
#define TESTBEDACCOUNT_H


//#include <kopeteaccount.h>
#include <kopetepasswordedaccount.h>
#include "webqqwebcamdialog.h"
#include <QMap>
#include <QList>
#include "webqqbridgecallback.h"
/*include the header of lwqq*/
extern "C"
{
#include "lwqq.h"
#include "lwdb.h"
}
#include "qq_types.h"


class KActionMenu;
namespace Kopete 
{ 
	class Contact;
	class MetaContact;
	class StatusMessage;
    class Message;
}

class WebqqProtocol;
class QByteArray;
class WebqqContact;
typedef QList<group_msg> MsgDataList;
/**
 * This represents an account connected to the webqq
 * @author Will Stephenson
*/
class WebqqAccount : public Kopete::PasswordedAccount
{
	Q_OBJECT
public:
	WebqqAccount( WebqqProtocol *parent, const QString& accountID );
	~WebqqAccount();
	/**
	 * Construct the context menu used for the status bar icon
	 */
	virtual void fillActionMenu( KActionMenu *actionMenu );



	/**
	 * Called when Kopete is set globally away
	 */
	virtual void setAway(bool away, const QString& reason);
	/**
	 * Called when Kopete status is changed globally
	 */
	virtual void setOnlineStatus(const Kopete::OnlineStatus& status , const Kopete::StatusMessage &reason = Kopete::StatusMessage(),
	                             const OnlineStatusOptions& options = None);
	virtual void setStatusMessage(const Kopete::StatusMessage& statusMessage);
	
	
	void initCategory();
    /**
      *
      */
    enum Find_Type{Buddy, Group};
    void find_add_contact(const QString name, Find_Type type ,QString groupName);
	/*
	* instance of LwqqClient
	*/
	LwqqClient *m_lc;

    QString m_groupName;
    QString m_contactName;
    QString m_contactQQ;
    QString m_contactNick;
	/*
	 * got a message, then send it to contact
	 */
	void buddy_message(LwqqClient* lc,LwqqMsgMessage* msg);
    void group_message(LwqqClient* lc,LwqqMsgMessage* msg);
    WebqqContact *contact(const QString &id);
    bool isOffline();
public slots:
	/**
	 * Called by the server when it has a message for us.
	 * This identifies the sending Kopete::Contact and passes it a Kopete::Message
	 */
	void receivedMessage( const QString &message );
	
		 /**
	 * Connect to the WebQQ service
	 */
	virtual void connectWithPassword( const QString & );
	/**
	 * Disconnect from the Yahoo service
	 */
	virtual void disconnect();
	
	void initLwqqAccount();
	void destoryLwqqAccount();
	void friend_come(LwqqClient *lc, LwqqBuddy *buddy);
	void group_come(LwqqClient* lc,LwqqGroup* group);
    void discu_come(LwqqClient* lc,LwqqGroup* group);
    void ac_need_verify2(LwqqClient* lc, LwqqVerifyCode *code);
    void ac_login_stage_1(LwqqClient* lc, LwqqErrorCode *p_err);
	void ac_login_stage_2(LwqqAsyncEvent* event,LwqqClient* lc);
	void ac_login_stage_3(LwqqClient* lc);
	void ac_login_stage_f(LwqqClient* lc);
	void ac_friend_avatar(LwqqClient *lc, LwqqBuddy *buddy);
    void ac_group_avatar(LwqqClient *lc, LwqqGroup *group);
    void ac_group_members(LwqqClient *lc, LwqqGroup *group);
	void ac_qq_msg_check(LwqqClient *lc);
    void ac_show_confirm_table(LwqqClient* lc, LwqqConfirmTable* table, add_info *info);
    void ac_show_messageBox(msg_type type, const char *title, const char *message );
    void ac_friend_come(LwqqClient* lc,LwqqBuddy* b);
    void ac_rewrite_whole_message_list(LwqqAsyncEvent* ev,qq_account* ac,LwqqGroup* group);
	void slotReceivedInstanceSignal(CallbackObject cb);
    void blist_change(LwqqClient* lc,LwqqMsgBlistChange* blist);
	void pollMessage();
	void afterLogin();
    /**
     * get group or disu all members
     */
    void slotGetGroupMembers(QString id);
	
    void slotBlock();
protected:
	/**
	 * This simulates contacts going on and offline in sync with the account's status changes
	 */
	void updateContactStatus();
    /**
     * Creates a protocol specific Kopete::Contact subclass and adds it to the supplie
     * Kopete::MetaContact
     */
    virtual bool createContact(const QString &contactId,  Kopete::MetaContact *parentContact);
    bool createChatSessionContact( const QString &id, const QString &name );
protected slots:
	/**
	 * Change the account's status.  Called by KActions and internally.
	 */
	void slotGoOnline();
	/**
	 * Change the account's status.  Called by KActions and internally.
	 */
	void slotGoAway();
	/**
	 * Change the account's status.  Called by KActions and internally.
	 */
	void slotGoBusy();
	/**
	 * Change the account's status.  Called by KActions and internally.
	 */
	void slotGoOffline();
	/**
	 * Show webcam.  Called by KActions and internally.
	 */
	void slotShowVideo();
	/**
	 * change the account's status, called by Kactions and internally.
	 */
	void slotGoHidden();


private: 
    /*
     *  use lwqq library login
     */
    int login();
    /*
     * use lwqq library logout
     */
    void logout();
    /*
     * init client events
     */
    void init_client_events(LwqqClient* lc);
    /*
     * clean all contact
     */
    void cleanAll_contacts();

    void whisper_message(LwqqClient* lc,LwqqMsgMessage* mmsg);

    void receivedGroupMessage(LwqqGroup* group, LwqqMsgMessage *msg);

    QString stransMsg(const QString &message);

    QString m_username;
    QString m_password;
    
    Kopete::OnlineStatus m_targetStatus; 
    /* lwqq async option */
    LwqqEvents m_async_opt;
    WebqqProtocol *m_protocol;
    
    QTimer *pollTimer;
    QByteArray avatarData;
    add_info *m_addInfo;
    QMap<QString, MsgDataList> m_msgMap;
    /*called by login stage1*/
    //void login_stage_2(LwqqAsyncEvent* ev,LwqqClient* lc);
};

static void cb_need_verify2(LwqqClient* lc, LwqqVerifyCode **code);
static void cb_login_stage_1(LwqqClient* lc, LwqqErrorCode *err);
static void cb_login_stage_2(LwqqAsyncEvent* event,LwqqClient* lc);
static void cb_login_stage_3(LwqqClient* lc);
static void cb_login_stage_f(LwqqClient* lc);
static void cb_friend_avatar(LwqqClient *lc, LwqqBuddy *buddy);
static void cb_group_avatar(LwqqClient *lc, LwqqGroup *group);
static void cb_group_members(LwqqClient *lc, LwqqGroup *group);
static void cb_qq_msg_check(LwqqClient* lc);
static void cb_show_confirm_table(LwqqClient* lc, LwqqConfirmTable* table, add_info *info);
static void cb_show_messageBox(msg_type type, const char *title, const char *message);
static void cb_friend_come(LwqqClient* lc, LwqqBuddy **buddy);
static void cb_group_come(LwqqClient* lc, LwqqGroup **group);
static void cb_lost_connection(LwqqClient* lc);
static void cb_upload_content_fail(LwqqClient* lc, const char **serv_id, LwqqMsgContent **c, int err);
static void cb_delete_group_local(LwqqClient* lc, const LwqqGroup **g);
static void cb_flush_group_members(LwqqClient* lc, LwqqGroup **g);
static void cb_return_friend_come(LwqqClient* lc,LwqqBuddy* b);
static void cb_rewrite_whole_message_list(LwqqAsyncEvent* ev,qq_account* ac,LwqqGroup* group);
static void confirm_table_yes(LwqqConfirmTable* table, const char *input, LwqqAnswer answer);
static void confirm_table_no(LwqqConfirmTable* table,const char *input);
static void system_message(LwqqClient* lc,LwqqMsgSystem* system,LwqqBuddy* buddy);
static char* hash_with_local_file(const char* uin,const char* ptwebqq,void* js);
static char* hash_with_remote_file(const char* uin,const char* ptwebqq,void* js);
static void friends_valid_hash(LwqqAsyncEvent* ev,LwqqHashFunc last_hash);
void qq_add_buddy( LwqqClient* lc, const char *username, const char *message);
static void add_friend(LwqqClient* lc,LwqqConfirmTable* c,LwqqBuddy* b,char* message);
static void get_friends_info_retry(LwqqClient* lc,LwqqHashFunc hashtry);
static void write_buddy_to_db(LwqqClient* lc,LwqqBuddy* b);

Kopete::OnlineStatus statusFromLwqqStatus(LwqqStatus status);


#endif
