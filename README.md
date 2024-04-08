# U8g2 Graphing Library
This is a library to create line graph easier on U8g2 supported displays with Arduino.

### Features:
- Static placement
- Supporting `float` and `int` data format
- Even works on Uno (with page buffer display and int data format)
- Y axis can be set on autorange or manually defined
- Graph pointer
- Line or dotted graph style
- Non-blocking interval sampling function

When you enable X axis display, it will show the approximate delay / duration of the current graph in miliseconds (ms), the rightmost label will always be at 0 (ms) and the leftmost label will be the (approximate) delay between the last (leftmost) data and the recent (rightmost) data. It will be switched to seconds (s) instead if the data input interval is >1 second. *(**Limitation note**: this value might jumps around and won't be accurate if data interval is not fixed)*
