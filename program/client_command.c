/*****************************************************************
ファイル名	: client_command.c
機能		: クライアントのコマンド処理
*****************************************************************/

#include"common.h"
#include"client_func.h"


static int inputEnableFlag = 1; // 入力可否フラグ

static void SetIntData2DataBlock(void *data,int intData,int *dataSize);
static void SetCharData2DataBlock(void *data,char charData,int *dataSize);
static void SetInputInfo2DataBlock(void *data,InputInfo inputInfoData,int *dataSize); // 入力された情報の構造体をセットする関数(安村)
static void RecvCharaData_y(void); // 全員のキャラの構造体を取得する関数(安村)
static void RecvFieldData_y(void); // フィールドの構造体を取得する関数(安村)
static void DrawFrame_y(void);

/*****************************************************************
関数名	: ExecuteCommand
機能	: サーバーから送られてきたコマンドを元に，
		  引き数を受信し，実行する
引数	: char	command		: コマンド
出力	: プログラム終了コマンドがおくられてきた時には0を返す．
		  それ以外は1を返す
*****************************************************************/
int ExecuteCommand(char command)
{
    int	endFlag = 1;
#ifndef NDEBUG
    printf("#####\n");
    printf("ExecuteCommand()\n");
    printf("command = %c\n",command);
#endif
    switch(command){
		case END_COMMAND:
			endFlag = 0;
			break;
	    case HAND_COMMAND:
			//RecvHandData();
			RecvCharaData_y(); // 全員のキャラの構造体を取得
			RecvFieldData_y(); // フィールドの構造体を取得
			break;
		case FRAME_COMMAND:
			DrawFrame_y();
			SendInputInfoCommand_y();
			break;
		case TITLE_COMMAND:
			RecvCharaData_y(); // 全員のキャラの構造体を取得
			RecvFieldData_y(); // フィールドの構造体を取得
			printf("OKBokujo\n");
			BlitWindow(); // 描画
			break;
		case CHARASEL_COMMAND:
			RecvCharaData_y(); // 全員のキャラの構造体を取得
			RecvFieldData_y(); // フィールドの構造体を取得
			BlitWindow(); // 描画
			break;
    }
    return endFlag;
}

/*****************************************************************
関数名	: SendInputInfoCommand_y
機能	: 入力された方向や攻撃をデータとして送る
引数	: なし
出力	: なし
*****************************************************************/
void SendInputInfoCommand_y(void)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

	if(inputEnableFlag == 0)
		return;
	inputEnableFlag = 0;
    /* 引き数チェック TODO 削除*/
    //assert(0<=input.hand && input.hand<3);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendInputInfoCommand()\n");
    printf("Send InputInfo Command\n");
#endif

    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,SEND_COMMAND,&dataSize);

	SetInputInfo2DataBlock(data,gInput,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
	
}

/*****************************************************************
関数名	: SendTitleCommand
機能	: サーバにタイトル画面でエンターが押されたことを通知する
引数	: なし
出力	: なし
*****************************************************************/
void SendTitleCommand(void)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendTitleCommand()\n");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,TITLE_COMMAND,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}

/*****************************************************************
関数名	: SendCharaSelCommand
機能	: サーバにキャラ選択が行われたことを通知する
引数	: なし
出力	: なし
*****************************************************************/
void SendCharaSelCommand(CharaType type)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendTitleCommand()\n");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data, CHARASEL_COMMAND, &dataSize);
	SetIntData2DataBlock(data, type, &dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}

/*****************************************************************
関数名	: SendBackTitleCommand
機能	: サーバにタイトルに戻ることを通知する
引数	: なし
出力	: なし
*****************************************************************/
void SendBackTitleCommand(void)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendBackTitleCommand()\n");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data, BACKTITLE_COMMAND, &dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}

/*****************************************************************
関数名	: SendEndCommand
機能	: プログラムの終了を知らせるために，
		  サーバーにデータを送る
引数	: なし
出力	: なし
*****************************************************************/
void SendEndCommand(void)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendEndCommand()\n");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,END_COMMAND,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}

/*****
static
*****/
/*****************************************************************
関数名	: SetIntData2DataBlock
機能	: int 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  int		intData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetIntData2DataBlock(void *data,int intData,int *dataSize)
{
    int tmp;

    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    tmp = htonl(intData);

    /* int 型のデータを送信用データの最後にコピーする */
    memcpy(data + (*dataSize),&tmp,sizeof(int));
    /* データサイズを増やす */
    (*dataSize) += sizeof(int);
}

/*****************************************************************
関数名	: SetCharData2DataBlock
機能	: char 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  int		intData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetCharData2DataBlock(void *data,char charData,int *dataSize)
{
    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    /* char 型のデータを送信用データの最後にコピーする */
    *(char *)(data + (*dataSize)) = charData;
    /* データサイズを増やす */
    (*dataSize) += sizeof(char);
}

/*****************************************************************
関数名	: SetInputInfo2DataBlock
機能	: InputInfo 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  InputInfo		inputInfoData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetInputInfo2DataBlock(void *data,InputInfo inputInfoData,int *dataSize)
{
    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    /* char 型のデータを送信用データの最後にコピーする */
    *(InputInfo *)(data + (*dataSize)) = inputInfoData;
    /* データサイズを増やす */
    (*dataSize) += sizeof(InputInfo);
}

/*****************************************************************
関数名	: RecvCharaData_y
機能	: 全員のキャラのデータを受信する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvCharaData_y(void)
{
	int i;
	printf("OK\n");
	for(i = 0; i < gClientNum; i++){ // 全てのクライアントに対して
		// TODO キャラの構造体をセット
		RecvCharaInfoData(&gChara[i]);

//		gImage[i].anipat = gChara[i].anipat; // koko
		#ifndef NDEBUG
		printf("#####\nRecvCharaData() RESULT\n");
		//printf("client[%d] : hand %d result %d\n",i, gClients[i].hand, gClients[i].result);
		printf("num : %d\n", i);
		#endif
	}
	inputEnableFlag = 1;
}

/*****************************************************************
関数名	: RecvFieldData_y
機能	: フィールドのデータを受信する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvFieldData_y(void)
{
	int i;
	printf("OK\n");
	// TODO キャラの構造体をセット
	RecvFieldInfoData(&gField);
	#ifndef NDEBUG
	printf("#####\nRecvFieldData() RESULT\n");
	//printf("client[%d] : hand %d result %d\n",i, gClients[i].hand, gClients[i].result);
	//printf("num : %d\n", i);
	#endif
	inputEnableFlag = 1;
}



/*****************************************************************
関数名	: DrawFrame_y
作成者 : 安村
機能	: ゲームの処理結果を表示する
引数	: なし
出力	: なし
*****************************************************************/
static void DrawFrame_y(void)
{
	DrawCharacter(); // キャラ表示
	DrawMypos(); // 自分の番号表示
	BlitWindow(); // 描画
	printf("gChara[0].type:%d\n",gChara[0].type);
	printf("gChara[0].stts:%d\n",gChara[0].stts);
	printf("gChara[1].type:%d\n",gChara[1].type);
	printf("gChara[1].stts:%d\n",gChara[1].stts);
	printf("gChara[2].type:%d\n",gChara[2].type);
	printf("gChara[2].stts:%d\n",gChara[2].stts);
	printf("gChara[3].type:%d\n",gChara[3].type);
	printf("gChara[4].stts:%d\n",gChara[4].stts);
	#ifndef NDEBUG
		printf("#####\nDrawFrame_y\n");
	#endif
}




