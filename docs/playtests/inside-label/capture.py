"""Capture the scouting cards on the exact ROM after renaming DUNK to INSIDE."""
import sys
import numpy as np
from PIL import Image
from pyboy import PyBoy
pb = PyBoy(sys.argv[1], window="null", cgb=True); pb.set_emulation_speed(0)
def tap(b, wait=40):
    pb.button_press(b); pb.tick(3, False); pb.button_release(b); pb.tick(wait, False)
pb.tick(600, False); tap("start", 60); tap("a", 60)
shots = []
def grab(): pb.tick(2, True); shots.append(Image.fromarray(np.asarray(pb.screen.ndarray)[:, :, :3].copy()))
for player in range(6):
    grab()                 # player card
    tap("select"); grab()  # ratings 1 of 2 (INSIDE row)
    tap("select"); tap("select")
    tap("right")
sheet = Image.new("RGB", (160 * 6, 144 * 2))
for i, s in enumerate(shots): sheet.paste(s, ((i // 2) * 160, (i % 2) * 144))
sheet.resize((sheet.width * 2, sheet.height * 2), Image.NEAREST).save(sys.argv[2])
