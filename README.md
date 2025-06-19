# reimagine-win32-ui

A minimal frameless Windows UI with custom drawn buttons using GDI+.

I’m currently learning how to create something clean and customizable from scratch.  
This started with a simple idea and turned into hours of tweaking things, debugging layout issues, and messing around with button logic.  
This version uses a custom TDM-GCC setup and draws everything manually—no Windows title bar, no default controls.

Let’s see what I can eventually build from this.

### Compile

```bash
g++ main_ui_test.cpp -o windown_test.exe -lgdiplus -lgdi32 -mwindows
```

> Tested on: Windows 11 + TDM-GCC + GDI+
![image](https://github.com/user-attachments/assets/ab7f7b6e-bee2-46b6-a213-65016d3bf1b6)
