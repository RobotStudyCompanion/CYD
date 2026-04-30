#ifndef ANIMATIONPLAYER_H
#define ANIMATIONPLAYER_H

#include <Arduino.h>

void switchFolder(const String& newFolder);
void loadMjpegFilesList();
void playSelectedMjpeg(int mjpegIndex);
void updateAnimationPlayer();
void used_to_be_setup();

#endif