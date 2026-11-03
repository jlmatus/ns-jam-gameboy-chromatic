"""Paint unused stack (lo..0xDEFF) after setup; report the deepest address overwritten, per frame window."""
import sys
from pathlib import Path
from pyboy import PyBoy
rom, steps, total, lo = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4],16)
pb = PyBoy(rom, window="null", cgb=True, sound_emulated=False); pb.set_emulation_speed(0)
held=set()
for line in Path(steps).read_text().split("\n"):
    line=line.strip()
    if not line: continue
    l,bt,f=line.split(); want=set() if bt=='-' else set(bt.split(','))
    for x in held-want: pb.button_release(x)
    for x in want-held: pb.button_press(x)
    held=want; pb.tick(int(f),True)
for x in held: pb.button_release(x)
m=pb.memory
sp=pb.register_file.SP
print('SP at sample', hex(sp))
paint_top=min(sp-16, 0xDEF0)
for a in range(lo, paint_top): m[a]=0xA5
deep=paint_top; deep_frame=None
for fr in range(total):
    pb.tick(1,True)
    for a in range(lo, deep):
        if m[a]!=0xA5:
            deep=a; deep_frame=fr; break
    if fr%600==0 or fr==total-1:
        pass
print('deepest stack write', hex(deep), 'at frame', deep_frame, 'map_tile_offset', m[0xDDEF])
