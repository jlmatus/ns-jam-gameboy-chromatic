"""Reconstruct the chosen portrait's color regions on the native 96px grid.

This is palette-aware artwork conversion, not a new face design. It removes
generated texture/alpha debris, rebuilds opaque connected regions as whole native
cells, and preserves the original landmark positions and aspect ratio. The card
compiler subsequently enforces actual four-color tile families.
"""
import hashlib
import json
from collections import deque
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'art/portraits/matus-v3/design-reference.png'
OUT = ROOT / 'art/portraits/matus-v4'
PALETTE = {
    '.': '#00000000', '#': '#22303AFF', 'h': '#455864FF',
    's': '#B96D47FF', 'o': '#F2C18DFF', 'b': '#7B4D36FF',
    'l': '#D68C70FF',
}

OUT.mkdir(exist_ok=True)
source = np.asarray(Image.open(SOURCE).convert('RGBA'))
keys = list(PALETTE)[1:]
colors = np.array([tuple(bytes.fromhex(PALETTE[k][1:7])) for k in keys], dtype=np.int32)
rgb = source[:,:,:3].astype(np.int32)
distances = ((rgb[:,:,None,:]-colors[None,None,:,:])**2).sum(axis=3)
region = distances.argmin(axis=2)+1
valid = (source[:,:,3]>=224) & (distances.min(axis=2)<100**2)

# Keep the connected head, dropping detached alpha/color specks around the study.
height,width = valid.shape
seen = np.zeros(valid.shape,dtype=bool)
largest=[]
for sy,sx in zip(*np.where(valid)):
    if seen[sy,sx]:continue
    seen[sy,sx]=True
    queue=deque([(sy,sx)]);component=[]
    while queue:
        y,x=queue.popleft();component.append((y,x))
        for ny,nx in ((y-1,x),(y+1,x),(y,x-1),(y,x+1)):
            if 0<=ny<height and 0<=nx<width and valid[ny,nx] and not seen[ny,nx]:
                seen[ny,nx]=True;queue.append((ny,nx))
    if len(component)>len(largest):largest=component
mask=np.zeros(valid.shape,dtype=bool)
for y,x in largest:mask[y,x]=True
ys,xs=np.where(mask)
bounds=[int(xs.min()),int(ys.min()),int(xs.max())+1,int(ys.max())+1]
x0,y0,x1,y1=bounds
scale=92/(y1-y0)
native_width=round((x1-x0)*scale)
left=(96-native_width)//2
cells=[['.']*96 for _ in range(96)]

# Rebuild every source color region into whole filled native cells. Cell areas
# preserve narrow feature clusters without importing gradients or antialiasing.
for ny in range(92):
    sy0=round(y0+ny/scale);sy1=round(y0+(ny+1)/scale)
    for nx in range(native_width):
        sx0=round(x0+nx/scale);sx1=round(x0+(nx+1)/scale)
        coverage=mask[sy0:sy1,sx0:sx1]
        if coverage.mean()<0.5:continue
        labels=region[sy0:sy1,sx0:sx1][coverage]
        counts=np.bincount(labels,minlength=len(keys)+1)
        cells[ny+2][nx+left]=keys[int(counts.argmax())-1]

spec={
    'name':'MATUS','version':4,'width':96,'height':96,
    'status':'Faithful reconstruction of the displayed portrait study; for user review',
    'method':'Source-connected-region reconstruction into flat whole native cells, with preserved aspect ratio/landmarks; no alternate hand-drawn face.',
    'reference':'../matus-v3/design-reference.png',
    'referenceSha256':hashlib.sha256(SOURCE.read_bytes()).hexdigest(),
    'referenceBounds':bounds,'nativeHeadBounds':[left,2,left+native_width,94],
    'palette':PALETTE,
    'rows':[],
}
for row in cells:
    value=''.join(row);run=value.strip('.')
    spec['rows'].append([len(value)-len(value.lstrip('.')) if run else 0,run])
(OUT/'pixels.json').write_text(json.dumps(spec,indent=2)+'\n')
im=Image.new('RGBA',(96,96))
for y,row in enumerate(cells):
    for x,c in enumerate(row):im.putpixel((x,y),tuple(bytes.fromhex(PALETTE[c][1:])))
im.save(OUT/'matus-96.png')
im.resize((576,576),Image.Resampling.NEAREST).save(OUT/'matus-preview.png')
print(json.dumps({k:spec[k] for k in ['referenceSha256','referenceBounds','nativeHeadBounds']}))
