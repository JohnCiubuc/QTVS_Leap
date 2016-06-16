#include "handcache.h"

HandCache::HandCache()
{
    fingers_p.append(Finger_Cache());
	fingers_p.append(Finger_Cache());
	fingers_p.append(Finger_Cache());
	fingers_p.append(Finger_Cache());
	fingers_p.append(Finger_Cache());
}

QList<Finger_Cache> HandCache::fingers()
{
    return fingers_p;
}
