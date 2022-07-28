# CPong

CPong is a simple and customizable pong clone written in C, which was created to mess around with basic linear algebra and CLI graphics. It is should be compatible with most unix shells, as long as *console_codes* and optionally *unicode/wchars* are supported.

## Installation

Use [GNU make](https://www.gnu.org/software/make/manual/make.html) to create executable or install into path.

```bash
sudo make install
```

append `CFLAGS='-D ASCII'` or edit makefile to use ASCII characters exclusively.

## How it works

**Horizontal Collisions:**
```C
vy *= -1; // reverse y-speed
y = -y; // @ top
y = ymax - (y - ymax); // @ bottom
```

**Paddle Collisions**

<i>(x|y) &wedgeq; ball position, (x<sub>p</sub>|y<sub>p</sub>) &wedgeq; previous ball position, (x<sub>i</sub>|y<sub>i</sub>) &wedgeq; interception</i><br>
x<sub>i</sub> = x<sub>p</sub> + &lambda;(x - x<sub>p</sub>) &rarr; &lambda; = (x<sub>i</sub> - x<sub>p</sub>) / (x - x<sub>p</sub>) into<br>
y<sub>i</sub> = y<sub>p</sub> + &lambda;(y - y<sub>p</sub>) &rarr; y<sub>i</sub> = y<sub>p</sub> + (x<sub>i</sub> - x<sub>p</sub>) / (x - x<sub>p</sub>) * (y - y<sub>p</sub>)

```C
if (iy >= p.y[0] && iy <= p.y[PADDLE_LENGTH - 1]) {
    collide...
}
```

## Contributing
Pull requests and changes are very welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[MIT](https://choosealicense.com/licenses/mit/)

