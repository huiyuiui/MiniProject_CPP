#ifndef BOOMTURRET_HPP
#define BOOMTURRET_HPP
#include "Turret.hpp"

class BoomTurret: public Turret {
public:
	static const int Price;
	static const int BoomRadius;
    BoomTurret(float x, float y);
	void CreateBullet() override;
	void OnExplode() override;
};
#endif // BOOMTURRET_HPP
