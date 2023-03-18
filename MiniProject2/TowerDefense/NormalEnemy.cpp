#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "AudioHelper.hpp"
#include "NormalEnemy.hpp"
#include "Group.hpp"
#include "EnemyBullet.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Turret.hpp"

NormalEnemy::NormalEnemy(int x, int y) : Enemy("play/enemy-1.png", x, y, 10, 50, 5, 5) {
    // TODO 2 (6/8): You can imitate the 2 files: 'NormalEnemy.hpp', 'NormalEnemy.cpp' to create a new enemy.
}

void NormalEnemy::CreateBullet(){
	Engine::Point diff = Engine::Point(-1, 0);
	float rotation = ALLEGRO_PI / 2;
	getPlayScene()->EnemyBulletGroup->AddNewObject(new EnemyBullet("play/bullet-7.png", 500, 1,Position, diff, rotation, this));
	AudioHelper::PlayAudio("missile.wav");
}
