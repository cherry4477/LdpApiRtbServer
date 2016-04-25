/*
 * TcpSocket.cpp
 *

 */
#include <stdexcept>

#include "TcpSocket.h"
#include <errno.h>


const char Base64ValTab[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#define AVal(x) Base64ValTab[x]

extern std::string strSslKeyPath;
extern std::string strSslKeyPassWord;
CTcpSocket::CTcpSocket(const int iSockfd, const u_int uiTimeout) : m_iSockfd(iSockfd), m_uiTimeout(uiTimeout)
{
	m_in_portPort = 0;
	m_strIp.clear();
	m_bConnect = true;
}

/*
 * 鏋勯��
 */
CTcpSocket::CTcpSocket(const in_port_t in_portPort, const std::string& strIp, const u_int uiTimeout)
 : m_in_portPort(in_portPort), m_strIp(strIp), m_uiTimeout(uiTimeout)
{	
	m_iSockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("m_iSockfd=%d is created....\n",m_iSockfd);
	if (m_iSockfd == -1) {
		throw std::runtime_error("get socketfd error.");
	}
	m_bConnect = false;
}

/*
 * 鏋愭瀯
 */
CTcpSocket::~CTcpSocket() throw()
{
	struct linger so_linger;
	so_linger.l_onoff = true;
	so_linger.l_linger = 0;
	setsockopt(m_iSockfd,SOL_SOCKET,SO_LINGER,&so_linger,sizeof(so_linger));
	if(m_iSockfd != -1){
		::close(m_iSockfd);
		m_iSockfd = -1;
	}
	
}

/*
 * 鑾峰彇socket ip
 * 杩斿洖鍊硷細鐐规牸寮廼p 閿欒绌�
 */
std::string CTcpSocket::TcpGetSip()
{
	std::string strIp;
	u_int uiSinSize = sizeof(struct sockaddr_in);
	struct sockaddr_in stSockaddr;
	if (m_iSockfd > 0 && getpeername(m_iSockfd, (struct sockaddr *)&stSockaddr, &uiSinSize) == 0) {//鑾峰彇socket 鍙傛暟缁撴瀯
		strIp = inet_ntoa(stSockaddr.sin_addr);
	}
	return strIp;
}

/*
 * 鑾峰彇socket鎻忚堪绗�
 * 杩斿洖鍊硷細socket 鎻忚堪绗�
 */
int CTcpSocket::TcpGetSockfd()
{
	return m_iSockfd;
}


u_int CTcpSocket::TcpGetTimeOut()
{
	return m_uiTimeout;
}

bool CTcpSocket::TcpGetConnectStatus()
{
	return m_bConnect;
}

/*
 *鑾峰彇鏁村瀷ip
 *杩斿洖鍊硷細鏁村瀷ip  閿欒0
 */
in_addr_t CTcpSocket::TcpGetIntIp()
{
	in_addr_t in_addrIp = 0;
	u_int uiSinSize = sizeof(struct sockaddr_in);
	struct sockaddr_in stSockaddr;
	if (m_iSockfd > 0 && getpeername(m_iSockfd, (struct sockaddr *)&stSockaddr, &uiSinSize) == 0) {
		in_addrIp = stSockaddr.sin_addr.s_addr;
		
	}
	return in_addrIp;
}

std::string CTcpSocket::TcpGetServerIp()
{
	//in_addr_t in_addrServerIp = 0;
	std::string strIp;
	u_int uiSinSize = sizeof(struct sockaddr_in);
	struct sockaddr_in stSockaddr;
	if (m_iSockfd > 0 && getsockname(m_iSockfd, (struct sockaddr *)&stSockaddr, &uiSinSize) == 0) {
		//in_addrServerIp = stSockaddr.sin_addr.s_addr;
		strIp = inet_ntoa(stSockaddr.sin_addr);
	}
	return strIp;
}


/*
 * get socket port
 * Return-Value锛歴uccess local Sequence port else return 0
 */
in_port_t CTcpSocket::TcpGetPort()
{
	in_port_t in_portPort = 0;
	u_int uiSinSize = sizeof(struct sockaddr_in);
	struct sockaddr_in stSockaddr;
	if (m_iSockfd > 0 && getpeername(m_iSockfd, (struct sockaddr *)&stSockaddr, &uiSinSize) == 0) {
		in_portPort = ntohs(stSockaddr.sin_port);
	}
	return in_portPort;
}


bool CTcpSocket::TcpGetScoketOpt()
{	bool so_keep_alive;	
	int bool_len =sizeof(so_keep_alive);	
	if (-1 == getsockopt(m_iSockfd, SOL_SOCKET,SO_KEEPALIVE,(char*)&so_keep_alive,(socklen_t*)&bool_len)) 
	{		fprintf(stderr, "%s\n", strerror(errno));		
		return false;	

	}	
	printf("Line:%d,so_keep_alive=%d\n",__LINE__,so_keep_alive);	
return true;

}

bool CTcpSocket::TcpSetKeepAliveOn()
{	int iKeepAlive = 1;	
	int nRet;

	 nRet = ::setsockopt(m_iSockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&iKeepAlive, sizeof(iKeepAlive));	

	if (nRet == -1)	
	{		
		return false;	
	}	

	return true;
}


/*
 * socket 缁戝畾ip 绔彛
 * 鍙傛暟锛�
 * port: 杩炴帴绔彛
 * ip:缁戝畾ip  榛樿涓虹┖
 * 杩斿洖鍊硷細鎴愬姛true  澶辫触false
 */
bool CTcpSocket::TcpBind(const in_port_t in_portPort, const std::string& strIp)
{
	std::string strTmpIp;
	in_port_t in_portTmpPort = 0;
	if (strIp.empty()) {//鏈嶅姟鍣╥p涓虹┖
		strTmpIp = m_strIp;
	} else {
		strTmpIp = strIp;
	}
	if (in_portPort == 0) {
		in_portTmpPort = m_in_portPort;
	} else {
		in_portTmpPort = in_portPort;
	}
	struct sockaddr_in stServerAddr;
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(in_portTmpPort);
	if (strTmpIp.empty()) {//ip涓虹┖
		stServerAddr.sin_addr.s_addr = INADDR_ANY;//浠绘剰鍦板潃鐩戝惉
	} else {//ip闈炵┖
		inet_aton(strTmpIp.c_str(),&stServerAddr.sin_addr);//鎸囧畾ip娈电洃鍚�
	}
	if (::bind(m_iSockfd,(struct sockaddr *)&stServerAddr,sizeof(struct sockaddr)) == -1) {//缁戝畾澶辫触
		return false;
	}
	return true;
}

/*
 * socket 鐩戝惉
 * 鍙傛暟锛�
 * reqNum: 鐩戝惉闃熷垪鏈�澶ч暱搴︼紝 榛樿鐢辩郴缁熸帶鍒�
 * 杩斿洖鍊�:鎴愬姛true 澶辫触false
 */
bool CTcpSocket::TcpListen(const int iReqNum)
{
	if (::listen(m_iSockfd, iReqNum) < 0) {
		return false;
	}
	return true;
}

/*
 * server 鎺ユ敹client 杩炴帴
 * 鍙傛暟锛�
 * 杩斿洖鍊硷細鎴愬姛锛氬鎴风socket 鎸囬拡, 澶辫触锛氱┖
 */
CTcpSocket* CTcpSocket::TcpAccept()
{
	if (m_iSockfd < 0) {//鎻忚堪绗﹂敊璇�
		return false;
	}
	struct sockaddr_in stClientAddr;
	u_int uiSinSize = sizeof(struct sockaddr_in);
	int iclientfd = -1;
	if((iclientfd = ::accept(m_iSockfd, (struct sockaddr *)&stClientAddr, &uiSinSize)) < 0) {//accept澶辫触
		return false;
	}
	return new CTcpSocket(iclientfd, m_uiTimeout); 
}

/*
 * 甯︽湁瓒呮椂鐨勭洃鍚�
 * 鍙傛暟锛歱socket 淇濆瓨瀹㈡埛绔殑socket瀵硅薄鎸囬拡
 * 杩斿洖鍊硷細锛�1 鍑洪敊  0 瓒呮椂  1鎴愬姛
 */
int CTcpSocket::TcpTimeoutAccept(CTcpSocket*& pEmSocket)
{
	struct sockaddr_in stClientAddr;
	u_int uiSinSize = sizeof(struct sockaddr_in);
	struct timeval stTimeval;
	fd_set fdReadfds;
	stTimeval.tv_sec = m_uiTimeout;
	stTimeval.tv_usec = 0;
	FD_ZERO(&fdReadfds);
	FD_SET(m_iSockfd, &fdReadfds);
	while(true) {
		switch(select(m_iSockfd + 1, &fdReadfds, 0, NULL, &stTimeval))
		{
		case -1://select鍑洪敊
			return -1;
			break;
		case 0://鐩戝惉瓒呮椂
			return 0;
			break;
		default:
			if (FD_ISSET(m_iSockfd, &fdReadfds)) {
				int iClientfd = -1;
				if ((iClientfd = ::accept(m_iSockfd, (struct sockaddr *) &stClientAddr, &uiSinSize)) == -1) {
					return -1;
				}
				pEmSocket = new CTcpSocket(iClientfd, m_uiTimeout);//鍒涘缓瀹㈡埛绔璞�
				return 1;
			}
			//LOG(ERROR, "which readfd is reading?!");
			return -1;
			break;
		}
	}
	return -1;
}

/*
 * socket杩炴帴鏈嶅姟鍣�
 * 鍙傛暟锛� 绔彛  ip
 * 杩斿洖鍊硷細-1 鍑洪敊  -2 瓒呮椂  -3 server鏈紑 0 鎴愬姛
 */
int CTcpSocket::TcpConnect(const in_port_t in_portPort, const std::string& strIp)
{
 
	if(m_iSockfd == -1) {//鎻忚堪绗﹂敊璇�
		return -1;
	}
	struct sockaddr_in stAddress;
	stAddress.sin_family = AF_INET;
	inet_aton(m_strIp.c_str(), &stAddress.sin_addr);
	stAddress.sin_port = htons(m_in_portPort);
	struct timeval stTimeval;
	//stTimeval.tv_sec = m_uiTimeout;
	//stTimeval.tv_usec = 0;
	
	stTimeval.tv_sec = 5;
	stTimeval.tv_usec = m_uiTimeout;
	if (-1 == setsockopt(m_iSockfd, SOL_SOCKET, SO_SNDTIMEO, &stTimeval, sizeof(stTimeval))) {
		return -1;
	}

	if (-1 == setsockopt(m_iSockfd, SOL_SOCKET, SO_RCVTIMEO, &stTimeval, sizeof(stTimeval))) {
		return -1;
	}

	if( ::connect(m_iSockfd, (struct sockaddr *)&stAddress, sizeof(stAddress)) < 0) {

		return -1;
	}
	m_bConnect = true;
	return 0;
}

/*
 * 閲嶈繛socket
 * 鍙傛暟锛�
 * sockfd : 閲嶈繛鍙ユ焺
 * ip:server銆�ip
 * port: server Port
 * 杩斿洖鍊硷細鎴愬姛true 澶辫触false
 */
int CTcpSocket::TcpReconnect()
{
	if(m_bConnect) {
		::close(m_iSockfd);//鍏虫帀鍘熸湁鐨勮繛鎺�
		m_iSockfd = -1;
		m_bConnect = false;
		m_iSockfd = socket(AF_INET,SOCK_STREAM,0);
		if(m_iSockfd < 0) {//鍒涘缓socket澶辫触
			return -1;
		}
	}
	return TcpConnect();
}

/*
 * socket璇诲彇鏁版嵁
 * 鍙傛暟锛�
 * recvBuf:淇濆瓨璇诲彇鏁版嵁buf
 * recvLen:璇诲彇闀垮害
 * 杩斿洖鍊硷細鎴愬姛杩斿洖璇诲彇鍒扮殑闀垮害   澶辫触杩斿洖-1  瓒呮椂锛�2 璇诲彇瀵硅薄涓嶅瓨鍦� 0
 */

int CTcpSocket::TcpRead(void *const pvRecvBuf, size_t sizeRecvLen)
{
	if(pvRecvBuf == NULL || sizeRecvLen <= 0 || m_iSockfd == -1) {//鍙傛暟鏈夎
		return -1;
	}
	struct timeval stTimeout;
	stTimeout.tv_sec =3;// m_uiTimeout;
	stTimeout.tv_usec = m_uiTimeout;
	//int sock_buf_size=9012;

if(::setsockopt(m_iSockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&stTimeout, sizeof(stTimeout)) == -1) {//璁剧疆璇诲彇瓒呮椂 澶辫触
		return -1;
	}
	/*
	if(::setsockopt(m_iSockfd, SOL_SOCKET,SO_RCVBUF,(char*)&sock_buf_size, sizeof(sock_buf_size)) == -1) {//璁剧疆璇诲彇瓒呮椂 澶辫触
		return -1;
	}
	*/
	int res = ::recv(m_iSockfd, pvRecvBuf, sizeRecvLen, 0);
	switch(res)
	{
	case -1:
		if(errno == EAGAIN) {//瓒呮椂
			errno = 0;
			return -2;
		}//else鍑洪敊
		return -1;
		break;
	case 0://socket 璇诲彇瀵硅薄涓嶅瓨鍦�
		return 0;
		break;
	default:
		if(res < 0)
			return -1;
		break;
	}
	return res;
}

/*
 * socket璇诲彇鏁版嵁
 * 鍙傛暟锛�
 * recvBuf:淇濆瓨璇诲彇鏁版嵁buf
 * recvLen:璇诲彇闀垮害
 * 杩斿洖鍊硷細鎴愬姛杩斿洖鎺ユ敹闀垮害   澶辫触杩斿洖-1  瓒呮椂锛�2 璇诲彇瀵硅薄涓嶅瓨鍦� 0
 */
int CTcpSocket::TcpReadAll(void *const pvRecvBuf, size_t sizeRecvLen)
{
	if(pvRecvBuf == NULL || sizeRecvLen == 0) {
		return -1;
	}
	int ilen = 0;
	int ilength = 0;
	while((ilength = TcpRead(((char *)pvRecvBuf + ilen), sizeRecvLen - ilen)) > 0)
	{//璇诲彇鎵�鏈夌殑socket 鏁版嵁
		ilen += ilength;//宸茶鍙栫殑闀垮害
		if((u_int)ilen >= sizeRecvLen) {//璇诲彇recvLen瀹屾瘯
			break;
		}
	}
	if(ilength < 0) {
		return ilength;
	}
	return ilen;
}


bool CTcpSocket::TcpWrite(const void *pvSendBuf,size_t sizeSendLen)
{
	if(!pvSendBuf || sizeSendLen <= 0) {//鍙傛暟鏈夎
		return false;
	}
	if(::send(m_iSockfd, pvSendBuf, sizeSendLen,0) != (int)sizeSendLen) {//鍙戦�佸け璐�
		return false;
	}
	return true;
}


bool CTcpSocket:: TcpClose()
{
	
	struct linger so_linger;
	so_linger.l_onoff = true;
	so_linger.l_linger = 0;
	printf("m_iSockfd=%d is closing....\n",m_iSockfd);
	setsockopt(m_iSockfd,SOL_SOCKET,SO_LINGER,&so_linger,sizeof(so_linger));
	if(m_bConnect) {
		if(m_iSockfd != -1)
		{
			::close(m_iSockfd);
			m_iSockfd=-1;
		}
		m_bConnect = false;
	}
	return true;
}
int CTcpSocket::EncodingBase64(char * pInput, char * pOutput)
{
 int i = 0;
 int loop = 0;
 int remain = 0;
 int iDstLen = 0;
 int iSrcLen = (int)strlen(pInput);

 loop = iSrcLen/3;
 remain = iSrcLen%3;

 // also can encode native char one by one as decode method
 // but because all of char in native string  is to be encoded so encode 3-chars one time is easier.

 for (i=0; i < loop; i++)
 {
  char a1 = (pInput[i*3] >> 2);
  char a2 = ( ((pInput[i*3] & 0x03) << 4) | (pInput[i*3+1] >> 4) );
  char a3 = ( ((pInput[i*3+1] & 0x0F) << 2) | ((pInput[i*3+2] & 0xC0) >> 6) );
  char a4 = (pInput[i*3+2] & 0x3F);

  pOutput[i*4] = AVal(a1);
  pOutput[i*4+1] = AVal(a2);
  pOutput[i*4+2] = AVal(a3);
  pOutput[i*4+3] = AVal(a4);
 }

 iDstLen = i*4;

 if (remain == 1)
 {
  // should pad two equal sign
  i = iSrcLen-1;
  char a1 = (pInput[i] >> 2);
  char a2 = ((pInput[i] & 0x03) << 4);

  pOutput[iDstLen++] = AVal(a1);
  pOutput[iDstLen++] = AVal(a2);
  pOutput[iDstLen++] = '=';
  pOutput[iDstLen++] = '=';
  pOutput[iDstLen] = 0x00;
 }
 else if (remain == 2)
 {
  // should pad one equal sign
  i = iSrcLen-2;
  char a1 = (pInput[i] >> 2);
  char a2 = ( ((pInput[i] & 0x03) << 4) | (pInput[i+1] >> 4));
  char a3 = ( (pInput[i+1] & 0x0F) << 2);

  pOutput[iDstLen++] = AVal(a1);
  pOutput[iDstLen++] = AVal(a2);
  pOutput[iDstLen++] = AVal(a3);
  pOutput[iDstLen++] = '=';
  pOutput[iDstLen] = 0x00;
 }
 else
 {
  // just division by 3
  pOutput[iDstLen] = 0x00;
 }

 return iDstLen;
}
int CTcpSocket::TcpSendEmail(char * smtpServer,int port,char* username,char * password,char* from,char * to,char * subject,char * body)
{

/*
	printf("port=%d,ip=%s\n",port,smtpServer);
	CTcpSocket(port,std::string(smtpServer));

	if(!TcpConnect(port,std::string(smtpServer)))
	{	
		printf("connect email info error\n");
		return -1;
	}

*/
 printf("connect email succefully\n");
 
 char buffer[1024]; 
 char recvBuff[1024*8]; 
 std::string strRcptTo;
 memset(buffer,0,sizeof(buffer));
 memset(recvBuff,0,sizeof(recvBuff));
 printf("from=%s,len %d\n",from,strlen(from));
 sprintf(buffer,"EHLO %s\n",from);//from为char数据。存储发送地址
 printf("buffer=%s,len=%d\n",buffer,strlen(buffer));
 if(!TcpWrite(buffer,strlen(buffer)))
 {
	printf("send EHLO error\n");
	return -1;
 }

 printf("send succefully\n");
 TcpReadAll(recvBuff,1024*8);
 printf("recvBuff=%s\n",recvBuff);

 sprintf(buffer,"AUTH LOGIN\r\n");
 TcpWrite(buffer,strlen(buffer));
 TcpReadAll(recvBuff,1024*8);
 printf("recvBuff=%s\n",recvBuff);


 //USER NAME
 //User name is coded by base64.
 //base64_encode_2(username,buffer);//先将用户帐号经过base64编码
 EncodingBase64(username,buffer);
 printf("username buffer=%s\n",buffer);
 strcat(buffer,"\r\n");
 TcpWrite(buffer,strlen(buffer));
 TcpReadAll(recvBuff,1024*8);
 printf("recvBuff=%s\n",recvBuff);


 //password coded by base64
 EncodingBase64(password,buffer);
 printf("password buffer=%s\n",buffer);
 strcat(buffer,"\r\n");
 TcpWrite(buffer,strlen(buffer));
 TcpReadAll(recvBuff,1024*8);
 printf("recvBuff=%s\n",recvBuff);



 sprintf(buffer,"MAIL FROM:<%s>\r\n",from);
 TcpWrite(buffer,strlen(buffer));
 TcpReadAll(recvBuff,1024*8);
 printf("recvBuff=%s\n",recvBuff);



int index=0;
char p[100][2048];
char *buf = to;
std::string strBuffer = to;
std::string strAddress;
printf("strBuffer=%s\n",strBuffer.c_str());
int iBegin = 0;
int iPos ;

printf("iBegin=%d\n",iBegin);
while((iPos=strBuffer.find(",",iBegin))!= -1 ) 
{	

	printf("iPos=%d\n",iPos);
	strAddress = strBuffer.substr(iBegin,iPos-iBegin);
	printf("strAddress=%s\n",strAddress.c_str());
	memset(p[index],0,sizeof(p[index]));
	sprintf(p[index],"%s",strAddress.c_str());
	index++;
	sprintf(buffer,"RCPT TO: <%s>\r\n",strAddress.c_str());
	TcpWrite(buffer,strlen(buffer));
	TcpReadAll(recvBuff,1024*8);
	printf("recvBuff=%s\n",recvBuff);
	iBegin = iPos+1;
	printf("iBegin=%d\n",iBegin);
	memset(buffer,0,sizeof(buffer));
}

	strAddress = strBuffer.substr(iBegin);
	memset(p[index],0,sizeof(p[index]));
	sprintf(p[index],"%s",strAddress.c_str());
	index++;
	printf("index=%d\n",index);
	printf("strAddress ttttt=%s\n",strAddress.c_str());
	sprintf(buffer,"RCPT TO: <%s>\r\n",strAddress.c_str());
	TcpWrite(buffer,strlen(buffer));
	TcpReadAll(recvBuff,1024*8);
	printf("recvBuff=%s\n",recvBuff);


/*
while((p[index]=strtok(buf,","))!=NULL) 
{	
	printf("list:%s\n",p[index]);
	sprintf(buffer,"RCPT TO: <%s>\r\n",p[index]);
	TcpWrite(buffer,strlen(buffer));
	TcpReadAll(recvBuff,1024*8);
	printf("recvBuff=%s\n",recvBuff);
	index++;
	buf=NULL; 
}
*/


 printf("===p[0]====%s\n",p[0]);
 printf("===p[1]====%s\n",p[1]);
 printf("===p[2]====%s\n",p[2]);
 
 //memset(buffer,0,sizeof(buffer));
 sprintf(buffer,"DATA\r\n");
 TcpWrite(buffer,strlen(buffer));
 TcpReadAll(recvBuff,1024*8);
 printf("recvBuff=%s\n",recvBuff);

 //DATA from
 sprintf(buffer,"From: %s\r\n",from);
 TcpWrite(buffer,strlen(buffer));

 //DATA to
 for(int i=0;i<index;i++)
 {
 printf("===p[%d]====%s\n",i,p[i]);
 memset(buffer,0,sizeof(buffer));
 sprintf(buffer,"To: %s\r\n",p[i]);
 TcpWrite(buffer,strlen(buffer));
 }
 


  //DATA cc
 //sprintf(buffer,"cc: %s\r\n","suwk@asiainfo.com");
 //TcpWrite(buffer,strlen(buffer));

 //DATA head
 sprintf(buffer,"Subject: %s\n\r\n",subject);
 TcpWrite(buffer,strlen(buffer));


 //DATA body
 sprintf(buffer,"%s\r\n.\r\n",body);
 TcpWrite(buffer,strlen(buffer));
 printf("recvBuff=%s\n",recvBuff);

 strcpy(buffer,"QUIT\r\n");
 TcpWrite(buffer,strlen(buffer));
 TcpReadAll(recvBuff,1024*8);
 printf("recvBuff=%s\n",recvBuff);

 //TcpClose();
 
 return 0;


}


bool CTcpSocket:: TcpCloseFinal()
{
	struct linger so_linger;
	so_linger.l_onoff = true;
	so_linger.l_linger = 0;
	setsockopt(m_iSockfd,SOL_SOCKET,SO_LINGER,&so_linger,sizeof(so_linger));
		if(m_iSockfd != -1)
		{
			::close(m_iSockfd);
			m_iSockfd=-1;
		}
		m_bConnect = false;
	return true;
}

/*
 *
 */
bool CTcpSocket::TcpNewSocket()
{
	if(m_bConnect) {
		::close(m_iSockfd);
		m_bConnect = false;
	}
	m_iSockfd = socket(AF_INET,SOCK_STREAM,0);
	if(m_iSockfd == -1) {
		return false;
	}
	return true;
}

/*
 * set socket ip
 */
void CTcpSocket::TcpSetIp(const std::string& strIp)
{
	m_strIp = strIp;
}

/*
 * set socket port
 */
void CTcpSocket::TcpSetPort(const in_port_t in_portPort)
{
	m_in_portPort = in_portPort;
}

/*
 * set socket timeout
 */
void CTcpSocket::TcpSetTimeout(const u_int uiTimeout)
{
	m_uiTimeout = uiTimeout;
}

bool CTcpSocket::TcpIsConnect()
{
	return m_bConnect;
}

int CTcpSocket::TcpGetSocket()
{
	return m_iSockfd;
}
bool CTcpSocket::TcpSetNoblock()
{
	if (-1 == fcntl(m_iSockfd, F_SETFL, fcntl(m_iSockfd, F_GETFD, 0) | O_NONBLOCK)) {
		fprintf(stderr, "%s\n", strerror(errno));
		return false;
	}
	return true;
}

void CTcpSocket::TcpSslInitParams()
{
	ssl = NULL;
	ssl_ctx = NULL;
	ssl_method = NULL;
	server_cert = NULL;
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
}

int CTcpSocket::TcpSslInitEnv()
{
		/*
		if( SSL_library_init()< 0 )
		{
			return -1;
		}

		OpenSSL_add_all_algorithms();
		OpenSSL_add_all_ciphers();
	//SSL_load_error_strings();
		*/

	//ssl_method = const_cast<SSL_METHOD *>(SSLv23_client_method());
	
		ssl_ctx = SSL_CTX_new(SSLv23_client_method());
		//ssl_ctx = SSL_CTX_new(TLSv1_method());
	  				
	  				#if 1
						/* Load the RSA CA certificate into the SSL_CTX structure */
						/* This will allow this client to verify the server's   */
						/* certificate.                             */
						SSL_CTX_load_verify_locations(ssl_ctx, strSslKeyPath.c_str(), NULL); 
						/* Set flag in context to require peer (server) certificate verification */
						SSL_CTX_set_verify(ssl_ctx,SSL_VERIFY_NONE,NULL); //鏄惁瑕佹眰瀵圭楠岃瘉 榛樿 SSL_VERIFY_NONE  ;SSL_VERIFY_PEER
						
						SSL_CTX_set_verify_depth(ssl_ctx,1);
						SSL_CTX_set_default_passwd_cb_userdata(ssl_ctx, (void*)strSslKeyPassWord.c_str());
						//SSL_CTX_set_default_passwd_cb_userdata(ssl_ctx, (void*)"dataMining");
						//璇诲彇璇佷功鏂囦欢
						SSL_CTX_use_certificate_file(ssl_ctx,strSslKeyPath.c_str(),SSL_FILETYPE_PEM);
						//璇诲彇瀵嗛挜鏂囦欢
						SSL_CTX_use_PrivateKey_file(ssl_ctx,strSslKeyPath.c_str(),SSL_FILETYPE_PEM);
						
						//楠岃瘉瀵嗛挜鏄惁涓庤瘉涔︿竴鑷�
						if(!SSL_CTX_check_private_key(ssl_ctx))
						{
								ERR_print_errors_fp(stderr);	
						}
								 
						#endif
		ssl = SSL_new(ssl_ctx);
	//openssl鐨勬枃妗ｄ笂涔熸槑鏂囪瀹氫笉鑳藉皢涓�涓猄SL鎸囬拡鐢ㄤ簬澶氫釜绾跨▼锛屾墍鏈夎皟鐢–reateThread鍑芥暟鍒涘缓绾跨▼锛屽弬鏁拌缃负SSL鎸囬拡蹇呯劧鍦ㄧ嚎绋嬩腑鏄簰鏂ョ殑锛岃�冭檻杩愮敤windows寮�婧愬簱pthread鏀归�犲绾跨▼
		return 0;
}

bool CTcpSocket::TcpSslConnect()
{	
	int s1=SSL_set_fd(ssl,m_iSockfd);					
	int s2=SSL_connect(ssl);
	//printf("s1=%d,s2=%d\n",s1,s2);
	if( s1!=1 || s2!=1 )
	{
		//ERR_print_errors_fp(stderr);
		return false;
	}
	
	return true;

}

int CTcpSocket::TcpSslReadLen( void* buf, int nBytes)
{
        int nleft;
        int nread;
        //char *ptr;
        //ptr = buf;
        nleft = nBytes;
        
        
        do {
        		
                nread=SSL_read(ssl, buf,nleft);
                switch(SSL_get_error(ssl,nread))
                {
                        case SSL_ERROR_NONE://ret >0 read all
                                //printf("SSL_ERROR_NONE\n");
                                nleft = 0;
                                break;
                        case SSL_ERROR_ZERO_RETURN://close
                                //printf("SSL_ERROR_ZERO_RETURN\n");
                                nread=0;//ret 0
                                //goto end;
                                break;
                        case SSL_ERROR_WANT_READ://try again
                                //printf("SSL_ERROR_WANT_READ\n");
                                //if(nread<0)
                                //nread = 0;
                                //break;
                                continue;
                        case SSL_ERROR_WANT_WRITE:
                                //printf("SSL_ERROR_WANT_WRITE\n");//try again
                                break;
                        default:
                        		printf("SSL read problem\n");
                                return -1;
                
                }//end switch
//end:

                nleft=nleft-nread;
				//int sss=SSL_pending(ssl);
        }while( nleft > 0 );

        return (nBytes-nleft);
}


int CTcpSocket::TcpSslWriteLen(void* buf, int nBytes)
{
        int nleft;
        int nwrite ;
        char *ptr;
        
        ptr=(char*)buf;
        nleft=nBytes;
        
        
        do {
                nwrite=SSL_write(ssl, ptr,nleft);
                switch(SSL_get_error(ssl,nwrite))
                {
                        case SSL_ERROR_NONE://ret >0 send all
                                //printf("SSL_ERROR_NONE\n");
                                break;
                        case SSL_ERROR_ZERO_RETURN://close
                                //printf("SSL_ERROR_ZERO_RETURN\n");
                                nwrite=0;//ret 0
                                //goto end;
                                break;
                        case SSL_ERROR_WANT_READ://try again
                                //printf("SSL_ERROR_WANT_READ\n");
                                break;
                        case SSL_ERROR_WANT_WRITE:
                               // printf("SSL_ERROR_WANT_WRITE\n");//try again
                                break;
                        default:
					ERR_print_errors_fp(stderr); 
					printf("%s\n",strerror(errno));
                        		printf("SSL write problem\n");
                                return -1;
                                
                
                }//end switch
//end:
                nleft = nleft-nwrite;
                ptr = ptr + nwrite;
        }while( nleft>0 );

        return (nBytes-nleft);
}

void CTcpSocket::TcpSslDestroy()
{
		SSL_shutdown(ssl);
		SSL_free(ssl);
		TcpClose(); 	  
		SSL_CTX_free(ssl_ctx);
}
