# Scouting Card Design QA

Source visual truth: `docs/design/selection-layout-review/concept-2-scouting-card.png`
(second displayed option, selected by the user). Implementation:
`docs/playtests/scouting-cards/matus-0.png`, plus browser proof
`docs/playtests/scouting-cards/browser-main.png` and `browser-ratings.png`.

Viewport: native Game Boy Color, **160×144**. The 1322×1190 generated reference
and genuine 160×144 native frame are normalized to 640×576 each in
`docs/playtests/scouting-cards/reference-and-native.png`. They share the same
MATUS main-card state. Browser proof includes its surrounding device frame;
that frame is not treated as game content. Browser viewport was 632×959 during QA.
No CSS viewport or responsive web app is substituted for the native game.

## Comparison history

1. **[P1, fixed] Enlarged-name gaps.** Initial native text scaling omitted the
   horizontal subpixel offset, leaving vertical gaps. Corrected the native pixel
   generator and rebuilt. Final frames show solid double-size names.
2. **[P2, fixed] Hair lost against backdrop.** The early native card mapped
   transparent backdrop cells to the same ink color as hair. Contrast was lower
   than the reference and the approved source silhouette was lost. Native palette
   families now give those background cells a light backdrop while retaining
   every approved opaque cell and its color. CHOMPA retains the green beanie.
   The before-contrast comparison remains retained for this finding.
3. **Post-fix comparison:** the final native card and selected concept were
   inspected together in `reference-and-native.png`. All 18 native card/detail
   views exactly match the deliberate native pixel layout and quantized palettes.
   Browser captures verify the main card, both details pages, browsing, A confirm,
   and B back. No console error/warning was reported in the checked browser log.

## Required fidelity surfaces

- **Fonts/typography:** bold native bitmap glyphs replace the thin font; names
  are 2× size. Native grid spacing and short footers prevent clipped controls.
  Glyphs stay sharp at native resolution. These are cartridge fonts, not a
  smooth desktop font placed over a raster mockup.
- **Spacing/layout:** selected two-column structure is retained: large portrait
  left, four skill labels/values/meters right, player identity above, playstyle
  below, A/B and SELECT controls at the bottom. The small stage caption clarifies
  whether the user is choosing themselves, a teammate, or an opponent.
- **Colors/tokens:** cream/navy/amber hierarchy follows the selected concept,
  quantized to RGB555. The light portrait backdrop is an intentional contrast
  correction. Stock UI palette 7 is preserved; menu palettes are independent.
- **Image quality:** the approved 32×32 portraits use exact 2× source-cell
  placement, yielding 64×64 portraits. The generated concept's altered facial
  detail and gradients are intentionally not copied over approved artwork.
  No blurred resizing, screenshot-as-menu, CSS/vector substitutes, or redesigned
  facial cells are used. One CHOMPA tile fills only transparent cells with ink
  to meet the four-color tile limit; opaque cells are untouched.
- **Copy/content:** the user approved `3PT PLAYMAKER`, `RIM PROTECTOR`,
  `POWER FINISHER`, `SPEED SLASHER`, `STRETCH BIG`, and `TWO-WAY SLASHER`.
  This replaces the concept's “TOP OF THE KEY” flavor wording. All eight numeric
  ratings remain exact; the generator checks source and featured values.

The full-view comparison is readable at the common size, including names,
ratings, meters, and controls, so no additional cropped comparison was needed.

## Interaction and runtime checks

- Host checks pass all 1,080 distinct lineup/difficulty combinations and the new
  three-page card cycle, held SELECT, B close/back, A pick, and page-reset cases.
- Exact native ROM tests cover all six cards and 12 rating pages, starting a
  reviewed lineup, restored court/body/HUD tiles, movement, pause, and return.
- Official browser build opens and handles arrows, P, X, and Z correctly.
- The previous human practice was checkpointed before preview service replacement.

No actionable P0/P1/P2 issue remains in this selected layout adaptation.
Physical handheld readability remains a user/hardware validation step; screenshot
matching is not a hardware certification. Further gameplay bugs are outside this pass.

final result: passed
