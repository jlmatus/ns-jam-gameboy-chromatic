// Run after plugin rom_build with captureDebugArtifacts:true.
// This checks static headroom; native replay still verifies deep banked calls.
import fs from 'node:fs';

const path = process.argv[2] ?? 'game/build/project.gb.debug/symbols.noi';
const source = fs.readFileSync(path, 'utf8');
const symbol = name => {
    const line = source.split(/\r?\n/).find(line => line.startsWith(`DEF ${name} `));
    if (!line) throw new Error(`Missing ${name} in ${path}`);
    return Number(line.trim().split(/\s+/)[2]);
};
const headroom = symbol('.STACK') - symbol('s__HEAP');
// The failing build had 172 bytes. Preserve the verified 512-byte reserve.
if (headroom < 512) throw new Error(`Native stack headroom ${headroom} bytes is below the 512-byte budget`);
console.log(`PASS: native stack headroom ${headroom} bytes (minimum 512)`);
