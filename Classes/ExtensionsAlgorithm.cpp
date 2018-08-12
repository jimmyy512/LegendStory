#include "ExtensionsAlgorithm.h"
#include <string>
#include <iostream>
using namespace cocos2d;
bool ExtensionsAlgorithm::ContainsPoint(Vec2 checkPoint, Vec2 rectAnglePoint, float width, float height)
{
	if (checkPoint.x<rectAnglePoint.x + width / 4 &&
		checkPoint.x>rectAnglePoint.x - width / 4 &&
		checkPoint.y<rectAnglePoint.y + height / 4 &&
		checkPoint.y>rectAnglePoint.y - height / 4
		)
	{
		return true;
	}
	return false;
}
long ExtensionsAlgorithm::getCurrentTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}