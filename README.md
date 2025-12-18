# Reaction and Memory Assessment System

## Overview 📖
This project is an embedded system designed to evaluate a user’s **reaction time** and **memory recall ability** using analog joystick input and an LCD interface. The system features two interactive test modes—Reaction Mode and Memory Mode—and provides real-time feedback and scoring. The project was developed by a team of four as the final project for an embedded systems course.

---

## System Menu 🔍
Upon startup, the system displays a menu allowing the user to select a test mode:
- **Push joystick LEFT** → Mode 1 (Reaction Mode)
- **Push joystick RIGHT** → Mode 2 (Memory Mode)

At any time during a test, the user may **push the joystick button DOWN** to exit the current mode and return to the main menu.

---

## Mode 1: Reaction Mode 🗲
In Reaction Mode, the system measures how quickly the user can respond to directional prompts.

- An **arrow** is displayed on the LCD indicating the required joystick direction.
- The user must push the joystick in the correct direction **before the timer reaches zero**.
- The **bottom-left** of the LCD displays the number of **consecutive correct inputs**.
- The **bottom-right** displays the **remaining time limit**.
- After each correct input, the allowed time decreases by **5%**, increasing difficulty.

The trial ends when:
- The timer reaches zero, or
- The joystick is pushed in the wrong direction.

At the end of the trial, the system displays the **average reaction time** across all successful inputs for **2 seconds**, then returns to the main menu.

---

## Mode 2: Memory Mode 🧠
In Memory Mode, the system evaluates short-term memory and recall.

- The LCD displays an expanding sequence of directional prompts.
- The user must repeat the full sequence using the joystick.
- The sequence length increases by one direction after each successful round.
- **No timer** is used in this mode.
- The current score (sequence length) is displayed at the bottom of the LCD.

The test ends when the user inputs an incorrect direction. The final score is displayed before returning to the main menu.

---

## Development Environment 🗔

### Software
- **KiCad** – schematic design
- **MPLAB X IDE** – embedded C development
- **AVR-GCC / AVR-LibC**
- LCD libraries from:
  - nongnu.org
  - Joerg Wunsch

---

## Hardware Setup 🖥️

### Input Power
- **Vcc = 5V**

### Components
- **ATmega328P** microcontroller
- **Analog Joystick Module**
- **GDM1602K LCD**
- **Potentiometer**
- **External Crystal Oscillator** with two load capacitors

---

## Interfaces Used 🕹️
- **ADC** – joystick position and analog input processing
- **Timers** – reaction time measurement and timing control
- **LCD Interface** – user instructions, feedback, and scores
- *(Optional internal peripherals such as interrupts were used for control flow and responsiveness)*

---

## Notes ❓
- Scores reset on power cycle.
- The system is designed for real-time interaction and accurate timing.
- The project demonstrates practical use of analog sensing, timing, and user-interface design in embedded systems.
