/*****************************************************************
ファイル名	: client_net.c
機能		: クライアントのネットワーク処理
*****************************************************************/

#include"common.h"
#include"client_func.h"
#include<sys/socket.h>
#include<netdb.h>

#define	BUF_SIZE	100

static int	gSocket;	/* ソケット */
static fd_set	gMask;	/* select()用のマスク */
static int	gWidth;		/* gMask中のチェックすべきビット数 */

static void GetAllName(void);
static void SetMask(void);
static int RecvData(void *data,int dataSize);

/*****************************************************************
関数名	: SetUpClient
機能	: サーバーとのコネクションを設立し，
		  ユーザーの名前の送受信を行う
引数	: char	*hostName		: ホスト
出力	: コネクションに失敗した時-1,成功した時0
*****************************************************************/
int SetUpClient(char *hostName)
{
    struct hostent	*servHost;
    struct sockaddr_in	server;
    int			len;
    char		str[BUF_SIZE];

    /* ホスト名からホスト情報を得る */
    if((servHost = gethostbyname(hostName))==NULL){
		fprintf(stderr,"Unknown host\n");
		return -1;
    }

    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    bcopy(servHost->h_addr,(char*)&server.sin_addr,servHost->h_length);

    /* ソケットを作成する */
    if((gSocket = socket(AF_INET,SOCK_STREAM,0)) < 0){
		fprintf(stderr,"socket allocation failed\n");
		return -1;
    }

    /* サーバーと接続する */
    if(connect(gSocket,(struct sockaddr*)&server,sizeof(server)) == -1){
		fprintf(stderr,"cannot connect\n");
		close(gSocket);
		return -1;
    }
    fprintf(stderr,"connected\n");

    /* 名前を読み込みサーバーに送る */
    do{
		printf("Enter Your Name\n");
		fgets(str,BUF_SIZE,stdin);
		len = strlen(str)-1;
		str[len]='\0';
    }while(len>MAX_NAME_SIZE-1 || len==0);
    SendData(str,MAX_NAME_SIZE);

    printf("Please Wait\n");

    /* 全クライアントのユーザー名を得る */
    GetAllName();

    /* select()のためのマスク値を設定する */
    SetMask();
    
    return 0;
}

/*****************************************************************
関数名	: SendRecvManager
機能	: サーバーから送られてきたデータを処理する
引数	: なし
出力	: プログラム終了コマンドが送られてきた時0を返す．
		  それ以外は1を返す
*****************************************************************/
int SendRecvManager(void)
{
    fd_set	readOK;
    char	command;
    int		i;
    int		endFlag = 1;
    struct timeval	timeout;

    /* select()の待ち時間を設定する */
    timeout.tv_sec = 0;
    timeout.tv_usec = 2;

    readOK = gMask;
    /* サーバーからデータが届いているか調べる */
    select(gWidth,&readOK,NULL,NULL,&timeout);
    if(FD_ISSET(gSocket,&readOK)){
		/* サーバーからデータが届いていた */
    	/* コマンドを読み込む */
		RecvData(&command,sizeof(char));
    	/* コマンドに対する処理を行う */
		endFlag = ExecuteCommand(command);
    }
    return endFlag;
}

/*****************************************************************
関数名	: RecvIntData
機能	: サーバーからint型のデータを受け取る
引数	: int		*intData	: 受信したデータ
出力	: 受け取ったバイト数
*****************************************************************/
int RecvIntData(int *intData)
{
    int n,tmp;
    
    /* 引き数チェック */
    assert(intData!=NULL);

    n = RecvData(&tmp,sizeof(int));
    (*intData) = ntohl(tmp);
    
    return n;
}

/*****************************************************************
関数名	: RecvCharaInfoData
機能	: サーバーからCharaInfo型のデータを受け取る
引数	: int		*charaInfoData	: 受信したデータ
出力	: 受け取ったバイト数
*****************************************************************/
int RecvCharaInfoData(CharaInfo *charaInfoData)
{
    int n;
    
    /* 引き数チェック */
    assert(charaInfoData!=NULL);

    n = RecvData(charaInfoData,sizeof(CharaInfo));
    
    return n;
}

/*****************************************************************
関数名	: RecvFieldInfoData
機能	: サーバーからFieldInfo型のデータを受け取る
引数	: int		*fieldInfoData	: 受信したデータ
出力	: 受け取ったバイト数
*****************************************************************/
int RecvFieldInfoData(FieldInfo *fieldInfoData)
{
    int n;
    
    /* 引き数チェック */
    assert(fieldInfoData!=NULL);

    n = RecvData(fieldInfoData,sizeof(FieldInfo));
    
    return n;
}

/*****************************************************************
関数名	: SendData
機能	: サーバーにデータを送る
引数	: void		*data		: 送るデータ
		  int		dataSize	: 送るデータのサイズ
出力	: なし
*****************************************************************/
void SendData(void *data,int dataSize)
{
    /* 引き数チェック */
    assert(data != NULL);
    assert(0 < dataSize);

    write(gSocket,data,dataSize);
}

/*****************************************************************
関数名	: CloseSoc
機能	: サーバーとのコネクションを切断する
引数	: なし
出力	: なし
*****************************************************************/
void CloseSoc(void)
{
    printf("...Connection closed\n");
    close(gSocket);
}

/*****
static
*****/
/*****************************************************************
関数名	: GetAllName
機能	: サーバーから全クライアントのユーザー名を受信する
引数	: なし
出力	: なし
*****************************************************************/
static void GetAllName(void)
{
    int	i;

    /* クライアント番号の読み込み */
    RecvIntData(&clientID);
    /* クライアント数の読み込み */
    RecvIntData(&gClientNum);

    /* 全クライアントのユーザー名を読み込む */
    for(i=0;i<(gClientNum);i++){
		RecvData(gName[i],MAX_NAME_SIZE);
    }
#ifndef NDEBUG
    printf("#####\n");
    printf("client number = %d\n",gClientNum);
    for(i=0;i<gClientNum;i++){
		printf("%d:%s\n",i,gName[i]);
    }
#endif
}

/*****************************************************************
関数名	: SetMask
機能	: select()のためのマスク値を設定する
引数	: なし
出力	: なし
*****************************************************************/
static void SetMask(void)
{
    int	i;

    FD_ZERO(&gMask);
    FD_SET(gSocket,&gMask);

    gWidth = gSocket+1;
}

/*****************************************************************
関数名	: RecvData
機能	: サーバーからデータを受け取る
引数	: void		*data		: 受信したデータ
	  int		dataSize	: 受信するデータのサイズ
出力	: 受け取ったバイト数
*****************************************************************/
int RecvData(void *data,int dataSize)
{
    /* 引き数チェック */
    assert(data != NULL);
    assert(0 < dataSize);

    return read(gSocket,data,dataSize);
}

