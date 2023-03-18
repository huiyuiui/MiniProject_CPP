#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "AudioHelper.hpp"
#include "Group.hpp"
#include "WallTurret.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Enemy.hpp"

const int WallTurret::Price = 40;
WallTurret::WallTurret(float x, float y) :
	Turret("play/turret-5.png", x, y, 20, Price, 0, 30, false) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
}

void WallTurret::CreateBullet(){}
