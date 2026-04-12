# CYBERRAIN - CHANGELOG

## [Release 1.0.0] - April 12, 2026

### 🎨 Features Added

#### Color Introduction System
- **Press 'C' key** to randomly change the rain droplets color
- Available colors: **Green** (default), **Purple**, **Red**, **Blue**
- Each new rain generation gets the newly selected color
- Already falling rain maintains its original color with smooth transitions
- 12 generation buffer ensures existing rain colors stay static while only new rain adopts the new color scheme
- Color changes are only applied after intro animation completes
- Prevents color repetition - never picks the same color twice in a row

#### Rain Control System
- **Press UP Arrow** - Decrease rain speed (slower fall)
- **Press DOWN Arrow** - Increase rain speed (faster fall)
- **Press 'F' key** - Toggle fog effect on/off
- **Press 'Q' key** - Quit the application

#### Visual Features
- **CYBERRAIN Logo Animation** - Beautiful intro sequence with ASCII art particles
- **Matrix Rain Effect** - Classic falling digital rain with gradient effects
- **Fog Particles** - Optional atmospheric fog effect that moves with rain
- **Color Gradients** - Multi-shade color transitions for depth perception
- **Bidirectional Rain** - Rain can fall down (positive speed) or up (negative speed)

#### Technical Features
- **Screen Resize Support** - Dynamically adapts to terminal window resizing
- **FPS Balancing** - Automatic frame rate optimization
- **High-Performance Rendering** - Uses ncurses double buffering for smooth animation
- **Smooth Color Transitions** - 12 generation color scheme slots prevent visual flickering

### 🔧 Technical Improvements
- Accumulator-based movement system for precise speed control
- Generation-based color mapping for persistent trail colors
- Dynamic memory allocation for screen resizing
- Optimized ncurses rendering pipeline

### 📋 Keyboard Controls

| Key | Action |
|-----|--------|
| `↑` | Decrease rain speed |
| `↓` | Increase rain speed |
| `C` | Change rain color |
| `F` | Toggle fog effect |
| `Q` | Quit application |

---

### Color Schemes Available
1. 🟢 **Green** - Classic Matrix-style green
2. 🟣 **Purple** - Mystical purple tones
3. 🔴 **Red** - Dangerous red hues
4. 🔵 **Blue** - Cool blue shades

---

## Release Notes

### This Release Highlights
✨ **Color system completely redesigned** for smooth, seamless transitions
- New rain droplets get the newly selected color instantly
- Existing falling rain retains its assigned color throughout its lifetime
- No color flickering or unexpected changes to existing rain trails
- 12-generation buffer prevents color reuse while rain is still falling

### Known Features
- Matrix rain effect with proper shading
- Interactive keyboard controls for speed and effects
- Beautiful CYBERRAIN ASCII logo with particle animation
- Fog particle effects for atmospheric enhancement
- Full terminal resize support

---

## Future Enhancements (Planned)
- [ ] Custom color palette support
- [ ] Adjustable trail length control
- [ ] Rain density control
- [ ] Configuration file support
- [ ] Multi-color rain mixing
- [ ] Recording/replay mode

---

**Build Command:**
```bash
gcc -o cyber_rain cyber_rain.c -lncurses -ltinfo
```

**Run Command:**
```bash
./cyber_rain
```

---

*Project: CYBERRAIN - A Matrix-Style Terminal Rain Effect*
*Version: 1.0.0*
*Last Updated: April 12, 2026*

