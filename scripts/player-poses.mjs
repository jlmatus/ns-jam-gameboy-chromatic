import {bodyProfiles} from './create-body-profiles.mjs';
// Editable native cells, anchored at the feet. No resampling or generated textures.
export const BODY_W = 16, BODY_H = 48, POSES = 10;
export const poseNames = ['ready','stride-a','stride-b','toward-camera','away-camera',
  'gather','set','release','follow-through','land'];
const digits = {
  '0':['111','101','101','101','111'],'1':['010','110','010','010','111'],
  '2':['110','001','010','100','111'],'3':['110','001','010','001','110'],
  '4':['101','101','111','001','001'],'7':['111','001','010','010','010']
};
export function bodyCells(player, pose, mirrored, reverseJersey=false) {
  let c=Array.from({length:BODY_H},()=>Array(BODY_W).fill('.'));
  const put=(x,y,v)=>{if(x>=0&&x<BODY_W&&y>=0&&y<BODY_H)c[y][x]=v;};
  const disc=(x,y,r,v)=>{for(let yy=Math.floor(y-r);yy<=y+r;yy++)for(let xx=Math.floor(x-r);xx<=x+r;xx++)if((xx-x)**2+(yy-y)**2<=r*r)put(xx,yy,v);};
  const stroke=(points,r,v)=>{for(let i=1;i<points.length;i++){
    const [x,y]=points[i-1],[xx,yy]=points[i],n=Math.max(Math.abs(xx-x),Math.abs(yy-y));
    for(let t=0;t<=n;t++)disc(Math.round(x+(xx-x)*t/(n||1)),Math.round(y+(yy-y)*t/(n||1)),r,v);
  }};
  const profile=bodyProfiles[player];
  const limb=points=>{stroke(points,1.5,'#');stroke(points,.8,'o');};
  const polygon=(pts,v,maxY=BODY_H)=>{for(let y=0;y<maxY;y++)for(let x=0;x<BODY_W;x++){
    let inside=false;
    for(let i=0,j=pts.length-1;i<pts.length;j=i++){
      const [a,b]=pts[i],[d,e]=pts[j];
      if((b>y)!=(e>y)&&x<(d-a)*(y-b)/(e-b)+a)inside=!inside;
    }
    if(inside)put(x,y,v);
  }};
  const shoe=(x,y)=>{stroke([[x-1,y],[x+2,y]],1,'#');stroke([[x-1,y-1],[x+1,y-1]],.6,'s');};
  const crouch=pose===5||pose===9||pose===10||pose===15?4:0;
  const lean=pose===1?2:pose===2?1:0;
  const front=pose===3,back=pose===4;
  // Build reads in ready/run/turn poses: heavy +2, lean -1 per side; broad and
  // stocky keep the base trunk so their tiles stay shared. Shot/contact poses
  // stay shared so the crew sheet fits below UI tile 192.
  const bulk=pose<5||pose>=10?(profile.mass===4?2:profile.mass===1?-1:0):0;
  const headY=48-profile.height_px+crouch,shoulder=headY+8,hip=29+crouch;
  // Legs bend at separate knee and ankle joints. Strides trade the planted foot.
  let legs;
  if(pose===11||pose===12)legs=[[[7,hip],[3,36],[1,40]],[[9,hip],[12,39],[10,46]]];
  else if(pose===13||pose===14)legs=[[[6,hip],[4,38],[6,46]],[[9,hip],[12,35],[14,40]]];
  else if(pose===1)legs=[[[7,hip],[4,38],[1,46]],[[9,hip],[12,36],[10,41]]];
  else if(pose===2)legs=[[[6,hip],[3,36],[4,41]],[[9,hip],[11,38],[14,46]]];
  else if(front||back)legs=[[[5,hip],[4,38],[3,46]],[[10,hip],[12,37],[11,43]]];
  else if(crouch)legs=[[[6,hip],[3,39],[5,46]],[[9,hip],[12,39],[12,46]]];
  else legs=[[[6,hip],[5,38],[4,46]],[[9,hip],[10,38],[11,46]]];
  legs.forEach(points=>{limb(points);shoe(...points.at(-1));});
  // Far arm behind the jersey; near arm is articulated over it.
  const arms=pose===11?[[[5,shoulder],[2,24],[1,26]],[[10,shoulder],[13,headY+8],[12,headY+1]]]:
    pose===12?[[[5,shoulder],[2,24],[1,25]],[[10,shoulder],[14,headY+2],[14,Math.max(0,headY-5)]]]:
    pose===13?[[[5,shoulder],[2,24],[3,28]],[[10,shoulder],[13,headY+2],[15,headY-4]]]:
    pose===14?[[[5,shoulder],[9,headY+6],[14,headY-4]],[[10,shoulder],[13,headY+3],[15,headY-4]]]:
    (pose===10||pose===15)?[[[5,shoulder],[3,26],[6,28]],[[10,shoulder],[13,25],[14,23]]]:
    pose===1?[[[6,shoulder+1],[3,23],[1,20]],[[10,shoulder],[12,23],[14,22]]]:
    pose===2?[[[6,shoulder],[4,23],[2,25]],[[11,shoulder],[13,20],[14,17]]]:
    (pose===5||pose===9)?[[[5,shoulder],[8,24],[12,23]],[[10,shoulder],[12,25],[14,22]]]:
    pose===6?[[[5,shoulder],[10,17],[12,10]],[[10,shoulder],[13,15],[13,9]]]:
    pose===7?[[[5,shoulder],[9,13],[12,3]],[[10,shoulder],[13,9],[14,1]]]:
    pose===8?[[[5,shoulder],[8,14],[10,7]],[[10,shoulder],[13,10],[13,2]]]:
    front||back?[[[4,shoulder],[2,24],[3,28]],[[11,shoulder],[13,21],[14,17]]]:
    [[[5,shoulder],[3,24],[5,28]],[[10,shoulder],[12,23],[14,24]]];
  limb(arms[0]);
  // Extra width stops above row 32 so leg tiles stay shared across the crew.
  const trunk=(b,h,maxY)=>{
    polygon([[4+lean-b,shoulder-1],[11+lean+b,shoulder],[12+b,hip-2],[10+b,hip+3],[3-b,hip+2]],'#',maxY);
    polygon([[5+lean-b,shoulder],[10+lean+b,shoulder+1],[10+b,hip-1],[5-b,hip-1]],'s',maxY);
    // Shorts, waistband, and separate leg openings.
    polygon([[4-h,hip-2],[11+h,hip-2],[12+h,hip+3],[8,hip+4],[7,hip+2],[3-h,hip+3]],'#',maxY);
    polygon([[5-h,hip-1],[10+h,hip-1],[11+h,hip+2],[8,hip+2],[7,hip],[4-h,hip+2]],'s',maxY);
  };
  if(bulk>0){trunk(0,0,BODY_H);trunk(bulk,bulk,32);}else trunk(bulk,0,BODY_H);
  stroke([[7+lean,shoulder],[7+lean,headY+6]],1,'o');
  // Ten-pixel heads retain readable portrait cues at handheld resolution.
  // Keep their height/foot anchor unchanged so reach and collision stay intact.
  const faces=[
    ['..#.#.#...','.#######..','##oooooo#.','#oo#oo#oo#','#oooooooo#','.oo####o#.','..oo##oo..','...####...'], // MATUS: spikes/goatee
    ['..######..','.########.','#oooooooo#','##########','#o#oo#oo##','.ooooooo#.','..oooooo..','...oooo...'], // MAGIC: glasses/clean jaw
    ['..######..','.##oooo##.','##oooooo##','#oo#oo#oo#','#oooooooo#','#oooooooo#','..oooooo..','...oooo...'], // BAMBI: receding hair/round clean face; MAGIC/MEAT chin shares tiles
    ['.########.','##########','#oooooooo#','#oo#oo#oo#','##oooooo##','###oooo###','.########.','..######..'], // CHRUX: crop/full beard
    ['.ssssssss.','ssssssssss','#oooooooo#','##########','#o#oo#oo##','#ooo##ooo#','.oo####oo.','..######..'], // CHOMPA: beanie/glasses/chin
    ['.###.###..','#########.','#oooooooo#','#oo#oo#oo#','#oooooooo#','.oo#oo#o#.','..oooooo..','...oooo...'] // MEAT: short textured hair
  ];
  const head=back?(player===4?['.ssssssss.','ssssssssss','ssssssssss','##########','##########','.########.','..oooooo..','...oooo...']:
    ['..######..','.########.','##########','##########','##########','.########.','..oooooo..','...oooo...']):faces[player];
  head.forEach((row,y)=>[...row].forEach((v,x)=>{if(v!=='.')put(x+3,y+headY,v);}));
  limb(arms[1]);
  if(mirrored)c=c.map(r=>r.toReversed());
  // Redraw numerals AFTER turning, so jerseys never become mirror writing.
  const jersey=['13','32','23','7','74','00'][player];
  if(pose===0 || pose===3 || pose===4 || pose>=10)[...jersey].forEach((d,i)=>digits[d].forEach((row,y)=>[...row].forEach((v,x)=>{
    const digitX=(jersey.length===1?6:4)+i*4+x;
    if(v==='1')put(reverseJersey?15-digitX:digitX,shoulder+3+y,'#');
  })));
  return c.map(r=>r.join(''));
}
