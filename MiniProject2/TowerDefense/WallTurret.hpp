#ifndef WALLTURRET_HPP
#define WALLTURRET_HPP
#include "Turret.hpp"

class WallTurret: public Turret {
public:
	static const int Price;
    WallTurret(float x, float y);
	void CreateBullet() override;
};
#endif // WALLTURRET_HPP
