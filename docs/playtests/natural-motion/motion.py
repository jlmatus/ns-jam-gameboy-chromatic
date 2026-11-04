import sys
from pathlib import Path
from pyboy import PyBoy
from PIL import Image
rom, lineup, out = sys.argv[1], sys.argv[2], Path(sys.argv[3]); out.mkdir(parents=True, exist_ok=True)
JAM=0xC1F6
pb=PyBoy(rom,window="null",cgb=True,sound_emulated=False); pb.set_emulation_speed(0)
held=set()
def press(b):
    global held
    w=set(b)
    for x in held-w: pb.button_release(x)
    for x in w-held: pb.button_press(x)
    held=w
def run(b,n): press(b); pb.tick(n,True)
for line in Path(lineup).read_text().split("\n"):
    line=line.strip()
    if not line or line[0] in "#g": continue
    _,b,n=line.split(); run([] if b=="-" else b.split(","),int(n))
def live():
    t=pb.memory[JAM+180]; pb.tick(4,True); return pb.memory[JAM+180]!=t
for _ in range(12):
    if live(): break
    run(["a"],4); run([],60)
def ours(): return pb.memory[JAM+168]==0 and pb.memory[JAM+166]==pb.memory[JAM+216]
for t in range(4000):
    if ours(): break
    run(["a"] if pb.memory[JAM+168]==6 and t%20<2 else [],1)
frames=[]
for d in ["left","right","up","down","right"]:
    seq=[]
    for k in range(24):
        run([d],3); seq.append(pb.screen.image.convert("RGB").copy())
    frames.append((d,seq))
    run([],10)
# strips: every frame scaled 2x, 8 per row
for d,seq in frames:
    w,h=seq[0].size
    im=Image.new("RGB",(w*8,h*3))
    for i,f in enumerate(seq): im.paste(f,((i%8)*w,(i//8)*h))
    im.resize((im.width*2,im.height*2),0).save(out/f"run-{d}.png")
print("ok", ours())
