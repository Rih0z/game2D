/*****************************************************************
ファイル名	: server_func.h
機能		: サーバーの外部関数の定義
*****************************************************************/

#ifndef _SERVER_FUNC_H_
#define _SERVER_FUNC_H_

#include"server_common.h"

/* server_main.c */
extern CharaInfo	*gChara;       /* キャラクター情報 */
extern CameraData	*gCam;       /* キャラクター情報 */
extern InputInfo	*gInput;	/* 入力データ */
extern int PrintError( const char *str );

/* server_net.c */
extern int RecvCamInfoData(CameraData *cameranoData,int pos);
extern int clientID; // 自分のクライアントID
extern int gClientNum; // 全てのクライアント数
extern int SetUpServer(int num);
extern void Ending(void);
extern int RecvIntData(int pos,int *intData);
extern int RecvInputInfoData_y(int pos); // 入力情報の受け取り(安村)
extern void SendData(int pos,void *data,int dataSize);
extern int SendRecvManager(void);
/* server_command.c */
static void RecvCamData_r(int pos);
static int CamStatus(int pos);
extern int ExecuteCommand(char command,int pos);
extern void SendFrame_r(void); // フレームの情報を送信
extern void SendResult_r(void); // フレームの情報を送信
extern void SendTitleCommand(void); // タイトル画面でスペースが押されたことをサーバに通知
extern void SendCharaSelCommand(void); // キャラ選択が行われてたことを通知
extern void SendResultCommand(void);
/* server_sys.c */
extern int		  gTheWorldID;
extern int		  gFlagHaruVio[MAX_CLIENTS];
extern int		  gFlagShuNinja[MAX_CLIENTS];
extern int		  gFlagAllLove[MAX_CLIENTS]; //  koko3　乾
extern FieldInfo  gField;       /* フィールド情報 */
extern IplImage  *gCvBGMask;
extern IplImage  *gCvCharaMask[ CI_NUM ];
extern IplImage *gCvAttackMask[ AI_NUM/2 ];
extern IplImage *gCvAttackLMask[ AI_NUM/2 ];
extern IplImage  *gCvAnd;
extern int InitSystem(int num);
extern int InitItemsys(void); //アイテム 追加 松本
extern void InitCharaField_y(void); // キャラ・フィールド構造体の初期化
extern void MoveChara(void);
extern void MoveItem(void);//アイテム移動　追加　松本
extern void Title_y(int pos); // タイトルの処理
void CharaSel_y(int pos, int charaType); // キャラ選択の処理
extern void End_y(int pos); // 終了の処理
extern void getRank_r(void); // 終了の処理
extern void Collision_a( CharaInfo *ci, CharaInfo *cj ); //当たり判定
extern void Collision_s( CharaInfo *ci, CharaInfo *cj ); //当たり判定 koko3 乾
extern void Collision_i_1( CharaInfo *ci); //当たり判定　追加　松本
extern void Collision_i_2( CharaInfo *ci); //当たり判定　追加　松本
extern void TheWorld_y(int flagInit); // The Worldの秒数・フラグ管理の関数  
extern void HarumafujiViolence_y(int flagInit); // 日馬富士バイオレンスの秒数・フラグ管理の関数 
extern void ShushuttoNinjaja_y(int flagInit); // シュシュッとニンジャジャの秒数・フラグ管理の関数
extern void AllMyLove4Seed_i(int flagInit); // AllMyLove4Seedの秒数・フラグ管理の関数 koko3 乾
extern void CountStandby_y(int flagInit); // スタンバイ状態の秒数・フラグ管理の関数 TODO ★★★追加安村s

#endif

