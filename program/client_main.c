/*****************************************************************
ファイル名	: client_main.c
機能		: クライアントのメインルーチン
*****************************************************************/

#include <stdio.h>
#include <SDL/SDL.h>
#include"common.h"
#include"client_func.h"

#include <libcwiimote/wiimote.h>
#include <libcwiimote/wiimote_api.h>

WiiInfo *wiiinfo;

int clientID; // 自分のクライアントID
int gClientNum; // 全てのクライアント数
char gName[MAX_CLIENTS][MAX_NAME_SIZE]; // クライアント名

//static int NetworkEvent(void *data);
//static Uint32 Timer( Uint32 interval, void *pram );

int main(int argc,char *argv[]) //i
{
	int	endFlag=1;
	char	localHostName[]="localhost";
	char	*serverName;
	SDL_Thread	*thr;

	wiiinfo = (WiiInfo*)malloc( sizeof(WiiInfo) * 10 );
	if( wiiinfo == NULL ){
		return PrintError("failed to allocate memory.");
	}
	wiiinfo->ac = argc;
	int i;
	for(i=0; i<=sizeof(argv); i++){
		wiiinfo->av[i] = argv[i];
	}

	/* 引き数チェック */
	if(argc == 1 || argc == 2){
		serverName = argv[1];
	}
	else if(argc == 3){
		serverName = argv[2];
	}
	else{
		fprintf(stderr, "Usage: %s, Cannot find a Server Name.\n", argv[0]);
		return -1;
	}

	/* サーバーとの接続 */
	if(SetUpClient(serverName)==-1){
		fprintf(stderr,"setup failed : SetUpClient\n");
		return -1;
	}

	//clientID = 0;	// 仮置き TODO 消す
	//num = 1;	// 仮置き

	/* SDLの初期化 */
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0){
		return PrintError( SDL_GetError() );
	}

	/* ウインドウの初期化 */
	if(InitWindows()==-1){
		fprintf(stderr,"setup failed : InitWindows\n");
		return -1;
	}
	/* タイマー */
	//SDL_TimerID timer = SDL_AddTimer(50, Timer, NULL);


	/* メインイベントループ */
	while(endFlag){
		printf("FieldType : %d\n", gField.back); // TODO 確認用
		switch(gField.back) {
			case BK_Title : // タイトル画面
				WindowEventT();
				endFlag = SendRecvManager();
				break;
			case BK_Title_Wait: // タイトル待機画面
				WindowEventW();
				endFlag = SendRecvManager();
				break;
			case BK_Chara_Sel: // キャラ選択
				WindowEventC();
				endFlag = SendRecvManager();
				break;
			case BK_Chara_Sel_Wait:
				WindowEventW();
				endFlag = SendRecvManager();
				break;
			case BK_Field : 
				WindowEventF();
				break;
			case BK_End : 
				WindowEventE();
				break;
		}
		endFlag = SendRecvManager();
	}

	/* 終了処理 */


//	SDL_WaitThread(thr,NULL);
	DestroyWindow();
	CloseSoc();

	return 0;
}

/*****************************************************************
関数名  : PrintError
機能    : エラーメッセージ
引数    : void		*data		: 終了判定フラグ
出力    : スレッド終了時に０を返す
*****************************************************************/
int PrintError( const char *str )
{
	fprintf( stderr, "%s\n", str );
	return -1;
}

