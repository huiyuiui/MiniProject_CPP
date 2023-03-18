#ifndef NERDENEMY_HPP
#define NERDENEMY_HPP
#include "Enemy.hpp"

class NerdEnemy : public Enemy {
public:
	NerdEnemy(int x, int y);
	void CreateBullet() override;
	void Update(float deltaTime) override;
};
#endif // NERDENEMY_HPP
