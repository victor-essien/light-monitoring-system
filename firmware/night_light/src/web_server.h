#pragma once

// Register the routes ("/" and "/api") and start the HTTP server.
void webBegin();

// Handle pending HTTP requests. Call from loop().
void webHandle();
