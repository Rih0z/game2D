/*****************************************************************
ファイル名	: client_command.c
機能		: クライアントのコマンド処理
*****************************************************************/

#include"common.h"
#include"client_func.h"


static int inputEnableFlag = 1; // 入力可否フラグ

extern CameraData   *gCam;  /* クライアント */

static void SetCamInfoData2DataBlock(void *data,CameraData cameranoData,int     *dataSize);
static void SetIntData2DataBlock(void *data,int intData,int *dataSize);
static void SetCharData2DataBlock(void *data,char charData,int *dataSize);
static void SetInputInfo2DataBlock(void *data,InputInfo inputInfoData,int *dataSize); // 入力された情報の構造体をセットする関数(安村)
static void RecvCharaData_y(void); // 全員のキャラの構造体を取得する関数(安村)
static void RecvFieldData_y(void); // フィールドの構造体を取得する関数(安村)
static void DrawFrame_y(void);
static void PlaySE_y(int flagInit); 
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
  static flagBgm = 0; 
#ifndef NDEBUG
  printf("#####\n");
  printf("ExecuteCommand()\n");
  printf("command = %c\n",command);
#endif
  //printf("waitF command 90 %d",waitF);
 // if(beforeBackF != gField.back)
   // waitF = 0 ;
 // beforeBackF = gField.back;

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
      if(flagBgm == 0) {
        Mix_HaltMusic(); // BGMの停止
        Mix_PlayMusic(gBgm[BT_Game], -1); // 戦闘中のBGMの再生（繰り返し再生）
        flagBgm = 1;
      }
      if ( gField.scroll > END_SCROLL ) 
      {
        SendResultCommand();
        //    gField.back = BK_Loading ; 
        DrawBack(BK_Loading);
        BlitWindow();
      }else{
        DrawFrame_y();
      }
      PlaySE_y(0);
      SendInputInfoCommand_y();
      break;
    case TITLE_COMMAND:
      RecvCharaData_y(); // 全員のキャラの構造体を取得
      RecvFieldData_y(); // フィールドの構造体を取得
      printf("OKBokujo\n");
      flagBgm = 0;
      Mix_HaltMusic(); // タイトル中のBGMの停止
      Mix_PlayMusic(gBgm[BT_Title], -1); // タイトル中のBGMの再生（繰り返し再生）
      PlaySE_y(-1); // TODO ★★★ 追加　安村
      DrawBack(gField.back);
     // if(waitF)
       // DrawBack(BK_Loading);
      BlitWindow(); // 描画
      break;
    case CHARASEL_COMMAND:
      RecvCharaData_y(); // 全員のキャラの構造体を取得
      RecvFieldData_y(); // フィールドの構造体を取得
//      DrawBack(gField.back);
  //    BlitWindow(); // 描画
      break;
    case RESULT_COMMAND: //結果計算hosihosihosi riho
      RecvCharaData_y(); // 全員のキャラの構造体を取得
      RecvFieldData_y(); // フィールドの構造体を取得
     // BlitWindow(); // 描画
      break;
    case CAMERA_COMMAND:
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
//  waitF = 1 ;
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
//  waitF = 1 ; 

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
//  waitF = 1 ;
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
  関数名	: SendResultCommand
  機能	: プログラムの終了を知らせるために，
  サーバーにデータを送る
  引数	: なし
  出力	: なし
 *****************************************************************/
void SendCdatasetCommand(void)
{
  unsigned char	data[MAX_DATA];
  int			dataSize;

#ifndef NDEBUG
  printf("#####\n");
  printf("SendEndCommand()\n");
#endif
  dataSize = 0;
  /* コマンドのセット */
  SetCharData2DataBlock(data,CDATASET_COMMAND,&dataSize);

  /* データの送信 */
  SendData(data,dataSize);
}
/*****************************************************************
  関数名	: SendResultCommand
  機能	: プログラムの終了を知らせるために，
  サーバーにデータを送る
  引数	: なし
  出力	: なし
 *****************************************************************/
void SendResultCommand(void)
{
  unsigned char	data[MAX_DATA];
  int			dataSize;

#ifndef NDEBUG
  printf("#####\n");
  printf("SendResultCommand()\n");
#endif
  dataSize = 0;
  /* コマンドのセット */
  SetCharData2DataBlock(data,RESULT_COMMAND,&dataSize);

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
//  waitF = 1 ;
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
/*****************************************************************
  関数名    : SendCharaSelCommand
  機能    : クライアントに現在の状態がキャラ選択画面であることを通知
  引数  : なし
  出力    : なし
 *****************************************************************/
void SendCamCommand(void)
{
  unsigned char  data[MAX_DATA];
  int          dataSize;
  int    i;

#ifndef NDEBUG
  printf("#####\n");
  printf("SendCamCommand()\n");
#endif
  dataSize = 0;
  /* コマンドのセット */
  SetCharData2DataBlock(data, CAMERA_COMMAND, &dataSize);
  for(i = 0; i < gClientNum; i++){
    // キャラ構造体をセット
    SetCamInfoData2DataBlock(data, gCam[i], &dataSize);
    printf("set Structure : %d\n", i);
  }

  /* データの送信 */
  SendData(data,dataSize);
}
/*****************************************************************
  関数名    : SetCharaInfo2DataBlock
  機能    : CharaInfo 型のデータを送信用データの最後にセットする
  引数  : void      *data       : 送信用データ
  CharaInfo       charaData       : セットするデータ
  int       *dataSize   : 送信用データの現在のサイズ
  出力    : なし
 *****************************************************************/
static void SetCamInfoData2DataBlock(void *data,CameraData cameranoData,int *dataSize)
{
  /* 引き数チェック */
  assert(data!=NULL);
  assert(0<=(*dataSize));

  /* CharaInfo 型のデータを送信用データの最後にコピーする */
  *(CameraData *)(data + (*dataSize)) = cameranoData;
  /* データサイズを増やす */
  (*dataSize) += sizeof(CameraData);
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
  DrawBack(gField.back);//描画をまとめましたriho hosihosihosi
  FieldDraw_r(0); 
  DrawCharacter(); // キャラ表示
  DrawMypos(); // 自分の番号表示
  DrawItem();//追加　松本
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

/*****************************************************************
  関数名	: PlaySE_y
  作成者 : 安村
  機能	: 効果音の再生を行う
  引数	: 0 : 効果音再生 -1 : フラグのリセット
  出力	: なし
 *****************************************************************/
static void PlaySE_y(int flagInit){
  int i;
	static flagJump[MAX_CLIENTS];
  static flagDeath[MAX_CLIENTS];
  static flagAttack[MAX_CLIENTS];
  static flagDamage[MAX_CLIENTS];
	static flagHissatsu[MAX_CLIENTS]; // ★★★ TODO 追加
	static countHissatsuSE[MAX_CLIENTS]; // ★★★ TODO 追加
	static flagThrow[MAX_CLIENTS]; // ★★★ TODO 追加
  if (flagInit == 0) { // SE再生
    for (i = 0; i < gClientNum; i++) {
      if (gChara[i].flagJumpSE == 1  && flagJump[i] == 0) { // ジャンプの効果音
        Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_JumpS], 0); // ジャンプの再生
				flagJump[i] = 1;
      }
			else if(gChara[i].flagJumpSE == 0 && flagJump[i] == 1) {
				flagJump[i] = 0;
			}

      if(gChara[i].motion == MT_Stnby && flagDeath[i] == 0) { // 死んだ時の効果音
        Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_DeathS], 0); // 効果音
        Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_DeathV], 0); // ボイス
        flagDeath[i] = 1;
      }
      else if (gChara[i].motion != MT_Stnby && flagDeath[i] == 1){
        flagDeath[i] = 0;
      }

      if(gChara[i].attack == AT_Punch && flagAttack[i] == 0) { //攻撃時の効果音
        if (gChara[i].type == CT_Osumo && gChara[i].flagHissatsu == 1) // おすも必殺時の効果音 TODO ★★★　追加安村
					Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_HissatsuAttackS], 0); // 効果音
				else //  TODO ★★★　追加安村
        	Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_AttackS], 0); // 効果音
        Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_AttackV], 0); // ボイス
        flagAttack[i] = 1;
      }
      else if (gChara[i].attack == AT_None && flagAttack[i] == 1){
        flagAttack[i] = 0;
      }

      if((gChara[i].motion == MT_Damage || gChara[i].motion == MT_Damage_l) && flagDamage[i] == 0) { // ダメージ時の効果音
        //Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_DamageS], 0); // 効果音
        Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_DamageV], 0); // ボイス
        flagDamage[i] = 1;
      }
      else if (gChara[i].motion == MT_Muteki && flagDamage[i] == 1){
        flagDamage[i] = 0;
      }

			// TODO ★★★　安村追加
			if(gChara[i].flagHissatsu == 1 && flagHissatsu[i] == 0) { // 必殺技時の効果音
        Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_HissatsuS], 0); // 効果音
        Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_HissatsuV], 0); // ボイス
        flagHissatsu[i] = 1;
				countHissatsuSE[i] = 1;
      }
      else if (gChara[i].flagHissatsu == 0 && flagHissatsu[i] == 1){
        flagHissatsu[i] = 0;
				countHissatsuSE[i] = 0;
      }
			
			// TODO ★★★　安村追加
			if(gChara[i].type == CT_Ponzu && countHissatsuSE[i] > 0) { // ポン酢マシンガン
				if (countHissatsuSE[i] == 1)
					Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_HissatsuAttackS], 0); // 効果音
				countHissatsuSE[i]++;
				if (countHissatsuSE[i] == 46)
					countHissatsuSE[i] = 1;
			}
			

			// TODO ★★★　安村追加
			if(gChara[i].throwAttack > 0 && flagThrow[i] == 0) { // 投げの効果音
        Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_ThrowS], 0); // 効果音
        //Mix_PlayChannel(-1, gSEChara[gChara[i].type][CSET_HissatsuV], 0); // ボイス
        flagThrow[i] = 1;
      }
      else if (gChara[i].throwAttack == 0 && flagThrow[i] == 1){
        flagThrow[i] = 0;
      }
			//printf("gChara flagHIssatsu : %d\n", gChara[i].flagHissatsu);
			//printf("flagHissatsu : %d \n", flagHissatsu[i]);
    }
  }
  else { // フラグリセット
    for (i = 0; i < gClientNum; i++) {
			flagJump[i] = 0;
      flagDeath[i] = 0;
      flagAttack[i] = 0;
      flagDamage[i] = 0;
			flagHissatsu[i] = 0;
			countHissatsuSE[i] = 0;
			flagThrow[i] = 0;
    }
  }

#ifndef NDEBUG
  printf("#####\nPlaySE_y\n");
#endif
}



