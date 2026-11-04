import sys
from pathlib import Path
from PIL import Image
d=Path(sys.argv[1])
rows=[]
for name in ["left","right","up","down"]:
    sheet=Image.open(d/f"run-{name}.png").convert("RGB")
    sheet=sheet.resize((sheet.width//2,sheet.height//2),0)
    w,h=160,144; crops=[]
    for i in range(24):
        f=sheet.crop(((i%8)*w,(i//8)*h,(i%8)*w+w,(i//8)*h+h)); px=f.load()
        ys=[(y,x) for y in range(10,120) for x in range(160) if px[x,y]==(248,208,112)]
        if not ys: continue
        y,x=min(ys)
        crops.append(f.crop((x-14,y-2,x+26,y+54)))
    rows.append(crops[:16])
W,H=40,56
im=Image.new("RGB",(W*16,H*4),(0,0,0))
for r,c in enumerate(rows):
    for i,f in enumerate(c): im.paste(f,(i*W,r*H))
im.resize((im.width*3,im.height*3),0).save(d/"closeup.png")
