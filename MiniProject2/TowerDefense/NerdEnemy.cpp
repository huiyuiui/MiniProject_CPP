#include <allegro5/allegro_primitives.h>
#include <allegro5/base.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "AudioHelper.hpp"
#include "NerdEnemy.hpp"
#include "Group.hpp"
#include "EnemyBullet.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Turret.hpp"
#include "Collider.hpp"
#include "Bullet.hpp"
#include "DirtyEffect.hpp"
#include "IScene.hpp"
#include "LOG.hpp"
#include "GameEngine.hpp"
#include "ExplosionEffect.hpp"

NerdEnemy::NerdEnemy(int x, int y) : Enemy("play/enemy-4.png", x, y, 14, 200, 5, 20) {
    // TODO 2 (6/8): You can imitate the 2 files: 'NormalEnemy.hpp', 'NormalEnemy.cpp' to create a new enemy.
}

void NerdEnemy::CreateBullet(){
	Engine::Point diff = Engine::Point(-1, 0);
	float rotation = ALLEGRO_PI / 2;
	getPlayScene()->EnemyBulletGroup->AddNewObject(new EnemyBullet("play/bullet-7.png", 300, 1, Position, diff, rotation, this));
	AudioHelper::PlayAudio("missile.wav");
}

void NerdEnemy::Update(float deltaTime){
	PlayScene* scene = getPlayScene();
	float remainSpeed = speed * deltaTime;
	currentSpeed += speed * 0.005;
	int canMove = 1;
	for (auto& it : scene->TowerGroup->GetObjects()) {
		Turret* turret = dynamic_cast<Turret*>(it);
		if (Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, turret->Position - turret->Size / 2, turret->Position + turret->Size / 2)) {
			canMove = 0;
			Velocity.x = 0;
			reload -= deltaTime;
			if (reload <= 0) {
				reload = coolDown;
				CreateBullet();
			}
		}
	}
	if (canMove) Velocity.x = currentSpeed;
	Position.x -= Velocity.x * deltaTime;
	Position.y += Velocity.y * deltaTime;
	if (Position.x <= PlayScene::EndGridPointx * PlayScene::BlockSize + PlayScene::BlockSize / 2) {
		Hit(hp);
		getPlayScene()->Hit();
		reachEndTime = 0;
		return;
	}
	Engine::Point vec = target - Position;
	reachEndTime = (vec.Magnitude() - remainSpeed) / speed;
}
