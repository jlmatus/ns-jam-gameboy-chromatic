#!/usr/bin/env python3
"""Pack native NS JAM front art and reconstructed portrait masters."""
import json
import math
from pathlib import Path
from PIL import Image, ImageDraw
from night_court_art import home as night_home, FAMILIES as HOME_FAMILIES

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'art/front-proof'
CORE = ROOT / 'game/plugins/basketball/engine/src/core'
OUT.mkdir(parents=True, exist_ok=True)
glyphs = json.loads((ROOT / 'art/prototype-pixels.json').read_text())['hudGlyphs']
roster = json.loads((ROOT / 'art/ui/scouting-cards.json').read_text())
matus_source = roster['players'][0]
portrait = json.loads((ROOT / 'art/portraits' / matus_source.get('cardSource',matus_source['source']) / 'pixels.json').read_text())
N = '#102333'
D = '#183A49'
T = '#287780'
G = '#EFA42E'
I = '#FFF5D6'
O = '#E87521'
S = portrait['palette']['o'][:7]
SH = portrait['palette']['s'][:7]
INK = portrait['palette']['#'][:7]
BEARD = portrait['palette'].get('b',portrait['palette']['#'])[:7]
HAIR = portrait['palette'].get('h',portrait['palette']['#'])[:7]
LIP = portrait['palette'].get('l',portrait['palette']['s'])[:7]

def rgb(c):
    return tuple(int(c[i:i+2], 16) for i in (1, 3, 5))

def text(im, x, y, value, color=I, scale=1):
    d = ImageDraw.Draw(im)
    for k, c in enumerate(value):
        rows = glyphs.get(c, glyphs[' '])
        if c == '<':rows=[row[::-1] for row in glyphs['>']]
        for yy, row in enumerate(rows):
            for xx, v in enumerate(row):
                if v == '1':
                    d.rectangle((x+k*6*scale+xx*scale, y+yy*scale,
                                 x+k*6*scale+(xx+1)*scale-1, y+(yy+1)*scale-1), fill=color)

def face(im, x, y, scale, source=portrait):
    d = ImageDraw.Draw(im)
    for yy, (offset, row) in enumerate(source['rows']):
        for xx, c in enumerate(row):
            if c == '.':
                continue
            d.rectangle((x+(offset+xx)*scale, y+yy*scale,
                         x+(offset+xx+1)*scale-1, y+(yy+1)*scale-1),
                        fill=source['palette'][c][:7])

def ball(im, x, y, r=4):
    d = ImageDraw.Draw(im)
    d.ellipse((x-r, y-r, x+r, y+r), fill=O, outline=N)
    d.line((x, y-r+1, x, y+r-1), fill=N)
    d.line((x-r+1, y, x+r-1, y), fill=N)
    d.point((x-2, y-2), fill=G)

def rim(im, shake=0, net=0):
    d = ImageDraw.Draw(im)
    d.rectangle((137, 45, 139, 103), fill=D)
    d.rectangle((119, 44, 147, 62), fill=T, outline=I)
    d.rectangle((128, 51, 138, 59), outline=I)
    d.line((124, 63, 134, 63), fill=O, width=2)
    for xx in (124, 127, 130, 133):
        d.line((xx, 65, 128+shake, 74+net), fill=I)
    d.line((125, 68+net, 132, 68+net), fill=I)

def palm(im, x, y, flip=False):
    d = ImageDraw.Draw(im)
    f = -1 if flip else 1
    d.line((x, y+43, x+f*3, y), fill=N, width=3)
    for dx, dy in [(-20, 5), (-14, -5), (-4, -11), (10, -9), (21, 2), (15, 11)]:
        d.line((x, y, x+f*dx//2, y+dy//2-2, x+f*dx, y+dy), fill=N, width=3)

def court():
    im = Image.new('RGB', (160, 144), N)
    d = ImageDraw.Draw(im)
    d.rectangle((0, 39, 159, 71), fill=D)
    for x, top, width in [(0,57,12),(18,54,13),(37,62,11),(52,51,17),(78,58,12),(99,54,14),(122,60,18),(148,50,12)]:
        d.rectangle((x, top, x+width, 79), fill=N)
        if width > 12:
            d.rectangle((x+4, top+5, x+5, top+6), fill=G)
    d.polygon((46, 71, 77, 49, 104, 71), fill=N)
    for y in range(76, 90, 4):
        for x in range(0, 160, 8):
            d.line((x+(y%8)//2, y, x+4+(y%8)//2, y+4), fill=T)
            d.line((x+4+(y%8)//2, y, x+(y%8)//2, y+4), fill=T)
    d.line((0, 75, 159, 75), fill=T)
    d.rectangle((0, 90, 159, 143), fill=D)
    d.polygon((17, 91, 145, 91, 159, 139, 0, 139), fill=T)
    d.line((17, 91, 145, 91, 159, 139, 0, 139, 17, 91), fill=G)
    d.line((80, 91, 79, 140), fill=G)
    d.arc((55, 94, 105, 111), 0, 359, fill=G)
    palm(im, 13, 52)
    palm(im, 152, 48, True)
    return im

def home(selected):
    return night_home(selected, text)

def help_screen():
    im = Image.new('RGB', (160,144), N)
    d = ImageDraw.Draw(im)
    text(im, 17, 8, 'HOW TO PLAY', G, 2)
    # 2x heading is 132 pixels; all lines retain six-pixel tracking.
    for y, line in [(36,'D-PAD  MOVE'),(49,'A      PASS / STEAL'),(62,'B      SHOOT / BLOCK'),
                    (75,'START  HOLD TURBO'),(88,'TURBO A  SHOVE'),(101,'TURBO B  DUNK NEAR RIM'),
                    (114,'SELECT  PAUSE'),(134,'A OR B  HOME')]:
        text(im, 8, y, line.replace('/', '-'), I if y != 134 else G)
    return im

def card(stage, player=matus_source, source=portrait):
    im = Image.new('RGB', (160,144), N)
    d = ImageDraw.Draw(im)
    d.rectangle((0, 27, 101, 126), fill=D)
    for y in range(35, 123, 16):
        d.line((0, y, 101, y), fill=T)
    # Native 96px master renders 1:1; older masters use whole-cell multiples.
    assert 96 % source['width'] == 0
    face(im, 3, 28, 96 // source['width'], source)
    for x,y,dx,dy in [(0,28,8,8),(101,28,-8,8),(0,125,8,-8),(101,125,-8,-8)]:
        d.line((x,y,x+dx,y), fill=G)
        d.line((x,y,x,y+dy), fill=G)
    label=['YOUR PLAYER','CPU TEAMMATE','OPPONENT ONE','OPPONENT TWO'][stage]
    text(im, (160-len(label)*6)//2, 0, label, I)
    text(im, 5, 11, player['name'], G, 2)
    text(im, 113, 15, player['jersey'], I, 2)
    text(im, 112, 32, player['position'], T)
    labels={'STEAL':'STL','POWER':'PWR','BLOCK':'BLK'}
    for i, (name, value) in enumerate(player['featured']):
        name=labels.get(name,name)
        y=45+i*17
        text(im, 107, y, name, I)
        text(im, 144, y, str(value), G)
        for b in range(10):
            d.rectangle((107+b*5, y+9, 110+b*5, y+12), fill=G if b<value else T)
    # A full-width specialty ribbon keeps long labels out of the 56px stat
    # column. The approved portrait remains at (3, 28), exactly 96 x 96.
    style=player['playstyle']
    assert len(style)*6 <= 156
    d.rectangle((0, 128, 159, 135), fill=G)
    text(im, (160-len(style)*6)//2, 129, style, N)
    text(im, 5, 137, 'A PICK B BACK SELECT STATS', I)
    text(im, 0, 13, '<', I)
    text(im, 96, 13, '>', I)
    return im

def shot(frame):
    im = court()
    d = ImageDraw.Draw(im)
    # A fixed, low-contrast staging area separates the athlete from the fence.
    d.rectangle((19, 39, 93, 115), fill=D)
    d.ellipse((39, 111, 75, 117), fill=N)
    jump=[0,0,2,7,12,14,12,7,2,0,0,0][frame]
    head_y=53-jump
    hip_y=90-jump
    # Short stocky 58-pixel athlete: broad torso and bent compact legs.
    d.line((48,hip_y,43,102-jump,40,110-jump), fill=SH, width=6)
    d.line((63,hip_y,69,100-jump,70,110-jump), fill=S, width=6)
    d.rectangle((35,109-jump,45,112-jump), fill=I)
    d.rectangle((66,109-jump,77,112-jump), fill=I)
    d.rectangle((44,80-jump,66,92-jump), fill=N)
    d.line((45,81-jump,65,81-jump), fill=I)
    d.polygon((46,68-jump,65,68-jump,68,82-jump,43,82-jump), fill=I, outline=N)
    d.rectangle((51,63-jump,58,70-jump), fill=S)
    # Hand-authored 3/4 profile, with MATUS's hair and compact goatee.
    d.rectangle((48,head_y,60,head_y+12), fill=S)
    d.rectangle((47,head_y,58,head_y+3), fill=INK)
    d.rectangle((47,head_y+2,49,head_y+8), fill=INK)
    d.rectangle((60,head_y+7,62,head_y+9), fill=S)
    d.point((58,head_y+6), fill=N)
    d.line((57,head_y+10,61,head_y+10), fill=INK)
    d.line((55,head_y+12,60,head_y+12), fill=INK)
    text(im, 49, 73-jump, '13', N)
    hands=[(65,88),(67,68),(68,49),(69,38),(70,33),(70,33),(67,41),(66,50),(65,64),(64,77),(63,82),(63,82)]
    hx,hy=hands[frame]
    d.line((63,70-jump,73,64-jump,hx,hy), fill=S, width=4)
    d.line((47,72-jump,54,65-jump,hx-4,hy+3), fill=SH, width=4)
    rim(im, (frame%2) if frame>=9 else 0, 2 if frame in (9,10) else 0)
    ball_positions=[(65,88),(67,67),(68,48),(72,36),(84,29),(99,28),(112,33),(123,42),(129,57),(129,68),(129,83),(129,100)]
    ball(im,*ball_positions[frame],4)
    d.rectangle((0,0,159,23), fill=N)
    text(im, 8, 1, 'NS JAM', G)
    text(im, 8, 12, 'MATUS 13 - 3PT PLAYMAKER', I)
    d.rectangle((0,128,159,143), fill=N)
    text(im, 8, 130, 'A OR START  SKIP', I)
    if frame>=9:
        text(im, 96, 105, 'SWISH', G)
    return im

# All native pixels are finally represented by <=4 colors per tile, <=7 families.
# Palette choice is tile-local; output PNGs are the exact packed RGB555 image.
FAMILIES = [
    [N,D,T,G], [N,T,G,I], [N,D,T,I], [N,SH,S,I],
    [N,SH,S,INK], [N,SH,S,D], [N,O,G,I],
]
PAL = [[rgb(c) for c in f] for f in FAMILIES]
def quant(c):
    return tuple(round(v*31/255)*255//31 for v in c)
PAL = [[quant(c) for c in f] for f in PAL]
def dist(a,b):
    return sum((aa-bb)**2 for aa,bb in zip(a,b))
HOME_PAL = [[quant(rgb(c)) for c in f] for f in HOME_FAMILIES]
CARD_PAL = [[quant(rgb(c)) for c in f] for f in [
    [N,T,G,I], [N,D,T,G], [D,INK,HAIR,SH], [INK,SH,S,BEARD],
    [BEARD,SH,S,LIP], [D,SH,S,INK], [D,SH,S,BEARD],
]]
CHOMPA_PAL = [[quant(rgb(c)) for c in f] for f in [
    [N,T,G,I], [N,D,T,G], [D,INK,SH,S], [INK,SH,S,I],
    [D,INK,'#A6CE39',SH], [INK,'#A6CE39',SH,S], [D,INK,'#A6CE39',S],
]]
def pack(im, palettes=PAL, flips=False):
    pixels=im.load();result=Image.new('RGB',(160,144));rp=result.load()
    patterns=[];lookup={};tilemap=[];attrs=[];error=0
    for ty in range(18):
        for tx in range(20):
            colors=[quant(pixels[tx*8+x,ty*8+y]) for y in range(8) for x in range(8)]
            scores=[]
            for family in palettes:
                scores.append(sum(min(dist(c,p) for p in family) for c in colors))
            slot=min(range(len(palettes)),key=lambda s:scores[s]);error+=scores[slot]
            indices=[min(range(4),key=lambda i:dist(c,palettes[slot][i])) for c in colors]
            data=[]
            for y in range(8):
                lo=hi=0
                for x in range(8):
                    c=indices[y*8+x];lo|=(c&1)<<(7-x);hi|=(c>>1)<<(7-x)
                    rp[tx*8+x,ty*8+y]=palettes[slot][c]
                data.extend((lo,hi))
            key=bytes(data)
            if key not in lookup:
                index=len(patterns);lookup[key]=(index,0);patterns.append(data)
                if flips:
                    horizontal=bytes(int(f'{b:08b}'[::-1],2) for b in data)
                    vertical=bytes(v for y in range(7,-1,-1) for v in data[y*2:y*2+2])
                    both=bytes(int(f'{b:08b}'[::-1],2) for b in vertical)
                    for variant,flags in [(horizontal,32),(vertical,64),(both,96)]:
                        lookup.setdefault(variant,(index,flags))
            index,flags=lookup[key];tilemap.append(index%128);attrs.append(slot|flags|(8 if index>=128 else 0))
    if len(patterns)>256:raise ValueError(f'Native background budget exceeded: {len(patterns)}')
    return result,patterns,tilemap,attrs,error

def arr(name,data):return f'static const unsigned char {name}[]={{'+','.join(map(str,data))+'};\n'
manifest=[]
images=[(f'home-{i}',home(i)) for i in range(3)]+[('help',help_screen())]
images += [(f'matus-card-{i}',card(i)) for i in range(4)]
images += [(f'matus-shot-{i:02}',shot(i)) for i in range(12)]
chompa_source=roster['players'][4]
chompa_portrait=json.loads((ROOT/'art/portraits'/chompa_source['cardSource']/'pixels.json').read_text())
images += [(f'chompa-card-{i}',card(i,chompa_source,chompa_portrait)) for i in range(4)]
card_palettes={i:CARD_PAL for i in range(4,8)}
card_palettes.update({i:CHOMPA_PAL for i in range(20,24)})
card_frames={0:4,4:20}
for player_id in (1,2,3,5):
    player=roster['players'][player_id]
    source=json.loads((ROOT/'art/portraits'/player.get('cardSource',player['source'])/'pixels.json').read_text())
    colors=source['palette']
    # Compact masters enlarge only by whole pixels; no redraw or smoothing.
    palettes=[[quant(rgb(c)) for c in f] for f in [
        [N,T,G,I], [N,D,T,G], [D,colors['#'][:7],colors['s'][:7],colors['o'][:7]],
    ]]
    palettes += [palettes[-1]]*(7-len(palettes))
    start=len(images)
    card_frames[player_id]=start
    for stage in range(4):
        images.append((f'{player["name"].lower()}-card-{stage}',card(stage,player,source)))
        card_palettes[start+stage]=palettes
packed=[pack(im,HOME_PAL,True) if i<3 else pack(im,card_palettes[i],True) if i in card_palettes else pack(im) for i,(_,im) in enumerate(images)]
shared=set.intersection(*(set(bytes(t) for t in p[1]) for p in packed[8:20]))
common=sorted(shared);common_ids={t:i for i,t in enumerate(common)}
dynamic_max=max(sum(bytes(t) not in shared for t in p[1]) for p in packed[8:20])
assert len(common)+2*dynamic_max<=256
common_code='/* Shared immutable intro tiles. */\n#pragma bank 255\n#include <gbdk/platform.h>\n'
for b in range(2):
    chunk=common[b*128:(b+1)*128]
    if chunk:common_code+=arr(f'common{b}',[v for t in chunk for v in t])
common_code+='void jam_front_common(void) BANKED {\n'
for b in range(2):
    count=len(common[b*128:(b+1)*128])
    if count:common_code+=f'VBK_REG={b};set_bkg_data(0,{count},common{b});\n'
common_code+='VBK_REG=0;}\n'
(CORE/'jam_front_common.c').write_text(common_code)
for index,(name,im) in enumerate(images):
    result,tiles,tilemap,attrs,error=packed[index]
    result.save(OUT/f'{name}.png')
    result.resize((640,576),Image.Resampling.NEAREST).save(OUT/f'{name}-4x.png')
    if index==0:
        review=ROOT/'art/night-court-v2'
        diagnostic=ROOT/'game/artifacts/front-art'
        review.mkdir(parents=True,exist_ok=True)
        diagnostic.mkdir(parents=True,exist_ok=True)
        result.save(review/'home.png')
        result.resize((640,576),Image.Resampling.NEAREST).save(review/'home-4x.png')
        result.save(diagnostic/'night-court.png')
    code='/* Generated by scripts/create-front-proof.py. */\n#pragma bank 255\n#include <gbdk/platform.h>\n'
    if index<8 or index>=20:
        code+=arr('tiles0',[v for tile in tiles[:128] for v in tile])
        if len(tiles)>128:code+=arr('tiles1',[v for tile in tiles[128:] for v in tile])
        code+=arr('map',tilemap)+arr('attrs',attrs)
        code+=f'void jam_front_frame_{index}(void) BANKED {{\nLCDC_REG|=LCDCF_WIN9C00;VBK_REG=0;set_bkg_data(0,{min(128,len(tiles))},tiles0);\n'
        if len(tiles)>128:code+=f'VBK_REG=1;set_bkg_data(0,{len(tiles)-128},tiles1);\n'
        code+='VBK_REG=0;set_win_tiles(0,0,20,18,map);VBK_REG=1;set_win_tiles(0,0,20,18,attrs);VBK_REG=0;\n}\n'
    else:
        dynamic=[bytes(t) for t in tiles if bytes(t) not in shared]
        dyn_ids={t:i for i,t in enumerate(dynamic)}
        code+='#include "jam_dunk_cam.h"\n'
        code+=arr('dynamic',[v for t in dynamic for v in t])
        for b in range(2):
            mapped=[];attributed=[]
            for n,t in enumerate(tilemap):
                key=bytes(tiles[t+(128 if attrs[n]&8 else 0)])
                k=common_ids[key] if key in shared else len(common)+b*dynamic_max+dyn_ids[key]
                mapped.append(k%128);attributed.append((attrs[n]&7)|(8 if k>=128 else 0))
            code+=arr(f'map{b}',mapped)+arr(f'attrs{b}',attributed)
        # Immutable common tiles + two disjoint pose regions; swap a hidden map.
        code+=f'void jam_front_frame_{index}(unsigned char buffer) BANKED {{\nunsigned char old=LCDC_REG;VBK_REG=1;set_bkg_data(buffer?{len(common)+dynamic_max-128}:{len(common)-128},{len(dynamic)},dynamic);VBK_REG=0;\n'
        code+='if(!jam_cam_upload){LCDC_REG|=LCDCF_WIN9C00;set_win_tiles(0,0,20,18,map0);VBK_REG=1;set_win_tiles(0,0,20,18,attrs0);VBK_REG=0;return;}\n'
        code+='jam_cam_map_target=(old&LCDCF_WIN9C00)?0:1;LCDC_REG=(old&~LCDCF_BG9C00)|(jam_cam_map_target?LCDCF_BG9C00:0);set_bkg_tiles(0,0,20,18,buffer?map1:map0);VBK_REG=1;set_bkg_tiles(0,0,20,18,buffer?attrs1:attrs0);VBK_REG=0;LCDC_REG=(LCDC_REG&~LCDCF_BG9C00)|(old&LCDCF_BG9C00);wait_vbl_done();LCDC_REG=(LCDC_REG&~LCDCF_WIN9C00)|(jam_cam_map_target?LCDCF_WIN9C00:0);}\n'
    (CORE/f'jam_front_frame_{index}.c').write_text(code)
    manifest.append({'index':index,'name':name,'tiles':len(tiles),'palette_error':error})
def words(palettes):
    result=[]
    for family in palettes:
        for c in family:
            r,g,b=[round(v*31/255) for v in c];result.append(r|(g<<5)|(b<<10))
    return result
palette_words=words(PAL)
home_words=words(HOME_PAL)
card_words=words(CARD_PAL)
code='/* Generated native background art; tiles 0-127 in each VRAM bank. */\n#pragma bank 255\n#include <gbdk/platform.h>\n#include "palette.h"\n#include <string.h>\n'
code+='#include "jam_dunk_cam.h"\nvoid jam_front_common(void) BANKED;\n'
code+=''.join(f'void jam_front_frame_{i}({"unsigned char buffer" if 8<=i<20 else "void"}) BANKED;\n' for i in range(len(images)))
code+='static const unsigned short palettes[]={'+','.join(map(str,palette_words))+'};\n'
code+='static const unsigned short home_palettes[]={'+','.join(map(str,home_words))+'};\n'
code+='static const unsigned short card_palettes[]={'+','.join(map(str,card_words))+'};\n'
code+='static const unsigned short chompa_palettes[]={'+','.join(map(str,words(CHOMPA_PAL)))+'};\n'
for player_id in (1,2,3,5):
    code+=f'static const unsigned short card_palettes_{player_id}[]={{'+','.join(map(str,words(card_palettes[card_frames[player_id]])))+'};\n'
code+='static const unsigned short dark[]={'+','.join([str(palette_words[0])]*28)+'};\n'
code+='void jam_front_art(unsigned char frame) BANKED {unsigned char buffer=jam_cam_upload==1?1:0,intro=frame>=8 && frame<20,transition=!intro || !jam_cam_upload;if(transition){memcpy(BkgPalette,dark,sizeof(dark));set_bkg_palette(0,7,dark);wait_vbl_done();}if(intro && !jam_cam_upload)jam_front_common();switch(frame){\n'
code+=''.join(f'case {i}:jam_front_frame_{i}({"buffer" if 8<=i<20 else ""});break;\n' for i in range(len(images)))
palette_select=''.join(f'frame>={card_frames[p]} && frame<{card_frames[p]+4}?card_palettes_{p}:' for p in (1,2,3,5))
code+='}if(!intro)jam_cam_upload=0;else jam_cam_upload=buffer+1;if(transition){const unsigned short *active='+palette_select+'frame<3?home_palettes:(frame>=20?chompa_palettes:(frame>=4 && frame<8?card_palettes:palettes));wait_vbl_done();memcpy(BkgPalette,active,sizeof(palettes));set_bkg_palette(0,7,active);}}\n'
code+='void jam_front_card(unsigned char player,unsigned char stage) BANKED {\n'
code+='static const unsigned char frames[]={'+','.join(str(card_frames[i]) for i in range(6))+'};\n'
code+='jam_front_art(frames[player]+stage);}\n'
(CORE/'jam_front_art.c').write_text(code)
(OUT/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
print(f'Intro double buffer: {len(common)} common + 2 x {dynamic_max} dynamic = {len(common)+2*dynamic_max}/256 BG tiles')
print(json.dumps(manifest))
