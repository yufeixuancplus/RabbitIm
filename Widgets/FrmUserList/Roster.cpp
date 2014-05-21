#include "Roster.h"
#include "../../MainWindow.h"

CRoster::CRoster(QObject *parent) :
    QObject(parent)
{
    Init((MainWindow*)parent);
}

CRoster::CRoster(QXmppRosterIq::Item item, MainWindow *parent) : QObject(parent)
{
    Init(parent);
    m_RosterItem = item;
}

CRoster::~CRoster()
{
    qDebug("CRoster::~CRoster");
    DeleteItems();
}

int CRoster::Init(MainWindow *parent)
{
    m_nNewMessageNumber = 0;
    m_pMainWindow = parent;

    m_Message.SetRoster(this, m_pMainWindow);

    return 0;
}

QString CRoster::Name()
{
    QString n = m_RosterItem.name();
    if(n.isEmpty())
        n = QXmppUtils::jidToUser(BareJid());
    return n;
}

QString CRoster::BareJid()
{
    return m_RosterItem.bareJid();
}

QString CRoster::Jid()
{
    return m_RosterItem.bareJid();
}

QString CRoster::Domain()
{
    return QXmppUtils::jidToDomain(BareJid());
}

QString CRoster::Resouce()
{
    return QXmppUtils::jidToResource(BareJid());
}

//int CRoster::SetJid(QString jid)
//{
//    m_szJid = jid;
//    return 0;
//}

QSet<QString> CRoster::Groups()
{
    return m_RosterItem.groups();
}

QXmppPresence::AvailableStatusType CRoster::GetStatus()
{
    return m_Status;
}

QXmppRosterIq::Item::SubscriptionType CRoster::GetSubScriptionType()
{
    return m_RosterItem.subscriptionType();
}

QString CRoster::GetSubscriptionTypeStr(QXmppRosterIq::Item::SubscriptionType type) const
{
    switch(type)
    {
    case QXmppRosterIq::Item::NotSet:
        return "";
    case QXmppRosterIq::Item::None:
        return tr("[none]");
    case QXmppRosterIq::Item::Both:
        return "";
    case QXmppRosterIq::Item::From:
        return tr("[From]");
    case QXmppRosterIq::Item::To:
        return "";
    case QXmppRosterIq::Item::Remove:
        return tr("[remove]");
    default:
        {
            qWarning("QXmppRosterIq::Item::getTypeStr(): invalid type");
            return "";
        }
    }
}

QList<QStandardItem*> CRoster::GetItem()
{
    //呢称条目
    QStandardItem* pItem = new QStandardItem(Name() + GetSubscriptionTypeStr(GetSubScriptionType()));
    QVariant v;
    v.setValue(this);
    pItem->setData(v);
    pItem->setEditable(true);//允许双击编辑
    m_lstUserListItem.push_back(pItem);

    //消息条目
    QStandardItem* pMessageCountItem = new QStandardItem("0");
    v.setValue(this);
    pMessageCountItem->setData(v);
    pMessageCountItem->setEditable(false);//禁止双击编辑

    //TODO:未读新消息数目树形控件中未显示
    QList<QStandardItem *> lstItems;
    lstItems.push_back(pItem);
    lstItems.push_back(pMessageCountItem);

    //更新条目显示内容
    UpdateItemDisplay();

    return lstItems;
}

int CRoster::DeleteItems()
{
    std::list<QStandardItem*>::iterator it;
    for(it = m_lstUserListItem.begin(); it != m_lstUserListItem.end(); it++)
    {
        QStandardItem* p = *it;
        if(p->parent())
        {
            p->parent()->removeRow(p->row()); //控件会自己释放 QStandardItem 内存
        }
    }
    m_lstUserListItem.clear();
    return 0;
}

int CRoster::UpdateItems(QXmppRosterIq::Item item)
{
    m_RosterItem = item;
    DeleteItems();
}

int CRoster::UpdateItemDisplay()
{
    std::list<QStandardItem*>::iterator it;
    for(it = m_lstUserListItem.begin(); it != m_lstUserListItem.end(); it++)
    {
        QStandardItem* p = *it;
        p->setData(g_Global.GetStatusColor(m_Status), Qt::BackgroundRole);
        QString szText = this->Name()
                + "[" + g_Global.GetStatusText(m_Status) + "]"
                +  GetSubscriptionTypeStr(GetSubScriptionType());
        p->setData(szText, Qt::DisplayRole);
    }
    return 0;
}

//TODO:修改成图片表示
int CRoster::ChangedPresence(QXmppPresence::AvailableStatusType status)
{
    m_Status = status;

    UpdateItemDisplay();

    emit sigChangedPresence(status);

    return 0;
}

int CRoster::ShowMessageDialog()
{
    int nRet = 0;

    m_Message.show();
    m_Message.activateWindow();//激活窗口，置顶

    return nRet;
}

int CRoster::AppendMessage(const QString &szMessage)
{
    m_nNewMessageNumber++;
    //TODO:设置控件计数

    return m_Message.AppendMessage(szMessage);
}

int CRoster::CleanNewMessageNumber()
{
    m_nNewMessageNumber = 0;
    //TODO:清除控件计数

    return 0;
}
