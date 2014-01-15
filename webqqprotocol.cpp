/*
    webqqprotocol.cpp - Kopete Webqq Protocol

    Copyright (c) 2003      by Will Stephenson		 <will@stevello.free-online.co.u>
    Kopete    (c) 2002-2003 by the Kopete developers <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/



#include <QList>
#include <kgenericfactory.h>
#include <kdebug.h>

#include "kopeteonlinestatusmanager.h"
#include "kopeteglobal.h"
#include "kopeteproperty.h"
#include "kopeteaccountmanager.h"

#include "webqqaccount.h"
#include "webqqcontact.h"
#include "webqqaddcontactpage.h"
#include "webqqeditaccountwidget.h"

#include "webqqprotocol.h"

K_PLUGIN_FACTORY( WebqqProtocolFactory, registerPlugin<WebqqProtocol>(); )
K_EXPORT_PLUGIN( WebqqProtocolFactory( "kopete_webqq" ) )

WebqqProtocol *WebqqProtocol::s_protocol = 0L;

WebqqProtocol::WebqqProtocol( QObject* parent, const QVariantList &/*args*/ )
	: Kopete::Protocol( WebqqProtocolFactory::componentData(), parent ),
	  WebqqLogout(  Kopete::OnlineStatus::Offline, 25, this, 0,  QStringList(QString()),
			  i18n( "Offline" ),   i18n( "Of&fline" ), Kopete::OnlineStatusManager::Offline ),
	  WebqqOnline(	Kopete::OnlineStatus::Online, 25, this, 0,  QStringList(QString()),
			  i18n( "Online" ),   i18n( "O&nline" ), Kopete::OnlineStatusManager::Online,Kopete::OnlineStatusManager::HasStatusMessage),
	  WebqqOffline(  Kopete::OnlineStatus::Offline, 25, this, 2,  QStringList(QString()),
			  i18n( "Offline" ),   i18n( "Of&fline" ), Kopete::OnlineStatusManager::Offline ),
	  WebqqAway(  Kopete::OnlineStatus::Away, 25, this, 1, QStringList(QLatin1String("away")),
			  i18n( "Away" ),   i18n( "&Away" ), Kopete::OnlineStatusManager::Away ),
	  WebqqBusy(  Kopete::OnlineStatus::Busy, 25, this, 1, QStringList(QLatin1String("Busy")),
			  i18n( "Busy" ),   i18n( "&Busy" ), Kopete::OnlineStatusManager::Busy ), 
	  WebqqHidden(  Kopete::OnlineStatus::Invisible, 25, this, 1, QStringList(QLatin1String("Invisible")),
			  i18n( "Invisible" ),   i18n( "&Invisible" ), Kopete::OnlineStatusManager::Invisible ), 
	  WebqqConnecting( Kopete::OnlineStatus::Connecting,2, this, 555, QStringList(QString::fromUtf8("webqq_connecting")),    
		      i18n( "Connecting" ), i18n("Connecting"), 0, Kopete::OnlineStatusManager::HideFromMenu ),	  
	  iconCheckSum("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),	  
	  propNick("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propLongNick("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propGender("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propBirthday("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propPhone("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propProvince("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propSchool("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propEmail("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propXingzuo("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propCountry("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propCity("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propMobile("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propPage("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propJob("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propPersonal("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty)
{
	kDebug( 14210 ) ;

	s_protocol = this;
}

WebqqProtocol::~WebqqProtocol()
{
}

Kopete::Contact *WebqqProtocol::deserializeContact(
	Kopete::MetaContact *metaContact, const QMap<QString, QString> &serializedData,
	const QMap<QString, QString> &/* addressBookData */)
{
	QString contactId = serializedData[ "contactId" ];
	QString accountId = serializedData[ "accountId" ];
	QString displayName = serializedData[ "displayName" ];
	QString type = serializedData[ "contactType" ];

	WebqqContact::Type tbcType;
	if ( type == QLatin1String( "group" ) )
		tbcType = WebqqContact::Group;
	else if ( type == QLatin1String( "echo" ) )
		tbcType = WebqqContact::Echo;
	else if ( type == QLatin1String( "null" ) )
		tbcType = WebqqContact::Null;
	else
		tbcType = WebqqContact::Null;

	QList<Kopete::Account*> accounts = Kopete::AccountManager::self()->accounts( this );
	Kopete::Account* account = 0;
	foreach( Kopete::Account* acct, accounts )
	{
		if ( acct->accountId() == accountId )
			account = acct;
	}

	if ( !account )
	{
		kDebug(14210) << "Account doesn't exist, skipping";
		return 0;
	}

	WebqqContact * contact = new WebqqContact(account, contactId, displayName, metaContact);
	contact->setType( tbcType );
	return contact;
}

AddContactPage * WebqqProtocol::createAddContactWidget( QWidget *parent, Kopete::Account * /* account */ )
{
	kDebug( 14210 ) << "Creating Add Contact Page";
	return new WebqqAddContactPage( parent );
}

KopeteEditAccountWidget * WebqqProtocol::createEditAccountWidget( Kopete::Account *account, QWidget *parent )
{
	kDebug(14210) << "Creating Edit Account Page";
	return new WebqqEditAccountWidget( parent, account );
}

Kopete::Account *WebqqProtocol::createNewAccount( const QString &accountId )
{
	return new WebqqAccount( this, accountId );
}

WebqqProtocol *WebqqProtocol::protocol()
{
	return s_protocol;
}



#include "webqqprotocol.moc"
