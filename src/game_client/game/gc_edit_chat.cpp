
#include "gc_include.h"
#include "gc_edit_chat.h"
#include "gc_chat_const.h"
#include "UIForm_Gem.h"
#include "ui_form_char.h"
#include "ui_form_pet_include.h"
#include "ui_form_pet_bank.h"
#include "ui_form_union_create.h"

extern BOOL bRenderUI;

CEditChat::CEditChat() : mlastindex(-1) {}

bool CEditChat::ParseText()  //�����ı�
{
    guard;

    //���ͬԭ������ͬ�����÷���
    string Message = GetWorld()->m_Chat.mpChatInput->GetText();
    if (mLastText == Message) {
        return false;
    }

    string Temp;
    Temp = Message;
    //�õ��ı����ݺ����֣�[ֻ��˽�Ĳ�������]
    if (GetWorld()->m_Chat.GetNowChannel() == PERSONAL_CHANNEL) {  //��˽��Ҫ��������
        string::size_type Loc;
        Loc = Temp.find(SPACE_FLAG1);
        if (Loc == string::npos) {  //��ǿո�û�ҵ���������ȫ�ǿո�
            Loc = Temp.find(SPACE_FLAG2);
            if (Loc == string::npos) {  //û���ҵ��ո�

                mName = "";    //����Ϊ��
                mText = Temp;  //�õ�����
            } else {           //�ҵ�ȫ�ǿո�
                mName = Temp.substr(0, Loc);
                mText = Temp.substr(Loc + 2);
            }
        } else {  //�ҵ���ǿո�
            mName = Temp.substr(0, Loc);
            mText = Temp.substr(Loc + 1);
        }
    } else {  //����˽��
        mText = Temp;
    }

    //��¼��ǰ��������Ϣ
    mLastText = Message;

    return true;
    unguard;
}

void CEditChat::DealText() {
    guard;
    if (ParseText()) {           //�ı���Ϣ�б仯
        if (mDoskey.IsLock()) {  //���Doskey�Ѿ�����
            if (GetWorld()->m_Chat.GetNowChannel() == PERSONAL_CHANNEL &&
                mName != "") {  //�Ѿ�����˽�ı��//�ر�doskey
                mDoskey.UnLock();
            }
            //else
            //{//���ڱ༭
            //	mDoskey.SetEditText(mName);
            //}
        } else {  //doskey��δ����
            if (GetWorld()->m_Chat.GetNowChannel() == PERSONAL_CHANNEL &&
                mName == "") {  //�����ĵı��//����Doskey[Lock]
                mDoskey.Lock();
            }
        }
    }

    return;
    unguard;
}

void CEditChat::KeyDownEvent(int key) {
    guard;
    if (mDoskey.IsLock()) {  //����doskey��ʱ���·�
        if (key == VK_UP) {
            mDoskey.Up();
            mName = mDoskey.Get();
            Show();
        } else if (key == VK_DOWN) {
            mDoskey.Down();
            mName = mDoskey.Get();
            Show();
        }
    } else {
        if (mTextlist.size() > 0) {
            if (mlastindex < 0 || mlastindex > mTextlist.size() - 1) {
                mlastindex = mTextlist.size() - 1;
            }

            if (key == VK_UP) {
                GetWorld()->m_Chat.mpChatInput->SetText(GetTestindex(mlastindex));
                mlastindex--;

                if (mlastindex < 0) {
                    mlastindex = mTextlist.size() - 1;
                }
            } else if (key == VK_DOWN) {
                GetWorld()->m_Chat.mpChatInput->SetText(GetTestindex(mlastindex));
                mlastindex++;

                if (mlastindex > mTextlist.size() - 1) {
                    mlastindex = 0;
                }
            }
            GetWorld()->m_Chat.mpChatInput->AutoAdjectCursor();
        }
    }
    DealText();
    unguard;
}

std::string CEditChat::GetTestindex(int index) {
    int num = 0;
    for (std::list<std::string>::iterator iterstr = mTextlist.begin(); iterstr != mTextlist.end();
         iterstr++) {
        std::string test = *iterstr;
        if (num == index) {
            return test;
        }
        num++;
    }
    return "";
}

void CEditChat::KeyCharDownEvent() {
    guard;
    //�����ı�����
    DealText();
    unguard;
}

void CEditChat::ReplacePrivateName(string& vrName) {
    guard;
    mDoskey.Lock();
    mDoskey.SetEditText(vrName);
    mName = vrName;
    mText = "";
    Show();
    DealText();
    unguard;
}

void CEditChat::AddName(string& vrAddName) {
    guard;
    if (mDoskey.IsLock()) {  //Doskey�Ѿ�����
        //�����ָ�ֵ
        mDoskey.SetEditText(vrAddName);
        mName = mDoskey.Get();
        Show();
    } else {                                                           //Doskey��δ����
        if (GetWorld()->m_Chat.GetNowChannel() == PERSONAL_CHANNEL) {  //��˽��
            mName = vrAddName;
            Show();
        } else {
            //��ӵ�ĩβ
            AddText(vrAddName);
        }
    }
    unguard;
}

void CEditChat::Change()  //��������ַ����ı�
{
    guard;
    DealText();
    unguard;
}

void CEditChat::AddText(string& vrAddText) {
    guard;
    GetWorld()->m_Chat.mpChatInput->SetText(GetWorld()->m_Chat.mpChatInput->GetText() + vrAddText);
    DealText();
    unguard;
}

void CEditChat::AddHyperLinkText(EChildHyperLinkType eChildType, DWORD itemTag, DWORD itemId,
                                 DWORD playerID, const std::string& strName, RtwPixel color) {
    std::string Name = "[";
    Name += strName;
    Name += "]";

    SHyperLink link;
    link.Type = hlinkClient;
    link.Text = "";
    link.childType = eChildType;
    link.vecParam.push_back(itemTag);
    link.vecParam.push_back(itemId);
    link.vecParam.push_back(playerID);

    GetWorld()->m_Chat.mpChatInput->AddHyperLinkText(Name, link, color);
    DealText();
}

//tim.yang do client message
bool CEditChat::DoClientCommand(std::string message) {
    int         nops = -1;
    std::string step = " ";
    if (std::string::npos != message.find("moveto", 0)) {
        message = message.substr(strlen("moveto") + 1);
        int start = 0;
        int end = message.find(step, start);
        int x = -99999;
        int y = -99999;
        if (std::string::npos != end)
            x = atoi(message.substr(start, end).c_str());
        start = end + 1;
        y = atoi(message.substr(start).c_str());
        if (-99999 != y && -99999 != x) {
            GetWorld()->m_pPlayer->GotoThere(x, y);
            return true;
        }
    }
    return false;
}

//end

const int NAME_LIMIT_COUNT = 32;

bool CEditChat::Send(char vMessage[])  //true��ʾ���ͳɹ�
{
    guard;
    if (GetWorld()->m_pPlayer && !GetWorld()->m_pPlayer->State_CanSpeak()) {
        PlayerFailOperate(R(MSG_FORBID_SPEAK), true);
        return true;
    }

    static int  dir = 0;
    std::string message = vMessage;

    //#ifdef _DEBUG
    //	if (message == "-GetLogToFile")
    //	{
    //		static int num = 0;
    //		char buf[256];
    //		sprintf(buf, "%d-mlgbdmemory.txt", ++num);
    //		RtCoreMem().SetLogToFile(buf);
    //		return true;
    //	}
    //#endif

    if (message == "-DrawPreformance mlgbd") {
        static bool bEnable = true;
        RtGetRender()->EnableDrawPref(bEnable, 5.f, 120.f);
        bEnable = !bEnable;
        return true;
    }

    //ͨ�����������
    if (message == "-CreateUnion mlgbd") {
        if (GetWorld()->m_pPlayer->GetUnionID() != 0) {
            PlayerFailOperate(R(MSG_UNION_YOUHAVEAUNIONALREADY));
        } else {
            //65535�ض��ַ�
            g_layerMain->m_formUnionCreate->Show(65535);
        }
        return true;
    }

    if ('-' == vMessage[0]) {
        int nops = message.find("-client");
        if (std::string::npos != nops && ' ' == vMessage[7]) {
            message = message.substr(strlen("-client") + 1);
            if (DoClientCommand(message)) {
                return false;
            }
        }
    }

    if (message == "-ability") {
        g_layerMain->m_formPetAbility->Show();
    } else if (message == "-merge") {
        g_layerMain->m_formPetMerge->Show();
    } else if (message == "-tobeads") {
        g_layerMain->m_formPetToBeads->Show();
    } else if (message == "-beads") {
        g_layerMain->m_formPetMergeBeads->Show();
    } else if (message == "-petbank") {
        g_layerMain->m_formPetBank->Show(GetWorld()->m_dwInteractiveNPCID);
    }

    //end
    const char*   pMessage = mText.c_str();
    SChannelAtom* pChannel = NULL;
    RtwWidget*    focusWidget = g_workspace.GetFocusWidget();
    pChannel = GetWorld()->m_Chat.GetChannel(GetWorld()->m_Chat.GetNowChannel());
    if (pChannel->mChannel == PERSONAL_CHANNEL) {  //˽��Ƶ��
        if (mName.size() > NAME_LIMIT_COUNT) {
            GetWorld()->m_Chat.SystemMessage(R(THAT_PLAYER_NOT_INLINE), true);
            return true;
        }

        if (mName == "") {
            ShowSystemMessage(R(MSG_THEREISNONAME));
            return false;
        }
    } else if (pChannel->mChannel == WORLD_CHANNEL) {
        //�߻�Ҫ�� 10��������ʱ���Զ�����ʹ������
        if (GetWorld()->m_pPlayer->m_core.Lev < 10) {
            ShowSystemMessage(R(TYRO_LIMIT_CHAT));
            return false;
        }
    }

    if (strlen(pMessage) > CHAT_MESSAGE_MAX_COUNT) {
        GetWorld()->m_Chat.SystemMessage(R(CHAT_LIMIT_STRING), true);
        static string Temp;
        Temp = pMessage;
        Temp = Temp.substr(0, CHAT_MESSAGE_MAX_COUNT);
        pMessage = Temp.c_str();
    }

    if (pMessage[0] == '\0') {  //���ݿգ�������
        return false;
    }

    if (!GetWorld()->m_Chat.CheckChatFrequency(pChannel->mChannel)) {  //�ٶ�̫�죬������
        return false;
    }

    if (!GetWorld()->m_bLogined) {
        return false;
    }

    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_chat);
    pPacket->WriteByte(pChannel->mNetChannel);  // ����Ƶ��
    if (pChannel->mChannel == PERSONAL_CHANNEL) {
        pPacket->WriteString((char*)(mName.c_str()));  //���� // ���������м�鳤��
    }
    pPacket->WriteString((char*)pMessage);  // ���� // ���������м�鳤��
    NetSend(pPacket);

    if (pChannel->mChannel == PERSONAL_CHANNEL) {
        mDoskey.Add(mName);
    }
    mDoskey.UnLock();
    GetWorld()->m_Chat.mpChatInput->SetText("");  //�����ֳ�
    return true;

    unguard;
}

void CEditChat::Receive(SChannelAtom* vpChannel, long vFromRegionID, char vFromName[],
                        char vMessage[]) {
    guard;
    /* gao 2010.1.4
	�����յ���Ϣʱ�Ĵ���
	*/
    DWORD         Color = vpChannel->mColor;  //��ɫҪ�ر���
    static string strFrom = "From ";
    static string strTo = "To ";

    if (vpChannel->mChannel == PERSONAL_CHANNEL) {
        //����Ƶ��,���������Ƶ������������ӽ�����
        //������ɫ����ͨһ����,ͬʱ��������ӵ�hotkey����ȥ
        if (GetWorld()->m_pPlayer->m_core.Name != vFromName) {
            string Temp(vFromName);
            mDoskey.Add(Temp);
        }
    }
    if (vpChannel->mChannel == SYSTEM_CHANNEL) {
        //����Ϣ���ϵ�ϵͳ��Ϣר�ÿ���
        LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)
            ->Add(vpChannel->mMask, Color, vpChannel->mDescription.c_str(), vFromName, vMessage);
    } else if (vpChannel->mChannel == SPEAKER_CHANNEL) {
        //���Ի��������ϵ�������
        //Ƶ�����ѡ��
        GetWorld()->m_Chat.m_worldchatFeedback->Add(vpChannel->mMask, Color,
                                                    vpChannel->mDescription.c_str(), vFromName,
                                                    vMessage, vFromRegionID, false);
    } else {
        std::list<CHATCONTENT*>& chatContent = GetWorld()->m_Chat.GetChatContent();
        CHATCONTENT* tmp = RT_NEW CHATCONTENT(vpChannel->mMask, Color, vFromRegionID,
                                              vpChannel->mDescription.c_str(), vFromName, vMessage);
        chatContent.push_back(tmp);
        if (chatContent.size() >
            100) {  //���������ݳ���100����ʱ�򣬽���һ����list��ȡ����������ɾ������
            std::list<CHATCONTENT*>::iterator it;
            it = chatContent.begin();
            DEL_ONE((*it)->Description);
            DEL_ONE((*it)->name);
            DEL_ONE((*it)->content);
            DEL_ONE(*it);
            chatContent.pop_front();
            // 			GetWorld()->m_Chat.RefreshChatWidget();
        }
        for (int i = 0; i < MAX_CHANNEL_CHATBOX; i++) {
            int* iMask = GetWorld()->m_Chat.GetChatMask();
            if ((iMask[i] & vpChannel->mMask) == vpChannel->mMask) {
                if (vpChannel->mChannel == PERSONAL_CHANNEL) {
                    //����Ƶ��,���������Ƶ������������ӽ�����
                    //������ɫ����ͨһ����,ͬʱ��������ӵ�hotkey����ȥ
                    if (GetWorld()->m_pPlayer->m_core.Name != vFromName) {
                        GetWorld()->m_Chat.m_chatFeedback[i]->Add(vpChannel->mMask, Color,
                                                                  strFrom.c_str(), vFromName,
                                                                  vMessage, vFromRegionID);
                    } else {
                        Color = 0xffa5a5;
                        GetWorld()->m_Chat.m_chatFeedback[i]->Add(
                            vpChannel->mMask, Color, strTo.c_str(), (char*)mName.c_str(), vMessage,
                            vFromRegionID);
                    }
                } else {
                    GetWorld()->m_Chat.m_chatFeedback[i]->Add(vpChannel->mMask, Color,
                                                              vpChannel->mDescription.c_str(),
                                                              vFromName, vMessage, vFromRegionID);
                }
            }
        }
    }
    /* end */
    unguard;
}

void CEditChat::Show() {
    guard;
    mLastText = mName + SPACE_FLAG1;
    GetWorld()->m_Chat.mpChatInput->SetTextnew(mLastText.c_str());
    unguard;
}
