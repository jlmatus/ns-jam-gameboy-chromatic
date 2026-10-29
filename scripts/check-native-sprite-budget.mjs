// Run after `sh scripts/build.sh export`: validate the actual loader's allocations.
import fs from 'node:fs';
const root='game/build/source/src/data/';
const read=name=>fs.readFileSync(root+name+'.c','utf8');
const scene=read('scene_managua_court');
const player=scene.match(/\.player_sprite = TO_FAR_PTR_T\((\w+)\)/)[1];
const sprites=[...read('scene_managua_court_sprites').matchAll(/TO_FAR_PTR_T\((\w+)\)/g)].map(m=>m[1]);
let allocation=0;
for(const sprite of [player,...sprites]) {
  const source=read(sprite);
  const counts=['tileset','cgb_tileset'].map(field=>{
    const m=source.match(new RegExp('\\.'+field+' = TO_FAR_PTR_T\\((\\w+)\\)'));
    return m?Number(read(m[1]).match(/\.n_tiles = (\d+)/)[1]):0;
  });
  const used=Math.max(...counts);
  allocation+=used;
  if(Number(source.match(/\.n_metasprites = (\d+)/)[1])>255)throw Error('Metasprite index exceeds byte range');
  console.log(`${sprite}: base ${allocation-used}, ${used} tiles per reserved bank span`);
}
if(allocation>192)throw Error(`Sprite loader uses ${allocation} tiles, crossing reserved UI tile 192`);
console.log(`PASS: loader allocation ${allocation}/192; UI tiles 192–255 remain separate`);
