/*
    webqqbridgecallback.cpp - Webqq c and Qt bridge

    Copyright (c) 2014 by Jun Zhang        <jun.zhang@i-soft.com.cn>

    Kopete    (c) 2002-2006 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/
#include "webqqbridgecallback.h"


void ObjectInstance::callSignal(CallbackObject callback)
{
    emit signal_from_instance(callback);
}

ObjectInstance::ObjectInstance()
{
}

ObjectInstance *ObjectInstance::m_instance = 0;

ObjectInstance* ObjectInstance::instance()
{
  if (!m_instance) {
    if (!m_instance)
    m_instance = new ObjectInstance;
  }
 
    return m_instance;
}


#include "webqqbridgecallback.moc"
