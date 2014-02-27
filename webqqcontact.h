/*
    webqqcontact.h - Kopete Webqq Protocol

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

#ifndef TESTBEDCONTACT_H
#define TESTBEDCONTACT_H

#include <qmap.h>
//Added by qt3to4:
#include <QList>


#include "kopetecontact.h"
#include "kopetemessage.h"
#include "kopetechatsession.h"
#include "webqqchatsession.h"
#include "webqqgroupchatsession.h"
#include "webqqdiscuchatsession.h"
#include "qq_types.h"
class KAction;
class KActionCollection;
namespace Kopete { class Account; }
namespace Kopete { class ChatSession; }
namespace Kopete { class MetaContact; }

/**
@author Will Stephenson
*/
class WebqqContact : public Kopete::Contact
{
	Q_OBJECT
public:
	/**
	 * The range of possible contact types
	 */
	enum Type { Null, Echo, Group };

	WebqqContact( Kopete::Account* _account, const QString &uniqueName, 
			const QString &displayName, 
			Kopete::MetaContact *parent );

    ~WebqqContact();

    virtual bool isReachable();
	/**
	 * Serialize the contact's data into a key-value map
	 * suitable for writing to a file
	 */
    virtual void serialize(QMap< QString, QString >& serializedData,
			QMap< QString, QString >& addressBookData);
	/**
	 * Return the actions for this contact
	 */
	virtual QList<KAction *> *customContextMenuActions();
	/**
	 * Returns a Kopete::ChatSession associated with this contact
	 */
	virtual Kopete::ChatSession *manager( CanCreateFlags canCreate = CannotCreate );
    WebqqGroupChatSession *groupSession(){
        return m_groupManager;
    }
    WebqqChatSession *chatSession(){
        return m_chatManager;
    }
    WebqqDiscuChatSession *discuSession(){
        return m_discuManager;
    }
	virtual void slotUserInfo();
    void imageContact(const QString &file);
    void buzzContact();
	/**
	 * Set the Type of this contact
	 */
	void setType( Type type );
	/**
	 * set icon of the contact
	 */
	void setDisplayPicture(const QByteArray &data);
	

    int qq_send_chat(const char *gid, const char *message);
    void setContactType(ConType type);
    void webqq_addcontacts(Kopete::Contact *others);
    void set_group_members();
    void set_session_info(const QString &gid, const QString &name);
    void clean_contact();
    static QString prepareMessage(const QString &messageText , const QString &plainText);

public slots:
	/**
	 * Transmits an outgoing message to the server 
	 * Called when the chat window send button has been pressed
	 * (in response to the relevant Kopete::ChatSession signal)
	 */
	void sendMessage( Kopete::Message &message );
	/**
	 * Called when an incoming message arrived
	 * This displays it in the chatwindow
	 */
	void receivedMessage( const QString &message );

    virtual void deleteContact();
protected slots:
	/**
	 * Show the settings dialog
	 */
	void showContactSettings();
	/**
	 * Notify the contact that its current Kopete::ChatSession was
	 * destroyed - probably by the chatwindow being closed
	 */
	void slotChatSessionDestroyed();
private slots:
    void slotTyping( bool );
protected:
    WebqqChatSession* m_chatManager;
    WebqqGroupChatSession* m_groupManager;
    WebqqDiscuChatSession* m_discuManager;
    Kopete::ContactPtrList m_groupMebers;
    bool m_isGroupDestory;
	KActionCollection* m_actionCollection;
	Type m_type;
	KAction* m_actionPrefs;
    ConType m_contactType;
    QString m_displayName;
    QString m_userId;
    QString m_sessionId;
    QString m_sessionName;
};

static int qq_send_im(LwqqClient *lc, const char *who, const char *what, ConType type);

#endif
