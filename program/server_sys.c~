/*****************************************************************
ファイル名	: server_sys.c
機能		: あとでserver_command.cに統合する
*****************************************************************/

#include <opencv/highgui.h>

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_rotozoom.h>

#include"common.h"
#include"server_func.h"

#define SHOW_MASK 

#define G 9.81
#define Vo -25.0

/* データファイルパス */ // サーバー側で扱う_i
static char gMapDataFile[] = "map.data";

CharaInfo *gChara;
CameraData *gCam;

int		  gFlagHaruVio[MAX_CLIENTS]; 	/* 日馬富士バイオレンス状態の判定変数  ★★★追加安村 */
int		  gFlagShuNinja[MAX_CLIENTS]; 	/* 日馬富士バイオレンス状態の判定変数  ★★★追加安村 */
/* サーフェイス */
static SDL_Surface *gMainWindow;
static SDL_Surface *gTheWorld;
static SDL_Surface *gBG;
static SDL_Surface *gBGMask;
static SDL_Surface *gMapRead;
static SDL_Surface *gMapImage;
static SDL_Surface *gBackImage[ BK_NUM ];
static SDL_Surface *gCharaImage[ CI_NUM ];
//static SDL_Surface *gMessages[ MSG_NUM ];
static SDL_Surface *gCharaMask[ CI_NUM ];
static SDL_Surface *gAttackImage[ AI_NUM/2 ];
static SDL_Surface *gAttackLImage[ AI_NUM/2 ];
static SDL_Surface *gAttackMask[ AI_NUM/2 ];
static SDL_Surface *gAttackLMask[ AI_NUM/2 ];

//追加　松本
static SDL_Surface *gItemImage[ IT_NUM ];
static SDL_Surface *gItemMask[IT_NUM];


/* OpenCV関連 */
IplImage *gCvBGMask;
IplImage *gCvCharaMask[ CI_NUM ];
IplImage *gCvAttackMask[ AI_NUM/2 ];
IplImage *gCvAttackLMask[ AI_NUM/2 ];
IplImage *gCvAnd;
//追加　松本
IplImage *gCvItemMask[IT_NUM];
/* マップの本体等 */
MapType gMaps[ MAP_Width ][ MAP_Height ];

/* 関数 */
static CvRect GetIntersectRect( SDL_Rect a, SDL_Rect b );
static int CountMaxXY( const IplImage *data, int *x, int *y );
static CvPoint2D32f FixItemPoint1(CvPoint2D32f point);//追加　松本
static CvPoint2D32f FixItemPoint2(CvPoint2D32f point);//追加　松本
static CvPoint2D32f MoveStand( int i, CharaInfo *ch );
static CvPoint2D32f MoveFall( int i, CharaInfo *ch );
static CvPoint2D32f MoveJump( int i, CharaInfo *ch );
static CvPoint2D32f MoveStnby( int i, CharaInfo *ch );
static CvPoint2D32f MoveDamage( int i, CharaInfo *ch );//koko
static CvPoint2D32f MoveDamage_l( int i, CharaInfo *ch );
static CvPoint2D32f MoveItemStand1(void );//追加　松本
static CvPoint2D32f MoveItemFall1(void );//追加　松本
static CvPoint2D32f MoveItemStnby1(void );//追加　松本
static CvPoint2D32f MoveItemStand2(void );//追加　松本
static CvPoint2D32f MoveItemFall2(void );//追加　松本
static CvPoint2D32f MoveItemStnby2(void );//追加　松本
static int CharaDie_r(int i );
/*****************************************************************
関数名	: InitSystem
機能	: 初期化（本来はサーバーが行う）
引数	: なし
出力	: コネクションに失敗した時-1,成功した時0
*****************************************************************/
int InitSystem(int num) //i
{
	/* 画像ファイルパス */
	static char gFontFileE[] = "AozoraMinchoHeavy.ttf";
	static char gMapImgFile[] = "map.png";
    //add19
	static char *gBackImgFile[ BK_NUM ] = {"title.png","title.png","chara_sel_img.png","chara_sel_img.png","field.png","loading.png","end_img.png","end_img.png"};
	static int i,j;

    /* 乱数初期化 */
    srand(time(NULL));

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
	
	/* 背景を白にする */
	SDL_FillRect(gMainWindow,NULL,0xffffff);

	/* ゲーム画面(フィールドバッファ)の作成 */
	gTheWorld = SDL_CreateRGBSurface(SDL_HWSURFACE, MAP_Width*MAP_ChipSize, MAP_Height*MAP_ChipSize, 32, 0, 0, 0, 0);
	if(gTheWorld == NULL) return PrintError( SDL_GetError() );

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

    /* キャラクター情報初期化 */
    gClientNum = num;
    gChara = (CharaInfo*)malloc( sizeof(CharaInfo) * CI_NUM );
    if( gChara == NULL ){
        fclose( fp );
        return PrintError("failed to allocate memory.");
    }
    gCam = (CameraData*)malloc( sizeof(CameraData) * CI_NUM );
    if( gCam == NULL ){
        fclose( fp );
        return PrintError("failed to allocate memory.146");
    }
	gInput = (InputInfo*)malloc( sizeof(InputInfo) * gClientNum );
    if( gInput == NULL ){
        fclose( fp );
        return PrintError("failed to allocate memory.");
    }
	InitItemsys();//追加　松本
    /* ステータスの初期化 */
	InitCharaField_y();


    return 0;

}
/*****************************************************************
関数名	: InitCharasys
機能	: キャラの表示，設定を行う
引数	: なし
出力	: 正常に設定できたとき0，失敗したとき-1
*****************************************************************/
int InitCharasys() // i
{
//	SDL_Surface *image;

  static char *gCharaImgFile[ CI_NUM ] = {"c_naomi.png", "c_osumo.png", "c_ponzu.png", "c_ninja.png","c_naomi2.png", "c_osumo2.png", "c_ponzu2.png", "c_ninja2.png","mask.png"};
  static char *gAttackImgFile[ AI_NUM ] = {"a_naomi.png", "a_osumo.png", "a_ponzu.png", "a_ninja.png", "a_punch.png", "a_shake.png", "a_naomi2.png", "a_osumo2.png", "a_ponzu2.png", "a_ninja2.png", "a_punch2.png", "a_shake2.png"}; //atkoko 統一
  int i,j;
  printf("1\n"); // TODO 消す

  for(i=0; i<gClientNum; i++){
    if(gChara[i].type == CT_Naomi){
      gChara[i].speed = 1.5;
      gChara[i].power = 10;
    }else if(gChara[i].type == CT_Osumo){
      gChara[i].speed = 1.0;
      gChara[i].power = 15;
    }else if(gChara[i].type == CT_Ponzu){
      gChara[i].speed = 2.5;
      gChara[i].power = 7;
    }else if(gChara[i].type == CT_Ninja){
      gChara[i].speed = 2.0;
      gChara[i].power = 8;
    }
printf("R ; %d \n G ; %d \n B : %d \n" , gCam[i].red, gCam[i].green , gCam[i].blue);
    gChara[i].hp += gCam[i].green;
    gChara[i].maxhp = gChara[i].hp;
    gChara[i].power += gCam[i].red/10;
    gChara[i].speed += gCam[i].blue/10;
printf("R ; %d \n G ; %d \n B : %d \n" , gCam[i].red, gCam[i].green , gCam[i].blue);
  }

  /* キャラ画像の読み込み */
  for(i=0; i<CI_NUM; i++){

    if(i < gClientNum){
      gCharaImage[i] = IMG_Load( gCharaImgFile[gChara[i].type] );
      //gCharaLImage[i] = IMG_Load( gCharaImgFile[gChara[i].type + CI_Dir ] );

    }else{
      gCharaImage[i] = IMG_Load( gCharaImgFile[CI_Mask] );
    }

    SDL_Rect r = {0};

    r.w = gCharaImage[i]->w / 4;
    r.h = gCharaImage[i]->h / 2;
    r.x = 0; 
    r.y = r.h;

    if(gCharaImage[i] == NULL){
      return PrintError("failed to open character image.");
    }

    // don't need mask for direction of Lest , because we use same mask of right .
    /* マスク画像作成(SDL画像をOpenCVで使う) */
    gCharaMask[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, r.w, r.h, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
    SDL_BlitSurface(gCharaImage[i], &r, gCharaMask[i], NULL);

    gCvCharaMask[i] = cvCreateImageHeader(cvSize(r.w, r.h), IPL_DEPTH_8U, gCharaMask[i]->format->BytesPerPixel);
    cvSetData(gCvCharaMask[i], gCharaMask[i]->pixels, gCvCharaMask[i]->widthStep);
  }

  /* 攻撃画像の読み込み */
  for(i=0; i< AI_NUM/2; i++){ // atkoko 全部
    if(i < gClientNum){
      gAttackImage[i] = IMG_Load( gAttackImgFile[gChara[i].type] );
      gAttackLImage[i] = IMG_Load( gAttackImgFile[gChara[i].type + AI_Dir] );
    }else{
      gAttackImage[i] = IMG_Load( gAttackImgFile[i] );
      gAttackLImage[i] = IMG_Load( gAttackImgFile[i + AI_Dir] );
    }

    SDL_Rect r = {0};

    r.w = gAttackImage[i]->w / 4;
    r.h = gAttackImage[i]->h / 2;
    r.x = 0; 
    r.y = r.h;

    SDL_Rect rl = {0};

    rl.w = gAttackLImage[i]->w / 4;
    rl.h = gAttackLImage[i]->h / 2;
    rl.x = 0; 
    rl.y = rl.h;

    if(gAttackImage[i] == NULL){
      return PrintError("failed to open character image.");
    }

    /* マスク画像作成(SDL画像をOpenCVで使う) */
    gAttackMask[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, r.w, r.h, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
    SDL_BlitSurface(gAttackImage[i], &r, gAttackMask[i], NULL);

    gCvAttackMask[i] = cvCreateImageHeader(cvSize(r.w, r.h), IPL_DEPTH_8U, gAttackMask[i]->format->BytesPerPixel);
    cvSetData(gCvAttackMask[i], gAttackMask[i]->pixels, gCvAttackMask[i]->widthStep);
  }

  // atkoko 分割
  /* 画像サイズ，アニメーションパターン数の設定 */
  for(i=0; i<CI_NUM; i++){
    gChara[i].pos.w     = gCharaMask[i]->w;
    gChara[i].pos.h     = gCharaMask[i]->h;
    gChara[i].anipatnum = gCharaImage[i]->w / gChara[i].pos.w;
  }

  /* 画像サイズ，アニメーションパターン数の設定 */
  for(i=0; i<AI_NUM/2; i++){
    gChara[i].posAttack.w     = gAttackMask[i]->w;
    gChara[i].posAttack.h     = gAttackMask[i]->h;
  }


  /* マスク合成用 */ //一番大きい画像を選択
  gCvAnd = cvCreateImage(cvSize(gCharaMask[CI_Mask]->w,gCharaMask[CI_Mask]->h), IPL_DEPTH_8U, gCharaMask[CI_Mask]->format->BytesPerPixel);

  /* 背景の作成 */
  gBG = SDL_CreateRGBSurface(SDL_HWSURFACE, MAP_Width*MAP_ChipSize, MAP_Height*MAP_ChipSize, 32, 0, 0, 0, 0);
  if(gBG == NULL) return PrintError( SDL_GetError() );
  gBGMask = SDL_CreateRGBSurface(SDL_HWSURFACE, MAP_Width*MAP_ChipSize, MAP_Height*MAP_ChipSize, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
  if(gBGMask == NULL) return PrintError( SDL_GetError() );

  //-----------MakeBG---------------//
  /* マップの配置 */
  SDL_Rect src  = { 0,0, MAP_ChipSize,MAP_ChipSize };
  SDL_Rect dst  = { 0 };
  for(i=0; i<MAP_Height; i++, dst.y+=MAP_ChipSize){
    dst.x = 0;
    for(j=0; j<MAP_Width; j++, dst.x+=MAP_ChipSize){
      src.x = gMaps[j][i] * MAP_ChipSize;
      src.y = 0;
      SDL_BlitSurface(gMapImage, &src, gBG, &dst);

      src.y = MAP_ChipSize;
      SDL_BlitSurface(gMapImage, &src, gBGMask, &dst);
    }
  }
  SDL_SetColorKey(gBG, SDL_SRCCOLORKEY, SDL_MapRGB(gBG->format, 0, 0, 0));
  //printf("error\n");
  gMapRead = SDL_DisplayFormat(gBG);

  /* マスク画像作成 */
  gCvBGMask = cvCreateImageHeader(cvSize(gBGMask->w,gBGMask->h), IPL_DEPTH_8U, gBGMask->format->BytesPerPixel);
  cvSetData( gCvBGMask, gBGMask->pixels, gCvBGMask->widthStep);

  //---------------------------------------------------------------------//		


  return 0;
}

/*****************************************************************
  関数名	: InitItemsys//★追加 松本
  機能	: アイテム設定を行う
  引数	: なし
  出力	: 正常に設定できたとき0，失敗したとき-1
 *****************************************************************/
int InitItemsys() // i
{
  //	SDL_Surface *image;

  static char *gItemImgFile[ IT_NUM ] = {"prote.png","wing.png"};
  int i,j;
  printf("2\n"); // TODO 消す

  /* アイテム画像の読み込み */
  for(i=0; i<IT_NUM; i++){
    gItemImage[i] = IMG_Load( gItemImgFile[i] );
    printf("10\n");
    SDL_Rect r = {0};
    printf("11\n");
    r.w = gItemImage[i]->w;
    //r.w = 80;
    printf("12\n");
    r.h = gItemImage[i]->h/2;
    //r.h = 70;
    r.x = 0; 
    r.y = r.h;
    printf("3\n");
    if(gItemImage[i] == NULL){
      return PrintError("failed to open character image.");
    }
    printf("4\n");
    /* マスク画像作成(SDL画像をOpenCVで使う) */
    gItemMask[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, r.w, r.h, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
    printf("5\n");
    SDL_BlitSurface(gItemImage[i], &r, gItemMask[i], NULL);
    printf("6\n");
    gCvItemMask[i] = cvCreateImageHeader(cvSize(r.w, r.h), IPL_DEPTH_8U, gItemMask[i]->format->BytesPerPixel);
    cvSetData(gCvItemMask[i], gItemMask[i]->pixels, gCvItemMask[i]->widthStep);
  }
  gField.posItem1.w     = gItemMask[IT_Prote]->w;
  gField.posItem1.h     = gItemMask[IT_Prote]->h;
  gField.posItem2.w     = gItemMask[IT_Wing]->w;
  gField.posItem2.h     = gItemMask[IT_Wing]->h;
  return 0;
}

/*****************************************************************
  関数名	: InitCharaField_y
  機能	: キャラ・フィールド情報の初期化
  引数	: なし
  出力	: なし
 *****************************************************************/
void InitCharaField_y(void) //y
{
  int i;

  /* ステータスの初期化 */
  for(i = 0; i < CI_NUM; i++){
    // キャラの構造体
    gChara[i].id      = i;
    gChara[i].type    = CT_None;
    gChara[i].stts    = CS_Disable;
    gChara[i].dir     = DR_Right;
    gChara[i].pos.x   = WD_Width / 5 * MAP_ChipSize * (gChara[i].id+1);
    gChara[i].pos.y   = (WD_Height - 6) * MAP_ChipSize;
    gChara[i].point.x = WD_Width / 5 * MAP_ChipSize * (gChara[i].id+1);
    gChara[i].point.y = (WD_Height - 6) * MAP_ChipSize;
    gChara[i].base.x  = gChara[i].base.y  = 0;
    gChara[i].hp      = 100;
    gChara[i].life    = 0;
    gChara[i].t       = 0.0;
    gChara[i].tAttack = 0.0;
    gChara[i].motion  = MT_Stand;
    gChara[i].item    = IT_None;
    gChara[i].posAttack.x = gChara[i].posAttack.y = 0;
    gChara[i].anipat  = 0;
    gChara[i].anipatnum  = 0;
    gChara[i].flagJumpSE = 0;

    // TODO　仮
		gChara[i].type = CT_None;
		gChara[i].motion  = MT_Fall;
  }

  for(i = 0; i < gClientNum; i++){
    /* 入力の構造体を初期化 */
    gInput[i].dir1 = 0; // 方向1
    gInput[i].dir2 = 0; // 方向2
    gInput[i].button1 = AT_None; // 攻撃
    gInput[i].button2 = MT_Stand; // モーション
    gInput[i].shake = AT_None; // 必殺技 
    gChara[i].stts = CS_Enable; // クライアントの数だけ
    gChara[i].attack = AT_None;
  }
	TheWorld_y(-1); // 初期化 TODO ★★★追加安村
	HarumafujiViolence_y(-1); // 初期化 TODO ★★★追加安村
	ShushuttoNinjaja_y(-1); // 初期化 TODO ★★★追加安村
  /* フィールド情報初期化 */
  gField.map     = 0;
  gField.back    = BK_Title;
  gField.scroll  = 0;
  gField.stts1    = IS_Enable;	
  gField.item1    = IT_Prote;
  gField.posItem1.x = WD_Width*10;
  gField.posItem1.y = WD_Height / 2;
  gField.pointItem1.x =  WD_Width * 10;
  gField.pointItem1.y =  WD_Height / 2;
  gField.stts2    = IS_Enable;
  gField.item2    = IT_Wing;
  gField.posItem2.x =  WD_Width / 2;
  gField.posItem2.y =  WD_Height / 2;
  gField.pointItem2.x    =  WD_Width / 2;
  gField.pointItem2.y    =  WD_Height / 2;
  gField.motionItem1 = IM_Fall;
  gField.motionItem2 = IM_Fall;
}

CvPoint2D32f MoveStand( int i, CharaInfo *ch )
{
  CvPoint2D32f ret = ch->point;

  gChara[i].t = 0.0;
  if(ch->base.x != 0){
    ret.x += ch->base.x * ch->speed;
    if(  ch->base.x > 0 )//riho decision of direction hosihosihosi 方向キーを入力していない間に向く向きをこていすることもできる。短い時間の入力なら方向を変えずに向きを変えるだけとかもできる
      gChara[i].dir = DR_Right ; 
    else if ( ch->base.x < 0  ) 
      gChara[i].dir = DR_Left ; 
  }

  return ret;

}
CvPoint2D32f MoveFall( int i, CharaInfo *ch )
{
  CvPoint2D32f ret = ch->point;

  if(gChara[i].t < 20){
    gChara[i].t += 1.0;
  }
  if(ch->base.x != 0){
    ret.x += ch->base.x * ch->speed;
    if(  ch->base.x > 0 )//riho decision of direction hosihosihosi 方向キーを入力していない間に向く向きをこていすることもできる。短い時間の入力なら方向を変えずに向きを変えるだけとかもできる
      gChara[i].dir = DR_Right ; 
    else if ( ch->base.x < 0  ) 
      gChara[i].dir = DR_Left ; 
  }

  ret.y += G * (gChara[i].t/5.0);
  return ret;

}
CvPoint2D32f MoveJump( int i, CharaInfo *ch ){

  CvPoint2D32f ret = ch->point;

  if(ch->base.x != 0){
    ret.x += ch->base.x * ch->speed;
    if(  ch->base.x > 0 )//riho decision of direction hosihosihosi 方向キーを入力していない間に向く向きをこていすることもできる。短い時間の入力なら方向を変えずに向きを変えるだけとかもできる
      gChara[i].dir = DR_Right ; 
    else if ( ch->base.x < 0  ) 
      gChara[i].dir = DR_Left ; 
  }

  float a = Vo + (G * (gChara[i].t/5.0));
  ret.y += a;

  gChara[i].t += 1.0;
  if(a > 0){
    gChara[i].t = 0.0;
    gChara[i].motion = MT_Fall;
  }
  return ret;
}

// スタンバイ状態の移動
CvPoint2D32f MoveStnby( int i, CharaInfo *ch )
{
  CvPoint2D32f ret = ch->point;

  gChara[i].t = 0.0;
  if(gTheWorldID == -1) //  ★★★ 変更　安村
  	ret.x = ch->pos.x+1;
  ret.x = ch->pos.x+1;
  ret.y = ch->pos.y;
  return ret;

}

// 変更　乾
CvPoint2D32f MoveDamage( int i, CharaInfo *ch )
{
  CvPoint2D32f ret = ch->point;

  if(ch->motion != MT_Muteki) gChara[i].motion = MT_Muteki;

  gChara[i].t += 1.0;
  ret.x = ch->pos.x+4;
  if(gChara[i].t > 5){
    gChara[i].motion = MT_Fall;
  }

  return ret;
}

CvPoint2D32f MoveDamage_l( int i, CharaInfo *ch )
{
  CvPoint2D32f ret = ch->point;

  if(ch->motion != MT_Muteki_l) gChara[i].motion = MT_Muteki_l;

  gChara[i].t += 1.0;
  ret.x = ch->pos.x-4;
  if(gChara[i].t > 5){
    gChara[i].motion = MT_Fall;
  }

  return ret;
}
/* 障害物との重なりを調べ，補正した移動先を返す */
CvPoint2D32f FixPoint( int i, CharaInfo *ch, CvPoint2D32f point )
{
  CvRect cr = {point.x, point.y, ch->pos.w, ch->pos.h};
  CvRect ar = {ch->posAttack.x, ch->posAttack.y, ch->posAttack.w, ch->posAttack.h};
  /* 画面外に出たとき */
  /* 横方向は画面内に戻す */

  if(cr.x < 0) point.x = 0;
  if((cr.x+cr.width) > (WD_Width*MAP_ChipSize)){
    point.x = (WD_Width*MAP_ChipSize) - cr.width;
  }

  if(cr.y < 0) point.y = 0;
  if((cr.y+cr.height) > (WD_Height*MAP_ChipSize)){
    point.x = WD_Width / 5 * MAP_ChipSize * (i+1);
    point.y = 0;
    //	ch->stts = CS_Disable;
    return point;
  }

  if(ch->type == CT_None || gChara[i].motion == MT_Damage) return point;

  /* 合成範囲設定 */
  cr.x = gField.scroll + cr.x;
  cvSetImageROI(gCvBGMask, cr);
  cr = cvGetImageROI(gCvBGMask);
  cr.x = cr.y = 0;
  cvSetImageROI(gCvCharaMask[i], cr);

  cvSetImageCOI(gCvAnd, 0);
  cvSetZero(gCvAnd);
  cvSetImageROI(gCvAnd, cr);
  /* 画像のANDを取る．色が残っていると重なっていることになる */
  cvAnd(gCvBGMask, gCvCharaMask[i], gCvAnd, NULL);

  cvSetImageCOI(gCvAnd, 1);//青
  int x, y;
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.y = ch->point.y;
    gChara[i].motion = MT_Stand;
  }else if(gChara[i].motion == MT_Stand){
    gChara[i].motion = MT_Fall;
  }

  cvSetImageCOI(gCvAnd, 2);//緑
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.x = WD_Width / 5 * MAP_ChipSize * (i+1);
    point.y = 200; 
    gChara[i].motion = MT_Stnby;
    CharaDie_r(i); //riho hosihosihosi
  }

  cvSetImageCOI(gCvAnd, 3);//赤
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.y = ch->point.y-3;
    gChara[i].motion = MT_Stand;
  }

  /* 合成範囲設定 */ //inui
  ar.x = gField.scroll + ar.x;
  cvSetImageROI(gCvBGMask, ar);
  ar = cvGetImageROI(gCvBGMask);
  ar.x = ar.y = 0;
  cvSetImageROI(gCvAttackMask[ch->attack], ar);

  cvSetImageCOI(gCvAnd, 0);
  cvSetZero(gCvAnd);
  cvSetImageROI(gCvAnd, ar);
  /* 画像のANDを取る．色が残っていると重なっていることになる */
  cvAnd(gCvBGMask, gCvAttackMask[ch->attack], gCvAnd, NULL);

  cvSetImageCOI(gCvAnd, 1);//青
  if( CountMaxXY(gCvAnd, &x, &y) ){
    ch->attack = AT_None;
  }

  cvSetImageCOI(gCvAnd, 2);//緑
  if( CountMaxXY(gCvAnd, &x, &y) ){
    ch->attack = AT_None;
  }

  cvSetImageCOI(gCvAnd, 3);//赤
  if( CountMaxXY(gCvAnd, &x, &y) ){
    ch->attack = AT_None;
  }

  return point;
}

/* X,Y方向それぞれの最大ピクセル数を返す
 *
 * 引数
 *   data : 対象画像(COI,ROIを設定しておく)
 *   x, y : 各方向の最大ピクセル数を返す
 *
 * 返値: ピクセルがあるとき1，無いとき0
 */
int CountMaxXY( const IplImage *data, int *x, int *y )
{
  int i, j, cnt;
  CvRect cr = cvGetImageROI(data);
  int coi = cvGetImageCOI(data) - 1;
  for(j=0, *x=0; j<cr.height; j++){
    for(i=0, cnt=0; i<cr.width; i++){
      if( data->imageData[ data->widthStep*(cr.y + j) + data->nChannels*(cr.x + i) + coi ] ){
        cnt++;
      }
    }
    if(*x < cnt) *x = cnt;
  }
  for(i=0, *y=0; i<cr.width; i++){
    for(j=0, cnt=0; j<cr.height; j++){
      if( data->imageData[ data->widthStep*(cr.y + j) + data->nChannels*(cr.x + i) + coi ] ){
        cnt++;
      }
    }
    if(*y < cnt) *y = cnt;
  }
  return (*x && *y);
}

//追加　松本　ココから
void MoveItem(void)
{
	CvPoint2D32f newpoint1;
	CvPoint2D32f newpoint2;
	switch(gField.stts1){
		case IS_Enable:
			switch(gField.motionItem1){
				case IM_Fall:
					newpoint1 = MoveItemFall1(); break;
				case IM_Stand:
					newpoint1 = MoveItemStand1();break;
        		default: break;
			}
		case IS_Disable:
			switch(gField.motionItem1){
				case IM_Stnby:			
				newpoint1 = MoveItemStnby1(); break;
        		default: break;
			}
	default:break;
	}
	
	switch(gField.stts2){
		case IS_Enable:
			switch(gField.motionItem2){
				case IM_Fall:
				newpoint2 = MoveItemFall2(); break;
				case IM_Stand:
				newpoint2 = MoveItemStand2();break;
        	default: break;
			}
		case IS_Disable:
			switch(gField.motionItem2){
				case IM_Stnby:			
				newpoint2 = MoveItemStnby2(); break;
        		default: break;
			}
	default:break;
		
	}
	gField.pointItem1 = FixItemPoint1(newpoint1);
	gField.posItem1.x = gField.pointItem1.x;
    gField.posItem1.y = gField.pointItem1.y;
	gField.pointItem2 = FixItemPoint2(newpoint2);
	gField.posItem2.x = gField.pointItem2.x;
    gField.posItem2.y = gField.pointItem2.y;
}

/* 障害物との重なりを調べ，補正した移動先を返す 追加*/
CvPoint2D32f FixItemPoint1(CvPoint2D32f point )
{
  CvRect cr = {point.x, point.y, gField.posItem1.w, gField.posItem1.h};
  // 画面外に出たとき 
  // 横方向は画面内に戻す 

  if(cr.x < 0) {
	gField.stts1 = IS_Disable;
	gField.motionItem1 = IM_Stnby;
    return point;
  }
  if((cr.x+cr.width) > (WD_Width*MAP_ChipSize)){
    point.x = (WD_Width*MAP_ChipSize) - cr.width;
  }

  if(cr.y < 0) point.y = 0;
  if((cr.y+cr.height) > (WD_Height*MAP_ChipSize)){
    point.x = WD_Width / 5 * MAP_ChipSize * (1);
    point.y = 0;
	gField.stts1 = IS_Disable;
	gField.motionItem1 = IM_Stnby;
    return point;
  }

  //if(ch->type == CT_None || gChara[i].motion == MT_Damage) return point;

  // 合成範囲設定 
  cr.x = gField.scroll + cr.x;
  cvSetImageROI(gCvBGMask, cr);
  cr = cvGetImageROI(gCvBGMask);
  cr.x = cr.y = 0;
  cvSetImageROI(gCvItemMask[IT_Prote], cr);

  cvSetImageCOI(gCvAnd, 0);
  cvSetZero(gCvAnd);
  cvSetImageROI(gCvAnd, cr);
  // 画像のANDを取る．色が残っていると重なっていることになる 
  cvAnd(gCvBGMask, gCvItemMask[IT_Prote], gCvAnd, NULL);

  cvSetImageCOI(gCvAnd, 1);//青
  int x, y;
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.y = gField.pointItem1.y;
    gField.motionItem1 = IM_Stand;
  }else if(gField.motionItem1 == MT_Stand){
    gField.motionItem1 = IM_Fall;
  }

  cvSetImageCOI(gCvAnd, 2);//緑
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.x = WD_Width / 5 * MAP_ChipSize *1;
    point.y = 0; 
    	gField.stts1 = IS_Disable;
	gField.motionItem1 = IM_Stnby;
   // CharaDie_r(i); //riho hosihosihosi
  }

  cvSetImageCOI(gCvAnd, 3);//赤
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.y = gField.pointItem1.y-3;
    gField.motionItem1 = IM_Stand;//あとで変更
  }

  return point;
}

/* 障害物との重なりを調べ，補正した移動先を返す 追加*/
CvPoint2D32f FixItemPoint2(CvPoint2D32f point )
{
  CvRect cr = {point.x, point.y, gField.posItem2.w, gField.posItem2.h};
  // 画面外に出たとき 
  // 横方向は画面内に戻す 

  if(cr.x < 0){ 	
    gField.stts2 = IS_Disable;
	gField.motionItem2 = IM_Stnby;
    return point;
  }
  if((cr.x+cr.width) > (WD_Width*MAP_ChipSize)){
    point.x = (WD_Width*MAP_ChipSize) - cr.width;
  }

  if(cr.y < 0) point.y = 0;
  if((cr.y+cr.height) > (WD_Height*MAP_ChipSize)){
    point.x = WD_Width / 5 * MAP_ChipSize * (1);
    point.y = 0;
    	gField.stts2 = IS_Disable;
	gField.motionItem2 = IM_Stnby;
    return point;
  }

  //if(ch->type == CT_None || gChara[i].motion == MT_Damage) return point;

  // 合成範囲設定 
  cr.x = gField.scroll + cr.x;
  cvSetImageROI(gCvBGMask, cr);
  cr = cvGetImageROI(gCvBGMask);
  cr.x = cr.y = 0;
  cvSetImageROI(gCvItemMask[IT_Wing], cr);

  cvSetImageCOI(gCvAnd, 0);
  cvSetZero(gCvAnd);
  cvSetImageROI(gCvAnd, cr);
  // 画像のANDを取る．色が残っていると重なっていることになる 
  cvAnd(gCvBGMask, gCvItemMask[IT_Wing], gCvAnd, NULL);

  cvSetImageCOI(gCvAnd, 1);//青
  int x, y;
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.y = gField.pointItem2.y;
    gField.motionItem2 = IM_Stand;
  }else if(gField.motionItem2 == MT_Stand){
    gField.motionItem2 = IM_Fall;
  }

  cvSetImageCOI(gCvAnd, 2);//緑
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.x = WD_Width / 5 * MAP_ChipSize *1;
    point.y = 0; 
    gField.stts2 = IS_Disable;
	gField.motionItem2 = IM_Stnby;
   // CharaDie_r(i); //riho hosihosihosi
  }

  cvSetImageCOI(gCvAnd, 3);//赤
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.y = gField.pointItem2.y-3;
    gField.motionItem2 = IM_Stand;
  }

  return point;
}

CvPoint2D32f MoveItemStand1(void)
{
  CvPoint2D32f ret = gField.pointItem1;
  ret.y += 0;
  ret.x -= 1;
  return ret;
}


CvPoint2D32f MoveItemFall1(void)
{
  CvPoint2D32f ret = gField.pointItem1;
  ret.y += G;
  ret.x -= 1;
  return ret;
}
//追加　松本
CvPoint2D32f MoveItemStnby1(void)
{
  static int count;
  CvPoint2D32f ret = gField.pointItem1;
  if(count >= 150){
    gField.stts1 = IS_Enable;
    gField.motionItem1 = IM_Fall;
    ret.x = rand()%1000;
    ret.y = 0;
    count = 0;
  }
  if(gField.stts1 == IS_Disable)
  count++;
  return ret;
}
CvPoint2D32f MoveItemStand2(void)
{
  CvPoint2D32f ret = gField.pointItem2;
  ret.x -=1;
  ret.y +=0;
  return ret;
}
CvPoint2D32f MoveItemFall2(void)
{
  CvPoint2D32f ret = gField.pointItem2;
  ret.y += G /2.0;
  ret.x -= 1;
  return ret;
}

CvPoint2D32f MoveItemStnby2(void)
{
  static int count;
  CvPoint2D32f ret = gField.pointItem2;
  if(count >= 150){
    gField.stts2 = IS_Enable;
    gField.motionItem2 = IM_Fall;
    ret.x = rand()%1000;
    ret.y = 0;
    count = 0;
  }
  if(gField.stts2 == IS_Disable)
  count++;
  return ret;
}

//ここまで

/* キャラの移動 */
void MoveChara(void)
{
  int i;
  for(i=0; i<gClientNum; i++){
    //	if( gChara[i].stts == CS_Disable ){ //アイテムで使う

    CvPoint2D32f newpoint;
    /* キャラ別に移動先を計算 */
    switch( gChara[i].stts ){
      case CS_Enable:
				if(gTheWorldID == -1 || i == gTheWorldID) {//  ★★★ 追加安村
          switch( gChara[i].motion ){
            case MT_Stand:
              newpoint = MoveStand( i, &gChara[i] ); break;
            case MT_Fall:
              newpoint = MoveFall( i, &gChara[i] ); break;
            case MT_Jump:
              newpoint = MoveJump( i, &gChara[i] ); break;
            case MT_Stnby:
              newpoint = MoveStnby( i, &gChara[i] ); break;
            case MT_Damage:
							newpoint = MoveDamage( i, &gChara[i] ); break;
            case MT_Damage_l:
              newpoint = MoveDamage_l( i, &gChara[i] ); break;
            case MT_Muteki:
              newpoint = MoveDamage( i, &gChara[i] ); break;
            case MT_Muteki_l:
            	newpoint = MoveDamage_l( i, &gChara[i] ); break;
            default: break;
    			}
				}
				else { //止まっている場合  追加安村
					newpoint.x = gChara[i].pos.x;
					newpoint.y = gChara[i].pos.y;
				}
        switch( gChara[i].attack ){
          case AT_Punch:
            gChara[i].tAttack++;
            switch(gChara[i].dir) {//hosihosihosi riho depend on the character's direction.
              case DR_Right :
                gChara[i].posAttack.x = gChara[i].pos.x + 35;
                gChara[i].posAttack.y = gChara[i].pos.y;
                break;
              case DR_Left :
                gChara[i].posAttack.x = gChara[i].pos.x - 45;
                gChara[i].posAttack.y = gChara[i].pos.y;
                break;
            }
            if(gChara[i].tAttack > 5.0){
              gChara[i].attack = AT_None;
              gChara[i].tAttack = 0.0;
            }
            break;
          case AT_Naomi:
          case AT_Osumo:
          case AT_Ponzu:
          case AT_Ninja:
            if(gChara[i].throwAttack == 0){
              switch(gChara[i].dir)//hosihosihosi riho depend on the character's direction.
              {
                case DR_Right :
                  gChara[i].posAttack.x = gChara[i].pos.x + 20;
                  gChara[i].posAttack.y = gChara[i].pos.y;
                  break;
                case DR_Left :
                  gChara[i].posAttack.x = gChara[i].pos.x - 20;
                  gChara[i].posAttack.y = gChara[i].pos.y;
                  break;
              }
              gChara[i].throwAttack = 1;
            }								
            break;
          default: break;
        }
      default: break;
    }
    gChara[i].point = FixPoint( i, &gChara[i], newpoint );
    gChara[i].pos.x = gChara[i].point.x;
    gChara[i].pos.y = gChara[i].point.y;
    printf("OK\n");
    if(gChara[i].throwAttack > 0){
      gChara[i].throwAttack++;
      switch( gChara[i].attack ){
        case AT_Naomi:
        case AT_Osumo:
        case AT_Ponzu:
        case AT_Ninja:
          if(gChara[i].posAttack.x > gChara[i].pos.x){
            gChara[i].posAttack.x += 20;
            gChara[i].posAttack.y += gChara[i].throwAttack;
          }else{
            gChara[i].posAttack.x -= 20;
            gChara[i].posAttack.y += gChara[i].throwAttack;
          }
          break;
        default: break;
      }
      if(gChara[i].attack == AT_None){
        gChara[i].throwAttack = 0;
      }
    }
  } 
  }

  /*****************************************************************
    関数名  : Title_y
    機能    : タイトルの処理
    引数    : posクライアント番号　or -1 フラグのリセット
    出力    : なし
   *****************************************************************/
  void Title_y(int pos)  //y
  {
    int i;
    int count = 0;
    static int flagInput[MAX_CLIENTS]; // 入力済みのフラグ配列

    if (pos != -1) {
      flagInput[pos] = 1; // 入力済みのクライアントの場所にフラグを立てる
      for (i = 0; i < gClientNum; i++)
        count += flagInput[i];
      if (count == gClientNum) { // 全員がスペースを押した
        gField.back = BK_Chara_Sel;
        SendCharaSelCommand();
        for (i = 0; i < gClientNum; i++)
          flagInput[i] = 0;
      }
    }
    else { // フラグのリセット
      for (i = 0; i < gClientNum; i++)
        flagInput[i] = 0;
    }

#ifndef NDEBUG
    printf("#####\n");
    printf("Title_y()\n");
#endif
    return;
  }

  /*****************************************************************
    関数名  : CharaSel_y
    機能    : キャラ選択の処理
    引数    : posクライアント番号　or -1 フラグのリセット
    charaType キャラの種類　or -1 なし
    出力    : なし
   *****************************************************************/
  void CharaSel_y(int pos, int charaType)  //y
  {
    int i;
    int count = 0;
    static int flagInitChara;


    if (pos != -1) {
      gChara[pos].type = charaType; // 選択したキャラの設定
      for (i = 0; i < gClientNum; i++) // 全てのクライアントを走査
        if (gChara[i].type != CT_None) // キャラが入力されていれば
          count++; // カウントを進める
      if(count == gClientNum) { // 全員がキャラ選択を押した
        /*		if(flagInitChara == 0) { // 一回だけ呼ばれる
                if(InitCharasys() < 0) { // 初期化
                printf("failed to initialize system.\n");
                exit(-1);
                }
                count = 0;
                flagInitChara = 1;
                }*/
        if(InitCharasys() < 0) { // 初期化
          printf("failed to initialize system.\n");
          exit(-1);
        }
        count = 0;
        gField.back = BK_Field;
        SendFrame_r(); // フレームを送る 
      }
    }

    else { // リセットする
      for (i = 0; i < gClientNum; i++) // 全てのクライアントを走査
        gChara[pos].type = CT_None; // キャラ未入力状態にする
    }


#ifndef NDEBUG
    printf("#####\n");
    printf("CharaSel_y()\n");
#endif
    return;
  }

  /*****************************************************************
    関数名  : End_y
    機能    : 終了の処理
    引数    : なし
    出力    : なし
   *****************************************************************/
  void End_y(int pos)  //y
  {
    int i;
    for(i=0; i<CI_NUM; i++){
      gChara[i].anipat = (gChara[i].anipat+1) % 4;
    }
#ifndef NDEBUG
    printf("#####\n");
    printf("End_y()\n");
#endif
    return;
  }
  /*****************************************************************
    関数名  : getRank_r
    機能    : ランキング表示
    引数    : なし
    出力    : なし
   *****************************************************************/
  void getRank_r(int pos)  
  {
    int i,j;

    int tmp ; 

    for(i=0; i<gClientNum; i++)
    {
      gChara[i].rank = i + 1 ;
    }
    for( i = 0 ; i<gClientNum- 1 ; i++)
    {
      for( j = i+1 ; j<gClientNum ; j++)
      {
        if(gChara[i].life > gChara[j].life)
        {
          tmp = gChara[j].rank;
          gChara[i].rank = gChara[j].rank;
          gChara[j].rank = tmp ;
        }
      }
    }
    /*
       for( i = 0 ; i<gClientNum- 1 ; i++)
       {
       for( j = i+1 ; j<gClientNum ; j++)
       {
       if(gChara[i].life == gChara[j].life)
       {

       }
       }
       }
     */
#ifndef NDEBUG
    printf("#####\n");
    printf("getResult()\n");
#endif

    printf("GET RANK FUNC\n");
    return;
  }
  /*****************************************************************
    関数名  : CharaDie_r hosihosihosi riho
    作成者　: riho
    機能    : キャラが死んだ場合の処理
    引数    : なし
    出力    : なし
   *****************************************************************/
  static int CharaDie_r(int i )
  {
    gChara[i].life += 1;
    gChara[i].hp = gChara[i].maxhp;
    gChara[i].motion = MT_Stnby;
    gChara[i].pos.x = gChara[i].point.x = WD_Width / 5 * MAP_ChipSize * (i+1);
    gChara[i].pos.y = gChara[i].point.y = 200;

    return 0; 
  }

  /* 当たり判定 */
  void Collision_a( CharaInfo *ci, CharaInfo *cj )//乾
  {
    int i;
    /* 判定が不要な組み合わせを除外 */
    if(ci->stts==CS_Disable || cj->attack==AT_None) return;
    if(ci->id == cj->id) return;

    /* まずは矩形での重なりを検出 */
    CvRect r = GetIntersectRect( ci->pos, cj->posAttack );
    if( r.width>0 && r.height>0 )
    {   /* 重なった領域のマスク画像を合成 */
      r.x -= ci->pos.x;
      r.y -= ci->pos.y;
      cvSetImageROI(gCvCharaMask[ ci->id ], r);
      r.x += ci->pos.x - cj->posAttack.x;
      r.y += ci->pos.y - cj->posAttack.y;
      if(cj->attack == AT_Punch){
        cvSetImageROI(gCvAttackMask[ AI_Punch ], r);
      }else{
        cvSetImageROI(gCvAttackMask[ cj->type ], r);
      }
      r.x = r.y = 0;
      cvSetImageCOI(gCvAnd, 0);
      cvSetImageROI(gCvAnd, r);

      if(cj->attack == AT_Punch){
        cvAnd(gCvCharaMask[ ci->id ], gCvAttackMask[ AI_Punch ], gCvAnd, NULL);
      }else{
        cvAnd(gCvCharaMask[ ci->id ], gCvAttackMask[ cj->type ], gCvAnd, NULL);
      }
      /* 当たった(青の重なりを調べる) */
      cvSetImageCOI(gCvAnd, 1);
      if( cvCountNonZero(gCvAnd) ){
        for (i = 0; i < gClientNum; i++){
          if(ci->id == i){
            if(gChara[i].motion != MT_Damage && gChara[i].motion != MT_Muteki && gChara[i].motion != MT_Damage_l && gChara[i].motion != MT_Muteki_l && gChara[i].motion != MT_Stnby){
              gChara[i].t = 0.0;
              gChara[i].hp -= cj->power;
              if(gChara[i].hp < 0) gChara[i].hp = 0;
              if(gChara[cj->id].dir == DR_Right){
                gChara[i].motion = MT_Damage;
              }else{
                gChara[i].motion = MT_Damage_l;
              }
              if(gChara[i].hp <= 0){
                CharaDie_r(i);
              }
              return;
            }
          }
        }
      }
    }
  }

  /* 2つの矩形が重なる矩形を返す */
  CvRect GetIntersectRect( SDL_Rect a, SDL_Rect b )
  {
    CvRect ret;
    ret.x = (a.x > b.x)? a.x:b.x;
    ret.y = (a.y > b.y)? a.y:b.y;
    ret.width = (a.x + a.w < b.x + b.w)? a.x + a.w - ret.x : b.x + b.w - ret.x;
    ret.height = (a.y + a.h < b.y + b.h)? a.y + a.h - ret.y : b.y + b.h - ret.y;
    return ret;
  }


//  ★★★　関数追加　安村
/*****************************************************************
関数名  : TheWorld_y
機能    : The Worldの秒数・フラグ管理の関数
引数    : flagInit : 0 通常 -1 初期化
出力    : なし
*****************************************************************/
void TheWorld_y(int flagInit) { // y
	int i;
	static int count;
	if(flagInit == 0) {
		if(gTheWorldID == -1){ // TheWorld状態でない
			for(i = 0; i < gClientNum; i++) {
				if(gChara[i].type == CT_Naomi && gChara[i].finisher == AT_Shake && gChara[i].motion != MT_Stnby){
					gTheWorldID = i;
					break;
				}
			}
		}
		else { // TheWorld発動中
			count++;
			printf("TheWorld!!\n");
			printf("count = %d\n", count);
			if(gChara[gTheWorldID].motion == MT_Stnby) {
				count = 0;
				gTheWorldID = -1;
			}
			if(count >= 150) {
				count = 0;
				for(i = 0; i < gClientNum; i++) {
					if(i != gTheWorldID) {
						if(gChara[i].motion != MT_Stnby) {
							gChara[i].t = 0;
							gChara[i].motion = MT_Fall;
						}
					}
				}
				gTheWorldID = -1;
			}
		}
	}
	if(flagInit == -1) { // 初期化
		gTheWorldID = -1;
		count = 0;
	}
#ifndef NDEBUG
    //printf("#####\n");
    //printf("TheWorld()\n");
	//printf("gTheWorldID = %d\n", gTheWorldID);
#endif
	return;
}


//  ★★★　関数追加　安村
/*****************************************************************
関数名  : HarumafujiViolence_y
機能    : 必殺技"日馬富士バイオレンス"の秒数・フラグ管理の関数
引数    : flagInit : 0 通常 -1 初期化
出力    : なし
*****************************************************************/
void HarumafujiViolence_y(int flagInit) { // y
	int i;
	static int count[MAX_CLIENTS];
	if(flagInit == 0) {
		for(i = 0; i < gClientNum; i++) {
			if(gFlagHaruVio[i] == 0){ // HaruVio状態でない
				if(gChara[i].type == CT_Osumo && gChara[i].finisher == AT_Shake && gChara[i].motion != MT_Stnby){
					gFlagHaruVio[i] = 1;
					break;
				}
			}
			else { // HaruVio発動中
				count[i]++;
				printf("HaruVio!!\n");
				printf("ID = %d count = %d\n", i, count[i]);
				if(gChara[i].motion == MT_Stnby) {
					count[i] = 0;
					gFlagHaruVio[i] = 0;
				}
				if(count[i] >= 150) {
					count[i] = 0;
					gFlagHaruVio[i] = 0;
				}
			}
		}
	}
	if(flagInit == -1) { // 初期化
		for(i = 0; i < gClientNum; i++) {
			gFlagHaruVio[i] = 0;
			count[0] = 0;
		}
	}
#ifndef NDEBUG
    printf("#####\n");
    printf("HarumafujiViolence()\n");
#endif
	return;
}

//  ★★★　関数追加　安村
/*****************************************************************
関数名  : ShushuttoNinjaja_y
機能    : 必殺技"シュシュッとニンジャジャ"の秒数・フラグ管理の関数
引数    : flagInit : 0 通常 -1 初期化
出力    : なし
*****************************************************************/
void ShushuttoNinjaja_y(int flagInit) { // y
	int i;
	static int count[MAX_CLIENTS];
	if(flagInit == 0) {
		for(i = 0; i < gClientNum; i++) {
			if(gFlagShuNinja[i] == 0){ // ShuNinja状態でない
				if(gChara[i].type == CT_Ninja && gChara[i].finisher == AT_Shake && gChara[i].motion != MT_Stnby){
					gFlagShuNinja[i] = 1;
					break;
				}
			}
			else { // ShuNinja発動中
				count[i]++;
				printf("ShuNinja!!\n");
				printf("ID = %d count = %d\n", i, count[i]);
				if(gChara[i].motion == MT_Stnby) {
					count[i] = 0;
					gFlagShuNinja[i] = 0;
				}
				if(count[i] >= 150) {
					count[i] = 0;
					gFlagShuNinja[i] = 0;
				}
			}
		}
	}
	if(flagInit == -1) { // 初期化
		for(i = 0; i < gClientNum; i++) {
			gFlagShuNinja[i] = 0;
			count[0] = 0;
		}
	}
#ifndef NDEBUG
    printf("#####\n");
    printf("ShushuttoNinjaja()\n");
#endif
	return;
}

//○○追加　松本
   /* 当たり判定 */
  void Collision_i_1( CharaInfo *ci)//松本
  {
    int i;
    /* 判定が不要な組み合わせを除外 */
    if(ci->stts==CS_Disable) return;
    //if(ci->id == cj->id) return;

    /* まずは矩形での重なりを検出 */
    CvRect r = GetIntersectRect( ci->pos, gField.posItem1 );
    if( r.width>0 && r.height>0 )
    {   /* 重なった領域のマスク画像を合成 */
      r.x -= ci->pos.x;
      r.y -= ci->pos.y;
      cvSetImageROI(gCvCharaMask[ ci->id ], r);
      r.x += ci->pos.x - gField.posItem1.x;
      r.y += ci->pos.y - gField.posItem1.y;
			if(gField.item1 == IT_Prote){
	      cvSetImageROI(gCvItemMask[ IT_Prote ], r);
			}
      r.x = r.y = 0;
      cvSetImageCOI(gCvAnd, 0);
      cvSetImageROI(gCvAnd, r);

			if(gField.item1 == IT_Prote){
      	cvAnd(gCvCharaMask[ ci->id ], gCvItemMask[ IT_Prote ], gCvAnd, NULL);
			}
      /* 当たった(青の重なりを調べる) */
      cvSetImageCOI(gCvAnd, 1);
      if( cvCountNonZero(gCvAnd) ){
        for (i = 0; i < gClientNum; i++){
          if(ci->id == i){
          		gField.stts1 = IS_Disable;
				ci->item = IT_Prote;
				gField.motionItem1 = IM_Stnby;
              return;
          }
        }
      }
    }
  }

   /* 当たり判定 */
  void Collision_i_2( CharaInfo *ci)//松本
  {
    int i;
    /* 判定が不要な組み合わせを除外 */
    if(ci->stts==CS_Disable) return;
    //if(ci->id == cj->id) return;

    /* まずは矩形での重なりを検出 */
    CvRect r = GetIntersectRect( ci->pos, gField.posItem2 );
    if( r.width>0 && r.height>0 )
    {   /* 重なった領域のマスク画像を合成 */
      r.x -= ci->pos.x;
      r.y -= ci->pos.y;
      cvSetImageROI(gCvCharaMask[ ci->id ], r);
      r.x += ci->pos.x - gField.posItem2.x;
      r.y += ci->pos.y - gField.posItem2.y;
			if(gField.item2 == IT_Wing){
	      cvSetImageROI(gCvItemMask[ IT_Wing ], r);
			}
      r.x = r.y = 0;
      cvSetImageCOI(gCvAnd, 0);
      cvSetImageROI(gCvAnd, r);

			if(gField.item2 == IT_Wing){//○○　追加　松本（変更）
      	cvAnd(gCvCharaMask[ ci->id ], gCvItemMask[ IT_Wing ], gCvAnd, NULL);
			}
      /* 当たった(青の重なりを調べる) */
      cvSetImageCOI(gCvAnd, 1);
      if( cvCountNonZero(gCvAnd) ){
        for (i = 0; i < gClientNum; i++){
          if(ci->id == i){
          		gField.stts2 = IS_Disable;
				gField.motionItem2 = IM_Stnby;
			    ci->item = IT_Wing;
              return;
          }
        }
      }
    }
  }

