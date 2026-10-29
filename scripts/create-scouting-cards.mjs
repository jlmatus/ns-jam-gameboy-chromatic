// Native tile composition from approved face cells and editable bitmap glyphs.
// All output pixels are deliberate integer cells. No generated concept art is imported.
import fs from 'node:fs';
const root='game/plugins/basketball/engine';
const spec=JSON.parse(fs.readFileSync('art/ui/scouting-cards.json','utf8'));
const canonical=[...fs.readFileSync(`${root}/include/jam_sim.h`,'utf8').matchAll(/\{"([A-Z]+)", "([0-9]+)", "([A-Z]+)", "[^"]+", \{([0-9,]+)\}\}/g)].map(m=>({name:m[1],jersey:m[2],position:m[3],ratings:m[4].split(',').map(Number)}));
const skillIndex={SPD:0,'3PT':1,PASS:2,STEAL:3,BLOCK:4,DUNK:5,POWER:6,CLUTCH:7};
if(canonical.length!==6)throw Error('Cannot resolve canonical roster');
for(const [i,p] of spec.players.entries()) {
  const c=canonical[i];
  if(p.name!==c.name||p.jersey!==c.jersey||p.position!==c.position||JSON.stringify(p.ratings)!==JSON.stringify(c.ratings))throw Error(`Roster drift: ${p.name}`);
  for(const [label,value] of p.featured)if(value!==c.ratings[skillIndex[label]])throw Error(`Featured rating drift: ${p.name} ${label}`);
}
const sourceGlyphs=JSON.parse(fs.readFileSync('art/prototype-pixels.json','utf8')).hudGlyphs;
const chars=' ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:>-';
const glyphs=Object.fromEntries(Object.entries(sourceGlyphs).map(([c,rows])=>[c,rows.map(row=>{
  const bits=Number.parseInt(row,2)<<1;return (bits|(bits>>1)).toString(2).padStart(6,'0');
})]));
const rgb555=hex=>{const c=hex.slice(1,7).match(/../g).map(v=>Math.round(parseInt(v,16)*31/255));return c[0]|(c[1]<<5)|(c[2]<<10);};
const stageNames=['YOUR PLAYER','CPU TEAMMATE','OPPONENT ONE','OPPONENT TWO'];
const stageCells=stageNames.map(name=>({name}));
function screen(){return {pixels:Array.from({length:144},()=>Array(160).fill(0)),pal:Array.from({length:18},()=>Array(20).fill(5))};}
function put(s,x,y,c){if(x<0||x>=160||y<0||y>=144)throw Error(`Out of bounds ${x},${y}`);s.pixels[y][x]=c;}
function text(s,x,y,str,scale=1){
  if(x<0||x+str.length*8*scale>160||y+7*scale>144)throw Error(`Text overflow: ${str}`);
  for(const [i,c] of [...str].entries())for(let gy=0;gy<7;gy++)for(let gx=0;gx<6;gx++)if((glyphs[c]||glyphs[' '])[gy][gx]==='1')
    for(let sy=0;sy<scale;sy++)for(let sx=0;sx<scale;sx++)put(s,x+i*8*scale+gx*scale+sx,y+gy*scale+sy,3);
}
const centered=(s,y,str,scale=1)=>text(s,Math.floor((160-str.length*8*scale)/16)*8,y,str,scale);
function meter(s,x,y,value){for(let i=0;i<10;i++)for(let dy=0;dy<5;dy++)for(let dx=0;dx<5;dx++)put(s,x+i*6+dx,y+dy,dy===0||dy===4||dx===0||dx===4?3:(i<value?1:0));}
function tile(s,tx,ty){const bytes=[];for(let y=0;y<8;y++){let a=0,b=0;for(let x=0;x<8;x++){const c=s.pixels[ty*8+y][tx*8+x];a|=(c&1)<<(7-x);b|=(c>>1)<<(7-x);}bytes.push(a,b);}return bytes;}
function compile(s,includeStages){
  const patterns=[],lookup=new Map();
  const index=bytes=>{const key=bytes.join(',');if(!lookup.has(key)){lookup.set(key,patterns.length);patterns.push(bytes);}return lookup.get(key);};
  const raw=[];for(let y=0;y<18;y++)for(let x=0;x<20;x++)raw.push(index(tile(s,x,y)));
  const stages=[];
  if(includeStages)for(const {name} of stageCells){const h=screen();centered(h,1,name);stages.push(Array.from({length:20},(_,x)=>index(tile(h,x,0))));}
  if(patterns.length>128)throw Error(`UI tile budget exceeded: ${patterns.length}`);
  return {count:patterns.length,bank0:patterns.slice(0,64).flat(),bank1:patterns.slice(64).flat(),map:raw.map(i=>192+i%64),attrs:raw.map((i,n)=>s.pal[Math.floor(n/20)][n%20]|(i>=64?8:0)),stages:stages.map(row=>({map:row.map(i=>192+i%64),attrs:row.map(i=>5|(i>=64?8:0))}))};
}
const arr=(name,v)=>`static const unsigned char ${name}[] = {${v.join(',')}};\n`;
const manifest=[];
for(const [player,p] of spec.players.entries()){
  const portrait=JSON.parse(fs.readFileSync(`art/portraits/${p.source}/pixels.json`,'utf8'));
  const faceColors=Object.fromEntries(Object.entries(portrait.palette).map(([k,v])=>[k,v.slice(0,7).toUpperCase()]));
  const families=[{slot:6,colors:[spec.palette[0],faceColors.s,faceColors.o,faceColors['#']]}];
  const familyFor=colors=>{
    let family=families.find(f=>colors.every(c=>f.colors.includes(c)));
    if(!family){
      const slots=[6,4,3,2,1];if(families.length>=slots.length)throw Error('Portrait palette budget exceeded');
      family={slot:slots[families.length],colors:[...colors]};while(family.colors.length<4)family.colors.push(family.colors[0]);families.push(family);
    }
    return family;
  };
  const pages=[];
  for(let page=0;page<3;page++){
    const s=screen();
    centered(s,1,page===0?stageNames[0]:page===1?'RATINGS 1 OF 2':'RATINGS 2 OF 2');
    centered(s,16,p.name,2);
    // Right and left native arrows, flipped by deliberate source cell placement.
    for(let y=0;y<7;y++)for(let x=0;x<6;x++)if(glyphs['>'][y][x]==='1'){put(s,13-x,20+y,3);put(s,144+x,20+y,3);}
    centered(s,32,`${p.jersey} ${p.position}`);
    if(page===0){
      const face=Array.from({length:32},()=>Array(32).fill('.'));
      portrait.rows.forEach(([offset,row],y)=>[...row].forEach((c,x)=>face[y][offset+x]=c));
      for(let ty=0;ty<8;ty++)for(let tx=0;tx<8;tx++){
        let colors=[];
        for(let y=0;y<4;y++)for(let x=0;x<4;x++){
          const c=face[ty*4+y][tx*4+x];const color=c==='.'?spec.palette[0]:faceColors[c];if(!colors.includes(color))colors.push(color);
        }
        // One CHOMPA edge tile has five colors including transparency. Fill only
        // its transparent cells with the outline shade; all approved opaque cells stay exact.
        const backdrop=colors.length>4?faceColors['#']:spec.palette[0];
        if(colors.length>4)colors=colors.filter(c=>c!==spec.palette[0]);
        const family=familyFor(colors);s.pal[6+ty][1+tx]=family.slot;
        for(let y=0;y<4;y++)for(let x=0;x<4;x++){
          const c=face[ty*4+y][tx*4+x],color=c==='.'?backdrop:faceColors[c];
          for(let dy=0;dy<2;dy++)for(let dx=0;dx<2;dx++)put(s,8+tx*8+x*2+dx,48+ty*8+y*2+dy,family.colors.indexOf(color));
        }
      }
      p.featured.forEach(([label,value],i)=>{text(s,80,48+i*16,label);text(s,136,48+i*16,String(value).padStart(2,' '));meter(s,80,58+i*16,value);});
      centered(s,112,p.playstyle);
    } else {
      const names=page===1?['SPEED','THREE','PASSING','DUNK']:['STEAL','BLOCK','POWER','CLUTCH'];
      const ids=page===1?[0,1,2,5]:[3,4,6,7];
      names.forEach((label,i)=>{text(s,8,48+i*16,label);text(s,136,48+i*16,String(p.ratings[ids[i]]).padStart(2,' '));meter(s,72,58+i*16,p.ratings[ids[i]]);});
      centered(s,112,page===1?'ATTACK AND SPEED':'DEFENSE AND CLUTCH');
    }
    centered(s,128,'A PICK  B BACK');centered(s,136,page===0?'SELECT ALL RATINGS':page===1?'SELECT NEXT PAGE':'SELECT PLAYER CARD');
    s.colors={5:spec.palette,...Object.fromEntries(families.map(f=>[f.slot,f.colors]))};
    fs.mkdirSync('game/build/scouting-pixels',{recursive:true});fs.writeFileSync(`game/build/scouting-pixels/${p.name.toLowerCase()}-${page}.json`,JSON.stringify(s));
    pages.push(compile(s,page===0));
  }
  let code=`/* Generated by scripts/create-scouting-cards.mjs; source cells retained in art/. */\n#pragma bank 255\n#include <gbdk/platform.h>\n#include "jam_cards.h"\n#include "palette.h"\n`;
  for(const [i,p] of pages.entries()){
    code+=arr(`tiles0_${i}`,p.bank0);if(p.bank1.length)code+=arr(`tiles1_${i}`,p.bank1);
    code+=arr(`map_${i}`,p.map)+arr(`attrs_${i}`,p.attrs);
    for(const [j,row] of p.stages.entries())code+=arr(`stage_map_${j}`,row.map)+arr(`stage_attrs_${j}`,row.attrs);
  }
  families.forEach(f=>code+=`static const unsigned short face_palette_${f.slot}[]={${f.colors.map(rgb555).join(',')}};\n`);
  code+=`void jam_card_${p.name.toLowerCase()}(unsigned char page,unsigned char stage) BANKED {\n`;
  for(const [i,p] of pages.entries()){
    code+=`${i?'else ':''}if(page==${i}) {\nVBK_REG=0;set_bkg_data(192,${Math.min(p.count,64)},tiles0_${i});\n`;
    if(p.count>64)code+=`VBK_REG=1;set_bkg_data(192,${p.count-64},tiles1_${i});\n`;
    code+=`VBK_REG=0;set_win_tiles(0,0,20,18,map_${i});VBK_REG=1;set_win_tiles(0,0,20,18,attrs_${i});VBK_REG=0;\n`;
    if(i===0)for(let stage=0;stage<4;stage++)code+=`${stage?'else ':''}if(stage==${stage}) {VBK_REG=0;set_win_tiles(0,0,20,1,stage_map_${stage});VBK_REG=1;set_win_tiles(0,0,20,1,stage_attrs_${stage});VBK_REG=0;}\n`;
    code+='}\n';
  }
  families.forEach(f=>code+=`memcpy(&BkgPalette[${f.slot}],face_palette_${f.slot},8);set_bkg_palette(${f.slot},1,face_palette_${f.slot});\n`);code+='}\n';
  code=code.replace('#include "palette.h"','#include "palette.h"\n#include <string.h>');
  fs.writeFileSync(`${root}/src/core/jam_card_${p.name.toLowerCase()}.c`,code);
  manifest.push({name:p.name,pages:pages.map(p=>({tiles:p.count,bank0:Math.min(p.count,64),bank1:Math.max(0,p.count-64)}))});
}
const font=[...chars].flatMap(c=>[...glyphs[c].flatMap(row=>{const bits=Number.parseInt(row,2)<<1;return [bits,bits];}),0,0]);
fs.writeFileSync(`${root}/include/jam_cards.h`,`#ifndef JAM_CARDS_H\n#define JAM_CARDS_H\n#include "jam_sim.h"\nvoid jam_card_render(uint8_t player,uint8_t page,uint8_t stage) JAM_BANKED;\nvoid jam_menu_style(void) JAM_BANKED;\n#endif\n`);
fs.writeFileSync(`${root}/src/core/jam_cards.c`,`/* Generated from art/ui/scouting-cards.json. */\n#pragma bank 255\n#include "jam_cards.h"\n#include "palette.h"\n#include <string.h>\n${spec.players.map(p=>`void jam_card_${p.name.toLowerCase()}(unsigned char page,unsigned char stage) BANKED;`).join('\n')}\nstatic const unsigned char menu_font[]={${font.join(',')}};\nstatic const unsigned short menu_palette[]={${spec.palette.map(rgb555).join(',')}};\nvoid jam_menu_style(void) BANKED {VBK_REG=0;set_bkg_data(192,40,menu_font);memcpy(&BkgPalette[5],menu_palette,8);set_bkg_palette(5,1,menu_palette);}\nvoid jam_card_render(uint8_t player,uint8_t page,uint8_t stage) BANKED {switch(player){${spec.players.map((p,i)=>`case ${i}:jam_card_${p.name.toLowerCase()}(page,stage);break;`).join('')}}}\n`);
fs.writeFileSync('art/ui/scouting-card-budget.json',JSON.stringify(manifest,null,2)+'\n');
console.log(JSON.stringify(manifest));
