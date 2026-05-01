// ============================================================
//  someEngine Visual Editor
// ============================================================

// ── CONSTANTS ────────────────────────────────────────────────
const LEVEL_W = 740, LEVEL_H = 600;
const SCREEN_W = 640, SCREEN_H = 480;
const GRID = 32;

const COLLISION_TYPES = ['TYPE_PLAYER', 'TYPE_WALL', 'TYPE_ENEMY', 'TYPE_PUSHOUT', 'TYPE_PUSHIN'];

const BG_PRESETS = [
  '#4a7c59', '#2d6a4f', '#7b5e3a', '#3a5e7b', '#5e3a7b',
  '#1a1a2e', '#3a3a3a', '#7b7b4a', '#5a2d2d', '#2d5a5a'
];

// ── STATE ─────────────────────────────────────────────────────
const S = {
  mode: 'level',         // 'level' | 'prefab' | 'character'
  bottomTab: 'timeline', // 'timeline' | 'code'

  // ─ Project data ─
  prefabs: [],
  characters: [],
  level: {
    name: 'Level 1',
    width: LEVEL_W,
    height: LEVEL_H,
    background: { type: 'color', color: '#4a7c59', imagePath: '' },
    entities: []   // { id, prefabId, x, y }
  },

  // ─ Level editor ─
  tool: 'select',           // 'select' | 'place' | 'erase'
  selectedEntityId: null,
  activePaletteId: null,    // prefab/char id chosen for placing
  showGrid: true,
  showCamera: true,
  snapToGrid: true,
  zoom: 1,
  panX: 20,
  panY: 20,
  isDragging: false,
  dragEntityId: null,
  dragOffX: 0,
  dragOffY: 0,
  isPanning: false,
  panStartMouseX: 0,
  panStartMouseY: 0,
  panStartX: 0,
  panStartY: 0,

  // ─ Prefab editor ─
  activePrefabId: null,

  // ─ Character editor ─
  activeCharId: null,
  activeAnimName: null,
  sheetZoom: 1,
  sheetPanX: 0,
  sheetPanY: 0,
  animPlaying: false,
  animFrame: 0,
  animTimer: null,
  pickingFrames: false,

  // ─ Image cache: id → HTMLImageElement ─
  imgCache: {}
};

let _uid = 1;
function uid() { return 'id_' + (_uid++); }

// ── DEFAULT DATA ──────────────────────────────────────────────
function buildDefaultProject() {
  S.prefabs = [
    {
      id: 'pf_obstacle', name: 'Obstacle', color: '#8B4513',
      spritePath: '', spriteData: null,
      frameX: 126, frameY: 64, frameW: 32, frameH: 32,
      scale: 3, xOffset: -50, yOffset: -50,
      displayW: 100, displayH: 100,
      colliders: [{ w: 80, h: 80, xOffset: 0, yOffset: 0, type: 'TYPE_PUSHOUT' }],
      ysort: false, type: 'static'
    },
    {
      id: 'pf_table', name: 'Table', color: '#DEB887',
      spritePath: '', spriteData: null,
      frameX: 0, frameY: 0, frameW: 48, frameH: 32,
      scale: 2, xOffset: -24, yOffset: -12,
      displayW: 96, displayH: 64,
      colliders: [{ w: 96, h: 19, xOffset: 0, yOffset: 10, type: 'TYPE_PUSHOUT' }],
      ysort: true, type: 'static'
    }
  ];
  S.characters = [
    {
      id: 'ch_player', name: 'Player', color: '#4169E1',
      spritePath: '', spriteData: null,
      sheetCols: 10, sheetRows: 12,
      frameW: 32, frameH: 32,
      scale: 2, xOffset: -16, yOffset: -27,
      displayW: 64, displayH: 64,
      colliders: [{ w: 30, h: 30, xOffset: 0, yOffset: -10, type: 'TYPE_PLAYER' }],
      animations: {
        'idle_down':  { frames: [], speed: 3, loop: true },
        'walk_down':  { frames: [], speed: 3, loop: true },
        'walk_left':  { frames: [], speed: 3, loop: true },
        'walk_up':    { frames: [], speed: 3, loop: true },
        'roll_down':  { frames: [], speed: 4, loop: false },
        'roll_left':  { frames: [], speed: 4, loop: false },
        'roll_up':    { frames: [], speed: 4, loop: false },
        'jump':       { frames: [], speed: 4, loop: false },
        'attack_h':   { frames: [], speed: 4, loop: false },
        'attack_v':   { frames: [], speed: 4, loop: false }
      },
      behaviorCode: `// Player behavior — maps to player.cpp
// Called each frame when a key event is received.
void handleInput(Player* player, ControllerManager* controller) {
    if (controller->isKeyPressed(KEY_PRESS_UP))    { /* move up */ }
    if (controller->isKeyPressed(KEY_PRESS_DOWN))  { /* move down */ }
    if (controller->isKeyPressed(KEY_PRESS_LEFT))  { /* move left */ }
    if (controller->isKeyPressed(KEY_PRESS_RIGHT)) { /* move right */ }
    if (controller->getLastKeyEvent() == KEY_PRESS_SHIFT) { /* roll */ }
    if (controller->getLastKeyEvent() == KEY_PRESS_SPACE) { /* jump */ }
}

// Called once per game-loop update.
void update(Player* player) {
    // physics, state transitions, etc.
}
`
    }
  ];
}

// ── IMAGE HELPERS ─────────────────────────────────────────────
function loadImg(dataUrl, id, cb) {
  if (!dataUrl) { cb && cb(null); return; }
  if (S.imgCache[id]) { cb && cb(S.imgCache[id]); return; }
  const img = new Image();
  img.onload = () => { S.imgCache[id] = img; cb && cb(img); };
  img.onerror = () => { cb && cb(null); };
  img.src = dataUrl;
}

function pickImageFile(cb) {
  const inp = document.createElement('input');
  inp.type = 'file'; inp.accept = 'image/*';
  inp.onchange = () => {
    const f = inp.files[0]; if (!f) return;
    const reader = new FileReader();
    reader.onload = e => cb(e.target.result, f.name);
    reader.readAsDataURL(f);
  };
  inp.click();
}

// ── CANVAS UTILS ──────────────────────────────────────────────
function resizeCanvas(canvas) {
  const wrap = document.getElementById('canvas-wrap');
  canvas.width = wrap.clientWidth;
  canvas.height = wrap.clientHeight;
}

function worldToCanvas(wx, wy) {
  return { x: (wx + S.panX) * S.zoom, y: (wy + S.panY) * S.zoom };
}
function canvasToWorld(cx, cy) {
  return { x: cx / S.zoom - S.panX, y: cy / S.zoom - S.panY };
}
function snapGrid(v) {
  return S.snapToGrid ? Math.round(v / GRID) * GRID : Math.round(v);
}
function getCanvasPos(e, canvas) {
  const r = canvas.getBoundingClientRect();
  return { x: e.clientX - r.left, y: e.clientY - r.top };
}

// find prefab or character by id
function findDef(id) {
  return S.prefabs.find(p => p.id === id) || S.characters.find(c => c.id === id) || null;
}
function allDefs() { return [...S.prefabs, ...S.characters]; }

// get bounding box of placed entity
function entityBounds(ent) {
  const def = findDef(ent.prefabId);
  if (!def) return { x: ent.x - 16, y: ent.y - 16, w: 32, h: 32 };
  return {
    x: ent.x + def.xOffset,
    y: ent.y + def.yOffset,
    w: def.displayW,
    h: def.displayH
  };
}

// ── NOTIFY ────────────────────────────────────────────────────
function notify(msg) {
  const el = document.getElementById('notification');
  el.textContent = msg; el.classList.remove('hidden');
  clearTimeout(el._t);
  el._t = setTimeout(() => el.classList.add('hidden'), 2000);
}

// ============================================================
//  LEVEL EDITOR
// ============================================================

function drawLevel() {
  const canvas = document.getElementById('main-canvas');
  const ctx = canvas.getContext('2d');
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  const bg = S.level.background;

  // draw level rect (background)
  const tl = worldToCanvas(0, 0);
  const br = worldToCanvas(S.level.width, S.level.height);
  const lw = br.x - tl.x, lh = br.y - tl.y;

  // fill background color (always drawn as base)
  ctx.fillStyle = bg.color || '#4a7c59';
  ctx.fillRect(tl.x, tl.y, lw, lh);

  // draw background image if set
  if (bg.type === 'image' && bg.spriteData) {
    const cachedImg = S.imgCache['bg'];
    if (cachedImg) {
      ctx.save();
      ctx.beginPath(); ctx.rect(tl.x, tl.y, lw, lh); ctx.clip();
      ctx.drawImage(cachedImg, tl.x, tl.y, lw, lh);
      ctx.restore();
    } else {
      loadImg(bg.spriteData, 'bg', () => drawLevel());
    }
  }

  drawLevelOverlay(canvas, ctx);
}

function drawLevelOverlay(canvas, ctx) {
  const tl = worldToCanvas(0, 0);
  const br = worldToCanvas(S.level.width, S.level.height);
  const lw = br.x - tl.x, lh = br.y - tl.y;

  // level border
  ctx.strokeStyle = '#ffffff30';
  ctx.lineWidth = 1;
  ctx.strokeRect(tl.x, tl.y, lw, lh);

  // out-of-bounds dim
  ctx.fillStyle = 'rgba(0,0,0,0.4)';
  ctx.fillRect(0, 0, canvas.width, tl.y);
  ctx.fillRect(0, br.y, canvas.width, canvas.height - br.y);
  ctx.fillRect(0, tl.y, tl.x, lh);
  ctx.fillRect(br.x, tl.y, canvas.width - br.x, lh);

  // grid
  if (S.showGrid) {
    ctx.strokeStyle = 'rgba(255,255,255,0.06)';
    ctx.lineWidth = 0.5;
    const gx = GRID * S.zoom;
    const startX = tl.x + (((-S.panX % GRID) + GRID) % GRID) * S.zoom;
    for (let x = startX; x < br.x; x += gx) {
      ctx.beginPath(); ctx.moveTo(x, tl.y); ctx.lineTo(x, br.y); ctx.stroke();
    }
    const gy = GRID * S.zoom;
    const startY = tl.y + (((-S.panY % GRID) + GRID) % GRID) * S.zoom;
    for (let y = startY; y < br.y; y += gy) {
      ctx.beginPath(); ctx.moveTo(tl.x, y); ctx.lineTo(br.x, y); ctx.stroke();
    }
  }

  // entities
  for (const ent of S.level.entities) {
    drawEntity(ctx, ent);
  }

  // camera overlay
  if (S.showCamera) {
    const camW = SCREEN_W, camH = SCREEN_H;
    const centerX = S.level.width / 2, centerY = S.level.height / 2;
    const camTL = worldToCanvas(centerX - camW / 2, centerY - camH / 2);
    const camBR = worldToCanvas(centerX + camW / 2, centerY + camH / 2);
    ctx.strokeStyle = 'rgba(124,158,248,0.5)';
    ctx.lineWidth = 1;
    ctx.setLineDash([6, 4]);
    ctx.strokeRect(camTL.x, camTL.y, camBR.x - camTL.x, camBR.y - camTL.y);
    ctx.setLineDash([]);
    ctx.fillStyle = 'rgba(124,158,248,0.12)';
    ctx.fillRect(camTL.x, camTL.y, camBR.x - camTL.x, camBR.y - camTL.y);
    ctx.fillStyle = 'rgba(124,158,248,0.7)';
    ctx.font = `${10 * S.zoom}px sans-serif`;
    ctx.fillText('Camera (640×480)', camTL.x + 4 * S.zoom, camTL.y + 14 * S.zoom);
  }
}

function drawEntity(ctx, ent) {
  const def = findDef(ent.prefabId);
  const b = entityBounds(ent);
  const tl = worldToCanvas(b.x, b.y);
  const w = b.w * S.zoom, h = b.h * S.zoom;
  const isSelected = ent.id === S.selectedEntityId;

  // try to draw sprite frame
  const img = def && def.spriteData ? S.imgCache[def.id] : null;
  if (img) {
    ctx.drawImage(img, def.frameX, def.frameY, def.frameW, def.frameH, tl.x, tl.y, w, h);
  } else {
    ctx.fillStyle = def ? def.color + 'cc' : '#8888cc';
    ctx.fillRect(tl.x, tl.y, w, h);
    // name label
    const fontSize = Math.max(10, 12 * S.zoom);
    ctx.font = `${fontSize}px sans-serif`;
    ctx.fillStyle = '#ffffffcc';
    ctx.textAlign = 'center';
    ctx.fillText(def ? def.name : '?', tl.x + w / 2, tl.y + h / 2 + fontSize / 3);
    ctx.textAlign = 'left';
  }

  // pivot dot
  const pv = worldToCanvas(ent.x, ent.y);
  ctx.fillStyle = isSelected ? '#ffcc00' : 'rgba(255,255,255,0.4)';
  ctx.beginPath();
  ctx.arc(pv.x, pv.y, isSelected ? 4 : 2.5, 0, Math.PI * 2);
  ctx.fill();

  // selection outline
  if (isSelected) {
    ctx.strokeStyle = '#ffcc00';
    ctx.lineWidth = 2;
    ctx.strokeRect(tl.x - 1, tl.y - 1, w + 2, h + 2);

    // colliders
    if (def && def.colliders) {
      ctx.strokeStyle = 'rgba(95,201,160,0.8)';
      ctx.lineWidth = 1;
      ctx.setLineDash([3, 2]);
      for (const col of def.colliders) {
        const cx = ent.x + (col.xOffset || 0) - col.w / 2;
        const cy = ent.y + (col.yOffset || 0) - col.h / 2;
        const cTL = worldToCanvas(cx, cy);
        ctx.strokeRect(cTL.x, cTL.y, col.w * S.zoom, col.h * S.zoom);
      }
      ctx.setLineDash([]);
    }
  }
}

// ── Level canvas events ───────────────────────────────────────
function levelMouseDown(e) {
  const canvas = document.getElementById('main-canvas');
  const cp = getCanvasPos(e, canvas);

  if (e.button === 1 || (e.button === 0 && e.altKey)) {
    S.isPanning = true;
    S.panStartMouseX = cp.x; S.panStartMouseY = cp.y;
    S.panStartX = S.panX; S.panStartY = S.panY;
    canvas.style.cursor = 'grabbing';
    return;
  }

  if (e.button === 2) return; // context menu handled separately

  const wp = canvasToWorld(cp.x, cp.y);

  if (S.tool === 'place' && S.activePaletteId) {
    const x = snapGrid(wp.x), y = snapGrid(wp.y);
    const ent = { id: uid(), prefabId: S.activePaletteId, x, y };
    S.level.entities.push(ent);
    S.selectedEntityId = ent.id;
    // preload sprite
    const def = findDef(S.activePaletteId);
    if (def && def.spriteData && !S.imgCache[def.id]) {
      loadImg(def.spriteData, def.id, () => drawLevel());
    }
    drawLevel();
    updateRightPanel();
    return;
  }

  if (S.tool === 'erase') {
    const hit = hitTestEntity(wp.x, wp.y);
    if (hit) {
      S.level.entities = S.level.entities.filter(e => e.id !== hit.id);
      if (S.selectedEntityId === hit.id) S.selectedEntityId = null;
      drawLevel(); updateRightPanel();
    }
    return;
  }

  // select tool
  const hit = hitTestEntity(wp.x, wp.y);
  if (hit) {
    S.selectedEntityId = hit.id;
    S.isDragging = true;
    S.dragEntityId = hit.id;
    S.dragOffX = wp.x - hit.x;
    S.dragOffY = wp.y - hit.y;
    canvas.style.cursor = 'move';
  } else {
    S.selectedEntityId = null;
  }
  drawLevel(); updateRightPanel();
}

function levelMouseMove(e) {
  const canvas = document.getElementById('main-canvas');
  const cp = getCanvasPos(e, canvas);

  if (S.isPanning) {
    S.panX = S.panStartX + (cp.x - S.panStartMouseX) / S.zoom;
    S.panY = S.panStartY + (cp.y - S.panStartMouseY) / S.zoom;
    drawLevel();
    return;
  }

  if (S.isDragging && S.dragEntityId) {
    const wp = canvasToWorld(cp.x, cp.y);
    const ent = S.level.entities.find(e => e.id === S.dragEntityId);
    if (ent) {
      ent.x = snapGrid(wp.x - S.dragOffX);
      ent.y = snapGrid(wp.y - S.dragOffY);
      drawLevel(); updateRightPanel();
    }
  }
}

function levelMouseUp(e) {
  const canvas = document.getElementById('main-canvas');
  S.isDragging = false;
  S.isPanning = false;
  canvas.style.cursor = S.tool === 'place' ? 'crosshair' : 'default';
}

function levelMouseWheel(e) {
  e.preventDefault();
  const canvas = document.getElementById('main-canvas');
  const cp = getCanvasPos(e, canvas);
  const factor = e.deltaY < 0 ? 1.1 : 0.9;
  const wx = cp.x / S.zoom - S.panX;
  const wy = cp.y / S.zoom - S.panY;
  S.zoom = Math.max(0.2, Math.min(4, S.zoom * factor));
  S.panX = cp.x / S.zoom - wx;
  S.panY = cp.y / S.zoom - wy;
  drawLevel();
}

function levelContextMenu(e) {
  e.preventDefault();
  const canvas = document.getElementById('main-canvas');
  const cp = getCanvasPos(e, canvas);
  const wp = canvasToWorld(cp.x, cp.y);
  const hit = hitTestEntity(wp.x, wp.y);
  if (!hit) return;
  S.selectedEntityId = hit.id;
  drawLevel(); updateRightPanel();
  showContextMenu(e.clientX, e.clientY);
}

function hitTestEntity(wx, wy) {
  for (let i = S.level.entities.length - 1; i >= 0; i--) {
    const ent = S.level.entities[i];
    const b = entityBounds(ent);
    if (wx >= b.x && wx <= b.x + b.w && wy >= b.y && wy <= b.y + b.h) return ent;
  }
  return null;
}

function showContextMenu(cx, cy) {
  const m = document.getElementById('ctx-menu');
  m.style.left = cx + 'px'; m.style.top = cy + 'px';
  m.classList.remove('hidden');
}
function hideContextMenu() {
  document.getElementById('ctx-menu').classList.add('hidden');
}

// ── Level canvas toolbar ──────────────────────────────────────
function renderCanvasToolbar() {
  const tb = document.getElementById('canvas-toolbar');
  if (S.mode === 'level') {
    tb.innerHTML = `
      <button class="tool-btn ${S.tool==='select'?'active':''}" data-tool="select" title="Select (S)">Select</button>
      <button class="tool-btn ${S.tool==='place'?'active':''}" data-tool="place" title="Place (P)">Place</button>
      <button class="tool-btn ${S.tool==='erase'?'active':''}" data-tool="erase" title="Erase (E)">Erase</button>
      <div class="separator" style="width:1px;height:20px;margin:0 4px;background:var(--border)"></div>
      <label class="row-flex" style="gap:4px;font-size:12px;cursor:pointer">
        <input type="checkbox" class="prop-check" id="chk-grid" ${S.showGrid?'checked':''}>Grid
      </label>
      <label class="row-flex" style="gap:4px;font-size:12px;cursor:pointer">
        <input type="checkbox" class="prop-check" id="chk-cam" ${S.showCamera?'checked':''}>Camera
      </label>
      <label class="row-flex" style="gap:4px;font-size:12px;cursor:pointer">
        <input type="checkbox" class="prop-check" id="chk-snap" ${S.snapToGrid?'checked':''}>Snap
      </label>
      <div class="separator" style="width:1px;height:20px;margin:0 4px;background:var(--border)"></div>
      <button class="tool-btn" id="btn-zoom-reset">100%</button>
      <span style="font-size:11px;color:var(--text-dim)">${Math.round(S.zoom*100)}%</span>
    `;
    tb.querySelectorAll('[data-tool]').forEach(b =>
      b.addEventListener('click', () => { S.tool = b.dataset.tool; renderCanvasToolbar(); updateCanvasCursor(); })
    );
    tb.querySelector('#chk-grid').addEventListener('change', e => { S.showGrid = e.target.checked; drawLevel(); });
    tb.querySelector('#chk-cam').addEventListener('change', e => { S.showCamera = e.target.checked; drawLevel(); });
    tb.querySelector('#chk-snap').addEventListener('change', e => { S.snapToGrid = e.target.checked; });
    tb.querySelector('#btn-zoom-reset').addEventListener('click', () => { S.zoom=1; S.panX=20; S.panY=20; drawLevel(); renderCanvasToolbar(); });
  } else if (S.mode === 'prefab') {
    tb.innerHTML = '<span style="font-size:12px;color:var(--text-dim)">Prefab Preview — sprite frame + colliders</span>';
  } else if (S.mode === 'character') {
    tb.innerHTML = `
      <span style="font-size:12px;color:var(--text-dim)">Sprite Sheet — click frames to add to animation</span>
      <div style="margin-left:auto;display:flex;gap:6px;">
        <button class="tool-btn" id="btn-sheet-zoom-out">−</button>
        <button class="tool-btn" id="btn-sheet-zoom-in">+</button>
        <button class="tool-btn ${S.pickingFrames?'active':''}" id="btn-pick-toggle">
          ${S.pickingFrames ? '✓ Picking Frames' : 'Pick Frames'}
        </button>
      </div>
    `;
    tb.querySelector('#btn-sheet-zoom-out').addEventListener('click', () => {
      S.sheetZoom = Math.max(0.5, S.sheetZoom - 0.25); drawSpriteSheet();
    });
    tb.querySelector('#btn-sheet-zoom-in').addEventListener('click', () => {
      S.sheetZoom = Math.min(4, S.sheetZoom + 0.25); drawSpriteSheet();
    });
    tb.querySelector('#btn-pick-toggle').addEventListener('click', () => {
      S.pickingFrames = !S.pickingFrames; renderCanvasToolbar();
    });
  }
}

function updateCanvasCursor() {
  const wrap = document.getElementById('canvas-wrap');
  wrap.style.cursor = S.tool === 'place' ? 'crosshair' : S.tool === 'erase' ? 'not-allowed' : 'default';
}

// ── Level left panel ──────────────────────────────────────────
function renderLevelLeft() {
  document.getElementById('left-panel-title').textContent = 'Entity Palette';
  const content = document.getElementById('left-panel-content');
  let html = '';

  if (S.prefabs.length) {
    html += `<div style="padding:6px 12px;font-size:10px;color:var(--text-dim);font-weight:700;text-transform:uppercase">Prefabs</div>`;
    for (const p of S.prefabs) {
      const sel = S.activePaletteId === p.id ? 'selected' : '';
      html += `<div class="list-item ${sel}" data-id="${p.id}">
        <span class="list-item-dot" style="background:${p.color}"></span>
        <span class="list-item-name">${p.name}</span>
      </div>`;
    }
  }
  if (S.characters.length) {
    html += `<div style="padding:6px 12px;font-size:10px;color:var(--text-dim);font-weight:700;text-transform:uppercase;margin-top:4px">Characters</div>`;
    for (const c of S.characters) {
      const sel = S.activePaletteId === c.id ? 'selected' : '';
      html += `<div class="list-item ${sel}" data-id="${c.id}">
        <span class="list-item-dot" style="background:${c.color}"></span>
        <span class="list-item-name">${c.name}</span>
      </div>`;
    }
  }
  content.innerHTML = html;

  content.querySelectorAll('.list-item[data-id]').forEach(el => {
    el.addEventListener('click', () => {
      S.activePaletteId = el.dataset.id;
      S.tool = 'place';
      renderLevelLeft(); renderCanvasToolbar(); updateCanvasCursor();
    });
  });
}

// ── Level right panel ─────────────────────────────────────────
function renderLevelRight() {
  const title = document.getElementById('right-panel-title');
  const content = document.getElementById('right-panel-content');
  const ent = S.level.entities.find(e => e.id === S.selectedEntityId);
  const bg = S.level.background;

  if (ent) {
    const def = findDef(ent.prefabId);
    title.textContent = 'Entity';
    content.innerHTML = `
      <div class="prop-group">
        <div class="prop-group-title">Transform</div>
        <div class="prop-row"><span class="prop-label">X</span>
          <input class="prop-input" type="number" id="ep-x" value="${ent.x}"></div>
        <div class="prop-row"><span class="prop-label">Y</span>
          <input class="prop-input" type="number" id="ep-y" value="${ent.y}"></div>
        <div class="prop-row"><span class="prop-label">Prefab</span>
          <span style="font-size:12px;color:var(--accent)">${def ? def.name : ent.prefabId}</span></div>
      </div>
      <div class="prop-group">
        <button class="tool-btn danger" id="ep-del" style="width:100%">Delete Entity</button>
      </div>
    `;
    content.querySelector('#ep-x').addEventListener('change', e => {
      ent.x = parseFloat(e.target.value) || 0; drawLevel();
    });
    content.querySelector('#ep-y').addEventListener('change', e => {
      ent.y = parseFloat(e.target.value) || 0; drawLevel();
    });
    content.querySelector('#ep-del').addEventListener('click', () => {
      S.level.entities = S.level.entities.filter(e => e.id !== ent.id);
      S.selectedEntityId = null; drawLevel(); updateRightPanel();
    });
    return;
  }

  // Show level properties when no entity selected
  title.textContent = 'Level Properties';
  content.innerHTML = `
    <div class="prop-group">
      <div class="prop-group-title">Level</div>
      <div class="prop-row"><span class="prop-label">Name</span>
        <input class="prop-input" id="lp-name" value="${S.level.name}"></div>
      <div class="prop-row"><span class="prop-label">Width</span>
        <input class="prop-input" type="number" id="lp-w" value="${S.level.width}"></div>
      <div class="prop-row"><span class="prop-label">Height</span>
        <input class="prop-input" type="number" id="lp-h" value="${S.level.height}"></div>
    </div>
    <div class="prop-group">
      <div class="prop-group-title">Background</div>
      <div class="prop-row"><span class="prop-label">Color</span>
        <input class="prop-input" type="color" id="lp-bgcolor" value="${bg.color}"></div>
      <div style="margin-bottom:6px;font-size:11px;color:var(--text-dim)">Presets:</div>
      <div class="bg-swatches" id="bg-swatches">
        ${BG_PRESETS.map(c=>`<div class="bg-swatch ${bg.color===c?'selected':''}" style="background:${c}" data-color="${c}"></div>`).join('')}
      </div>
      <div class="prop-row">
        <button class="img-pick-btn" id="lp-bgimg">Pick Image</button>
        <span class="img-path-label" id="lp-bgimg-label">${bg.imagePath || 'none'}</span>
      </div>
      ${bg.spriteData ? `<button class="tool-btn danger" id="lp-bgclear" style="font-size:11px;margin-top:4px">Clear Image</button>` : ''}
    </div>
    <div class="prop-group">
      <div class="prop-group-title">Info</div>
      <div style="font-size:12px;color:var(--text-dim)">
        ${S.level.entities.length} entities placed
      </div>
    </div>
  `;

  content.querySelector('#lp-name').addEventListener('change', e => { S.level.name = e.target.value; });
  content.querySelector('#lp-w').addEventListener('change', e => { S.level.width = parseInt(e.target.value)||740; drawLevel(); });
  content.querySelector('#lp-h').addEventListener('change', e => { S.level.height = parseInt(e.target.value)||600; drawLevel(); });
  content.querySelector('#lp-bgcolor').addEventListener('input', e => {
    S.level.background.color = e.target.value; S.level.background.type = 'color'; drawLevel();
  });
  content.querySelectorAll('.bg-swatch').forEach(sw => {
    sw.addEventListener('click', () => {
      S.level.background.color = sw.dataset.color;
      S.level.background.type = 'color';
      drawLevel(); renderLevelRight();
    });
  });
  content.querySelector('#lp-bgimg').addEventListener('click', () => {
    pickImageFile((data, name) => {
      S.level.background = { type: 'image', color: S.level.background.color, imagePath: name, spriteData: data };
      delete S.imgCache['bg'];
      loadImg(data, 'bg', () => { drawLevel(); renderLevelRight(); });
    });
  });
  if (content.querySelector('#lp-bgclear')) {
    content.querySelector('#lp-bgclear').addEventListener('click', () => {
      S.level.background = { type: 'color', color: S.level.background.color, imagePath: '' };
      delete S.imgCache['bg']; drawLevel(); renderLevelRight();
    });
  }
}

// ============================================================
//  PREFAB EDITOR
// ============================================================

function renderPrefabLeft() {
  document.getElementById('left-panel-title').textContent = 'Prefabs';
  const content = document.getElementById('left-panel-content');
  let html = '';
  for (const p of S.prefabs) {
    const sel = S.activePrefabId === p.id ? 'selected' : '';
    html += `<div class="list-item ${sel}" data-id="${p.id}">
      <span class="list-item-dot" style="background:${p.color}"></span>
      <span class="list-item-name">${p.name}</span>
      <span class="list-item-tag">${p.type}</span>
    </div>`;
  }
  html += `<button class="list-add-btn" id="btn-add-prefab">+ New Prefab</button>`;
  content.innerHTML = html;

  content.querySelectorAll('.list-item[data-id]').forEach(el => {
    el.addEventListener('click', () => {
      S.activePrefabId = el.dataset.id;
      renderPrefabLeft(); renderPrefabRight(); drawPrefabPreview();
    });
  });
  content.querySelector('#btn-add-prefab').addEventListener('click', addPrefab);
}

function addPrefab() {
  const p = {
    id: uid(), name: 'NewPrefab', color: '#' + Math.floor(Math.random()*0xFFFFFF).toString(16).padStart(6,'0'),
    spritePath: '', spriteData: null,
    frameX: 0, frameY: 0, frameW: 32, frameH: 32,
    scale: 1, xOffset: 0, yOffset: 0,
    displayW: 32, displayH: 32,
    colliders: [], ysort: false, type: 'static'
  };
  S.prefabs.push(p); S.activePrefabId = p.id;
  renderPrefabLeft(); renderPrefabRight(); drawPrefabPreview();
}

function renderPrefabRight() {
  const title = document.getElementById('right-panel-title');
  const content = document.getElementById('right-panel-content');
  const p = S.prefabs.find(x => x.id === S.activePrefabId);
  if (!p) { title.textContent = 'Prefab Properties'; content.innerHTML = '<div style="padding:12px;color:var(--text-dim);font-size:12px">Select a prefab to edit.</div>'; return; }
  title.textContent = 'Prefab Properties';

  let colHTML = p.colliders.map((c, i) => `
    <div class="collider-row" data-ci="${i}">
      <div class="collider-row-header">
        <span class="collider-row-title">Collider ${i+1}</span>
        <button class="btn-del" data-del-col="${i}">✕</button>
      </div>
      <div class="collider-fields">
        <div class="collider-field"><label>W</label><input type="number" value="${c.w}" data-ci="${i}" data-cf="w"></div>
        <div class="collider-field"><label>H</label><input type="number" value="${c.h}" data-ci="${i}" data-cf="h"></div>
        <div class="collider-field"><label>OffX</label><input type="number" value="${c.xOffset||0}" data-ci="${i}" data-cf="xOffset"></div>
        <div class="collider-field"><label>OffY</label><input type="number" value="${c.yOffset||0}" data-ci="${i}" data-cf="yOffset"></div>
        <div class="collider-field" style="grid-column:1/-1">
          <label>Type</label>
          <select data-ci="${i}" data-cf="type">
            ${COLLISION_TYPES.map(t=>`<option value="${t}" ${c.type===t?'selected':''}>${t}</option>`).join('')}
          </select>
        </div>
      </div>
    </div>
  `).join('');

  content.innerHTML = `
    <div class="prop-group">
      <div class="prop-group-title">Identity</div>
      <div class="prop-row"><span class="prop-label">Name</span><input class="prop-input" id="pp-name" value="${p.name}"></div>
      <div class="prop-row"><span class="prop-label">Color</span><input class="prop-input" type="color" id="pp-color" value="${p.color}"></div>
      <div class="prop-row"><span class="prop-label">Type</span>
        <select class="prop-input" id="pp-type">
          <option value="static" ${p.type==='static'?'selected':''}>Static</option>
          <option value="dynamic" ${p.type==='dynamic'?'selected':''}>Dynamic</option>
          <option value="background" ${p.type==='background'?'selected':''}>Background</option>
        </select>
      </div>
      <div class="prop-row"><span class="prop-label">Y-Sort</span><input type="checkbox" class="prop-check" id="pp-ysort" ${p.ysort?'checked':''}></div>
    </div>
    <div class="prop-group">
      <div class="prop-group-title">Sprite</div>
      <div class="prop-row">
        <button class="img-pick-btn" id="pp-sprite-pick">Pick Sprite</button>
        <span class="img-path-label" id="pp-sprite-label">${p.spritePath||'none'}</span>
      </div>
      ${p.spriteData ? `<button class="tool-btn danger" id="pp-sprite-clear" style="font-size:11px;margin-top:4px;margin-bottom:4px">Clear</button>` : ''}
      <div class="prop-row"><span class="prop-label">Frame X</span><input class="prop-input" type="number" id="pp-fx" value="${p.frameX}"></div>
      <div class="prop-row"><span class="prop-label">Frame Y</span><input class="prop-input" type="number" id="pp-fy" value="${p.frameY}"></div>
      <div class="prop-row"><span class="prop-label">Frame W</span><input class="prop-input" type="number" id="pp-fw" value="${p.frameW}"></div>
      <div class="prop-row"><span class="prop-label">Frame H</span><input class="prop-input" type="number" id="pp-fh" value="${p.frameH}"></div>
    </div>
    <div class="prop-group">
      <div class="prop-group-title">Transform</div>
      <div class="prop-row"><span class="prop-label">Scale</span><input class="prop-input" type="number" id="pp-scale" value="${p.scale}" step="0.5"></div>
      <div class="prop-row"><span class="prop-label">X Offset</span><input class="prop-input" type="number" id="pp-xoff" value="${p.xOffset}"></div>
      <div class="prop-row"><span class="prop-label">Y Offset</span><input class="prop-input" type="number" id="pp-yoff" value="${p.yOffset}"></div>
      <div class="prop-row"><span class="prop-label">Display W</span><input class="prop-input" type="number" id="pp-dw" value="${p.displayW}"></div>
      <div class="prop-row"><span class="prop-label">Display H</span><input class="prop-input" type="number" id="pp-dh" value="${p.displayH}"></div>
    </div>
    <div class="prop-group">
      <div class="prop-group-title">Colliders</div>
      ${colHTML}
      <button class="list-add-btn" id="pp-add-col">+ Add Collider</button>
    </div>
    <div class="prop-group">
      <button class="tool-btn danger" id="pp-del" style="width:100%">Delete Prefab</button>
    </div>
  `;

  // bindings
  const bind = (id, field, parse) => {
    const el = content.querySelector('#' + id);
    if (!el) return;
    el.addEventListener('change', e => {
      p[field] = parse ? parse(e.target.value) : e.target.value;
      drawPrefabPreview();
    });
  };
  bind('pp-name', 'name');
  bind('pp-color', 'color');
  bind('pp-type', 'type');
  content.querySelector('#pp-ysort').addEventListener('change', e => { p.ysort = e.target.checked; });
  bind('pp-fx', 'frameX', v => parseInt(v)||0);
  bind('pp-fy', 'frameY', v => parseInt(v)||0);
  bind('pp-fw', 'frameW', v => parseInt(v)||32);
  bind('pp-fh', 'frameH', v => parseInt(v)||32);
  bind('pp-scale', 'scale', v => parseFloat(v)||1);
  bind('pp-xoff', 'xOffset', v => parseInt(v)||0);
  bind('pp-yoff', 'yOffset', v => parseInt(v)||0);
  bind('pp-dw', 'displayW', v => parseInt(v)||32);
  bind('pp-dh', 'displayH', v => parseInt(v)||32);

  content.querySelector('#pp-sprite-pick').addEventListener('click', () => {
    pickImageFile((data, name) => {
      p.spritePath = name; p.spriteData = data;
      delete S.imgCache[p.id];
      loadImg(data, p.id, () => { drawPrefabPreview(); renderPrefabRight(); });
    });
  });
  if (content.querySelector('#pp-sprite-clear')) {
    content.querySelector('#pp-sprite-clear').addEventListener('click', () => {
      p.spritePath = ''; p.spriteData = null; delete S.imgCache[p.id];
      drawPrefabPreview(); renderPrefabRight();
    });
  }

  // collider field changes
  content.querySelectorAll('[data-ci][data-cf]').forEach(el => {
    el.addEventListener('change', e => {
      const i = parseInt(el.dataset.ci);
      const f = el.dataset.cf;
      const val = f === 'type' ? el.value : (parseInt(el.value) || 0);
      p.colliders[i][f] = val;
      drawPrefabPreview();
    });
  });
  content.querySelectorAll('[data-del-col]').forEach(btn => {
    btn.addEventListener('click', () => {
      p.colliders.splice(parseInt(btn.dataset.delCol), 1);
      renderPrefabRight(); drawPrefabPreview();
    });
  });
  content.querySelector('#pp-add-col').addEventListener('click', () => {
    p.colliders.push({ w: 32, h: 32, xOffset: 0, yOffset: 0, type: 'TYPE_PUSHOUT' });
    renderPrefabRight(); drawPrefabPreview();
  });
  content.querySelector('#pp-del').addEventListener('click', () => {
    S.prefabs = S.prefabs.filter(x => x.id !== p.id);
    S.activePrefabId = S.prefabs[0]?.id || null;
    renderPrefabLeft(); renderPrefabRight(); drawPrefabPreview();
  });
}

function drawPrefabPreview() {
  const canvas = document.getElementById('main-canvas');
  const ctx = canvas.getContext('2d');
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  const p = S.prefabs.find(x => x.id === S.activePrefabId);
  if (!p) {
    ctx.fillStyle = '#333344';
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = 'var(--text-dim)';
    ctx.font = '14px sans-serif';
    ctx.textAlign = 'center';
    ctx.fillText('Select a prefab', canvas.width/2, canvas.height/2);
    ctx.textAlign = 'left';
    return;
  }

  // checkerboard background
  const cs = 20;
  for (let y = 0; y < canvas.height; y += cs) {
    for (let x = 0; x < canvas.width; x += cs) {
      ctx.fillStyle = ((Math.floor(x/cs)+Math.floor(y/cs))%2===0) ? '#1a1a2e' : '#252540';
      ctx.fillRect(x, y, cs, cs);
    }
  }

  const cx = canvas.width / 2, cy = canvas.height / 2;
  const scale = Math.min(4, Math.min((canvas.width * 0.6) / p.displayW, (canvas.height * 0.6) / p.displayH));
  const dw = p.displayW * scale, dh = p.displayH * scale;
  const dx = cx - dw/2, dy = cy - dh/2;

  const img = p.spriteData ? S.imgCache[p.id] : null;
  if (img) {
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(img, p.frameX, p.frameY, p.frameW, p.frameH, dx, dy, dw, dh);
  } else {
    ctx.fillStyle = p.color + 'cc';
    ctx.fillRect(dx, dy, dw, dh);
    ctx.fillStyle = '#fff';
    ctx.font = '13px sans-serif';
    ctx.textAlign = 'center';
    ctx.fillText(p.name, cx, cy + 5);
    ctx.textAlign = 'left';
  }

  // draw colliders
  ctx.setLineDash([4, 3]);
  for (const col of p.colliders) {
    ctx.strokeStyle = col.type === 'TYPE_PLAYER' ? '#4169e1' : col.type === 'TYPE_PUSHIN' ? '#e14141' : '#5fc9a0';
    ctx.lineWidth = 1.5;
    const colX = cx + (col.xOffset * scale) - (col.w * scale / 2);
    const colY = cy + (col.yOffset * scale) - (col.h * scale / 2);
    ctx.strokeRect(colX, colY, col.w * scale, col.h * scale);
    ctx.fillStyle = ctx.strokeStyle + '22';
    ctx.fillRect(colX, colY, col.w * scale, col.h * scale);

    // label
    ctx.font = '10px sans-serif';
    ctx.fillStyle = ctx.strokeStyle;
    ctx.fillText(col.type, colX + 2, colY + 11);
  }
  ctx.setLineDash([]);

  // pivot cross
  ctx.strokeStyle = '#ffcc00';
  ctx.lineWidth = 1;
  ctx.beginPath(); ctx.moveTo(cx-8,cy); ctx.lineTo(cx+8,cy); ctx.stroke();
  ctx.beginPath(); ctx.moveTo(cx,cy-8); ctx.lineTo(cx,cy+8); ctx.stroke();

  // info text
  ctx.fillStyle = '#888899';
  ctx.font = '11px sans-serif';
  ctx.fillText(`${p.displayW}×${p.displayH}px  scale:${p.scale}  offsets:(${p.xOffset},${p.yOffset})`, 10, canvas.height - 10);
}

// ============================================================
//  CHARACTER EDITOR
// ============================================================

function renderCharLeft() {
  document.getElementById('left-panel-title').textContent = 'Characters';
  const content = document.getElementById('left-panel-content');
  let html = '';
  for (const c of S.characters) {
    const sel = S.activeCharId === c.id ? 'selected' : '';
    html += `<div class="list-item ${sel}" data-id="${c.id}">
      <span class="list-item-dot" style="background:${c.color}"></span>
      <span class="list-item-name">${c.name}</span>
    </div>`;
  }
  html += `<button class="list-add-btn" id="btn-add-char">+ New Character</button>`;

  const ch = S.characters.find(c => c.id === S.activeCharId);
  if (ch) {
    html += `<div style="height:1px;background:var(--border);margin:4px 0"></div>`;
    html += `<div style="padding:6px 12px;font-size:10px;color:var(--text-dim);font-weight:700;text-transform:uppercase">Animations</div>`;
    for (const name of Object.keys(ch.animations)) {
      const sel = S.activeAnimName === name ? 'selected' : '';
      const frames = ch.animations[name].frames.length;
      html += `<div class="list-item ${sel}" data-anim="${name}">
        <span class="list-item-name" style="font-size:11px">${name}</span>
        <span class="list-item-tag">${frames}f</span>
      </div>`;
    }
    html += `<button class="list-add-btn" id="btn-add-anim">+ New Animation</button>`;
  }

  content.innerHTML = html;

  content.querySelectorAll('.list-item[data-id]').forEach(el => {
    el.addEventListener('click', () => {
      S.activeCharId = el.dataset.id;
      S.activeAnimName = null;
      stopAnimPreview();
      renderCharLeft(); renderCharRight(); drawSpriteSheet(); renderBottomPanel();
    });
  });
  content.querySelectorAll('.list-item[data-anim]').forEach(el => {
    el.addEventListener('click', () => {
      S.activeAnimName = el.dataset.anim;
      stopAnimPreview();
      renderCharLeft(); renderCharRight(); drawSpriteSheet(); renderBottomPanel();
    });
  });
  content.querySelector('#btn-add-char')?.addEventListener('click', addCharacter);
  content.querySelector('#btn-add-anim')?.addEventListener('click', addAnimation);
}

function addCharacter() {
  const c = {
    id: uid(), name: 'NewCharacter',
    color: '#' + Math.floor(Math.random()*0xFFFFFF).toString(16).padStart(6,'0'),
    spritePath: '', spriteData: null,
    sheetCols: 8, sheetRows: 8,
    frameW: 32, frameH: 32,
    scale: 2, xOffset: -16, yOffset: -16,
    displayW: 64, displayH: 64,
    colliders: [{ w: 24, h: 24, xOffset: 0, yOffset: 0, type: 'TYPE_PLAYER' }],
    animations: { 'idle': { frames: [], speed: 3, loop: true } },
    behaviorCode: '// Character behavior code\n'
  };
  S.characters.push(c);
  S.activeCharId = c.id;
  S.activeAnimName = 'idle';
  renderCharLeft(); renderCharRight(); drawSpriteSheet(); renderBottomPanel();
}

function addAnimation() {
  const ch = S.characters.find(c => c.id === S.activeCharId);
  if (!ch) return;
  const name = 'anim_' + Object.keys(ch.animations).length;
  ch.animations[name] = { frames: [], speed: 3, loop: true };
  S.activeAnimName = name;
  renderCharLeft(); renderCharRight(); renderBottomPanel();
}

function renderCharRight() {
  const title = document.getElementById('right-panel-title');
  const content = document.getElementById('right-panel-content');
  const ch = S.characters.find(c => c.id === S.activeCharId);
  if (!ch) {
    title.textContent = 'Character Properties';
    content.innerHTML = '<div style="padding:12px;color:var(--text-dim);font-size:12px">Select a character to edit.</div>';
    return;
  }

  const anim = S.activeAnimName ? ch.animations[S.activeAnimName] : null;

  let colHTML = ch.colliders.map((c, i) => `
    <div class="collider-row">
      <div class="collider-row-header">
        <span class="collider-row-title">Collider ${i+1}</span>
        <button class="btn-del" data-del-col="${i}">✕</button>
      </div>
      <div class="collider-fields">
        <div class="collider-field"><label>W</label><input type="number" value="${c.w}" data-ci="${i}" data-cf="w"></div>
        <div class="collider-field"><label>H</label><input type="number" value="${c.h}" data-ci="${i}" data-cf="h"></div>
        <div class="collider-field"><label>OffX</label><input type="number" value="${c.xOffset||0}" data-ci="${i}" data-cf="xOffset"></div>
        <div class="collider-field"><label>OffY</label><input type="number" value="${c.yOffset||0}" data-ci="${i}" data-cf="yOffset"></div>
        <div class="collider-field" style="grid-column:1/-1">
          <label>Type</label>
          <select data-ci="${i}" data-cf="type">
            ${COLLISION_TYPES.map(t=>`<option value="${t}" ${c.type===t?'selected':''}>${t}</option>`).join('')}
          </select>
        </div>
      </div>
    </div>
  `).join('');

  title.textContent = 'Character';
  content.innerHTML = `
    <div class="prop-group">
      <div class="prop-group-title">Identity</div>
      <div class="prop-row"><span class="prop-label">Name</span><input class="prop-input" id="cp-name" value="${ch.name}"></div>
      <div class="prop-row"><span class="prop-label">Color</span><input class="prop-input" type="color" id="cp-color" value="${ch.color}"></div>
    </div>
    <div class="prop-group">
      <div class="prop-group-title">Sprite Sheet</div>
      <div class="prop-row">
        <button class="img-pick-btn" id="cp-sprite-pick">Pick Sheet</button>
        <span class="img-path-label">${ch.spritePath||'none'}</span>
      </div>
      ${ch.spriteData ? `<button class="tool-btn danger" id="cp-sprite-clear" style="font-size:11px;margin-top:4px;margin-bottom:4px">Clear</button>` : ''}
      <div class="prop-row"><span class="prop-label">Cols</span><input class="prop-input" type="number" id="cp-cols" value="${ch.sheetCols}"></div>
      <div class="prop-row"><span class="prop-label">Rows</span><input class="prop-input" type="number" id="cp-rows" value="${ch.sheetRows}"></div>
      <div class="prop-row"><span class="prop-label">Frame W</span><input class="prop-input" type="number" id="cp-fw" value="${ch.frameW}"></div>
      <div class="prop-row"><span class="prop-label">Frame H</span><input class="prop-input" type="number" id="cp-fh" value="${ch.frameH}"></div>
    </div>
    <div class="prop-group">
      <div class="prop-group-title">Transform</div>
      <div class="prop-row"><span class="prop-label">Scale</span><input class="prop-input" type="number" id="cp-scale" value="${ch.scale}" step="0.5"></div>
      <div class="prop-row"><span class="prop-label">X Offset</span><input class="prop-input" type="number" id="cp-xoff" value="${ch.xOffset}"></div>
      <div class="prop-row"><span class="prop-label">Y Offset</span><input class="prop-input" type="number" id="cp-yoff" value="${ch.yOffset}"></div>
      <div class="prop-row"><span class="prop-label">Display W</span><input class="prop-input" type="number" id="cp-dw" value="${ch.displayW}"></div>
      <div class="prop-row"><span class="prop-label">Display H</span><input class="prop-input" type="number" id="cp-dh" value="${ch.displayH}"></div>
    </div>
    <div class="prop-group">
      <div class="prop-group-title">Colliders</div>
      ${colHTML}
      <button class="list-add-btn" id="cp-add-col">+ Add Collider</button>
    </div>
    ${anim ? `
    <div class="prop-group">
      <div class="prop-group-title">Animation: ${S.activeAnimName}</div>
      <div class="prop-row"><span class="prop-label">Speed (ticks)</span><input class="prop-input" type="number" id="ap-speed" value="${anim.speed}" min="1"></div>
      <div class="prop-row"><span class="prop-label">Loop</span><input type="checkbox" class="prop-check" id="ap-loop" ${anim.loop?'checked':''}></div>
      <div class="prop-row"><button class="tool-btn" id="ap-clear">Clear Frames</button></div>
      <div class="prop-row">
        <button class="tool-btn danger" id="ap-del-anim">Delete Animation</button>
      </div>
    </div>
    ` : ''}
    <div class="prop-group">
      <button class="tool-btn danger" id="cp-del" style="width:100%">Delete Character</button>
    </div>
  `;

  // character bindings
  const cbind = (id, field, parse) => {
    const el = content.querySelector('#' + id); if (!el) return;
    el.addEventListener('change', e => { ch[field] = parse ? parse(e.target.value) : e.target.value; drawSpriteSheet(); });
  };
  cbind('cp-name', 'name');
  cbind('cp-color', 'color');
  cbind('cp-cols', 'sheetCols', v => Math.max(1,parseInt(v)||1));
  cbind('cp-rows', 'sheetRows', v => Math.max(1,parseInt(v)||1));
  cbind('cp-fw', 'frameW', v => parseInt(v)||32);
  cbind('cp-fh', 'frameH', v => parseInt(v)||32);
  cbind('cp-scale', 'scale', v => parseFloat(v)||1);
  cbind('cp-xoff', 'xOffset', v => parseInt(v)||0);
  cbind('cp-yoff', 'yOffset', v => parseInt(v)||0);
  cbind('cp-dw', 'displayW', v => parseInt(v)||32);
  cbind('cp-dh', 'displayH', v => parseInt(v)||32);

  content.querySelector('#cp-sprite-pick').addEventListener('click', () => {
    pickImageFile((data, name) => {
      ch.spritePath = name; ch.spriteData = data;
      delete S.imgCache[ch.id];
      loadImg(data, ch.id, () => { drawSpriteSheet(); renderCharRight(); });
    });
  });
  if (content.querySelector('#cp-sprite-clear')) {
    content.querySelector('#cp-sprite-clear').addEventListener('click', () => {
      ch.spritePath = ''; ch.spriteData = null; delete S.imgCache[ch.id];
      drawSpriteSheet(); renderCharRight();
    });
  }

  content.querySelectorAll('[data-ci][data-cf]').forEach(el => {
    el.addEventListener('change', () => {
      const i = parseInt(el.dataset.ci), f = el.dataset.cf;
      ch.colliders[i][f] = f === 'type' ? el.value : (parseInt(el.value)||0);
    });
  });
  content.querySelectorAll('[data-del-col]').forEach(btn => {
    btn.addEventListener('click', () => {
      ch.colliders.splice(parseInt(btn.dataset.delCol), 1);
      renderCharRight();
    });
  });
  content.querySelector('#cp-add-col').addEventListener('click', () => {
    ch.colliders.push({ w: 24, h: 24, xOffset: 0, yOffset: 0, type: 'TYPE_PLAYER' });
    renderCharRight();
  });

  if (anim) {
    content.querySelector('#ap-speed').addEventListener('change', e => { anim.speed = Math.max(1,parseInt(e.target.value)||1); });
    content.querySelector('#ap-loop').addEventListener('change', e => { anim.loop = e.target.checked; });
    content.querySelector('#ap-clear').addEventListener('click', () => {
      anim.frames = []; renderBottomPanel(); renderCharLeft();
    });
    content.querySelector('#ap-del-anim').addEventListener('click', () => {
      delete ch.animations[S.activeAnimName];
      S.activeAnimName = Object.keys(ch.animations)[0] || null;
      renderCharLeft(); renderCharRight(); renderBottomPanel();
    });
  }

  content.querySelector('#cp-del').addEventListener('click', () => {
    S.characters = S.characters.filter(c => c.id !== ch.id);
    S.activeCharId = S.characters[0]?.id || null;
    S.activeAnimName = null;
    renderCharLeft(); renderCharRight(); drawSpriteSheet(); renderBottomPanel();
  });
}

// ── Sprite sheet canvas ───────────────────────────────────────
function drawSpriteSheet() {
  const canvas = document.getElementById('main-canvas');
  const ctx = canvas.getContext('2d');
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  const ch = S.characters.find(c => c.id === S.activeCharId);
  if (!ch) {
    ctx.fillStyle = '#1a1a2e';
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = '#666';
    ctx.font = '14px sans-serif';
    ctx.textAlign = 'center';
    ctx.fillText('Select a character', canvas.width/2, canvas.height/2);
    ctx.textAlign = 'left';
    return;
  }

  // checkerboard
  const cs = 16;
  for (let y = 0; y < canvas.height; y += cs)
    for (let x = 0; x < canvas.width; x += cs) {
      ctx.fillStyle = ((Math.floor(x/cs)+Math.floor(y/cs))%2===0) ? '#1a1a2e' : '#222238';
      ctx.fillRect(x, y, cs, cs);
    }

  const fw = ch.frameW, fh = ch.frameH;
  const cols = ch.sheetCols, rows = ch.sheetRows;
  const z = S.sheetZoom;
  const ox = 20, oy = 20; // top-left offset on canvas

  const totalW = cols * fw * z, totalH = rows * fh * z;
  const img = ch.spriteData ? S.imgCache[ch.id] : null;

  if (img) {
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(img, ox, oy, totalW, totalH);
  } else {
    // draw placeholder grid
    for (let r = 0; r < rows; r++) {
      for (let c2 = 0; c2 < cols; c2++) {
        const hue = (c2 + r * cols) * 17 % 360;
        ctx.fillStyle = `hsl(${hue},40%,25%)`;
        ctx.fillRect(ox + c2*fw*z, oy + r*fh*z, fw*z-1, fh*z-1);
      }
    }
  }

  // grid lines
  ctx.strokeStyle = 'rgba(124,158,248,0.3)';
  ctx.lineWidth = 0.5;
  for (let c2 = 0; c2 <= cols; c2++) {
    const x = ox + c2*fw*z;
    ctx.beginPath(); ctx.moveTo(x, oy); ctx.lineTo(x, oy+totalH); ctx.stroke();
  }
  for (let r = 0; r <= rows; r++) {
    const y = oy + r*fh*z;
    ctx.beginPath(); ctx.moveTo(ox, y); ctx.lineTo(ox+totalW, y); ctx.stroke();
  }

  // highlight frames in active animation
  const anim = S.activeAnimName ? ch.animations[S.activeAnimName] : null;
  if (anim) {
    anim.frames.forEach((f, fi) => {
      const fx = ox + f.col * fw * z, fy = oy + f.row * fh * z;
      ctx.fillStyle = 'rgba(95,201,160,0.25)';
      ctx.fillRect(fx, fy, fw*z, fh*z);
      ctx.strokeStyle = '#5fc9a0';
      ctx.lineWidth = 1.5;
      ctx.strokeRect(fx+1, fy+1, fw*z-2, fh*z-2);
      // frame index
      ctx.fillStyle = '#5fc9a0';
      ctx.font = `bold ${Math.max(9, 11*z)}px sans-serif`;
      ctx.textAlign = 'center';
      ctx.fillText(fi+1, fx+fw*z/2, fy+fh*z/2 + 4*z);
      ctx.textAlign = 'left';
    });
  }

  // instructions
  ctx.fillStyle = '#555566';
  ctx.font = '11px sans-serif';
  ctx.fillText(S.pickingFrames
    ? '▶ Click frames to add to animation | Shift+Click to remove'
    : '▶ Enable "Pick Frames" in toolbar to edit animation',
    ox, oy + totalH + 18);
}

// sprite sheet click — pick frame
function sheetMouseDown(e) {
  if (!S.pickingFrames) return;
  const canvas = document.getElementById('main-canvas');
  const cp = getCanvasPos(e, canvas);
  const ch = S.characters.find(c => c.id === S.activeCharId);
  if (!ch) return;
  const anim = S.activeAnimName ? ch.animations[S.activeAnimName] : null;
  if (!anim) { notify('Select an animation first'); return; }

  const ox = 20, oy = 20;
  const z = S.sheetZoom;
  const fw = ch.frameW * z, fh = ch.frameH * z;
  const col = Math.floor((cp.x - ox) / fw);
  const row = Math.floor((cp.y - oy) / fh);
  if (col < 0 || row < 0 || col >= ch.sheetCols || row >= ch.sheetRows) return;

  if (e.shiftKey) {
    // remove last matching frame
    let idx = -1;
    for (let i = anim.frames.length - 1; i >= 0; i--) {
      if (anim.frames[i].col === col && anim.frames[i].row === row) { idx = i; break; }
    }
    if (idx >= 0) anim.frames.splice(idx, 1);
  } else {
    anim.frames.push({ col, row });
  }
  drawSpriteSheet(); renderBottomPanel(); renderCharLeft();
}

// ── Animation preview ─────────────────────────────────────────
function renderBottomPanel() {
  const content = document.getElementById('bottom-content');
  if (S.bottomTab === 'code') {
    renderCodeEditor(content);
    return;
  }
  // timeline
  const ch = S.characters.find(c => c.id === S.activeCharId);
  const anim = (ch && S.activeAnimName) ? ch.animations[S.activeAnimName] : null;

  if (S.mode !== 'character' || !anim) {
    content.innerHTML = `<div style="padding:10px;color:var(--text-dim);font-size:12px">
      ${S.mode === 'character' ? 'Select a character and animation to edit frames.' : 'Switch to Character mode to edit animations.'}
    </div>`;
    return;
  }

  // anim preview canvas + controls
  content.innerHTML = `
    <div id="anim-preview-wrap">
      <canvas id="anim-preview-canvas" width="80" height="80"></canvas>
      <div class="anim-ctrl">
        <button class="tool-btn" id="btn-anim-play">${S.animPlaying ? '⏸ Pause' : '▶ Play'}</button>
        <div style="font-size:11px;color:var(--text-dim)">Frame ${S.animFrame+1}/${anim.frames.length||1}</div>
        <div style="font-size:11px;color:var(--text-dim)">Speed: ${anim.speed} ticks</div>
        <div style="font-size:11px;color:var(--text-dim)">${anim.loop ? 'Looping' : 'One-shot'}</div>
      </div>
      <div style="flex:1;overflow-x:auto">
        <div id="timeline-frames"></div>
      </div>
    </div>
  `;

  // draw anim preview frame
  drawAnimPreviewFrame();

  // timeline thumbnails
  const framesDiv = content.querySelector('#timeline-frames');
  let html = '';
  anim.frames.forEach((f, fi) => {
    const isActive = fi === S.animFrame;
    html += `<div class="frame-thumb ${isActive?'active-frame':''}" data-fi="${fi}" title="Frame ${fi+1} (col:${f.col} row:${f.row})">
      <canvas class="frame-thumb-canvas" data-col="${f.col}" data-row="${f.row}" width="40" height="40" style="width:40px;height:40px"></canvas>
      <span class="frame-num">${fi+1}</span>
    </div>`;
  });
  html += `<div class="frame-thumb-add" title="Frames are added by clicking the sprite sheet above">+</div>`;
  framesDiv.innerHTML = html;

  // draw mini thumbnails
  framesDiv.querySelectorAll('.frame-thumb-canvas').forEach(c => {
    const col = parseInt(c.dataset.col), row = parseInt(c.dataset.row);
    drawFrameThumb(c, ch, col, row);
  });

  framesDiv.querySelectorAll('.frame-thumb[data-fi]').forEach(el => {
    el.addEventListener('click', () => {
      S.animFrame = parseInt(el.dataset.fi);
      renderBottomPanel();
    });
    el.addEventListener('contextmenu', e => {
      e.preventDefault();
      anim.frames.splice(parseInt(el.dataset.fi), 1);
      if (S.animFrame >= anim.frames.length) S.animFrame = Math.max(0, anim.frames.length - 1);
      renderBottomPanel(); drawSpriteSheet(); renderCharLeft();
    });
  });

  content.querySelector('#btn-anim-play').addEventListener('click', () => {
    S.animPlaying ? stopAnimPreview() : startAnimPreview();
    renderBottomPanel();
  });
}

function drawFrameThumb(canvas, ch, col, row) {
  const ctx = canvas.getContext('2d');
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  const img = ch.spriteData ? S.imgCache[ch.id] : null;
  if (img) {
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(img, col*ch.frameW, row*ch.frameH, ch.frameW, ch.frameH, 0, 0, 40, 40);
  } else {
    const hue = (col + row * ch.sheetCols) * 17 % 360;
    ctx.fillStyle = `hsl(${hue},40%,30%)`;
    ctx.fillRect(0, 0, 40, 40);
    ctx.fillStyle = '#aaa';
    ctx.font = '9px sans-serif';
    ctx.textAlign = 'center';
    ctx.fillText(`${col},${row}`, 20, 22);
    ctx.textAlign = 'left';
  }
}

function drawAnimPreviewFrame() {
  const canvas = document.getElementById('anim-preview-canvas');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  const ch = S.characters.find(c => c.id === S.activeCharId);
  const anim = (ch && S.activeAnimName) ? ch.animations[S.activeAnimName] : null;
  if (!anim || anim.frames.length === 0) {
    ctx.fillStyle = '#1a1a2e';
    ctx.fillRect(0, 0, 80, 80);
    ctx.fillStyle = '#555';
    ctx.font = '10px sans-serif';
    ctx.textAlign = 'center';
    ctx.fillText('No frames', 40, 42);
    ctx.textAlign = 'left';
    return;
  }

  const frame = anim.frames[S.animFrame % anim.frames.length];
  // checkerboard
  for (let y=0;y<80;y+=8) for(let x=0;x<80;x+=8) {
    ctx.fillStyle=((Math.floor(x/8)+Math.floor(y/8))%2===0)?'#1a1a2e':'#222238';
    ctx.fillRect(x,y,8,8);
  }
  const img = ch.spriteData ? S.imgCache[ch.id] : null;
  if (img) {
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(img, frame.col*ch.frameW, frame.row*ch.frameH, ch.frameW, ch.frameH, 0, 0, 80, 80);
  } else {
    const hue = (frame.col + frame.row * ch.sheetCols) * 17 % 360;
    ctx.fillStyle = `hsl(${hue},50%,35%)`;
    ctx.fillRect(0, 0, 80, 80);
  }
}

function startAnimPreview() {
  stopAnimPreview();
  S.animPlaying = true;
  const ch = S.characters.find(c => c.id === S.activeCharId);
  const anim = (ch && S.activeAnimName) ? ch.animations[S.activeAnimName] : null;
  if (!anim || anim.frames.length === 0) return;
  let tick = 0;
  S.animTimer = setInterval(() => {
    tick++;
    if (tick >= anim.speed) {
      tick = 0;
      S.animFrame = (S.animFrame + 1) % anim.frames.length;
      if (!anim.loop && S.animFrame === 0) { stopAnimPreview(); renderBottomPanel(); return; }
      drawAnimPreviewFrame();
      // update active frame highlight in timeline
      document.querySelectorAll('.frame-thumb').forEach((el, i) => {
        el.classList.toggle('active-frame', i === S.animFrame);
      });
      const bp = document.querySelector('#bottom-content .anim-ctrl div');
      if (bp) bp.textContent = `Frame ${S.animFrame+1}/${anim.frames.length}`;
    }
  }, 1000/60);
}

function stopAnimPreview() {
  if (S.animTimer) { clearInterval(S.animTimer); S.animTimer = null; }
  S.animPlaying = false;
}

// ── Code editor ───────────────────────────────────────────────
function renderCodeEditor(container) {
  const ch = S.characters.find(c => c.id === S.activeCharId);
  if (!ch) {
    container.innerHTML = `<div style="padding:10px;color:var(--text-dim);font-size:12px">Select a character to edit behavior code.</div>`;
    return;
  }
  container.innerHTML = `
    <div style="display:flex;align-items:center;gap:8px;padding:4px 10px;border-bottom:1px solid var(--border)">
      <span style="font-size:11px;color:var(--text-dim)">${ch.name} — behavior code (C++)</span>
      <button class="tool-btn" id="btn-copy-code" style="margin-left:auto">Copy</button>
    </div>
    <textarea class="prop-textarea" id="code-area" spellcheck="false">${ch.behaviorCode || ''}</textarea>
  `;
  container.querySelector('#code-area').addEventListener('input', e => { ch.behaviorCode = e.target.value; });
  container.querySelector('#btn-copy-code').addEventListener('click', () => {
    navigator.clipboard.writeText(ch.behaviorCode || '').then(() => notify('Code copied!'));
  });
}

// ============================================================
//  PANEL ROUTING
// ============================================================

function updateLeftPanel()  {
  if (S.mode === 'level')      renderLevelLeft();
  else if (S.mode === 'prefab') renderPrefabLeft();
  else                          renderCharLeft();
}
function updateRightPanel() {
  if (S.mode === 'level')       renderLevelRight();
  else if (S.mode === 'prefab') renderPrefabRight();
  else                          renderCharRight();
}
function updateCanvas() {
  const canvas = document.getElementById('main-canvas');
  resizeCanvas(canvas);
  if (S.mode === 'level')       drawLevel();
  else if (S.mode === 'prefab') drawPrefabPreview();
  else                          drawSpriteSheet();
}
function renderBottomTabs() {
  document.querySelectorAll('.bottom-tab').forEach(t => {
    t.classList.toggle('active', t.dataset.tab === S.bottomTab);
  });
  renderBottomPanel();
}

// ── Mode switch ───────────────────────────────────────────────
function setMode(mode) {
  S.mode = mode;
  stopAnimPreview();
  document.querySelectorAll('.mode-tab').forEach(t => t.classList.toggle('active', t.dataset.mode === mode));
  renderCanvasToolbar();
  updateLeftPanel();
  updateCanvas();
  updateRightPanel();
  renderBottomPanel();
}

// ============================================================
//  FILE I/O
// ============================================================

function saveProject() {
  const data = {
    prefabs: S.prefabs,
    characters: S.characters.map(c => ({ ...c, spriteData: c.spriteData ? '[base64]' : null })),
    level: S.level
  };
  // include sprite data for actual save
  const full = { prefabs: S.prefabs, characters: S.characters, level: S.level };
  const blob = new Blob([JSON.stringify(full, null, 2)], { type: 'application/json' });
  const a = document.createElement('a');
  a.href = URL.createObjectURL(blob);
  a.download = 'project.someengine.json';
  a.click();
  notify('Project saved!');
}

function loadProject() {
  document.getElementById('file-input').click();
}

function onProjectFileLoad(e) {
  const f = e.target.files[0]; if (!f) return;
  const reader = new FileReader();
  reader.onload = ev => {
    try {
      const data = JSON.parse(ev.target.result);
      if (data.prefabs) S.prefabs = data.prefabs;
      if (data.characters) { S.characters = data.characters; S.imgCache = {}; }
      if (data.level) S.level = data.level;
      _uid = 1000;
      // reload images
      for (const p of S.prefabs) if (p.spriteData) loadImg(p.spriteData, p.id, () => updateCanvas());
      for (const c of S.characters) if (c.spriteData) loadImg(c.spriteData, c.id, () => updateCanvas());
      if (S.level.background?.spriteData) loadImg(S.level.background.spriteData, 'bg', () => updateCanvas());
      S.activePrefabId = S.prefabs[0]?.id || null;
      S.activeCharId = S.characters[0]?.id || null;
      S.activeAnimName = null;
      setMode(S.mode);
      notify('Project loaded!');
    } catch(err) { alert('Failed to load project: ' + err.message); }
  };
  reader.readAsText(f);
  e.target.value = '';
}

function exportJSON() {
  // Export engine-ready JSON (no base64 images, use path references)
  const out = {
    level: {
      name: S.level.name,
      width: S.level.width,
      height: S.level.height,
      background: {
        type: S.level.background.type,
        color: S.level.background.color,
        imagePath: S.level.background.imagePath || ''
      },
      entities: S.level.entities.map(e => ({ id: e.id, prefabId: e.prefabId, x: e.x, y: e.y }))
    },
    prefabs: S.prefabs.map(p => ({
      id: p.id, name: p.name, type: p.type,
      spritePath: p.spritePath, frameX: p.frameX, frameY: p.frameY,
      frameW: p.frameW, frameH: p.frameH,
      scale: p.scale, xOffset: p.xOffset, yOffset: p.yOffset,
      displayW: p.displayW, displayH: p.displayH,
      ysort: p.ysort, colliders: p.colliders
    })),
    characters: S.characters.map(c => ({
      id: c.id, name: c.name,
      spritePath: c.spritePath,
      frameW: c.frameW, frameH: c.frameH,
      sheetCols: c.sheetCols, sheetRows: c.sheetRows,
      scale: c.scale, xOffset: c.xOffset, yOffset: c.yOffset,
      displayW: c.displayW, displayH: c.displayH,
      colliders: c.colliders,
      animations: c.animations,
      behaviorCode: c.behaviorCode
    }))
  };
  const blob = new Blob([JSON.stringify(out, null, 2)], { type: 'application/json' });
  const a = document.createElement('a');
  a.href = URL.createObjectURL(blob);
  a.download = 'level_export.json';
  a.click();
  notify('Exported!');
}

// ============================================================
//  KEYBOARD SHORTCUTS
// ============================================================

function onKeyDown(e) {
  if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA' || e.target.tagName === 'SELECT') return;
  if (S.mode === 'level') {
    if (e.key === 's' || e.key === 'S') { S.tool = 'select'; renderCanvasToolbar(); updateCanvasCursor(); }
    if (e.key === 'p' || e.key === 'P') { S.tool = 'place';  renderCanvasToolbar(); updateCanvasCursor(); }
    if (e.key === 'e' || e.key === 'E') { S.tool = 'erase';  renderCanvasToolbar(); updateCanvasCursor(); }
    if ((e.key === 'Delete' || e.key === 'Backspace') && S.selectedEntityId) {
      S.level.entities = S.level.entities.filter(en => en.id !== S.selectedEntityId);
      S.selectedEntityId = null; drawLevel(); updateRightPanel();
    }
    if (e.key === 'g' || e.key === 'G') { S.showGrid = !S.showGrid; renderCanvasToolbar(); drawLevel(); }
  }
  if ((e.ctrlKey || e.metaKey) && e.key === 's') { e.preventDefault(); saveProject(); }
}

// ============================================================
//  INIT
// ============================================================

function init() {
  buildDefaultProject();

  // resize canvas on init and window resize
  const canvas = document.getElementById('main-canvas');
  resizeCanvas(canvas);
  window.addEventListener('resize', () => { resizeCanvas(canvas); updateCanvas(); });

  // mode tabs
  document.querySelectorAll('.mode-tab').forEach(t =>
    t.addEventListener('click', () => setMode(t.dataset.mode))
  );

  // bottom tabs
  document.querySelectorAll('.bottom-tab').forEach(t =>
    t.addEventListener('click', () => { S.bottomTab = t.dataset.tab; renderBottomTabs(); })
  );

  // file ops
  document.getElementById('btn-save').addEventListener('click', saveProject);
  document.getElementById('btn-load').addEventListener('click', loadProject);
  document.getElementById('btn-export').addEventListener('click', exportJSON);
  document.getElementById('file-input').addEventListener('change', onProjectFileLoad);

  // canvas events (level editor)
  canvas.addEventListener('mousedown', e => {
    if (S.mode === 'level') levelMouseDown(e);
    else if (S.mode === 'character') sheetMouseDown(e);
  });
  canvas.addEventListener('mousemove', e => { if (S.mode === 'level') levelMouseMove(e); });
  canvas.addEventListener('mouseup',   e => { if (S.mode === 'level') levelMouseUp(e); });
  canvas.addEventListener('wheel',     e => { if (S.mode === 'level') levelMouseWheel(e); }, { passive: false });
  canvas.addEventListener('contextmenu', e => { if (S.mode === 'level') levelContextMenu(e); else e.preventDefault(); });

  // context menu actions
  document.getElementById('ctx-delete').addEventListener('click', () => {
    if (S.selectedEntityId) {
      S.level.entities = S.level.entities.filter(e => e.id !== S.selectedEntityId);
      S.selectedEntityId = null; drawLevel(); updateRightPanel();
    }
    hideContextMenu();
  });
  document.getElementById('ctx-duplicate').addEventListener('click', () => {
    const src = S.level.entities.find(e => e.id === S.selectedEntityId);
    if (src) {
      const clone = { ...src, id: uid(), x: src.x + GRID, y: src.y + GRID };
      S.level.entities.push(clone);
      S.selectedEntityId = clone.id;
      drawLevel(); updateRightPanel();
    }
    hideContextMenu();
  });
  document.addEventListener('click', () => hideContextMenu());

  // keyboard shortcuts
  document.addEventListener('keydown', onKeyDown);

  // initial render
  setMode('level');
}

document.addEventListener('DOMContentLoaded', init);
