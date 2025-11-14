#ifndef SOUND_H
#define SOUND_H

extern Sound paddleHitSound;
extern Sound brickHitSound;
extern Sound gameOverSound;
extern Sound restartSound;

void LoadSounds(void);

void UnloadSounds(void);

#endif // SOUND_H