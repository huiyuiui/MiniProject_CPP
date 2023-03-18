//Bomb.hpp
#ifndef BOMB_HPP
#define BOMB_HPP

#include "Turret.hpp"

class Bomb : public Turret {
public:
	static const int Price;
	Bomb(float x, float y);
	void CreateBullet() override;
	void OnExplode() override;
	void Update(float deltaTime) override;
};

#endif // BOMB_HPP