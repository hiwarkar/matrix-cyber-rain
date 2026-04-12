# CyberRain 🌧️

A high-performance **Matrix rain effect** for the terminal, written in C using the ncurses library.

> A visually enhanced and customizable take on the classic Matrix digital rain.

---

## 🚀 Features

- 🌌 **Matrix Rain Animation** – Smooth falling characters with fading trails
- 🎬 **Animated Intro** – "CYBER RAIN" startup animation
- 🌫️ **Fog Effect** – Atmospheric particle effect for added depth
- 🎨 **Dynamic Color Shifting** – Change rain color on-the-fly (Green, Purple, Red, Blue)
- 🔴 **Glitch Effects** – Corrupt specific streams with speed changes, garbage characters, and color chaos
- ⚡ **Real-time Controls** – Adjust speed and toggle effects on the fly
- 📐 **Responsive Design** – Adapts to terminal resizing dynamically

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| `q` | Quit |
| `c` | Change rain color (cycles through Green, Purple, Red, Blue) |
| `g` | Toggle glitch effect (corrupts random streams) |
| `↑` | Increase speed |
| `↓` | Decrease speed |
| `f` | Toggle fog effect |

---

## 🎨 Color Effects

The **Color Effect** feature allows you to dynamically change the color of newly falling rain characters in real-time:

- **Press `c`** – Cycles through available colors: Green → Purple → Red → Blue → Green
- **Already Falling Text** – Characters that are already falling retain their original color
- **Newly Falling Text** – New characters that fall after pressing `c` will use the new color
- **Smooth Transitions** – Enjoy seamless color changes without affecting existing rain patterns

### Available Colors:
- 🟢 **Green** – Classic Matrix-style green (default)
- 🟣 **Purple** – Mysterious violet hue
- 🔴 **Red** – Intense red tone
- 🔵 **Blue** – Cool cyan blue

---

## 🔴 Glitch Effects

The **Glitch Effect** introduces massive cybernetic corruption to the rain streams for a chaotic, virus-like attack experience:

### What Happens When You Activate Glitch:
- **Press `g`** – MASSIVE viral attack: 50-83% of all columns get corrupted simultaneously
- **Widespread Corruption** – Affected columns overwhelm the screen like a digital plague
- **Three Random Effects** – Each column randomly picks one of three glitch behaviors:
  - 💥 **Flash Bursts** – Random columns flash bright white/inverted text
  - 🔀 **Character Scrambling** – Text jumps positions vertically for disorienting chaos
  - 🗑️ **Garbage Injection** – 70% random symbols replace normal characters
- **Extreme Speed Chaos** – Columns move at wildly different speeds (-40 to +40 pixels)
- **Intense Garbage Characters** – Multiple columns show random symbols simultaneously
- **Brightness Distortion** – Text rapidly flickers between bright, normal, and dim
- **Extended Freezes** – Columns randomly stop moving for 0.3-0.8 seconds
- **Color Mayhem** – Glitched streams display multiple color schemes simultaneously
- **Smooth Fade** – Glitch effects fade in smoothly over ~0.8 seconds when activated
- **Full Recovery** – Press `g` again to fade out and restore the beautiful matrix rain

### Glitch Characteristics:
- ⚡ **Viral Spread** – 50-83% of terminal gets infected with glitch
- 💥 **Multi-Effect Chaos** – Random effects per column create unpredictable behavior
- 🔀 **Dynamic Scrambling** – Characters jump positions for disorienting visual distortion
- 🌊 **Chaotic Behavior** – Each column acts unpredictably with random effect assignment
- 🎬 **Smooth Transitions** – Chaos blends naturally without jarring changes
- 🔄 **Toggleable** – Activate and deactivate glitch mode anytime after the intro
- 💥 **Intense & Noticeable** – Creates a full-screen matrix meltdown effect

---

## 🛠️ Requirements

- C Compiler (GCC recommended)
- ncurses library

---

## ⚙️ Installation

```bash
git clone https://github.com/hiwarkar/cyber-rain.git
cd cyber-rain
gcc cyber_rain.c -lncurses -ltinfo -o cyber_rain
./cyber_rain
