/*
    webqqgroupchatsession.cpp - Manages group chats

    Copyright (c) 2014      by Jun Zhang		 <jun.zhang@i-soft.com.cn>
    Copyright (c) 2002      by Duncan Mac-Vicar Prett <duncan@kde.org>
    Copyright (c) 2002      by Daniel Stone           <dstone@kde.org>
    Copyright (c) 2002-2003 by Martijn Klingens       <klingens@kde.org>
    Copyright (c) 2002-2004 by Olivier Goffart        <ogoffart@kde.org>
    Copyright (c) 2003      by Jason Keirstead        <jason@keirstead.org>
    Copyright (c) 2005      by Michaël Larouche      <larouche@kde.org>
    Copyright (c) 2009      by Fabian Rami          <fabian.rami@wowcompany.com>

    Kopete    (c) 2002-2003 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#include <kdebug.h>
#include <klocale.h>
#include <kcomponentdata.h>
#include <kaction.h>
#include <QFileDialog>
#include <QDir>
#include <kopetecontactlist.h>
#include <kopetecontact.h>
#include <kopetechatsessionmanager.h>
#include <kopeteuiglobal.h>
#include <kopetemessage.h>
#include <kactioncollection.h>
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
    KAction *buzzAction = new KAction( KIcon("bell"), i18n( "Buzz Contact" ), this );
        actionCollection()->addAction( "WebqqBuzz", buzzAction );
    //buzzAction->setShortcut( KShortcut("Ctrl+G") );
    //connect( buzzAction, SIGNAL(triggered(bool)), this, SLOT(slotBuzzContact()) );

    KAction *imageAction = new KAction( KIcon("image"), i18n( "Image send" ), this );
        actionCollection()->addAction( "Webqqimage", imageAction );
    //buzzAction->setShortcut( KShortcut("Ctrl+G") );
    connect( imageAction, SIGNAL(triggered(bool)), this, SLOT(slotimageContact()) );

    KAction *userInfoAction = new KAction( KIcon("help-about"), i18n( "Show User Info" ), this );
        actionCollection()->addAction( "WebqqShowInfo",  userInfoAction) ;
    connect( userInfoAction, SIGNAL(triggered(bool)), this, SLOT(slotUserInfo()) );
    setXMLFile("webqqgroupui.rc");
}

WebqqGroupChatSession::~WebqqGroupChatSession()
{
    //emit leavingChat( this );
}

void WebqqGroupChatSession::slotimageContact()
{
    QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"),
                                                     QDir::homePath(),
                                                     tr("Images (*.png *.xpm *.jpg *.gif *.bmp *jpeg)"));
    if(!fileName.isNull())
    {
        QList<Kopete::Contact*>contacts = members();
        static_cast<WebqqContact *>(contacts.first())->imageContact(fileName);
    }
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
    setDisplayName(i18n("%1", topic));
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

