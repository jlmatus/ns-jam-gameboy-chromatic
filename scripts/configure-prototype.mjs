// One-time native resource setup. IDs already present on disk are retained.
import fs from 'node:fs';
import {randomUUID,createHash} from 'node:crypto';
const read=p=>JSON.parse(fs.readFileSync(p,'utf8'));
const write=(p,v)=>fs.writeFileSync(p,JSON.stringify(v,null,2)+'\n');
const playerPath='game/assets/sprites/players.png.gbsres';
const base=fs.existsSync(playerPath)?read(playerPath):read('game/assets/sprites/ball.png.gbsres');
base.id=base.filename==='players.png'?base.id:randomUUID();
base.name='Crew - athletic directional poses';base.symbol='sprite_prototype_crew';base.filename='players.png';base.width=3840;base.height=48;base.canvasHeight=48;base.boundsY=-40;base.boundsHeight=48;base.numTiles=360;base.animSpeed=255;
const state=base.states[0];state.animationType='fixed';
const frames=state.animations[0].frames;
state.animations[0].frames=Array.from({length:240},(_,i)=>({id:frames[i]?.id||randomUUID(),tiles:[0,1,2,3,4,5].map((t)=>({id:frames[i]?.tiles[t]?.id||randomUUID(),x:(t%2)*8,y:32-Math.floor(t/2)*16,sliceX:i*16+(t%2)*8,sliceY:Math.floor(t/2)*16,flipX:false,flipY:false,palette:0,paletteIndex:Math.floor(i/20)%6===4 && t<2?7:Math.floor(i/20)%6,objPalette:'OBP0',priority:false}))}));
base.checksum=createHash('sha1').update(fs.readFileSync('game/assets/sprites/players.png')).digest('hex');
write(playerPath,base);
for(const [name,slot] of [['ball',6],['marker',6]]) {
 const p=`game/assets/sprites/${name}.png.gbsres`,s=read(p);
 for(const a of s.states[0].animations)for(const f of a.frames)for(const t of f.tiles)t.paletteIndex=slot;
 write(p,s);
}
{
 const p='game/assets/sprites/ball.png.gbsres',s=read(p),a=s.states[0].animations[0],old=a.frames;
 a.frames=[old[0],{id:old[1]?.id||randomUUID(),tiles:[0,1].map(t=>({...old[0].tiles[t],id:old[1]?.tiles[t]?.id||randomUUID(),x:t*8,y:0,sliceX:16+t*8,sliceY:0,paletteIndex:6}))}];
 s.width=32;s.numTiles=6;s.checksum=createHash('sha1').update(fs.readFileSync('game/assets/sprites/ball.png')).digest('hex');write(p,s);
}
const skin=['D69C74','C99870','E8B492','C99368','E5B493','D7A079'];
for(const [i,name] of ['matus','magic','bambi','torres','chompa','meat'].entries())
 write(`game/project/palettes/jam-player-${name}.gbsres`,{_resourceType:'palette',id:`jam-player-${name}`,name:`${name.toUpperCase()} skin and kit`,colors:[skin[i],'EAEFF7','EAEFF7','182738']});
{
 const p='game/assets/sprites/marker.png.gbsres',s=read(p),old=s.states[0].animations[0].frames;
 s.width=96;s.numTiles=12;
 s.states[0].animations[0].frames=Array.from({length:6},(_,i)=>({id:old[i]?.id||randomUUID(),tiles:[{...old[0].tiles[0],id:old[i]?.tiles[0]?.id||randomUUID(),sliceX:i*16,paletteIndex:6}]}));
 s.checksum=createHash('sha1').update(fs.readFileSync('game/assets/sprites/marker.png')).digest('hex');write(p,s);
}
for(const [id,name,colors] of [
 ['jam-court','Managua court',['FFF3CF','DDB377','B87C47','162638']],
 ['jam-ball','Ball and marker',['FFD270','E89328','E89328','302820']],
 ['jam-head-chompa','CHOMPA green beanie',[skin[4],'94C83D','94C83D','182738']]
])write(`game/project/palettes/${id}.gbsres`,{_resourceType:'palette',id,name,colors});
const settings=read('game/project/settings.gbsres');
settings.defaultSceneTypeId='BASKETBALL';settings.defaultPlayerSprites.BASKETBALL=read('game/assets/sprites/net.png.gbsres').id;
settings.defaultSpritePaletteIds=['jam-player-matus','jam-player-magic','jam-player-bambi','jam-player-torres','jam-player-chompa','jam-player-meat','jam-ball','jam-head-chompa'];
settings.defaultBackgroundPaletteIds[0]='jam-court';
write('game/project/settings.gbsres',settings);
const scenePath='game/project/scenes/start/scene.gbsres',scene=read(scenePath);
scene.type='BASKETBALL';scene.name='Managua Court';scene.symbol='scene_managua_court';scene.width=50;scene.height=18;scene.backgroundId=read('game/assets/backgrounds/court.png.gbsres').id;scene.collisions='';scene.paletteIds=['jam-court'];scene.spritePaletteIds=settings.defaultSpritePaletteIds;scene.playerSpriteSheetId=settings.defaultPlayerSprites.BASKETBALL;
write(scenePath,scene);
const bgPath='game/assets/backgrounds/court.png.gbsres',bg=read(bgPath);
bg.width=50;bg.imageWidth=400;bg.tileColors='';write(bgPath,bg);
{
 const path='game/assets/sprites/net.png.gbsres';
 const existing=fs.existsSync(path),net=read(existing?path:'game/assets/sprites/ball.png.gbsres');
 if(!existing){net.id=randomUUID();net.filename='net.png';net.symbol='sprite_reactive_net';net.states[0].id=randomUUID();net.states[0].animations.forEach(a=>a.id=randomUUID());}
 const a=net.states[0].animations[0],old=a.frames;
 a.frames=Array.from({length:4},(_,i)=>({id:old[i]?.id||randomUUID(),tiles:[{...(old[i]?.tiles[0]||old[0].tiles[0]),id:old[i]?.tiles[0]?.id||randomUUID(),sliceX:i*16,paletteIndex:6}]}));
 net.name='Reactive basket net';net.width=64;net.numTiles=4;net.checksum=createHash('sha1').update(fs.readFileSync('game/assets/sprites/net.png')).digest('hex');write(path,net);
}
console.log(`Native custom scene and editable crew sprite configured (${base.id}).`);

// Four shared body actors; PLAYER is the net, avoiding a second full crew load.
for(const [name,sprite,label] of [['ball','players','Depth slot 4'],['human_indicator','ball','Ball'],['reactive_net','marker','Human indicator']]) {
 const p=`game/project/scenes/start/actors/${name}.gbsres`,a=read(p);
 a.spriteSheetId=read(`game/assets/sprites/${sprite}.png.gbsres`).id;a.name=label;write(p,a);
}
