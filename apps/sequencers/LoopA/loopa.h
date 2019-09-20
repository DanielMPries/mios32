// LoopA Core Logic
// (c) Hawkeye 2015-2019

#define TRACKS 6
#define SCENES 6

// Current "menu" position in app

// --- Consts ---
#define METRONOME_PSEUDO_PORT 111
#define MAXNOTES 256 // per clip, * TRACKS * SCENES for total note storage


// --- Data structures ---

enum LiveMode
{
   LIVEMODE_TRANSPOSE,
   LIVEMODE_BEATLOOP
};

typedef struct
{
   u16 tick;
   u16 length;
   u8 note;
   u8 velocity;
} NoteData;

// --- Export global tables ---
extern s8 liveTransposeSemitones_[15]; // Live transpose table

// --- Export global variables ---

extern u32 millisecondsSinceStartup_;        // global "uptime" timer
extern u16 inactivitySeconds_;               // screensaver timer
extern u32 tick_;                            // global seq tick
extern u16 sessionNumber_;                   // currently active session number (directories will be auto-created)
extern u8 sessionExistsOnDisk_;              // is the currently selected session number already saved on disk/sd card
extern enum LoopAPage page_;                 // currently active page/view
extern enum Command command_;                // currently active command
extern s8 tempoFade_;                        // 0: no tempo change ongoing | +1: increase tempo button pressed | -1: decrease tempo button pressed (ongoing event)

extern u8 activeTrack_;                      // currently active track number (0-5)
extern u8 activeScene_;                      // currently active scene number (0-15)
extern u16 activeNote_;                      // currently active edited note number, when in noteroll editor
extern u8 isRecording_;                      // set, if currently recording to the selected clip
extern u8 oledBeatFlashState_;               // 0: don't flash, 1: flash slightly (normal 1/4th note), 2: flash intensively (after four 1/4th notes or 16 steps)
extern s16 notePtrsOn_[128];                 // during recording - pointers to notes that are currently "on" (and waiting for an "off", therefore length yet undetermined) (-1: note not depressed)

extern char filename_[20];                   // global, for filename operations

extern u8 trackMute_[TRACKS];                // mute state of each track
extern s8 trackMidiOutPort_[TRACKS];         // if negative: map to user defined instrument, if positive: standard mios port number
extern u8 trackMidiOutChannel_[TRACKS];

extern u16 clipSteps_[TRACKS][SCENES];       // number of steps for each clip
extern u32 clipQuantize_[TRACKS][SCENES];    // brings all clip notes close to the specified timing, e.g. qunatize = 4 - close to 4th notes, 16th = quantize to step, ...
extern s8 clipTranspose_[TRACKS][SCENES];
extern s16 clipScroll_[TRACKS][SCENES];
extern u8 clipStretch_[TRACKS][SCENES];      // 1: compress to 1/16th, 2: compress to 1/8th ... 16: no stretch, 32: expand 2x, 64: expand 4x, 128: expand 8x

extern NoteData clipNotes_[TRACKS][SCENES][MAXNOTES]; // clip note data storage of active scene (chained list, note start time and length/velocity)
extern u16 clipNotesSize_[TRACKS][SCENES];   // Active number of notes in use for that clip

// TODO: Save to disk
extern float bpm_;                           // bpm
extern u8 liveMode_;                         // currently active upper-right encoder live mode
extern s8 liveTranspose_;                    // Live transpose value (+/- 7)
extern s8 liveBeatLoop_;                     // Live beatloop value (+/- 7)
extern u16 stepsPerMeasure_;                 // number of steps for one measure (adjustable) - 16 steps default for a 4/4 beat
extern u8 stepsPerBeat_;                     // number of steps for one beat (adjustable) - 4 steps default for a 4/4 beat
extern u8 metronomeEnabled_;                 // Set to 1, if metronome is turned on in bpm screen
extern s8 trackMidiInPort_[TRACKS];          // If set to 0: enable recording from all midi ports (default)
extern u8 trackMidiInChannel_[TRACKS];       // If set to 16: enable recording from all midi channels (default)
extern u8 trackMidiForward_[TRACKS];         // If set to 1: forward midi notes to out port/channel (live play)
extern u8 trackLiveTranspose_[TRACKS];       // If set to >0: selection of live transposer table (live transposing enabled for this track)
extern s8 clipSwing_[TRACKS][SCENES];        // If set to >0: clip swing enabled (affects only notes on quantized steps)
extern s8 clipProbability_[TRACKS][SCENES];  // If set to >0: percentage of note drops occuring
extern u8 clipFTSMode_[TRACKS][SCENES];      // If set to >0: FTS enabled, contains FTS scale (major, minor ...)
extern u8 clipFTSNote_[TRACKS][SCENES];      // FTS base note (C, C#, ...)


// --- Secondary data (not on disk) ---
extern u8 trackMuteToggleRequested_[TRACKS]; // 1: perform a mute/unmute toggle of the clip at the next measure (synced mute/unmute)
extern u8 sceneChangeRequested_;             // If != activeScene_, this will be the scene we are changing to at the next measure
extern s8 liveTransposeRequested_;           // 1: perform a live transpose change at the next measure
extern u16 clipActiveNote_[TRACKS][SCENES];  // currently active edited note number, when in noteroll editor


// Help function: convert tick number to step number
u16 tickToStep(u32 tick);

// Help function: convert step number to tick number
u32 stepToTick(u16 step);

// Bound tick to clip steps
u32 boundTickToClipSteps(u32 tick, u8 clip);

// Quantize a tick time event
u32 quantize(u32 tick, u32 quantizeMeasure, u32 clipLengthInTicks);

// Transform (stretch and scroll) and then quantize a note in a clip
s32 quantizeTransform(u8 clip, u16 noteNumber);

// Get the clip length in ticks
u32 getClipLengthInTicks(u8 clip);

// Request (or cancel) a synced mute/unmute toggle
void toggleMute(u8 clipNumber);

// Perform live LED updates (upper right encoder section)
void updateLiveLEDs();

// convert sessionNumber to global filename_
void sessionNumberToFilename(u16 sessionNumber);

// Save session to defined session number file
void saveSession(u16 sessionNumber);

// Load session from defined session number file
void loadSession(u16 sessionNumber);

// First callback from app - render Loopa Startup logo on screen
void loopaStartup();

// Called periodically (1 mS) to check for timestamped MIDI events which have to be sent.
s32 LoopA_MIDI_OUT_Handler(void);

// --- LoopA Sequencer Section ---

s32  seqPlayOffEvents(void);
s32  seqReset(u8 play_off_events);
s32  seqSongPos(u16 new_song_pos);
void updateBeatLEDsAndClipPositions(u32 bpm_tick);
s32  loopaSeqTick(u32 bpmTick);
s32  hookMIDISendPackage(s8 loopaTrack, mios32_midi_package_t package);
s32  seqPlayEvent(s8 loopaTrack, mios32_midi_package_t midi_package, u32 tick);
s32  seqIgnoreMetaEvent(s8 loopaTrack, u8 meta, u32 len, u8 *buffer, u32 tick);

// Handle a stop request
void handleStop();

// Initialize LoopA SEQ
s32 seqInit();

// This main sequencer handler is called periodically to poll the clock/current tick from BPM generator
s32 loopaSeqHandler(void);

// SD Card Available, initialize
void loopaSDCardAvailable();

// Record midi event
void loopaRecord(mios32_midi_port_t port, mios32_midi_package_t midi_package);

