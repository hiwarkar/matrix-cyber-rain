# CyberRain 🌧️

A high-performance **Matrix rain effect** for the terminal, written in C using the ncurses library.

> A visually enhanced and customizable take on the classic Matrix digital rain.

---

## 🚀 Features

- 🌌 **Matrix Rain Animation** – Smooth falling characters with fading trails
- 🎬 **Animated Intro** – "CYBER RAIN" startup animation
- 🌫️ **Fog Effect** – Atmospheric particle effect for added depth
- 🎨 **Dynamic Color Shifting** – Change rain color on-the-fly (Green, Purple, Red, Blue)
- ⚡ **Real-time Controls** – Adjust speed and toggle effects on the fly
- 📐 **Responsive Design** – Adapts to terminal resizing dynamically

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| `q` | Quit |
| `c` | Change rain color (cycles through Green, Purple, Red, Blue) |
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

## 🛠️ Requirements

- C Compiler (GCC recommended)
- ncurses library

---

## ⚙️ Installation

```bash
git clone https://github.com/hiwarkar/cyber-rain.git
cd cyber-rain
gcc cyber_rain.c -lncurses -o cyber_rain