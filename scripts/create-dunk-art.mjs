// Native cinematic frames: deliberate poses from the existing editable crew cells.
// Raster scaling uses nearest whole cells; every compiled 8x8 tile has one palette.
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import {createRequire} from 'node:module';
import {dunkCells} from './dunk-poses.mjs';
import {bodyProfiles} from './create-body-profiles.mjs';
const require=createRequire(path.join(process.env.JAM_PNGJS_ROOT||path.join(os.homedir(),'.cache/codex-runtimes/codex-primary-runtime/dependencies/node'),'package.json'));
const {PNG}=require('pngjs');
const root='game/plugins/basketball/engine',out='art/dunks-v2';
fs.mkdirSync(out,{recursive:true});
const font=[...fs.readFileSync(`${root}/include/jam_hud.h`,'utf8').match(/\{([^}]+)\}/)[1].matchAll(/\d+/g)].map(m=>Number(m[0]));
// Nonzero background pixels let CGB tile priority clip contestants at the HUD.
const opaqueFont=font.map((v,i)=>i%2===0?255:v);
fs.writeFileSync(`${root}/include/jam_dunk_font.h`,`/* Generated opaque Dunk Cam HUD; ordinary HUD remains unchanged. */\nstatic const unsigned char jam_dunk_font[]={${opaqueFont.join(',')}};\n`);
const names=['matus','magic','bambi','chrux','chompa','meat'];
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
 const owners=Array.from({length:128},()=>Array(160).fill(1)),foregroundTiles=new Set();
 const attrs=Array.from({length:16},()=>Array(20).fill(1));
 const range=10+ratings[player]*4,travel=t<8?0:t<28?(t-8)/20:1;
 const px=-range+(range-8)*travel,j=jump(player,t,fail);
 const hanging=!fail&&t>=26&&t<42?Math.min((t-26)*6,12,(42-t)*3):0;
 const feet=88-j+hanging,cel=dunkCells(player,t,fail);
 const minY=Math.min(20,feet-56+Math.min(cel.headY,cel.hand[1])-7),maxY=Math.max(52,feet);
 const ramp=t<4?1:t<20?1+(t-4)/16:t>48?1+(64-t)/16:2;
 const zoom=Math.min(ramp,112/(maxY-minY)),originY=minY-8/zoom;
 const shake=!fail&&t>=28&&t<36?[[2,1],[-2,-1],[1,1],[-1,0]][(t-28)/2]:[0,0];
 const sx=x=>Math.round(116+x*zoom)+shake[0],sy=y=>Math.round((y-originY)*zoom)+shake[1];
 const put=(x,y,c,p)=>{if(x>=0&&x<160&&y>=0&&y<128){pixels[y][x]=c;owners[y][x]=p;attrs[y>>3][x>>3]=p;if(p!==1&&p!==4)foregroundTiles.add((y>>3)*20+(x>>3));}};
 const line=(x,y,xx,yy,c,p)=>{const n=Math.max(Math.abs(xx-x),Math.abs(yy-y));for(let i=0;i<=n;i++)put(Math.round(x+(xx-x)*i/(n||1)),Math.round(y+(yy-y)*i/(n||1)),c,p);};
 // Moving sideline streaks emphasize ascent without filling the face/ball area.
 for(let y=8;y<112;y+=16){const shift=t>=8&&t<28?(t*3)%16:0;line(0,y+shift,12+(t%6),y+shift-3,2,1);line(151,y+shift,159,y+shift-2,2,1);}
 line(0,124,159,124,2,1);
 // Arena-sized lettering is behind the athlete, rim and effects. It never
 // consumes resident HUD/gauge tiles, and is omitted during the court wipes.
 const word=new Map(),name=names[player].toUpperCase(),scale=3,scaleY=2;
 if(t>=4&&t<54&&fail!==2){
  const start=Math.floor((160-(name.length*6-1)*scale)/2),nameY=24;
  const ink=[];
  [...name].forEach((ch,i)=>{for(let y=0;y<7;y++)for(let x=0;x<5;x++)
   if(font[(ch.charCodeAt(0)-64)*16+y*2]&(1<<(6-x)))
    for(let yy=0;yy<scaleY;yy++)for(let xx=0;xx<scale;xx++)ink.push([start+i*6*scale+x*scale+xx,nameY+y*scaleY+yy]);
  });
  for(const [x,y] of ink)word.set(y*160+x,2);
  for(const [n,c] of word)put(n%160,Math.floor(n/160),c,4);
 }
 // Impact rays expand once; no full-screen flashes. Crowd arms pop on a make.
 if(!fail&&t>=28&&t<40){const r=12+(t-28)*2;for(const [dx,dy] of [[1,0],[-1,0],[.7,-.7],[-.7,-.7],[0,-1]])line(sx(dx*(r-5)),sy(44+dy*(r-5)),sx(dx*r),sy(44+dy*r),2,3);}
 for(let x=8;x<154;x+=24){const cheer=!fail&&t>=28&&t<42?3:0;line(x,120,x,115-cheer,2,1);line(x,117-cheer,x-3,119-cheer*2,2,1);line(x,117-cheer,x+3,119-cheer*2,2,1);}
 if(t>=44&&t<52){const r=(t-44)*2;line(sx(px-r-4),sy(89),sx(px-r),sy(87),2,1);line(sx(px+r),sy(87),sx(px+r+4),sy(89),2,1);}
 // Angled backboard, inward rim and laced net. Skin-tone ivory preserves hands
 // in the same contact tiles without introducing a fifth hardware color.
 const rattle=!fail&&t>=28&&t<38?[2,-1,1,0,0][(t-28)/2]:0;
 const board=sx(16)+rattle,bt=sy(20),bb=sy(48);
 for(let x=board;x<Math.min(160,board+Math.round(5*zoom));x++)line(x,bt+Math.floor((x-board)/2),x,bb,3,3);
 line(board,bt,board,bb,1,3);line(board,bt,Math.min(159,board+Math.round(5*zoom)),bt+3,1,3);
 line(sx(11),sy(43),board,sy(40),3,3);
 const stretch=!fail&&t>=28&&t<42?[8,12,7,3,6,2,0][(t-28)/2]:0;
 const netBottom=sy(60+stretch);
 for(let y=sy(46);y<netBottom;y++){
  const inset=Math.min(4,Math.floor((y-sy(46))/7)),sway=Math.round(rattle*(y-sy(46))/12),lx=sx(-6)+inset+sway,rx=sx(6)-inset+sway;
  put(lx,y,1,3);put(rx,y,1,3);
  if(y%5===0)line(lx,y,rx,y,1,3);
  for(let x=lx;x<=rx;x++)if((x+y)%7===0)put(x,y,1,3);
 }
 for(let a=0;a<120;a++){const r=a*Math.PI/60;put(sx(Math.cos(r)*8),sy(44+Math.sin(r)*3)+rattle,2,3);}
 const {cells,headY,headX}=cel,hatTiles=new Set();
 const left=sx(px-12),top=sy(feet-56),w=Math.round(24*zoom),h=Math.round(56*zoom);
 for(let y=0;y<h;y++)for(let x=0;x<w;x++){
  const cy=Math.min(55,Math.floor(y/zoom)),cx=Math.min(23,Math.floor(x/zoom)),c=cells[cy][cx];
  if(c!=='.')put(left+x,top+y,c==='#'?3:c==='o'?1:2,6);
  if(player===4 && cy>=headY && cy<headY+2 && cx>=headX && cx<headX+8 && c==='s' && left+x>=0 && left+x<160 && top+y>=0 && top+y<128)hatTiles.add(((top+y)>>3)*20+((left+x)>>3));
 }
 // Hat colors are tile-wide; assign after skin pixels so they cannot overwrite it.
 for(const tile of hatTiles)attrs[Math.floor(tile/20)][tile%20]=2;
 // A CGB tile has one palette. Clear background lettering from every tile
 // touched by the foreground, preserving the athlete's exact skin/kit colors.
 for(const [n] of word){const x=n%160,y=Math.floor(n/160);if(foregroundTiles.has((y>>3)*20+(x>>3))&&owners[y][x]===4){pixels[y][x]=0;owners[y][x]=1;}}
 // Precompensate only the jersey cells before mirroring the left-basket shot.
 // Hair, face, limbs and court turn; the number remains readable.
 const leftPixels=pixels.map(row=>row.slice()),leftAttrs=attrs.map(row=>row.slice()),leftCells=dunkCells(player,t,fail,true).cells;
 // Pre-mirror only the arena word; the final left-basket map flip then leaves
 // the name readable while the athlete, basket and motion turn naturally.
 for(const [n] of word){const x=n%160,y=Math.floor(n/160);if(owners[y][x]===4)leftPixels[y][x]=0;}
 for(const [n,c] of word){const x=159-n%160,y=Math.floor(n/160);if(!foregroundTiles.has((y>>3)*20+(x>>3))){leftPixels[y][x]=c;leftAttrs[y>>3][x>>3]=4;}}
 for(let y=0;y<h;y++)for(let x=0;x<w;x++){
  const cy=Math.min(55,Math.floor(y/zoom)),cx=Math.min(23,Math.floor(x/zoom));
  const c=leftCells[cy][cx];
  if(c!==cells[cy][cx] && top+y>=0 && top+y<128 && left+x>=0 && left+x<160 && attrs[(top+y)>>3][(left+x)>>3]===6)
   leftPixels[top+y][left+x]=c==='#'?3:c==='o'?1:c==='s'?2:0;
 }
 // The rejected-dunk composition uses an opaque, tile-aligned impact stamp.
 // Keep it inside the 0..127 cinematic page; 128..191 overlap player OBJ art.
 if(fail===2&&t>=8&&t<54){
  const bounce=t<32?[0,-2,1,0][Math.floor(t/2)%4]:0;
  const stamp=(canvas,attributes,preMirror)=>{
   const ink=(x,y,c)=>{if(preMirror)x=159-x;canvas[y][x]=c;attributes[y>>3][x>>3]=4;};
   for(let y=40;y<72;y++)for(let x=24;x<136;x++)ink(x,y,3);
   for(let x=24;x<136;x++)if(x<40||x>=120){ink(x,40,1);ink(x,71,1);}
   for(let y=40;y<72;y++)if(y<47||y>=65){ink(24,y,1);ink(135,y,1);}
   [...'BLOCKED'].forEach((ch,i)=>{for(let y=0;y<7;y++)for(let x=0;x<5;x++)
    if(font[(ch.charCodeAt(0)-64)*16+y*2]&(1<<(6-x)))
     for(let yy=0;yy<2;yy++)for(let xx=0;xx<2;xx++)ink(39+i*12+x*2+xx,49+y*2+yy+bounce,2);
   });
  };
  stamp(pixels,attrs,false);stamp(leftPixels,leftAttrs,true);
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
 const limit=t===0||t===54?83:128;
 if(patterns.length>limit)throw Error(`Dunk Cam tile budget: ${player} t=${t} fail=${fail}: ${patterns.length}/${limit}`);
 const palette={...themes,2:['#102333',skin[player],'#94C83D','#182738'],3:['#102333',skin[player],'#E89328','#182738'],4:fail===2?['#102333','#EDB82B','#FFF7D5','#182738']:['#102333','#29495A','#528C9C','#182738'],6:['#102333',skin[player],'#EAEFF7','#182738']};
 if([0,8,20,28,32,44,60].includes(t)&&(!fail||fail===2)){
  const image=new PNG({width:160,height:128});
  for(let y=0;y<128;y++)for(let x=0;x<160;x++)image.data.set([...palette[attrs[y>>3][x>>3]][pixels[y][x]].slice(1).match(/../g).map(v=>parseInt(v,16)),255],(y*160+x)*4);
  fs.writeFileSync(`${out}/${names[player]}-${fail===2?'blocked-':''}${String(t).padStart(2,'0')}.png`,PNG.sync.write(image));
 }
 return {tiles:patterns.flat(),map,leftmap,pal,leftpal:leftAttrs.flat(),count:patterns.length,
  zoom:Math.round(zoom*256),origin:Math.round(originY*256),avatar:Math.round(px*256),
  handX:sx(px+cel.hand[0]-12),handY:sy(feet-56+cel.hand[1]-3),shake};
}
const chunks=[];
// Replace only this generator's own native frame modules.
for(const name of fs.readdirSync(`${root}/src/core`))if(/^jam_dunk_frame_.*\.c$/.test(name))fs.unlinkSync(`${root}/src/core/${name}`);
for(let p=0;p<6;p++)for(let fail=0;fail<3;fail++){
 const frames=[];
 for(let t=fail===1?16:fail===2?8:0;t<64;t+=2)frames.push({t,...make(p,t,fail)});
 for(let start=0;start<frames.length;start+=3){
  const batch=frames.slice(start,start+3),id=`${names[p]}_${fail}_${start/3}`;
  let code='#pragma bank 255\n#include <gbdk/platform.h>\n#include "jam_dunk_cam.h"\n';
  for(const [i,f] of batch.entries()){
   const reverse=a=>Array.from({length:320},(_,n)=>a[Math.floor(n/20)*20+19-n%20]);
   // Font 192..231 and turbo gauge 232..236 stay resident in bank zero.
   const ui=f.map.map(n=>n<19?237+n:192+n-19),uiAttrs=f.pal.map((v,n)=>v|(f.map[n]>=19?8:0));
   const leftUi=f.leftmap.map(n=>n<19?237+n:192+n-19),leftUiAttrs=f.leftpal.map((v,n)=>v|(f.leftmap[n]>=19?8:0));
   code+=byteArray(`tiles_${i}`,f.tiles)+byteArray(`map_${i}`,f.map)+byteArray(`mirror_map_${i}`,reverse(f.leftmap));
   if(f.t===0||f.t===54)code+=byteArray(`ui_map_${i}`,ui)+byteArray(`mirror_ui_map_${i}`,reverse(leftUi));
   for(let bank=0;bank<2;bank++)code+=byteArray(`attrs${bank}_${i}`,f.pal.map(v=>v|(bank?8:0)|(fail===2&&v===4?128:0)))+byteArray(`mirror_attrs${bank}_${i}`,reverse(f.leftpal).map(v=>v|32|(bank?8:0)|(fail===2&&v===4?128:0)));
   if(f.t===0||f.t===54)code+=byteArray(`ui_attrs_${i}`,uiAttrs)+byteArray(`mirror_ui_attrs_${i}`,reverse(leftUiAttrs).map(v=>v|32));
  }
  code+=`void jam_dunk_frame_${id}(unsigned char frame,unsigned char mirror) BANKED {\nunsigned char old;const unsigned char *map,*attrs;\nswitch(frame){\n`;
  batch.forEach((f,i)=>{
   code+=`case ${f.t/2}:`;
   if(f.t===0||f.t===54){
    code+=`if(!jam_cam_upload){VBK_REG=0;set_bkg_data(237,${Math.min(f.count,19)},tiles_${i});`;
    if(f.count>19)code+=`VBK_REG=1;set_bkg_data(192,${f.count-19},tiles_${i}+304);`;
    code+=`map=mirror?mirror_ui_map_${i}:ui_map_${i};attrs=mirror?mirror_ui_attrs_${i}:ui_attrs_${i};}else{`;
   }else code+='if(!jam_cam_upload)return;{';
   code+=`VBK_REG=jam_cam_upload-1;set_bkg_data(0,${f.count},tiles_${i});map=mirror?mirror_map_${i}:map_${i};attrs=jam_cam_upload==2?(mirror?mirror_attrs1_${i}:attrs1_${i}):(mirror?mirror_attrs0_${i}:attrs0_${i});}`;
   code+=`jam_cam_zoom=${f.zoom};jam_cam_origin=${f.origin};jam_cam_avatar=${f.avatar};jam_cam_hand_x=${f.handX};jam_cam_hand_y=${f.handY};jam_cam_shake_x=${f.shake[0]};jam_cam_shake_y=${f.shake[1]};break;\n`;
  });
  code+='default:return;}\nif(!jam_cam_upload && frame==0){VBK_REG=0;set_win_tiles(0,0,20,16,map);VBK_REG=1;set_win_tiles(0,0,20,16,attrs);VBK_REG=0;jam_dunk_cam_objects();wait_vbl_done();return;}\nold=LCDC_REG;LCDC_REG=(old&~LCDCF_BG9C00)|(jam_cam_map_target?LCDCF_BG9C00:0);VBK_REG=0;set_bkg_tiles(0,0,20,16,map);VBK_REG=1;set_bkg_tiles(0,0,20,16,attrs);VBK_REG=0;LCDC_REG=(LCDC_REG&~LCDCF_BG9C00)|(old&LCDCF_BG9C00);jam_dunk_cam_objects();wait_vbl_done();LCDC_REG=(LCDC_REG&~LCDCF_WIN9C00)|(jam_cam_map_target?LCDCF_WIN9C00:0); }\n';
  fs.writeFileSync(`${root}/src/core/jam_dunk_frame_${id}.c`,code);
  chunks.push({p,fail,id,frames:batch.map(f=>f.t/2)});
 }
 manifest.push({player:names[p],style:p===2||p===4?'two-hand power':'one-hand tomahawk',failed:!!fail,result:fail===2?'blocked':fail?'missed':'made',frames:frames.map(f=>({time:f.t,tiles:f.count,zoom:f.zoom/256,originY:f.origin/256,hand:[f.handX,f.handY],shake:f.shake}))});
}
let dispatch='#pragma bank 255\n#include "jam_dunk_cam.h"\n';
chunks.forEach(c=>dispatch+=`void jam_dunk_frame_${c.id}(unsigned char frame,unsigned char mirror) BANKED;\n`);
dispatch+='void jam_dunk_art(unsigned char player,unsigned char frame,unsigned char failed,unsigned char mirror) BANKED {\nif(frame<8&&failed!=2)failed=0;switch(player){\n';
for(let p=0;p<6;p++){
 dispatch+=`case ${p}:if(failed==2){switch(frame){\n`;
 for(const c of chunks.filter(c=>c.p===p&&c.fail===2))dispatch+=c.frames.map(f=>`case ${f}:`).join('')+`jam_dunk_frame_${c.id}(frame,mirror);break;\n`;
 dispatch+='}}else if(failed){switch(frame){\n';
 for(const c of chunks.filter(c=>c.p===p&&c.fail===1))dispatch+=c.frames.map(f=>`case ${f}:`).join('')+`jam_dunk_frame_${c.id}(frame,mirror);break;\n`;
 dispatch+='}}else{switch(frame){\n';
 for(const c of chunks.filter(c=>c.p===p&&!c.fail))dispatch+=c.frames.map(f=>`case ${f}:`).join('')+`jam_dunk_frame_${c.id}(frame,mirror);break;\n`;
 dispatch+='}}break;\n';
}
dispatch+='}}\n';
fs.writeFileSync(`${root}/src/core/jam_dunk_art.c`,dispatch);
fs.writeFileSync(`${out}/manifest.json`,JSON.stringify({method:'Articulated 24x56 crew cels, native tile animation, nearest whole-cell zoom; impact rays, rim rattle, net whip, crowd reaction and landing dust',durationUpdates:64,frameEveryUpdates:2,cardinalDirection:'right; left uses native tile-map reversal and X flip',frames:manifest},null,2)+'\n');
console.log(JSON.stringify({chunks:chunks.length,frames:manifest.reduce((n,p)=>n+p.frames.length,0),maxTiles:Math.max(...manifest.flatMap(p=>p.frames.map(f=>f.tiles)))}));
