//-----------------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-----------------------------------------------------------------------------
#include  "BChara.h"
#include  "MyPG.h"
#include  "randomLib.h"
#include  "Task_Game.h"

/// <summary>
/// 近傍オブジェクトのリストを作成する。
/// クワッドツリー使用時は、同一領域内に存在するオブジェクトのみを取得し、
/// 非使用時は全オブジェクトを取得する。
/// 結果は hitObjects に格納され、後続の当たり判定処理で使用される。
/// </summary>
void BChara::ScanNearbyObjects()
{
	auto game = ge->GetTask<Game::Object>("MainGame", "NoName");
	// クワッドツリー使用時（同一領域のオブジェクトのみ）
	if (true == game->IsQuadtreeOn) {					
		game->quadTree->Query(this->hitBase, hitObjects);
	}
	// クワッドツリー未使用時（全オブジェクト）
	else												
	{
		auto others = ge->GetTasks<BChara>("Square");
		for (auto& obj : *others) {
			if (obj.get() != this) {
				this->hitObjects.push_back(obj);
			}
		}
	}
}

/// <summary>
/// 移動処理
/// </summary>
/// <param name="est_"></param>
/// <param name="boundaries_"></param>
void BChara::MovementPrediction(ML::Vec2 est_, const ML::Box2D& boundaries_)
{
	ScanNearbyObjects();

	ML::Vec2 tempPos;
	//横軸に対する移動
	while (est_.x != 0.f) {
		tempPos.x = this->pos.x;
		if (est_.x >= 1.f) { this->pos.x += 1.f;		est_.x -= 1.f; }
		else if (est_.x <= -1.f) { this->pos.x -= 1.f;		est_.x += 1.f; }
		else { this->pos.x += est_.x;		est_.x = 0.f; }
		UpdateHitBase();
		if (CheckBoundaries(this->pos + est_)) {
			this->pos.x = tempPos.x;		//移動をキャンセル
			WallBounce();
			ForceBoundaries();
			break;
		}
	}

	//縦軸に対する移動
	while (est_.y != 0.f) {
		tempPos.y = this->pos.y;
		if (est_.y >= 1.f) { this->pos.y += 1.f;		est_.y -= 1.f; }
		else if (est_.y <= -1.f) { this->pos.y -= 1.f;		est_.y += 1.f; }
		else { this->pos.y += est_.y;		est_.y = 0.f; }
		UpdateHitBase();
		if (CheckBoundaries(this->pos + est_)) {
			this->IsOut = true;
			this->pos.y = tempPos.y;		//移動をキャンセル
			WallBounce();
			ForceBoundaries();
			break;
		}
	}

	//衝突処理
	for (auto& obj : hitObjects) {
		if (obj.get() != this && this->hitBase.Hit(obj->hitBase)) {
			if (this->ID > obj->ID) {
				SquareCollider(obj);
				break;
			}
		}
	}

	UpdateHitBase();
	this->hitObjects.clear();

	return;
}


/// <summary>
/// 壁に跳ね返る処理
/// </summary>
void BChara::WallBounce() 
{
	if (this->IsOut) return;

	if (this->pos.x + this->moveVec.x <= this->boundaries.x) {
		this->moveVec.x = -1.f * this->moveVec.x;
	}
	else if (this->pos.x + this->moveVec.x >= this->boundaries.w) {
		this->moveVec.x = -1.f * this->moveVec.x;
	}

	if (this->pos.y + this->moveVec.y <= this->boundaries.y) {
		this->moveVec.y = -1.f * this->moveVec.y;
	}
	else if (this->pos.y + this->moveVec.y >= this->boundaries.h) {
		this->moveVec.y = -1.f * this->moveVec.y;
	}
	this->pos += this->moveVec;
	this->IsCorrected = true;  //次の移動をスキップする。そうしないと同じフレーム内で2回移動してしまう

	return;
}

/// <summary>
/// 二次元の矩形当たり判定（弾性衝突）を計算し、速度ベクトルを更新する。
/// 衝突後はオブジェクトの重なりを解消する。
/// 数式は 1次元弾性衝突の式をベクトル化したもの:
/// v1' = v1 + (2*m2 / (m1 + m2)) * ((v2 - v1)・n) * n
/// v2' = v2 + (2*m1 / (m1 + m2)) * ((v1 - v2)・n) * n
/// （n は正規化された衝突方向ベクトル）
/// </summary>
/// <param name="obj"></param>
void BChara::SquareCollider(const BChara::SP& obj)
{
	// デバッグ用カラー設定（this: 赤, obj: 黄）
	this->color = ML::Color(1.0f, 1.0f, 0.0f, 0.0f);
	obj->color = ML::Color(1.0f, 1.0f, 1.0f, 0.0f);

	// 位置差ベクトルとその長さ（衝突方向）
	ML::Vec2 impactVec = obj->pos - this->pos;
	float impactMagnitude = impactVec.Length();

	// 質量と速度差の計算
	float massSum = this->mass + obj->mass;
	ML::Vec2 velocityDiff = obj->moveVec - this->moveVec;
	
	// 内積（速度差と衝突方向）を求め、衝突式の分子分母を計算
	float num = velocityDiff.x * impactVec.x + velocityDiff.y * impactVec.y;
	float den = massSum * impactMagnitude * impactMagnitude;

	// this 側の速度ベクトル更新（外部に出ていない場合のみ）
	if (this->IsOut == false) {
		ML::Vec2 deltaVthis = impactVec;
		deltaVthis *= (2 * obj->mass * num / den);
		this->moveVec += deltaVthis;
	}

	// obj 側の速度ベクトル更新（外部に出ていない場合のみ）
	if (obj->IsOut == false) {
		ML::Vec2 deltaVobj = -impactVec;
		deltaVobj *= (2 * this->mass * num / den);
		obj->moveVec += deltaVobj;
	}

	// 衝突後の重なり解消処理
	Overlap(obj);

	return;
}

/// <summary>
/// 強制的にオブジェクトを画面内に戻す処理。
/// 境界外に出る移動を検知した場合、座標を補正しフラグを立てる。
/// EPSILON は浮動小数点誤差や重なり防止用の微小値。
/// </summary>
void BChara::ForceBoundaries()
{
	const float EPSILON = 0.02f;  // 誤差回避・境界から少し内側に押し戻す値

	// 左端判定
	if (this->pos.x + this->moveVec.x <= this->boundaries.x) {
		this->pos.x = this->boundaries.x - this->moveVec.x + EPSILON;						
		this->IsOut = true;
	}
	// 右端判定
	else if (this->pos.x + this->moveVec.x >= this->boundaries.x + this->boundaries.w) { 
		this->pos.x = this->boundaries.w + this->boundaries.x - this->moveVec.x - EPSILON;	
		this->IsOut = true; }
	// 上端判定
	else if (this->pos.y + this->moveVec.y <= this->boundaries.y) { 
		this->pos.y = this->boundaries.y - this->moveVec.y + EPSILON;
		this->IsOut = true; }
	// 下端判定
	else if (this->pos.y + this->moveVec.y >= this->boundaries.y + this->boundaries.h) {
		this->pos.y = this->boundaries.h + this->boundaries.y - this->moveVec.y - EPSILON;
		this->IsOut = true;
	}

	this->UpdateHitBase();

	// 次の移動をスキップする。そうしないと同じフレーム内で2回移動してしまう
	this->IsCorrected = true; 

	return;
}

/// <summary>
/// 壁との当たり判定
/// </summary>
/// <param name="nextPos_"></param>
/// <returns></returns>
bool BChara::CheckBoundaries(ML::Vec2 nextPos_)
{
	if (nextPos_.x <= this->boundaries.x)						return true;
	if (nextPos_.x >= this->boundaries.x + this->boundaries.w)	return true;
	if (nextPos_.y <= this->boundaries.y)						return true;
	if (nextPos_.y >= this->boundaries.y + this->boundaries.h)	return true;

	else {
		this->IsOut = false;
		return false;
	}
}

void BChara::UpdateHitBase()
{
	this->hitBase.x = this->pos.x - this->sizeVec.x / 2.f;
	this->hitBase.y = this->pos.y - this->sizeVec.y / 2.f;
}


/// <summary>
/// 衝突後のオブジェクト同士の重なり（オーバーラップ）を解消する処理。
/// 各オブジェクトを等距離だけ反対方向へ押し戻し、衝突によるめり込みを防ぐ。
/// EPSILON は浮動小数点誤差や境界からの微小な余裕を確保するために使用。
/// </summary>
/// <param name="obj">重なり解消対象の相手オブジェクト</param>
void BChara::Overlap(const BChara::SP& obj) 
{
	this->UpdateHitBase();
	obj->UpdateHitBase();
	ML::Vec2 overlap = CalculateOverlap(obj);
	float EPSILON = 0.02f;	// 誤差回避用の微小値

	// X方向優先の押し戻し
	if (this->pos.x > obj->pos.x) {
		this->pos.x += (overlap.x * 0.5f) + EPSILON;
		obj->pos.x -= (overlap.x * 0.5f) - EPSILON;
	}

	// Y方向優先の押し戻し
	else if (this->pos.y > obj->pos.y) {
		this->pos.y += (overlap.y * 0.5f) + EPSILON;
		obj->pos.y -= (overlap.y * 0.5f) - EPSILON;
	}

	// 両方向の押し戻し（斜め衝突など）
	else {
		this->pos -= (overlap * 0.5f) - ML::Vec2{ EPSILON ,EPSILON };
		obj->pos += (overlap * 0.5f) + ML::Vec2{ EPSILON ,EPSILON };
		this->UpdateHitBase();
		obj->UpdateHitBase();
	}

	return;
}

/// <summary>
/// 2つの矩形ヒットボックス間の重なり量（オーバーラップ量）を計算する。
/// 返されるベクトルは、押し戻し処理に使用される軸方向の補正値。
/// 衝突していない場合は {0,0} を返す。
/// </summary>
/// <param name="obj">比較対象のオブジェクト</param>
/// <returns>
/// /// X方向またはY方向の重なり量（より浅い軸を優先）。
/// 両方同じ場合は両方向の重なりを返す（完全重なり防止用）。
/// </returns>
ML::Vec2 BChara::CalculateOverlap(const BChara::SP& obj)
{
	// 衝突していない場合は {0,0}
	if (this->hitBase.Hit(obj->hitBase) == false) return { 0.f, 0.f };

	// 自オブジェクトと相手オブジェクトの右下隅の座標を算出
	float thisCornerx = this->hitBase.x + this->hitBase.w;
	float thisCornery = this->hitBase.y + this->hitBase.h;
	float objCornerx = obj->hitBase.x + obj->hitBase.w;
	float objCornery = obj->hitBase.y + obj->hitBase.h;


	// 各軸方向の重なり量を計算（正の値であれば衝突）
	float px = 0.f;
	float py = 0.f;

	// X方向の重なり量
	if (this->pos.x > obj->pos.x) {
		px = objCornerx - this->hitBase.x;	
	}
	else if (this->pos.x <= obj->pos.x) {
		px = thisCornerx - obj->hitBase.x;
	}

	// Y方向の重なり量
	if (this->pos.y > obj->pos.y) {
		py = objCornery - this->hitBase.y;
	}
	else if (this->pos.y <= obj->pos.y) {
		py = thisCornery - obj->hitBase.y;
	}

	// 浅い軸で押し出す（XかYのどちらか）
	if (px < py)			return { px, 0.f };
	else if (px > py)       return { 0.f, py };
	else					return { px, py }; // 完全重なり対策

	// 万が一衝突していない場合
	return { 0.f, 0.f };
}

