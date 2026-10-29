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
export function bodyCells(player, pose, mirrored) {
  let c=Array.from({length:BODY_H},()=>Array(BODY_W).fill('.'));
  const put=(x,y,v)=>{if(x>=0&&x<BODY_W&&y>=0&&y<BODY_H)c[y][x]=v;};
  const disc=(x,y,r,v)=>{for(let yy=Math.floor(y-r);yy<=y+r;yy++)for(let xx=Math.floor(x-r);xx<=x+r;xx++)if((xx-x)**2+(yy-y)**2<=r*r)put(xx,yy,v);};
  const stroke=(points,r,v)=>{for(let i=1;i<points.length;i++){
    const [x,y]=points[i-1],[xx,yy]=points[i],n=Math.max(Math.abs(xx-x),Math.abs(yy-y));
    for(let t=0;t<=n;t++)disc(Math.round(x+(xx-x)*t/(n||1)),Math.round(y+(yy-y)*t/(n||1)),r,v);
  }};
  const limb=points=>{stroke(points,1.5,'#');stroke(points,.8,'o');};
  const polygon=(pts,v)=>{for(let y=0;y<BODY_H;y++)for(let x=0;x<BODY_W;x++){
    let inside=false;
    for(let i=0,j=pts.length-1;i<pts.length;j=i++){
      const [a,b]=pts[i],[d,e]=pts[j];
      if((b>y)!=(e>y)&&x<(d-a)*(y-b)/(e-b)+a)inside=!inside;
    }
    if(inside)put(x,y,v);
  }};
  const shoe=(x,y)=>{stroke([[x-1,y],[x+2,y]],1,'#');stroke([[x-1,y-1],[x+1,y-1]],.6,'s');};
  const crouch=pose===5||pose===9?4:0;
  const lean=pose===1?2:pose===2?1:0;
  const front=pose===3,back=pose===4;
  const profile=bodyProfiles[player];
  const bulk=pose<5?(profile.mass===4?1:profile.mass===1?-1:0):0;
  const headY=48-profile.height_px+crouch,shoulder=headY+8,hip=29+crouch;
  // Legs bend at separate knee and ankle joints. Strides trade the planted foot.
  let legs;
  if(pose===1)legs=[[[7,hip],[4,38],[1,46]],[[9,hip],[12,36],[10,41]]];
  else if(pose===2)legs=[[[6,hip],[3,36],[4,41]],[[9,hip],[11,38],[14,46]]];
  else if(front||back)legs=[[[5,hip],[4,38],[3,46]],[[10,hip],[12,37],[11,43]]];
  else if(crouch)legs=[[[6,hip],[3,39],[5,46]],[[9,hip],[12,39],[12,46]]];
  else legs=[[[6,hip],[5,38],[4,46]],[[9,hip],[10,38],[11,46]]];
  legs.forEach(points=>{limb(points);shoe(...points.at(-1));});
  // Far arm behind the jersey; near arm is articulated over it.
  const arms=pose===1?[[[6,shoulder+1],[3,23],[1,20]],[[10,shoulder],[12,23],[14,22]]]:
    pose===2?[[[6,shoulder],[4,23],[2,25]],[[11,shoulder],[13,20],[14,17]]]:
    pose===5?[[[5,shoulder],[8,24],[12,23]],[[10,shoulder],[12,25],[14,22]]]:
    pose===6?[[[5,shoulder],[10,17],[12,10]],[[10,shoulder],[13,15],[13,9]]]:
    pose===7?[[[5,shoulder],[9,13],[12,3]],[[10,shoulder],[13,9],[14,1]]]:
    pose===8?[[[5,shoulder],[8,14],[10,7]],[[10,shoulder],[13,10],[13,2]]]:
    front||back?[[[4,shoulder],[2,24],[3,28]],[[11,shoulder],[13,21],[14,17]]]:
    [[[5,shoulder],[3,24],[5,28]],[[10,shoulder],[12,23],[14,24]]];
  limb(arms[0]);
  polygon([[4+lean-bulk,shoulder-1],[11+lean+bulk,shoulder],[12+bulk,hip-2],[10+bulk,hip+3],[3-bulk,hip+2]],'#');
  polygon([[5+lean-bulk,shoulder],[10+lean+bulk,shoulder+1],[10+bulk,hip-1],[5-bulk,hip-1]],'s');
  // Shorts, waistband, and separate leg openings.
  polygon([[4,hip-2],[11,hip-2],[12,hip+3],[8,hip+4],[7,hip+2],[3,hip+3]],'#');
  polygon([[5,hip-1],[10,hip-1],[11,hip+2],[8,hip+2],[7,hip],[4,hip+2]],'s');
  stroke([[7+lean,shoulder],[7+lean,headY+6]],1,'o');
  const head=back?['.####..','######.','######.','######.','.oooo#.','..oo#..']:
    front?['.####..','######.','#oooo#.','#o#o##.','#oooo#.','.oo##..']:
    ['.####..','######.','##ooo#.','#ooo#o#','#ooooo#','.ooo##.'];
  head.forEach((row,y)=>[...row].forEach((v,x)=>{if(v!=='.')put(x+5+lean,y+headY,v);}));
  // Identity stays secondary to the silhouette at this scale.
  if(player===1||player===4){put(10+lean,headY+3,'#');put(11+lean,headY+3,'#');}
  if(player===3||player===2)for(let x=7;x<11;x++)put(x+lean,headY+5,'#');
  if(player===4)for(let x=5;x<11;x++)put(x+lean,headY+1,'s');
  limb(arms[1]);
  if(mirrored)c=c.map(r=>r.toReversed());
  // Redraw numerals AFTER turning, so jerseys never become mirror writing.
  const jersey=['13','32','23','7','74','00'][player];
  if(pose===0 || pose===3 || pose===4)[...jersey].forEach((d,i)=>digits[d].forEach((row,y)=>[...row].forEach((v,x)=>{
    if(v==='1')put((jersey.length===1?6:4)+i*4+x,shoulder+3+y,'#');
  })));
  return c.map(r=>r.join(''));
}
