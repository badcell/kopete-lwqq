/*
    webqqaddcontactpage.cpp - Kopete Webqq Protocol

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

#include "webqqaddcontactpage.h"

#include <qlayout.h>
#include <qradiobutton.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <qlineedit.h>
#include <kdebug.h>

#include "kopeteaccount.h"
#include "kopetecontactlist.h"
#include "kopetemetacontact.h"

#include "webqqcontact.h"

WebqqAddContactPage::WebqqAddContactPage( QWidget* parent )
		: AddContactPage(parent)
{
	kDebug(14210) ;
	QVBoxLayout* l = new QVBoxLayout( this );
	QWidget* w = new QWidget();
	m_webqqAddUI.setupUi( w );
	l->addWidget( w );
	m_webqqAddUI.m_uniqueName->setFocus();
}

WebqqAddContactPage::~WebqqAddContactPage()
{
}

bool WebqqAddContactPage::apply( Kopete::Account* a, Kopete::MetaContact* m )
{
	if ( validateData() )
	{
		QString name = m_webqqAddUI.m_uniqueName->text();

		if ( a->addContact(name, m, Kopete::Account::ChangeKABC ) )
		{
			WebqqContact * newContact = qobject_cast<WebqqContact*>( Kopete::ContactList::self()->findContact( a->protocol()->pluginId(), a->accountId(), name ) );
			if ( newContact )
			{
				newContact->setType( m_webqqAddUI.m_rbEcho->isChecked() ? WebqqContact::Echo : WebqqContact::Group );
				return true;
			}
		}
		else
			return false;
	}
	return false;
}

bool WebqqAddContactPage::validateData()
{
    return true;
}


#include "webqqaddcontactpage.moc"
