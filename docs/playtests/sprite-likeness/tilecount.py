import sys
from PIL import Image
im=Image.open(sys.argv[1]).convert('RGB'); W,H=im.size
px=im.load()
def tile(x0,y0,h):
    return tuple(tuple(px[x0+x,y0+y] for x in range(8)) for y in range(h))
for h in (8,16):
    s=set(); sf=set()
    for fx in range(0,W,16):
        for tx in (0,8):
            for ty in range(0,48,h):
                t=tile(fx+tx,ty,h); s.add(t)
                f=tuple(tuple(reversed(r)) for r in t); sf.add(min(t,f))
    print(f'{h}px tiles: unique={len(s)} flip-merged={len(sf)}')
