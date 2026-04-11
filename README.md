# cyber-rain

A high-performance Matrix rain terminal effect written in C using the `ncurses` library. This program simulates the iconic Matrix rain with customizable speed, fog effects, and an animated intro logo.

## Features

- **Matrix Rain**: Classic falling characters with fading trails.
- **Intro Animation**: Animated logo "CYBER RAIN" at startup.
- **Fog Effect**: Optional fog particles for added atmosphere.
- **Real-time Controls**: Adjust rain speed and toggle fog on the fly.
- **Responsive**: Handles terminal resizing dynamically.

## Requirements

- `ncurses` library
- C compiler (e.g., GCC)

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/cyber-rain.git
   cd cyber-rain
   ```

2. Compile the program:
   ```bash
   gcc cyber_rain.c -lncurses -o cyber_rain
   ```

## Usage

Run the program:
```bash
./cyber_rain
```

## Controls

- `q`: Quit the program
- `↑` (Up arrow): Increase rain speed
- `↓` (Down arrow): Decrease rain speed
- `f` (key): Enable and Disable fog effect
 
## License

This project is open-source and licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
