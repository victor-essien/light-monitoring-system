#pragma once

// Configure the analog inputs. Call once from setup().
void sensorsBegin();

// Read the light sensor and potentiometer and update the shared state
// (rawLight, rawPot, brightness, darkness, threshold).
void sensorsRead();
