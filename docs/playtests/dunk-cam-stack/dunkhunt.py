"""Run a lineup with idle human; save Dunk Cam frames (dark mid-band) every 3 frames."""
import sys
from pathlib import Path
from collections import Counter
from pyboy import PyBoy
rom, steps, out, total = sys.argv[1], sys.argv[2], Path(sys.argv[3]), int(sys.argv[4])
out.mkdir(parents=True, exist_ok=True)
pb = PyBoy(rom, window="null", cgb=True, sound_emulated=False); pb.set_emulation_speed(0)
held=set()
for line in Path(steps).read_text().split("\n"):
    line=line.strip()
    if not line or line.startswith("#"): continue
    label,b,f=line.split(); want=set() if b=='-' else set(b.split(','))
    for x in held-want: pb.button_release(x)
    for x in want-held: pb.button_press(x)
    held=want; pb.tick(int(f),True)
for x in held: pb.button_release(x)
saved=0; run=0
for fr in range(total):
    pb.tick(1,True)
    if fr%3: continue
    im=pb.screen.image.convert("RGB")
    band=[im.getpixel((x,y)) for x in range(0,160,4) for y in range(70,110,4)]
    dark=sum(1 for p in band if sum(p)<130)/len(band)
    if dark>0.8:
        if saved<400: im.save(out/f"{fr:06d}.png"); saved+=1
print("saved",saved)
