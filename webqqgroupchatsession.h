/*
    WebqqGroupChatSession.h - Webqq Chat Chatsession

    Copyright (c) 2003 by Duncan Mac-Vicar Prett <duncan@kde.org>
    Copyright (c) 2006 by André Duffeck        <duffeck@kde.org>

    Kopete    (c) 2002-2006 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#ifndef WebqqGroupChatSession_H
#define WebqqGroupChatSession_H

#include "kopetechatsession.h"

class WebqqContact;
class WebqqAccount;

/**
 * @author André Duffeck
 */
class WebqqGroupChatSession : public Kopete::ChatSession
{
	Q_OBJECT
public:
    WebqqGroupChatSession( Kopete::Protocol *protocol, const Kopete::Contact *user, Kopete::ContactPtrList others );
    ~WebqqGroupChatSession();

    void joined( WebqqContact *c);
    void left( WebqqContact *c );
    WebqqAccount *account();
	void setTopic( const QString & topic );	

	void removeAllContacts();
private slots:
    void slotimageContact();
signals:
    void leavingChat( WebqqGroupChatSession *s );
private:
	QString m_topic;
};

#endif

// vim: set noet ts=4 sts=4 tw=4:

