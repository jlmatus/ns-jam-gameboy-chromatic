import {bodyCells} from './player-poses.mjs';
import {bodyProfiles} from './create-body-profiles.mjs';

// Dunk-only 24x56 cels. Court animation and approved portrait cells stay intact.
// Joint keyframes articulate the body; no whole-sprite rotation or smoothing.
export function dunkCells(player, time, failed=false, reverseJersey=false) {
  const width=24,height=56,c=Array.from({length:height},()=>Array(width).fill('.'));
  const profile=bodyProfiles[player],power=player===2||player===4;
  const put=(x,y,v)=>{x=Math.round(x);y=Math.round(y);if(x>=0&&x<width&&y>=0&&y<height)c[y][x]=v;};
  const disc=(x,y,r,v)=>{for(let yy=Math.floor(y-r);yy<=y+r;yy++)for(let xx=Math.floor(x-r);xx<=x+r;xx++)if((xx-x)**2+(yy-y)**2<=r*r)put(xx,yy,v);};
  const stroke=(points,r,v)=>{for(let i=1;i<points.length;i++){
    const [x,y]=points[i-1],[xx,yy]=points[i],n=Math.max(Math.abs(xx-x),Math.abs(yy-y));
    for(let k=0;k<=n;k++)disc(Math.round(x+(xx-x)*k/(n||1)),Math.round(y+(yy-y)*k/(n||1)),r,v);
  }};
  const limb=points=>{stroke(points,1.7,'#');stroke(points,1,'o');};
  const polygon=(points,v)=>{for(let y=0;y<height;y++)for(let x=0;x<width;x++){
    let inside=false;for(let i=0,j=points.length-1;i<points.length;j=i++){
      const [a,b]=points[i],[d,e]=points[j];if((b>y)!=(e>y)&&x<(d-a)*(y-b)/(e-b)+a)inside=!inside;
    }if(inside)put(x,y,v);
  }};
  // time, crouch, torso lean, near elbow/wrist, far wrist, left knee/foot,
  // right knee/foot. Airborne keyframes scissor and then swing under the rim.
  const keys=[
    [0,3,-2,20,21,20,25,13,26,7,45,8,54,18,45,19,54],
    [6,6,-3,20,22,21,24,16,25,5,45,8,54,19,46,20,54],
    [10,0,1,19,8,17,0,5,22,5,42,2,45,17,47,15,54],
    [16,0,-2,7,2,7,-6,21,15,3,42,1,44,18,44,21,49],
    [20,0,0,16,-1,18,-9,5,20,7,43,6,51,18,40,22,43],
    [26,0,3,21,0,22,-1,5,23,8,45,10,54,19,43,23,48],
    [28,0,2,20,-4,20,-8,17,-8,8,45,10,54,18,42,21,48],
    [32,0,1,19,-4,20,-8,17,-8,5,43,3,49,17,44,20,51],
    [36,0,0,19,-4,20,-8,17,-8,9,46,12,54,17,42,20,46],
    [40,0,0,21,13,19,20,5,22,8,45,7,54,17,45,17,54],
    [46,5,2,21,19,21,24,5,25,6,46,9,54,19,46,20,54],
    [52,0,0,21,11,22,5,4,14,9,46,8,54,17,46,18,54],
    [60,0,0,20,14,19,20,6,23,9,46,8,54,17,46,18,54],
    [64,0,0,20,14,19,20,6,23,9,46,8,54,17,46,18,54]
  ];
  let a=keys[0],b=keys[1];for(let i=1;i<keys.length;i++)if(time>=keys[i-1][0]&&time<=keys[i][0]){a=keys[i-1];b=keys[i];break;}
  const q=(time-a[0])/(b[0]-a[0]),k=a.map((v,i)=>Math.round(v+(b[i]-v)*q));
  const headY=56-profile.height_px+k[1],lean=k[2],shoulder=headY+8,hip=37+k[1];
  const bulk=profile.mass>=3?2:profile.mass===1?0:1;
  let wrist=[k[5],headY+k[6]],far=[k[7],headY+k[8]],elbow=[k[3],headY+k[4]];
  if(power&&time>=10&&time<28){wrist=[Math.min(21,wrist[0]+1),wrist[1]+1];far=[wrist[0]-4,wrist[1]+1];}
  if(failed&&time>=28){wrist=[22,headY+17];far=[2,headY+15];elbow=[23,headY+10];}
  const left=[[10,hip],[k[9],k[10]],[k[11],k[12]]],right=[[15,hip],[k[13],k[14]],[k[15],k[16]]];
  for(const leg of [left,right]){limb(leg);stroke([[leg[2][0]-1,leg[2][1]],[leg[2][0]+2,leg[2][1]]],1.2,'s');}
  limb([[8+lean,shoulder],[(far[0]+8)/2,headY+13],far]);
  polygon([[7+lean-bulk,shoulder-1],[16+lean+bulk,shoulder],[17+bulk,hip-2],[15+bulk,hip+3],[6-bulk,hip+2]],'#');
  polygon([[8+lean-bulk,shoulder],[15+lean+bulk,shoulder+1],[15+bulk,hip-1],[8-bulk,hip-1]],'s');
  polygon([[7-bulk,hip-2],[16+bulk,hip-2],[18+bulk,hip+3],[13,hip+4],[11,hip+1],[5-bulk,hip+3]],'#');
  polygon([[8-bulk,hip-1],[15+bulk,hip-1],[16+bulk,hip+2],[13,hip+2],[11,hip],[7-bulk,hip+2]],'s');
  stroke([[12+lean,shoulder],[12+lean,headY+6]],1,'o');
  const base=bodyCells(player,0,false),baseY=48-profile.height_px;
  for(let y=0;y<8;y++)for(let x=0;x<10;x++){const v=base[baseY+y][x+3];if(v!=='.')put(x+7+lean,headY+y,v);}
  // Neck, face and jersey retain the six canonical identity cues.
  const number=['13','32','23','7','74','00'][player];
  const numerals={'0':['111','101','101','101','111'],'1':['010','110','010','010','111'],'2':['110','001','010','100','111'],'3':['110','001','010','001','110'],'4':['101','101','111','001','001'],'7':['111','001','010','010','010']};
  [...number].forEach((d,i)=>numerals[d].forEach((row,y)=>[...row].forEach((v,x)=>{
    const dx=(number.length===1?10:8)+i*4+x;
    if(v==='1')put(reverseJersey?23-dx:dx,shoulder+3+y,'#');
  })));
  limb([[16+lean,shoulder],elbow,wrist]);
  return {cells:c.map(row=>row.join('')),hand:wrist,headY,headX:7+lean,power};
}
