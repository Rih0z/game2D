/*****************************************************************
ファイル名	: client_win.c
機能		: クライアントのユーザーインターフェース処理
*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h> // TODO ★★★ ヘッダー読み込み追加 安村
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_rotozoom.h>

#include <SDL/SDL_ttf.h>

#include"common.h"
#include"client_func.h"

#include <libcwiimote/wiimote.h>
#include <libcwiimote/wiimote_api.h>

/* サーフェイス */
static SDL_Surface *gMainWindow;
static SDL_Surface *gTheWorld;
static SDL_Surface *gBG;
static SDL_Surface *gBGMask;
static SDL_Surface *gMapRead;
static SDL_Surface *gMapImage;
static SDL_Surface *gBackImage[ BK_NUM ];
static SDL_Surface *gCharaImage[ CI_NUM ];
static SDL_Surface *gCharaLImage[ CI_NUM ];
//static SDL_Surface *gMessages[ MSG_NUM ];
static SDL_Surface *gAttackImage[ CT_NUM * AT_NUM ];
static SDL_Surface *gAttackLImage[ CT_NUM * AT_NUM ];

/* サウンド TODO ★★★ 変数追加 安村*/
Mix_Music *gBgm[BT_NUM]; // BGM
Mix_Chunk *gSE[ ST_NUM ]; // 効果音(操作)
Mix_Chunk *gSEChara[CT_NUM][ NAST_NUM ]; // 効果音(キャラ)
//Mix_Chunk *gSENaomi[ NAST_NUM ]; // 効果音(なおみ)
//Mix_Chunk *gSEOsumo[ OST_NUM ]; // 効果音(おすも)
//Mix_Chunk *gSEPonzu[ PST_NUM ]; // 効果音(ぽんず)
//Mix_Chunk *gSENinja[ NIST_NUM ]; // 効果音(にんじゃ)
// サウンドパス
// BGM
static char *gBgmFile[BT_NUM]          ={"bgm_battle.mp3",
										 "bgm_title.mp3",
										 "bgm_result.mp3"};
// システム効果音
static char *gSEFile[ST_NUM] = {"select.wav",
								 "decide.wav"};
// なおみ効果音
static char * gSECharaFile[CT_NUM][NAST_NUM] = {{"naomi_attack_voice.wav", // 攻撃SE
										"naomi_attack_voice.wav", // 攻撃Voice
										"naomi_damage_voice.wav", // ダメージSE
										"naomi_damage_voice.wav", // ダメージVoice
										"naomi_jump_se.wav",   // ジャンプSE
										"naomi_jump_se.wav",   // ジャンプVoice
										"naomi_death_voice.wav",  // 死亡SE
										"naomi_death_voice.wav"}, // 死亡Voice
										{"naomi_attack_voice.wav", // 攻撃SE
										"naomi_attack_voice.wav", // 攻撃Voice
										"naomi_damage_voice.wav", // ダメージSE
										"naomi_damage_voice.wav", // ダメージVoice
										"naomi_jump_se.wav",   // ジャンプSE
										"naomi_jump_se.wav",   // ジャンプVoice
										"naomi_death_voice.wav",  // 死亡SE
										"naomi_death_voice.wav"}, // 死亡Voice
										{"naomi_attack_voice.wav", // 攻撃SE
										"naomi_attack_voice.wav", // 攻撃Voice
										"naomi_damage_voice.wav", // ダメージSE
										"naomi_damage_voice.wav", // ダメージVoice
										"naomi_jump_se.wav",   // ジャンプSE
										"naomi_jump_se.wav",   // ジャンプVoice
										"naomi_death_voice.wav",  // 死亡SE
										"naomi_death_voice.wav"}, // 死亡Voice
										{"naomi_attack_voice.wav", // 攻撃SE
										"naomi_attack_voice.wav", // 攻撃Voice
										"naomi_damage_voice.wav", // ダメージSE
										"naomi_damage_voice.wav", // ダメージVoice
										"naomi_jump_se.wav",   // ジャンプSE
										"naomi_jump_se.wav",   // ジャンプVoice
										"naomi_death_voice.wav",  // 死亡SE
										"naomi_death_voice.wav"}};// 死亡Voice

/* wii用 */
wiimote_t wiimote = WIIMOTE_INIT;
static int flagUp = 0;
static int flagDown = 0;
static int flagOne = 0;
static int flagTwo = 0;
static int flagHome = 0;
static int flagPlus = 0;
static int flagA = 0;
static int flagB = 0;

/* マップの本体等 */
WiiInfo *memory;
MapType gMaps[ MAP_Width ][ MAP_Height ];

CharaInfo	*gChara;	/* キャラデータ */
InputInfo	gInput;		/* 入力データ */
FieldInfo  gField;
SDL_Rect b_src  = { 0,0, WD_Width*MAP_ChipSize,WD_Height*MAP_ChipSize };
/* データファイルパス */ // サーバー側で扱う_i
static char gMapDataFile[] = "map.data";

static int myid;
static int flagInit = 0;

int FieldDraw_r(int reset);
static void Chara_SelDraw_r(int kflag); // キャラ選択画面の描画
static int DrowLine_r(int x , int y ,int size, int thick);

static long map_r(long x, long in_min, long in_max, long out_min, long out_max);
//riho ★★★　フォント読み込みのために追加
static TTF_Font *gTTF;
static char gFontFileE[] = "AozoraMinchoHeavy.ttf";

static char *gScrStr[2] = { "%d" , "死亡回数 %d 回"} ;
static TTF_Font *gTTF;
static SDL_Color colF = {255,255,255};
static SDL_Color colB = {0,0,0};
/*****************************************************************
  関数名	: InitWindows
  機能	: メインウインドウの表示，設定を行う
  引数	: なし
  出力	: 正常に設定できたとき0，失敗したとき-1
 *****************************************************************/
int InitWindows(void) // i
{

  int i;
  myid = clientID;
  /* 画像ファイルパス */
  static char gMapImgFile[] = "map.png";
  static char *gBackImgFile[ BK_NUM ] = {"title.png","title.png","chara_sel_img.png","chara_sel_img.png","field.png","end_img.png"};

  char *s,title[10];

  /* 引き数チェック */
  //assert(0<num && num<=MAX_CLIENTS);

  // キャラ・画像構造体の初期化(メモリ確保)
  gChara = (CharaInfo *)malloc(sizeof(CharaInfo) * CI_NUM);

  /* SDLの初期化 */
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("failed to initialize SDL.\n");
    return -1;
  }

  /* メインのウインドウを作成する */
  if((gMainWindow = SDL_SetVideoMode(WD_Width*MAP_ChipSize, WD_Height*MAP_ChipSize, 32, SDL_HWSURFACE|SDL_DOUBLEBUF)) == NULL) {
    printf("failed to initialize videomode.\n");
    return -1;
  }
  /* ウインドウのタイトルをセット */
  sprintf(title,"Player:%d",clientID);
  SDL_WM_SetCaption(title,NULL);

  /* 背景を白にする */
  SDL_FillRect(gMainWindow,NULL,0xffffff);

  /* ゲーム画面(フィールドバッファ)の作成 */
  gTheWorld = SDL_CreateRGBSurface(SDL_HWSURFACE, MAP_Width*MAP_ChipSize, MAP_Height*MAP_ChipSize, 32, 0, 0, 0, 0);
  if(gTheWorld == NULL) return PrintError( SDL_GetError() );

  /*wiiリモコンの接続を促す画面*/
  SDL_FillRect(gMainWindow, NULL, 0x000000ff);
  stringColor(gMainWindow, 0, 0, "Press buttons 1 and 2 on the wiimote now to connect.\n Or press any buttons.\n", 0xffffffff);
  SDL_Flip(gMainWindow);

  /* 画像の読み込み */
  gMapImage = IMG_Load( gMapImgFile );
  if(gMapImage == NULL) return PrintError("failed to open map image.");
  for(i=0; i<BK_NUM; i++){ //背景
    gBackImage[i] = IMG_Load( gBackImgFile[i] );
    gBackImage[i] = zoomSurface( gBackImage[i], (WD_Width*MAP_ChipSize)/512.0, (WD_Height*MAP_ChipSize)/320.0, 0 ); // 背景のサイズは固定

    if(gBackImage[i] == NULL){
      return PrintError("failed to open character image.");
    }
  }

  /*キャラの初期化*/
  for(i=0; i<CI_NUM; i++){
    gChara[i].stts = CS_Disable;
    gChara[i].type = CT_None;
    gChara[i].pos.x = WD_Width / 5 * MAP_ChipSize * (gChara[i].id+3);
    gChara[i].pos.y = (WD_Height - 3) * MAP_ChipSize;
  }

  /* 背景の作成 */
  gBG = SDL_CreateRGBSurface(SDL_HWSURFACE, MAP_Width*MAP_ChipSize, MAP_Height*MAP_ChipSize, 32, 0, 0, 0, 0);
  if(gBG == NULL) return PrintError( SDL_GetError() );
  gBGMask = SDL_CreateRGBSurface(SDL_HWSURFACE, MAP_Width*MAP_ChipSize, MAP_Height*MAP_ChipSize, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
  if(gBGMask == NULL) return PrintError( SDL_GetError() );
  MakeBG();

  //PlayWii();
  //gField.back = BK_Title; //★★★初期で背景がタイトルになるように


  /* 入力の構造体を初期化 */
  InitInput_y();

  //	gField.back = BK_Title;

  return 0;
}

// TODO ★★★　関数追加　安村
/*****************************************************************
  関数名	: InitSound
  機能	: BGM・効果音の初期化
  引数	: なし
  出力	: 正常に設定できたとき0，失敗したとき-1
 *****************************************************************/
int InitSound(void) {
	// サウンドの初期化
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		printf("failed to initialize SDL_mixer.\n");
		SDL_Quit();
		exit(-1);
	}

	// BGMの読み込み
	int i, j;
	for (i = 0; i < BT_NUM; i++) {
		gBgm[i] = Mix_LoadMUS(gBgmFile[i]);
		if (gBgm[i] == NULL){
			return PrintError("failed to open BGM file");
		}
	}
	
	// システム効果音の読み込み
	for (i = 0; i < ST_NUM; i++) {
        gSE[i] = Mix_LoadWAV( gSEFile[i] );
        if(gSE[i] == NULL){
            return PrintError("failed to open SE file.");
        }
    }
	// キャラ効果音の読み込み
	for (i = 0; i < CT_NUM; i++) {
		for (j = 0; j < NAST_NUM; j++) {
		    gSEChara[i][j] = Mix_LoadWAV( gSECharaFile[i][j] );
		    if(gSEChara[i][j] == NULL){
		        return PrintError("failed to open SEChara file.");
		    }
		}
	}

	Mix_VolumeMusic(30); // BGMの音量調整
	Mix_Volume(-1, 20); // 効果音の音量調整

	return 0;

}

/*****************************************************************
  関数名	: InitInput_y
  機能	: 入力構造体の初期化
  引数	: なし
  出力	: なし
 *****************************************************************/
void InitInput_y(void) // y
{
  gInput.dir1 = 0; // 方向1
  gInput.dir2 = 0; // 方向2
  gInput.button1 = AT_None; // 攻撃
  gInput.button2 = MT_Stand; // モーション
  gInput.shake = AT_None; // 必殺技 
}

/*****************************************************************
  関数名	: InitChara
  機能	: キャラの表示，設定を行う
  引数	: なし
  出力	: 正常に設定できたとき0，失敗したとき-1
 *****************************************************************/
int InitChara() // i
{
  static char *gCharaImgFile[ CI_NUM ] = {"c_naomi.png", "c_osumo.png", "c_ponzu.png", "c_ninja.png","c_naomi2.png", "c_osumo2.png", "c_ponzu2.png", "c_ninja2.png","mask.png"};
  static char *gAttackImgFile[ AI_NUM ] = {"a_naomi.png", "a_osumo.png", "a_ponzu.png", "a_ninja.png", "a_punch.png", "a_shake.png"};
  static char *gAttackLImgFile[ AI_NUM ] = {"a_naomi2.png", "a_osumo2.png", "a_ponzu2.png", "a_ninja2.png", "a_punch2.png", "a_shake2.png"};

  int i;

  /* 画像の読み込み */
  for(i=0; i<gClientNum; i++){
    if(gChara[i].type != CT_None){
      printf("gChara[i].type : %d\n",gChara[i].type);
      gCharaImage[i] = IMG_Load( gCharaImgFile[gChara[i].type] );
      gCharaLImage[i] = IMG_Load( gCharaImgFile[gChara[i].type + CI_Dir ] );
    }else{
      gCharaImage[i] = IMG_Load( gCharaImgFile[i] );
    }


  }
  for(i=0; i< CT_NUM*AT_NUM; i++){
    if(i < CT_NUM && gChara[i].type != CT_None){
      gAttackImage[i] = IMG_Load( gAttackImgFile[gChara[i].type] );
      gAttackLImage[i] = IMG_Load( gAttackLImgFile[gChara[i].type] );
    }else if(i < CT_NUM){
      gAttackImage[i] = IMG_Load( gAttackImgFile[i] );
      gAttackLImage[i] = IMG_Load( gAttackLImgFile[i] );
    }else if(i < CT_NUM * (AT_NUM-1)){
      gAttackImage[i] = IMG_Load( gAttackImgFile[AI_Punch] );
      gAttackLImage[i] = IMG_Load( gAttackLImgFile[AI_Punch] );
    }else if(i < CT_NUM * AT_NUM){
      gAttackImage[i] = IMG_Load( gAttackImgFile[AI_Shake] );
      gAttackLImage[i] = IMG_Load( gAttackLImgFile[AI_Shake] );
    }
    gChara[i].posAttack.x = gChara[i].posAttack.y = 0;
  }
  return 0;
}


/*****************************************************************
  関数名	: PlayWii
  機能	: wiiでプレイする
  引数	: なし
  出力	: なし
 *****************************************************************/
int PlayWii(void) //i
{
  // ***** Wiiリモコン処理 *****
  memory = (WiiInfo*)malloc( sizeof(WiiInfo) * 4 );
  if( wiiinfo == NULL ){
    return PrintError("failed to allocate memory.");
  }
  if (wiiinfo->ac < 2) {	// Wiiリモコン識別情報がコマンド引数で与えられなければ
    printf("Designate the wiimote ID to the application.\n");
    exit(1);
  }

  // Wiiリモコンの接続（１つのみ）
  if (wiimote_connect(&wiimote, wiiinfo->av[1]) < 0) {	// コマンド引数に指定したWiiリモコン識別情報を渡して接続
    printf("unable to open wiimote: %s\n", wiimote_get_error());
    exit(1);
  }

  wiimote.led.one  = 1;	// WiiリモコンのLEDの一番左を点灯させる（接続を知らせるために）
  // wiimote.led.four  = 1;	// WiiリモコンのLEDの一番右を点灯させる

  // Wiiリモコンスピーカの初期化
  wiimote_speaker_init(&wiimote, WIIMOTE_FMT_S4, WIIMOTE_FREQ_44800HZ);

  // センサからのデータを受け付けるモードに変更
  wiimote.mode.acc = 1;
}	

/*****************************************************************
  関数名	: DestroyWindow
  機能	: SDLを終了する
  引数	: なし
  出力	: なし
 *****************************************************************/
void DestroyWindow(void)
{
  /* サーフェイス,OpenCV */
  int i;
  for(i=0; i<BK_NUM; i++){
    SDL_FreeSurface(gBackImage[i]);
  }
  for(i=0; i<CI_NUM; i++)	
    SDL_FreeSurface(gCharaImage[i]);
  SDL_FreeSurface(gMapImage);
  SDL_FreeSurface(gMapRead);
  SDL_FreeSurface(gBG);
  SDL_FreeSurface(gBGMask);
  SDL_FreeSurface(gTheWorld);
  SDL_FreeSurface(gMainWindow);
  SDL_Quit();
}

/*****************************************************************
  関数名  : WindowEventT
  機能    : タイトル時のメインウインドウに対するイベント処理を行う
  引数    : なし
  出力    : なし
 *****************************************************************/
void WindowEventT(void) //i
{
  SDL_Event event;

  DrawBack(gField.back);

  FieldDraw_r(1);//値の初期化をするだけです
  if(wiimote_is_open(&wiimote)) {
    // Wiiリモコンの状態を取得・更新する
    if (wiimote_update(&wiimote) < 0) {
      wiimote_disconnect(&wiimote);
    }
    if (wiimote.keys.home && flagHome == 0) {
      flagHome = 1;
      SendEndCommand(); // 終了コマンド
    }
    else if(wiimote.keys.home ==0 && flagHome == 1)
      flagHome = 0;

    if (wiimote.keys.plus && flagPlus == 0) {
      flagPlus = 1;
      Mix_PlayChannel(-1, gSE[ST_Decide], 0); // 決定音の再生 TODO ★★★ 再生追加 安村
      SendTitleCommand(); // タイトルでスペースがをされたことを通知
      gField.back = BK_Title_Wait;
      flagInit = 0;
    }
    else if(wiimote.keys.plus == 1&& flagPlus == 1)
      flagPlus = 0;
  }
  if(SDL_PollEvent(&event)) {
    switch(event.type){
      case SDL_QUIT:
        break;
      case SDL_KEYDOWN:
        /* キーが押された方向を設定 */
        switch( event.key.keysym.sym ){
          case SDLK_ESCAPE:
            SendEndCommand(); // 終了コマンド
            break;
          case SDLK_SPACE :
            printf("pushed Space\n");
						Mix_PlayChannel(-1, gSE[ST_Decide], 0); // 決定音の再生 TODO ★★★ 再生追加 安村
            SendTitleCommand(); // タイトルでスペースがをされたことを通知
            gField.back = BK_Title_Wait; // 待機状態に遷移
            flagInit = 0;
            break;
          case SDLK_UP :
            break;
          case SDLK_DOWN :
            break;
          case SDLK_RIGHT :
            gInput.dir1 += 6;
            break;
          case SDLK_LEFT :
            gInput.dir1 += -6;
            break;
          default: break;
        }
        break;
      case SDL_KEYUP:
        /* 離されたときは解除 */
        switch( event.key.keysym.sym ){
          case SDLK_RIGHT:
            gInput.dir1 += -6;
            break;
          case SDLK_LEFT : 
            gInput.dir1 += 6;
            break;
          default: break;
        }
      default: break;
    }
  }
  BlitWindow();
}

/*****************************************************************
  関数名  : WindowEventC
  機能    : キャラ選択時のメインウインドウに対するイベント処理を行う
  引数    : なし
  出力    : なし
 *****************************************************************/
void WindowEventC(void) //i
{
  SDL_Event event;
  //画面選択画面用のフラグとして追加しました．
  static int keyFlag;
  static int CharaFlag = 0;
  static int i;

  DrawBack(gField.back);

  if(wiimote_is_open(&wiimote)) {
    Chara_SelDraw_r(keyFlag);
    // Wiiリモコンの状態を取得・更新する
    if (wiimote_update(&wiimote) < 0) {
      wiimote_disconnect(&wiimote);
    }
    if (wiimote.keys.two && flagTwo == 0) {
      flagTwo = 1;
      gChara[myid].type = keyFlag;
      Mix_PlayChannel(-1, gSE[ST_Decide], 0); // 決定音の再生 TODO ★★★ 再生追加 安村
      SendCharaSelCommand(keyFlag); // キャラ選択を通知
      gField.back = BK_Chara_Sel_Wait; // 待機状態に遷移
    }
    else if(wiimote.keys.two == 1 && flagTwo == 1)
      flagTwo = 0;

    if (wiimote.keys.down && flagDown == 0){
      flagDown = 1;
      gInput.dir1 += 6;
      keyFlag++;
      Mix_PlayChannel(-1, gSE[ST_Select], 0); // 選択音の再生 TODO ★★★ 再生追加 安村
      if(keyFlag == CT_NUM)
        keyFlag = 0 ;
      //drawFlag = 0 ;

    }	
    else if (wiimote.keys.down == 0 && flagDown == 1){
      flagDown = 0;
      gInput.dir1 -= 6;
    }
    printf("%d flag:%d\n",wiimote.keys.down,flagDown);
    if (wiimote.keys.up && flagUp == 0){
      flagUp = 1;
      gInput.dir1 += -6;
      keyFlag--;
      Mix_PlayChannel(-1, gSE[ST_Select], 0); // 選択音の再生 TODO ★★★ 再生追加 安村
      if(keyFlag == -1 ) keyFlag = CT_NUM - 1  ;
      //drawFlag = 0 ;

    }
    else if(wiimote.keys.up == 0&& flagUp == 1){ 
      flagUp = 0;
      gInput.dir1 += 6;
    }
    if (wiimote.keys.home && flagHome == 0) {
      flagHome = 1;
      gChara[myid].type = keyFlag;
      SendCharaSelCommand(keyFlag); // キャラ選択を通知
      gField.back = BK_Chara_Sel_Wait; // 待機状態に遷移
      flagInit = 0;
      //return 0;
    }
    else if(wiimote.keys.home == 1&& flagHome == 1)
      flagHome = 0;
  }

  Chara_SelDraw_r(keyFlag);

  if(SDL_PollEvent(&event) ) {
    switch(event.type){
      case SDL_QUIT: 
      case SDL_KEYDOWN:
        // キーが押された方向を設定 
        switch( event.key.keysym.sym ){
          case SDLK_ESCAPE:
            printf("pushed Escape\n");
            keyFlag = 0;
            SendBackTitleCommand(); // タイトルに戻ることをサーバに通知
            break;
          case SDLK_SPACE :
            printf("pushed Space\n");
            gChara[myid].type = keyFlag;
            Mix_PlayChannel(-1, gSE[ST_Decide], 0); // 決定音の再生 TODO ★★★ 再生追加 安村
            SendCharaSelCommand(keyFlag); // キャラ選択を通知
            gField.back = BK_Chara_Sel_Wait; // 待機状態に遷移
            flagInit = 0;
            break;
          case SDLK_UP :
            break;
          case SDLK_DOWN :
            break;
          case SDLK_RIGHT :
						Mix_PlayChannel(-1, gSE[ST_Select], 0); // 選択音の再生 TODO ★★★ 再生追加 安村
            gInput.dir1 += 6;
            keyFlag++;
            if(keyFlag == CT_NUM)
              keyFlag = 0 ;
            break;
          case SDLK_LEFT :
						Mix_PlayChannel(-1, gSE[ST_Select], 0); // 選択音の再生 TODO ★★★ 再生追加 安村
            gInput.dir1 += -6;
            keyFlag--;
            if(keyFlag == -1 ) keyFlag = CT_NUM - 1  ;
            break;
          default: break;
        }
        break;
      case SDL_KEYUP:
        /* 離されたときは解除 */
        switch( event.key.keysym.sym ){
          case SDLK_RIGHT:
            gInput.dir1 += -6;
            break;
          case SDLK_LEFT : 
            gInput.dir1 += 6;
            break;
          default: break;
        }
      default: break;
    }
  }
  BlitWindow();
}

//chara
/*****************************************************************
  関数名	: WindowEventF
  機能	: メインウインドウに対するイベント処理を行う
  引数	: なし
  出力	: なし
 *****************************************************************/
void WindowEventF(void) //i
{
  SDL_Event event;
  if(flagInit == 0){
    if(InitChara()==-1){
      fprintf(stderr,"setup failed : InitChara\n");
      //終了処理
      SendEndCommand(); // 終了コマンドを送る
    }
    gField.back = BK_Field;
    flagInit = 1;
  }

  DrawBack(gField.back);
  FieldDraw_r(0);
  if(wiimote_is_open(&wiimote)) {
    // Wiiリモコンの状態を取得・更新する
    if (wiimote_update(&wiimote) < 0) {
      wiimote_disconnect(&wiimote);
    }

    if (wiimote.keys.home && flagHome == 0) {
      flagHome = 1;
      InitInput_y(); // 入力構造体の初期化
      SendBackTitleCommand(); // タイトルに戻ることをサーバに通知
      flagInit = 0;
    }

    else if(wiimote.keys.home ==0 && flagHome == 1)
      flagHome = 0;

    if (wiimote.keys.two && flagTwo == 0) {
      flagTwo = 1;
      gInput.button2 = MT_Jump;
    }
    else if(wiimote.keys.two == 0&& flagTwo == 1){
      flagTwo = 0;
      gInput.button2 = MT_Stand;
    }

    if (wiimote.keys.one && flagOne == 0) {
      flagOne = 1;
      gInput.button1 = AT_Punch;
    }
    else if(wiimote.keys.one == 0&& flagOne == 1){
      flagOne = 0;
      gInput.button1 = AT_None;
    }		
    if (wiimote.keys.down && flagDown == 0) {
      flagDown = 1;
      gInput.dir1 += 6;
    }
    else if(wiimote.keys.down == 0 && flagDown == 1){
      flagDown = 0;
      gInput.dir1 += -6;

    }
    if (wiimote.keys.up && flagUp == 0) {
      flagUp = 1;
      gInput.dir1 += -6;
    }
    else if(wiimote.keys.up == 0 && flagUp == 1){
      gInput.dir1 += 6;
      flagUp = 0;
    }
  }

  if(SDL_PollEvent(&event)) {

    switch(event.type){
      case SDL_QUIT:
      case SDL_KEYDOWN:
        /* キーが押された方向を設定 */
        switch( event.key.keysym.sym ){
          case SDLK_ESCAPE:
            //case SDLK_SPACE :
            InitInput_y(); // 入力構造体の初期化
            SendBackTitleCommand(); // タイトルに戻ることをサーバに通知
            flagInit = 0;
            break;
          case SDLK_z :
            printf("pushed Z\n");
            gInput.button2 = MT_Jump;
            break;
          case SDLK_x :
            printf("pushed X\n");
            gInput.button1 = AT_Punch;
            break;
          case SDLK_RIGHT: 
            printf("pushed Right\n");
            gInput.dir1 += 6;
            break;
          case SDLK_LEFT :
            printf("pushed Left\n");
            gInput.dir1 += -6;
            break;
          default: break;
        }
        break;
      case SDL_KEYUP:
        /* 離されたときは解除 */
        switch( event.key.keysym.sym ){
          case SDLK_RIGHT:
            gInput.dir1 += -6;
            break;
          case SDLK_LEFT : 
            gInput.dir1 += 6;
            break;
          case SDLK_z :
            gInput.button2 = MT_Stand;
            break;
          case SDLK_x :
            gInput.button1 = AT_None;
            break;
          default: break;
        }
      default: break;
    }
  }
  return;
}

/*****************************************************************
  関数名   : WindowEventE
  機能   : 結果画面時のメインウインドウに対するイベント処理を行う
  引数   : なし
  出力   : なし
 *****************************************************************/
void WindowEventE(void) //i
{
  SDL_Event event;

  DrawBack(gField.back);

  /*
     if(wiimote_is_open(&wiimote) && flag == 0) {
     flag = 1;
  // Wiiリモコンの状態を取得・更新する
  if (wiimote_update(&wiimote) < 0) {
  wiimote_disconnect(&wiimote);
  }
  if (wiimote.keys.plus) {
  gField.back = BK_Title;
  }
  }
  else if (flag == 1){
  flag = 0;
  }
   */
  BlitWindow();

  if(wiimote_is_open(&wiimote) || SDL_PollEvent(&event) ) {
    switch(event.type){
      case SDL_QUIT:
      case SDL_KEYDOWN:
        /* キーが押された方向を設定 */
        switch( event.key.keysym.sym ){
          case SDLK_ESCAPE:
            SendBackTitleCommand(); // タイトルに戻ることをサーバに通知
            break;
          case SDLK_SPACE :
            printf("pushed Space\n");
            //gField.back = BK_Title; 
            break;
          case SDLK_UP :
            break;
          case SDLK_DOWN :
            break;
          case SDLK_RIGHT :
            gInput.dir1 += 6;
            break;
          case SDLK_LEFT :
            gInput.dir1 += -6;
            break;
          default: break;
        }
        break;
      case SDL_KEYUP:
        /* 離されたときは解除 */
        switch( event.key.keysym.sym ){
          case SDLK_RIGHT:
            gInput.dir1 += -6;
            break;
          case SDLK_LEFT : 
            gInput.dir1 += 6;
            break;
          default: break;
        }
      default: break;
    }
  }
  return;
}

/*****************************************************************
  関数名   : WindowEventW
  機能   : 待機時のメインウインドウに対するイベント処理を行う
  引数 : なし
  出力   : なし
 *****************************************************************/
int WindowEventW(void) //y
{
  SDL_Event event;

  DrawBack(gField.back);
  if(wiimote_is_open(&wiimote)) {
    // Wiiリモコンの状態を取得・更新する
    if (wiimote_update(&wiimote) < 0) {
      wiimote_disconnect(&wiimote);
    }
    if (wiimote.keys.down && flagDown == 0){
      flagDown = 1;
      gInput.dir1 += 6;
      //drawFlag = 0 ;

    }	
    else if (wiimote.keys.down == 0 && flagDown == 1){
      flagDown = 0;
      gInput.dir1 += -6;
    }

    //printf("%d flag:%d\n",wiimote.keys.down,flagDown);
    if (wiimote.keys.up && flagUp == 0){
      flagUp = 1;
      gInput.dir1 += -6;
      //drawFlag = 0 ;

    }
    else if(wiimote.keys.up == 0&& flagUp == 1){ 
      flagUp = 0;
      gInput.dir1 += 6;
    }
    if (wiimote.keys.home && flagHome == 0) {
      flagHome = 1;
      wiimote_speaker_free(&wiimote);	// Wiiリモコンのスピーカを解放
      wiimote_disconnect(&wiimote);	// Wiiリモコンとの接続を解除
      //return 0;
    }
    else if(wiimote.keys.home == 1 && flagHome == 1)
      flagHome = 0;
  }
  if(SDL_PollEvent(&event) ) {

    switch(event.type){
      case SDL_QUIT: return 0;
      case SDL_KEYDOWN:
                     /* キーが押された方向を設定 */
                     switch( event.key.keysym.sym ){
                       case SDLK_ESCAPE:
                         SendBackTitleCommand(); // タイトルに戻ることをサーバに通知
                         return 0;
                         break;
                       case SDLK_RIGHT:
                         gInput.dir1 += 6;
                         break;
                       case SDLK_LEFT : 
                         gInput.dir1 += -6;
                         break;
                       default: break;
                     }
                     break;
      case SDL_KEYUP:
                     /* 離されたときは解除 */
                     switch( event.key.keysym.sym ){
                       case SDLK_RIGHT:
                         gInput.dir1 += -6;
                         break;
                       case SDLK_LEFT : 
                         gInput.dir1 += 6;
                         break;
                       default: break;
                     }
      default: break;
    }
    //BlitWindow();
  }
  return 1;
}

/*************************************************************************
  新しく作りました riho　★★★
  関数名 : CharaSelDraw_r
  機能　: 死亡回数体力を表示する場所
  引数　: なし
  出力　: キャラ選択画面描画

 **************************************************************************/
int FieldDraw_r(int reset)
{
  int i ;
  static char gScore[64];
  static int FontSize = MAP_ChipSize;
  static int firstFlag = 1 ; 
  static int gHp;
  static int dick = 8;
  static SDL_Surface *img[CT_NUM], *hart_img;
  static SDL_Surface *lifeMes[CT_NUM];

  static SDL_Rect gChImgRect[CT_NUM ],gChImgRectH[CT_NUM ];
  static SDL_Rect gChLifeRect[CT_NUM ],gChHpRect[CT_NUM ];
  static char *gChImg[ CT_NUM ] = {"field_naomi_img.png","field_osumo_img.png","field_ponzu_img.png","field_ninja_img.png"};

  if( reset  )
  {
    firstFlag = 1 ;
  }
  else
  {


    if( gField.back == BK_Title )
      firstFlag = 1;
    // ★★★　11/27 選択したキャラクターに応じて表示するステータスを変えたいのでInitWindowではなくここで初期化することにしました rho
    if(firstFlag)
    {

      hart_img = IMG_Load("field_hart_img.png");
      if(hart_img == NULL) printf("hart IMG is not inserted");
      SDL_SetColorKey( hart_img, SDL_SRCCOLORKEY, SDL_MapRGB( hart_img->format, 255, 255, 255) );


      /* フォントからメッセージ画像作成 */
      /* 初期化 */
      if(TTF_Init() < 0){
        return PrintError( TTF_GetError() );
      }
      /* フォントを開く */
      gTTF = TTF_OpenFont( gFontFileE, FontSize );
      if( gTTF == NULL ) return PrintError( TTF_GetError() );

      for(i = 0 ; i < gClientNum ; i++)
      {

        gChImgRect[i].x = MAP_ChipSize *8.5 +  i * (MAP_ChipSize*3.5 + MAP_ChipSize/3) ;
        gChImgRect[i].y = MAP_ChipSize * (WD_Height -3) + MAP_ChipSize/8   ;
        gChImgRect[i].w = MAP_ChipSize * 2.5;
        gChImgRect[i].h = MAP_ChipSize * 2.5 ;

        img[i] = IMG_Load( gChImg[gChara[i].type]);
        if(img[i] == NULL){
          printf("%d 's imgdata is null\n client_win.c 660",i);
          return 2 ;
        }

        gChImgRectH[i].x = gChImgRect[i].x;
        gChImgRectH[i].y = gChImgRect[i].y + MAP_ChipSize *2; //- dick;
        gChImgRectH[i].w = MAP_ChipSize * 1;
        gChImgRectH[i].h = MAP_ChipSize * 0.5 ;

        gChLifeRect[i].x = gChImgRectH[i].x + MAP_ChipSize*1.2;
        gChLifeRect[i].y = gChImgRect[i].y + MAP_ChipSize * 1.5;// - dick;
        gChLifeRect[i].w = MAP_ChipSize * 2 ;     
        gChLifeRect[i].h = MAP_ChipSize * 2 ;     

        gChHpRect[i].x = gChImgRect[i].x ;
        gChHpRect[i].y = gChImgRect[i].y + MAP_ChipSize * 2.5 + dick;

      }
      firstFlag = 0 ;
    }
    for(i = 0 ; i < gClientNum ; i++)
    {
      sprintf(gScore, gScrStr[0], gChara[i].life);
      lifeMes[i] = TTF_RenderUTF8_Blended(gTTF, gScore, colB);
      SDL_BlitSurface(img[i],NULL,gTheWorld,&(gChImgRect[i]));
      SDL_BlitSurface(hart_img,NULL,gTheWorld,&(gChImgRectH[i]));
      SDL_BlitSurface(lifeMes[i],NULL,gTheWorld,&(gChLifeRect[i]));

      gHp = map_r(gChara[i].hp,0,100,0,120);

      thickLineColor(gTheWorld ,gChHpRect[i].x ,gChHpRect[i].y , gChHpRect[i].x + gHp ,gChHpRect[i].y, 8 ,  0xff0000ff);
      printf("Score Char %s\n",gScore);
      SDL_FreeSurface(lifeMes[i]);
    }
  }
  // printf("FieldDraw_r\n");
  // }
  //dflagf = 1;
return 0;
}
/*************************************************************************
  新しく作りました riho ★★★　11/22
  関数名 : CharaSelDraw_r
  機能　: キャラ選択画面描画
  引数　: なし
  出力　: キャラ選択画面描画

 **************************************************************************/
static void Chara_SelDraw_r(int kflag)
{
  int i ; 
  static SDL_Surface *image[CT_NUM], *image2[CT_NUM];//image selection image / image2 discription of image(character)
  static SDL_Rect gChSelRect[CT_NUM ];
  static char *gCharaSel[ CT_NUM ] = {"naomi_sel_img.png","osumo_sel_img.png","ponzu_sel_img.png","ninja_sel_img.png"};
  static char *gCharaDes[ CT_NUM ] = {"naomi_des_img.png","osumo_des_img.png","ponzu_des_img.png","ninja_des_img.png"};
  static int firstFlag = 1 ;
  static SDL_Rect gChDesRect = { MAP_ChipSize * 5.5 , MAP_ChipSize * 7   , MAP_ChipSize * 20 , MAP_ChipSize * 6}; 
  if(firstFlag)
  {
    for(i = 0 ; i < CT_NUM  ; i++)
    {
      gChSelRect[i].x = MAP_ChipSize * 5.5 + i * ( MAP_ChipSize * 5 + MAP_ChipSize/3 ) ;
      gChSelRect[i].y = MAP_ChipSize * 2 ;
      gChSelRect[i].w = MAP_ChipSize * 4;
      gChSelRect[i].h = MAP_ChipSize * 4 ;
      image[i] = IMG_Load(gCharaSel[i]);
      image2[i] = IMG_Load(gCharaDes[i]);

      SDL_SetAlpha( image[i], SDL_SRCALPHA, 0);
      SDL_SetColorKey( image[i], SDL_SRCCOLORKEY, SDL_MapRGB( image[i]->format, 255, 255, 255) );

    }
    firstFlag = 0 ;
  }
  //switch分の糞長いところ二行になりましたすいません★★★ riho
  for(i = 0 ; i < CT_NUM  ; i++)
  {
    SDL_BlitSurface(image[i],NULL,gTheWorld,&(gChSelRect[i]));
  }
  SDL_BlitSurface(image2[kflag],NULL,gTheWorld,&(gChDesRect));    
  DrowLine_r(gChSelRect[kflag].x,gChSelRect[kflag].y , gChSelRect[kflag].w,8);
  return  ;
}

/*************************************************************************
  新しく作りました riho ★★★　プロトタイプ宣言あり
  関数名 : DrawLine_r
  機能　: 4角形の枠を書く
  引数　: (x,y)始点の座標　size 大きさの幅 ,線の太さ
  出力　: 4角形を描く

 **************************************************************************/

static int DrowLine_r(int x , int y ,int size,int thick)
{
  thickLineColor(gTheWorld ,x,y , x + size  ,y, thick ,  0xff0000ff);
  thickLineColor(gTheWorld ,x,y ,x ,y+ size  , thick ,  0xff0000ff);
  thickLineColor(gTheWorld ,x + size ,y + size ,x + size , y,  thick ,  0xff0000ff);
  thickLineColor(gTheWorld ,x,y + size  ,x + size,y+ size, thick ,  0xff0000ff);
  return 0 ;
}
/*************************************************************************
  新しく作りました riho ★★★　プロトタイプ宣言あり
  関数名 : map_r
  機能　: arduinoのmap

 **************************************************************************/

static long map_r(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/*****************************************************************
  関数名	: DrawBack
  機能	: 背景を表示
  引数	: int		back		:背景の種類
  出力	: なし
 *****************************************************************/
int DrawBack(int back) //i
{
  SDL_Rect src  = { 0,0, WD_Width*MAP_ChipSize,WD_Height*MAP_ChipSize };
  SDL_Rect src2 = {gField.scroll , 0, WD_Width*MAP_ChipSize, WD_Height*MAP_ChipSize};
  SDL_Rect dst  = { 0 };
  int ret = SDL_BlitSurface(gBackImage[back], &src, gTheWorld, &dst);
  if (gField.back == BK_Field){
    ret += SDL_BlitSurface(gMapRead, &src2, gTheWorld, &dst);
  }
  return ret;
}

/*****************************************************************
  関数名	: DrawCharacter
  機能	: メインウインドウにキャラクタをすべて表示する
  引数	: なし
  出力	: なし
 *****************************************************************/
int DrawCharacter(void) //サーバーから構造体を送って表示できるようにする_m,i
{
  int i;
  int ret = 0;

  for(i=0; i<gClientNum; i++){
    if(gChara[i].stts != CS_Disable){
      SDL_Rect src = gChara[i].pos;
      src.x = src.w * gChara[i].anipat;
      src.y = 0;
      SDL_Rect dst = gChara[i].pos;
      switch(gChara[i].dir)
      {
        case DR_Right :
          ret += SDL_BlitSurface( gCharaImage[i], &src, gTheWorld, &dst );
          break;
        case DR_Left : 
          ret += SDL_BlitSurface( gCharaLImage[i], &src, gTheWorld, &dst );
          break;
      }
    }
    if(gChara[i].attack != AT_None){
      SDL_Rect src = gChara[i].posAttack;
      src.x = src.w * gChara[i].anipat;
      src.y = 0;
      SDL_Rect dst = gChara[i].posAttack;
      switch(gChara[i].dir)
      {
        case DR_Right :
          ret += SDL_BlitSurface( gAttackImage[i], &src, gTheWorld, &dst );
          break;
        case DR_Left : 
          ret += SDL_BlitSurface( gAttackLImage[i], &src, gTheWorld, &dst );
          break;
      }
    }
  }
  return ret;
}

/*****************************************************************
  関数名	: DrawMypos
  機能	: メインウインドウに自分のキャラクタ上に目印を表示する
  引数	: なし
  出力	: なし
 *****************************************************************/
int DrawMypos(void) //m
{
  int i;
  int ret = 0;
  for(i=0; i<gClientNum; i++){
    if(gChara[i].stts != CS_Disable && myid == i){
      ret += circleColor(gTheWorld,gChara[i].pos.x+((gChara[i].pos.w/2)-8),gChara[i].pos.y - 10,8,0xff0000ff);
    }
  }
  return ret;
}

/*****************************************************************
  関数名	: MakeBG
  機能	: 
  引数	: 
  出力	: 
 *****************************************************************/
int MakeBG(void) //m
{
  int i, j, ret = 0;
  SDL_Rect src  = { 0,0, MAP_ChipSize,MAP_ChipSize };
  SDL_Rect dst  = { 0 };
  /* マップ読み込み */
  FILE* fp = fopen( gMapDataFile, "r" );
  if( fp == NULL ) return PrintError("failed to open map data.");
  for(i=0; i<MAP_Height; i++){
    for(j=0; j<MAP_Width; j++){
      if(fscanf(fp, "%u", &gMaps[j][i]) !=1){
        fclose( fp );
        return PrintError("failed to load map data.");
      }
    }
  }
  fclose( fp );
  /* マップの配置 */
  for(i=0; i<MAP_Height; i++, dst.y+=MAP_ChipSize){
    dst.x = 0;
    for(j=0; j<MAP_Width; j++, dst.x+=MAP_ChipSize){
      src.x = gMaps[j][i] * MAP_ChipSize;
      src.y = 0;
      ret += SDL_BlitSurface(gMapImage, &src, gBG, &dst);

      src.y = MAP_ChipSize;
      ret += SDL_BlitSurface(gMapImage, &src, gBGMask, &dst);
    }
  }
  SDL_SetColorKey(gBG, SDL_SRCCOLORKEY, SDL_MapRGB(gBG->format, 0, 0, 0));
  gMapRead = SDL_DisplayFormat(gBG);

  return ret;
}

int BlitWindow(void)
{
  int ret;
  SDL_Rect src = {0, 0, WD_Width*MAP_ChipSize, WD_Height*MAP_ChipSize};
  /* メインウインドウへの転送 */
  ret = SDL_BlitSurface(gTheWorld, &src, gMainWindow, NULL);
  /* 描画更新 */
  ret += SDL_Flip(gMainWindow);
  return ret;
}

