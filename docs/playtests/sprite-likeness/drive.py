"""Drive the ROM with a step script: each line 'label buttons frames' (buttons comma list or -). Saves 4x PNG for labels not starting with '_'."""
import sys
from pathlib import Path
from pyboy import PyBoy
rom, steps_file, out = sys.argv[1], sys.argv[2], Path(sys.argv[3])
out.mkdir(parents=True, exist_ok=True)
pb = PyBoy(rom, window="null", cgb=True, sound_emulated=False)
pb.set_emulation_speed(0)
held = set()
n = 0
for line in Path(steps_file).read_text().split("\n"):
    line = line.strip()
    if not line or line.startswith("#"): continue
    label, buttons, frames = line.split()
    want = set() if buttons == "-" else set(buttons.split(","))
    for b in held - want: pb.button_release(b)
    for b in want - held: pb.button_press(b)
    held = want
    pb.tick(int(frames), True)
    if not label.startswith("_"):
        n += 1
        img = pb.screen.image.convert("RGB")
        img.resize((640, 576), 0).save(out / f"{n:02d}-{label}.png")
        print(n, label)
# Crop each roster player's on-court sprite by CGB OBJ palette (palette id == roster id).
names = ["MATUS","MAGIC","BAMBI","CHRUX","CHOMPA","MEAT"]
img = pb.screen.image.convert("RGB")
boxes = {}
for i in range(40):
    s = pb.get_sprite(i)
    if not s.on_screen: continue
    p = pb.memory[0xFE00 + i*4 + 3] & 7; p = 4 if p == 7 else p
    x, y = s.x, s.y
    b = boxes.setdefault(p, [999, 999, -1, -1])
    b[0]=min(b[0],x); b[1]=min(b[1],y); b[2]=max(b[2],x+8); b[3]=max(b[3],y+16)
for p, b in sorted(boxes.items()):
    if p < 6 and b[2]-b[0] <= 24 or True:
        crop = img.crop((max(0,b[0]-2), max(0,b[1]-6), min(160,b[2]+2), min(144,b[3]+2)))
        crop.resize((crop.width*8, crop.height*8), 0).save(out / f"sprite-{names[p]}.png")
        print("sprite", names[p], b)
