/*
    webqqaddcontactpage.cpp - Kopete Webqq add Contact

    Copyright (c) 2014      by Jun Zhang		 <jun.zhang@i-soft.com.cn>
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
#include "kopetegroup.h"
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
        kDebug(WEBQQ_GEN_DEBUG);
		QString name = m_webqqAddUI.m_uniqueName->text();
        WebqqAccount *acc = dynamic_cast< WebqqAccount *>(a);
        QStringList groupNames;
        Kopete::GroupList groupList = m->groups();
        foreach(Kopete::Group *group, groupList)
        {
            if (group->type() == Kopete::Group::Normal)
                groupNames += group->displayName();
            else if (group->type() == Kopete::Group::TopLevel)
                groupNames += QString();
        }
        acc->find_add_contact(name, (m_webqqAddUI.m_rbEcho->isChecked() ? WebqqAccount::Buddy : WebqqAccount::Group), groupNames.at(0));
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
