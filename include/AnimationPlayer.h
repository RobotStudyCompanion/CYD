#ifndef ANIMATIONPLAYER_H
#define ANIMATIONPLAYER_H

#include <Arduino.h>

void switchFolder(const String& newFolder);
void loadMjpegFilesList();
void playSelectedMjpeg(int mjpegIndex);
void updateAnimationPlayer();
void animation_setup();
void requestFolderSwitch(const String& newFolder);
void setCommandPoller(void (*poller)());

#endif