/*
    webqqchatsession.h - Manages friend and session chats

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

#ifndef WEBQQCHATSESSION_H
#define WEBQQCHATSESSION_H

#include "kopetechatsession.h"
//#include "kopete_export.h"

class QLabel;


/**
 * @author Andre Duffeck
 */
class WebqqChatSession : public Kopete::ChatSession
{
	Q_OBJECT

public:
    WebqqChatSession(Kopete::Protocol *protocol, const Kopete::Contact *user, Kopete::ContactPtrList others);
    ~WebqqChatSession();
    void setTopic( const QString & topic );
private slots:
	void slotDisplayPictureChanged();
    void slotimageContact();
	void slotBuzzContact();
	void slotUserInfo();
	void slotSendFile();

private:
	QLabel *m_image;
};

#endif
