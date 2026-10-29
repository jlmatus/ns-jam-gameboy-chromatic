// Deliberately authored native pixels; approved portrait studies are untouched.
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { createRequire } from 'node:module';
import {BODY_W, BODY_H, POSES, bodyCells, poseNames} from './player-poses.mjs';
const pngRoot = process.env.JAM_PNGJS_ROOT || path.join(os.homedir(), '.cache/codex-runtimes/codex-primary-runtime/dependencies/node');
const require = createRequire(path.join(pngRoot, 'package.json'));
const { PNG } = require('pngjs');
const source = ['#e0f8cf','#86c06c','#306850','#071821'];
const rgb = c => c.slice(1).match(/../g).map(v=>parseInt(v,16));
function raster(w,h,fill) {
  const p = new PNG({width:w,height:h});
  const put=(x,y,c)=>{if(x<0||y<0||x>=w||y>=h)return;const i=(y*w+x)*4;p.data.set([...rgb(c),255],i);};
  for(let y=0;y<h;y++)for(let x=0;x<w;x++)put(x,y,fill);
  return {p,put,save:path=>fs.writeFileSync(path,PNG.sync.write(p))};
}
const bg=raster(400,144,source[3]);
const physicalPut=bg.put;
bg.put=(x,y,c)=>{for(let xx=Math.floor(x*5/4);xx<Math.floor((x+1)*5/4);xx++)physicalPut(xx,y,c);};
function line(x0,y0,x1,y1,c) {
  let dx=Math.abs(x1-x0),sx=x0<x1?1:-1,dy=-Math.abs(y1-y0),sy=y0<y1?1:-1,e=dx+dy;
  for(;;){bg.put(x0,y0,source[c]);if(x0===x1&&y0===y1)break;const q=2*e;if(q>=dy){e+=dy;x0+=sx;}if(q<=dx){e+=dx;y0+=sy;}}
}
function ellipse(cx,cy,rx,ry,c) {for(let a=0;a<720;a++){const t=a*Math.PI/360;bg.put(Math.round(cx+Math.cos(t)*rx),Math.round(cy+Math.sin(t)*ry),source[c]);}}
const skew=y=>Math.floor((y-88)/6);
for(let y=32;y<128;y++)for(let x=16+skew(y);x<=303+skew(y);x++)bg.put(x,y,source[1]);
for(let y=38;y<=124;y++)for(let x=24+skew(y);x<=295+skew(y);x++)bg.put(x,y,source[((x+32)%40===0 && y%3===0)?2:1]);
for(let x=24;x<296;x++){bg.put(x+skew(40),40,source[0]);bg.put(x+skew(124),124,source[0]);}
line(24+skew(40),40,24+skew(124),124,0);line(295+skew(40),40,295+skew(124),124,0);
line(160+skew(40),40,160+skew(124),124,0);ellipse(160,84,22,17,0);
for(const [x,s] of [[24,1],[295,-1]]) {
  for(let y=67;y<112;y++)for(let n=1;n<48;n++)bg.put(x+s*n+skew(y),y,source[2]);
  line(x+skew(67),67,x+s*48+skew(67),67,0);
  line(x+skew(112),112,x+s*48+skew(112),112,0);
  line(x+s*48+skew(67),67,x+s*48+skew(112),112,0);
  ellipse(x+s*48,89,12,22,0);
  for(let y=40;y<=124;y++) {const dy=(y-88)/44;if(Math.abs(dy)<=1){const px=x+s*16+s*Math.round(88*Math.sqrt(1-dy*dy))+skew(y);bg.put(px,y,source[0]);}}
}
// Less empty grandstand; sideline lettering and rails provide a horizontal reference.
for(let y=6;y<26;y+=7){line(0,y,319,y,2);for(let x=4;x<320;x+=12){bg.put(x,y-2,source[0]);bg.put(x+1,y-3,source[1]);}}
line(0,28,319,28,0);
// Thin angled backboards face IN toward each other. Rim center is (40/280,54).
for(const [rim,board,s] of [[40,24,1],[280,296,-1]]) {
  for(let y=20;y<=47;y++)for(let n=0;n<5;n++)bg.put(board+s*n,y,source[n===0?0:3]);
  line(board,20,board+s*5,23,0);line(board+s*5,23,board+s*5,48,0);line(board,45,board+s*5,48,0);
  line(board,45,board-s*7,92,3);line(board-s*7,92,board-s*7,104,3);
  line(board+s*5,41,rim-s*5,43,3);
  ellipse(rim,44,8,3,3);line(rim-6,43,rim+6,43,0);
  // The central net is an animated native sprite so it reacts to a made basket.
}
bg.save('game/assets/backgrounds/court.png');
// Athletic poses use native 16x48 canvases with confirmed 36–42px standing silhouettes.
const bodies=raster(12*2*POSES*BODY_W,BODY_H,'#65ff00');
const pal={'.':'#65ff00','#':'#071821','o':'#e0f8cf','s':'#86c06c'};
const rows=bodyCells(0,0,false);
for(let group=0;group<12;group++)for(let dir=0;dir<2;dir++)for(let pose=0;pose<POSES;pose++) {
  const cells=bodyCells(group%6,pose,!!dir);
  const frame=group*2*POSES+dir*POSES+pose;
  for(let y=0;y<BODY_H;y++)for(let x=0;x<BODY_W;x++)bodies.put(frame*BODY_W+x,y,pal[cells[y][x]]);
}
bodies.save('game/assets/sprites/players.png');
for(const [name,rows] of [['ball',['..####..','.sooss#.','#ososs##','#soosss#','#sssoos#','#soosos#','.soss#..','..####..']],['marker',['...oo...','..oooo..','.oooooo.','..oooo..','...oo...','........']]]) {
  const a=raster(16,16,'#65ff00');
  rows.forEach((r,y)=>[...r].forEach((c,x)=>a.put(x,y+4,pal[c]||pal['.'])));
  a.save(`game/assets/sprites/${name}.png`);
}
// The close-up ball occupies the remaining four OBJ tiles; normal art stays exact.
{
 const small=PNG.sync.read(fs.readFileSync('game/assets/sprites/ball.png'));
 const ball=raster(32,16,pal['.']);
 for(let y=0;y<16;y++)for(let x=0;x<16;x++){
  const i=(y*16+x)*4;ball.put(x,y,'#'+[...small.data.subarray(i,i+3)].map(v=>v.toString(16).padStart(2,'0')).join(''));
  const dx=x-7.5,dy=y-7.5;
  if(dx*dx+dy*dy<=55)ball.put(16+x,y,dx*dx+dy*dy>39||x===7||y===7||Math.abs(x-y)===3?pal['#']:x<7&&y<7?pal.o:pal.s);
 }
 ball.save('game/assets/sprites/ball.png');
}
// Six readable jersey markers use one hardware object each.
const marker=raster(96,16,'#65ff00');
const markerDigits={
 '0':['111','101','101','101','111'],'1':['010','110','010','010','111'],
 '2':['110','001','010','100','111'],'3':['110','001','010','001','110'],
 '4':['101','101','111','001','001'],'7':['111','001','010','010','010']
};
['13','32','23','7','74','00'].forEach((jersey,id)=>{
 for(let y=5;y<12;y++)for(let x=0;x<8;x++)marker.put(id*16+x,y,pal.o);
 [...jersey].forEach((d,i)=>markerDigits[d].forEach((row,y)=>[...row].forEach((v,x)=>{
  if(v==='1')marker.put(id*16+(jersey.length===1?2:0)+i*4+x,y+6,pal['#']);
 })));
 marker.put(id*16+3,13,pal.o);marker.put(id*16+4,13,pal.o);marker.put(id*16+3,14,pal.o);
});
marker.save('game/assets/sprites/marker.png');
const net=raster(64,16,'#65ff00');
for(let pose=0;pose<4;pose++) {
  const length=pose===0?10:pose===1?14:12;
  for(let y=0;y<=length;y++) {
    const inset=y>length/2?1:0,shift=pose===2?Math.floor(y/5):pose===3?-Math.floor(y/6):0;
    for(let x=0;x<8;x++)if(x===inset+shift || x===7-inset+shift || (y%3===1 && (x+y)%3===0)) net.put(pose*16+x,y,pal['o']);
  }
  for(let x=0;x<8;x++)net.put(pose*16+x,0,pal['#']);
}
net.save('game/assets/sprites/net.png');
// Small fixed-width HUD font, retained as editable bit rows.
const glyphs={
' ':['00000','00000','00000','00000','00000','00000','00000'],
A:['01110','10001','10001','11111','10001','10001','10001'],B:['11110','10001','10001','11110','10001','10001','11110'],C:['01111','10000','10000','10000','10000','10000','01111'],D:['11110','10001','10001','10001','10001','10001','11110'],E:['11111','10000','10000','11110','10000','10000','11111'],F:['11111','10000','10000','11110','10000','10000','10000'],G:['01111','10000','10000','10111','10001','10001','01111'],H:['10001','10001','10001','11111','10001','10001','10001'],I:['11111','00100','00100','00100','00100','00100','11111'],J:['00111','00010','00010','00010','10010','10010','01100'],K:['10001','10010','10100','11000','10100','10010','10001'],L:['10000','10000','10000','10000','10000','10000','11111'],M:['10001','11011','10101','10101','10001','10001','10001'],N:['10001','11001','10101','10011','10001','10001','10001'],O:['01110','10001','10001','10001','10001','10001','01110'],P:['11110','10001','10001','11110','10000','10000','10000'],Q:['01110','10001','10001','10001','10101','10010','01101'],R:['11110','10001','10001','11110','10100','10010','10001'],S:['01111','10000','10000','01110','00001','00001','11110'],T:['11111','00100','00100','00100','00100','00100','00100'],U:['10001','10001','10001','10001','10001','10001','01110'],V:['10001','10001','10001','10001','10001','01010','00100'],W:['10001','10001','10001','10101','10101','11011','10001'],X:['10001','10001','01010','00100','01010','10001','10001'],Y:['10001','10001','01010','00100','00100','00100','00100'],Z:['11111','00001','00010','00100','01000','10000','11111'],
'0':['01110','10001','10011','10101','11001','10001','01110'],'1':['00100','01100','00100','00100','00100','00100','01110'],'2':['01110','10001','00001','00010','00100','01000','11111'],'3':['11110','00001','00001','01110','00001','00001','11110'],'4':['00010','00110','01010','10010','11111','00010','00010'],'5':['11111','10000','10000','11110','00001','00001','11110'],'6':['01110','10000','10000','11110','10001','10001','01110'],'7':['11111','00001','00010','00100','01000','01000','01000'],'8':['01110','10001','10001','01110','10001','10001','01110'],'9':['01110','10001','10001','01111','00001','00001','01110'],':':['00000','00100','00100','00000','00100','00100','00000'],'>':['10000','01000','00100','00010','00100','01000','10000'],'-':['00000','00000','00000','11111','00000','00000','00000']};
const chars=' ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:>-';
const font=[...chars].flatMap(c=>[...glyphs[c].flatMap(r=>{const bits=parseInt(r,2)<<2;return [bits,bits];}),0,0]);
fs.writeFileSync('game/plugins/basketball/engine/include/jam_hud.h',`/* Generated by scripts/create-prototype-art.mjs. */\nstatic const unsigned char jam_font[] = {${font.join(',')}};\n`);
fs.writeFileSync('art/prototype-pixels.json',JSON.stringify({bodyRows:rows,bodyPalette:pal,bodyPoseNames:poseNames,bodyWidth:BODY_W,bodyHeight:BODY_H,hudCharacters:chars,hudGlyphs:glyphs},null,2)+'\n');
console.log('Authored court, temporary body frames, ball, marker, and HUD glyphs.');
