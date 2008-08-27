/*
 * kvi_network.h
 *
 *  Created on: 27.08.2008
 *      Author: Alexey
 */

#ifndef KVI_NETWORK_H_
#define KVI_NETWORK_H_

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_pointerlist.h"
#include <QString>

class KviNickServRuleSet;
class KviServer;
class QStringList;

class KVILIB_API KviNetwork : public KviHeapObject
{
	friend class KviServerDataBase;
public:
	KviNetwork(const KviNetwork &src);
	KviNetwork(const QString &name);
	~KviNetwork();
protected:
	QString              m_szName;
	QString              m_szDescription;
	QString              m_szEncoding;                // if empty, use system default
	QString              m_szTextEncoding;                // if empty, use system default
	QString              m_szNickName;                // preferred nick name
	QString              m_szUserName;                // preferred user name
	QString              m_szRealName;                // preferred real name
	QString              m_szOnConnectCommand;        // the command to run on connect
	QString              m_szOnLoginCommand;          // the command to run after login
	QStringList        * m_pChannelList;              // Channels to auto join
	KviNickServRuleSet * m_pNickServRuleSet;          // set of nick serv rules
	bool                 m_bAutoConnect;              // autoconnect
	QString              m_szUserIdentityId;          // The user identity to use for this server: if empty
	                                                  // Then use the global primary identity
	//moved from KviServerDataBaseRecord
	KviPointerList<KviServer> * m_pServerList;
	KviServer             * m_pCurrentServer;
public:
	inline const QString & name() const { return m_szName; };
	inline const QString & encoding() const { return m_szEncoding; };
	inline const QString & textEncoding() const { return m_szTextEncoding; };
	inline const QString & description() const { return m_szDescription; };
	inline const QString & nickName() const { return m_szNickName; };
	inline const QString & realName() const { return m_szRealName; };
	inline const QString & userName() const { return m_szUserName; };
	inline const QString & onLoginCommand() const { return m_szOnLoginCommand; };
	inline const QString & onConnectCommand() const { return m_szOnConnectCommand; };
	inline const QString & userIdentityId() const { return m_szUserIdentityId; };
	inline bool autoConnect() const { return m_bAutoConnect; };
	inline QStringList* autoJoinChannelList(){ return m_pChannelList; };
	inline KviNickServRuleSet * nickServRuleSet(){ return m_pNickServRuleSet; };
	
	void setNickServRuleSet(KviNickServRuleSet * s);
	void copyFrom(const KviNetwork &d);

	inline void setName(const QString &szName){ m_szName = szName; };
	inline void setEncoding(const QString &szEncoding){ m_szEncoding = szEncoding; };
	inline void setTextEncoding(const QString &szEncoding){ m_szTextEncoding = szEncoding; };
	inline void setDescription(const QString &szDescription){ m_szDescription = szDescription; };
	inline void setOnConnectCommand(const QString &cmd){ m_szOnConnectCommand = cmd; };
	inline void setOnLoginCommand(const QString &cmd){ m_szOnLoginCommand = cmd; };
	inline void setNickName(const QString &n){ m_szNickName = n; };
	inline void setRealName(const QString &r){ m_szRealName = r; };
	inline void setUserName(const QString &u){ m_szUserName = u; };
	
	void setAutoJoinChannelList(QStringList * pNewChannelList);
	
	inline void setAutoConnect(bool bAutoConnect){ m_bAutoConnect = bAutoConnect; };
	inline void setUserIdentityId(const QString &szUserIdentityId){ m_szUserIdentityId = szUserIdentityId; };

	inline KviPointerList<KviServer> * serverList(){ return m_pServerList; };
	KviServer * currentServer();
	void        insertServer(KviServer *srv);

	KviServer * findServer(const QString& szHostname);
	KviServer * findServer(const KviServer * pServer);

	void setCurrentServer(KviServer *srv);
};

#endif /* KVI_NETWORK_H_ */
