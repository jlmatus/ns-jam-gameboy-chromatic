# Card portrait and shooting balance verification

## Changes

MATUS v2 rebalances eye spacing, cheek width and jaw shape on the existing 32×32 cell grid. The approved v1 and its references remain intact; v2 is integrated for user review. Other portrait sources and numeric ratings are unchanged.

The card background direction is **approved by the user**: dark navy courtside with teal and gold accents. The implementation uses a quiet teal portrait panel and dividers, gold face rails, and cream text. No texture competes with faces or rating bars. All six cards fit within 111 of the 128 reserved window-tile slots across the two VRAM banks. This background approval does not mark the revised MATUS portrait as approved.

The previous human shot roll used `byte % 100`. A nominal 52% attempt could make about 61% before defensive blocks. All shots now use the existing unbiased percentage sampler. Three-point rating has a curved contribution and shots beyond 100 horizontal court pixels lose one chance point per additional three pixels. Warm-up still reduces opponent pressure; it does not inflate the controlled shooter’s accuracy.

## Verification

- `sh scripts/test.sh` passes with address/undefined-behavior sanitizers and warnings as errors. Existing passing, control transfer, inbounds, contact and difficulty checks pass.
- Across 65,534 nonzero seeds per player, timed open threes from 90 pixels make MAGIC 28,815 (44%), CHOMPA 42,611 (65%), MATUS 53,744 (82%). Tests also check deeper range, close finishing, timing and defensive pressure. These are isolated simulation probabilities, not observed match shooting percentages.
- Official native ROM: `77da756f7a9150e532842a5e8393fa714c0cb17b486e72ef95fcd63aff97a60c`. Genuine PyBoy frames verify all six cards, both MATUS rating pages, and a Warm-up MAGIC gather/release, made three and opponent baseline inbound.
- **Evidence correction:** the recording’s original stop reason mistakenly calls the MAGIC attempt a miss/rebound. It made a three. The retained frames, state and this correction take precedence; that clip does not establish missed-shot behavior or shooting accuracy.
- Actual exported sprite allocation remains 188/192 per reserved bank span. The native build succeeds with the existing CLI deprecation/optimizer warnings.
- Official browser export `aa8e2e23d7244fdab2429e915660b2da8446e6417ec8dfde0e91a5b4513f7258` opens in the existing owned tab. Arrow navigation MAGIC → MATUS and the revised card were verified visually. The prior browser game was saved before replacement; the preview manager needed an owned-process restart after an unresolved idle close handshake. No recording journal or save was deleted.

See [evidence JSON](cards-shooting-v2-evidence.json), [host results](cards-shooting-v2/host-tests.txt), [native shot/inbound clip](cards-shooting-v2/magic-three-inbound.gif), and [browser card](cards-shooting-v2/browser-card.png). This pass does not establish final game feel, physical hardware performance or complete match rules.
