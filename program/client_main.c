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
//add19
int cameraWF = 0;
int cameraWFW = 0;

// ★★★ スレッド関数追加 安村↓
int ThreadWii_y(void* args);
//  ★★★ スレッド関数追加 riho hosihosihosicamera↓
int ThreadCamera_r(void* args);

//wiimote_t wiimote = WIIMOTE_INIT;
static int flagUp = 0;
static int flagDown = 0;
static int flagOne = 0;
static int flagTwo = 0;
static int flagHome = 0;
static int flagPlus = 0;
static int flagA = 0;
static int flagB = 0;
int keyFlag_r = 0;
int flagInit_y = 0; // ★★★変更安村
int waitF   = 0 ;// hosihosihosi1 riho
static int cameraSF = 1 ;
int main(int argc,char *argv[]) //i
{
  static int cameraF;
  cameraF = 1 ; 
  int	endFlag=1;
  char	localHostName[]="localhost";
  char	*serverName;
  SDL_Thread	*thrWii; //  ★★★ 変数追加 安村
  SDL_Thread	*thrCamera; //  ★★★ 変数追加 安村

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

  /* SDLの初期化 */
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0){
    return PrintError( SDL_GetError() );
  }

  /* ウインドウの初期化 */
  if(InitWindows()==-1){
    fprintf(stderr,"setup failed : InitWindows\n");
    return -1;
  }
  /* サウンド */ 
  if( InitSound() < 0){
    return PrintError("failed to initialize Sound");		
  }
  /* タイマー */
  //SDL_TimerID timer = SDL_AddTimer(50, Timer, NULL);

  thrWii = SDL_CreateThread(ThreadWii_y, NULL); // スレッドthr3を作成し、スレッド関数thread3を実行（引数なし）  ★★★ 追加　安村


  /* メインイベントループ */
  while(endFlag){
    printf("FieldType : %d\n", gField.back);
    switch(gField.back) {
      case BK_Title : // タイトル画面
        WindowEventT();
        //add19
        cameraWFW = 0 ;
        break;
      case BK_Title_Wait: // タイトル待機画面
        WindowEventW();
        break;
        //    add19
      case BK_Chara_Sel: // キャラ選択
        if(cameraWFW== 0)
        {
          WindowEventCam();
          if(cameraF)//sureddo
            thrCamera = SDL_CreateThread(ThreadCamera_r, NULL); // スレッドthr3を作成し、スレッド関数thread3を実行（引数なし）  ★★★ 追加　riho
          cameraF = 0 ;
        }else
          WindowEventC();
        break;
      case BK_Chara_Sel_Wait:
        WindowEventW();
        break;
      case BK_Field : 
        WindowEventF();
        break;
      case BK_Loading :
        WindowEventW();
        break;
      case BK_Result :
        WindowEventR();
        break;
      default : 
        WindowEventW();
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
  関数名  : ThreadCamera_r
  機能    : Wiiのスレッド
  引数    : void		args		: わからん
  出力    : 0
 *****************************************************************/
int ThreadCamera_r( void * args)
{
  static int endcamera =  1 ;
  static int roopcount = 1 ;
  static int preroopcount = 0 ;
  //add19 カメラの処理が終わると1が帰ってくる
  while(1){
    //    if(cameraWFW == 0 )
    cameraWF = Camera_r(clientID,cameraWF);
    //  else
    //  Camera_r(clientID,1); 

    //    if(roopcount =! preroopcount && endcamera == 1)
    // {
    //if returned 1
    if(cameraWF)
      cameraWFW = 1;
    // preroopcount = roopcount;
    //   }
  }
  // cameraSF = 0 ;
  printf("camerafin\n");
  return 0 ; 
}

/*****************************************************************
  関数名  : ThreadWii_y
  機能    : Wiiのスレッド
  引数    : void		args		: わからん
  出力    : 0
 *****************************************************************/
int ThreadWii_y(void* args) {
  while(1){
    switch(gField.back) {
      case BK_Title :
        if(wiimote_is_open(&wiimote)) {
          // Wiiリモコンの状態を取得・更新する
          if (wiimote_update(&wiimote) < 0) {
            wiimote_disconnect(&wiimote);
          }
          // ホームボタン
          if (wiimote.keys.home && flagHome == 0) { 
            flagHome = 1;
            SendEndCommand(); // 終了コマンド
          }
          else if(wiimote.keys.home == 0 && flagHome == 1)
            flagHome = 0;

          // プラスボタン
          if (wiimote.keys.plus && flagPlus == 0) {
            flagPlus = 1;
            Mix_PlayChannel(-1, gSE[ST_Decide], 0);
            SendTitleCommand(); // タイトルでスペースがをされたことを通知
            gField.back = BK_Title_Wait;
            flagInit_y = 0;
          }
          else if(wiimote.keys.plus == 0 && flagPlus == 1)
            flagPlus = 0;

          // 1ボタン
          if (wiimote.keys.one && flagOne == 0) {
            flagOne = 1;
          }
          else if(wiimote.keys.one == 0 && flagOne == 1){
            flagOne = 0;
          }

          // 2ボタン
          if (wiimote.keys.two && flagTwo == 0) {
            flagTwo = 1;
          }
          else if(wiimote.keys.two == 0 && flagTwo == 1){
            flagTwo = 0;
          }

          // 右ボタン
          if (wiimote.keys.down && flagDown == 0){
            flagDown = 1;
            gInput.dir1 += 6;
          }	
          else if (wiimote.keys.down == 0 && flagDown == 1){
            flagDown = 0;
            gInput.dir1 += -6;
          }

          // 左ボタン
          if (wiimote.keys.up && flagUp == 0){
            flagUp = 1;
            gInput.dir1 += -6;
          }
          else if(wiimote.keys.up == 0 && flagUp == 1){ 
            flagUp = 0;
            gInput.dir1 += 6;
          }
        }
        break;
      case BK_Title_Wait:
      case BK_Chara_Sel_Wait:
        if(wiimote_is_open(&wiimote)) {
          // Wiiリモコンの状態を取得・更新する
          if (wiimote_update(&wiimote) < 0) {
            wiimote_disconnect(&wiimote);
          }
          // ホームボタン
          if (wiimote.keys.home && flagHome == 0) {
            flagHome = 1;
            SendBackTitleCommand(); // タイトルに戻ることをサーバに通知  変更安村
          }
          else if(wiimote.keys.home == 0 && flagHome == 1)
            flagHome = 0;

          // プラスボタン
          if (wiimote.keys.plus && flagPlus == 0) {
            flagPlus = 1;
          }
          else if(wiimote.keys.plus == 0 && flagPlus == 1)
            flagPlus = 0;

          // 1ボタン
          if (wiimote.keys.one && flagOne == 0) {
            flagOne = 1;
          }
          else if(wiimote.keys.one == 0 && flagOne == 1){
            flagOne = 0;
          }

          // 2ボタン
          if (wiimote.keys.two && flagTwo == 0) {
            flagTwo = 1;
          }
          else if(wiimote.keys.two == 0 && flagTwo == 1){
            flagTwo = 0;
          }

          // 右ボタン
          if (wiimote.keys.down && flagDown == 0){
            flagDown = 1;
            gInput.dir1 += 6;	
          }	
          else if (wiimote.keys.down == 0 && flagDown == 1){
            flagDown = 0;
            gInput.dir1 += -6;
          }

          // 左ボタン
          if (wiimote.keys.up && flagUp == 0){
            flagUp = 1;
            gInput.dir1 += -6;
          }
          else if(wiimote.keys.up == 0&& flagUp == 1){ 
            flagUp = 0;
            gInput.dir1 += 6;
          }
        }
        break;
      case BK_Chara_Sel:
        if(wiimote_is_open(&wiimote)) {
          // Wiiリモコンの状態を取得・更新する
          if (wiimote_update(&wiimote) < 0) {
            wiimote_disconnect(&wiimote);
          }
          // ホームボタン
          if (wiimote.keys.home && flagHome == 0) {
            flagHome = 1;
            SendBackTitleCommand();
          }
          else if(wiimote.keys.home == 0 && flagHome == 1)
            flagHome = 0;

          // プラスボタン
          if (wiimote.keys.plus && flagPlus == 0) {
            flagPlus = 1;
          }
          else if(wiimote.keys.plus == 0 && flagPlus == 1)
            flagPlus = 0;

          // 1ボタン
          if (wiimote.keys.one && flagOne == 0) {
            flagOne = 1;
          }
          else if(wiimote.keys.one == 0 && flagOne == 1){
            flagOne = 0;
          }

          // 2ボタン
          if (wiimote.keys.two && flagTwo == 0) {
            flagTwo = 1;
            gChara[clientID].type = keyFlag_r;
            Mix_PlayChannel(-1, gSE[ST_Decide], 0);
            SendCharaSelCommand(keyFlag_r); // キャラ選択を通知
            gField.back = BK_Chara_Sel_Wait; // 待機状態に遷移
          }
          else if(wiimote.keys.two == 0 && flagTwo == 1)
            flagTwo = 0;

          // 右ボタン
          if (wiimote.keys.down && flagDown == 0){
            flagDown = 1;
            gInput.dir1 += 6;
            keyFlag_r++;
            Mix_PlayChannel(-1, gSE[ST_Select], 0);
            if(keyFlag_r == CT_NUM)
              keyFlag_r = 0 ;
          }	
          else if (wiimote.keys.down == 0 && flagDown == 1){
            flagDown = 0;
            gInput.dir1 -= 6;
          }

          // 左ボタン
          if (wiimote.keys.up && flagUp == 0){
            flagUp = 1;
            gInput.dir1 += -6;
            keyFlag_r--;
            Mix_PlayChannel(-1, gSE[ST_Select], 0);
            if(keyFlag_r == -1 ) keyFlag_r = CT_NUM - 1;
          }
          else if(wiimote.keys.up == 0 && flagUp == 1){ 
            flagUp = 0;
            gInput.dir1 += 6;
          }
        }
        break;
      case BK_Field :
        if(wiimote_is_open(&wiimote)) {
          // Wiiリモコンの状態を取得・更新する
          if (wiimote_update(&wiimote) < 0) {
            wiimote_disconnect(&wiimote);
          }
          // ホームボタン
          if (wiimote.keys.home && flagHome == 0) {
            flagHome = 1;
            InitInput_y(); // 入力構造体の初期化
            SendBackTitleCommand(); // タイトルに戻ることをサーバに通知
            flagInit_y = 0;
          }
          else if(wiimote.keys.home == 0 && flagHome == 1)
            flagHome = 0;

          // プラスボタン
          if (wiimote.keys.plus && flagPlus == 0) {
            flagPlus = 1;
          }
          else if(wiimote.keys.plus == 0 && flagPlus == 1)
            flagPlus = 0;

          // 1ボタン
          if (wiimote.keys.one && flagOne == 0) {
            flagOne = 1;
            gInput.button1 = AT_Punch;
          }
          else if(wiimote.keys.one == 0 && flagOne == 1){
            flagOne = 0;
            gInput.button1 = AT_None;
          }

          // 2ボタン
          if (wiimote.keys.two && flagTwo == 0) {
            flagTwo = 1;
            gInput.button2 = MT_Jump;
          }
          else if(wiimote.keys.two == 0 && flagTwo == 1){
            flagTwo = 0;
            gInput.button2 = MT_Stand;
          }

          // 右ボタン	
          if (wiimote.keys.down && flagDown == 0) {
            flagDown = 1;
            gInput.dir1 += 6;
          }
          else if(wiimote.keys.down == 0 && flagDown == 1){
            flagDown = 0;
            gInput.dir1 += -6;
          }

          // 左ボタン
          if (wiimote.keys.up && flagUp == 0) {
            flagUp = 1;
            gInput.dir1 += -6;
          }
          else if(wiimote.keys.up == 0 && flagUp == 1){
            gInput.dir1 += 6;
            flagUp = 0;
          }
          // ★★★ 必殺技追加　安村
          if(wiimote.axis.z > 210) {
            gInput.shake = AT_Shake;
            printf("axis.z = %d**************************************\n", wiimote.axis.z);
          }
          else
            gInput.shake = AT_None;
        }
        break;
      case BK_Result :
        if(wiimote_is_open(&wiimote)) {
          // Wiiリモコンの状態を取得・更新する
          if (wiimote_update(&wiimote) < 0) {
            wiimote_disconnect(&wiimote);
          }
          // ホームボタン
          if (wiimote.keys.home && flagHome == 0) {
            flagHome = 1;
            SendBackTitleCommand(); // タイトルに戻ることをサーバに通知
          }
          else if(wiimote.keys.home == 0 && flagHome == 1)
            flagHome = 0;

          // プラスボタン
          if (wiimote.keys.plus && flagPlus == 0) {
            flagPlus = 1;
          }
          else if(wiimote.keys.plus == 0 && flagPlus == 1)
            flagPlus = 0;

          // 1ボタン
          if (wiimote.keys.one && flagOne == 0) {
            flagOne = 1;
          }
          else if(wiimote.keys.one == 0 && flagOne == 1){
            flagOne = 0;
          }

          // 2ボタン
          if (wiimote.keys.two && flagTwo == 0) {
            flagTwo = 1;
          }
          else if(wiimote.keys.two == 0 && flagTwo == 1){
            flagTwo = 0;
          }

          // 右ボタン
          if (wiimote.keys.down && flagDown == 0){
            flagDown = 1;
            gInput.dir1 += 6;
          }	
          else if (wiimote.keys.down == 0 && flagDown == 1){
            flagDown = 0;
            gInput.dir1 += -6;
          }

          // 左ボタン
          if (wiimote.keys.up && flagUp == 0){
            flagUp = 1;
            gInput.dir1 += -6;
          }
          else if(wiimote.keys.up == 0&& flagUp == 1){ 
            flagUp = 0;
            gInput.dir1 += 6;
          }


        }
        break;
    }
  }
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

