/*****************************************************************
  ファイル名	: common.h
  機能		: サーバーとクライアントで使用する定数の宣言を行う
 *****************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<assert.h>
#include<math.h>

#include <SDL/SDL.h> //i
#include <opencv/cv.h> //i

#define PORT		(u_short)8888		/* ポート番号 */

#define MAX_CLIENTS	4			/* クライアント数の最大値 */
#define MAX_NAME_SIZE	10 			/* ユーザー名の最大値*/

#define MAX_DATA	500			/* 送受信するデータの最大値 */
#define END_SCROLL	400			/* ゲームを終了してランキングに移るまでのスクロール数　hosihosihosi riho */
#define CAMERA_PER 100
#define END_COMMAND	'E'		  	/* プログラム終了コマンド */
#define HAND_COMMAND    'H'             /* ハンドコマンド */
#define FRAME_COMMAND   'F'				/* フレーム送信完了コマンド */
#define SEND_COMMAND	'S'				/* 入力データ送信コマンド */
#define RESULT_COMMAND 'R'
#define CAMERA_COMMAND 'M'        /*カメラのデータ送信コマンド*/

#define TITLE_COMMAND   'T'		/* タイトル送信コマンド */
#define BACKTITLE_COMMAND 'B'   /* タイトルに戻るコマンド */
#define CHARASEL_COMMAND 'C'	/* キャラ選択コマンド */

#define CDATASET_COMMAND 'D'/*カメラのデータセット関数*/ 
/* マップサイズ */
enum {
  MAP_Width    = 120,
  MAP_Height   = 16,
  WD_Width     = 31,
  WD_Height    = 16,
  MAP_ChipSize = 48
};

/* wiiの情報 */
typedef struct {
  int ac;
  char *av[];
} WiiInfo;

/* マップの種類 */
typedef enum {
  MT_Wall      = 0,           /* 壁 */
  MT_Water     = 1,           /* 水 */
  MT_Grass     = 2,           /* 草 */
  MT_Sand      = 3            /* 砂 */
} MapType;

/* キャラクターの種類 */
typedef enum {
  CT_None   = -1,              /* なし */
  CT_Naomi  = 0,               /* 直美 */
  CT_Osumo  = 1,               /* オスモさん */
  CT_Ponzu  = 2,               /* ポン酢 */
  CT_Ninja  = 3,               /* 忍者 */
  CT_NUM    = 4                /* キャラの数 */
} CharaType;

/* キャラクターの状態 */
typedef enum {
  CS_Disable    = 0,           /* 非表示 */
  CS_Enable   = 1,           /* 表示 */
  CS_NUM       = 2            /* 状態の個数 */
} CharaStts;

/* アイテムの状態 　追加　松本*/
typedef enum {
  IS_Disable    = 0,           /* 非表示 */
  IS_Enable   = 1,           /* 表示 */
  IS_NUM       = 2            /* 状態の個数 */
} ItemStts;

/* キャラクターの向き */
typedef enum {
  DR_None     = -1,           /* なし */
  DR_Right    = 0,            /* 右 */
  DR_Left     = 1,            /* 左 */
  DR_Up       = 2,            /* 上 */
  DR_Down     = 3             /* 下 */
} CharaDir;

/* モーションの種類 */
typedef enum {
  MT_Stand  = 0,               /* 基本 */
  MT_Attack = 1,               /* 攻撃 */
  MT_Fall   = 2,               /* 落ちる */
  MT_Jump   = 3,               /* ジャンプ */
  MT_Stnby  = 4,
  MT_Damage = 5,         			 /* ダメージ */
  MT_Damage_l = 6,         		 /* ダメージ左 */
  MT_Muteki = 7,         			 /* 無敵 */
  MT_Muteki_l = 8,         		 /* 無敵左 */
  MT_Sit    = 9,               /* しゃがみ（これいる？） */
  MT_NUM    = 10               /* モーションの数 */
} MotionType;

/* アイテムの種類 追加　松本*/
typedef enum {
  IT_None   = -1,              /* なし */
  IT_Prote  = 0,               /* プロテイン */
  IT_Wing   = 1,               /* 羽 */
  IT_NUM    = 2                /* アイテムの数 */
} ItemType;
/* アイテムモーションの種類 追加　松本*/

typedef enum {
  IM_Stand  = 0,               /* 基本 */
  IM_Fall   = 1,               /* 落ちる */
  IM_Stnby	= 2,
  IM_NUM    = 3
} MotionItemType;

/* 攻撃の種類 */
typedef enum {
  AT_None   = -1,              /* なし */
  AT_Naomi  = 0,               /* 固有の技 */
  AT_Osumo  = 1,               /* 固有の技 */
  AT_Ponzu  = 2,               /* 固有の技 */
  AT_Ninja  = 3,               /* 固有の技 */
  AT_Punch  = 4,               /* パンチ */
  AT_Shake  = 5,               /* 振る技 */
  AT_NUM    = 6                /* 攻撃の数 */
} AttackType;

/* キャラクターの情報 */
typedef struct {
  int    id;    /* プレイヤーのID */
  CharaType    type;    /* キャラの種類 */
  CharaStts    stts;    /* キャラの状態 */
  CharaDir     dir;           /* キャラの向き */
  SDL_Rect     pos;           /* 位置(ゲーム画面上の座標) */
  CvPoint2D32f point;         /* 計算上の座標 */
  CvPoint      base;          /* 移動方向，基準位置 */
  int          hp;            /* 体力 */
  int maxhp;
  int          life;          /* 残機 */
  float        t;             /* ジャンプ時間 */
  float        tAttack;             /* 攻撃時間 */
  int rank  ;    
  MotionType   motion;        /* モーション */
  ItemType     item;          /* 持っているアイテム */
  AttackType   attack;        /* 攻撃の種類 */
  SDL_Rect     posAttack;     /* 攻撃エフェクトの座標 */
  int          anipatnum;     /* パターン数 */
  float    speed;     /* キャラのスピード */
  int    power;     /* キャラのパワー */
  int			 flagJumpSE;	/* ジャンプ効果音のタイミング */ 
  AttackType	finisher;		/* 必殺技  ★★★　追加　安村*/
  // TODO 仮
  int   fd; 
  char  name[MAX_NAME_SIZE];
  int     anipat;        /* アニメーションパターン */
  //乾 追加
  int					 throwAttack;
} CharaInfo;
// ability of character 
typedef struct {
  int speed ;
  int power ;
} CharaAbility;

typedef struct {
  int red ;
  int green ; 
  int blue ;
} CameraData;

/* 画像用 */
typedef enum {
  /* キャラ */
  CI_Naomi  = 0,               /* 直美 */
  CI_Osumo  = 1,               /* オスモさん */
  CI_Ponzu  = 2,               /* ポン酢 */
  CI_Ninja  = 3,               /* 忍者 */
  CI_NaomiL  = 4,               /* 直美 2は左向き */
  CI_OsumoL  = 5,               /* オスモさん */
  CI_PonzuL  = 6,               /* ポン酢 */
  CI_NinjaL  = 7,               /* 忍者 */

  /* アイテム（未実装） */
  //    II_Prote  = 11,               /* プロテイン */
  //    II_Wing   = 12,               /* 羽 */
  CI_Mask = 8,
  CI_NUM    = 9 ,             /* 画像の数 */
  CI_Dir = 4                /*  この値を足せば左右方向が切り替わる   */
} CharaImage;

/* 攻撃画像用 */ //atkoko
typedef enum {
  /* 攻撃 */
  AI_Naomi  = 0,               /* 直美 */
  AI_Osumo  = 1,               /* オスモさん */
  AI_Ponzu  = 2,               /* ポン酢 */
  AI_Ninja  = 3,               /* 忍者 */
  AI_Punch  = 4,               /* パンチ */
  AI_Shake  = 5,               /* 振る技 */
  AI_Naomi2 = 6,               /* 直美 */
  AI_Osumo2 = 7,               /* オスモさん */
  AI_Ponzu2 = 8,               /* ポン酢 */
  AI_Ninja2 = 9,               /* 忍者 */
  AI_Punch2 = 10,               /* パンチ */
  AI_Shake2 = 11,               /* 振る技 */
  AI_NUM    = 12,                /* 画像の数 */
  AI_Dir    = 6                /*  この値を足せば左右方向が切り替わる   */
} AttackImage;

/* 背景の種類 */
typedef enum {
  BK_Title		= 0,             /* タイトル */
  BK_Title_Wait	= 1,
  BK_Chara_Sel	= 2,             /* キャラ選択 */
  BK_Chara_Sel_Wait = 3,
  BK_Field		= 4,             /* フィールド */
  BK_Loading   = 5,
  BK_Result			= 6,             /* 終了画面 */
  //add19
  BK_Cam			= 7,             /* 終了画面 */
  BK_NUM			= 8              /* 背景の数 */
} BackType;

// BGMタイプ
typedef enum { 
  BT_Title  = 0,
  BT_Game   = 1,
  BT_Result = 2,
  BT_NUM    = 3
} BgmType;

// 効果音タイプ(操作など)
typedef enum {
  ST_Select   = 0,
  ST_Decide   = 1,
  ST_NUM      = 2
} SEType;

// 効果音タイプ(なおみ)
typedef enum {
  NAST_AttackS   = 0,
  NAST_AttackV   = 1,
  NAST_DamageS   = 2,
  NAST_DamageV   = 3,
  NAST_JumpS     = 4,
  NAST_JumpV     = 5,
  NAST_DeathS    = 6,
  NAST_DeathV    = 7,
  NAST_NUM       = 8
} NASEType;

// 効果音タイプ(おすも)
typedef enum {
  OST_AttackS   = 0,
  OST_AttackV   = 1,
  OST_damageS   = 2,
  OST_damageV   = 3,
  OST_JumpS     = 4,
  OST_JumpV     = 5,
  OST_DeathS    = 6,
  OST_DeathV    = 7,
  OST_NUM       = 8
} OSEType;

// 効果音タイプ(ぽんず)
typedef enum {
  PST_AttackS   = 0,
  PST_AttackV   = 1,
  PST_damageS   = 2,
  PST_damageV   = 3,
  PST_JumpS     = 4,
  PST_JumpV     = 5,
  PST_DeathS    = 6,
  PST_DeathV    = 7,
  PST_NUM       = 8
} PSEType;

// 効果音タイプ(にんじゃ)
typedef enum {
  NIST_AttackS   = 0,
  NIST_AttackV   = 1,
  NIST_damageS   = 2,
  NIST_damageV   = 3,
  NIST_JumpS     = 4,
  NIST_JumpV     = 5,
  NIST_DeathS    = 6,
  NIST_DeathV    = 7,
  NIST_NUM       = 8
} NISEType;



/* フィールドの情報 */
typedef struct {
  int    	 map;		/* 表示中のマップ */
  BackType	 back;		/* 表示中の背景 */
  int          scroll;        /* スクロール用 */
  ItemStts     stts1;
  ItemStts     stts2;
  ItemType     item1;          /* アイテムの種類 */
  ItemType	   item2;
  SDL_Rect     posItem1;       /* アイテムが降る座標 */
  SDL_Rect     posItem2; 
  CvPoint2D32f pointItem1;         /* 計算上の座標 */
  CvPoint2D32f pointItem2;         /* 計算上の座標 */
  MotionItemType motionItem1;
  MotionItemType motionItem2;
  float t1;
  float t2;
} FieldInfo;

/* 入力されたボタンの情報 */
typedef struct {
  CharaDir dir1; // 方向1
  CharaDir dir2; // 方向2
  AttackType button1; // ボタン1(攻撃)
  MotionType button2; // ボタン2(ジャンプ)
  AttackType shake; // 振る(必殺技)

  // TODO 仮
  int hand;

} InputInfo;

#endif

