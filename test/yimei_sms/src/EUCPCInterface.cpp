#include "MTNProtocol.h"
#include "EUCPCInterface.h"
#include "MTNMsgApi.h"	
#include "socktool.h"
#include "Md5.h"
		
/*	
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <limits.h>
#include <linux/rtc.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>  
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/types.h>
#include <unistd.h>
#include <iostream> 
	
#ifdef EUCPC_DEBUG  
	#include "log.h"
#endif

using namespace std;	
namespace MTNMsgApi_NP
{

#ifdef EUCPC_DEBUG    		
	CLog g_Log;
#endif

	const char* vertemp = "4.3.2_Linux C++";
	char* domain = "sdk410.eucp.b2m.cn";//解析域名
	char* defdomain = "219.239.91.76";//默认ip地址 

	//for test
	//char* domain = "";//解析域名
	//char* defdomain = "172.16.1.110";    //默认ip地址 



	TMTNMsgApi api;
	
#ifdef EUCPC_DEBUG    		
	int SetLog(char*path,char*name,int size,int mask)
	{
		//enum enumMessage{M1,M2,WA,ER,FA,D1,D2};  /// 日志等级
		g_Log.openLog(path,name,size,mask);
		return 0;
	}
#endif
	
#define TIMEDELTA_MS(time1,time0) \
  ((time1.tv_sec-time0.tv_sec)*1000+(time1.tv_usec-time0.tv_usec)/1000)
   
	
	int Close()
	{
		int ret = api.Close();
		return ret;
	}
	
	void  GetSDKVersion(char* ver)
	{   
	   if (ver == NULL) return; 
	   memcpy(ver, vertemp, strlen(vertemp));
	   return;
	}
	
	
	//成功返回1，失败返回0
	int ServerConnect()
	{	
		try 
		{
			if(!api.m_bSockError) return 1;
			//
			if (api.m_ip == NULL || 0 == strlen(api.m_ip))
			{
				int ret = api.domainToIP(const_cast<char*>(domain));  
				if(ret < 0)
				{
#ifdef EUCPC_DEBUG    		
					//printf("gethostbyname() ret = %d ,please check host file\n" ,ret);
					g_Log.writeLog(D1,"gethostbyname() ret = [%d] ,please check host file",ret);			
#endif
					sprintf(api.m_ip,"%s",defdomain);
				}
			}
			//int timeout = 3 * 60 * 1000; //超时时间为1分钟
			//设置发送超时时间
			//设置接收超时时间
			//
			int nSock = CSockTool::connect(api.m_ip, api.m_port, 1000);
				/*		
				sockServer.sin_family = AF_INET;
				sockServer.sin_port = htons(port);
				sockServer.sin_addr.S_un.S_addr=inet_addr(ip);
				sock = socket(AF_INET,SOCK_STREAM,0);
		   			
				int timeout = 3 * 60 * 1000; //超时时间为1分钟
				//设置发送超时时间
				int ret = setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout));
				if (ret == SOCKET_ERROR) return 0;
				//设置接收超时时间
				ret = setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,sizeof(timeout));
				if (ret == SOCKET_ERROR) return 0;
				*/
	
			
			
			//if( nSock == -1 )
			if( nSock < 0 )
			{
#ifdef EUCPC_DEBUG    		
				//printf("connect failed  errno = %d,%s\n", errno, strerror(errno));
				g_Log.writeLog(D1,"connect failed  errno = [%d],%s",errno, strerror(errno));						
#endif			
				sleep(1);
				return 0;//error
			} 	
			else
			{	
#ifdef EUCPC_DEBUG    		
				//printf("connect nSock =   %d\n", nSock);
				g_Log.writeLog(D1,"connect nSock =   [%d]",nSock);
#endif			
	
				api.Close();
				api.m_bStop = false;//muset first set, or thread not start up.
				api.m_bThreadCreated = false;//judge if start heart thread.
				api.SetSocket(nSock); 	
				api.ActiveThread();
				api.m_bSockError = false;
				return 1;
			}
		}
		catch(...) 
		{
			return 0;	
		}
	}
	//检查发送内容是否是ascii 0-127的。如果全是允许发送140个字节，否则发70个字节。
	int  CheckCode(char *Code)
	{
		int i_2 = 0;
		int k =	strlen(Code);
		char * content = Code;
			
		for (int j=0; j < k; j++)
		{	
		
	
			char con_f[2];
			memset(con_f,0,2);
			memcpy(con_f,content,1);
			content++;
	
			int asc_i = (int)*con_f;
			if(( asc_i < 0 ) || ( asc_i > 127 )) 
			{
				i_2 ++;
			}
	
			/*	if( (( asc_i < 0 ) || ( asc_i > 127 )) && (strlen(Code) > 140) ) 
				{
					free(con_f);
					return 1 ;
			
				}*/
	
		}
		if ((i_2 == 0) && (strlen(Code) > 1000))
		{
			return 1 ;
		}
		else if((((strlen(Code)-i_2)+i_2/2) >500 ) && !(i_2 == 0))
		{
			return 1 ;
		}
		//
		return 0 ;
	}
	int ValidateDigit(const char* mobile) 
	{
		if (mobile == NULL) return 0;
		for(unsigned int i = 0; i < strlen(mobile); i++) 
		{		
			if(isdigit(mobile[i]) == 0)	
			{	   
				return 0;
			}
		}
		return 1;
	}
	
	int SetKey(char* key)
	{
		int keyLen = strlen(key); 
	    if(key==NULL || keyLen==0) 
		{
	    	memset(api.m_KeyVal,0,sizeof(api.m_KeyVal));
			return 1;
		}
	    
		memset(api.m_KeyVal, 0, 33);
	    char* temp = Md5(key);
		memcpy(api.m_KeyVal, temp, strlen(temp));
	    delete[] temp;
		temp = NULL;
	
		return 1;
	}
	//注册消息
	int Register(char* sn, char* pwd, char* EntName,char* LinkMan,char* Phone,
				char* Mobile,char* Email,char* Fax,char* sAddress,char* Postcode) 
	{
		auto_lock lock(api.m_MuxConnObj);	

		if (sn == NULL) return SERIAL_ERROR; 
		if (pwd == NULL) return NOTENOUGHINFO_ERROR;
		
		int pwdLen = strlen(pwd); 
	    if(pwd==NULL || pwdLen==0 || pwdLen!=6) 
			return NOTENOUGHINFO_ERROR;
		if(!ValidateDigit(pwd))
			return NOTENOUGHINFO_ERROR;
		if (EntName == NULL) return NOTENOUGHINFO_ERROR;
		if (!api.ValidateFlag()) return FLAG_ERROR;
		
		//4.0.4:judge 20s
    	static struct timeval lastCallTime;	
		struct timeval curCallTime;
		gettimeofday(&curCallTime,0);
		unsigned int timeuse = TIMEDELTA_MS(curCallTime,lastCallTime);
		if(timeuse < SLEEP_SECOND_20 * 1000) //20s
		{
			//延时
			return FUN_CALL_TOO_FAST;
		}		
		//
		if (ServerConnect()) 
		{
			//
			gettimeofday(&lastCallTime,0);
			//
			int ret = api.SendRegister_1(sn,pwd);		
			if (ret)
			{
				ret = api.SendRegister_2(sn,EntName,LinkMan,Phone,Mobile,Email,Fax,sAddress,Postcode);
				if (!ret) {
					ret = ENTITYINFO_ERROR;
				}				
				return ret;
			}
			return FAILURE;
		}
		else 
		{
	
			return NET_ERROR;
		}	
		return FAILURE;
	}
	
	
	
	
	int GetBalance(char* sn,char* balance) 
	{	
		auto_lock lock(api.m_MuxConnObj);	
	
		if (sn == NULL) return SERIAL_ERROR; 
		if (balance == NULL) return PARAMISNULL_ERROR;
	
		if (!api.ValidateFlag()) return FLAG_ERROR;
		//first search from memory.
		struct timeval timeFromSendSms;
		int retVal = api.getSnAndBalanceFromArray(sn,balance,&timeFromSendSms);
		if(0 == retVal) //not find value in memory.
		{
			//
		}
		else if(1 == retVal)//find value in meory.
		{
			return 1;
		}
		//4.0.4:judge 20s
    	static struct timeval lastCallTime;	
		struct timeval curCallTime;
		gettimeofday(&curCallTime,0);
		unsigned int timeuse = TIMEDELTA_MS(curCallTime,lastCallTime);
		if(timeuse < SLEEP_SECOND_10 * 1000) //10s
		{
			return FUN_CALL_TOO_FAST;
		}				
		if (ServerConnect()) 
		{
			//
			gettimeofday(&lastCallTime,0);
			//
			
			int ret = api.GetBalance(sn,balance);
			return ret;
		}
		else 
		{
			return NET_ERROR;
		}	
		return FAILURE;
	}
	//获得发送一条短信的价格
	int GetPrice(char* sn,char* balance) 
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		//for 4.0.4
		strcpy(balance,"0.1");
		return 1;
		//for 4.0.3
		if (sn == NULL) return SERIAL_ERROR; 
		if (balance == NULL) return PARAMISNULL_ERROR;
	
		if (!api.ValidateFlag()) return FLAG_ERROR;
		if (ServerConnect()) 
		{
			int ret = api.GetPrice(sn,balance);
			return ret;
		}
		else 
		{
			return NET_ERROR;
		}	
		return FAILURE;
	}
	
	//修改软件序列号对应的密码
	int RegistryPwdUpd(char* sn, char* oldpwd, char* newpwd) 
	{
		auto_lock lock(api.m_MuxConnObj);	
	
		if (sn == NULL) return SERIAL_ERROR; 
		if (oldpwd == NULL || newpwd == NULL || strcmp(oldpwd,"")==0 || strcmp(newpwd,"")==0) return PWD_ERROR;
		
		int pwdLen = strlen(oldpwd); 
	    if(oldpwd==NULL || pwdLen==0 || pwdLen!=6) 
			return PWD_ERROR;
		if(!ValidateDigit(oldpwd))
			return PWD_ERROR;
	
		pwdLen = strlen(newpwd); 
	    if(newpwd==NULL || pwdLen==0 || pwdLen!=6) 
			return PWD_ERROR;
		if(!ValidateDigit(newpwd))
			return PWD_ERROR;
	
		if (!api.ValidateFlag()) return FLAG_ERROR;
		
		//4.0.4:judge 20s
    	static struct timeval lastCallTime;	
		struct timeval curCallTime;
		gettimeofday(&curCallTime,0);
		unsigned int timeuse = TIMEDELTA_MS(curCallTime,lastCallTime);
		if(timeuse < SLEEP_SECOND_20 * 1000) //20s
		{
			return FUN_CALL_TOO_FAST;		
		}		
		if (ServerConnect()) 
		{
			//
			gettimeofday(&lastCallTime,0);
			//
			int ret = api.RegistryPwdUpd(sn,oldpwd,newpwd);
			return ret;
		}
		else 
		{
			return NET_ERROR;
		}	
		return FAILURE;
	}
	
	//短信充值
	int ChargeUp(char* sn,char* acco,char* pass)
	{
		auto_lock lock(api.m_MuxConnObj);	
	
		if (sn == NULL) return SERIAL_ERROR; 
		if (acco == NULL || pass == NULL) return CARDISNULL_ERROR;
	
		if (!api.ValidateFlag()) return FLAG_ERROR;
		
		//4.0.4:judge 20s
    	static struct timeval lastCallTime;	
		struct timeval curCallTime;
		gettimeofday(&curCallTime,0);
		unsigned int timeuse = TIMEDELTA_MS(curCallTime,lastCallTime);
		if(timeuse < SLEEP_SECOND_20 * 1000) //20s
		{
			return FUN_CALL_TOO_FAST;
		}	
		if (ServerConnect()) 
		{
			//
			gettimeofday(&lastCallTime,0);
			//			
			int ret = api.ChargeUp(sn,acco,pass);
			return ret;
		}
		else 
		{
			return NET_ERROR;
		}	
		return FAILURE;
	}
	//取消转移功能
	int CancelTransfer(char* sn) 
	{
		auto_lock lock(api.m_MuxConnObj);	

		if (sn == NULL) return SERIAL_ERROR; 
	
		if (!api.ValidateFlag()) return FLAG_ERROR;
		
		//4.0.4:judge 20s
    	static struct timeval lastCallTime;	
		struct timeval curCallTime;
		gettimeofday(&curCallTime,0);
		unsigned int timeuse = TIMEDELTA_MS(curCallTime,lastCallTime);
		if(timeuse < SLEEP_SECOND_20 * 1000) //20s
		{
			return FUN_CALL_TOO_FAST;
		}				
		if (ServerConnect())
		{
			//
			gettimeofday(&lastCallTime,0);
			//
			int ret = api.CancelTransfer(sn);
			return ret;
		}else 
		{
			return NET_ERROR;
		}	
		return FAILURE;
	}
	//设置MO转发服务
	int RegistryTransfer(char* sn,char* phone) 
	{
		auto_lock lock(api.m_MuxConnObj);	

		if (sn == NULL) return SERIAL_ERROR; 
		if (phone == NULL) return MOBILEISNULL_ERROR;	
	
		if (!api.ValidateFlag()) return FLAG_ERROR;
		char m[10][24];
		memset(m,0,sizeof(m));
		
		char *p_m[10];
		int i = 0;
		for(i=0;i<10;i++)
		{
			p_m[i] = m[i];
		}
		
		//分解手机号码
		int j=0;
		char* pos = strchr(phone ,',');
		if (pos == NULL) 
		{
			//效验手机号码
			if (!api.ValidateMobile(phone)) 
			{
				return MOBILESPEA_ERROR;
			}
			memcpy(m[0], phone,strlen(phone));
		}
		else 
		{
			while (pos != NULL) 
			{
				memcpy(m[j], phone, pos - phone);
				//效验手机号码
				if (!api.ValidateMobile(m[j])) 
				{
					return  MOBILESPEA_ERROR;
				}
				j += 1;
				phone = pos + 1;
				pos = strchr(phone ,',');
				//最后一个号码
				if (pos == NULL && phone != NULL && strlen(phone) > 0)
				{
					memcpy(m[j], phone, strlen(phone));
					//效验手机号码
					if (!api.ValidateMobile(m[j])) 
					{
						return  MOBILESPEA_ERROR;
					}				
				}
			}
			
			
		}
		
		j++;
		for(;j<10;j++)
		{
			p_m[j] = NULL;
		}
		
		//4.0.4:judge 20s
    	static struct timeval lastCallTime;	
		struct timeval curCallTime;
		gettimeofday(&curCallTime,0);
		unsigned int timeuse = TIMEDELTA_MS(curCallTime,lastCallTime);
		if(timeuse < SLEEP_SECOND_20 * 1000) //20s
		{
			return FUN_CALL_TOO_FAST;
		}				
		if (ServerConnect()) 
		{
			//
			gettimeofday(&lastCallTime,0);
			//
			int ret = api.RegistryTransfer(sn,p_m);
			return ret;
		}
		else 
		{
			return NET_ERROR;
		}	
		return FAILURE;
	}
	//注销注册
	int UnRegister(char* sn) 
	{
		auto_lock lock(api.m_MuxConnObj);	

		if (sn == NULL) return SERIAL_ERROR; 
	
		if (!api.ValidateFlag()) return FLAG_ERROR;
		
		//4.0.4:judge 20s
    	static struct timeval lastCallTime;	
		struct timeval curCallTime;
		gettimeofday(&curCallTime,0);
		unsigned int timeuse = TIMEDELTA_MS(curCallTime,lastCallTime);
		if(timeuse < SLEEP_SECOND_20 * 1000) //20s
		{
			return FUN_CALL_TOO_FAST;
		}		
		if (ServerConnect())
		{
			//
			gettimeofday(&lastCallTime,0);
			//

			int ret = api.UnRegister(sn);
			return ret;
		}else 
		{
			return NET_ERROR;
		}	
		return FAILURE;
	}
	
	//接收短消息
	int receiveCommSMS(long commID,char* sn, MTNRecvContent* rcex,MTNRecvContentCHL* channel)
	{
		
		if (sn == NULL) return SERIAL_ERROR; 
		if(1 == commID || 2 == commID){
			if (rcex == NULL) return PARAMISNULL_ERROR;			
		}else if(3 == commID){
			if (channel == NULL) return PARAMISNULL_ERROR;	
		}
		else{
			return PARAMISNULL_ERROR;
		}
		if (!api.ValidateFlag()) return FLAG_ERROR;
		
		//4.0.4:judge 20s
    	static struct timeval lastCallTime;	
		struct timeval curCallTime;
		gettimeofday(&curCallTime,0);
		unsigned int timeuse = TIMEDELTA_MS(curCallTime,lastCallTime);
		if(timeuse < SLEEP_SECOND_10 * 1000) //10s
		{
			return 1;
		}				
		if (ServerConnect()) 
		{
			//
			gettimeofday(&lastCallTime,0);
			//
			
			int ret;
			if(1 == commID)
			{ 
				ret = api.ReceiveSMS(sn,rcex);
			}
			else if(2 == commID)
			{
				ret = api.ReceiveSMSEx(sn,rcex);
			}
			else if(3 == commID)
			{
				ret = api.ReceiveSMSCHL(sn,channel);
			}
			return ret;
		}
		else 
		{
			return NET_ERROR;
		}	
		return FAILURE;
	}
	//接收短消息
	int ReceiveSMS(char* sn, MTNRecvContent* rcex)
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = receiveCommSMS(1,sn,rcex,NULL);
		return ret;
	}
	//接收短消息
	int ReceiveSMSEx(char* sn, MTNRecvContent* rcex)
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = receiveCommSMS(2,sn,rcex,NULL);
		return ret;
	}
	//接收短消息
	int ReceiveSMSCHL(char* sn, MTNRecvContentCHL* rcex)
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = receiveCommSMS(3,sn,NULL,rcex);
		return ret;
	}	
	//接收短消息报告
	int ReceiveStatusReportEx(char* sn, MTNGetStatusReportEx* Stax) 
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		if (sn == NULL) return SERIAL_ERROR; 
		if (Stax == NULL) return PARAMISNULL_ERROR;
	
		if (!api.ValidateFlag()) return FLAG_ERROR;
		
		//4.0.4:judge 20s
    	static struct timeval lastCallTime;	
		struct timeval curCallTime;
		gettimeofday(&curCallTime,0);
		unsigned int timeuse = TIMEDELTA_MS(curCallTime,lastCallTime);
		if(timeuse < SLEEP_SECOND_20 * 1000) //20s
		{
			return 1;
		}	
		if (ServerConnect()) 
		{
			//
			gettimeofday(&lastCallTime,0);
			//
			
			int ret = api.ReceiveStatusReportEx(sn,Stax);
			return ret;
		}
		else 
		{
			return NET_ERROR;
		}	
		return FAILURE;
	}
	int sendCommSMS(int callMethod,char* sn,char* mn,char* ct,char* sendtime,char* addi,char* seqnum,char* priority)
	{		
		try
		{  
			if (sn == NULL) return SERIAL_ERROR; 
			if (mn == NULL) return MOBILEISNULL_ERROR;
	
			if (CheckCode(ct))  return SMSCONTENT_ERROR;
			if ((ct == NULL) || strlen(ct) > 1000) return SMSCONTENT_ERROR;
				
			if(	METONE_SEND_SCHEDULED_SMS_2  == callMethod
				|| METONE_SEND_SCHEDULED_SMS_EX_2  == callMethod
				|| METONE_SEND_SMS_2  == callMethod
				|| METONE_SEND_SMS_EX_2 == callMethod)
			{
				if (seqnum == NULL || strlen(seqnum) == 0 || strlen(seqnum) > 19 || !api.ValidateDigit(seqnum)) 
					 return SEQNUM_ERROR;
			}

			if (!api.ValidateFlag()) return FLAG_ERROR;
				
			if(METONE_SEND_SCHEDULED_SMS == callMethod
				|| METONE_SEND_SCHEDULED_SMS_2 == callMethod
				|| METONE_SEND_SCHEDULED_SMS_EX == callMethod
				|| METONE_SEND_SCHEDULED_SMS_EX_2 == callMethod) //SendScheSMS
			{
				if (sendtime == NULL || strlen(sendtime) != 14 || !api.ValidateDigit(sendtime)) 
					return SCHTIME_ERROR;	
			}
			
			int len = strlen(mn) + 1;
			char* phone = (char *)malloc(len);
			memset(phone,0,len);
	
			int ret = 1;
			int count = 0;
			int i = api.FilterMobile(phone,mn,&count); //过滤手机号码
			if (i == 0 || count <= 0 || count > 1000) 
			{
				free(phone);
				phone = NULL;
	
				return MOBILEISNULL_ERROR;
			}
			if (i == MOBILEPART_ERROR) ret = i;
	
			int mobilecount = strlen(phone);
	    
			if (ServerConnect()) 
			{
				//api.SetSocket(sock);
				int ii = 0;
					
				//
				if(METONE_SEND_SMS == callMethod) //SendSMS
					ii = api.SendSMS(sn,phone,ct,mobilecount,count,priority);
				else if(METONE_SEND_SMS_2 == callMethod) //SendSMS2
					ii = api.SendSMS2(sn,phone,ct,mobilecount,count,seqnum,priority);
				else if(METONE_SEND_SMS_EX == callMethod) //SendSMSEx
					ii = api.SendSMSEx(sn,phone,ct,addi,mobilecount,count,priority);
				else if(METONE_SEND_SMS_EX_2 == callMethod) //SendSMSEx2
					ii = api.SendSMSEx2(sn,phone,ct,addi,mobilecount,count,seqnum,priority);
				else if(METONE_SEND_SCHEDULED_SMS == callMethod) //SendScheSMS
					ii = api.SendScheSMS(sn,phone,ct,sendtime,mobilecount,count,priority);
				else if(METONE_SEND_SCHEDULED_SMS_2 == callMethod) //SendScheSMS2
					ii = api.SendScheSMS2(sn,phone,ct,sendtime,mobilecount,count,seqnum,priority);
				else if(METONE_SEND_SCHEDULED_SMS_EX == callMethod) //SendScheSMSEx
					ii = api.SendScheSMSEx(sn,phone,ct,sendtime,addi,mobilecount,count,priority);
				else if(METONE_SEND_SCHEDULED_SMS_EX_2 == callMethod) //SendScheSMSEx2
					ii = api.SendScheSMSEx2(sn,phone,ct,sendtime,addi,mobilecount,count,seqnum,priority);
				
				//
				if (ii == 0) 
				{	
					free(phone);
					phone = NULL;
	
					return FAILURE;
				}
				else 
				{
					free(phone);
					phone = NULL;
					
					return ii;
				}		
			}
			else
			{
				free(phone);
				phone = NULL;		
				return NET_ERROR;
			}	
		}
		catch(...)
		{
			return NET_ERROR;
		}
	}
	
	//发送短消息
	int SendSMS(char* sn,char* mn,char* ct,char* priority) 
	{	
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = sendCommSMS(METONE_SEND_SMS,sn,mn,ct,NULL,NULL,0,priority);
		return ret;
	}
	
	//发送短消息
	int SendSMS2(char* sn,char* mn,char* ct, char* seqnum,char* priority) 
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = sendCommSMS(METONE_SEND_SMS_2,sn,mn,ct,NULL,NULL,seqnum,priority);
		return ret;
	}
	//发送短消息到EUCP平台,可以带附加号码
	int SendSMSEx(char* sn,char* mn,char* ct,char* addi,char* priority)
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = sendCommSMS(METONE_SEND_SMS_EX,sn,mn,ct,NULL,addi,0,priority);
		return ret;
	}
	//发送短消息到EUCP平台,可以带附加号码
	int SendSMSEx2(char* sn,char* mn,char* ct,char* addi, char* seqnum,char* priority) 
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = sendCommSMS(METONE_SEND_SMS_EX_2,sn,mn,ct,NULL,addi,seqnum,priority);
		return ret;
	}
	
	int SendCommSMS(char* sn,char* mn,char* ct,char* addi, char* seqnum,char* priority)
	{
		int callMethod = -1;
		bool isSeqnum = false;
		bool isAddi = false;
		if(NULL == addi)
			isAddi = false;
		else
			isAddi = true;
		//
		if(0 == seqnum)
			isSeqnum = false;
		else
			isSeqnum = true;
		//
		if(isAddi == false && isSeqnum == false) callMethod = METONE_SEND_SMS;
		if(isAddi == false && isSeqnum == true)  callMethod = METONE_SEND_SMS_2;
		if(isAddi == true  && isSeqnum == false) callMethod = METONE_SEND_SMS_EX;
		if(isAddi == true  && isSeqnum == true)  callMethod = METONE_SEND_SMS_EX_2;

		int ret = sendCommSMS(callMethod,sn,mn,ct,NULL,addi,seqnum,priority);
		
		//延时
		CSockTool::wait(30);
		//

		return ret;		
	}
	
	
	//发送定时短信,时间格式yyyymmddhhnnss
	int SendScheSMS(char* sn,char* mn,char* ct,char* sendtime,char* priority) 
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = sendCommSMS(METONE_SEND_SCHEDULED_SMS,sn,mn,ct,sendtime,NULL,0,priority);
		return ret;
	}
	
	//发送定时短信,时间格式yyyymmddhhnnss
	int SendScheSMS2(char* sn,char* mn,char* ct, char* sendtime,char* seqnum,char* priority) 
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = sendCommSMS(METONE_SEND_SCHEDULED_SMS_2,sn,mn,ct,sendtime,NULL,seqnum,priority);
		return ret;
	}
	////发送定时短信,时间格式yyyymmddhhnnss 发送短消息到EUCP平台,可以带附加号码
	int SendScheSMSEx(char* sn,char* mn,char* ct,char* sendtime,char* addi,char* priority)
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = sendCommSMS(METONE_SEND_SCHEDULED_SMS_EX,sn,mn,ct,sendtime,addi,0,priority);
		return ret;
	}
	////发送定时短信,时间格式yyyymmddhhnnss 发送短消息到EUCP平台,可以带附加号码
	int SendScheSMSEx2(char* sn,char* mn,char* ct,char* sendtime,char* addi, char* seqnum,char* priority) 
	{
		auto_lock lock(api.m_MuxConnObj);	
		
		int ret = sendCommSMS(METONE_SEND_SCHEDULED_SMS_EX_2,sn,mn,ct,sendtime,addi,seqnum,priority);
		return ret;
	}
	
	int SendCommScheSMS(char* sn,char* mn,char* ct,char* sendtime,char* addi, char* seqnum,char* priority)
	{
		int callMethod = -1;
		bool isSeqnum = false;
		bool isAddi = false;
		if(NULL == addi)
			isAddi = false;
		else
			isAddi = true;
		//
		if(0 == seqnum)
			isSeqnum = false;
		else
			isSeqnum = true;
		//
		if(isAddi == false && isSeqnum == false) callMethod = METONE_SEND_SCHEDULED_SMS;
		if(isAddi == false && isSeqnum == true)  callMethod = METONE_SEND_SCHEDULED_SMS_2;
		if(isAddi == true  && isSeqnum == false) callMethod = METONE_SEND_SCHEDULED_SMS_EX;
		if(isAddi == true  && isSeqnum == true)  callMethod = METONE_SEND_SCHEDULED_SMS_EX_2;

		int ret = sendCommSMS(callMethod,sn,mn,ct,sendtime,addi,seqnum,priority);
		//延时
		CSockTool::wait(30);
		//
		return ret;				
	}
	


} //namespace end



