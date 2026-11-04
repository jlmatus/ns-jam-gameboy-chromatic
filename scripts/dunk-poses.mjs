import {bodyCells} from './player-poses.mjs';
import {bodyProfiles} from './create-body-profiles.mjs';

// Dunk-only 24x56 cels. Court animation and approved portrait cells stay intact.
// Joint keyframes articulate the body; no whole-sprite rotation or smoothing.
//
// Every player has a signature dunk on the shared 64-update simulation
// timeline: gather 0-8, rise 8-20, finish 20-28 (impact at 28), rim hang,
// drop and landing. Only the art differs; rules, scoring and blocks do not.
//
// Arm keys: t time, c crouch, l torso lean, w near wrist, f far wrist,
// e/fe optional near/far elbows (default: a bend between shoulder and wrist),
// h head view from this key on (front, side, back, sideL). Arm y values are
// relative to the top of the head; x values and all leg values are cel cells.
// The cel faces right, toward the rim; left-basket frames are mirrored later.
const k=(t,c,l,w,f,more={})=>({t,c,l,w,f,...more});
const LANDING=[
  k(40,0,0,[19,20],[5,22],{e:[21,13]}),
  k(46,5,2,[21,24],[5,25],{e:[21,19]}),
  k(52,0,0,[22,5],[4,14],{e:[21,11]}),
  k(60,0,0,[19,20],[6,23],{e:[20,14]}),
  k(64,0,0,[19,20],[6,23],{e:[20,14]})
];
// t, left knee, left foot, right knee, right foot: scissor kick and landing.
const SCISSOR=[
  [0,[7,45],[8,54],[18,45],[19,54]],
  [6,[5,45],[8,54],[19,46],[20,54]],
  [10,[5,42],[2,45],[17,47],[15,54]],
  [16,[3,42],[1,44],[18,44],[21,49]],
  [20,[7,43],[6,51],[18,40],[22,43]],
  [26,[8,45],[10,54],[19,43],[23,48]],
  [28,[8,45],[10,54],[18,42],[21,48]],
  [32,[5,43],[3,49],[17,44],[20,51]],
  [36,[9,46],[12,54],[17,42],[20,46]],
  [40,[8,45],[7,54],[17,45],[17,54]],
  [46,[6,46],[9,54],[19,46],[20,54]],
  [52,[9,46],[8,54],[17,46],[18,54]],
  [64,[9,46],[8,54],[17,46],[18,54]]
];
const legsWith=(replace)=>{
  const keep=SCISSOR.filter(([t])=>!replace.some(([r])=>r===t));
  return [...keep,...replace].sort((a,b)=>a[0]-b[0]);
};
const HANG=(a,b,l=2)=>[k(28,0,l,a,b,{e:[a[0],a[1]+5]}),k(32,0,1,a,b,{e:[a[0],a[1]+5]}),k(36,0,0,a,b,{e:[a[0],a[1]+5]})];
const GATHER=[k(0,3,-2,[20,24],[14,25],{h:'front'}),k(6,6,-3,[20,23],[15,25])];

export const dunkStyles=[
  // MATUS: rock-the-cradle. The ball swings low and back, then up and in.
  {name:'cradle jam',hands:1,keys:[...GATHER,
    k(10,0,1,[19,27],[6,20],{e:[18,19]}),
    k(13,0,0,[10,25],[17,20],{e:[14,18]}),
    k(16,0,-1,[4,19],[19,17],{e:[9,15]}),
    k(19,0,1,[14,25],[6,18],{e:[14,17]}),
    k(22,0,2,[22,11],[5,19],{e:[20,13]}),
    k(25,0,3,[22,-6],[5,20],{e:[21,2]}),
    k(26,0,3,[22,-2],[5,22],{e:[21,2]}),
    ...HANG([20,-8],[4,18]),...LANDING],legs:SCISSOR},
  // MAGIC: windmill. The straight arm circles front, down, back and over.
  {name:'windmill',hands:1,keys:[...GATHER,
    k(10,0,1,[24,6],[5,18]),
    k(12,0,1,[19,21],[4,12]),
    k(14,0,0,[7,19],[19,12]),
    k(16,0,-1,[2,7],[20,10]),
    k(18,0,-1,[5,-6],[20,12]),
    k(20,0,0,[14,-11],[5,18]),
    k(23,0,2,[22,-7],[5,20]),
    k(26,0,3,[23,-1],[5,23],{e:[21,0]}),
    ...HANG([20,-8],[17,-8]),...LANDING],legs:SCISSOR},
  // BAMBI: two-hand power slam with tucked knees and the heaviest impact.
  {name:'two-hand power',hands:2,shake:2,keys:[
    k(0,3,-1,[17,17],[11,17],{h:'front'}),k(6,6,-2,[17,18],[11,18]),
    k(10,0,0,[16,-2],[11,-2],{e:[18,4],fe:[9,3]}),
    k(14,0,-2,[11,-9],[7,-8],{e:[15,0],fe:[7,1]}),
    k(18,0,-2,[10,-10],[6,-9],{e:[15,-1],fe:[6,0]}),
    k(21,0,1,[19,-10],[15,-10],{e:[19,-1],fe:[12,-1]}),
    k(24,0,3,[22,-4],[19,-4],{e:[21,2],fe:[15,2]}),
    k(26,0,3,[22,-1],[19,0],{e:[21,3],fe:[15,3]}),
    ...HANG([20,-8],[17,-8]),...LANDING],
    legs:legsWith([[12,[13,40],[8,46],[17,41],[12,48]],[16,[14,40],[9,46],[17,40],[12,47]],
      [20,[14,40],[9,46],[17,40],[13,47]],[24,[12,42],[9,49],[17,42],[15,50]]])},
  // CHRUX: reverse jam. He drifts under the rim and slams back over his head.
  {name:'reverse jam',hands:2,reach:2,keys:[...GATHER,
    k(10,0,0,[18,6],[14,9],{e:[20,12]}),
    k(14,0,1,[17,-4],[12,-3],{e:[19,3],fe:[11,2]}),
    k(18,0,2,[17,-10],[13,-10],{e:[19,-1],fe:[11,-1]}),
    k(22,0,3,[15,-11],[11,-10],{e:[19,-2],fe:[10,-1]}),
    k(25,0,4,[12,-8],[8,-7],{e:[19,-1],fe:[9,0]}),
    k(26,0,4,[11,-5],[7,-4],{e:[19,0],fe:[9,1]}),
    ...[28,32,36].map(t=>k(t,0,t===28?4:t===32?3:2,[12,-8],[8,-8],{e:[18,-1],fe:[9,0]})),
    ...LANDING],legs:SCISSOR},
  // CHOMPA: two-hand stuff, then a long rim hang with swinging legs.
  {name:'rim hang',hands:2,hangEnd:44,drop:6,keys:[
    k(0,3,-2,[19,22],[15,22],{h:'front'}),k(6,6,-3,[19,23],[15,23]),
    k(10,0,0,[19,4],[15,4],{e:[20,8],fe:[12,7]}),
    k(14,0,0,[18,-8],[14,-8],{e:[19,0],fe:[11,0]}),
    k(20,0,1,[20,-10],[16,-10],{e:[20,-1],fe:[12,-1]}),
    k(24,0,2,[22,-5],[18,-5],{e:[21,1],fe:[14,1]}),
    k(26,0,2,[22,-1],[18,-1],{e:[21,3],fe:[14,3]}),
    ...[28,31,34,37,40,43].map((t,i)=>k(t,0,[2,1,-1,1,-1,0][i],[20,-8],[16,-8],{e:[20,-3],fe:[15,-3]})),
    k(46,0,0,[19,18],[5,20],{e:[21,12]}),
    k(50,5,2,[21,24],[5,25],{e:[21,19]}),
    k(56,0,0,[22,5],[4,14],{e:[21,11]}),
    k(64,0,0,[19,20],[6,23],{e:[20,14]})],
    legs:[...SCISSOR.filter(([t])=>t<28),
      [28,[11,45],[14,52],[15,45],[18,52]],[31,[13,45],[18,51],[16,45],[21,50]],
      [34,[8,46],[4,52],[11,46],[7,52]],[37,[13,45],[18,51],[16,45],[21,50]],
      [40,[8,46],[4,52],[11,46],[7,52]],[43,[10,46],[10,53],[15,46],[16,53]],
      [46,[9,46],[9,54],[16,46],[17,54]],[50,[6,46],[9,54],[19,46],[20,54]],
      [56,[9,46],[8,54],[17,46],[18,54]],[64,[9,46],[8,54],[17,46],[18,54]]]},
  // MEAT: 360. Front, profile, back, opposite profile, then a one-hand slam.
  {name:'360 spin',hands:1,spin:true,keys:[...GATHER,
    k(10,0,0,[20,15],[15,16],{h:'side',e:[19,10]}),
    k(13,0,0,[19,18],[5,18],{h:'back'}),
    k(16,0,0,[5,14],[9,16],{h:'sideL',e:[10,11]}),
    k(19,0,0,[18,-4],[5,15],{h:'front',e:[19,3]}),
    k(22,0,1,[18,-10],[5,18],{e:[17,-2]}),
    k(26,0,3,[22,-1],[5,23],{e:[21,0]}),
    ...HANG([20,-8],[17,-8]),...LANDING],
    legs:legsWith([[12,[9,42],[7,48],[15,42],[17,48]],[16,[9,42],[7,48],[15,42],[17,48]]])}
];

const lerp=(a,b,q)=>Math.round(a+(b-a)*q);
const between=(keys,time)=>{
  let a=keys[0],b=keys[0];
  for(let i=1;i<keys.length;i++){if(keys[i].t>=time){a=keys[i-1];b=keys[i];break;}a=b=keys[i];}
  return [a,b,b.t===a.t?0:Math.min(1,Math.max(0,(time-a.t)/(b.t-a.t)))];
};
function headView(keys,time){let h='front';for(const key of keys)if(key.t<=time&&key.h)h=key.h;return h;}
function headRows(player,view){
  const pose=view==='back'?4:0,body=bodyCells(player,pose,false,false,view==='front'||view==='back');
  const y0=48-bodyProfiles[player].height_px,rows=body.slice(y0,y0+8).map(r=>r.slice(3,13));
  return view==='sideL'?rows.map(r=>[...r].reverse().join('')):rows;
}

export function dunkCells(player, time, failed=false, reverseJersey=false) {
  const width=24,height=56,c=Array.from({length:height},()=>Array(width).fill('.'));
  const profile=bodyProfiles[player],style=dunkStyles[player];
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
  // Elbows default to a bend between shoulder and wrist, resolved per key.
  const elbows=key=>({e:key.e??[Math.round((16+key.l+key.w[0])/2)+1,Math.round((8+key.w[1])/2)+1],
    fe:key.fe??[Math.round((8+key.l+key.f[0])/2)-1,Math.round((8+key.f[1])/2)+2]});
  const [a,b,q]=between(style.keys,time),ea=elbows(a),eb=elbows(b);
  const crouch=lerp(a.c,b.c,q),lean=lerp(a.l,b.l,q);
  const pt=(p,r)=>[lerp(p[0],r[0],q),lerp(p[1],r[1],q)];
  const headY=56-profile.height_px+crouch,shoulder=headY+8,hip=37+crouch,view=headView(style.keys,time);
  const bulk=profile.mass>=3?2:profile.mass===1?0:1,side=view==='side'||view==='sideL';
  const rel=p=>[p[0],headY+p[1]];
  let wrist=rel(pt(a.w,b.w)),far=rel(pt(a.f,b.f)),elbow=rel(pt(ea.e,eb.e)),farElbow=rel(pt(ea.fe,eb.fe));
  if(failed&&time>=28){wrist=[22,headY+17];far=[2,headY+15];elbow=[23,headY+10];farElbow=[4,headY+12];}
  let li=0;while(li<style.legs.length-2&&style.legs[li+1][0]<=time)li++;
  const la=style.legs[li],lb=style.legs[li+1],lq=lb[0]===la[0]?0:Math.min(1,Math.max(0,(time-la[0])/(lb[0]-la[0])));
  const leg=i=>[lerp(la[i][0],lb[i][0],lq),lerp(la[i][1],lb[i][1],lq)];
  const left=[[10,hip],leg(1),leg(2)],right=[[15,hip],leg(3),leg(4)];
  for(const one of [left,right]){limb(one);stroke([[one[2][0]-1,one[2][1]],[one[2][0]+2,one[2][1]]],1.2,'s');}
  limb([[8+lean,shoulder],farElbow,far]);
  const trim=side?1:0;
  polygon([[7+lean-bulk+trim,shoulder-1],[16+lean+bulk-trim,shoulder],[17+bulk-trim,hip-2],[15+bulk-trim,hip+3],[6-bulk+trim,hip+2]],'#');
  polygon([[8+lean-bulk+trim,shoulder],[15+lean+bulk-trim,shoulder+1],[15+bulk-trim,hip-1],[8-bulk+trim,hip-1]],'s');
  polygon([[7-bulk,hip-2],[16+bulk,hip-2],[18+bulk,hip+3],[13,hip+4],[11,hip+1],[5-bulk,hip+3]],'#');
  polygon([[8-bulk,hip-1],[15+bulk,hip-1],[16+bulk,hip+2],[13,hip+2],[11,hip],[7-bulk,hip+2]],'s');
  stroke([[12+lean,shoulder],[12+lean,headY+6]],1,'o');
  const head=headRows(player,view);
  for(let y=0;y<8;y++)for(let x=0;x<10;x++){const v=head[y][x];if(v!=='.')put(x+7+lean,headY+y,v);}
  // Neck, face and jersey retain the six canonical identity cues. Profiles
  // hide the chest number; the back of the jersey carries it too.
  const number=['13','32','23','7','74','00'][player];
  const numerals={'0':['111','101','101','101','111'],'1':['010','110','010','010','111'],'2':['110','001','010','100','111'],'3':['110','001','010','001','110'],'4':['101','101','111','001','001'],'7':['111','001','010','010','010']};
  if(!side)[...number].forEach((d,i)=>numerals[d].forEach((row,y)=>[...row].forEach((v,x)=>{
    const dx=(number.length===1?10:8)+i*4+x;
    if(v==='1')put(reverseJersey?23-dx:dx,shoulder+3+y,'#');
  })));
  limb([[16+lean,shoulder],elbow,wrist]);
  // Two-hand dunks carry the ball between both wrists.
  const hand=style.hands===2?[Math.round((wrist[0]+far[0])/2),Math.round((wrist[1]+far[1])/2)]:wrist;
  return {cells:c.map(row=>row.join('')),hand,headY,headX:7+lean,view,style:style.name};
}
