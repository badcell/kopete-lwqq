/*
    webqqfakeserver.cpp - Kopete Webqq Protocol

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

#include "webqqfakeserver.h"
#include <qtimer.h>
#include <kdebug.h>
#include "webqqincomingmessage.h"


WebqqFakeServer::WebqqFakeServer()
{
}

WebqqFakeServer::~WebqqFakeServer()
{
        qDeleteAll( m_incomingMessages );
}

void WebqqFakeServer::sendMessage( const QString &contactId, const QString &message )
{
	// see what contact the message is for
	// if it's for Echo, respond immediately
	kDebug( 14210 ) << "Message for: " << contactId << ", is: " << message;
	kDebug( 14210 ) << "recipient is echo, coming back at you.";
	// put the message in a map and start a timer to tell it to deliver itself.
	//emit messageReceived( QString::fromLatin1( "echo: " ) + message );
	QString messageId = contactId + QString::fromLatin1(": ");
	WebqqIncomingMessage* msg = new WebqqIncomingMessage( this, messageId + message );
	m_incomingMessages.append( msg );
	QTimer::singleShot( 1000, msg, SLOT(deliver()) );
	
	// This removes any delivered messages 
	purgeMessages();
}

void WebqqFakeServer::incomingMessage( QString message )
{
	emit messageReceived( message );
}

void WebqqFakeServer::purgeMessages()
{
	for ( int i = m_incomingMessages.count() - 1; i >= 0; --i )
	{
		if ( m_incomingMessages[i]->delivered() )
			m_incomingMessages.removeAt(i);
	}
}

#include "webqqfakeserver.moc"
