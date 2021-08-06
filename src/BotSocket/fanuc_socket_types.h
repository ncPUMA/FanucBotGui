#pragma once

#include <QVector>

typedef QVector<double> joint_data;
typedef struct xyzwpr_data_s {
    QVector<double> xyzwpr;
    bool flip = false; // Flip/Unflip
    bool left = false;
    bool up = true;    // Up/Down
    bool top = true;   // Top/Bottom
    int t1 = 0, t2 = 0, t3 = 0;
} xyzwpr_data;

inline int fanuc_config_make(const xyzwpr_data &data)
{
    uint32_t config = ((data.t1 & 0xFF) << 0) |
                      ((data.t2 & 0xFF) << 8) |
                      ((data.t3 & 0xFF) << 16);
    if(data.flip)
        config |= 0x80000000;
    if(data.left)
        config |= 0x40000000;
    if(data.up)
        config |= 0x20000000;
    if(data.top)
        config |= 0x10000000;
    return config;
}

inline void fanuc_config_parse(int config, xyzwpr_data &data)
{
    data.flip = (config & 0x80000000) != 0; // flip / unflip
    data.left = (config & 0x40000000) != 0; // left / right
    data.up   = (config & 0x20000000) != 0; // up / down
    data.top  = (config & 0x10000000) != 0; // top / bottom
    data.t1   = (int8_t)(config & 0xff);
    data.t2   = (int8_t)((config >> 8) & 0xff);
    data.t3   = (int8_t)((config >> 16) & 0xff);
}

