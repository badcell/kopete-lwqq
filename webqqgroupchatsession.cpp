/*
    WebqqGroupChatSession.cpp - Webqq Chat Chatsession

    Copyright (c) 2003 by Duncan Mac-Vicar <duncan@kde.org>
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

#include <kdebug.h>
#include <klocale.h>
#include <kcomponentdata.h>

#include <kopetecontactlist.h>
#include <kopetecontact.h>
#include <kopetechatsessionmanager.h>
#include <kopeteuiglobal.h>
#include <kopetemessage.h>
#include "kopeteprotocol.h"
#include "webqqgroupchatsession.h"
#include "webqqcontact.h"
#include "webqqaccount.h"

WebqqGroupChatSession::WebqqGroupChatSession( Kopete::Protocol *protocol, const Kopete::Contact *user,
	Kopete::ContactPtrList others )
: Kopete::ChatSession( user, others, protocol )
{
	Kopete::ChatSessionManager::self()->registerChatSession( this );
	setComponentData(protocol->componentData());
    setXMLFile("webqqgroupui.rc");
}

WebqqGroupChatSession::~WebqqGroupChatSession()
{
    //emit leavingChat( this );
}

void WebqqGroupChatSession::removeAllContacts()
{
	Kopete::ContactPtrList m = members();
	foreach( Kopete::Contact *c, m )
	{
		removeContact( c );
	}
}

void WebqqGroupChatSession::setTopic( const QString &topic )
{
    setDisplayName(topic);
}

WebqqAccount *WebqqGroupChatSession::account()
{
    return static_cast< WebqqAccount *>( Kopete::ChatSession::account() );
}

void WebqqGroupChatSession::joined( WebqqContact *c)
{
    addContact(c);
}

void WebqqGroupChatSession::left( WebqqContact *c )
{
	removeContact( c );
}



#include "webqqgroupchatsession.moc"

// vim: set noet ts=4 sts=4 sw=4:

