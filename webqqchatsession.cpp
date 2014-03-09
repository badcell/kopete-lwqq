/*
    webqqchatsession.cpp - Manages friend and session chats

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

#include "webqqchatsession.h"

#include <qlabel.h>
#include <qimage.h>

#include <qfile.h>
#include <qicon.h>
#include <QFileDialog>
//Added by qt3to4:
#include <QPixmap>
#include <QList>

#include <kconfig.h>
#include <kdebug.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <ktemporaryfile.h>
#include <kxmlguiwindow.h>
#include <ktoolbar.h>
#include <krun.h>
#include <kiconloader.h>
#include <kicon.h>

//#include "kopetecontactaction.h"
//#include "kopetemetacontact.h"
//#include "kopetecontactlist.h"
#include "kopetechatsessionmanager.h"
#include "kopeteprotocol.h"
//#include "kopeteuiglobal.h"
//#include "kopeteglobal.h"
//#include "kopeteview.h"

#include "webqqcontact.h"
#include "webqqaccount.h"
#include <kactioncollection.h>

WebqqChatSession::WebqqChatSession( Kopete::Protocol *protocol, const Kopete::Contact *user,
	Kopete::ContactPtrList others )
: Kopete::ChatSession( user, others, protocol )
{	
    setComponentData(protocol->componentData());
    Kopete::ChatSessionManager::self()->registerChatSession( this );
	// Add Actions
	KAction *buzzAction = new KAction( KIcon("bell"), i18n( "Buzz Contact" ), this );
        actionCollection()->addAction( "WebqqBuzz", buzzAction );
    //buzzAction->setShortcut( KShortcut("Ctrl+G") );
	connect( buzzAction, SIGNAL(triggered(bool)), this, SLOT(slotBuzzContact()) );

    KAction *imageAction = new KAction( KIcon("image"), i18n( "Image send" ), this );
        actionCollection()->addAction( "Webqqimage", imageAction );
    //buzzAction->setShortcut( KShortcut("Ctrl+G") );
    connect( imageAction, SIGNAL(triggered(bool)), this, SLOT(slotimageContact()) );

	KAction *userInfoAction = new KAction( KIcon("help-about"), i18n( "Show User Info" ), this );
        actionCollection()->addAction( "WebqqShowInfo",  userInfoAction) ;
	connect( userInfoAction, SIGNAL(triggered(bool)), this, SLOT(slotUserInfo()) );


//	if(c->hasProperty(Kopete::Global::Properties::self()->photo().key())  )
//	{
//		connect( Kopete::ChatSessionManager::self() , SIGNAL(viewActivated(KopeteView*)) , this, SLOT(slotDisplayPictureChanged()) );
//	}
//	else
//	{
//		m_image = 0L;
//	}

    setXMLFile("webqqchatui.rc");
}

WebqqChatSession::~WebqqChatSession()
{

}

void WebqqChatSession::slotBuzzContact()
{
	QList<Kopete::Contact*>contacts = members();
    static_cast<WebqqContact *>(contacts.first())->buzzContact();
}

void WebqqChatSession::slotimageContact()
{

    QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"),
                                                     "/home",
                                                     tr("Images (*.png *.xpm *.jpg *.gif *.bmp *jpeg)"));
    if(!fileName.isNull())
    {
        QList<Kopete::Contact*>contacts = members();
        static_cast<WebqqContact *>(contacts.first())->imageContact(fileName);
    }
}

void WebqqChatSession::setTopic(const QString &topic)
{
     setDisplayName(i18n("%1", topic));
}

void WebqqChatSession::slotUserInfo()
{
	QList<Kopete::Contact*>contacts = members();
    static_cast<WebqqContact *>(contacts.first())->slotUserInfo();
}


void WebqqChatSession::slotSendFile()
{
	QList<Kopete::Contact*>contacts = members();
    static_cast<WebqqContact *>(contacts.first())->sendFile();
}

void WebqqChatSession::slotDisplayPictureChanged()
{
	QList<Kopete::Contact*> mb=members();
    WebqqContact *c = static_cast<WebqqContact *>( mb.first() );
//	if ( c && m_image )
//	{
//		if(c->hasProperty(Kopete::Global::Properties::self()->photo().key()))
//		{
//#ifdef __GNUC__
//#warning Port or remove this KToolBar hack
//#endif
//#if 0
//			int sz=22;
//			// get the size of the toolbar were the aciton is plugged.
//			//  if you know a better way to get the toolbar, let me know
//			KXmlGuiWindow *w= view(false) ? dynamic_cast<KXmlGuiWindow*>( view(false)->mainWidget()->topLevelWidget() ) : 0L;
//			if(w)
//			{
//				//We connected that in the constructor.  we don't need to keep this slot active.
//				disconnect( Kopete::ChatSessionManager::self() , SIGNAL(viewActivated(KopeteView*)) , this, SLOT(slotDisplayPictureChanged()) );

//                KAction *imgAction=actionCollection()->action("WebqqDisplayPicture");
//				if(imgAction)
//				{
//					QList<KToolBar*> toolbarList = w->toolBarList();
//					QList<KToolBar*>::Iterator it, itEnd = toolbarList.end();
//					for(it = toolbarList.begin(); it != itEnd; ++it)
//					{
//						KToolBar *tb=*it;
//						if(imgAction->isPlugged(tb))
//						{
//							sz=tb->iconSize();
//							//ipdate if the size of the toolbar change.
//							disconnect(tb, SIGNAL(modechange()), this, SLOT(slotDisplayPictureChanged()));
//							connect(tb, SIGNAL(modechange()), this, SLOT(slotDisplayPictureChanged()));
//							break;
//						}
//						++it;
//					}
//				}
//			}

//			QString imgURL=c->property(Kopete::Global::Properties::self()->photo()).value().toString();
//			QImage scaledImg = QPixmap( imgURL ).toImage().smoothScale( sz, sz );
//			if(!scaledImg.isNull())
//				m_image->setPixmap( QPixmap(scaledImg) );
//			else
//			{ //the image has maybe not been transferred correctly..  force to download again
//				c->removeProperty(Kopete::Global::Properties::self()->photo());
//				//slotDisplayPictureChanged(); //don't do that or we might end in a infinite loop
//			}
//			m_image->setToolTip( "<qt><img src=\"" + imgURL + "\"></qt>" );
//#endif
//		}
//	}
}

#include "webqqchatsession.moc"
