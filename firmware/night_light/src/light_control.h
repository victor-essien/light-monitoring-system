#pragma once

// Set the RGB pins as outputs and start with the light off.
void lightBegin();

// Decide between day and night mode (with hysteresis).
void lightUpdateMode();

// Drive the RGB LED according to the current mode and darkness.
void lightUpdateOutput();

// Turn the LED off and reset the reported LED brightness.
void lightTurnOff();
