// Native cinematic frames: deliberate poses from the existing editable crew cells.
// Raster scaling uses nearest whole cells; every compiled 8x8 tile has one palette.
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import {createRequire} from 'node:module';
import {bodyCells} from './player-poses.mjs';
import {bodyProfiles} from './create-body-profiles.mjs';
const require=createRequire(path.join(process.env.JAM_PNGJS_ROOT||path.join(os.homedir(),'.cache/codex-runtimes/codex-primary-runtime/dependencies/node'),'package.json'));
const {PNG}=require('pngjs');
const root='game/plugins/basketball/engine',out='art/dunks-v1';
fs.mkdirSync(out,{recursive:true});
const font=[...fs.readFileSync(`${root}/include/jam_hud.h`,'utf8').match(/\{([^}]+)\}/)[1].matchAll(/\d+/g)].map(m=>Number(m[0]));
// Nonzero background pixels let CGB tile priority clip contestants at the HUD.
const opaqueFont=font.map((v,i)=>i%2===0?255:v);
fs.writeFileSync(`${root}/include/jam_dunk_font.h`,`/* Generated opaque Dunk Cam HUD; ordinary HUD remains unchanged. */\nstatic const unsigned char jam_dunk_font[]={${opaqueFont.join(',')}};\n`);
const names=['matus','magic','bambi','torres','chompa','meat'];
const ratings=[3,9,7,8,7,8];
const skin=['#D69C74','#C99870','#E8B492','#C99368','#E5B493','#D7A079'];
const themes={0:['#102333','#E6B37C','#287780','#182738'],1:['#102333','#294953','#356270','#182738']};
const byteArray=(name,a)=>`static const unsigned char ${name}[]={${a.join(',')}};\n`;
const manifest=[];
function jump(player,t,fail){
 const peak=30+ratings[player]*2,hang=48-bodyProfiles[player].height_px;
 return t<8?0:t<20?Math.floor((t-8)*peak/12):t<28?peak-Math.floor((peak-hang)*(t-20)/8):
  fail?(t<40?Math.floor(hang*(40-t)/12):0):t<38?hang:t<48?Math.floor(hang*(48-t)/10):0;
}
function make(player,t,fail){
 const pixels=Array.from({length:128},()=>Array(160).fill(0));
 const attrs=Array.from({length:16},()=>Array(20).fill(1));
 const range=10+ratings[player]*4,travel=t<8?0:t<28?(t-8)/20:1;
 const px=-range+(range-8)*travel,j=jump(player,t,fail),feet=88-j;
 const minY=Math.min(20,feet-bodyProfiles[player].height_px-(t>=8&&t<28?13:0)),maxY=Math.max(52,feet);
 const ramp=t<4?1:t<20?1+(t-4)/16:t>48?1+(64-t)/16:2;
 const zoom=Math.min(ramp,112/(maxY-minY)),originY=minY-8/zoom;
 const sx=x=>Math.round(116+x*zoom),sy=y=>Math.round((y-originY)*zoom);
 const put=(x,y,c,p)=>{if(x>=0&&x<160&&y>=0&&y<128){pixels[y][x]=c;attrs[y>>3][x>>3]=p;}};
 const line=(x,y,xx,yy,c,p)=>{const n=Math.max(Math.abs(xx-x),Math.abs(yy-y));for(let i=0;i<=n;i++)put(Math.round(x+(xx-x)*i/(n||1)),Math.round(y+(yy-y)*i/(n||1)),c,p);};
 // Quiet dark arena; audience pixels stay outside the action silhouette.
 for(let y=8;y<120;y+=16)for(let x=0;x<160;x++)if(x<16||x>150)put(x,y,2,1);
 line(0,124,159,124,2,1);
 const pose=t<8?10:t<14?11:t<24?12:t<30?13:t<38&&!fail?14:t<48?15:0;
 const cells=bodyCells(player,pose,false),hatTiles=new Set();
 const headY=48-bodyProfiles[player].height_px+(pose===10||pose===15?4:0);
 const left=sx(px-8),top=sy(feet-48),w=Math.round(16*zoom),h=Math.round(48*zoom);
 for(let y=0;y<h;y++)for(let x=0;x<w;x++){
  const cy=Math.min(47,Math.floor(y/zoom)),cx=Math.min(15,Math.floor(x/zoom)),c=cells[cy][cx];
  if(c!=='.')put(left+x,top+y,c==='#'?3:c==='o'?1:2,6);
  if(player===4 && cy>=headY && cy<headY+2 && cx>=4 && cx<12 && c==='s' && left+x>=0 && left+x<160 && top+y>=0 && top+y<128)hatTiles.add(((top+y)>>3)*20+((left+x)>>3));
 }
 // Hat colors are tile-wide; assign after skin pixels so they cannot overwrite it.
 for(const tile of hatTiles)attrs[Math.floor(tile/20)][tile%20]=2;
 // Angled backboard, inward rim and laced net. Skin-tone ivory preserves hands
 // in the same contact tiles without introducing a fifth hardware color.
 const board=sx(16),bt=sy(20),bb=sy(48);
 for(let x=board;x<Math.min(160,board+Math.round(5*zoom));x++)line(x,bt+Math.floor((x-board)/2),x,bb,3,3);
 line(board,bt,board,bb,1,3);line(board,bt,Math.min(159,board+Math.round(5*zoom)),bt+3,1,3);
 line(sx(11),sy(43),board,sy(40),3,3);
 const netBottom=sy(60+(t>=28&&t<=38&&!fail?3:0));
 for(let y=sy(46);y<netBottom;y++){
  const inset=Math.floor((y-sy(46))/7),lx=sx(-6)+inset,rx=sx(6)-inset;
  put(lx,y,1,3);put(rx,y,1,3);
  if(y%5===0)line(lx,y,rx,y,1,3);
  for(let x=lx;x<=rx;x++)if((x+y)%7===0)put(x,y,1,3);
 }
 for(let a=0;a<120;a++){const r=a*Math.PI/60;put(sx(Math.cos(r)*8),sy(44+Math.sin(r)*3),2,3);}
 // Precompensate only the jersey cells before mirroring the left-basket shot.
 // Hair, face, limbs and court turn; the number remains readable.
 const leftPixels=pixels.map(row=>row.slice()),leftCells=bodyCells(player,pose,false,true);
 for(let y=0;y<h;y++)for(let x=0;x<w;x++){
  const cy=Math.min(47,Math.floor(y/zoom)),cx=Math.min(15,Math.floor(x/zoom));
  const c=leftCells[cy][cx];
  if(c!==cells[cy][cx] && top+y>=0 && top+y<128 && left+x>=0 && left+x<160 && attrs[(top+y)>>3][(left+x)>>3]===6)
   leftPixels[top+y][left+x]=c==='#'?3:c==='o'?1:c==='s'?2:0;
 }
 const patterns=[],lookup=new Map();
 const add=bytes=>{const key=bytes.join(',');if(!lookup.has(key)){lookup.set(key,patterns.length);patterns.push(bytes);}return lookup.get(key);};
 add(Array(16).fill(0));
 const pal=attrs.flat();
 const pack=canvas=>{
  const map=[];
  for(let ty=0;ty<16;ty++)for(let tx=0;tx<20;tx++){
   const bytes=[];
   for(let y=0;y<8;y++){let lo=0,hi=0;for(let x=0;x<8;x++){const c=canvas[ty*8+y][tx*8+x];lo|=(c&1)<<(7-x);hi|=(c>>1)<<(7-x);}bytes.push(lo,hi);}
   map.push(add(bytes));
  }
  return map;
 };
 const map=pack(pixels),leftmap=pack(leftPixels);
 if(patterns.length>88)throw Error(`Dunk Cam tile budget: ${player} t=${t} fail=${fail}: ${patterns.length}`);
 const palette={...themes,2:['#102333',skin[player],'#94C83D','#182738'],3:['#102333',skin[player],'#E89328','#182738'],6:['#102333',skin[player],'#EAEFF7','#182738']};
 if([0,8,20,28,32,44,60].includes(t)&&!fail){
  const image=new PNG({width:160,height:128});
  for(let y=0;y<128;y++)for(let x=0;x<160;x++)image.data.set([...palette[attrs[y>>3][x>>3]][pixels[y][x]].slice(1).match(/../g).map(v=>parseInt(v,16)),255],(y*160+x)*4);
  fs.writeFileSync(`${out}/${names[player]}-${String(t).padStart(2,'0')}.png`,PNG.sync.write(image));
 }
 return {tiles:patterns.flat(),map,leftmap,pal,count:patterns.length,
  zoom:Math.round(zoom*256),origin:Math.round(originY*256),avatar:Math.round(px*256)};
}
const chunks=[];
// Replace only this generator's own native frame modules.
for(const name of fs.readdirSync(`${root}/src/core`))if(/^jam_dunk_frame_.*\.c$/.test(name))fs.unlinkSync(`${root}/src/core/${name}`);
for(let p=0;p<6;p++)for(let fail=0;fail<2;fail++){
 const frames=[];
 for(let t=fail?16:0;t<64;t+=2)frames.push({t,...make(p,t,!!fail)});
 for(let start=0;start<frames.length;start+=3){
  const batch=frames.slice(start,start+3),id=`${names[p]}_${fail}_${start/3}`;
  let code='#pragma bank 255\n#include <gbdk/platform.h>\n#include "jam_dunk_cam.h"\n';
  for(const [i,f] of batch.entries()){
   const reverse=a=>Array.from({length:320},(_,n)=>a[Math.floor(n/20)*20+19-n%20]);
   const ui=f.map.map(n=>n<24?232+n:192+n-24),uiAttrs=f.pal.map((v,n)=>v|(f.map[n]>=24?8:0));
   const leftUi=f.leftmap.map(n=>n<24?232+n:192+n-24),leftUiAttrs=f.pal.map((v,n)=>v|(f.leftmap[n]>=24?8:0));
   code+=byteArray(`tiles_${i}`,f.tiles)+byteArray(`map_${i}`,f.map)+byteArray(`mirror_map_${i}`,reverse(f.leftmap));
   code+=byteArray(`ui_map_${i}`,ui)+byteArray(`mirror_ui_map_${i}`,reverse(leftUi));
   for(let bank=0;bank<2;bank++)code+=byteArray(`attrs${bank}_${i}`,f.pal.map(v=>v|(bank?8:0)))+byteArray(`mirror_attrs${bank}_${i}`,reverse(f.pal).map(v=>v|32|(bank?8:0)));
   code+=byteArray(`ui_attrs_${i}`,uiAttrs)+byteArray(`mirror_ui_attrs_${i}`,reverse(leftUiAttrs).map(v=>v|32));
  }
  code+=`void jam_dunk_frame_${id}(unsigned char frame,unsigned char mirror) BANKED {\nunsigned char old;const unsigned char *map,*attrs;\nswitch(frame){\n`;
  batch.forEach((f,i)=>{
   code+=`case ${f.t/2}:if(!jam_cam_upload){VBK_REG=0;set_bkg_data(232,${Math.min(f.count,24)},tiles_${i});`;
   if(f.count>24)code+=`VBK_REG=1;set_bkg_data(192,${f.count-24},tiles_${i}+384);`;
   code+=`map=mirror?mirror_ui_map_${i}:ui_map_${i};attrs=mirror?mirror_ui_attrs_${i}:ui_attrs_${i};}else{VBK_REG=jam_cam_upload-1;set_bkg_data(0,${f.count},tiles_${i});map=mirror?mirror_map_${i}:map_${i};attrs=jam_cam_upload==2?(mirror?mirror_attrs1_${i}:attrs1_${i}):(mirror?mirror_attrs0_${i}:attrs0_${i});}`;
   code+=`jam_cam_zoom=${f.zoom};jam_cam_origin=${f.origin};jam_cam_avatar=${f.avatar};break;\n`;
  });
  code+='default:return;}\nif(!jam_cam_upload && frame==0){VBK_REG=0;set_win_tiles(0,0,20,16,map);VBK_REG=1;set_win_tiles(0,0,20,16,attrs);VBK_REG=0;wait_vbl_done();return;}\nold=LCDC_REG;LCDC_REG=(old&~LCDCF_BG9C00)|(jam_cam_map_target?LCDCF_BG9C00:0);VBK_REG=0;set_bkg_tiles(0,0,20,16,map);VBK_REG=1;set_bkg_tiles(0,0,20,16,attrs);VBK_REG=0;LCDC_REG=(LCDC_REG&~LCDCF_BG9C00)|(old&LCDCF_BG9C00);wait_vbl_done();LCDC_REG=(LCDC_REG&~LCDCF_WIN9C00)|(jam_cam_map_target?LCDCF_WIN9C00:0); }\n';
  fs.writeFileSync(`${root}/src/core/jam_dunk_frame_${id}.c`,code);
  chunks.push({p,fail,id,frames:batch.map(f=>f.t/2)});
 }
 manifest.push({player:names[p],failed:!!fail,frames:frames.map(f=>({time:f.t,tiles:f.count,zoom:f.zoom/256,originY:f.origin/256}))});
}
let dispatch='#pragma bank 255\n#include "jam_dunk_cam.h"\n';
chunks.forEach(c=>dispatch+=`void jam_dunk_frame_${c.id}(unsigned char frame,unsigned char mirror) BANKED;\n`);
dispatch+='void jam_dunk_art(unsigned char player,unsigned char frame,unsigned char failed,unsigned char mirror) BANKED {\nif(frame<8)failed=0;switch(player){\n';
for(let p=0;p<6;p++){
 dispatch+=`case ${p}:if(failed){switch(frame){\n`;
 for(const c of chunks.filter(c=>c.p===p&&c.fail))dispatch+=c.frames.map(f=>`case ${f}:`).join('')+`jam_dunk_frame_${c.id}(frame,mirror);break;\n`;
 dispatch+='}}else{switch(frame){\n';
 for(const c of chunks.filter(c=>c.p===p&&!c.fail))dispatch+=c.frames.map(f=>`case ${f}:`).join('')+`jam_dunk_frame_${c.id}(frame,mirror);break;\n`;
 dispatch+='}}break;\n';
}
dispatch+='}}\n';
fs.writeFileSync(`${root}/src/core/jam_dunk_art.c`,dispatch);
fs.writeFileSync(`${out}/manifest.json`,JSON.stringify({method:'Deliberate crew-cell poses, native tile animation, nearest whole-cell zoom',durationUpdates:64,frameEveryUpdates:2,cardinalDirection:'right; left uses native tile-map reversal and X flip',frames:manifest},null,2)+'\n');
console.log(JSON.stringify({chunks:chunks.length,frames:manifest.reduce((n,p)=>n+p.frames.length,0),maxTiles:Math.max(...manifest.flatMap(p=>p.frames.map(f=>f.tiles)))}));
