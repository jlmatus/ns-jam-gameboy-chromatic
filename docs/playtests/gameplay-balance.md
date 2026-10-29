# Gameplay Balance and Facing Pass

Addresses the user's feedback after accepting the scouting cards: frequent
takeaways, opponents appearing unable to miss, and front-facing court bodies.
Generous human shooting on Warm-up is deliberately preserved.

## Behavior

- Human/team catches and rebounds receive 90 updates of steal protection on
  Warm-up and 60 on other levels. Inbounds retain 120 updates.
- Opponents guard with more space and need closer contact for a swipe. Warm-up
  has lower swipe probability and a longer retry cooldown. Steal and power
  ratings still influence outcomes.
- Pass interceptions are probabilistic: one attempt per defender per pass.
  Warm-up reduces reach/probability and protects release/arrival frames. An
  intended receiver in catch range receives the arrival catch instead of a
  failed lane attempt turning into an automatic second interception.
- CPU shots no longer receive a free perfect-release bonus. Opponent shot
  probability is capped at 58%/80%/92% by level. New opponent shot/swipe rolls
  reject incomplete random buckets; human shot formulas and rolls are unchanged.
- Existing 16×32 frames now show side profiles, directional noses/eyes, and
  hands raised toward the basket. Idle defenders turn toward the ball. Jerseys,
  frame layout, stable IDs, approved portraits, and card ratings are retained.

## Validation

`sh scripts/test.sh` passes with warnings as errors, ASan, and UBSan. In 512 seeded
identical scenarios, Warm-up/Winners Stay produced **24/173 swipes** and
**78/163 lane interceptions**. Warm-up open CPU shots made **295/512**. These are
controlled scenarios, not observed match-wide rates. Checks include all six
opponent accuracy ceilings, protected catches, unchanged human shot chances,
rating differentiation, 1,080 lineups, and multi-level soak tests.

Final native ROM: 262,144 bytes, valid GBC-only header/logo; SHA-256
`dac2a5bd266a02c9db5363a154b8e326bdd717aa01930cbf7cc93acff6899c16`.
The official build succeeded with the existing Node deprecation and three SDCC
optimizer warnings. PyBoy 2.7.0 retained inputs/captures through frame **4786**:
Warm-up lineup, pass/call-back, dribbling without an instant takeaway, movement
in both directions, missed shots/rebounds, and both teams scoring. At frame 4786
the score was 2–2, with 10 shots, eight rebounds, and two baskets. This short run
does not establish final balance. The finalized recording outcome is passed.

Source sprite analysis found 142 distinct 8×8 tiles and one palette family, with
no source violations. Scene metadata alone cannot establish compiled sprite
allocation; the actual ROM rendered the new frames. A crowded OAM sample had
19 visible objects and a maximum of nine on one scanline (hardware limit ten).
This single sample is not a worst-case or physical-device performance claim.

Evidence: `gameplay-balance-evidence.json`, native captures in
`gameplay-balance/`, and the preserved source comparison in `art/prototype-v2/`.
The superseded first iteration recording is retained as needs-review.

## Browser Preview

The separate official web export succeeded, SHA-256
`fed4a6d3d63ca715804166494fd8721455910df08d632a8efa633fd669318129`.
The built-in browser verified selection, Warm-up review, starting practice,
movement, side profiles, and native SELECT pause. The previous human session
was preserved before replacement; the checked browser console had no errors or
warnings. The preview is left on Warm-up, paused in-game; P resumes. The session
was explicitly saved at frame 60991, state
`e023cb4caf890dc27a8119626a23968ad10f08e6ddbc0892f582a83bc15fb1e6`.
The old service subsequently reported an unclean close; the acknowledged state
save is retained and was not represented as a clean service-close receipt.

Ratings already affect movement, shooting, passing, defense, contact, and clutch.
Playstyle labels do not add special moves. Animated dunks, Dunk Cam, fire streaks,
four-quarter match flow, final uniforms, sound, and saved records remain planned.
