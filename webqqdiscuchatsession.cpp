/*
    webqqconferencemessagemanager.h - Webqq Conference Message Manager

    Copyright (c) 2003 by Duncan Mac-Vicar <duncan@kde.org>
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

#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <kconfig.h>

#include <kopetecontactaction.h>
#include <kopetecontactlist.h>
#include <kopetecontact.h>
#include <kopetechatsessionmanager.h>
#include <kopeteuiglobal.h>
#include <kicon.h>

#include "webqqdiscuchatsession.h"
#include "webqqcontact.h"
#include "webqqaccount.h"
#include "kopeteprotocol.h"
//#include "webqqinvitelistimpl.h"
#include <kactioncollection.h>

WebqqDiscuChatSession::WebqqDiscuChatSession(Kopete::Protocol *protocol, const Kopete::Contact *user,
	Kopete::ContactPtrList others )
: Kopete::ChatSession( user, others, protocol )
{

	Kopete::ChatSessionManager::self()->registerChatSession( this );
	setComponentData(protocol->componentData());

    //m_webqqRoom = webqqRoom;

//	m_actionInvite = new KAction( KIcon("x-office-contact"), i18n( "&Invite others" ), this ); // icon should probably be "contact-invite", but that doesn't exist... please request an icon on http://techbase.kde.org/index.php?title=Projects/Oxygen/Missing_Icons
//        actionCollection()->addAction( "webqqInvite", m_actionInvite );
//	connect ( m_actionInvite, SIGNAL(triggered(bool)), this, SLOT(slotInviteOthers()) );

    setXMLFile("webqqdiscuui.rc");
}

WebqqDiscuChatSession::~WebqqDiscuChatSession()
{
    //emit leavingConference( this );
}

WebqqAccount *WebqqDiscuChatSession::account()
{
    return static_cast< WebqqAccount *>( Kopete::ChatSession::account() );
}

const QString &WebqqDiscuChatSession::room()
{
    return m_webqqRoom;
}

void WebqqDiscuChatSession::joined( WebqqContact *c )
{
	addContact( c );
}

void WebqqDiscuChatSession::left( WebqqContact *c )
{
	removeContact( c );
}

void WebqqDiscuChatSession::removeAllContacts()
{
    Kopete::ContactPtrList m = members();
    foreach( Kopete::Contact *c, m )
    {
        removeContact( c );
    }
}

void WebqqDiscuChatSession::setTopic( const QString & topic )
{
    setDisplayName(topic);
}

//void WebqqDiscuChatSession::slotMessageSent( Kopete::Message & message, Kopete::ChatSession * )
//{

//    WebqqAccount *acc = dynamic_cast< WebqqAccount *>( account() );
//	if( acc )
//		acc->sendConfMessage( this, message );
//	appendMessage( message );
//	messageSucceeded();
//}

//void WebqqDiscuChatSession::slotInviteOthers()
//{
//	QStringList buddies;

//	QHash<QString, Kopete::Contact*>::ConstIterator it, itEnd = account()->contacts().constEnd();
//	for( it = account()->contacts().constBegin(); it != itEnd; ++it )
//	{
//		if( !members().contains( it.value() ) )
//			buddies.push_back( it.value()->contactId() );
//	}

//    WebqqInviteListImpl *dlg = new WebqqInviteListImpl( Kopete::UI::Global::mainWidget() );
//	QObject::connect( dlg, SIGNAL(readyToInvite(QString,QStringList,QStringList,QString)),
//				account(), SLOT(slotAddInviteConference(QString,QStringList,QStringList,QString)) );
//    dlg->setRoom( m_webqqRoom );
//	dlg->fillFriendList( buddies );
//	for( QList<Kopete::Contact*>::ConstIterator it = members().constBegin(); it != members().constEnd(); ++it )
//		dlg->addParticipant( (*it)->contactId() );
//	dlg->show();
//}

#include "webqqdiscuchatsession.moc"

// vim: set noet ts=4 sts=4 sw=4:

