/*****************************************************************
ファイル名	: client_func.h
機能		: クライアントの外部関数の定義
*****************************************************************/

#ifndef _CLIENT_FUNC_H_
#define _CLIENT_FUNC_H_

#include <SDL/SDL.h>
#include"common.h"

/* client main.c */
extern int clientID; // 自分のクライアントID
extern int gClientNum; // 全てのクライアント数
extern char gName[MAX_CLIENTS][MAX_NAME_SIZE]; // クライアント名
extern int PrintError( const char *str );

/* client_net.c */
extern int SetUpClient(char* hostName);
extern void CloseSoc(void);
extern int RecvIntData(int *intData);
extern int RecvCharaInfoData(CharaInfo *charaInfoData); // キャラ情報の取得(安村)
extern int RecvFieldInfoData(FieldInfo *fieldInfoData); // フィールド情報の取得(安村)
extern void SendData(void *data,int dataSize);
extern int SendRecvManager(void);

extern CharaInfo	*gChara;	/* クライアント */
extern InputInfo	gInput;		/* 入力データ */


/* client_win.c */
//i
extern WiiInfo   *wiiinfo;
extern CharaInfo *gChara;       /* キャラクター情報 */
extern InputInfo	gInput;		/* 入力データ */
extern FieldInfo  gField;       /* フィールド情報 */
extern MapType    gMaps[ MAP_Width ][ MAP_Height ];


extern int InitWindows(void);
extern void InitInput_y(void); // 入力構造体の初期化
extern int InitChara(void);
extern int PlayWii(void);
extern void DestroyWindow(void);
extern void WindowEventT(void); // タイトルのウィンドウイベント
extern void WindowEventC(void); // キャラ選択のウィンドウイベント
extern void WindowEventF(void); // 戦闘中のウィンドウイベント
extern void WindowEventE(void); // 終了のウィンドウイベント
extern int WindowEventW(void); // 待機のウィンドウイベント
extern int DrawBack(int back);
extern int DrawCharacter(void);
extern int DrawMypos(void);
extern int MakeBG(void);
extern int BlitWindow(void);

/* server_sys.c */ //あとでserver_func.hに移動
//extern int InitSystem(int num);
extern void MoveChara(void);


/* client_command.c */
extern int ExecuteCommand(char command);
extern void SendInputInfoCommand_y(void); // 入力したボタンの情報をサーバに送信する
extern void SendTitleCommand(void); // タイトル画面でスペースが押されたことをサーバに通知
extern void SendBackTitleCommand(void); // タイトルに戻ることをサーバに通知
extern void SendCharaSelCommand(CharaType type); // キャラ選択が行われてたことを通知
extern void SendEndCommand(void);


#endif

