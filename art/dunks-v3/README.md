# Signature dunk studies (v3)

`scripts/dunk-poses.mjs` gives each player their own keyframes (`dunkStyles`). All six use the same 64-update timeline as `jam_dunk_update()`: the ball reaches the rim at update 28 and blocks happen in the same window. Only the art differs; ratings, scoring and blocks are unchanged.

| Player | Dunk | Notes |
|---|---|---|
| MATUS | Cradle jam | Ball swings low and back, then up for a one-hand flush. |
| MAGIC | Windmill | Straight arm circles front, down, back and over. |
| BAMBI | Two-hand power slam | Tucked knees; double impact shake, rim rattle and landing dust. |
| CHRUX | Reverse jam | Drifts under the rim (`reach: 2`) and slams back over his head. |
| CHOMPA | Two-hand rim hang | Hangs until update 44 with swinging legs; art only, after the score. |
| MEAT | 360 spin | Front, profile, back and mirrored-profile heads, with motion arcs. |

`node scripts/create-dunk-art.mjs` writes the 504 banked frames, this folder's manifest and the PNG previews (made and blocked). The previews omit the live OBJ ball and are not emulator captures. The busiest frame uses 124 of the 128 cinematic tiles. The v1 and v2 studies are preserved.
