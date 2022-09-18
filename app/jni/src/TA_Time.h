#ifndef TA_TIME_H
#define TA_TIME_H

inline int convertMsec(int msec)
{
    return msec * 1000;
}

inline int convertSec(int sec)
{
    return sec * 1000000;
}

inline int convertFrames(int frames)
{
    return frames * 1000000 / 60;
}

#endif // TA_TIME_H
