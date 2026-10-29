// Pack approved 32x32 whole-cell portraits into native 2bpp window tiles.
// No resampling or edits to the approved source cells; empty cells use outline color.
import fs from 'node:fs';
const paths=['matus-v1','magic-v1','bambi-v1','torres-v1','chompa-v2','meat-v1'];
const packed=[], palettes=[];
for(const path of paths) {
  const p=JSON.parse(fs.readFileSync(`art/portraits/${path}/pixels.json`,'utf8'));
  const keys=['#','s','o','g'];
  const map={'.':0,'#':0,s:1,o:2,g:3};
  const cells=Array.from({length:32},()=>Array(32).fill('.'));
  p.rows.forEach(([offset,row],y)=>[...row].forEach((c,x)=>cells[y][offset+x]=c));
  const bytes=[];
  for(let ty=0;ty<4;ty++)for(let tx=0;tx<4;tx++)for(let y=0;y<8;y++) {
    let lo=0,hi=0;
    for(let x=0;x<8;x++){const v=map[cells[ty*8+y][tx*8+x]];lo|=(v&1)<<(7-x);hi|=(v>>1)<<(7-x);}
    bytes.push(lo,hi);
  }
  packed.push(bytes);
  palettes.push(keys.map(k=>{const c=p.palette[k]||p.palette['#'];const rgb=c.slice(1,7).match(/../g).map(v=>Math.round(parseInt(v,16)*31/255));return rgb[0]|rgb[1]<<5|rgb[2]<<10;}));
}
fs.writeFileSync('game/plugins/basketball/engine/include/jam_portraits.h',`/* Generated from approved approved portrait pixel JSON by scripts/create-roster-portraits.mjs. */\nstatic const unsigned char jam_portrait_tiles[6][256] = {\n${packed.map(p=>`{${p.join(',')}}`).join(',\n')}\n};\nstatic const unsigned short jam_portrait_palettes[6][4] = {${palettes.map(p=>`{${p.join(',')}}`).join(',')}};\n`);
console.log('Packed all six approved portraits, including full-color CHOMPA v2.');
