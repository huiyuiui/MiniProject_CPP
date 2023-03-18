#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "AudioHelper.hpp"
#include "StrongEnemy.hpp"
#include "Group.hpp"
#include "EnemyBullet.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Turret.hpp"

StrongEnemy::StrongEnemy(int x, int y) : Enemy("play/enemy-3.png", x, y, 20, 20, 100, 50) {
	// Use bounding circle to detect collision is for simplicity, pixel-perfect collision can be implemented quite easily,
	// and efficiently if we use AABB collision detection first, and then pixel-perfect collision.
}

void StrongEnemy::CreateBullet(){
	Engine::Point diff = Engine::Point(-1, 0);
	float rotation = ALLEGRO_PI / 2;
	getPlayScene()->EnemyBulletGroup->AddNewObject(new EnemyBullet("play/bullet-7.png", 500, 2, Position, diff, rotation, this));
	AudioHelper::PlayAudio("missile.wav");
}
