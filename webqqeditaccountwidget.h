/*
    webqqeditaccountwidget.h - Kopete Webqq Protocol

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

#ifndef TESTBEDEDITACCOUNTWIDGET_H
#define TESTBEDEDITACCOUNTWIDGET_H

#include <qwidget.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <editaccountwidget.h>

class QVBoxLayout;
namespace Kopete { class Account; }
namespace Ui { class WebqqAccountPreferences; }

/**
 * A widget for editing this protocol's accounts
 * @author Will Stephenson
*/
class WebqqEditAccountWidget : public QWidget, public KopeteEditAccountWidget
{
Q_OBJECT
public:
    WebqqEditAccountWidget( QWidget* parent, Kopete::Account* account);

    ~WebqqEditAccountWidget();

	/**
	 * Make an account out of the entered data
	 */
	virtual Kopete::Account* apply();
	/**
	 * Is the data correct?
	 */
	virtual bool validateData();
protected:
	Kopete::Account *m_account;
	Ui::WebqqAccountPreferences *m_preferencesWidget;
};

#endif
