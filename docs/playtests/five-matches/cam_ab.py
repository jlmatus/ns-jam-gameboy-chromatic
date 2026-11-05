import json,glob,sys,os
S=os.path.dirname(os.path.abspath(__file__))
rows={}
# Reads the A/B summaries in cam-ab/ (old = previous build, cam = selective Dunk Cam build).
for f in sorted(glob.glob(S+'/cam-ab/*.json')):
    k=os.path.basename(f)[:-5]; rom,g,seed=k.split('-')
    d=json.load(open(f)); c=d['dunk_cam']
    sh=c['shots']
    def b(lo,hi,team=None): 
        x=[s for s in sh if lo<=s['dist']<hi and (team is None or s['team']==team)]; return len(x),sum(s['made'] for s in x)
    ms=[s for s in sh if s['who']=='MATUS' and s['dist']>=130]
    rows[(g,seed,rom)]=dict(score=d['final_score'],frames=d['frames'],dunks=len(c['dunks']),dframes=c['dunk_frames'],cframes=c['cam_frames'],
        camstart=sum(x['cam'] for x in c['dunks']),skipped=sum(1 for x in c['dunks'] if x['cam'] and not x['cam_end']),
        deep=b(130,999),mid=b(88,130),matus_deep=(len(ms),sum(s['made'] for s in ms)),made_dunks=d['totals']['dunks'],att=d['totals']['dunk_attempts'],phase=d['phase'])
tot={'old':{},'cam':{}}
for (g,seed,rom),r in sorted(rows.items()):
    print(g,seed,rom,r['score'],'dunks',r['dunks'],'cam',r['camstart'],'skip',r['skipped'],'camshare %.1f%%'%(100*r['cframes']/r['frames']),'dunkshare %.1f%%'%(100*r['dframes']/r['frames']),'deep',r['deep'],'88-130',r['mid'],'MATUSdeep',r['matus_deep'],'ph',r['phase'])
    t=tot[rom]
    for k in ('frames','dunks','dframes','cframes','camstart','skipped'): t[k]=t.get(k,0)+r[k]
    for k in ('deep','mid','matus_deep'): a=t.get(k,(0,0)); t[k]=(a[0]+r[k][0],a[1]+r[k][1])
for rom,t in tot.items():
    if t: print(rom,t,'camshare %.1f%%'%(100*t['cframes']/t['frames']),'cam per dunk frames %.0f'%(t['cframes']/max(1,t['camstart'])))
