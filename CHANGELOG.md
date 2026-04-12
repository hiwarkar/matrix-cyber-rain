# CYBERRAIN - CHANGELOG

## [Release 1.0.3] - April 12, 2026

### 🌸 Features Added - Cherry Blossom Pink & Golden Glitch Lines

#### Cherry Blossom Pink Color
- 🌸 **9th Color Added** - Beautiful soft pink tone
- Part of regular color rotation (press 'c')
- Available for mix mode combinations
- Adds feminine/elegant option to palette

#### Golden Glitch Lines
- 🟡 **5-8 Golden Alert Columns** always visible during glitch
- Complements red alert lines for striking visual contrast
- Creates **Red + Golden alert combination** (danger + warning)
- Golden lines use yellow_shades (bright, noticeable)
- Ensures glitch is immediately visually striking

#### Technical Updates
- Total colors now: **9** (was 8)
- Glitch dual-alert system: Red + Golden
- Smart color assignment in glitch mode
- Smooth golden column integration

---

## [Release 1.0.2] - April 12, 2026

### 🎨 Features Added - Extended Color Palette & Mix Mode

#### New Color Schemes (8 Total!)
- **Press 'C' key** cycles through all 8 colors
- 🟡 **Yellow/Gold** - Bright golden matrix style
- 🟠 **Orange** - Warm vibrant tone
- 🔷 **Cyan** - Bright light blue
- 🟣 **Magenta** - Vibrant purple-pink
- Plus existing: Green, Purple, Red, Blue

#### Mix Mode Feature
- **Press 'M' key** to enable/disable mix mode
- **Automatic Contrast Selection** - Picks 2-3 complementary colors
- **Per-Column Color Cycling** - Each column uses mix colors sequentially
- **Stunning Visual Combos** - Like Red+Yellow+Blue, Green+Orange+Magenta
- **No Color Repetition** - Ensures non-identical colors in mix
- **Works with all effects** - Compatible with glitch, fog, speed controls

#### Technical Improvements
- Total 8 color schemes now available
- Mix mode uses dynamic pair initialization (pair 150+)
- Contrasting color selection algorithm
- Smooth blending between columns

---

## [Release 1.0.1] - April 12, 2026

### 🔴 Features Added - Glitch Effect System

#### Glitch Effect Feature
- **Press 'G' key** to toggle MASSIVE viral glitch attack (only works after intro completes)
- **🔴 RED ALERT LINES** - 8-12 red columns ALWAYS visible on screen when glitch active
- **Viral Spread** - 50-83% of screen columns get corrupted simultaneously (not just 3-7!)
- **Three Random Glitch Effects** - Each column randomly gets one of:
  - 💥 **Flash Bursts** - Random bright white/inverted text flashes
  - 🔀 **Character Scrambling** - Text positions jump vertically for chaos
  - 🗑️ **Garbage Injection** - 70% random symbol replacement
- **Extreme Speed Chaos** - Glitched columns move at wildly different speeds (-40 to +40 pixel modifications)
- **Brightness Distortion** - Text rapidly flickers between bright (bold), normal, and dim states
- **Extended Freezes** - Affected columns randomly stop moving for 0.3-0.8 seconds
- **Color Mayhem** - Multiple color schemes visible, with guaranteed red alert indicators
- **Smooth Transitions** - Glitch effects fade in over ~0.8 seconds when activated
- **Smooth Recovery** - Press 'G' again to fade out glitch effects with smooth transitions
- **Full-Screen Meltdown** - Creates overwhelming digital plague effect across entire terminal

#### Technical Implementation
- Per-column glitch state tracking with intensity values (0.0-1.0)
- Smooth fade-in/fade-out using frame-based interpolation
- Massive viral coverage: 50-83% of terminal columns corrupted
- **Red alert system**: 8-12 columns forced to use red_shades for constant visibility
- Three distinct glitch effects assigned randomly per column:
  - Flash effect: A_REVERSE attribute for bright white inversion
  - Scramble effect: Random vertical character position offsets (-1, 0, +1)
  - Garbage effect: 70% noise injection in glitched columns
- Extreme speed modifiers: -40, -20, 0, +20, +40 pixel variations
- High garbage character probability: 70% noise injection in glitched columns
- Extended freeze durations: 0.3-0.8 second stops for unpredictability
- Flash timing: Randomized 0.15-0.55 second intervals between flashes
- Zero impact on non-glitched columns - unaffected rain uninterrupted
- Seamless integration with existing color system

---

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
| `G` | Toggle glitch effect |
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

### Release 1.0.1 Highlights
🔴 **MASSIVE Glitch Effect System** - Viral Attack Like Digital Plague
- 50-83% of screen corrupted simultaneously (full-screen meltdown!)
- **🔴 8-12 RED ALERT LINES** always visible on screen (critical warning)
- Three random glitch effects per column:
  - 💥 Flash bursts for sudden bright white text
  - 🔀 Character scrambling for disorienting jumps
  - 🗑️ Garbage injection for visual noise
- Extreme speed variations create chaotic jerky movements
- Extended freeze intervals for unpredictable behavior
- Multiple color schemes active simultaneously
- Smooth fade transitions prevent jarring changes
- Feels like a real virus attack spreading across the terminal

### Release 1.0.0 Highlights
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
- Glitch effect system with smooth transitions

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
*Version: 1.0.1*
*Last Updated: April 12, 2026*

