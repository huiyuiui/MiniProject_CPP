#include <allegro5/base.h>
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro.h>
#include <cmath>
#include <string>
#include <utility>
#include <random>
#include <vector>

#include "AudioHelper.hpp"
#include "Group.hpp"
#include "Bomb.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Enemy.hpp"
#include "GameEngine.hpp"
#include "Collider.hpp"
#include "ExplosionEffect.hpp"
#include "IScene.hpp"
#include "Turret.hpp"
#include "IObject.hpp"
#include "Point.hpp"
#include "DirtyEffect.hpp"



const int Bomb::Price = 30;
Bomb::Bomb(float x, float y) :
	Turret("play/bomb.png", x, y, 20, Price, 0, 1, false) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
}

void Bomb::CreateBullet() {}

void Bomb::OnExplode() {
	//Explode Turret
	for (auto& it : getPlayScene()->TowerGroup->GetObjects()) {
		Turret* turret = dynamic_cast<Turret*>(it);
		if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, turret->Position, turret->CollisionRadius)) {
			turret->Hit(INFINITY);
			break;
		}
	}
	getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
	for (int i = 0; i < 10; i++) {
		getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
	}
	getPlayScene()->TowerGroup->RemoveObject(objectIterator);
}

void Bomb::Update(float deltaTime){
	Sprite::Update(deltaTime);
	PlayScene* scene = getPlayScene();
	if (!Enabled)
		return;
	OnExplode();
}
