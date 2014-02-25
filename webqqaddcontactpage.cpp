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
#include "kopetecontact.h"
#include <kmessagebox.h>
#include <kopeteuiglobal.h>
#include "qq_types.h"
#include "webqqcontact.h"
#include "webqqaccount.h"
static void qq_add_buddy(const char *username, const char *message);
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
        WebqqAccount *acc = dynamic_cast< WebqqAccount *>(a);
        acc->find_add_contact(name, (m_webqqAddUI.m_rbEcho->isChecked() ? WebqqAccount::Buddy : WebqqAccount::Group), m);
//		if ( a->addContact(name, m, Kopete::Account::ChangeKABC ) )
//		{
//			WebqqContact * newContact = qobject_cast<WebqqContact*>( Kopete::ContactList::self()->findContact( a->protocol()->pluginId(), a->accountId(), name ) );
//			if ( newContact )
//			{
//				newContact->setType( m_webqqAddUI.m_rbEcho->isChecked() ? WebqqContact::Echo : WebqqContact::Group );
//				return true;
//			}
//		}
//		else
//			return false;
	}
	return false;
}

bool WebqqAddContactPage::validateData()
{
    if(m_webqqAddUI.m_uniqueName->text().isEmpty())
    {
        QString message = i18n( "Please name ");
        KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Error, message);
        return false;
    }else
        return true;
}


#include "webqqaddcontactpage.moc"
