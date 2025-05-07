# Asteroids (Allegro5/C++)

A lightweight, 2D vector-based clone of the classic **Asteroids** arcade game, written in C++ using [Allegro 5](https://liballeg.org/). This project demonstrates modern C++ design, dynamic polygon generation, collision detection, and multiple rendering back-ends.

---

## Table of Contents

- [Demo](#demo)  
- [Features](#features)  
- [How This Differs from My 2007 Version](#how-this-differs-from-my-2007-version)  
- [Requirements](#requirements)  
- [Building & Installation](#building--installation)  
- [Usage](#usage)  
- [Project Structure](#project-structure)  
- [Contributing](#contributing)  
- [License](#license)  
- [Contact](#contact)  

---

## Demo

Watch a quick play-through of the current implementation :

[![Asteroids remake (Allegro 5/C++)](https://img.youtube.com/vi/4pxsWMEwXTQ/0.jpg)](https://www.youtube.com/watch?v=4pxsWMEwXTQ)

---

## Features

- **Vector-based rendering**: All ships, asteroids, and bullets are drawn as polygons—no sprites required.  
- **Dynamic asteroid shapes**: Procedurally generate jagged, natural-looking asteroids with configurable granularity and radius variation :contentReference[oaicite:0]{index=0}.  
- **Collision detection**: Efficient polygon-polygon collision checks for bullets and asteroids, with separation logic and performance tuning :contentReference[oaicite:1]{index=1}.  
- **Particle effects**: Simple particle system for thrust and explosions, with configurable lifetimes and fade‐out :contentReference[oaicite:2]{index=2}.  
- **Multiple rendering back-ends**: Use Allegro’s software renderer or switch to Direct3D for improved performance :contentReference[oaicite:3]{index=3}.  
- **OOP design**: Modular classes for `Ship`, `Asteroid`, `Bullet`, and `Particle`, with a common `GameObject` interface for updating and drawing :contentReference[oaicite:4]{index=4}.  
- **Memory management**: Smart pointers and RAII to prevent leaks; reorganized codebase for clarity and maintainability :contentReference[oaicite:5]{index=5}.  

---

## How This Differs from My 2007 Version

In **2007**, after a 10-year hiatus from programming, I wrote my first Asteroids clone using basic keyboard input, fixed-shape geometry, and a rudimentary game loop:

**CLICK TO DIVE BACK IN TIME INTO 2006 YOUTUBE ▶️**
**DIVE INTO YOUTUBE LIKE IT WAS 2006▶️** 

[![My 2007 Asteroids approach](https://github.com/user-attachments/assets/f0f94f69-919c-45f4-a6d9-a6e76f3ff1d4)](https://www.youtube.com/watch?v=KdhjOEiZ8jY)


- **Fixed shapes** vs. **procedural polygons**: The original used hard-coded vertices; now shapes are generated at runtime.  
- **No particle effects** vs. **full particle system**: 2007 version had no thrust/explosion particles.  

---

## Requirements

- **C++17** (or later) compiler (GCC, Clang, MSVC)  
- [Allegro 5](https://liballeg.org/) development libraries  
- CMake 3.10+ (or a manually-written Makefile)  
- Linker flags for Allegro5 (e.g. `-lallegro -lallegro_primitives -lallegro_font -lallegro_ttf`)  

---

## Building & Installation

1. **Clone** the repository:  
   ```bash
   git clone https://github.com/ppotepa/Asteroids.git
   cd Asteroids
