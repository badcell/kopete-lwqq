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
static void qq_add_buddy(const char *username);
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

static void show_confirm_table(LwqqClient* lc,LwqqConfirmTable* table)
{
    qq_account* ac = lwqq_client_userdata(lc);
    PurpleRequestFields *fields;
    PurpleRequestFieldGroup *field_group;

    fields = purple_request_fields_new();
    field_group = purple_request_field_group_new((gchar*)0);
    purple_request_fields_add_group(fields, field_group);

    if(table->body){
        PurpleRequestField* str = purple_request_field_string_new("body", table->title, table->body, TRUE);
        purple_request_field_string_set_editable(str, FALSE);
        purple_request_field_group_add_field(field_group, str);
    }

    if(table->exans_label||table->flags&LWQQ_CT_ENABLE_IGNORE||table->flags&LWQQ_CT_CHOICE_MODE){
        PurpleRequestField* choice = purple_request_field_choice_new("choice", _("Please Select"), table->answer);
        purple_request_field_choice_add(choice,table->no_label?:_("Deny"));
        purple_request_field_choice_add(choice,table->yes_label?:_("Accept"));
        if(table->exans_label)
            purple_request_field_choice_add(choice,table->exans_label);
        purple_request_field_group_add_field(field_group,choice);
    }

    if(table->input_label){
        PurpleRequestField* i = purple_request_field_string_new("input", table->input_label, table->input?:"", FALSE);
        s_free(table->input);
        purple_request_field_group_add_field(field_group,i);
    }

    purple_request_fields(ac->gc, NULL,
                          _("Confirm"), NULL,
                          fields, _("Confirm"), G_CALLBACK(confirm_table_yes),
                          table->flags&LWQQ_CT_ENABLE_IGNORE?_("Ignore"):_("Deny"), G_CALLBACK(confirm_table_no),
                          ac->account, NULL, NULL, table);
}

static void add_friend_receipt(LwqqAsyncEvent* ev)
{
    int err = ev->result;
    LwqqClient* lc = ev->lc;
    qq_account* ac = lc->data;
    if(err == 6 ){
        QString message = i18n( "ErrCode:6\nPlease try agagin later\n");
        KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Error, message);
        //purple_notify_message(ac->gc,PURPLE_NOTIFY_MSG_INFO,_("Error Message"),_("ErrCode:6\nPlease try agagin later\n"),NULL,NULL,NULL);
    }
}
static void add_friend_ask_message(LwqqClient* lc,LwqqConfirmTable* ask,LwqqBuddy* b)
{
    add_friend(lc,ask,b,s_strdup(ask->input));
}
static void add_friend(LwqqClient* lc,LwqqConfirmTable* c,LwqqBuddy* b,char* message)
{
    if(c->answer == LWQQ_NO){
        goto done;
    }
    if(message==NULL){
        LwqqConfirmTable* ask = s_malloc0(sizeof(*ask));
        ask->input_label = s_strdup(_("Invite Message"));
        ask->cmd = _C_(3p,add_friend_ask_message,lc,ask,b);
        show_confirm_table(lc, ask);
        lwqq_ct_free(c);
        return ;
    }else{
        LwqqAsyncEvent* ev = lwqq_info_add_friend(lc, b,message);
        lwqq_async_add_event_listener(ev, _C_(p,add_friend_receipt,ev));
    }
done:
    lwqq_ct_free(c);
    lwqq_buddy_free(b);
    s_free(message);
}

static void format_body_from_buddy(char* body,size_t buf_len,LwqqBuddy* buddy)
{
    char body_[1024] = {0};
#define ADD_INFO(k,v)  if(v) format_append(body_,"%s:%s\n",k,v)
    ADD_INFO(_("QQ"), buddy->qqnumber);
    ADD_INFO(_("Nick"), buddy->nick);
    ADD_INFO(_("Longnick"), buddy->personal);
    ADD_INFO(_("Gender"), qq_gender_to_str(buddy->gender));
    ADD_INFO(_("Shengxiao"), qq_shengxiao_to_str(buddy->shengxiao));
    ADD_INFO(_("Constellation"), qq_constel_to_str(buddy->constel));
    ADD_INFO(_("Blood Type"), qq_blood_to_str(buddy->blood));
    ADD_INFO(_("Birthday"),lwqq_date_to_str(buddy->birthday));
    ADD_INFO(_("Country"), buddy->country);
    ADD_INFO(_("Province"), buddy->province);
    ADD_INFO(_("City"), buddy->city);
    ADD_INFO(_("Phone"), buddy->phone);
    ADD_INFO(_("Mobile"), buddy->mobile);
    ADD_INFO(_("Email"), buddy->email);
    ADD_INFO(_("Occupation"), buddy->occupation);
    ADD_INFO(_("College"), buddy->college);
    ADD_INFO(_("Homepage"), buddy->homepage);
    //ADD_INFO("说明", buddy->);
#undef ADD_INFO
    strncat(body,body_,buf_len-strlen(body));
}

static void search_buddy_receipt(LwqqAsyncEvent* ev,LwqqBuddy* buddy,char* uni_id,char* message)
{
    int err = ev->result;
    LwqqClient* lc = ev->lc;
    qq_account* ac = lc->data;
    QString message;
    //captcha wrong
    if(err == 10000){
        LwqqAsyncEvent* event = lwqq_info_search_friend(lc,uni_id,buddy);
        lwqq_async_add_event_listener(event, _C_(4p,search_buddy_receipt,event,buddy,uni_id,message));
        return;
    }
    if(err == LWQQ_EC_NO_RESULT){
        message = i18n( "Account not exists or not main display account");
        KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Error, message);
        //purple_notify_message(ac->gc,PURPLE_NOTIFY_MSG_INFO,_("Error Message"),_("Account not exists or not main display account"),NULL,NULL,NULL);
        goto failed;
    }
    if(!buddy->token){
        message = i18n( "Get friend infomation failed");
        KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Error, message);
        //purple_notify_message(ac->gc,PURPLE_NOTIFY_MSG_INFO,_("Error Message"),_("Get friend infomation failed"),NULL,NULL,NULL);
        goto failed;
    }
    LwqqConfirmTable* confirm = s_malloc0(sizeof(*confirm));
    confirm->title = s_strdup(_("Friend Confirm"));
    char body[1024] = {0};
    format_body_from_buddy(body,sizeof(body),buddy);
    confirm->body = s_strdup(body);
    confirm->cmd = _C_(4p,add_friend,lc,confirm,buddy,message);
    show_confirm_table(lc,confirm);
    s_free(uni_id);
    return;
failed:
    lwqq_buddy_free(buddy);
    s_free(message);
    s_free(uni_id);
}

void qq_add_buddy(const char *username)
{
//    LwqqBuddy* buddy = lwqq_buddy_new();
//    const char *passwd = "XXX";
//    LwqqClient* lc = lwqq_client_new(username,passwd);
//    LwqqFriendCategory* cate = lwqq_category_find_by_name(lc,group->name);
//    if(cate == NULL){
//        buddy->cate_index = 0;
//    }else
//        buddy->cate_index = cate->index;
//    LwqqAsyncEvent* ev = lwqq_info_search_friend(lc,username,buddy);
//    const char* msg = NULL;
//    lwqq_async_add_event_listener(ev, _C_(4p,search_buddy_receipt,ev,buddy,s_strdup(username),s_strdup(msg)));
    qq_account* ac = ((WebqqAccount*)account())->m_lc;
    LwqqClient* lc = ac->qq;
    const char* uni_id = username;
    LwqqGroup* g = NULL;
    LwqqSimpleBuddy* sb = NULL;
    LwqqBuddy* f_buddy = lwqq_buddy_new();
    LwqqFriendCategory* cate = NULL;
    if(cate == NULL){
        f_buddy->cate_index = 0;
    }else
        f_buddy->cate_index = cate->index;

    if(find_group_and_member_by_card(lc, uni_id, &g, &sb)){
        LwqqAsyncEvset* set = lwqq_async_evset_new();
        LwqqAsyncEvent* ev;
        f_buddy->uin = s_strdup(sb->uin);
        ev = lwqq_info_get_group_member_detail(lc, sb->uin, f_buddy);
        lwqq_async_evset_add_event(set, ev);
        ev = lwqq_info_get_friend_qqnumber(lc,f_buddy);
        lwqq_async_evset_add_event(set,ev);
        lwqq_async_add_evset_listener(set, _C_(3p,lwqq_info_add_group_member_as_friend,lc,f_buddy,NULL));
    }else{
        //friend->qqnumber = s_strdup(qqnum);
        LwqqAsyncEvent* ev = lwqq_info_search_friend(ac->qq,uni_id,f_buddy);
        const char* msg = NULL;
        lwqq_async_add_event_listener(ev, _C_(4p,search_buddy_receipt,ev,f_buddy,s_strdup(uni_id),s_strdup(msg)));
}


#include "webqqaddcontactpage.moc"
