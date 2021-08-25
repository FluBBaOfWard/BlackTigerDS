#ifndef SOUND_HEADER
#define SOUND_HEADER

void soundInit(void);
void setMuteSoundGUI(void);
void soundMixer(int length, s16* buffer);

int ym1StatusR(void);
int ym1DataR(void);
void ym1IndexW(int index);
void ym1DataW(int data);
int ym2StatusR(void);
int ym2DataR(void);
void ym2IndexW(int index);
void ym2DataW(int data);

#endif // SOUND_HEADER
