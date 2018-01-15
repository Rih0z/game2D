/*****************************************************************
ファイル名	: client_func.h
機能		: クライアントの外部関数の定義
*****************************************************************/

#ifndef _CLIENT_FUNC_H_
#define _CLIENT_FUNC_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <libcwiimote/wiimote.h> // TODO ★★★ 追加安村
#include <libcwiimote/wiimote_api.h> // TODO ★★★ 追加安村
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
extern CameraData	*gCam;	/* クライアント */
extern InputInfo	gInput;		/* 入力データ */


/* client_win.c */
//i
extern WiiInfo   *wiiinfo;
extern wiimote_t wiimote; // TODO ★★★ 追加　安村
extern int keyFlag_r; // TODO ★★★ 追加　安村
extern int flagInit_y; // TODO ★★★ 追加　安村
//extern int waitF ;
extern CharaInfo *gChara;       /* キャラクター情報 */
extern InputInfo	gInput;		/* 入力データ */
extern FieldInfo  gField;       /* フィールド情報 */
extern MapType    gMaps[ MAP_Width ][ MAP_Height ];
Mix_Music *gBgm[BT_NUM]; // BGM
Mix_Chunk *gSE[ ST_NUM ]; // 効果音(操作)
Mix_Chunk *gSEChara[CT_NUM][ NAST_NUM ]; // 効果音(キャラ)
//Mix_Chunk *gSENaomi[ NAST_NUM ]; // 効果音(なおみ)
//Mix_Chunk *gSEOsumo[ OST_NUM ]; // 効果音(おすも)
//Mix_Chunk *gSEPonzu[ PST_NUM ]; // 効果音(ぽんず)
//Mix_Chunk *gSENinja[ NIST_NUM ]; // 効果音(にんじゃ)

extern int InitWindows(void);
extern int InitSound(void);  // BGM・効果音の初期化
extern void InitInput_y(void); // 入力構造体の初期化
extern int InitChara(void);
extern int PlayWii(void);
extern void DestroyWindow(void);
extern void WindowEventT(void); // タイトルのウィンドウイベント
extern void WindowEventC(void); // キャラ選択のウィンドウイベント
extern void WindowEventF(void); // 戦闘中のウィンドウイベント
extern void WindowEventR(void); // 終了のウィンドウイベント
extern int WindowEventW(void); // 待機のウィンドウイベント
extern int DrawBack(int back);
extern int DrawCharacter(void);
extern int DrawItem(void);
extern int DrawMypos(void);
extern int MakeBG(void);
extern int BlitWindow(void);

/* server_sys.c */ //あとでserver_func.hに移動
//extern int InitSystem(int num);
extern void MoveChara(void);


/* client_command.c */
extern void SendCdatasetCommand(void)  ;
extern void SendCamCommand(void);
extern int ExecuteCommand(char command);
extern void SendInputInfoCommand_y(void); // 入力したボタンの情報をサーバに送信する
extern void SendTitleCommand(void); // タイトル画面でスペースが押されたことをサーバに通知
extern void SendBackTitleCommand(void); // タイトルに戻ることをサーバに通知
extern void SendCharaSelCommand(CharaType type); // キャラ選択が行われてたことを通知
extern void SendEndCommand(void);
extern void SendResultCommand(void);
/*client_camera.c hosihosihosicamera*/
//add19
extern int Camera_r(int pos, int waifcamf);
extern int Time_r(int time);
#endif

