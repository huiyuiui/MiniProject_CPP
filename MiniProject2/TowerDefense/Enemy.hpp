#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <vector>
#include <string>

#include "Point.hpp"
#include "Sprite.hpp"

class Bullet;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite {
protected:
	Engine::Point target;
	float speed;
	float hp;
	float coolDown = 0.5;
	float reload = 0;
	float rotateRadian = 2 * ALLEGRO_PI;
	int money;
	std::list<Enemy*>::iterator lockedEnemyIterator;
	PlayScene* getPlayScene();
	virtual void OnExplode();
	virtual void CreateBullet() = 0;
public:
	Turret* Target = nullptr;
	float reachEndTime;
	float currentSpeed;
	std::list<Turret*> lockedTurrets;
	std::list<Bullet*> lockedBullets;
	Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money );
 	void Hit(float damage);
	void Update(float deltaTime) override;
	void Draw() const override;
};
#endif // ENEMY_HPP
