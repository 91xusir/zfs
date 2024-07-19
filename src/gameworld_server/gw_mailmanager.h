#ifndef _INC_GW_MAIL_MANAGER_H_
#define _INC_GW_MAIL_MANAGER_H_

class GWMailManager
{
public:
	bool			AddMail(int userID, int senderID, const string& title, const string& content, bool notify ,const string &StrItemID,long Money);

	void			RemoveMail(long MailID);

	void			CheckMail(int userID);	

	bool            MoveItem(long MailID);

	bool            Update(short IsOpen,long MailID);

	bool            UpDateItemInfo(long MailID);

private:
	static void		OnMailSent(GWDBResult* result);

	static void		OnMailChecked(GWDBResult* result);
	static void		OnMailItem(GWDBResult* result);


};

#endif // _INC_GW_MAIL_MANAGER_H_
