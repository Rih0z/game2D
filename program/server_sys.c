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
static SDL_Surface *gAttackImage[ CT_NUM * AT_NUM ];
static SDL_Surface *gAttackMask[ CT_NUM * AT_NUM ];

/* OpenCV関連 */
IplImage *gCvBGMask;
IplImage *gCvCharaMask[ CI_NUM ];
IplImage *gCvAttackMask[ CT_NUM * AT_NUM ];
IplImage *gCvAnd;

/* マップの本体等 */
MapType gMaps[ MAP_Width ][ MAP_Height ];

/* 関数 */
static CvRect GetIntersectRect( SDL_Rect a, SDL_Rect b );
static int CountMaxXY( const IplImage *data, int *x, int *y );
static CvPoint2D32f MoveStand( int i, CharaInfo *ch );
static CvPoint2D32f MoveFall( int i, CharaInfo *ch );
static CvPoint2D32f MoveJump( int i, CharaInfo *ch );
static CvPoint2D32f MoveStnby( int i, CharaInfo *ch );// TODO ★★★ 追加 安村
static CvPoint2D32f MoveDamage( int i, CharaInfo *ch );//koko

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
	static char *gBackImgFile[ BK_NUM ] = {"title.png","title.png","chara_sel_img.png","chara_sel_img.png","field.png","end_img.png"};
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

	gInput = (InputInfo*)malloc( sizeof(InputInfo) * gClientNum );
    if( gInput == NULL ){
        fclose( fp );
        return PrintError("failed to allocate memory.");
    }

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
	static char *gAttackImgFile[ AI_NUM ] = {"a_naomi.png", "a_osumo.png", "a_ponzu.png", "a_ninja.png", "a_punch.png", "a_shake.png"};
	int i,j;
	printf("1\n"); // TODO 消す

	for(i=0; i<gClientNum; i++){
		if(gChara[i].type == CT_Naomi){
			gChara[i].speed = 6.0;
			gChara[i].power = 10;
		}else if(gChara[i].type == CT_Osumo){
			gChara[i].speed = 4.0;
			gChara[i].power = 15;
		}else if(gChara[i].type == CT_Ponzu){
			gChara[i].speed = 8.0;
			gChara[i].power = 7;
		}else if(gChara[i].type == CT_Ninja){
			gChara[i].speed = 7.0;
			gChara[i].power = 8;
		}
	}

	/* キャラ画像の読み込み */
	for(i=0; i<CI_NUM; i++){
		if(gChara[i].type != CT_None){
			gCharaImage[i] = IMG_Load( gCharaImgFile[gChara[i].type] );
		}else{
			gCharaImage[i] = IMG_Load( gCharaImgFile[i] );
		}

		SDL_Rect r = {0};

		r.w = gCharaImage[i]->w / 4;
		r.h = gCharaImage[i]->h / 2;
		r.x = 0; 
		r.y = r.h;

		if(gCharaImage[i] == NULL){
			return PrintError("failed to open character image.");
		}

		/* マスク画像作成(SDL画像をOpenCVで使う) */
		gCharaMask[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, r.w, r.h, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
    	SDL_BlitSurface(gCharaImage[i], &r, gCharaMask[i], NULL);

        gCvCharaMask[i] = cvCreateImageHeader(cvSize(r.w, r.h), IPL_DEPTH_8U, gCharaMask[i]->format->BytesPerPixel);
    	cvSetData(gCvCharaMask[i], gCharaMask[i]->pixels, gCvCharaMask[i]->widthStep);
	}

	/* 攻撃画像の読み込み */
	for(i=0; i< CT_NUM*AT_NUM; i++){
		if(i < CT_NUM && gChara[i].type != CT_None){
			gAttackImage[i] = IMG_Load( gAttackImgFile[gChara[i].type] );
		}else if(i < CT_NUM){
			gAttackImage[i] = IMG_Load( gAttackImgFile[i] );
		}else if(i < CT_NUM * (AT_NUM-1)){
			gAttackImage[i] = IMG_Load( gAttackImgFile[AI_Punch] );
		}else if(i < CT_NUM * AT_NUM){
			gAttackImage[i] = IMG_Load( gAttackImgFile[AI_Shake] );
		}

		SDL_Rect r = {0};

		r.w = gAttackImage[i]->w / 4;
		r.h = gAttackImage[i]->h / 2;
		r.x = 0; 
		r.y = r.h;

		if(gAttackImage[i] == NULL){
			return PrintError("failed to open character image.");
		}

		/* マスク画像作成(SDL画像をOpenCVで使う) */
		gAttackMask[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, r.w, r.h, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
    	SDL_BlitSurface(gAttackImage[i], &r, gAttackMask[i], NULL);

        gCvAttackMask[i] = cvCreateImageHeader(cvSize(r.w, r.h), IPL_DEPTH_8U, gAttackMask[i]->format->BytesPerPixel);
    	cvSetData(gCvAttackMask[i], gAttackMask[i]->pixels, gCvAttackMask[i]->widthStep);
	}

	/* 画像サイズ，アニメーションパターン数の設定 */
	for(i=0; i<CI_NUM; i++){
		gChara[i].pos.w     = gCharaMask[i]->w;
		gChara[i].pos.h     = gCharaMask[i]->h;
		gChara[i].anipatnum = gCharaImage[i]->w / gChara[i].pos.w;
		gChara[i].posAttack.w     = gAttackMask[i]->w;
		gChara[i].posAttack.h     = gAttackMask[i]->h;
		gChara[i].anipatnum = gAttackImage[i]->w / gChara[i].posAttack.w;
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

    /* フィールド情報初期化 */
    gField.map     = 0;
    gField.back    = BK_Title;
    gField.scroll  = 0;
    gField.item    = IT_None;
    gField.posItem = 0;
}

CvPoint2D32f MoveStand( int i, CharaInfo *ch )
{
  CvPoint2D32f ret = ch->point;

  gChara[i].t = 0.0;
  if(ch->base.x != 0){
    ret.x += ch->base.x;
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
    ret.x += ch->base.x;
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
    ret.x += ch->base.x;
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

// TODO ★★★ 関数追加　安村
CvPoint2D32f MoveStnby( int i, CharaInfo *ch )
{
  CvPoint2D32f ret = ch->point;

  gChara[i].t = 0.0;
  ret.x = ch->pos.x+1;
  ret.y = ch->pos.y;
  return ret;

}

// koko
CvPoint2D32f MoveDamage( int i, CharaInfo *ch )
{
  CvPoint2D32f ret = ch->point;

  gChara[i].motion = MT_Fall;
  return ret;

}

/* 障害物との重なりを調べ，補正した移動先を返す */
CvPoint2D32f FixPoint( int i, CharaInfo *ch, CvPoint2D32f point )
{
  CvRect cr = {point.x, point.y, ch->pos.w, ch->pos.h};
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

  // TODO 追加項目(安村)
  cvSetImageCOI(gCvAnd, 2);//緑
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.x = WD_Width / 5 * MAP_ChipSize * (i+1);
    point.y = 200; // TODO ★★★ 変更 安村
    gChara[i].motion = MT_Stnby;// TODO ★★★ 変更 安村
  }

  cvSetImageCOI(gCvAnd, 3);//赤
  if( CountMaxXY(gCvAnd, &x, &y) ){
    point.y = ch->point.y-3;
    gChara[i].motion = MT_Stand;
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
        switch( gChara[i].motion ){
          case MT_Stand:
            newpoint = MoveStand( i, &gChara[i] ); break;
          case MT_Fall:
            newpoint = MoveFall( i, &gChara[i] ); break;
          case MT_Jump:
            newpoint = MoveJump( i, &gChara[i] ); break;
          case MT_Stnby:// TODO ★★★ 追加 安村
            newpoint = MoveStnby( i, &gChara[i] ); break;
          case MT_Damage:// koko
            newpoint = MoveDamage( i, &gChara[i] ); break;
          default: break;
        }
        switch( gChara[i].attack ){
          case AT_Punch:
            gChara[i].tAttack++;
            gChara[i].posAttack.x = gChara[i].pos.x + 10;
            gChara[i].posAttack.y = gChara[i].pos.y;
            if(gChara[i].tAttack > 10.0){
              gChara[i].attack = AT_None;
              gChara[i].tAttack = 0.0;
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

  /* 当たり判定 */
  void Collision_a( CharaInfo *ci, CharaInfo *cj )
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
      cvSetImageROI(gCvAttackMask[ cj->id ], r);
      r.x = r.y = 0;
      cvSetImageCOI(gCvAnd, 0);
      cvSetImageROI(gCvAnd, r);

      cvAnd(gCvCharaMask[ ci->id ], gCvAttackMask[ cj->id ], gCvAnd, NULL);

      /* 当たった(青の重なりを調べる) */
      cvSetImageCOI(gCvAnd, 1);
      if( cvCountNonZero(gCvAnd) ){
        for (i = 0; i < gClientNum; i++){
          if(ci->id == i){
            if(cj->attack == AT_Punch && gChara[i].motion != MT_Damage && gChara[i].motion != MT_Stnby){
              gChara[i].hp -= 2;
              gChara[i].point.x = gChara[i].pos.x += 4;
              gChara[i].motion = MT_Damage;
              if(gChara[i].hp < 0){
                gChara[i].life += 1;
                gChara[i].hp = 100;
                gChara[i].motion = MT_Stnby;// TODO ★★★ 変更 安村
                gChara[i].pos.x = gChara[i].point.x = WD_Width / 5 * MAP_ChipSize * (i+1);
                gChara[i].pos.y = gChara[i].point.y = 200; // TODO ★★★ 変更 安村
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


