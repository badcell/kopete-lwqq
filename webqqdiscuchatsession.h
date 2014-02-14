/*
    yahooconferencemessagemanager.h - Webqq Conference Message Manager

    Copyright (c) 2003 by Duncan Mac-Vicar Prett <duncan@kde.org>
    Copyright (c) 2005 by André Duffeck        <duffeck@kde.org>

    Kopete    (c) 2002-2005 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#ifndef WEBQQDISCUCHATSESSION_H
#define WEBQQDISCUCHATSESSION_H

#include "kopetechatsession.h"

class WebqqContact;
class WebqqAccount;

/**
 * @author Duncan Mac-Vicar Prett
 */
class WebqqDiscuChatSession : public Kopete::ChatSession
{
	Q_OBJECT

public:
    WebqqDiscuChatSession(Kopete::Protocol *protocol, const Kopete::Contact *user, Kopete::ContactPtrList others );
    ~WebqqDiscuChatSession();

    void joined( WebqqContact *c );
    void left( WebqqContact *c );
    void removeAllContacts();
	const QString &room();
    WebqqAccount *account();
    void setTopic( const QString & topic );
//signals:
//    void leavingConference( WebqqDiscuChatSession *s );
//protected slots:
//	void slotMessageSent( Kopete::Message &message, Kopete::ChatSession * );
//	void slotInviteOthers();
private:
    QString m_webqqRoom;

//	KAction *m_actionInvite;
};

#endif

// vim: set noet ts=4 sts=4 tw=4:

