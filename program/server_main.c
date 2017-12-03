/*****************************************************************
ファイル名	: server_main.c
機能		: サーバーのメインルーチン
*****************************************************************/

#include<SDL/SDL.h>
#include"server_common.h"
#include"time.h"
#include"sys/time.h"

static int FrameUpdate(void);
static void Animetion(void); // アニメーションの関数(安村)

CharaInfo	*gChara;    /* キャラ */
InputInfo	*gInput;	/* 入力データ */
FieldInfo	gField;     /* フィールド情報 */

int main(int argc,char *argv[])
{
	int	num;
	int	endFlag = 1;
	int	charaFlag = 1;

	/* 引き数チェック */
	if(argc != 2){
		fprintf(stderr,"Usage: number of clients\n");
		exit(-1);
	}
	if((num = atoi(argv[1])) < 0 ||  num > MAX_CLIENTS){
		fprintf(stderr,"clients limit = %d \n",MAX_CLIENTS);
		exit(-1);
	}
	
	/* SDLの初期化 */
	if(SDL_Init(SDL_INIT_TIMER) < 0) {
		printf("failed to initialize SDL.\n");
		exit(-1);
	}

	/* システムの初期化 */
	if(InitSystem(num) < 0) {
		printf("failed to initialize system.\n");
		exit(-1);
	}
	
	printf("OKbokujo\n");
	printf("system initialize\n");
	/* クライアントとの接続 */
	if(SetUpServer(num) == -1){
		fprintf(stderr,"Cannot setup server\n");
		exit(-1);
	}

	SendTitleCommand(); // 全クライアントをタイトル状態にする
	
	/* メインイベントループ */
	while(endFlag){
		printf("FieldType : %d\n", gField.back); // TODO 確認用
		switch(gField.back) {
			case BK_Title: // タイトル
				endFlag = SendRecvManager();
				break;
			case BK_Chara_Sel: // キャラ選択
				endFlag = SendRecvManager();
				break;
			case BK_Chara_Sel_Wait:
				endFlag = SendRecvManager();
				break;
			case BK_Field:
				if( FrameUpdate () ) { // 一定間隔が経過すると呼び出される
					Animetion(); // アニメーション
					MoveChara(); // キャラクターを動かす
					int i,j;
					for(i=0; i<num; i++){
						gChara[i].pos.x = gChara[i].point.x -= 1; 
						for(j=0; j<num; j++)
						    Collision_a( &gChara[i], &gChara[j] );
					}
					SendFrame_r(); // フレームを送る 
					gField.scroll++;
				}
				endFlag = SendRecvManager();
				break;
			case BK_End: // 終了
				endFlag = SendRecvManager();
				break;
		}
	}

	/* 終了処理 */
	Ending();

	return 0;
}

/*****************************************************************
関数名  : FrameUpdate
機能    : 一定間隔で呼び出される(フレームの処理)
引数    : なし
出力    : 1 : 一定時間経過　0 : 経過していない
*****************************************************************/
static int FrameUpdate(void)
{
	static flag = 0;
	static struct timeval timeNow , timePre;
	double sec, micro, passed;

	if (flag == 0) { // 時間の初期化
		gettimeofday(&timeNow, NULL);
		timePre = timeNow;
		flag = 1;
	}
	gettimeofday(&timeNow, NULL); // 現在の時間を取得
	
	sec = (double)(timeNow.tv_sec - timePre.tv_sec);
	micro = (double)(timeNow.tv_usec - timePre.tv_usec);
	passed = sec + micro / 1000.0 / 1000.0;
	
	//printf("timeNow = %f\n",passed); // 確認
	//if (passed >= 0.03333) // 一定間隔
	if (passed >= 0.05)
	{
		timePre = timeNow ; 
		return 1 ;
	}
	return 0 ; 
}

/*****************************************************************
関数名  : Animetion
機能    : アニメーションの更新
引数    : なし
出力    : なし
*****************************************************************/
void Animetion(void)  //i
{
	int i;
	static int count;
	// TODO 変更項目(安村)
	if(count == 4) {
		for(i=0; i<CI_NUM; i++){
			gChara[i].anipat = (gChara[i].anipat+1) % 4;
		}
		count = 0;
	}
	count++;
#ifndef NDEBUG
    printf("#####\n");
    printf("Animetion()\n");
#endif
	return;
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

