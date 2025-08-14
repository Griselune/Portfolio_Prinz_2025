#pragma once
#pragma warning(disable:4996)
#pragma once
//-----------------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-----------------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BChara : public BTask
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラクタ共通メンバ変数
	int			ID;			//キャラクタの身元
	float		mass;		//キャラクタの重さ
	ML::Vec2    pos;		//キャラクタ位置
	ML::Vec2	est;		//キャラクタ予測位置
	ML::Box2D   hitBase;	//あたり判定範囲
	ML::Vec2	sizeVec;	//大きさ
	ML::Vec2	accelVec;	//加速度
	ML::Vec2	moveVec;	//移動ベクトル
	ML::Color	color;		//レンダーの色とアルファ
	ML::Box2D	boundaries; //出られない領域の座標(画面サイズ等)
	int			moveCnt;	//行動カウンタ
	bool		IsOut;
	bool		IsColliding;
	bool		IsCorrected; //次の移動をスキップさせたい時にTRUEにする
	//向き（2D視点）
	float angle;

	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加するS事★★
	BChara()
		: ID(0)
		, mass(0.f)
		, pos(0.f, 0.f)
		, est(0.f, 0.f)
		, hitBase(0.f, 0.f, 0.f, 0.f)
		, sizeVec(0.f, 0.f)
		, accelVec(0.f, 0.f)
		, moveVec(0.f, 0.f)
		, moveCnt(0)
		, angle(0.f)
		, color(1.0f, 1.0f, 1.0f, 1.0f)
		, boundaries(0.f, 0.f, 0.f, 0.f)
		, IsOut(false)
		, IsColliding(false)
		, IsCorrected(false)
	{
	}
	virtual  ~BChara() {}

protected:
	std::vector<BChara::SP> hitObjects;

	void ScanNearbyObjects();
	void WallBounce();							//壁との反射
	void Overlap(const BChara::SP& obj);		//オブジェクト同士の重なり判定
	ML::Vec2 CalculateOverlap(const BChara::SP& obj);
	void MovementPrediction(ML::Vec2 est_, const ML::Box2D& boundaries_);
	void SquareCollider(const BChara::SP& obj);
	void ForceBoundaries();
	bool CheckBoundaries(ML::Vec2 est_);
	
public:
	void UpdateHitBase();
};