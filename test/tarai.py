#!/usr/bin/env python3
i = 0


def tarai(x, y, z):
    global i
    i += 1
    if x <= y:
        return y
    else:
        return tarai(tarai(x - 1, y, z), tarai(y - 1, z, x), tarai(z - 1, x, y))


print(tarai(11, 5, 0))
print(i)
