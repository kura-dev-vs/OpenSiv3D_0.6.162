#pragma once
# include "Common.hpp"

struct RotController {
	double totalAngleDeg = 0.0;   // 回したい合計角度（正でY回転の一方向）
	double angleDeg = 0.0;        // 現在の角度（deg）
	double omegaDeg = 0.0;        // 現在の角速度（deg/s）
	double omegaMaxDeg = 720.0;    // 到達したい（頂点の）角速度（deg/s）
	double accelDeg = 360.0;      // 加速度（deg/s^2）
	double decelDeg = 360.0;      // 減速度（deg/s^2）
	bool finished = false;
	bool isMaxReached = false;

	// 初期化: 回転回数(revolutions)を与える
	// desiredOmegaMaxDeg は正の値で与えてください（内部で方向に応じる）
	void Start(double revolutions,
			   double desiredOmegaMaxDeg,
			   double accelDegPerSec2,
			   double decelDegPerSec2)
	{
		// 合計角度（正方向に回す想定。負回転にしたい場合は revolutions を負に）
		double initialAngleDeg = -70.0;
		totalAngleDeg = initialAngleDeg + revolutions * 360.0;
		angleDeg = initialAngleDeg;
		omegaDeg = 0.0;
		omegaMaxDeg = std::abs(desiredOmegaMaxDeg);
		accelDeg = std::abs(accelDegPerSec2);
		decelDeg = std::abs(decelDegPerSec2);
		finished = false;

		double absTotal = std::abs(totalAngleDeg);
		if (absTotal <= 0.0) {
			// 回転しない
			finished = true;
			return;
		}

		// 到達可能性チェック（台形プロファイルか三角プロファイルか）
		double neededToAccel = (omegaMaxDeg * omegaMaxDeg) / (2.0 * accelDeg);
		double neededToDecel = (omegaMaxDeg * omegaMaxDeg) / (2.0 * decelDeg);
		double minNeeded = neededToAccel + neededToDecel;

		if (minNeeded > absTotal) {
			// 希望のピーク速度に到達できない -> 三角プロファイルに調整
			// omega_peak = sqrt( 2 * accel * decel * total / (accel + decel) )
			double omegaPeak = std::sqrt(2.0 * accelDeg * decelDeg * absTotal / (accelDeg + decelDeg));
			omegaMaxDeg = omegaPeak;
		}

		// 保持：totalAngleDeg の符号で方向を決定（ここでは正方向のみ扱う想定だが、負回転も可）
	}

	// 毎フレーム呼ぶ。現在の角度（deg）を返す。
	double UpdateAndGetAngle() {
		if (finished) return angleDeg;

		double dt = Scene::DeltaTime(); // 秒（double）
		if (dt <= 0.0) return angleDeg;

		double absTotal = std::abs(totalAngleDeg);
		double sign = (totalAngleDeg >= 0.0) ? 1.0 : -1.0;
		double absAngle = std::abs(angleDeg);
		double rem = absTotal - absAngle;
		if (rem <= 0.0) {
			// 完了
			angleDeg = totalAngleDeg;
			omegaDeg = 0.0;
			finished = true;
			return angleDeg;
		}

		// 停止に必要な残り角度（符号無視）
		double omegaAbs = std::abs(omegaDeg);
		double stopAngle = (omegaAbs * omegaAbs) / (2.0 * decelDeg);

		if (stopAngle < rem) {
			// 加速または巡航フェーズ
			double targetOmega = omegaMaxDeg;
			// 加速（符号は後で角度に反映）
			omegaAbs += accelDeg * dt;
			// 上限は targetOmega
			if (omegaAbs > targetOmega) omegaAbs = targetOmega;
		}
		else {
			isMaxReached = true;
			// 減速フェーズ
			omegaAbs -= decelDeg * dt;
			if (omegaAbs < 0.0) omegaAbs = 0.0;
		}

		// omegaDeg に方向を付与
		omegaDeg = sign * omegaAbs;

		// 角度更新
		angleDeg += omegaDeg * dt;

		// オーバー調整チェック
		if ((sign > 0 && angleDeg > totalAngleDeg) || (sign < 0 && angleDeg < totalAngleDeg)) {
			angleDeg = totalAngleDeg;
			omegaDeg = 0.0;
			finished = true;
		}

		return angleDeg;
	}

	bool IsFinished() const { return finished; }
	bool IsMaxReached() const { return isMaxReached; }
};

// タイトルシーン
class EvoScene : public App::Scene
{
public:

	EvoScene(const InitData& init);

	void update() override;

	void draw() const override;

private:
	double currentTime = 0;
	int modelIdx = 0;
	int beforeIdx = 0;
	int afterIdx = 1;
	double groundDeg = 0;
	const Vec2 center = Scene::CenterF();
	const ColorF backgroundColor = ColorF{ 0, 0, 0 }.removeSRGBCurve();

	const Mesh groundPlane{ MeshData::Disc(2.5, 128) };
	const Texture groundTexture{ U"example/texture/22029897_.png", TextureDesc::MippedSRGB };
	// 3D をレンダリングするターゲット（深度バッファ付き）
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	// カメラ
	//BasicCamera3D camera{ Graphics3D::GetRenderTargetSize(), 40_deg, Vec3{ 0, 3, -16 } };
	BasicCamera3D camera{ Graphics3D::GetRenderTargetSize(), 20_deg, Vec3{ 0, 2, -8 } };
	FbxModelManager& fbx = FbxModelManager::Instance();
	AudioManager& audioManager = AudioManager::Instance();

	RotController rot;
	double currentAngleDeg = -70;
	bool evoDone = false;
};
