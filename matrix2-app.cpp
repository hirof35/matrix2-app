#include "DxLib.h"
#include <vector>

// 2次元座標の構造体
struct Point2D {
    double x, y;
};

// 画面サイズ
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int CENTER_X = SCREEN_WIDTH / 2;
const int CENTER_Y = SCREEN_HEIGHT / 2;
const double SCALE = 50.0; // 1単位あたりのピクセル数

// 元の図形（緑の三角形）の頂点データ
const std::vector<Point2D> original_pts = {
    { 1.0, 1.0 },
    { 2.0, 1.0 },
    { 1.0, 2.0 }
};

// 数値を画面に描画する補助関数
void DrawMatrixInfo(double a, double b, double c, double d) {
    int color = GetColor(0, 0, 0);
    DrawString(20, 20, "--- 行列の成分 (キーで変更可能) ---", color);
    DrawFormatString(20, 40, color, "  [ Q / A ] a = %5.2f      [ W / S ] b = %5.2f", a, b);
    DrawFormatString(20, 60, color, "  [ E / D ] c = %5.2f      [ R / F ] d = %5.2f", c, d);
    DrawString(20, 90, "※ Q, W, E, R で増加 / A, S, D, F で減少", color);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 画面設定
    ChangeWindowMode(TRUE);
    SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
    SetMainWindowText("行列による一次変換の可視化");

    if (DxLib_Init() == -1) return -1;
    SetDrawScreen(DX_SCREEN_BACK);

    // 行列の初期値（単位行列）
    double a = 1.0, b = 0.0;
    double c = 0.0, d = 1.0;

    // ループ
    while (ProcessMessage() == 0 && ClearDrawScreen() == 0) {
        
        // --- 1. 入力処理（リアルタイムで行列の値を変更） ---
        double change_speed = 0.02;
        // aの変更
        if (CheckHitKey(KEY_INPUT_Q)) a += change_speed;
        if (CheckHitKey(KEY_INPUT_A)) a -= change_speed;
        // bの変更
        if (CheckHitKey(KEY_INPUT_W)) b += change_speed;
        if (CheckHitKey(KEY_INPUT_S)) b -= change_speed;
        // cの変更
        if (CheckHitKey(KEY_INPUT_E)) c += change_speed;
        if (CheckHitKey(KEY_INPUT_D)) c -= change_speed;
        // dの変更
        if (CheckHitKey(KEY_INPUT_R)) d += change_speed;
        if (CheckHitKey(KEY_INPUT_F)) d -= change_speed;

        // --- 2. 背景とグリッド線の描画 ---
        // 背景を白に
        DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GetColor(255, 255, 255), TRUE);

        // グリッド線（黒）
        int grid_color = GetColor(200, 200, 200);
        for (int i = -7; i <= 7; i++) {
            int posX = CENTER_X + static_cast<int>(i * SCALE);
            int posY = CENTER_Y + static_cast<int>(i * SCALE);
            DrawLine(posX, 0, posX, SCREEN_HEIGHT, grid_color);
            DrawLine(0, posY, SCREEN_WIDTH, posY, grid_color);
        }

        // 座標軸（赤）
        int axis_color = GetColor(255, 0, 0);
        DrawLine(CENTER_X, 0, CENTER_X, SCREEN_HEIGHT, axis_color);
        DrawLine(0, CENTER_Y, SCREEN_WIDTH, CENTER_Y, axis_color);


        // --- 3. 元の図形（緑）の描画 ---
        int green_color = GetColor(0, 150, 0);
        // DxLibのDrawTriangleは整数型なのでキャストする
        DrawTriangle(
            CENTER_X + static_cast<int>(original_pts[0].x * SCALE), CENTER_Y - static_cast<int>(original_pts[0].y * SCALE),
            CENTER_X + static_cast<int>(original_pts[1].x * SCALE), CENTER_Y - static_cast<int>(original_pts[1].y * SCALE),
            CENTER_X + static_cast<int>(original_pts[2].x * SCALE), CENTER_Y - static_cast<int>(original_pts[2].y * SCALE),
            green_color, TRUE
        );


        // --- 4. 行列変換後の図形（青）の計算と描画 ---
        std::vector<Point2D> transformed_pts(3);
        for (size_t i = 0; i < original_pts.size(); i++) {
            // JavaScriptのロジックと同じ一次変換
            // x' = a*x + b*y
            // y' = c*x + d*y
            transformed_pts[i].x = a * original_pts[i].x + b * original_pts[i].y;
            transformed_pts[i].y = c * original_pts[i].x + d * original_pts[i].y;
        }

        int blue_color = GetColor(0, 0, 255);
        DrawTriangle(
            CENTER_X + static_cast<int>(transformed_pts[0].x * SCALE), CENTER_Y - static_cast<int>(transformed_pts[0].y * SCALE),
            CENTER_X + static_cast<int>(transformed_pts[1].x * SCALE), CENTER_Y - static_cast<int>(transformed_pts[1].y * SCALE),
            CENTER_X + static_cast<int>(transformed_pts[2].x * SCALE), CENTER_Y - static_cast<int>(transformed_pts[2].y * SCALE),
            blue_color, FALSE // 透過っぽく見せるため、あるいは重なりが見えるように枠線だけに設定（好みでTRUEにしてください）
        );
        // 塗りつぶしも重ねて少し透過させたい場合は、DrawTriangleの最後の引数をTRUEにしたものをもう一つ描くと良いです。


        // --- 5. 情報表示 ---
        DrawMatrixInfo(a, b, c, d);

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}
