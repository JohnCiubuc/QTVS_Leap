#ifndef HANDCACHE_H
#define HANDCACHE_H

#include <QObject>
#include <QList>


struct Finger_Cache
{
public:
	bool isExtended;
	bool bGestureToggle = true;
	float pos_x;
	float pos_y;
	float pos_z;
};

class HandCache
{
public:
    HandCache();

    QList<Finger_Cache> fingers();
    QList<Finger_Cache> fingers_p;

	bool bGestureToggle = true;
private:

};

#endif // HANDCACHE_H
