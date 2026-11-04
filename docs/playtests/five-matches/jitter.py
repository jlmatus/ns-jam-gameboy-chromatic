"""Count camera back-and-forth episodes in a per-frame trace (JAM_TRACE=file.npy play_match.py ...)."""
import numpy as np, sys, collections
t=np.load(sys.argv[1]); scx=t[:,1].astype(int)
d=np.diff(scx); d[d>128]-=256; d[d<-128]+=256
sgn=np.sign(d)
# reversal = nonzero move opposite the previous nonzero move within 3 frames
rev=[]
last=0;lastf=-99
for i,s in enumerate(sgn):
    if s==0: continue
    if last and s!=last and i-lastf<=3: rev.append(i)
    last=s;lastf=i
rev=np.array(rev)
# episodes: >=4 reversals within 30 frames
eps=[];i=0
while i<len(rev):
    j=i
    while j+1<len(rev) and rev[j+1]-rev[j]<=10: j+=1
    if j-i>=3: eps.append((rev[i],rev[j],j-i+1))
    i=j+1
print('frames',len(t),'reversals',len(rev),'episodes',len(eps))
modes=collections.Counter()
for a,b,k in eps[:40]:
    row=t[a]; seg=t[a:b+1]
    # which body moves back and forth in x the most
    xs=seg[:,5:13:2]; flips=[int(np.sum(np.abs(np.diff(np.sign(np.diff(xs[:,p]))))>0)) for p in range(4)]
    bx=seg[:,13]; bflip=int(np.sum(np.abs(np.diff(np.sign(np.diff(bx))))>0))
    modes[int(row[2])]+=1
    print(f"f{row[0]} len{b-a+1} rev{k} mode{row[2]} owner{row[3]} ctrl{row[4]} xflips{flips} ballflips{bflip} stun{list(row[15:19])} push{list(row[19:23])} pos{[tuple(seg[0,5+2*p:7+2*p]//4) for p in range(4)]}")
print(modes)
