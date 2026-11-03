# Other-player portrait review

Review only: no roster artwork, approval status, game behavior or live browser
state was changed. MATUS v4 and the locked Night Court remain intact.

[Full comparison](all-characters.png) shows, left to right, a crop of the original
photo, the original generated portrait study and an actual current cartridge card
enlarged exactly 2×. Magenta in the middle column belongs to the original study;
that column is not presented as native game art.

| Player | Photo reference | Current portrait | Individual comparison |
|---|---|---|---|
| MAGIC #32 | Supplied 2026-10-28 08:45 screenshot | `magic-v1` | [Review](magic-comparison.png) |
| BAMBI #23 | Supplied 2026-10-27 17:45 screenshot | `bambi-v1` | [Review](bambi-comparison.png) |
| CHRUX #7 | Supplied 2026-10-28 08:47 screenshot | `chrux-v1` | [Review](chrux-comparison.png) |
| CHOMPA #74 | Supplied closer 2026-10-28 09:04 beanie photo | `chompa-v2` | [Review](chompa-comparison.png) |
| MEAT #00 | None supplied; approved invented appearance | `meat-v1` | [Review](meat-comparison.png) |

All five still use 32px portrait sources in the older card layout, rather than
MATUS's new 96px source-faithful card. The comparison exposes both a possible
photo-to-study likeness issue and detail lost during study-to-card conversion.
Review those separately before changing an approved face.

Main points to inspect: MAGIC's glasses/smile and face length, BAMBI's cheek and
chin proportions, CHRUX's full beard and head angle, and CHOMPA's glasses,
hooded eyes, tilted smile and stubble. MEAT can be reviewed for style and
distinctiveness, but there is no supplied photo against which to assess likeness.

The card images come from current ROM SHA-256
`f1991756fd48f8b08e1880a3300ff78d3bc7c6ba644436e30fde4026fb83edd5`.
Recorded PyBoy session `b6c41b992b8342fc97e19b5ac556625b`,
`game/build/roster-review`, finalized passed at frame 563. [Frame/input receipts](evidence.json)
and [photo/crop sources](sources.json) retain provenance. No game rebuild or
physical-device operation was needed for this read-only review.
