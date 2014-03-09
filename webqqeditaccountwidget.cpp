/*
    webqqeditaccountwidget.h - Kopete Webqq edit Account

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

#include "webqqeditaccountwidget.h"

#include <qlayout.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <kdebug.h>
#include "kopeteaccount.h"
#include "kopetecontact.h"
#include "ui_webqqaccountpreferences.h"
#include "webqqaccount.h"
#include "webqqprotocol.h"

WebqqEditAccountWidget::WebqqEditAccountWidget( QWidget* parent, Kopete::Account* account)
: QWidget( parent ), KopeteEditAccountWidget( account )
{
	QVBoxLayout *layout = new QVBoxLayout( this );
				kDebug(14210) ;
	QWidget *widget = new QWidget( this );
	m_preferencesWidget = new Ui::WebqqAccountPreferences();
	m_preferencesWidget->setupUi( widget );
	layout->addWidget( widget );
}

WebqqEditAccountWidget::~WebqqEditAccountWidget()
{
	delete m_preferencesWidget;
}

Kopete::Account* WebqqEditAccountWidget::apply()
{
	QString accountName;
	if ( m_preferencesWidget->m_acctName->text().isEmpty() )
		accountName = "Webqq Account";
	else
		accountName = m_preferencesWidget->m_acctName->text();
	
	if ( account() )
		// FIXME: ? account()->setAccountLabel(accountName);
		account()->myself()->setProperty( Kopete::Global::Properties::self()->nickName(), accountName );
	else
		setAccount( new WebqqAccount( WebqqProtocol::protocol(), accountName ) );

	return account();
}

bool WebqqEditAccountWidget::validateData()
{
    //return !( m_preferencesWidget->m_acctName->text().isEmpty() );
	return true;
}

#include "webqqeditaccountwidget.moc"
