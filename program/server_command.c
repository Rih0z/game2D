/*****************************************************************
ファイル名	: server_command.c
機能		: サーバーのコマンド処理
*****************************************************************/

#include"server_common.h"
#include"server_func.h"

static void SetIntData2DataBlock(void *data,int intData,int *dataSize);
static void SetCharData2DataBlock(void *data,char charData,int *dataSize);
static void SetCharaInfoData2DataBlock(void *data,CharaInfo charaInfoData,int *dataSize); // キャラの構造体をセットする関数(安村)
static void SetFieldData2DataBlock(void *data,FieldInfo fieldData,int *dataSize); // フィールドの構造体をセットする関数(安村)
static void InputDataToCharaData(int pos);
void SendFrame_r(void); // フレームの情報を送信

/*****************************************************************
関数名	: ExecuteCommand
機能	: クライアントから送られてきたコマンドを元に，
		  引き数を受信し，実行する
引数	: char	command		: コマンド
	  int	pos		: コマンドを送ったクライアント番号
出力	: プログラム終了コマンドが送られてきた時には0を返す．
		  それ以外は1を返す
*****************************************************************/
int ExecuteCommand(char command,int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize,intData;
	int 		charaType; // プレイヤーが選択したキャラ
    int			endFlag = 1;

    /* 引き数チェック */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("ExecuteCommand()\n");
    printf("Get command %c\n",command);
#endif
    switch(command){
	    case END_COMMAND:
			dataSize = 0;
			/* コマンドのセット */
			SetCharData2DataBlock(data,command,&dataSize);

			/* 全ユーザーに送る */
			SendData(ALL_CLIENTS,data,dataSize);

			endFlag = 0;
			break;

	    case SEND_COMMAND:
			printf("------------------------\n");
			RecvInputInfoData_y(pos); // 入力の構造体を取得
			InputDataToCharaData(pos); // 入力情報をキャラに反映
			break;

		case TITLE_COMMAND:
			//RecvInputInfoData_y(pos); // 入力の構造体を取得
			//TODO タイトルでの処理
			//if(gField.back == BK_Title) { // タイトル画面である場合
				Title_y(pos); // 人数の計算
			//}
 			//else {// タイトル画面以外の場合
			//	InitCharaField_y(); // キャラ・フィールド構造体の初期化
			//	SendTitleCommand(); // 全クライアントをタイトル状態にする
			//}
			break;
		
		case BACKTITLE_COMMAND:
			Title_y(-1); // フラグのリセット
			CharaSel_y(-1, -1); // キャラ入力状態のリセット
			InitCharaField_y(); // キャラ・フィールド構造体の初期化
			SendTitleCommand(); // 全クライアントをタイトル状態にする
			break;

		case CHARASEL_COMMAND:
			RecvIntData(pos,&charaType);
			// TODO キャラ選択の処理
			CharaSel_y(pos, charaType);
			break;
		
	    default:
			/* 未知のコマンドが送られてきた */
			fprintf(stderr,"0x%02x is not command!\n",command);
    }
    return endFlag;
}


/*****************************************************************
関数名	: SendFrame_r
作成者：RIHO
機能	: 1フレームのキャラ情報を送信
引数	: なし
出力	: なし
*****************************************************************/
void SendFrame_r(void)
{

    unsigned char	data[MAX_DATA];
    int			dataSize;
	int 		i ; 

	dataSize = 0;
	// コマンドのセット 
	SetCharData2DataBlock(data,HAND_COMMAND,&dataSize);
	
	for(i = 0; i < gClientNum; i++){
		// キャラ構造体をセット
		SetCharaInfoData2DataBlock(data, gChara[i], &dataSize);
		printf("set Structure : %d\n", i);
	}
	SetFieldData2DataBlock(data,gField,&dataSize); // フィールド構造体をセット

	SendData(ALL_CLIENTS,data,dataSize);
	dataSize = 0; // 送るデータを初期化
	SetCharData2DataBlock(data,FRAME_COMMAND,&dataSize);
	SendData(ALL_CLIENTS,data,dataSize);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendFrame_r()\n");
#endif
}

/*****************************************************************
関数名	: SendTitleCommand
機能	: クライアントに現在の状態がタイトル画面であることを通知
引数	: なし
出力	: なし
*****************************************************************/
void SendTitleCommand(void)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;
	int 	i;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendTitleCommand()\n");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,TITLE_COMMAND,&dataSize);

	for(i = 0; i < gClientNum; i++){
		// キャラ構造体をセット
		SetCharaInfoData2DataBlock(data, gChara[i], &dataSize);
		printf("set Structure : %d\n", i);
	}
	SetFieldData2DataBlock(data,gField,&dataSize); // フィールド構造体をセット

    /* データの送信 */
    SendData(ALL_CLIENTS, data, dataSize);
}

/*****************************************************************
関数名	: SendCharaSelCommand
機能	: クライアントに現在の状態がキャラ選択画面であることを通知
引数	: なし
出力	: なし
*****************************************************************/
void SendCharaSelCommand(void)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;
	int 	i;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCharaSelCommand()\n");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data, CHARASEL_COMMAND, &dataSize);
	for(i = 0; i < gClientNum; i++){
		// キャラ構造体をセット
		SetCharaInfoData2DataBlock(data, gChara[i], &dataSize);
		printf("set Structure : %d\n", i);
	}
	SetFieldData2DataBlock(data,gField,&dataSize); // フィールド構造体をセット

    /* データの送信 */
    SendData(ALL_CLIENTS, data, dataSize);
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

    /* int 型のデータを送信用データの最後にコピーする */
    *(char *)(data + (*dataSize)) = charData;
    /* データサイズを増やす */
    (*dataSize) += sizeof(char);
}


/*****************************************************************
関数名	: SetCharaInfo2DataBlock
機能	: CharaInfo 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  CharaInfo		charaData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetCharaInfoData2DataBlock(void *data,CharaInfo charaInfoData,int *dataSize)
{
    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    /* CharaInfo 型のデータを送信用データの最後にコピーする */
    *(CharaInfo *)(data + (*dataSize)) = charaInfoData;
    /* データサイズを増やす */
    (*dataSize) += sizeof(CharaInfo);
}

/*****************************************************************
関数名	: SetFieldData2DataBlock
機能	: CharaInfo 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  CharaInfo		charaData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetFieldData2DataBlock(void *data,FieldInfo fieldData,int *dataSize)
{
    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    /* FieldInfo 型のデータを送信用データの最後にコピーする */
    *(FieldInfo *)(data + (*dataSize)) = fieldData;
    /* データサイズを増やす */
    (*dataSize) += sizeof(FieldInfo);
}


/*****************************************************************
関数名	: InputDataToCharaData
機能	: 入力情報をキャラの情報に反映させる
引数	: int		pos	        : クライアント番号
出力	: なし
*****************************************************************/
static void InputDataToCharaData(int pos)
{
	static int flagJump[MAX_CLIENTS];
	static int countJump[MAX_CLIENTS];

	if(gChara[pos].flagJumpSE == 1)
		gChara[pos].flagJumpSE = 0;

    gChara[pos].base.x = gInput[pos].dir1; // 方向
	// ダブルジャンプ
	if (flagJump[pos] == 0 && countJump[pos] < 2 && gInput[pos].button2 == MT_Jump) { 
		if(countJump[pos] > 0){
			gChara[pos].t = 0.0;
		}
		gChara[pos].motion = gInput[pos].button2; // ジャンプ
		gChara[pos].flagJumpSE = 1; // TODO ★★★ 追加 安村
		flagJump[pos] = 1;
		countJump[pos] += 1;
	}
	else if (flagJump[pos] == 1 && gInput[pos].button2 == MT_Stand) {
		flagJump[pos] = 0;
		if(gChara[pos].motion == MT_Jump){
			gChara[pos].motion = MT_Fall;
			gChara[pos].t = 0.0;
		}
	}

	if (gChara[pos].motion == MT_Stand || gChara[pos].motion == MT_Stnby) {
		flagJump[pos] = 0;
		countJump[pos] = 0;
	}

	if(gInput[pos].button1 == AT_Punch){
		gChara[pos].attack = AT_Punch;
	}
	
}


