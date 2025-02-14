#pragma once

#define NOMINMAX
//共通
#include <array>
#include <list>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <array>
#include <vector>
#include <D3D11.h>
#include <memory>
//DXLIB
#include "DxLib.h"
//追加
#include"DXLib_mat.hpp"
#include "SoundHandle.hpp"
#include "GraphHandle.hpp"
#include "FontHandle.hpp"
//VR
#define BUTTON_TRIGGER vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger)
#define BUTTON_SIDE vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip)
#define BUTTON_TOUCHPAD vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad)
#define BUTTON_TOPBUTTON vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu)
#define DEVICE_HMD vr::TrackedDeviceClass_HMD
#define DEVICE_CONTROLLER vr::TrackedDeviceClass_Controller
#define DEVICE_TRACKER vr::TrackedDeviceClass_GenericTracker
#define DEVICE_BASESTATION vr::TrackedDeviceClass_TrackingReference
//
using std::int8_t;
using std::size_t;
using std::uint16_t;
using std::uint8_t;

constexpr float M_GR = -9.8f;				  /*重力加速度*/
//デスクトップサイズ
inline const int deskx = (GetSystemMetrics(SM_CXSCREEN)); 
inline const int desky = (GetSystemMetrics(SM_CYSCREEN)); 
//マウス判定
#define in2_(mx, my, x1, y1, x2, y2) (mx >= x1 && mx <= x2 && my >= y1 && my <= y2)
#define in2_mouse(x1, y1, x2, y2) (in2_(mousex, mousey, x1, y1, x2, y2))
//その他
 //角度からラジアンに
template <typename T>
static float deg2rad(T p1) {
	return float(p1) * DX_PI_F / 180.f;
}
//ラジアンから角度に
template <typename T>
static float rad2deg(T p1) {
	return float(p1) * 180.f / DX_PI_F;
}
//文字列から数値を取り出す
class getparams {
	static std::string getright(const char* p1) {
		std::string tempname = p1;
		return tempname.substr(tempname.find('=') + 1);
	}
public:
	static const char* _char(int p1) {
		char mstr[64];
		FileRead_gets(mstr, 64, p1);
		return getright(mstr).c_str();
	}
	static auto _str(int p1) {
		char mstr[64];
		FileRead_gets(mstr, 64, p1);
		return getright(mstr);
	}
	static auto get_str(int p1) {
		char mstr[64];
		FileRead_gets(mstr, 64, p1);
		return std::string(mstr);
	}
	static const long int _int(int p1) {
		char mstr[64];
		FileRead_gets(mstr, 64, p1);
		return std::stoi(getright(mstr));
	}
	static const long int _long(int p1) {
		char mstr[64];
		FileRead_gets(mstr, 64, p1);
		return std::stol(getright(mstr));
	}
	static const unsigned long int _ulong(int p2) {
		char mstr[64];
		FileRead_gets(mstr, 64, p2);
		return std::stoul(getright(mstr));
	}
	static const float _float(int p1) {
		char mstr[64];
		FileRead_gets(mstr, 64, p1);
		return std::stof(getright(mstr));
	}
	static const bool _bool(int p1) {
		char mstr[64];
		FileRead_gets(mstr, 64, p1);
		return (getright(mstr).find("true") != std::string::npos);
	}
};
//イージング
void easing_set(float* first, const float& aim, const float& ratio, const float& fps) {
	if (ratio == 0.f) {
		*first = aim;
	}
	else {
		if (aim != 0.f) {
			*first += (aim - *first) * (1.f - powf(ratio, 60.f / fps));
		}
		else {
			*first *= powf(ratio, 60.f / fps);
		}
	}
};
void easing_set(VECTOR_ref* first, const VECTOR_ref& aim, const float& ratio, const float& fps) {
	if (ratio == 0.f) {
		*first = aim;
	}
	else {
		*first += (VECTOR_ref(aim) - *first) * (1.f - powf(ratio, 60.f / fps));
	}
};
//ID割り当て
template <class T>
void fill_id(std::vector<T>& vect) {
	for (int i = 0; i < vect.size(); i++) {
		vect[i].id = i;
	}
}
template <class T, size_t N>
void fill_id(std::array<T, N>& vect) {
	for (int i = 0; i < vect.size(); i++) {
		vect[i].id = i;
	}
}
//cosAを出す
float getcos_tri(const float& a, const float& b, const float& c) {
	if (b + c > a && c + a > b && a + b > c) {
		return std::clamp((b * b + c * c - a * a) / (2.f * b*c), -1.f, 1.f);
	}
	return 1.f;
}
//起動
void createProcess(char* szCmd, DWORD flag, bool fWait) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	CreateProcess(NULL, szCmd, NULL, NULL, FALSE, flag, NULL, NULL, &si, &pi);
	if (fWait) WaitForSingleObject(pi.hProcess, INFINITE);	//終了を待つ.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
//自信を多重起動
void start_me(void) {
	char Path[MAX_PATH];
	// EXEのあるフォルダのパスを取得
	::GetModuleFileName(NULL, Path, MAX_PATH);
	createProcess(Path, SW_HIDE, false);
}
//
class DXDraw {
public:
	int disp_x = 1920;
	int disp_y = 1080;
	int out_disp_x = 1920;
	int out_disp_y = 1080;

	struct cam_info {
		VECTOR_ref campos, camvec, camup;	//カメラ
		float fov = deg2rad(90);			//カメラ
		float near_ = 0.1f, far_ = 10.f;	//ニアファー

		void set_cam_info(const VECTOR_ref& cam_pos, const VECTOR_ref& cam_vec, const VECTOR_ref& cam_up, const float& cam_fov_, const float& cam_near_, const float& cam_far_) {
			campos=cam_pos;
			camvec=cam_vec;
			camup=cam_up;	//カメラ
			fov = cam_fov_;			//カメラ
			near_ = cam_near_, far_ = cam_far_;	//ニアファー
		}
	};
private:
	bool use_shadow = true;			/*影描画*/
	int shadow_near = 0;			/*近影*/
	int shadow_far = 0;				/*遠影*/
	size_t shadow_size = 10;		/*影サイズ*/
	bool use_pixellighting = true;			     /**/
	bool use_vsync = false;				     /*垂直同期*/
	float frate = 60.f;				     /*フレームレート*/
	//鏡
	struct Mirror_mod {
		VECTOR_ref WorldPos[4];	// 鏡のワールド座標
		COLOR_F AmbientColor;	// 鏡の Ambient Color
		COLOR_U8 DiffuseColor;	// 鏡の Diffuse Color
		int BlendParam[2];		// 鏡のブレンドモードとパラメータ
		GraphHandle Handle;		// 鏡に映る映像の取得に使用するスクリーン
		FLOAT4 ScreenPosW[4];	// 鏡に映る映像の取得に使用するクリーンの中の鏡の四隅の座標( 同次座標 )
		bool canlook = false;	//鏡が見えているか否か
	};
	std::vector<Mirror_mod> Mirror_obj;
	int MIRROR_POINTNUM = 64;	// 鏡の描画に使用する面の頂点分割数
	int MIRROR_NUM = 2;			// 鏡の数
	std::vector <VERTEX3D> Vert;
	std::vector <unsigned short> Index;
	MATERIALPARAM Material;
	VECTOR_ref HUnitPos;
	VECTOR_ref VUnitPos[2];
	VECTOR_ref HPos;
	VECTOR_ref VPos[2];
	FLOAT4 HUnitUV;
	FLOAT4 VUnitUV[2];
	FLOAT4 HUV;
	FLOAT4 VUV[2];
	VECTOR_ref MirrorNormal;
	COLOR_U8 DiffuseColor;
	COLOR_U8 SpecularColor;
	int TextureW, TextureH;
	std::array<GraphHandle, 3> outScreen;	//スクリーンバッファ
public:
	std::vector<char> tracker_num;

	DXDraw(const char* title, const int& xsize, const int& ysize, const float& fps = 60.f) {

		this->use_shadow = true;
		this->shadow_size = 13;
		{
			this->disp_x = xsize;
			this->disp_y = ysize;
			this->out_disp_x = this->disp_x;
			this->out_disp_y = this->disp_y;
		}

		this->frate = fps;
		SetOutApplicationLogValidFlag(false ? TRUE : FALSE);				/*log*/
		SetMainWindowText(title);											/*タイトル*/
		ChangeWindowMode(TRUE);												/*窓表示*/
		SetUseDirect3DVersion(DX_DIRECT3D_11);								/*directX ver*/
		SetGraphMode(this->disp_x, this->disp_y, 32);						/*解像度*/
		SetUseDirectInputFlag(TRUE);										/**/
		SetDirectInputMouseMode(TRUE);										/**/
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);						/*ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする*/
		SetUsePixelLighting(use_pixellighting ? TRUE : FALSE);				/*ピクセルシェーダの使用*/
		SetFullSceneAntiAliasingMode(4, 2);									/*アンチエイリアス*/
		SetEnableXAudioFlag(TRUE);											/**/
		Set3DSoundOneMetre(1.0f);											/**/
		SetWaitVSyncFlag(use_vsync ? TRUE : FALSE);							/*垂直同期*/
		DxLib_Init();														/**/
		SetAlwaysRunFlag(TRUE);												/*background*/
		SetUseZBuffer3D(TRUE);												/*zbufuse*/
		SetWriteZBuffer3D(TRUE);											/*zbufwrite*/
		MV1SetLoadModelPhysicsWorldGravity(-9.8f);							/*重力*/
		SetWindowSize(this->out_disp_x, this->out_disp_y);
		SetWindowPosition(
			//deskx + 
			(deskx - this->out_disp_x) / 2 - 8, (desky - this->out_disp_y) / 2 - 32);
		outScreen[0] = GraphHandle::Make(this->disp_x, this->disp_y);	/*左目*/
		outScreen[1] = GraphHandle::Make(this->disp_x, this->disp_y);	/*右目*/
		outScreen[2] = GraphHandle::Make(this->disp_x, this->disp_y);	/*TPS用*/
	}
	~DXDraw(void) {
		DxLib_End();
	}
	template <typename T>
	bool Set_Light_Shadow(const VECTOR_ref& farsize, const VECTOR_ref& nearsize, const VECTOR_ref& Light_dir, T doing) {
		SetGlobalAmbientLight(GetColorF(0.12f, 0.11f, 0.10f, 0.0f));
		SetLightDirection(Light_dir.get());
		if (this->use_shadow) {
			shadow_near = MakeShadowMap(int(pow(2, this->shadow_size)), int(pow(2, this->shadow_size)));
			shadow_far = MakeShadowMap(int(pow(2, this->shadow_size)), int(pow(2, this->shadow_size)));
			SetShadowMapAdjustDepth(shadow_near, 0.0005f);
			SetShadowMapLightDirection(shadow_near, Light_dir.get());
			SetShadowMapLightDirection(shadow_far, Light_dir.get());
			SetShadowMapDrawArea(shadow_far, nearsize.get(), farsize.get());
			ShadowMap_DrawSetup(shadow_far);
			doing();
			ShadowMap_DrawEnd();
		}
		return true;
	}
	bool Delete_Shadow() {
		if (this->use_shadow) {
			DeleteShadowMap(shadow_near);
			DeleteShadowMap(shadow_far);
		}
		return true;
	}
	template <typename T>
	bool Ready_Shadow(const VECTOR_ref& pos, T doing, const VECTOR_ref& nearsize) {
		if (this->use_shadow) {
			SetShadowMapDrawArea(shadow_near, (nearsize*(-1.f) + pos).get(), (VECTOR_ref(nearsize) + pos).get());
			ShadowMap_DrawSetup(shadow_near);
			doing();
			ShadowMap_DrawEnd();
			return true;
		}
		return false;
	}
	template <typename T>
	bool Draw_by_Shadow(T doing) {
		if (this->use_shadow) {
			SetUseShadowMap(0, shadow_near);
			SetUseShadowMap(1, shadow_far);
		}
		doing();
		if (this->use_shadow) {
			SetUseShadowMap(0, -1);
			SetUseShadowMap(1, -1);
		}
		return true;
	}
	bool Screen_Flip(const LONGLONG& waits) {
		ScreenFlip();
		if (!use_vsync) {
			while (GetNowHiPerformanceCount() - waits < 1000000.0f / frate) {}
		}
		return true;
	}
	static bool Capsule3D(const VECTOR_ref& p1, const VECTOR_ref& p2, const float& range, const unsigned int& color, const unsigned int& speccolor) {
		return DxLib::DrawCapsule3D(p1.get(), p2.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	static bool Line2D(const int& p1x, const int& p1y, const int& p2x, const int& p2y, const unsigned int& color, const int& thickness=1) {
		return DxLib::DrawLine(p1x,p1y,p2x,p2y,color,thickness) == TRUE;
	}
	//鏡
	VECTOR_ref Mirrorcampos, Mirrorcamtgt;
	auto& get_Mirror_obj() {
		return Mirror_obj;
	}
	auto& get_Mirror_obj(int i) {
		return Mirror_obj[std::min(i, int(Mirror_obj.size() - 1))];
	}
	// 鏡の初期化
	void Mirror_init(int p) {
		MIRROR_NUM = p;
		Mirror_obj.clear();
		for (int i = 0; i < MIRROR_NUM; i++) {
			Mirror_obj.resize(Mirror_obj.size() + 1);
			Mirror_obj.back().Handle = GraphHandle::Make(this->disp_x, this->disp_y, FALSE);	// 鏡に映る映像の取得に使用するスクリーンの作成
		}
		Vert.resize(MIRROR_POINTNUM * MIRROR_POINTNUM);
		Index.resize((MIRROR_POINTNUM - 1) * (MIRROR_POINTNUM - 1) * 6);
	}
	void set_Mirror_obj(int i,
		VECTOR_ref pos1, VECTOR_ref pos2, VECTOR_ref pos3, VECTOR_ref pos4,
		COLOR_F ambcol,
		COLOR_U8 difcol,
		int param0,int param1
		) {
		auto& m = Mirror_obj[std::min(i, int(Mirror_obj.size() - 1))];
		m.WorldPos[0] = pos1;
		m.WorldPos[1] = pos2;
		m.WorldPos[2] = pos3;
		m.WorldPos[3] = pos4;
		m.AmbientColor = ambcol;
		m.DiffuseColor = difcol;
		m.BlendParam[0] = param0;
		m.BlendParam[1] = param1;
	}
	void Mirror_delete() {
		for (int i = 0; i < MIRROR_NUM; i++) {
			Mirror_obj[i].Handle.Dispose();
		}
		Mirror_obj.clear();
		Vert.clear();
		Index.clear();
	}
	// 鏡に映る映像を描画するためのカメラの設定を行う.Mirrorcampos,Mirrorcamtgtに反映
	void Mirror_SetupCamera(Mirror_mod& MirrorNo, const VECTOR_ref& campos, const VECTOR_ref& camtgt, const VECTOR_ref& camup, const float& fov, const float& near_distance = 100.f, const float& far_distance = 1000.f) {
		auto& id = MirrorNo;
		// 鏡の面の法線を算出
		MirrorNormal = ((id.WorldPos[1] - id.WorldPos[0]).cross(id.WorldPos[2] - id.WorldPos[0])).Norm();
		// 鏡の面からカメラの座標までの最短距離、鏡の面からカメラの注視点までの最短距離を算出
		float EyeLength = Plane_Point_MinLength(id.WorldPos[0].get(), MirrorNormal.get(), campos.get());
		float TargetLength = Plane_Point_MinLength(id.WorldPos[0].get(), MirrorNormal.get(), camtgt.get());
		// 鏡に映る映像を描画する際に使用するカメラの座標とカメラの注視点を算出
		Mirrorcampos = VECTOR_ref(campos) + MirrorNormal * (-EyeLength * 2.0f);
		Mirrorcamtgt = VECTOR_ref(camtgt) + MirrorNormal * (-TargetLength * 2.0f);
		// 鏡に映る映像の中での鏡の四隅の座標を算出( 同次座標 )
		id.Handle.SetDraw_Screen(Mirrorcampos, Mirrorcamtgt, VGet(0, 1.f, 0.f), fov, near_distance, far_distance);
		for (int i = 0; i < 4; i++) {
			id.ScreenPosW[i] = ConvWorldPosToScreenPosPlusW(id.WorldPos[i].get());
		}
		// 鏡に映る映像の中での鏡の四隅の座標を算出( 同次座標 )
		id.Handle.SetDraw_Screen(campos, camtgt, camup, fov, near_distance,far_distance);
		id.canlook = true;
		for (int z = 0; z < 4; z++) {
			if (id.canlook) {
				float p = ConvWorldPosToScreenPos(id.WorldPos[z].get()).z;
				if (p < 0.0f || p > 1.1f) {
					id.canlook = false;
				}
			}
		}
		//ミラーオフ
		id.canlook = false;

	}
	// 鏡の描画
	void Mirror_Render(void) {
		for (auto& obj : Mirror_obj) {
			if (obj.canlook) {
				// 鏡の描画に使用するマテリアルのセットアップ
				Material.Ambient = obj.AmbientColor;
				Material.Diffuse = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
				Material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
				Material.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
				Material.Power = 1.0f;
				SetMaterialParam(Material);
				// 鏡の面の法線を算出
				MirrorNormal = ((obj.WorldPos[1] - obj.WorldPos[0]).cross(obj.WorldPos[2] - obj.WorldPos[0])).Norm();
				// 鏡に映る映像を書き込んだ画像のテクスチャのサイズを取得
				GetGraphTextureSize(obj.Handle.get(), &TextureW, &TextureH);
				// 鏡の描画に使用する頂点のセットアップ
				{
					VUnitPos[0] = (obj.WorldPos[2] - obj.WorldPos[0])*(1.0f / (MIRROR_POINTNUM - 1));
					VUnitPos[1] = (obj.WorldPos[3] - obj.WorldPos[1])*(1.0f / (MIRROR_POINTNUM - 1));
					VUnitUV[0] = F4Scale(F4Sub(obj.ScreenPosW[2], obj.ScreenPosW[0]), 1.0f / (MIRROR_POINTNUM - 1));
					VUnitUV[1] = F4Scale(F4Sub(obj.ScreenPosW[3], obj.ScreenPosW[1]), 1.0f / (MIRROR_POINTNUM - 1));
					DiffuseColor = obj.DiffuseColor;
					SpecularColor = GetColorU8(0, 0, 0, 0);
					VPos[0] = obj.WorldPos[0];
					VPos[1] = obj.WorldPos[1];
					VUV[0] = obj.ScreenPosW[0];
					VUV[1] = obj.ScreenPosW[1];
					int k = 0;
					for (auto& v : Vert) {
						if (k%MIRROR_POINTNUM == 0) {
							HUnitPos = (VPos[1] - VPos[0])*(1.0f / (MIRROR_POINTNUM - 1));
							HPos = VPos[0];
							HUnitUV = F4Scale(F4Sub(VUV[1], VUV[0]), 1.0f / (MIRROR_POINTNUM - 1));
							HUV = VUV[0];
						}
						{
							v.pos = HPos.get();
							v.norm = MirrorNormal.get();
							v.dif = DiffuseColor;
							v.spc = SpecularColor;
							v.u = HUV.x / (HUV.w * TextureW);
							v.v = HUV.y / (HUV.w * TextureH);
							v.su = 0.0f;
							v.sv = 0.0f;
							HUV = F4Add(HUV, HUnitUV);
							HPos += HUnitPos;
						}
						if (k%MIRROR_POINTNUM == 0) {
							VUV[0] = F4Add(VUV[0], VUnitUV[0]);
							VUV[1] = F4Add(VUV[1], VUnitUV[1]);
							VPos[0] += VUnitPos[0];
							VPos[1] += VUnitPos[1];
						}
						k++;
					}
				}
				// 鏡の描画に使用する頂点インデックスをセットアップ
				{
					int k = 0;
					for (int i = 0; i < MIRROR_POINTNUM - 1; i++) {
						for (int j = 0; j < MIRROR_POINTNUM - 1; j++) {
							Index[k++] = unsigned short((i + 0) * MIRROR_POINTNUM + j + 0);
							Index[k++] = unsigned short((i + 0) * MIRROR_POINTNUM + j + 1);
							Index[k++] = unsigned short((i + 1) * MIRROR_POINTNUM + j + 0);
							Index[k++] = unsigned short((i + 1) * MIRROR_POINTNUM + j + 1);
							Index[k++] = unsigned short((i + 1) * MIRROR_POINTNUM + j + 0);
							Index[k++] = unsigned short((i + 0) * MIRROR_POINTNUM + j + 1);
						}
					}
				}
				// 鏡を描画
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				SetDrawBlendMode(obj.BlendParam[0], obj.BlendParam[1]);
				DrawPolygonIndexed3D(&Vert[0], int(Vert.size()), &Index[0], int(Index.size() / 3), obj.Handle.get(), FALSE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				SetDrawMode(DX_DRAWMODE_NEAREST);
			}
		}
	}
	/**/
	template <typename T2>
	void draw_VR(T2 draw_doing,const cam_info& cams) {
		if (this->use_vr) {
			for (char i = 0; i < 2; i++) {
				VECTOR_ref eyepos = VECTOR_ref(cams.campos) + this->GetEyePosition_minVR(i);
				outScreen[i].SetDraw_Screen(eyepos, eyepos + cams.camvec, cams.camup, cams.fov, cams.near_, cams.far_);
				draw_doing();
				GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				outScreen[i].DrawGraph(0, 0, false);
				this->PutEye((ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D(), i);
			}
		}
		else {
			outScreen[0].SetDraw_Screen(cams.campos, VECTOR_ref(cams.campos) + cams.camvec, cams.camup, cams.fov, cams.near_, cams.far_);
			draw_doing();
		}
		GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
		{
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			outScreen[0].DrawExtendGraph(0, 0, this->out_disp_x, this->out_disp_y, true);
		}
	}
	/**/
};

typedef std::pair<int, VECTOR_ref> frames;
class switchs {
public:
	bool first;
	uint8_t second;

	switchs() {
		first = false;
		second = 0;
	};

	void ready(bool on) {
		first = on;
		second = 0;
	}

	void get_in(bool key) {
		second = std::clamp<uint8_t>(second + 1, 0, (key ? 2 : 0));
		if (second == 1) {
			first ^= 1;
		}
	}
};

//ついで
#include "debug.hpp"
