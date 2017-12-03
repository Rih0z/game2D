/*****************************************************************
ファイル名	: server_func.h
機能		: サーバーの外部関数の定義
*****************************************************************/

#ifndef _SERVER_FUNC_H_
#define _SERVER_FUNC_H_

#include"server_common.h"

/* server_main.c */
extern CharaInfo	*gChara;       /* キャラクター情報 */
extern InputInfo	*gInput;	/* 入力データ */
extern int PrintError( const char *str );

/* server_net.c */
extern int clientID; // 自分のクライアントID
extern int gClientNum; // 全てのクライアント数
extern int SetUpServer(int num);
extern void Ending(void);
extern int RecvIntData(int pos,int *intData);
extern int RecvInputInfoData_y(int pos); // 入力情報の受け取り(安村)
extern void SendData(int pos,void *data,int dataSize);
extern int SendRecvManager(void);

/* server_command.c */
extern int ExecuteCommand(char command,int pos);
extern void SendFrame_r(void); // フレームの情報を送信
extern void SendTitleCommand(void); // タイトル画面でスペースが押されたことをサーバに通知
extern void SendCharaSelCommand(void); // キャラ選択が行われてたことを通知

/* server_sys.c */
extern FieldInfo  gField;       /* フィールド情報 */
extern IplImage  *gCvBGMask;
extern IplImage  *gCvCharaMask[ CI_NUM ];
extern IplImage  *gCvAttackMask[ CT_NUM * AT_NUM ];
extern IplImage  *gCvAnd;
extern int InitSystem(int num);
extern void InitCharaField_y(void); // キャラ・フィールド構造体の初期化
extern void MoveChara(void);
extern void Title_y(int pos); // タイトルの処理
void CharaSel_y(int pos, int charaType); // キャラ選択の処理
extern void End_y(int pos); // 終了の処理
extern void Collision_a( CharaInfo *ci, CharaInfo *cj ); //当たり判定

#endif
