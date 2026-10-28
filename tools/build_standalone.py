"""Bundle the built sound check into a single downloadable HTML file."""
import base64
import json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
site=root/'build/web'
html=(site/'index.html').read_text()
css=(site/'style.css').read_text()
js=(site/'app.js').read_text()
worklet=json.dumps((site/'worklet.js').read_text())
wasm=base64.b64encode((site/'firmware.wasm').read_bytes()).decode()
js=js.replace("await ctx.audioWorklet.addModule('worklet.js');",
    f'const workletURL=URL.createObjectURL(new Blob([{worklet}],{{type:"text/javascript"}}));\n'
    '    try { await ctx.audioWorklet.addModule(workletURL); } finally { URL.revokeObjectURL(workletURL); }')
js=js.replace("const response=await fetch('firmware.wasm');if(!response.ok)throw Error('Could not load the WebAssembly build.');\n    const wasm=await response.arrayBuffer();",
    f'const wasm=Uint8Array.from(atob("{wasm}"),c=>c.charCodeAt(0)).buffer;')
html=html.replace('<link rel="stylesheet" href="style.css">',f'<style>{css}</style>')
html=html.replace('<script type="module" src="app.js"></script>',f'<script type="module">{js}</script>')
notices=(root/'vendor/foma/LICENSING.md').read_text()+'\n'+(root/'vendor/foma/platform/gen/Terminus-LICENSE.txt').read_text()
html+='\n<!-- Upstream notices (not all upstream components bundled):\n'+notices.replace('--','—')+'\n-->\n'
out=root/'preview/echolog-soundcheck.html';out.parent.mkdir(exist_ok=True)
out.write_text(html)
print('Built standalone sound check:',out)
