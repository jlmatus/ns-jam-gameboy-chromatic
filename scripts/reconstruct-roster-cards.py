"""Recover detailed 96px card masters from the original approved studies.

Use source color-region coverage, rather than enlarging the compact 32px cels.
Keep every source study and compact portrait intact, as with MATUS/CHOMPA.
"""
import hashlib
import json
from collections import deque
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]


def reconstruct(name):
    source_path = ROOT / f'art/portraits/{name}-v1/design-reference.png'
    output = ROOT / f'art/portraits/{name}-v2'
    output.mkdir(exist_ok=True)
    palette = {
        '.': '#00000000', '#': '#22303AFF',
        's': '#B96D47FF', 'o': '#F2C18DFF',
    }
    if name == 'chrux':
        palette['h'] = '#455864FF'
    source = np.asarray(Image.open(source_path).convert('RGBA'))
    keys = list(palette)[1:]
    colors = np.array([tuple(bytes.fromhex(palette[k][1:7])) for k in keys], dtype=np.int32)
    rgb = source[:, :, :3].astype(np.int32)
    distances = ((rgb[:, :, None, :] - colors[None, None, :, :]) ** 2).sum(axis=3)
    region = distances.argmin(axis=2) + 1
    valid = (source[:, :, 3] >= 224) & (distances.min(axis=2) < 100 ** 2)
    valid &= ~((rgb[:, :, 0] > 180) & (rgb[:, :, 2] > 180) & (rgb[:, :, 1] < 100))

    # Remove disconnected generation specks, keeping the complete head.
    height, width = valid.shape
    seen = np.zeros(valid.shape, dtype=bool)
    largest = []
    for sy, sx in zip(*np.where(valid)):
        if seen[sy, sx]:
            continue
        seen[sy, sx] = True
        queue = deque([(sy, sx)])
        component = []
        while queue:
            y, x = queue.popleft()
            component.append((y, x))
            for ny, nx in ((y-1, x), (y+1, x), (y, x-1), (y, x+1)):
                if 0 <= ny < height and 0 <= nx < width and valid[ny, nx] and not seen[ny, nx]:
                    seen[ny, nx] = True
                    queue.append((ny, nx))
        if len(component) > len(largest):
            largest = component
    mask = np.zeros(valid.shape, dtype=bool)
    for y, x in largest:
        mask[y, x] = True
    ys, xs = np.where(mask)
    bounds = [int(xs.min()), int(ys.min()), int(xs.max())+1, int(ys.max())+1]
    x0, y0, x1, y1 = bounds
    scale = min(92/(y1-y0), 92/(x1-x0))
    native_width, native_height = round((x1-x0)*scale), round((y1-y0)*scale)
    left, top = (96-native_width)//2, (96-native_height)//2
    cells = [['.']*96 for _ in range(96)]
    for ny in range(native_height):
        sy0, sy1 = round(y0+ny/scale), round(y0+(ny+1)/scale)
        for nx in range(native_width):
            sx0, sx1 = round(x0+nx/scale), round(x0+(nx+1)/scale)
            coverage = mask[sy0:sy1, sx0:sx1]
            if coverage.mean() < 0.5:
                continue
            labels = region[sy0:sy1, sx0:sx1][coverage]
            counts = np.bincount(labels, minlength=len(keys)+1)
            cells[ny+top][nx+left] = keys[int(counts.argmax())-1]
    rows = []
    for row in cells:
        value = ''.join(row)
        run = value.strip('.')
        rows.append([len(value)-len(value.lstrip('.')) if run else 0, run])
    spec = {
        'name': name.upper(), 'version': 2, 'width': 96, 'height': 96,
        'status': 'Detailed reconstruction of the original study; for user review',
        'method': 'Source-connected color regions reconstructed as whole native cells; preserved aspect ratio and facial landmarks.',
        'reference': f'../{name}-v1/design-reference.png',
        'referenceSha256': hashlib.sha256(source_path.read_bytes()).hexdigest(),
        'referenceBounds': bounds,
        'nativeHeadBounds': [left, top, left+native_width, top+native_height],
        'palette': palette, 'rows': rows,
    }
    (output/'pixels.json').write_text(json.dumps(spec, indent=2)+'\n')
    image = Image.new('RGBA', (96, 96))
    for y, row in enumerate(cells):
        for x, c in enumerate(row):
            image.putpixel((x, y), tuple(bytes.fromhex(palette[c][1:])))
    image.save(output/f'{name}-96.png')
    image.resize((576, 576), Image.Resampling.NEAREST).save(output/f'{name}-preview.png')
    print(name, spec['nativeHeadBounds'], spec['referenceSha256'])


if __name__ == '__main__':
    for player in ('magic', 'bambi', 'chrux'):
        reconstruct(player)
