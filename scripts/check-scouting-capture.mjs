import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import {createRequire} from 'node:module';
const require=createRequire(path.join(process.env.JAM_PNGJS_ROOT||path.join(os.homedir(),'.cache/codex-runtimes/codex-primary-runtime/dependencies/node'),'package.json'));
const {PNG}=require('pngjs');
const spec=JSON.parse(fs.readFileSync('art/ui/scouting-cards.json','utf8'));
const evidence=JSON.parse(fs.readFileSync('docs/playtests/scouting-card-evidence.json','utf8'));
const quant=hex=>hex.slice(1,7).match(/../g).map(v=>Math.round(parseInt(v,16)*31/255)<<3);
const checks=[];fs.mkdirSync('docs/playtests/scouting-cards',{recursive:true});
for(let player=0;player<6;player++)for(let page=0;page<3;page++){
 const frame=player===0?[120,144,168][page]:[216,240,264][page]+(player-1)*96;
 const sample=evidence.samples.find(s=>s.frame===frame);if(!sample)throw Error(`Missing frame ${frame}`);
 const retained=`docs/playtests/scouting-cards/${spec.players[player].name.toLowerCase()}-${page}.png`;
 const source=fs.existsSync(sample.path)?sample.path:retained;
 const img=PNG.sync.read(fs.readFileSync(source));
 const stencil=JSON.parse(fs.readFileSync(`game/build/scouting-pixels/${spec.players[player].name.toLowerCase()}-${page}.json`,'utf8'));

 const colors=Object.fromEntries(Object.entries(stencil.colors).map(([slot,colors])=>[slot,colors.map(quant)]));
 let mismatches=0,maxError=0;
 for(let y=0;y<144;y++)for(let x=0;x<160;x++){
  const rgb=colors[stencil.pal[y>>3][x>>3]][stencil.pixels[y][x]];
  const error=Math.max(...rgb.map((v,k)=>Math.abs(v-img.data[(y*160+x)*4+k])));
  maxError=Math.max(maxError,error);if(error>0)mismatches++;
 }
 const dest=`docs/playtests/scouting-cards/${spec.players[player].name.toLowerCase()}-${page}.png`;if(source!==dest)fs.copyFileSync(source,dest);
 checks.push({player:spec.players[player].name,page,frame,source:sample.path,retained:dest,mismatches,maxError});
 if(maxError>8)throw Error(`Native card mismatch ${checks.at(-1).player} page ${page}: ${mismatches}, max ${maxError}`);
}
evidence.selectionChecks=checks;fs.writeFileSync('docs/playtests/scouting-card-evidence.json',JSON.stringify(evidence,null,2)+'\n');
console.log(JSON.stringify(checks.map(({player,page,frame,mismatches,maxError})=>({player,page,frame,mismatches,maxError})),null,2));
const ref=PNG.sync.read(fs.readFileSync('docs/design/selection-layout-review/concept-2-scouting-card.png'));
const native=PNG.sync.read(fs.readFileSync('docs/playtests/scouting-cards/matus-0.png'));
const out=new PNG({width:1320,height:576});
for(let y=0;y<576;y++)for(let x=0;x<1320;x++)out.data.set([255,255,255,255],(y*1320+x)*4);
function copy(img,x0,w,h){for(let y=0;y<h;y++)for(let x=0;x<w;x++){
 const src=(Math.floor(y*img.height/h)*img.width+Math.floor(x*img.width/w))*4;
 out.data.set(img.data.subarray(src,src+4),(y*1320+x0+x)*4);
}}
copy(ref,0,640,576);copy(native,680,640,576);
fs.writeFileSync('docs/playtests/scouting-cards/reference-and-native.png',PNG.sync.write(out));
