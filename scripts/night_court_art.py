"""Native 160x144 reconstruction of the approved Night Court composition.

All shapes/clusters are authored at final resolution, never sampled from the
concept sheet. Independent palettes leave the existing intro/cards unchanged.
"""
from PIL import Image, ImageDraw

N = '#081C29'
DEEP = '#103443'
TEAL = '#28606B'
BLUE = '#385966'
DUSK = '#79716B'
COPPER = '#B57749'
GOLD = '#F0AA39'
CREAM = '#FFF0BC'
SHADE = '#9D6029'
ORANGE = '#DF782C'
BROWN = '#743B24'

FAMILIES = [
    [N, BLUE, GOLD, CREAM],
    [N, TEAL, GOLD, CREAM],
    [N, BLUE, DUSK, COPPER],
    [N, TEAL, COPPER, GOLD],
    [N, DEEP, TEAL, GOLD],
    [N, BROWN, ORANGE, GOLD],
    [N, TEAL, GOLD, CREAM],
]

LOGO = {
 'N':['1100011','1110011','1111011','1111011','1101111','1101111','1100111','1100011','1100011'],
 'S':['0111111','1111111','1100000','1100000','0111110','0000011','0000011','1111111','1111110'],
 'J':['1111111','1111111','0000110','0000110','0000110','0000110','1100110','1111110','0111100'],
 'A':['0011100','0111110','1110111','1100011','1100011','1111111','1111111','1100011','1100011'],
 'M':['1100011','1110111','1111111','1101011','1101011','1100011','1100011','1100011','1100011'],
}

def palm(d, x, y, flip=False):
    f=-1 if flip else 1
    d.polygon([(x-2,y+48),(x+2,y+48),(x+5*f,y+16),(x+3*f,y)],fill=N)
    for dx,dy in [(-16,3),(-13,-7),(-3,-12),(10,-9),(19,-1),(14,9),(-10,12)]:
        ex=x+dx*f
        d.polygon([(x,y),(x+dx*f//2,y+dy//2-3),(ex,y+dy),(ex-3*f,y+dy+3),(x+dx*f//2,y+dy//2)],fill=N)
    d.line((x-1,y+28,x+f,y+13),fill=DEEP)

def wordmark(im):
    mask=Image.new('1',im.size)
    md=ImageDraw.Draw(mask)
    # 127-pixel custom block logo: broad strokes and a staggered baseline.
    for char,x,y in [('N',17,9),('S',41,8),('J',76,8),('A',100,9),('M',124,11)]:
        for yy,row in enumerate(LOGO[char]):
            for xx,c in enumerate(row):
                if c=='1':md.rectangle((x+xx*3,y+yy*3,x+xx*3+2,y+yy*3+2),fill=1)
    d=ImageDraw.Draw(im)
    # Deep extrusion, then warm bevels; no fine font strokes in the title.
    for off in range(5,0,-1):
        im.paste(N,(-off,off),mask)
    for off in (3,2,1):im.paste(TEAL,(-off,off),mask)
    im.paste(GOLD,(0,0),mask)
    for y in range(7,40):
        for x in range(160):
            if not mask.getpixel((x,y)):continue
            if x<65 or y==0 or not mask.getpixel((x,y-1)):
                d.point((x,y),fill=CREAM)
            elif y>30 and mask.getpixel((x,max(0,y-3))):
                d.point((x,y),fill=GOLD)

def home(selected, text):
    im=Image.new('RGB',(160,144),N)
    d=ImageDraw.Draw(im)
    # Banded dusk sky. Broken cloud clusters repeat instead of random noise.
    for y,c in [(32,DEEP),(40,BLUE),(56,DUSK),(64,COPPER),(72,GOLD)]:
        d.rectangle((0,y,159,79),fill=c)
    for x,y,w in [(0,39,23),(31,43,14),(118,40,22),(144,47,16),(18,52,23),
                  (58,55,12),(93,50,22),(4,62,24),(36,66,17),(112,61,25),(135,68,25)]:
        d.line((x,y,x+w,y),fill=BLUE if y<56 else DUSK,width=2)
        d.line((x+3,y+2,x+w-3,y+2),fill=DEEP if y<48 else COPPER)
    # Broad volcanic silhouette, visible above the city and below the logo.
    d.polygon([(46,81),(63,75),(76,69),(89,63),(96,60),(101,62),(106,62),
               (114,67),(125,71),(139,77),(154,81)],fill=N)
    d.line((66,75,79,69,93,64,97,64),fill=TEAL)
    d.line((102,66,108,69,111,69),fill=TEAL)
    # Skyline is in front of the mountain: varied roofs, antennae, warm windows.
    for x,y,w in [(0,78,7),(9,74,7),(18,77,5),(25,75,10),(37,80,7),
                  (47,77,6),(55,79,11),(68,75,5),(75,78,9),(86,73,7),
                  (95,77,10),(108,74,5),(116,79,7),(126,76,9),(138,78,7),(148,73,12)]:
        d.rectangle((x,y,x+w,87),fill=N)
        if w>=7:d.line((x+2,y-1,x+w-2,y-1),fill=N)
        for wy in range(y+3,86,4):
            for wx in range(x+2,x+w-1,4):
                d.rectangle((wx,wy,wx+1,wy+1),fill=GOLD if (wx+wy)%3 else TEAL)
    # Fence stays subordinate to the horizon; crossed mesh is a repeated tile.
    d.rectangle((0,88,159,94),fill=N)
    for y in range(88,94):
        for x in range(160):
            if (x+y)%4==0 or (x-y)%4==0:d.point((x,y),fill=DEEP)
    d.line((0,87,159,87),fill=TEAL)
    d.line((0,94,159,94),fill=TEAL)
    for x in (0,32,64,96,128,159):d.line((x,84,x,95),fill=N)
    # Dark reflective court, with horizontal glints and perspective markings.
    d.rectangle((0,95,159,111),fill=DEEP)
    for y in (96,99,102,105,108,111):
        for x in range((y*3)%17-9,160,19):
            d.line((x,y,x+10,y),fill=TEAL)
    d.line((0,97,159,97),fill=GOLD)
    d.arc((97,97,172,108),0,359,fill=GOLD)
    d.arc((-35,102,106,129),180,355,fill=GOLD)
    # Lamp reflections are interrupted bands, suggesting wet asphalt.
    for x in (18,148):
        for y,w in [(98,2),(100,3),(103,2),(106,4),(109,3)]:
            d.line((x-w,y,x+w,y),fill=GOLD)
    # Street lamps frame the scene instead of competing with the volcano.
    for x,y in [(18,54),(148,55)]:
        d.line((x+1,y+3,x+1,97),fill=N,width=2)
        d.rectangle((x-3,y-3,x+3,y+3),fill=COPPER)
        d.rectangle((x-2,y-2,x+2,y+2),fill=GOLD)
        d.line((x-1,y-1,x+1,y-1),fill=CREAM)
    palm(d,3,45)
    palm(d,157,43,True)
    # Backboard and white net at the right, with a thin gold rim.
    d.line((130,78,130,99),fill=N,width=3)
    d.polygon([(120,69),(140,67),(140,79),(120,81)],fill=N,outline=GOLD)
    d.line((122,71,137,70,137,77),fill=TEAL)
    d.rectangle((125,74,132,78),outline=CREAM)
    d.line((124,80,132,80),fill=GOLD)
    d.line((124,81,126,86,130,86,132,81),fill=CREAM)
    d.line((126,81,129,85,130,81,127,85),fill=CREAM)
    # Foreground ball with a round silhouette, lit edge and curved seams.
    d.ellipse((36,107,63,111),fill=N)
    d.ellipse((39,91,61,112),fill=N)
    d.ellipse((40,91,60,111),fill=BROWN)
    d.ellipse((42,91,60,109),fill=ORANGE)
    d.arc((42,92,59,108),195,280,fill=GOLD,width=2)
    d.arc((46,89,58,112),90,260,fill=N)
    d.arc((35,97,65,110),200,340,fill=N)
    d.line((48,92,45,96,44,102,46,108),fill=N)
    d.line((58,94,58,100,55,105,52,109),fill=N)
    wordmark(im)
    # Menu matches the concept's quiet navy footer; no border through the scene.
    d.rectangle((0,113,159,143),fill=N)
    for i,name in enumerate(['PLAY GAME','HOW TO PLAY','REPLAY INTRO']):
        text(im,38,115+i*10,name,GOLD if i==selected else CREAM)
    text(im,26,115+selected*10,'>',GOLD)
    return im
