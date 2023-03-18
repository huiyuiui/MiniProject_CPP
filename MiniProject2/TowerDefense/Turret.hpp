#ifndef TURRET_HPP
#define TURRET_HPP
#include <allegro5/base.h>
#include <list>
#include <string>
#include <vector>

#include "Sprite.hpp"
#include "Point.hpp"

class Enemy;
class PlayScene;
class EnemyBullet;

class Turret: public Engine::Sprite {
protected:
    Engine::Point target;
    int hp;
    int price;
    float coolDown;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    bool isBoom = false;
    std::list<Turret*>::iterator lockedTurretIterator;
    PlayScene* getPlayScene();
    // Reference: Design Patterns - Factory Method.
    virtual void CreateBullet() = 0;
    virtual void OnExplode();

public:
    Enemy* Target = nullptr;
    std::list<Enemy*> lockedEnemys;
    std::list<EnemyBullet*> lockedEnemyBullets;
    bool Enabled = true;
    bool Preview = false;
    Turret(/*std::string imgBase,*/std::string imgTurret, float x, float y, float radius, int price, float coolDown, int hp, bool isBoom);
    void Update(float deltaTime) override;
    void Draw() const override;
	int GetPrice() const;
    void Hit(float damage);
};
#endif // TURRET_HPP
